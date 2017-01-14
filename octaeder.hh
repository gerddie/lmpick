#ifndef OCTAEDER_HH
#define OCTAEDER_HH

#include "globalscenestate.hh"

#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QOpenGLBuffer>


class Octaeder : private QOpenGLFunctions {
public:
        Octaeder();

        void draw(const GlobalSceneState& state);

        virtual void attach_gl();
        virtual void detach_gl();
private:

        QOpenGLBuffer m_arrayBuf;
        QOpenGLBuffer m_indexBuf;
        QOpenGLShaderProgram m_program;
};

#endif // OCTAEDER_HH
