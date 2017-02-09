/* -*- mia-c++  -*-
 *
 * This file is part of qtlmpick- a tool for landmark picking and
 * visualization in volume data
 * Copyright (c) Genoa 2017,  Gert Wollny
 *
 * qtlmpick is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with MIA; if not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "renderingthread.hh"

#include <QMouseEvent>

using std::make_shared;
RenderingThread::RenderingThread(QWidget *parent):
        m_parent(parent),
        m_is_gl_attached(false),
        m_context(nullptr),
        m_mouse_lb_is_down(false),
        m_mouse_mb_is_down(false),
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
                m_volume->attach_gl(m_context);

        m_lmp.attach_gl(m_context);

}

void RenderingThread::set_volume(VolumeData::Pointer volume)
{
        std::swap(m_volume, volume);
        if (m_is_gl_attached) {
                if (volume)
                        volume->detach_gl();

                if (m_volume) {
                        m_volume->attach_gl(m_context);
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

        if (lm.has(Landmark::lm_camera)) {
                m_state.camera = lm.getCamera();
                update_projection();
        }
        if (m_volume && lm.has(Landmark::lm_iso_value))
                m_volume->set_iso_value(lm.getIsoValue());
}


void RenderingThread::set_landmark_model(LandmarkTableModel *ltm)
{
        m_landmark_tm = ltm;
}

void RenderingThread::set_landmark_list(PLandmarkList list)
{
        assert(m_landmark_tm);
        m_lmp.set_landmark_list(list);
        m_current_landmarks = list;
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
                m_current_landmarks->setDirtyFlag(true);
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
                m_volume->draw(m_state);

        m_lmp.draw(m_state);
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

void RenderingThread::update_shift(QMouseEvent *ev)
{
        QPointF delta = (ev->localPos() - m_mouse_old_position) * 0.004;
        auto cpos = m_state.camera.get_position();
        cpos.setX(cpos.x() + delta.x());
        cpos.setY(cpos.y() - delta.y());
        m_state.camera.set_position(cpos);
}

bool RenderingThread::mouse_release(QMouseEvent *ev)
{
        switch (ev->button()) {
        case Qt::LeftButton:{
                if (!m_mouse_lb_is_down)
                        return false;
                m_mouse_lb_is_down = false;
                update_rotation(ev);
                m_mouse_old_position = ev->localPos();
                break;
        }
        case Qt::MiddleButton: {
                m_mouse_mb_is_down = false;
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
                m_mouse_lb_is_down = true;
                m_mouse_old_position = ev->localPos();
                break;
        }
        case Qt::MiddleButton: {
                m_mouse_mb_is_down = true;
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
        if (!m_mouse_lb_is_down && !m_mouse_mb_is_down)
                return false;

        if (m_mouse_lb_is_down)
                update_rotation(ev);

        else if (m_mouse_mb_is_down) {
                update_shift(ev);
        }
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
        m_state.projection.frustum(-zw, zw, -zh, zh, 248, 252);
}

void RenderingThread::detach_gl()
{
        m_is_gl_attached = false;
        qDebug() << "Detach";
        if (m_volume)
                m_volume->detach_gl();

        m_lmp.detach_gl();
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
