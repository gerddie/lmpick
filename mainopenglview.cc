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
#include <cmath>

/*
  This class implemnts the actuaal OpenGL rendering. It is planned
  to move the OpenGL handling to a separate thread that will be
  encaapuslated by this here.
*/

class RenderingThread : private QOpenGLFunctions {
public:
        RenderingThread(QWidget *widget);

        void initialize();

        void run();

        void paint();

        void resize(int w, int h);

        bool mouse_press(QMouseEvent *ev);

        bool mouse_tracking(QMouseEvent *ev);
private:
        void update_rotation(QMouseEvent *ev);
        QVector3D get_mapped_point(const QPointF& localPos) const;

        QWidget *m_parent;
        QOpenGLContext *m_context;

        QMatrix4x4 m_projection;
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
        setMouseTracking( true );
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
    update();
}

void MainopenGLView::mouseReleaseEvent(QMouseEvent *ev)
{
    if (!m_rendering->mouse_press(ev)) {
        // handle mouse here

    }
}

void MainopenGLView::mousePressEvent(QMouseEvent *ev)
{
    if (!m_rendering->mouse_press(ev)) {
        // handle mouse here

    }
    update();
}

RenderingThread::RenderingThread(QWidget *parent):
        m_parent(parent),
        m_context(nullptr),
        m_camera_location(0,0,-550),
        m_rotation_center(0, 0, 0),
        m_rotation(1,0,0,0),
        m_zoom(1.0),
        m_mouse1_is_down(false)
	
{
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
        QMatrix4x4 modelview;

        modelview.setToIdentity();
        modelview.translate(m_camera_location);
        modelview.rotate(m_rotation);
        modelview.translate(m_rotation_center);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        if (!m_view_program.bind())
                qWarning() << "Error binding m_view_program', view will be clobbered\n";;

        QVector3D ld(1, 1, 1);
        ld.normalize();

        m_view_program.setUniformValue("qt_mvp", m_projection * modelview);
        m_view_program.setUniformValue("qt_mv", modelview);
        m_view_program.setUniformValue("qt_LightDirection", ld);

        if (m_octaeder)
                m_octaeder->draw(m_view_program);
        else
                qDebug() << "nothing to draw\n";



}

QVector3D RenderingThread::get_mapped_point(const QPointF& localPos) const
{
        double x = (2 * localPos.x() - m_viewport.x()) / m_viewport.x();
        double y = (m_viewport.y() - 2 * localPos.y()) / m_viewport.y();
        double z;
        auto l2 = x * x + y * y;
        if (l2 < 0.5) {
                z = std::sqrt(1.0 - l2);
        } else {
                z = 0.5 / std::sqrt(l2);
        }
        return QVector3D(x,y,z);
}

void RenderingThread::update_rotation(QMouseEvent *ev)
{
        // trackball like rotation
        QVector3D pnew = get_mapped_point(ev->localPos());
        QVector3D pold = get_mapped_point(m_mouse_old_position);
        m_rotation  = QQuaternion::rotationTo(pold, pnew) * m_rotation;
}

bool RenderingThread::mouse_press(QMouseEvent *ev)
{
        qDebug() << "Mouse press ";
        switch (ev->button()) {
        case Qt::LeftButton:{

                qDebug() << "LeftButton";
                switch (ev->type()) {
                case QEvent::MouseButtonPress:
                        qDebug() << "  down";
                        m_mouse1_is_down = true;
                        m_mouse_old_position = ev->localPos();
                        break;
                case QEvent::MouseButtonRelease:
                        qDebug() << "   up";
                        if (!m_mouse1_is_down)
                                return false;
                        m_mouse1_is_down = false;
                        update_rotation(ev);
                        m_mouse_old_position = ev->localPos();
                        break;
                default:
                        return false;
                }
        }
        default:
                return false;
        }
        return true;
}

bool RenderingThread::mouse_tracking(QMouseEvent *ev)
{
        if (!m_mouse1_is_down)
                return false;
        update_rotation(ev);
        m_mouse_old_position = ev->localPos();
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
        m_projection.frustum(-zw, zw, -zh, zh, 500, 600);

    m_viewport = QVector2D(w, h);
}

void RenderingThread::run()
{

}
