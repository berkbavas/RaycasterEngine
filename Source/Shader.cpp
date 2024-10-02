#include "Shader.h"

#include "Helper.h"

#include <QDebug>
#include <QFile>

RaycasterEngine::Shader::Shader(const QString& name)
    : mProgram(nullptr)
    , mName(name)
{
}

void RaycasterEngine::Shader::Initialize()
{
    LOG_INFO("Shader::Initialize: '{}' is being initializing.", mName.toStdString());

    initializeOpenGLFunctions();

    mProgram = QSharedPointer<QOpenGLShaderProgram>(new QOpenGLShaderProgram);

    for (const auto [shaderType, path] : mPaths)
    {
        const auto bytes = Helper::GetBytes(path);
        if (!mProgram->addShaderFromSourceCode(shaderType, bytes))
        {
            RE_EXIT_FAILURE("Shader::Initialize: '{}' could not be loaded.", GetShaderTypeString(shaderType).toStdString());
        }
    }

    if (!mProgram->link())
    {
        RE_EXIT_FAILURE("Shader::Initialize: Could not link shader program.");
    }

    if (!mProgram->bind())
    {
        RE_EXIT_FAILURE("Shader::Initialize: Could not bind shader program.");
    }

    LOG_INFO("Shader::Initialize: '{}' has been initialized.", mName.toStdString());
}

bool RaycasterEngine::Shader::Bind()
{
    return mProgram->bind();
}

void RaycasterEngine::Shader::Release()
{
    mProgram->release();
}

void RaycasterEngine::Shader::AddPath(QOpenGLShader::ShaderTypeBit type, const QString& path)
{
    mPaths.emplace(type, path);
}

QString RaycasterEngine::Shader::GetName() const
{
    return mName;
}

QString RaycasterEngine::Shader::GetShaderTypeString(QOpenGLShader::ShaderTypeBit type)
{
    switch (type)
    {
        case QOpenGLShader::Vertex:
            return "Vertex Shader";
        case QOpenGLShader::Fragment:
            return "Fragment Shader";
        case QOpenGLShader::Geometry:
            return "Geometry Shader";
        case QOpenGLShader::TessellationControl:
            return "Tessellation Control Shader";
        case QOpenGLShader::TessellationEvaluation:
            return "Tesselation Evaluation Shader";
        case QOpenGLShader::Compute:
            return "Compute Shader";
        default:
            return "Unknown Shader";
    }
}
