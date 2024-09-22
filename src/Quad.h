#pragma once

#include <QOpenGLExtraFunctions>
#include <QOpenGLFunctions>
#include <QOpenGLVertexArrayObject>

namespace RaycasterEngine
{
    class Quad : protected QOpenGLExtraFunctions
    {
    public:
        Quad();
        ~Quad();

        void Render();

    private:
        unsigned int mVAO;
        unsigned int mVBO;
        static const float VERTICES[24];
    };
}
