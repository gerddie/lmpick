#ifndef RENDERINGTHREAD_HH
#define RENDERINGTHREAD_HH

#include "volumedata.hh"
#include "landmarklistpainter.hh"

#include "octaeder.hh"

#include <QObject>
#include <QOpenGLFunctions>


class QMouseEvent;
class QWheelEvent;

class RenderingThread : public QObject, private QOpenGLFunctions {
public:
        RenderingThread(QWidget *widget);

        ~RenderingThread();

        void initialize();

        void run();

        void paint();

        void resize(int w, int h);

        bool mouse_release(QMouseEvent *ev);

        bool mouse_press(QMouseEvent *ev);

        bool mouse_tracking(QMouseEvent *ev);

        bool mouse_wheel(QWheelEvent *ev);

        void setVolume(PVolumeData volume);

        void setLandmarkList(PLandmarkList list);

        void set_volume_iso_value(int value);

        void detach_gl();

private:
        void update_rotation(QMouseEvent *ev);
        QVector3D get_mapped_point(const QPointF& localPos) const;
        void update_projection();

        QWidget *m_parent;
        bool m_is_gl_attached;
        QOpenGLContext *m_context;

        GlobalSceneState m_state;


        // the shader (must be moved into the actual geometries
        QOpenGLShaderProgram m_view_program;


        // used for mouse tracking
        bool m_mouse1_is_down;
        QPointF m_mouse_old_position;


        QVector2D m_viewport;

        VolumeData::Pointer m_volume;
        Octaeder::Pointer m_octaeder;
        LandmarkListPainter m_lmp;
};

#endif // RENDERINGTHREAD_HH
