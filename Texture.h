#ifndef TEXTURE_H
#define TEXTURE_H

#include <QOpenGLExtraFunctions>
#include <QOpenGLTexture>

class Texture : protected QOpenGLExtraFunctions
{
public:
    Texture(const QString &path);
    ~Texture();

    unsigned int id() const;
    const QString &path() const;

private:
    unsigned int mID;
    QString mPath;
};

#endif // TEXTURE_H
