#include "mainopenglview.hh"
#include "globalscenestate.hh"
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
        ~RenderingThread();

        void initialize();

        void run();

        void paint();

        void resize(int w, int h);

        bool mouse_press(QMouseEvent *ev);

        bool mouse_tracking(QMouseEvent *ev);

        bool mouse_wheel(QWheelEvent *ev);
private:
        void update_rotation(QMouseEvent *ev);
        QVector3D get_mapped_point(const QPointF& localPos) const;
        void update_projection();

        QWidget *m_parent;
        QOpenGLContext *m_context;

        GlobalSceneState m_state;

        // some OpenGL stuff globally required
        QOpenGLVertexArrayObject m_vao;

        // the shader (must be moved into the actual geometries
        QOpenGLShaderProgram m_view_program;


        // used for mouse tracking
	bool m_mouse1_is_down;
	QPointF m_mouse_old_position; 


	QVector2D m_viewport; 

        std::vector<Drawable::Pointer> m_objects;
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

void MainopenGLView::wheelEvent(QWheelEvent *ev)
{
        if (m_rendering->mouse_wheel(ev))
                update();
        else
                ev->ignore();
}

RenderingThread::RenderingThread(QWidget *parent):
        m_parent(parent),
        m_context(nullptr),

        m_mouse1_is_down(false)
	
{
        m_objects.push_back(Drawable::Pointer(new Octaeder));
}

RenderingThread::~RenderingThread()
{
        for (auto d: m_objects)
                d->detach_gl();
}

void RenderingThread::initialize()
{
        initializeOpenGLFunctions();
        m_context =  QOpenGLContext::currentContext();

        glClearColor(0.1,0.1,0.1,1);

        glEnable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE);

        m_vao.create();
        m_vao.bind();

        for (auto d: m_objects)
                d->attach_gl();
}

void RenderingThread::paint()
{
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        for (auto d: m_objects)
                d->draw(m_state, *this);
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
        m_state.rotation  = QQuaternion::rotationTo(pold, pnew) * m_state.rotation;
}

bool RenderingThread::mouse_press(QMouseEvent *ev)
{
        qDebug() << "Mouse press " << ev->button();
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

bool RenderingThread::mouse_wheel(QWheelEvent *ev)
{
        auto delta = ev->angleDelta();

        if (delta.y() < 0) {
                m_state.zoom *= -1.05 * delta.y() / 120.0;
        }else if (delta.y() > 0) {
                m_state.zoom *= 0.9 * delta.y() / 120.0;
        }else
                return false;

        update_projection();
        return true;
}

void RenderingThread::resize(int w, int h)
{
        glViewport(0,0,w,h);
        m_viewport = QVector2D(w, h);
        update_projection();
}

void RenderingThread::update_projection()
{
        m_state.projection.setToIdentity();
        float zw, zh;
        if (m_viewport.x() > m_viewport.y()) {
                zw = m_state.zoom * m_viewport.x() / m_viewport.y();
                zh = m_state.zoom;
        }else{
                zh = m_state.zoom * m_viewport.y() / m_viewport.x();
                zw = m_state.zoom;
        }
        m_state.projection.frustum(-zw, zw, -zh, zh, 500, 600);
}

void RenderingThread::run()
{

}
