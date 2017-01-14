#include "globalscenestate.hh"

GlobalSceneState::GlobalSceneState():
        camera_location(0,0,-550),
        rotation_center(0, 0, 0),
        light_source(1,1,1),
        rotation(1,0,0,0),
        zoom(1.0)
{
        light_source.normalize();
}

QMatrix4x4 GlobalSceneState::get_modelview_matrix() const
{
        QMatrix4x4 modelview;

        modelview.setToIdentity();
        modelview.translate(camera_location);
        modelview.rotate(rotation);
        modelview.translate(rotation_center);

        return modelview;
}

