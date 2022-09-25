#include "Helper.h"

#include <QDebug>
#include <QFile>
#include <QtMath>

Helper::Helper() {}

QByteArray Helper::getBytes(QString path)
{
    QFile file(path);
    if (file.open(QFile::ReadOnly))
    {
        return file.readAll();
    } else
    {
        qWarning() << QString("Could not open '%1'").arg(path);
        return QByteArray();
    }
}

QQuaternion Helper::rotateX(float angleRadians)
{
    return QQuaternion::fromAxisAndAngle(QVector3D(1, 0, 0), qRadiansToDegrees(angleRadians));
}

QQuaternion Helper::rotateY(float angleRadians)
{
    return QQuaternion::fromAxisAndAngle(QVector3D(0, 1, 0), qRadiansToDegrees(angleRadians));
}

QQuaternion Helper::rotateZ(float angleRadians)
{
    return QQuaternion::fromAxisAndAngle(QVector3D(0, 0, 1), qRadiansToDegrees(angleRadians));
}

QQuaternion Helper::invert(const QQuaternion &rotation)
{
    float yaw, pitch, roll;
    getEulerDegrees(rotation, yaw, pitch, roll);

    QQuaternion r = QQuaternion::fromAxisAndAngle(QVector3D(0, 1, 0), yaw);
    r = r * QQuaternion::fromAxisAndAngle(QVector3D(1, 0, 0), -pitch);
    r = r * QQuaternion::fromAxisAndAngle(QVector3D(0, 0, 1), roll);
    return r;
}

float Helper::angleBetween(const QVector3D &v1, const QVector3D &v2, const QVector3D &left)
{
    QVector3D u1 = v1.normalized();
    QVector3D u2 = v2.normalized();

    float dot = QVector3D::dotProduct(u1, u2);

    if (qFuzzyCompare(dot, 1.0f))
    {
        return 0.0f;

    } else if (qFuzzyCompare(dot, -1.0f))
    {
        return 180.0f;

    } else
    {
        float angle = qRadiansToDegrees(acos(dot));
        QVector3D u1xu2 = QVector3D::crossProduct(u1, u2);

        if (QVector3D::dotProduct(u1xu2, left) < 0.0001f)
            return angle;
        else
            return -angle;
    }
}

void Helper::getEulerDegrees(const QQuaternion &rotation, float &yaw, float &pitch, float &roll)
{
    QVector3D zAxis = rotation * QVector3D(0, 0, -1);
    float x = zAxis.x();
    float y = zAxis.y();
    float z = zAxis.z();
    yaw = qRadiansToDegrees(atan2(-z, x)) - 90;
    pitch = qRadiansToDegrees(atan2(y, sqrt(z * z + x * x)));

    QVector3D xAxis = rotation * QVector3D(1, 0, 0);

    QVector3D xAxisProj = QQuaternion::fromAxisAndAngle(QVector3D(0, 1, 0), yaw) *   //
                          QQuaternion::fromAxisAndAngle(QVector3D(1, 0, 0), pitch) * //
                          QVector3D(1, 0, 0);

    QVector3D left = QQuaternion::fromAxisAndAngle(QVector3D(0, 1, 0), yaw) *   //
                     QQuaternion::fromAxisAndAngle(QVector3D(1, 0, 0), pitch) * //
                     QVector3D(0, 0, -1);

    roll = Helper::angleBetween(xAxis, xAxisProj, left);

    if (yaw < 0.0f)
        yaw += 360.0f;
}

QVector3D Helper::getEulerDegrees(const QQuaternion &rotation)
{
    float yaw, pitch, roll;

    getEulerDegrees(rotation, yaw, pitch, roll);

    return QVector3D(yaw, pitch, roll);
}

QQuaternion Helper::constructFromEulerDegrees(float yaw, float pitch, float roll)
{
    return QQuaternion::fromAxisAndAngle(QVector3D(0, 1, 0), yaw) *   //
           QQuaternion::fromAxisAndAngle(QVector3D(1, 0, 0), pitch) * //
           QQuaternion::fromAxisAndAngle(QVector3D(0, 0, -1), roll);
}

QVector2D Helper::rotate(const QVector2D &subject, float angle)
{
    auto rotation = QQuaternion::fromAxisAndAngle(QVector3D(0, 0, 1), angle);
    return QVector2D(rotation * QVector3D(subject, 0));
}
