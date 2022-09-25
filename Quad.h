#ifndef QUAD_H
#define QUAD_H

#include <QOpenGLExtraFunctions>
#include <QOpenGLFunctions>
#include <QOpenGLVertexArrayObject>

class Quad : protected QOpenGLExtraFunctions
{
public:
    Quad();
    ~Quad();

    void render();

private:
    unsigned int mVAO;
    unsigned int mVBO;
    static const float VERTICES[24];
};

#endif // QUAD_H
