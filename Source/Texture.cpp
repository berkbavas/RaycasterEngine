#include "Texture.h"

#include "Logger.h"

#include <QImage>

RaycasterEngine::Texture::Texture(const QString& path, const QString& name, GLuint unit)
    : mName(name)
    , mUnit(unit)

{
    initializeOpenGLFunctions();

    QImage image(path);
    image = image.convertToFormat(QImage::Format_RGBA8888);

    if (image.isNull())
    {
        RE_EXIT_FAILURE("Texture::Texture: Could not load image at '{}'.", path.toStdString());
    }

    glGenTextures(1, &mId);
    glBindTexture(GL_TEXTURE_2D, mId);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, image.width(), image.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, image.constBits());
    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    mWidth = image.width();
    mHeight = image.height();
    mTarget = GL_TEXTURE_2D;

    LOG_DEBUG("Texture::Texture: Texture '{}' has been loaded.", path.toStdString());
}