#ifndef HELPER_H
#define HELPER_H

#include <QMatrix4x4>
#include <QQuaternion>
#include <QString>

class Helper
{
private:
    Helper();

public:
    static QByteArray getBytes(QString path);
    static QQuaternion rotateX(float angleRadians);
    static QQuaternion rotateY(float angleRadians);
    static QQuaternion rotateZ(float angleRadians);
    static QQuaternion invert(const QQuaternion &rotation);
    static float angleBetween(const QVector3D &v1, const QVector3D &v2, const QVector3D &left);
    static void getEulerDegrees(const QQuaternion &rotation, float &yaw, float &pitch, float &roll);
    static QVector3D getEulerDegrees(const QQuaternion &rotation);
    static QQuaternion constructFromEulerDegrees(float yaw, float pitch, float roll);
    static QVector2D rotate(const QVector2D &subject, float angle);
};

#endif // HELPER_H
