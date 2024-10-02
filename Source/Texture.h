#pragma once

#include "Macros.h"

#include <QOpenGLExtraFunctions>
#include <QString>

namespace RaycasterEngine
{
    class Texture : protected QOpenGLExtraFunctions
    {
      public:
        Texture(const QString& path, const QString& name, GLuint unit);

        DEFINE_MEMBER_CONST(GLuint, Id);     // OpenGL handle
        DEFINE_MEMBER_CONST(GLuint, Target); // GL_TEXTURE_2D, GL_TEXTURE_CUBE_MAP etc.
        DEFINE_MEMBER_CONST(GLuint, Unit);   // Uniform location
        DEFINE_MEMBER_CONST(QString, Name);  // Texture name in the shader
        DEFINE_MEMBER_CONST(int, Width);
        DEFINE_MEMBER_CONST(int, Height);
    };
}