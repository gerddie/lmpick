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
private:

        QOpenGLBuffer m_arrayBuf;
        QOpenGLBuffer m_indexBuf;
        QOpenGLShaderProgram m_program;
};

#endif // OCTAEDER_HH
