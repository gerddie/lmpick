#ifndef OCTAEDER_HH
#define OCTAEDER_HH


#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QOpenGLBuffer>


class Octaeder : private QOpenGLFunctions {
public:
        Octaeder();

        void draw(QOpenGLShaderProgram& program);
private:

        QOpenGLBuffer m_arrayBuf;
        QOpenGLBuffer m_indexBuf;

};

#endif // OCTAEDER_HH
