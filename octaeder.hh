#ifndef OCTAEDER_HH
#define OCTAEDER_HH

#include "drawable.hh"

#include <QOpenGLShaderProgram>
#include <QOpenGLBuffer>

class Octaeder : public  Drawable {
public:
        Octaeder();

        virtual void detach_gl();
private:
        void do_draw(const GlobalSceneState& state, QOpenGLFunctions& ogl) const override;
        virtual void do_attach_gl() override;

        mutable QOpenGLBuffer m_arrayBuf;
        mutable QOpenGLBuffer m_indexBuf;
        mutable QOpenGLShaderProgram m_program;

        // some OpenGL stuff globally required
        mutable QOpenGLVertexArrayObject m_vao;
};


#endif // OCTAEDER_HH
