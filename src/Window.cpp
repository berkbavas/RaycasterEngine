#include "Window.h"
#include "Controller.h"

#include <QDateTime>
#include <QKeyEvent>

#include <QDebug>

RaycasterEngine::Window::Window(QWindow *parent)
    : QOpenGLWindow(QOpenGLWindow::UpdateBehavior::NoPartialUpdate, parent)

{
    QSurfaceFormat format = QSurfaceFormat::defaultFormat();
    format.setMajorVersion(4);
    format.setMinorVersion(3);
    format.setProfile(QSurfaceFormat::CoreProfile);
    format.setSamples(4);
    format.setSwapInterval(1);
    setFormat(format);

    connect(this, &QOpenGLWindow::frameSwapped, this, [=]() { update(); });
}

void RaycasterEngine::Window::initializeGL()
{
    initializeOpenGLFunctions();

    mCurrentTime = QDateTime::currentMSecsSinceEpoch();
    mPreviousTime = mCurrentTime;

    QtImGui::initialize(this);

    mController = new Controller;
    mController->SetWindow(this);
    mController->Init();
}

void RaycasterEngine::Window::resizeGL(int w, int h)
{
    mController->Resize(w, h);
}

void RaycasterEngine::Window::paintGL()
{
    mCurrentTime = QDateTime::currentMSecsSinceEpoch();
    float ifps = (mCurrentTime - mPreviousTime) * 0.001f;
    mPreviousTime = mCurrentTime;

    mController->Render(ifps);
}

void RaycasterEngine::Window::keyPressEvent(QKeyEvent *event)
{
    mController->KeyPressed(event);
}

void RaycasterEngine::Window::keyReleaseEvent(QKeyEvent *event)
{
    mController->KeyReleased(event);
}

void RaycasterEngine::Window::mousePressEvent(QMouseEvent *event)
{
    mController->MousePressed(event);
}

void RaycasterEngine::Window::mouseReleaseEvent(QMouseEvent *event)
{
    mController->MouseReleased(event);
}

void RaycasterEngine::Window::mouseMoveEvent(QMouseEvent *event)
{
    mController->MouseMoved(event);
}

void RaycasterEngine::Window::wheelEvent(QWheelEvent *event)
{
    mController->WheelMoved(event);
}
