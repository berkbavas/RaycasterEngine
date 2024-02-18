#include "Quad.h"

RaycasterEngine::Quad::Quad()
    : mVAO(0)
    , mVBO(0)
{
    initializeOpenGLFunctions();
    glGenVertexArrays(1, &mVAO);
    glBindVertexArray(mVAO);

    glGenBuffers(1, &mVBO);
    glBindBuffer(GL_ARRAY_BUFFER, mVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(VERTICES), VERTICES, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
}

RaycasterEngine::Quad::~Quad()
{
    glDeleteVertexArrays(1, &mVAO);
    glDeleteBuffers(1, &mVBO);
}

void RaycasterEngine::Quad::Render()
{
    glBindVertexArray(mVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}

const float RaycasterEngine::Quad::VERTICES[24] = { 
                                  -1.0f, 1.0f,  0.0f, 1.0f, //
                                  -1.0f, -1.0f, 0.0f, 0.0f, //
                                  1.0f,  -1.0f, 1.0f, 0.0f, //
                                  -1.0f, 1.0f,  0.0f, 1.0f, //
                                  1.0f,  -1.0f, 1.0f, 0.0f, //
                                  1.0f,  1.0f,  1.0f, 1.0f };