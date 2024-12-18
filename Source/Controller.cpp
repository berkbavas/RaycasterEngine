#include "Controller.h"

#include "Constants.h"
#include "Helper.h"
#include "Window.h"

RaycasterEngine::Controller::Controller()
{
    mWindow = new Window;

    connect(mWindow, &RaycasterEngine::Window::Initialize, this, &RaycasterEngine::Controller::Initialize);
    connect(mWindow, &RaycasterEngine::Window::Render, this, &RaycasterEngine::Controller::Render);
    connect(mWindow, &RaycasterEngine::Window::MousePressed, this, &RaycasterEngine::Controller::MousePressed);
    connect(mWindow, &RaycasterEngine::Window::MouseReleased, this, &RaycasterEngine::Controller::MouseReleased);
    connect(mWindow, &RaycasterEngine::Window::MouseMoved, this, &RaycasterEngine::Controller::MouseMoved);
    connect(mWindow, &RaycasterEngine::Window::KeyPressed, this, &RaycasterEngine::Controller::KeyPressed);
    connect(mWindow, &RaycasterEngine::Window::KeyReleased, this, &RaycasterEngine::Controller::KeyReleased);
}

void RaycasterEngine::Controller::Initialize()
{
    initializeOpenGLFunctions();
    glEnable(GL_DEPTH_TEST);

    mQuad = new Quad;

    // Textures
    mTextures.insert(TextureName::All, new Texture(":/Resources/Textures/all.png", "all", 0));

    mScreenShader = new Shader("Screen Shader");
    mScreenShader->AddPath(QOpenGLShader::Vertex, ":/Resources/Shaders/Screen.vert");
    mScreenShader->AddPath(QOpenGLShader::Fragment, ":/Resources/Shaders/Screen.frag");
    mScreenShader->Initialize();

    mRaycasterFlatShader = new Shader("Raycaster Flat Compute Shader");
    mRaycasterFlatShader->AddPath(QOpenGLShader::Compute, ":/Resources/Shaders/RaycasterFlat.glsl");
    mRaycasterFlatShader->Initialize();

    mRaycasterTexturedShader = new Shader("Raycaster Textured Compute Shader");
    mRaycasterTexturedShader->AddPath(QOpenGLShader::Compute, ":/Resources/Shaders/RaycasterTextured.glsl");
    mRaycasterTexturedShader->Initialize();

    mRaycasterFloorShader = new Shader("Raycaster Floor Compute Shader");
    mRaycasterFloorShader->AddPath(QOpenGLShader::Compute, ":/Resources/Shaders/RaycasterFloor.glsl");
    mRaycasterFloorShader->Initialize();

    mRaycasterSpritesShader = new Shader("Raycaster Sprites Compute Shader");
    mRaycasterSpritesShader->AddPath(QOpenGLShader::Compute, ":/Resources/Shaders/RaycasterSprites.glsl");
    mRaycasterSpritesShader->Initialize();

    glGenTextures(1, &mRaycasterOutputImage);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, mRaycasterOutputImage);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, SCREEN_WIDTH, SCREEN_HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
    glGenerateMipmap(GL_TEXTURE_2D);

    glGenTextures(1, &mRaycasterWorldMap);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, mRaycasterWorldMap);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_R8I, 24, 24, 0, GL_RED_INTEGER, GL_INT, mWorldMap);
    glGenerateMipmap(GL_TEXTURE_2D);

    glGenTextures(1, &mRaycasterDepthBuffer);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_1D, mRaycasterDepthBuffer);
    glTexImage1D(GL_TEXTURE_1D, 0, GL_RGBA32F, SCREEN_WIDTH, 0, GL_RGBA, GL_FLOAT, NULL);
    glGenerateMipmap(GL_TEXTURE_1D);

    glGenFramebuffers(1, &mCleanerFramebuffer);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, mCleanerFramebuffer);
    glFramebufferTexture(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, mRaycasterOutputImage, 0);

    mPlayer.position = QVector2D(7, 4);
    mPlayer.direction = QVector2D(-1, 0);
    mPlayer.movementSpeed = 5;
    mPlayer.angularSpeed = 120; // Degree per second

    mCamera.plane = QVector2D(0, float(SCREEN_WIDTH) / SCREEN_HEIGHT);

    mSpriteIndices.resize(mSprites.size());

    QtImGui::initialize(mWindow);

    LOG_INFO("Controller::Initialize: The application has been initialized.");
}

void RaycasterEngine::Controller::Run()
{
    mWindow->resize(SCREEN_WIDTH, SCREEN_HEIGHT);
    mWindow->show();
}

void RaycasterEngine::Controller::Render(float ifps)
{
    Update(ifps);

    // Clean
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, mCleanerFramebuffer);
    glFramebufferTexture(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, mRaycasterOutputImage, 0);
    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT);

    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, mCleanerFramebuffer);
    glFramebufferTexture(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, mRaycasterDepthBuffer, 0);
    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT);

    RenderFloor();
    RenderWalls();
    RenderSprites();

    // Render to screen
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    mScreenShader->Bind();
    mScreenShader->SetUniformValue("screenTexture", 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, mRaycasterOutputImage);
    mQuad->Render();
    mScreenShader->Release();

    QtImGui::newFrame();
    ImGui::SetNextWindowSize(ImVec2(420, 820), ImGuiCond_FirstUseEver);

    ImGui::Begin("Debug");
    ImGui::Text("Player Position: (%.2f, %.2f)", mPlayer.position.x(), mPlayer.position.y());
    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

    ImGui::End();
    glViewport(0, 0, mWindow->width(), mWindow->height());
    ImGui::Render();
    QtImGui::render();
}

void RaycasterEngine::Controller::RenderFloor()
{
    // Floor and Ceiling
    mRaycasterFloorShader->Bind();
    mRaycasterFloorShader->SetUniformValue("player.position", mPlayer.position);
    mRaycasterFloorShader->SetUniformValue("player.direction", mPlayer.direction);
    mRaycasterFloorShader->SetUniformValue("camera.plane", mCamera.plane);
    mRaycasterFloorShader->SetUniformValue("screen.width", SCREEN_WIDTH);
    mRaycasterFloorShader->SetUniformValue("screen.height", SCREEN_HEIGHT);
    glBindImageTexture(0, mRaycasterOutputImage, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);
    glBindImageTexture(1, mTextures.value(TextureName::All)->GetId(), 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA8);
    glDispatchCompute(1, SCREEN_HEIGHT / 2 - 1, 1);
    glMemoryBarrier(GL_ALL_BARRIER_BITS);
    mRaycasterFloorShader->Release();
}

void RaycasterEngine::Controller::RenderWalls()
{
    // Textured Raycaster Walls
    mRaycasterTexturedShader->Bind();
    mRaycasterTexturedShader->SetUniformValue("player.position", mPlayer.position);
    mRaycasterTexturedShader->SetUniformValue("player.direction", mPlayer.direction);
    mRaycasterTexturedShader->SetUniformValue("camera.plane", mCamera.plane);
    mRaycasterTexturedShader->SetUniformValue("screen.width", SCREEN_WIDTH);
    mRaycasterTexturedShader->SetUniformValue("screen.height", SCREEN_HEIGHT);
    glBindImageTexture(0, mRaycasterOutputImage, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);
    glBindImageTexture(1, mRaycasterWorldMap, 0, GL_FALSE, 0, GL_READ_ONLY, GL_R8I);
    glBindImageTexture(2, mTextures.value(TextureName::All)->GetId(), 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA8);
    glBindImageTexture(3, mRaycasterDepthBuffer, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
    glDispatchCompute(SCREEN_WIDTH, 1, 1);
    glMemoryBarrier(GL_ALL_BARRIER_BITS);
    mRaycasterTexturedShader->Release();
}

void RaycasterEngine::Controller::RenderSprites()
{
    // Sprites
    for (int i = 0; i < mSprites.size(); i++)
    {
        mSprites[i].distance = pow(mPlayer.position.x() - mSprites[i].x, 2) + pow(mPlayer.position.y() - mSprites[i].y, 2);
        mSpriteIndices[i] = i;
    }

    // Sort sprites
    std::vector<std::pair<double, int>> sortedSprites(mSprites.size());
    for (int i = 0; i < sortedSprites.size(); i++)
    {
        sortedSprites[i].first = mSprites[i].distance;
        sortedSprites[i].second = mSpriteIndices[i];
    }

    std::sort(sortedSprites.begin(), sortedSprites.end());

    // Restore in reverse order to go from farthest to nearest
    for (int i = 0; i < sortedSprites.size(); i++)
    {
        mSpriteIndices[i] = sortedSprites[sortedSprites.size() - i - 1].second;
    }

    for (int i = 0; i < mSprites.size(); i++)
    {
        int j = mSpriteIndices[i];
        // Translate sprite position to relative to camera
        double spriteX = mSprites[j].x - mPlayer.position.x();
        double spriteY = mSprites[j].y - mPlayer.position.y();

        QVector2D spritePos(mSprites[j].x, mSprites[j].y);
        QVector2D spritePosRelative = spritePos - mPlayer.position;
        QVector2D spriteDirRelative = spritePosRelative.normalized();
        float dot = QVector2D::dotProduct(spriteDirRelative, mPlayer.direction);

        // Frustum culling
        if (dot < 0) // Sprite is behind us, don't draw
        {
            continue;
        }

        // Transform sprite with the inverse camera matrix
        // [ planeX   dirX ] -1                                       [ dirY      -dirX ]
        // [               ]       =  1/(planeX*dirY-dirX*planeY) *   [                 ]
        // [ planeY   dirY ]                                          [ -planeY  planeX ]

        double invDet = 1.0 / (mCamera.plane.x() * mPlayer.direction.y() - mPlayer.direction.x() * mCamera.plane.y());
        float transformX = invDet * (mPlayer.direction.y() * spriteX - mPlayer.direction.x() * spriteY);
        float transformY = invDet * (-mCamera.plane.y() * spriteX + mCamera.plane.x() * spriteY); // this is actually the depth inside the screen, that what Z is in 3D
        int spriteScreenX = int((SCREEN_WIDTH / 2.0f) * (1 + transformX / transformY));

        // Calculate height of the sprite on screen
        int spriteHeight = abs(int(SCREEN_HEIGHT / (transformY))); // Using 'transformY' instead of the real distance prevents fisheye

        // Calculate lowest and highest pixel to fill in current stripe
        int drawStartY = -spriteHeight / 2 + SCREEN_HEIGHT / 2.0;

        if (drawStartY < 0)
        {
            drawStartY = 0;
        }

        int drawEndY = spriteHeight / 2 + SCREEN_HEIGHT / 2.0;

        if (drawEndY >= SCREEN_HEIGHT)
        {
            drawEndY = SCREEN_HEIGHT - 1;
        }

        // calculate width of the sprite
        int spriteWidth = abs(int(SCREEN_HEIGHT / transformY));
        int drawStartX = -spriteWidth / 2 + spriteScreenX;

        if (drawStartX < 0)
        {
            drawStartX = 0;
        }

        int drawEndX = spriteWidth / 2 + spriteScreenX;

        if (drawEndX >= SCREEN_WIDTH)
        {
            drawEndX = SCREEN_WIDTH - 1;
        }

        mRaycasterSpritesShader->Bind();
        mRaycasterSpritesShader->SetUniformValue("screen.height", SCREEN_HEIGHT);
        mRaycasterSpritesShader->SetUniformValue("sprite.width", spriteWidth);
        mRaycasterSpritesShader->SetUniformValue("sprite.height", spriteHeight);
        mRaycasterSpritesShader->SetUniformValue("sprite.screenX", spriteScreenX);
        mRaycasterSpritesShader->SetUniformValue("sprite.textureIndex", mSprites[j].texture);
        mRaycasterSpritesShader->SetUniformValue("draw.startX", drawStartX);
        mRaycasterSpritesShader->SetUniformValue("draw.startY", drawStartY);
        mRaycasterSpritesShader->SetUniformValue("draw.endY", drawEndY);
        mRaycasterSpritesShader->SetUniformValue("draw.transformY", transformY);
        glBindImageTexture(0, mRaycasterOutputImage, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);
        glBindImageTexture(1, mTextures.value(TextureName::All)->GetId(), 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA8);
        glBindImageTexture(2, mRaycasterDepthBuffer, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
        glDispatchCompute(drawEndX - drawStartX, 1, 1);
        glMemoryBarrier(GL_ALL_BARRIER_BITS);
        mRaycasterSpritesShader->Release();
    }
}

void RaycasterEngine::Controller::Update(float ifps)
{
    const auto pressedKeys = mPressedKeys.keys();

    for (const auto key : pressedKeys)
    {
        if (!mPressedKeys.value(key))
        {
            continue;
        }

        if (key == Qt::Key_W)
        {
            mPlayer.position += mPlayer.direction * ifps * mPlayer.movementSpeed;
        }

        if (key == Qt::Key_S)
        {
            mPlayer.position -= mPlayer.direction * ifps * mPlayer.movementSpeed;
        }

        if (key == Qt::Key_A)
        {
            mPlayer.direction = Helper::Rotate(mPlayer.direction, ifps * mPlayer.angularSpeed);
            mCamera.plane = Helper::Rotate(mCamera.plane, ifps * mPlayer.angularSpeed);
        }

        if (key == Qt::Key_D)
        {
            mPlayer.direction = Helper::Rotate(mPlayer.direction, -ifps * mPlayer.angularSpeed);
            mCamera.plane = Helper::Rotate(mCamera.plane, -ifps * mPlayer.angularSpeed);
        }
    }

    mPlayer.position[0] = qMax(1.0f, qMin(mPlayer.position[0], 22.0f));
    mPlayer.position[1] = qMax(1.0f, qMin(mPlayer.position[1], 22.0f));
}

void RaycasterEngine::Controller::MousePressed(QMouseEvent* event)
{
    if (mImGuiWantsMouseCapture)
    {
        return;
    }

    mPressedButton = event->button();
}

void RaycasterEngine::Controller::MouseReleased(QMouseEvent* event)
{
    if (mImGuiWantsMouseCapture)
    {
        return;
    }

    mPressedButton = Qt::NoButton;
}

void RaycasterEngine::Controller::MouseMoved(QMouseEvent* event)
{
    if (mImGuiWantsMouseCapture)
    {
        return;
    }
}

void RaycasterEngine::Controller::KeyPressed(QKeyEvent* event)
{
    if (mImGuiWantsKeyboardCapture)
    {
        return;
    }

    mPressedKeys.insert((Qt::Key) event->key(), true);
}

void RaycasterEngine::Controller::KeyReleased(QKeyEvent* event)
{
    if (mImGuiWantsKeyboardCapture)
    {
        return;
    }

    mPressedKeys.insert((Qt::Key) event->key(), false);
}