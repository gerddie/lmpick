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

#ifndef MAINOPENGLVIEW_HH
#define MAINOPENGLVIEW_HH

#include "volumedata.hh"
#include "landmarklist.hh"
#include <QOpenGLWidget>
#include <QAction>

class RenderingThread;

class MainopenGLView : public QOpenGLWidget
{
        Q_OBJECT
public:
        MainopenGLView(QWidget *parent);

        ~MainopenGLView();

        void setVolume(PVolumeData volume);
        void setLandmarkList(PLandmarkList list);

private slots:
        void detachGL();

        void set_volume_isovalue(int value);

        void on_set_landmark();
        void on_add_landmark();
private:
        void initializeGL()override;
        void paintGL()override;
        void resizeGL(int w, int h)override;
        void mouseMoveEvent(QMouseEvent *ev) override;
        void mousePressEvent(QMouseEvent *ev) override;
        void mouseReleaseEvent(QMouseEvent *ev) override;
        void wheelEvent(QWheelEvent *ev) override;
	
        void contextMenuEvent ( QContextMenuEvent * event );

        RenderingThread *m_rendering;
        QAction *m_add_landmark_action;
        QAction *m_set_landmark_action;

};

#endif // MAINOPENGLVIEW_HH
