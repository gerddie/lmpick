#include "mainopenglview.hh"
#include "octaeder.hh"
#include <mia/3d/camera.hh>
#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QOpenGLVertexArrayObject>
#include <QMouseEvent>
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

        bool mouse_tracking(QMouseEvent *ev);
private:
        void update_rotation(QMouseEvent *ev);
        QWidget *m_parent;
        QOpenGLContext *m_context;

        QMatrix4x4 m_projection;
        QMatrix4x4 m_modelview;
        QOpenGLShaderProgram m_view_program;
        std::unique_ptr<Octaeder> m_octaeder;

        QOpenGLVertexArrayObject m_vao;

        QVector3D m_camera_location;
        QVector3D m_rotation_center;
        QQuaternion m_rotation;
        float m_zoom;

	bool m_mouse1_is_down;
	QPointF m_mouse_old_position; 

	QVector2D m_viewport; 
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

void MainopenGLView::mouseMoveEvent(QMouseEvent *ev)
{
	if (!m_rendering->mouse_tracking(ev)) {
		// handle mouse here
		
	}
}

RenderingThread::RenderingThread(QWidget *parent):
        m_parent(parent),
        m_context(nullptr),
        m_camera_location(0,0,-250),
        m_rotation_center(0, 0, 0),
        m_rotation(0,0,0,1),
        m_zoom(1.0),
	m_mouse1_is_down(false)
	
{
        m_modelview.setToIdentity();
        m_modelview.translate(m_camera_location);
        m_modelview.rotate(m_rotation);
        m_modelview.translate(m_rotation_center);

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
         glEnable(GL_CULL_FACE);

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

void RenderingThread::update_rotation(QMouseEvent *ev)
{
    QVector2D pos(ev->localPos());
    QVector2D old_pos(m_mouse_old_position);

	auto rel_pos = (2 * pos - m_viewport) / m_viewport;
	auto old_rel_pos = (2 * old_pos - m_viewport) / m_viewport;

	// implement trackball 

	
	
}

bool RenderingThread::mouse_tracking(QMouseEvent *ev)
{
	switch (ev->button()) {
	case Qt::LeftButton:
		switch (ev->type()) {
		case QEvent::MouseButtonPress:
			m_mouse1_is_down = true; 
			m_mouse_old_position = ev->localPos();
			break;
		case QEvent::MouseButtonRelease:
			m_mouse1_is_down = false;
			break;
		case QEvent::MouseMove:
			if (m_mouse1_is_down) {
                update_rotation(ev);
				m_mouse_old_position = ev->localPos();
			}
		default:
			return false;
		}
	default:
		return false; 
	}
	return true; 
}


void RenderingThread::resize(int w, int h)
{
        glViewport(0,0,w,h);
        m_projection.setToIdentity();
        float zw, zh;
        if (w > h) {
               zw = m_zoom * w / h;
               zh = m_zoom;
        }else{
                zh = m_zoom * h / w;
                zw = m_zoom;
        }
        m_projection.frustum(-zw, zw, -zh, zh, 200, 300);
        qDebug() << "m_perspective = " << m_projection;

    m_viewport = QVector2D(w, h);
}

void RenderingThread::run()
{

}
