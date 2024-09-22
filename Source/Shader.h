#pragma once

#include <QOpenGLFunctions>
#include <QOpenGLShader>

namespace RaycasterEngine
{
    class Shader : protected QOpenGLFunctions
    {
    public:
        Shader(const QString& name);
        ~Shader();

        bool Init();
        bool Bind();
        void Release();

        void AddPath(QOpenGLShader::ShaderTypeBit type, const QString& path);
        void AddUniform(const QString& uniform);
        void AddUniforms(const QStringList& uniforms);
        void SetUniformArray(const QString& uniform, int size);
        void AddAttribute(const QString& attribute);
        void AddAttributes(const QStringList& attributes);

        void SetUniformValue(const QString& name, int value);
        void SetUniformValue(const QString& name, unsigned int value);
        void SetUniformValue(const QString& name, float value);
        void SetUniformValue(const QString& name, const QVector2D& value);
        void SetUniformValue(const QString& name, const QVector3D& value);
        void SetUniformValue(const QString& name, const QVector4D& value);
        void SetUniformValue(const QString& name, const QMatrix4x4& value);
        void SetUniformValue(const QString& name, const QMatrix3x3& value);
        void SetUniformValueArray(const QString& name, const QVector<QVector3D>& values);
        void SetSampler(const QString& name, unsigned int unit, unsigned int id, GLenum target = GL_TEXTURE_2D);

        QString GetShaderTypeString(QOpenGLShader::ShaderTypeBit type);

    private:
        QString mName;
        QOpenGLShaderProgram* mProgram;
        QMap<QOpenGLShader::ShaderTypeBit, QString> mPaths;
        QMap<QString, GLuint> mLocations;
        QStringList mAttributes;
        QStringList mUniforms;
        QMap<QString, int> mUniformArrays;
        bool mSuccess;
    };
}
