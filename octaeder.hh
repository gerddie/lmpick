#ifndef OCTAEDER_HH
#define OCTAEDER_HH

#include "drawable.hh"

#include <QOpenGLShaderProgram>
#include <QOpenGLBuffer>

class Octaeder : public  Drawable {
public:
        Octaeder();


        virtual void attach_gl();
        virtual void detach_gl();
private:
        void do_draw(const GlobalSceneState& state, QOpenGLFunctions& ogl) const override;

        mutable QOpenGLBuffer m_arrayBuf;
        mutable QOpenGLBuffer m_indexBuf;
        mutable QOpenGLShaderProgram m_program;
};


#endif // OCTAEDER_HH
