#pragma once

#include "Quad.h"
#include "Shader.h"
#include "Texture.h"

#include <QMouseEvent>
#include <QObject>
#include <QOpenGLFunctions_4_5_Core>
#include <QtImGui.h>
#include <imgui.h>

namespace RaycasterEngine
{
    class Window;

    class Controller : public QObject, protected QOpenGLFunctions_4_5_Core
    {
        Q_OBJECT
      public:
        Controller();

        void Run();
        void Initialize();
        void Render(float ifps);
        void Update(float ifps);

        void MousePressed(QMouseEvent* event);
        void MouseReleased(QMouseEvent* event);
        void MouseMoved(QMouseEvent* event);
        void KeyPressed(QKeyEvent* event);
        void KeyReleased(QKeyEvent* event);

      private:
        void RenderFloor();
        void RenderWalls();
        void RenderSprites();

        Window* mWindow;
        float mAspectRatio;

        // Framebuffers
        unsigned int mCleanerFramebuffer{ 0 };
        unsigned int mRaycasterOutputImage{ 0 };
        unsigned int mRaycasterWorldMap{ 0 };
        unsigned int mRaycasterDepthBuffer{ 0 };

        // Shaders
        Shader* mScreenShader;
        Shader* mRaycasterFlatShader;
        Shader* mRaycasterFloorShader;
        Shader* mRaycasterTexturedShader;
        Shader* mRaycasterSpritesShader;

        Quad* mQuad;

        // Events
        Qt::MouseButton mPressedButton{ Qt::NoButton };
        bool mImGuiWantsMouseCapture{ false };
        bool mImGuiWantsKeyboardCapture{ false };

        // Game
        struct Player
        {
            QVector2D position;
            QVector2D direction;
            double movementSpeed;
            double angularSpeed;
        } mPlayer;

        struct Camera
        {
            QVector2D plane;
        } mCamera;

        struct Sprite
        {
            double x;
            double y;
            int texture;
            double distance;
        };

        QMap<Qt::Key, bool> mPressedKeys;

        enum class TextureName
        { //
            Barrel,
            BlueStone,
            ColorStone,
            Eagle,
            GreenLight,
            GreyStone,
            Mossy,
            Pillar,
            PurpleStone,
            RedBrick,
            Wood,
            All
        };

        QVector<int> mSpriteIndices;

        QMap<TextureName, Texture*> mTextures;

        QVector<Sprite> mSprites = {
            // Green light in front of playerstart
            { 20.5, 11.5, 10 },

            // Green lights in every room
            { 18.5, 4.5, 10 },
            { 10.0, 4.5, 10 },
            { 10.0, 12.5, 10 },
            { 3.5, 6.5, 10 },
            { 3.5, 20.5, 10 },
            { 3.5, 14.5, 10 },
            { 14.5, 20.5, 10 },

            // Row of pillars in front of wall: fisheye test
            { 18.5, 10.5, 9 },
            { 18.5, 11.5, 9 },
            { 18.5, 12.5, 9 },

            // Some barrels around the map
            { 21.5, 1.5, 8 },
            { 16.0, 1.8, 8 },
            { 3.5, 2.5, 8 },
            { 10.5, 15.8, 8 },
        };

        int mWorldMap[24][24] = {
            { 2, 2, 2, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 2, 2, 2, 2, 2, 2, 1, 2, 2 }, //
            { 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2 }, //
            { 2, 0, 2, 2, 0, 0, 0, 0, 0, 2, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2 }, //
            { 1, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2 }, //
            { 2, 0, 2, 2, 0, 0, 0, 0, 0, 2, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2 }, //
            { 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 2, 0, 0, 0, 0, 0, 2, 2, 2, 0, 2, 2, 2 }, //
            { 2, 2, 2, 2, 0, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 2, 2, 2, 0, 0, 0, 0, 0, 2 }, //
            { 2, 2, 2, 2, 0, 2, 2, 2, 2, 0, 2, 0, 2, 0, 2, 0, 2, 2, 0, 8, 0, 8, 0, 2 }, //
            { 2, 2, 0, 0, 0, 0, 0, 0, 2, 2, 0, 2, 0, 2, 0, 2, 2, 2, 0, 0, 0, 0, 0, 2 }, //
            { 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 2, 0, 0, 0, 0, 0, 2 }, //
            { 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 2, 0, 2, 0, 2, 0, 2 }, //
            { 2, 2, 0, 0, 0, 0, 0, 0, 2, 2, 0, 2, 0, 2, 0, 2, 2, 2, 2, 2, 0, 2, 2, 2 }, //
            { 2, 2, 2, 2, 0, 2, 2, 2, 2, 2, 2, 2, 0, 2, 2, 2, 2, 2, 2, 2, 0, 2, 2, 2 }, //
            { 2, 2, 2, 2, 0, 2, 2, 2, 2, 2, 2, 2, 0, 0, 2, 0, 2, 2, 0, 0, 0, 0, 0, 2 }, //
            { 2, 2, 0, 0, 0, 0, 0, 2, 2, 2, 0, 0, 0, 0, 0, 0, 2, 2, 0, 0, 0, 0, 0, 2 }, //
            { 2, 0, 0, 0, 0, 0, 0, 0, 2, 2, 0, 0, 0, 0, 0, 0, 2, 2, 0, 0, 0, 0, 0, 2 }, //
            { 1, 0, 0, 0, 0, 0, 0, 0, 1, 2, 2, 2, 2, 2, 2, 0, 2, 2, 2, 0, 0, 0, 2, 2 }, //
            { 2, 0, 0, 0, 0, 0, 0, 0, 2, 2, 2, 1, 2, 2, 2, 2, 2, 0, 0, 2, 0, 8, 0, 2 }, //
            { 2, 2, 0, 0, 0, 0, 0, 2, 2, 2, 0, 0, 0, 2, 2, 0, 2, 0, 2, 0, 0, 0, 2, 2 }, //
            { 2, 0, 0, 0, 0, 0, 0, 0, 8, 0, 0, 0, 0, 0, 2, 2, 0, 8, 0, 8, 0, 8, 0, 2 }, //
            { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2 }, //
            { 2, 0, 0, 0, 0, 0, 0, 0, 8, 0, 0, 0, 0, 0, 2, 2, 0, 8, 0, 8, 0, 8, 0, 2 }, //
            { 2, 2, 0, 0, 0, 0, 0, 2, 2, 2, 0, 0, 0, 2, 2, 0, 2, 0, 2, 0, 0, 0, 2, 2 }, //
            { 2, 2, 2, 2, 1, 2, 2, 2, 2, 2, 2, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2 }  //
        };
    };
}