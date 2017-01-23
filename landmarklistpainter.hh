#ifndef LANDMARKLISTPAINTER_HH
#define LANDMARKLISTPAINTER_HH

#include "drawable.hh"

class LandmarkListPainter : public Drawable
{
public:
        LandmarkListPainter();

        virtual void detach_gl(QOpenGLContext& context);
private:
        virtual void do_attach_gl(QOpenGLContext& context);
        virtual void do_draw(const GlobalSceneState& state, QOpenGLContext& context) const;

        struct LandmarkListPainterImpl *impl;
};

#endif // LANDMARKLISTPAINTER_HH
