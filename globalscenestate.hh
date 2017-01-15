#ifndef GLOBALSCENESTATE_HH
#define GLOBALSCENESTATE_HH

#include <QVector3D>
#include <QMatrix4x4>
#include <QQuaternion>
#include <QSize>

class GlobalSceneState
{
public:
        GlobalSceneState();

        QMatrix4x4 get_modelview_matrix() const;

        QVector3D camera_location;
        QVector3D rotation_center;
        QVector3D light_source;
        QQuaternion rotation;
        QMatrix4x4 projection;
        float zoom;
        QSize viewport;
};

#endif // GLOBALSCENESTATE_HH
