#include "Window.h"

#include <QDateTime>

RaycasterEngine::Window::Window(QWindow* parent)
    : QOpenGLWindow(QOpenGLWindow::UpdateBehavior::NoPartialUpdate, parent)
{
    connect(this, &QOpenGLWindow::frameSwapped, [=]()
            { update(); });
}

void RaycasterEngine::Window::initializeGL()
{
    initializeOpenGLFunctions();

    mCurrentTime = QDateTime::currentMSecsSinceEpoch();
    mPreviousTime = mCurrentTime;

    emit Initialize();
}

void RaycasterEngine::Window::resizeGL(int width, int height)
{
    emit Resize(width, height);
}

void RaycasterEngine::Window::paintGL()
{
    mCurrentTime = QDateTime::currentMSecsSinceEpoch();
    const float ifps = (mCurrentTime - mPreviousTime) * 0.001f;
    mPreviousTime = mCurrentTime;

    emit Render(ifps);
}

void RaycasterEngine::Window::keyPressEvent(QKeyEvent* event)
{
    emit KeyPressed(event);
}

void RaycasterEngine::Window::keyReleaseEvent(QKeyEvent* event)
{
    emit KeyReleased(event);
}

void RaycasterEngine::Window::mousePressEvent(QMouseEvent* event)
{
    emit MousePressed(event);
}

void RaycasterEngine::Window::mouseReleaseEvent(QMouseEvent* event)
{
    emit MouseReleased(event);
}

void RaycasterEngine::Window::mouseMoveEvent(QMouseEvent* event)
{
    emit MouseMoved(event);
}

void RaycasterEngine::Window::wheelEvent(QWheelEvent* event)
{
    emit WheelMoved(event);
}
