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
#include "renderingthread.hh"

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
        m_rendering->set_volume(volume);
        doneCurrent();
}

void MainopenGLView::setLandmarkList(PLandmarkList list)
{
        makeCurrent();
        m_rendering->set_landmark_list(list);
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
        m_rendering->attach_gl();
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
        if (!m_rendering->mouse_release(ev)) {
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

void MainopenGLView::contextMenuEvent ( QContextMenuEvent * event )
{
        qDebug() << "Context menu requested:";
}
