#pragma once

#include <QOpenGLExtraFunctions>
#include <QOpenGLTexture>

namespace RaycasterEngine
{
    class Texture : protected QOpenGLExtraFunctions
    {
    public:
        Texture(const QString& path);
        ~Texture();

        unsigned int ID() const;
        const QString& GetPath() const;

    private:
        unsigned int mID;
        QString mPath;
    };
}
