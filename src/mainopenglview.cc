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
#include <QMenu>
#include <QInputDialog>
#include <cassert>
#include <memory>
#include <cmath>

using mia::C3DFVector;

MainopenGLView::MainopenGLView(QWidget *parent):
        QOpenGLWidget(parent),
        m_rendering(nullptr)
{
        m_rendering = new RenderingThread(this);
        setMouseTracking( true );

        m_add_landmark_action = new QAction(tr("Add new landmark here"), this);
        m_set_landmark_action = new QAction(tr("Set landmark location"), this);

        connect(m_add_landmark_action, SIGNAL(triggered()), this, SLOT(on_add_landmark()));
        connect(m_set_landmark_action, SIGNAL(triggered()), this, SLOT(on_set_landmark()));

}

void MainopenGLView::setVolume(VolumeData::Pointer volume)
{
        makeCurrent();
        m_rendering->set_volume(volume);
        doneCurrent();
}

void MainopenGLView::setLandmarkModel(LandmarkTableModel *model)
{
        m_rendering->set_landmark_model(model);
}

void MainopenGLView::selected_landmark_changed(int row)
{
        m_rendering->set_selected_landmark(row);
        emit isovalue_changed();
        update();
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
        qDebug() << "MainopenGLView::detachGL()";
        makeCurrent();
        m_rendering->detach_gl();
}

void MainopenGLView::initializeGL()
{
        qDebug() << "MainopenGLView::initializeGL()";
        m_rendering->attach_gl();
        connect(QOpenGLContext::currentContext(), &QOpenGLContext::aboutToBeDestroyed,
                this, &MainopenGLView::detachGL);
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

void MainopenGLView::contextMenuEvent ( QContextMenuEvent * event)
{
        QMenu context(tr("Landmarks"), this);

        QString active_landmark = m_rendering->get_active_landmark_name();
        if (!active_landmark.isEmpty()) {
                m_set_landmark_action->setText(tr("Set location of landmark '") + active_landmark + "'");
                m_set_landmark_action->setData(QVariant(event->pos()));
                context.addAction(m_set_landmark_action);
        }

        m_add_landmark_action->setData(QVariant(event->pos()));
        context.addAction(m_add_landmark_action);
        context.exec(event->globalPos());
}

void MainopenGLView::on_set_landmark()
{
        QVariant data = m_add_landmark_action->data();
        m_rendering->set_active_landmark_details(data.toPoint());

        emit availabledata_changed();
        update();
}

void MainopenGLView::on_add_landmark()
{
        QString prompt(tr("Name:"));

        while (true) {
                bool ok_pressed = false;
                QString name = QInputDialog::getText(this, tr("Add new landmark"),
                                                     prompt, QLineEdit::Normal,
                                                     QString(), &ok_pressed);
                if (!ok_pressed) {
                        qDebug() << "Canceled input";
                        break;
                }

                if (!name.isEmpty()) {
                        qDebug() << "Will add landmark ..." << name;
                        QVariant data = m_add_landmark_action->data();
                        qDebug() << "... from " << data.toPoint();
                        if (m_rendering->add_landmark(name, data.toPoint())){
                                emit availabledata_changed();
                                update();
                                break;
                        }else{
                                prompt =QString(tr("Name (") + name + tr(" is already in list):"));
                        }
                }else{
                        prompt =QString(tr("Name (must not be empty):"));
                }
        }
}

void MainopenGLView::snapshot(const QString& filename)
{
        update();
        QImage img  = grabFramebuffer();
        img.save(filename);
}
