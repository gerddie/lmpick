#ifndef SPHERE_HH
#define SPHERE_HH

#include "drawable.hh"

class Sphere : public Drawable
{
public:
        explicit Sphere(const QVector4D& color);
        ~Sphere();
        virtual void detach_gl(QOpenGLContext& context);
private:
        virtual void do_attach_gl(QOpenGLContext& context);
        virtual void do_draw(const GlobalSceneState& state, QOpenGLContext& context) const;

        QVector4D m_base_color;
        static int m_instances;
        static int m_instances_gl_attached;
        static struct SphereImpl *impl;
};

#endif // SPHERE_HH
