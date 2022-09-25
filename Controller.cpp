#include "Controller.h"
#include "Constants.h"
#include "Helper.h"
#include "Window.h"

Controller::Controller(QObject *parent)
    : QObject(parent)
    , mIfps(0.0f)
    , mImGuiWantsMouseCapture(false)
    , mImGuiWantsKeyboardCapture(false)
{}

Controller::~Controller() {}

void Controller::init()
{
    initializeOpenGLFunctions();
    glEnable(GL_DEPTH_TEST);

    mQuad = new Quad;

    // Textures
    mTextures.insert(TextureName::All, new Texture(":/Resources/Textures/all.png"));

    mScreenShader = new Shader("Screen Shader");
    mScreenShader->addPath(QOpenGLShader::Vertex, ":/Resources/Shaders/Screen.vert");
    mScreenShader->addPath(QOpenGLShader::Fragment, ":/Resources/Shaders/Screen.frag");
    mScreenShader->addUniform("screenTexture");
    mScreenShader->addAttribute("position");
    mScreenShader->addAttribute("textureCoords");
    mScreenShader->init();

    mRaycasterFlatShader = new Shader("Raycaster Flat Compute Shader");
    mRaycasterFlatShader->addPath(QOpenGLShader::Compute, ":/Resources/Shaders/RaycasterFlat.glsl");
    mRaycasterFlatShader->addUniform("player.position");
    mRaycasterFlatShader->addUniform("player.direction");
    mRaycasterFlatShader->addUniform("camera.plane");
    mRaycasterFlatShader->addUniform("screen.width");
    mRaycasterFlatShader->addUniform("screen.height");
    mRaycasterFlatShader->init();

    mRaycasterTexturedShader = new Shader("Raycaster Textured Compute Shader");
    mRaycasterTexturedShader->addPath(QOpenGLShader::Compute, ":/Resources/Shaders/RaycasterTextured.glsl");
    mRaycasterTexturedShader->addUniform("player.position");
    mRaycasterTexturedShader->addUniform("player.direction");
    mRaycasterTexturedShader->addUniform("camera.plane");
    mRaycasterTexturedShader->addUniform("screen.width");
    mRaycasterTexturedShader->addUniform("screen.height");
    mRaycasterTexturedShader->init();

    mRaycasterFloorShader = new Shader("Raycaster Floor Compute Shader");
    mRaycasterFloorShader->addPath(QOpenGLShader::Compute, ":/Resources/Shaders/RaycasterFloor.glsl");
    mRaycasterFloorShader->addUniform("player.position");
    mRaycasterFloorShader->addUniform("player.direction");
    mRaycasterFloorShader->addUniform("camera.plane");
    mRaycasterFloorShader->addUniform("screen.width");
    mRaycasterFloorShader->addUniform("screen.height");
    mRaycasterFloorShader->init();

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
    glTexImage2D(GL_TEXTURE_2D, 0, GL_R8I, 24, 24, 0, GL_RED_INTEGER, GL_INT, WORLD_MAP);
    glGenerateMipmap(GL_TEXTURE_2D);

    glGenFramebuffers(1, &mCleanerFramebuffer);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, mCleanerFramebuffer);
    glFramebufferTexture(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, mRaycasterOutputImage, 0);

    mPlayer.position = QVector2D(7, 4);
    mPlayer.direction = QVector2D(-1, 0);
    mPlayer.movementSpeed = 5;
    mPlayer.angularSpeed = 120; // Degree per second

    mCamera.plane = QVector2D(0, float(SCREEN_WIDTH) / SCREEN_HEIGHT);
}

void Controller::render(float ifps)
{
    mIfps = ifps;

    update();

    // Raycaster stuff

    // Clean
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, mCleanerFramebuffer);
    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    // Floor and Ceiling
    mRaycasterFloorShader->bind();
    mRaycasterFloorShader->setUniformValue("player.position", mPlayer.position);
    mRaycasterFloorShader->setUniformValue("player.direction", mPlayer.direction);
    mRaycasterFloorShader->setUniformValue("camera.plane", mCamera.plane);
    mRaycasterFloorShader->setUniformValue("screen.width", SCREEN_WIDTH);
    mRaycasterFloorShader->setUniformValue("screen.height", SCREEN_HEIGHT);
    glBindImageTexture(0, mRaycasterOutputImage, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);
    glBindImageTexture(1, mTextures.value(TextureName::All)->id(), 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA8);
    glDispatchCompute(1, SCREEN_HEIGHT / 2 - 1, 1);
    glMemoryBarrier(GL_ALL_BARRIER_BITS);
    mRaycasterFloorShader->release();

    // Textured Raycaster
    mRaycasterTexturedShader->bind();
    mRaycasterTexturedShader->setUniformValue("player.position", mPlayer.position);
    mRaycasterTexturedShader->setUniformValue("player.direction", mPlayer.direction);
    mRaycasterTexturedShader->setUniformValue("camera.plane", mCamera.plane);
    mRaycasterTexturedShader->setUniformValue("screen.width", SCREEN_WIDTH);
    mRaycasterTexturedShader->setUniformValue("screen.height", SCREEN_HEIGHT);
    glBindImageTexture(0, mRaycasterOutputImage, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);
    glBindImageTexture(1, mRaycasterWorldMap, 0, GL_FALSE, 0, GL_READ_ONLY, GL_R8I);
    glBindImageTexture(2, mTextures.value(TextureName::All)->id(), 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA8);
    glDispatchCompute(SCREEN_WIDTH, 1, 1);
    glMemoryBarrier(GL_ALL_BARRIER_BITS);
    mRaycasterTexturedShader->release();

    // Render to screen
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClearColor(0, 0, 0, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    mScreenShader->bind();
    mScreenShader->setSampler("screenTexture", 0, mRaycasterOutputImage);
    mQuad->render();
    mScreenShader->release();

    QtImGui::newFrame();
    //ImGui::ShowDemoWindow();

    ImGui::SetNextWindowSize(ImVec2(420, 820), ImGuiCond_FirstUseEver);
    ImGui::Begin("Debug");
    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

    glViewport(0, 0, mWindow->width(), mWindow->height());
    ImGui::Render();
    QtImGui::render();
}

void Controller::setWindow(Window *newWindow)
{
    mWindow = newWindow;
}

void Controller::update()
{
    auto pressedKeys = mPressedKeys.keys();
    for (const auto key : pressedKeys)
    {
        if (!mPressedKeys.value(key))
            continue;

        if (key == Qt::Key_W)
        {
            mPlayer.position += mPlayer.direction * mIfps * mPlayer.movementSpeed;
        }

        if (key == Qt::Key_S)
        {
            mPlayer.position -= mPlayer.direction * mIfps * mPlayer.movementSpeed;
        }

        if (key == Qt::Key_A)
        {
            mPlayer.direction = Helper::rotate(mPlayer.direction, mIfps * mPlayer.angularSpeed);
            mCamera.plane = Helper::rotate(mCamera.plane, mIfps * mPlayer.angularSpeed);
        }

        if (key == Qt::Key_D)
        {
            mPlayer.direction = Helper::rotate(mPlayer.direction, -mIfps * mPlayer.angularSpeed);
            mCamera.plane = Helper::rotate(mCamera.plane, -mIfps * mPlayer.angularSpeed);
        }
    }

    mPlayer.position[0] = qMax(1.0f, qMin(mPlayer.position[0], 22.0f));
    mPlayer.position[1] = qMax(1.0f, qMin(mPlayer.position[1], 22.0f));
}

void Controller::wheelMoved(QWheelEvent *event)
{
    if (mImGuiWantsMouseCapture)
        return;
}

void Controller::mousePressed(QMouseEvent *event)
{
    if (mImGuiWantsMouseCapture)
        return;

    mPressedButton = event->button();
}

void Controller::mouseReleased(QMouseEvent *event)
{
    if (mImGuiWantsMouseCapture)
        return;

    mPressedButton = Qt::NoButton;
}

void Controller::mouseMoved(QMouseEvent *event)
{
    if (mImGuiWantsMouseCapture)
        return;
}

void Controller::keyPressed(QKeyEvent *event)
{
    if (mImGuiWantsKeyboardCapture)
        return;

    mPressedKeys.insert((Qt::Key) event->key(), true);
}

void Controller::keyReleased(QKeyEvent *event)
{
    if (mImGuiWantsKeyboardCapture)
        return;

    mPressedKeys.insert((Qt::Key) event->key(), false);
}

void Controller::resize(int w, int h)
{
    mWindow->makeCurrent();

    mWindow->doneCurrent();
}

int const Controller::WORLD_MAP[24][24] = {
    {2, 2, 2, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 2, 2, 2, 2, 2, 2, 1, 2, 2}, //
    {2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2}, //
    {2, 0, 2, 2, 0, 0, 0, 0, 0, 2, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2}, //
    {1, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2}, //
    {2, 0, 2, 2, 0, 0, 0, 0, 0, 2, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2}, //
    {2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 2, 0, 0, 0, 0, 0, 2, 2, 2, 0, 2, 2, 2}, //
    {2, 2, 2, 2, 0, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 2, 2, 2, 0, 0, 0, 0, 0, 2}, //
    {2, 2, 2, 2, 0, 2, 2, 2, 2, 0, 2, 0, 2, 0, 2, 0, 2, 2, 0, 8, 0, 8, 0, 2}, //
    {2, 2, 0, 0, 0, 0, 0, 0, 2, 2, 0, 2, 0, 2, 0, 2, 2, 2, 0, 0, 0, 0, 0, 2}, //
    {2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 2, 0, 0, 0, 0, 0, 2}, //
    {2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 2, 0, 2, 0, 2, 0, 2}, //
    {2, 2, 0, 0, 0, 0, 0, 0, 2, 2, 0, 2, 0, 2, 0, 2, 2, 2, 2, 2, 0, 2, 2, 2}, //
    {2, 2, 2, 2, 0, 2, 2, 2, 2, 2, 2, 2, 0, 2, 2, 2, 2, 2, 2, 2, 0, 2, 2, 2}, //
    {2, 2, 2, 2, 0, 2, 2, 2, 2, 2, 2, 2, 0, 0, 2, 0, 2, 2, 0, 0, 0, 0, 0, 2}, //
    {2, 2, 0, 0, 0, 0, 0, 2, 2, 2, 0, 0, 0, 0, 0, 0, 2, 2, 0, 0, 0, 0, 0, 2}, //
    {2, 0, 0, 0, 0, 0, 0, 0, 2, 2, 0, 0, 0, 0, 0, 0, 2, 2, 0, 0, 0, 0, 0, 2}, //
    {1, 0, 0, 0, 0, 0, 0, 0, 1, 2, 2, 2, 2, 2, 2, 0, 2, 2, 2, 0, 0, 0, 2, 2}, //
    {2, 0, 0, 0, 0, 0, 0, 0, 2, 2, 2, 1, 2, 2, 2, 2, 2, 0, 0, 2, 0, 8, 0, 2}, //
    {2, 2, 0, 0, 0, 0, 0, 2, 2, 2, 0, 0, 0, 2, 2, 0, 2, 0, 2, 0, 0, 0, 2, 2}, //
    {2, 0, 0, 0, 0, 0, 0, 0, 8, 0, 0, 0, 0, 0, 2, 2, 0, 8, 0, 8, 0, 8, 0, 2}, //
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2}, //
    {2, 0, 0, 0, 0, 0, 0, 0, 8, 0, 0, 0, 0, 0, 2, 2, 0, 8, 0, 8, 0, 8, 0, 2}, //
    {2, 2, 0, 0, 0, 0, 0, 2, 2, 2, 0, 0, 0, 2, 2, 0, 2, 0, 2, 0, 0, 0, 2, 2}, //
    {2, 2, 2, 2, 1, 2, 2, 2, 2, 2, 2, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2}  //
};
