#ifndef DRAWABLE_HH
#define DRAWABLE_HH

#include "globalscenestate.hh"
#include <QOpenGLFunctions>
#include <QOpenGLVertexArrayObject>
#include <memory>

class Drawable {
public:
        typedef std::shared_ptr<Drawable> Pointer;

        Drawable();
        virtual ~Drawable();

        void draw(const GlobalSceneState& state, QOpenGLFunctions& ogl) const;
        void attach_gl(QOpenGLFunctions& ogl);
        virtual void detach_gl(QOpenGLFunctions& ogl) = 0;
private:
        virtual void do_attach_gl(QOpenGLFunctions& ogl) = 0;
        virtual void do_draw(const GlobalSceneState& state, QOpenGLFunctions& ogl) const = 0;


};



#endif // DRAWABLE_HH
