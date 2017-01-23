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

#include "mainopenglview.hh"
#include "landmarklistpainter.hh"
#include "globalscenestate.hh"
#include "octaeder.hh"
#include "sphere.hh"
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

using mia::C3DFVector;
class RenderingThread : public QObject, private QOpenGLFunctions {
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

        void setVolume(VolumeData::Pointer volume);

        void set_volume_iso_value(int value);

        void detach_gl();
private:
        void update_rotation(QMouseEvent *ev);
        QVector3D get_mapped_point(const QPointF& localPos) const;
        void update_projection();

        QWidget *m_parent;
        bool m_is_gl_attached;
        QOpenGLContext *m_context;

        GlobalSceneState m_state;


        // the shader (must be moved into the actual geometries
        QOpenGLShaderProgram m_view_program;


        // used for mouse tracking
	bool m_mouse1_is_down;
	QPointF m_mouse_old_position; 


	QVector2D m_viewport; 

        VolumeData::Pointer m_volume;
        Octaeder::Pointer m_octaeder;
        LandmarkListPainter m_lmp;
};

MainopenGLView::MainopenGLView(QWidget *parent):
        QOpenGLWidget(parent),
        m_rendering(nullptr)
{
        QSurfaceFormat format;
        format.setDepthBufferSize(24);
        format.setVersion(3, 3);
        format.setRenderableType(QSurfaceFormat::OpenGL);
        format.setProfile(QSurfaceFormat::CoreProfile);
        setFormat(format);
        m_rendering = new RenderingThread(this);
        setMouseTracking( true );
}

void MainopenGLView::setVolume(VolumeData::Pointer volume)
{
        makeCurrent();
        m_rendering->setVolume(volume);
        doneCurrent();
}

void MainopenGLView::set_volume_isovalue(int value)
{
        m_rendering->set_volume_iso_value(value);
        update();
}

MainopenGLView::~MainopenGLView()
{
        delete m_rendering;
}

void MainopenGLView::detachGL()
{
        m_rendering->detach_gl();
}

void MainopenGLView::initializeGL()
{
        m_rendering->initialize();
        connect(QOpenGLContext::currentContext(), SIGNAL(aboutToBeDestroyed()),
                this, SLOT(detachGL()));
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
        if (m_rendering->mouse_tracking(ev)) {
                update();
        }else{
                // handle mouse here
        }

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
        m_is_gl_attached(false),
        m_context(nullptr),

        m_mouse1_is_down(false)
	
{
#if 1
        auto img = new mia::C3DFImage(mia::C3DBounds(128,256,128));

        auto i = img->begin();
        for (unsigned int z = 0; z < 128; ++z) {
                float fz = sin (z * M_PI / 127);
                for (unsigned int y = 0; y < 256; ++y) {
                        float fy = sin (y * M_PI / 255) * fz;
                        for (unsigned int x = 0; x < 128; ++x, ++i)  {
                                *i = fy * sin (x * M_PI / 127);
                        }
                }
        }
        img->set_voxel_size(C3DFVector(2.2, 1.1, 2.0));
        VolumeData::Pointer v(new VolumeData(mia::P3DImage(img)));
        setVolume(v);
        m_octaeder.reset(new Octaeder);
#endif

}

RenderingThread::~RenderingThread()
{
}

void RenderingThread::initialize()
{
        initializeOpenGLFunctions();
        m_context =  QOpenGLContext::currentContext();
        m_is_gl_attached = true;

        qDebug() << "OpenGL: " << (char*)glGetString(GL_VERSION);

        if (m_volume)
                m_volume->attach_gl(*m_context);

        if (m_octaeder)
                m_octaeder->attach_gl(*m_context);

        m_lmp.attach_gl(*m_context);

}

void RenderingThread::setVolume(VolumeData::Pointer volume)
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

        if ( m_octaeder )
                m_octaeder->draw(m_state, *m_context);

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

bool RenderingThread::mouse_press(QMouseEvent *ev)
{
        switch (ev->button()) {
        case Qt::LeftButton:{

                switch (ev->type()) {
                case QEvent::MouseButtonPress:
                        m_mouse1_is_down = true;
                        m_mouse_old_position = ev->localPos();
                        break;
                case QEvent::MouseButtonRelease:
                        if (!m_mouse1_is_down)
                                return false;
                        m_mouse1_is_down = false;
                        update_rotation(ev);
                        m_mouse_old_position = ev->localPos();
                        break;
                default:
                        return false;
                }
                break;
        }
        case Qt::RightButton:{
                switch (ev->type()) {
                case QEvent::MouseButtonPress:
                        if (m_volume) {
                                qDebug() << "Left mouse at:" << ev->pos();
                                qDebug() << "  translates to: "<<
                                            m_volume->get_surface_coordinate(ev->pos());
                        }
                        break;
                default:
                                return false;
                }
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
        m_state.projection.frustum(-zw, zw, -zh, zh, 548, 552);
}

void RenderingThread::detach_gl()
{
        m_is_gl_attached = false;
        qDebug() << "Detach";
        if (m_volume)
                m_volume->detach_gl(*m_context);

        if(m_octaeder)
                m_octaeder->detach_gl(*m_context);

}

void RenderingThread::run()
{

}
