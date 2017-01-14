#ifndef DRAWABLE_HH
#define DRAWABLE_HH

#include "globalscenestate.hh"
#include <QOpenGLFunctions>
#include <memory>

class Drawable {
public:
        typedef std::shared_ptr<Drawable> Pointer;

        Drawable();
        virtual ~Drawable();

        void draw(const GlobalSceneState& state, QOpenGLFunctions& ogl) const;
        virtual void attach_gl() = 0;
        virtual void detach_gl() = 0;
private:
        virtual void do_draw(const GlobalSceneState& state, QOpenGLFunctions& ogl) const = 0;
};



#endif // DRAWABLE_HH
