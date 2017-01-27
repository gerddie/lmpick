#include "renderingthread.hh"

#include <QMouseEvent>

using std::make_shared;
RenderingThread::RenderingThread(QWidget *parent):
        m_parent(parent),
        m_is_gl_attached(false),
        m_context(nullptr),
        m_mouse1_is_down(false),
        m_landmark_tm(nullptr)
{

}

RenderingThread::~RenderingThread()
{
}

void RenderingThread::attach_gl()
{
        initializeOpenGLFunctions();
        m_context =  QOpenGLContext::currentContext();
        m_is_gl_attached = true;

        qDebug() << "OpenGL: " << (char*)glGetString(GL_VERSION);

        if (m_volume)
                m_volume->attach_gl(*m_context);

        m_lmp.attach_gl(*m_context);

}

void RenderingThread::set_volume(VolumeData::Pointer volume)
{
        std::swap(m_volume, volume);
        if (m_is_gl_attached) {
                if (volume)
                        volume->detach_gl(*m_context);

                if (m_volume) {
                        m_volume->attach_gl(*m_context);
                }
        }
        if (m_volume) {
                m_lmp.set_viewspace_correction(m_volume->get_viewspace_scale(),
                                               m_volume->get_viewspace_shift());
        }
}

void RenderingThread::set_selected_landmark(int idx)
{
        m_lmp.set_active_landmark(idx);
        auto& lm = m_lmp.get_active_landmark();

        m_state.camera = lm.get_camera();
        update_projection();
        if (m_volume)
                m_volume->set_iso_value(lm.get_iso_value());
}


void RenderingThread::set_landmark_model(LandmarkTableModel *ltm)
{
        m_landmark_tm = ltm;
}

void RenderingThread::set_landmark_list(PLandmarkList list)
{
        assert(m_landmark_tm);
        m_lmp.set_landmark_list(list);
        m_landmark_tm->setLandmarkList(list);
}

void RenderingThread::set_active_landmark_details(const QPoint& loc)
{
        auto& lm = m_lmp.get_active_landmark();

        auto location = m_volume->get_surface_coordinate(loc);
        if (location.first) {
                float iso = m_volume->get_iso_value();
                Camera c = m_state.camera;
                lm.set(location.second, iso, c);
        }else{
                qDebug() << "RenderingThread::acquire_landmark_details: "
                         <<"no landmark coordinates available, because hit empty space.";
        }
}

void RenderingThread::set_volume_iso_value(int value)
{
        if (m_volume)
                m_volume->set_iso_value(value);
}

void RenderingThread::paint()
{
        glClearColor(0.1,0.1,0.1,1);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);
        glDepthMask(GL_TRUE);

        if (m_volume)
                m_volume->draw(m_state, *m_context);

        m_lmp.draw(m_state, *m_context);
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
        m_state.camera.rotate(QQuaternion::rotationTo(pold, pnew));
}

bool RenderingThread::mouse_release(QMouseEvent *ev)
{
        switch (ev->button()) {
        case Qt::LeftButton:{
                if (!m_mouse1_is_down)
                        return false;
                m_mouse1_is_down = false;
                update_rotation(ev);
                m_mouse_old_position = ev->localPos();
                break;
        }
        default:
                return false;
        }
        return true;
}

bool RenderingThread::mouse_press(QMouseEvent *ev)
{
        switch (ev->button()) {
        case Qt::LeftButton:{
                m_mouse1_is_down = true;
                m_mouse_old_position = ev->localPos();
                break;
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

        // Todo: make the boundaries and the change factor
        // a configurable option
        if (delta.y() < 0) {
                m_state.camera.zoom_out();
        }else if (delta.y() > 0) {
                m_state.camera.zoom_in();
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
        m_state.viewport = QSize(w,h);
}

void RenderingThread::update_projection()
{
        m_state.projection.setToIdentity();
        float zw, zh;
        if (m_viewport.x() > m_viewport.y()) {
                zw = m_state.camera.get_zoom() * m_viewport.x() / m_viewport.y();
                zh = m_state.camera.get_zoom();
        }else{
                zh = m_state.camera.get_zoom()* m_viewport.y() / m_viewport.x();
                zw = m_state.camera.get_zoom();
        }
        m_state.projection.frustum(-zw, zw, -zh, zh, 240, 260);
}

void RenderingThread::detach_gl()
{
        m_is_gl_attached = false;
        qDebug() << "Detach";
        if (m_volume)
                m_volume->detach_gl(*m_context);

        m_lmp.detach_gl(*m_context);
}

const QString RenderingThread::get_active_landmark_name() const
{
        return m_lmp.get_active_landmark_name();
}

bool RenderingThread::add_landmark(const QString& name, const QPoint& mouse_loc)
{
        assert(m_landmark_tm);
        auto lml = m_landmark_tm->getLandmarkList();

        if (lml->has(name))
                return false;

        auto location = m_volume->get_surface_coordinate(mouse_loc);
        if (location.first) {
                float iso = m_volume->get_iso_value();
                Camera c = m_state.camera;
                PLandmark lm = make_shared<Landmark>(name, location.second, iso, c);
                m_landmark_tm->addLandmark(lm);
                qDebug() << "Add landmark at " << location.second;
        }else{
                PLandmark lm = make_shared<Landmark>(name);
                m_landmark_tm->addLandmark(lm);

                qDebug() << "RenderingThread::acquire_landmark_details: "
                         <<"no landmark coordinates available, because hit empty space.";

        }
        return true;
}

void RenderingThread::run()
{

}
