#ifndef LANDMARKLISTPAINTER_HH
#define LANDMARKLISTPAINTER_HH

#include "landmarklist.hh"
#include "drawable.hh"

class LandmarkListPainter : public Drawable
{
public:
        LandmarkListPainter();
        ~LandmarkListPainter();

        void set_landmark_list(PLandmarkList list);
        void set_viewspace_correction(const QVector3D& scale, const QVector3D& shift);

        const QString get_active_landmark_name() const;

        virtual void detach_gl(QOpenGLContext& context);
private:
        virtual void do_attach_gl(QOpenGLContext& context);
        virtual void do_draw(const GlobalSceneState& state, QOpenGLContext& context) const;

        struct LandmarkListPainterImpl *impl;
};

#endif // LANDMARKLISTPAINTER_HH
