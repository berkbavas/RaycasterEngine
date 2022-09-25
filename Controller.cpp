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

    mScreenShader = new Shader("Screen Shader");
    mScreenShader->addPath(QOpenGLShader::Vertex, ":/Resources/Shaders/Screen.vert");
    mScreenShader->addPath(QOpenGLShader::Fragment, ":/Resources/Shaders/Screen.frag");
    mScreenShader->addUniform("screenTexture");
    mScreenShader->addAttribute("position");
    mScreenShader->addAttribute("textureCoords");
    mScreenShader->init();

    mFlatRaycasterShader = new Shader("Flat Raycaster Compute Shader");
    mFlatRaycasterShader->addPath(QOpenGLShader::Compute, ":/Resources/Shaders/RaycasterFlat.glsl");
    mFlatRaycasterShader->addUniform("player.position");
    mFlatRaycasterShader->addUniform("player.direction");
    mFlatRaycasterShader->addUniform("camera.plane");
    mFlatRaycasterShader->addUniform("screen.width");
    mFlatRaycasterShader->addUniform("screen.height");
    mFlatRaycasterShader->init();

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

    mPlayer.position = QVector2D(20, 12);
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
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, mCleanerFramebuffer);
    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    mFlatRaycasterShader->bind();
    mFlatRaycasterShader->setUniformValue("player.position", mPlayer.position);
    mFlatRaycasterShader->setUniformValue("player.direction", mPlayer.direction);
    mFlatRaycasterShader->setUniformValue("camera.plane", mCamera.plane);
    mFlatRaycasterShader->setUniformValue("screen.width", (float) SCREEN_WIDTH);
    mFlatRaycasterShader->setUniformValue("screen.height", (float) SCREEN_HEIGHT);

    glBindImageTexture(0, mRaycasterOutputImage, 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA32F);
    glBindImageTexture(1, mRaycasterWorldMap, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_R8I);
    glDispatchCompute(SCREEN_WIDTH, 1, 1);
    glMemoryBarrier(GL_ALL_BARRIER_BITS);
    mFlatRaycasterShader->release();

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
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, //
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1}, //
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1}, //
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1}, //
    {1, 0, 0, 0, 0, 0, 2, 2, 2, 2, 2, 0, 0, 0, 0, 3, 0, 3, 0, 3, 0, 0, 0, 1}, //
    {1, 0, 0, 0, 0, 0, 2, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1}, //
    {1, 0, 0, 0, 0, 0, 2, 0, 0, 0, 2, 0, 0, 0, 0, 3, 0, 0, 0, 3, 0, 0, 0, 1}, //
    {1, 0, 0, 0, 0, 0, 2, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1}, //
    {1, 0, 0, 0, 0, 0, 2, 2, 0, 2, 2, 0, 0, 0, 0, 3, 0, 3, 0, 3, 0, 0, 0, 1}, //
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1}, //
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1}, //
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1}, //
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1}, //
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1}, //
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1}, //
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1}, //
    {1, 4, 4, 4, 4, 4, 4, 4, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1}, //
    {1, 4, 0, 4, 0, 0, 0, 0, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1}, //
    {1, 4, 0, 0, 0, 0, 5, 0, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1}, //
    {1, 4, 0, 4, 0, 0, 0, 0, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1}, //
    {1, 4, 0, 4, 4, 4, 4, 4, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1}, //
    {1, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1}, //
    {1, 4, 4, 4, 4, 4, 4, 4, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1}, //
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}  //
};
