#include "mainopenglview.hh"
#include "octaeder.hh"
#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QOpenGLVertexArrayObject>
#include <QMatrix4x4>
#include <cassert>
#include <memory>

class RenderingThread : private QOpenGLFunctions {
public:
        RenderingThread(QWidget *widget);

        void initialize();

        void run();

        void paint();

        void resize(int w, int h);
private:
        QWidget *m_parent;
        QOpenGLContext *m_context;

        QMatrix4x4 m_projection;
        QMatrix4x4 m_modelview;
        QOpenGLShaderProgram m_view_program;
        std::unique_ptr<Octaeder> m_octaeder;

        QOpenGLVertexArrayObject m_vao;
};

MainopenGLView::MainopenGLView(QWidget *parent):
        QOpenGLWidget(parent),
        m_rendering(nullptr)
{
        QSurfaceFormat format;
        format.setDepthBufferSize(24);
        format.setVersion(3, 2);
        format.setProfile(QSurfaceFormat::CoreProfile);
        setFormat(format);
        m_rendering = new RenderingThread(this);


}

MainopenGLView::~MainopenGLView()
{
        delete m_rendering;
}

void MainopenGLView::initializeGL()
{
        m_rendering->initialize();
}

void MainopenGLView::paintGL()
{
        m_rendering->paint();
}

void MainopenGLView::resizeGL(int w, int h)
{
        m_rendering->resize(w,h);
}

RenderingThread::RenderingThread(QWidget *parent):
        m_parent(parent),
        m_context(nullptr)
{
        m_modelview.setToIdentity();
        m_modelview.lookAt(QVector3D(-10, -5, 0), QVector3D(0,0,0),QVector3D(0,0,1));
        qDebug() << "m_modelview= " << m_modelview;
}

void RenderingThread::initialize()
{
        initializeOpenGLFunctions();
        m_context =  QOpenGLContext::currentContext();
        if (!m_view_program.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/view.glsl"))
                qWarning() << "Error compiling ':/shaders/view.glsl', view will be clobbered\n";

        if (!m_view_program.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/basic_frag.glsl"))
                qWarning() << "Error compiling ':/s makeCurrent();haders/fshader.glsl', view will be clobbered\n";

        if (!m_view_program.link())
                qWarning() << "Error linking m_view_program', view will be clobbered\n";;

        glClearColor(0,0.1,0,1);

        glEnable(GL_DEPTH_TEST);

        // Enable back face culling
        // glEnable(GL_CULL_FACE);

        m_vao.create();
        m_vao.bind();

        m_octaeder.reset(new Octaeder);
}

void RenderingThread::paint()
{

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        if (!m_view_program.bind())
                qWarning() << "Error binding m_view_program', view will be clobbered\n";;

        QVector3D ld(1, 1, 1);
        ld.normalize();

        m_view_program.setUniformValue("qt_mvp", m_projection * m_modelview);
        auto error = glGetError();
        qDebug() << "m_view_program.setUniformValue(qt_mvp: " << error;

        m_view_program.setUniformValue("qt_mv", m_modelview);
        error = glGetError();
        qDebug() << "m_view_program.setUniformValue(qt_mv: " << error;

        m_view_program.setUniformValue("qt_LightDirection", ld);
        error = glGetError();
        qDebug() << "m_view_program.setUniformValue(qt_LightDirection: " << error;

        if (m_octaeder)
                m_octaeder->draw(m_view_program);
        else
                qDebug() << "nothing to draw\n";



}

void RenderingThread::resize(int w, int h)
{
        glViewport(0,0,w,h);
        m_projection.setToIdentity();
        m_projection.perspective(15.0f, w/static_cast<float>(h), 10, 30);
        qDebug() << "m_perspective = " << m_projection;
}

void RenderingThread::run()
{

}
