#pragma once

#include <QOpenGLExtraFunctions>

namespace RaycasterEngine
{
    class Quad : protected QOpenGLExtraFunctions
    {
      public:
        Quad();

        void Render();
        void Destroy();

      private:
        unsigned int mVertexArray;
        unsigned int mVertexBuffer;
    };
}
