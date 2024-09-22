#include "Texture.h"

#include <QDebug>

RaycasterEngine::Texture::Texture(const QString& path)
    : mPath(path)
{
    initializeOpenGLFunctions();

    glGenTextures(1, &mID);
    QImage image(mPath);
    image = image.convertToFormat(QImage::Format_RGBA8888);

    glBindTexture(GL_TEXTURE_2D, mID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, image.width(), image.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, image.constBits());
    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    qInfo() << "Texture" << path << "is loaded and created. ID is" << mID << "Size is" << image.width() << "x" << image.height();
}

RaycasterEngine::Texture::~Texture()
{
    glDeleteTextures(1, &mID);
}

unsigned int RaycasterEngine::Texture::ID() const
{
    return mID;
}

const QString& RaycasterEngine::Texture::GetPath() const
{
    return mPath;
}