#ifndef CONTROLLER_H
#define CONTROLLER_H

#include "Quad.h"
#include "Shader.h"

#include <QKeyEvent>
#include <QObject>
#include <QOpenGLFunctions_4_3_Core>

#include <imgui.h>
#include <QtImGui.h>

class Window;

class Controller : public QObject, protected QOpenGLFunctions_4_3_Core
{
    Q_OBJECT
public:
    explicit Controller(QObject *parent = nullptr);
    virtual ~Controller();

    void init();
    void wheelMoved(QWheelEvent *event);
    void mousePressed(QMouseEvent *event);
    void mouseReleased(QMouseEvent *event);
    void mouseMoved(QMouseEvent *event);
    void keyPressed(QKeyEvent *event);
    void keyReleased(QKeyEvent *event);
    void resize(int w, int h);
    void render(float ifps);
    void setWindow(Window *newWindow);
    void update();

private:
    Window *mWindow;
    float mIfps;

    // Render
    unsigned int mCleanerFramebuffer;
    unsigned int mRaycasterOutputImage;
    unsigned int mRaycasterWorldMap;

    Shader *mScreenShader;
    Shader *mFlatRaycasterShader;
    Quad *mQuad;

    // Events
    Qt::MouseButton mPressedButton;
    bool mImGuiWantsMouseCapture;
    bool mImGuiWantsKeyboardCapture;

    // Game
    struct Player {
        QVector2D position;
        QVector2D direction;
        double movementSpeed;
        double angularSpeed;

    } mPlayer;

    struct Camera {
        QVector2D plane;

    } mCamera;

    QMap<Qt::Key, bool> mPressedKeys;

    static const int WORLD_MAP[24][24];
};

#endif // CONTROLLER_H
