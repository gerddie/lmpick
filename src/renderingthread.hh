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

#ifndef RENDERINGTHREAD_HH
#define RENDERINGTHREAD_HH

#include "volumedata.hh"
#include "landmarklistpainter.hh"
#include "landmarktablemodel.hh"

#include "octaeder.hh"

#include <QImage>
#include <QObject>
#include <QOpenGLFunctions>


class QMouseEvent;
class QWheelEvent;

class RenderingThread : public QObject, private QOpenGLFunctions {
public:
        RenderingThread(QWidget *widget);

        ~RenderingThread();

        void attach_gl();

        void detach_gl();

        void run();

        void paint();

        void resize(int w, int h);

        bool mouse_release(QMouseEvent *ev);

        bool mouse_press(QMouseEvent *ev);

        bool mouse_tracking(QMouseEvent *ev);

        bool mouse_wheel(QWheelEvent *ev);

        void set_volume(PVolumeData volume);

        void set_landmark_list(PLandmarkList list);

        void set_landmark_model(LandmarkTableModel *ltm);

        void set_volume_iso_value(int value);

        void set_active_landmark_details(const QPoint& loc);

        const QString get_active_landmark_name() const;

        bool add_landmark(const QString& name, const QPoint& mouse_loc);

        void set_selected_landmark(int idx);

private:
        void update_rotation(QMouseEvent *ev);

        void update_shift(QMouseEvent *ev);

        QVector3D get_mapped_point(const QPointF& localPos) const;

        void update_projection();


        QWidget *m_parent;

        // OpenGL state and view context
        bool m_is_gl_attached;
        QOpenGLContext *m_context;
        GlobalSceneState m_state;

        // used for mouse tracking
        bool m_mouse_lb_is_down;
        bool m_mouse_mb_is_down;
        QPointF m_mouse_old_position;
        QVector2D m_viewport;

        // Data to display
        VolumeData::Pointer m_volume;
        LandmarkTableModel *m_landmark_tm;

        PLandmarkList m_current_landmarks;
        LandmarkListPainter m_lmp;

        bool m_snapshot_pending;
        QImage m_last_snapshot;
};

#endif // RENDERINGTHREAD_HH
