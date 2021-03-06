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

#include "landmarklistpainter.hh"
#include "landmarklist.hh"
#include "sphere.hh"
#include <cassert>

struct LandmarkListPainterImpl {

        LandmarkListPainterImpl();

        PLandmarkList m_the_list;
        int m_active_index;

        Sphere m_active_sphere;
        Sphere m_normal_sphere;
        QVector3D m_viewspace_scale;
        QVector3D m_viewspace_shift;
        bool m_viewspace_is_startup;

};

LandmarkListPainter::LandmarkListPainter()
{
        impl = new LandmarkListPainterImpl();

}

LandmarkListPainter::~LandmarkListPainter()
{
        delete impl;
}

void LandmarkListPainter::set_viewspace_correction(const QVector3D& scale, const QVector3D& shift)
{
        qDebug() << "Set new scale " << scale << " and shift "  << shift;
        impl->m_viewspace_scale = scale;
        impl->m_viewspace_shift = shift;
        impl->m_viewspace_is_startup = false;
}

void LandmarkListPainter::set_landmark_list(PLandmarkList list)
{
        impl->m_the_list = list;
        impl->m_active_index = -1;

        if (impl->m_viewspace_is_startup) {
                // get landmarks cover area and adjust viewspace so that all fit in a [0,1]^3 cube
                QVector3D min_qube(3000, 3000, 3000);
                QVector3D max_qube(0, 0, 0);
                int n = 0;
                for (auto lpm = impl->m_the_list->begin(); lpm != impl->m_the_list->end(); ++lpm) {
                        if ((*lpm)->has(Landmark::lm_location)) {
                                QVector3D p = (*lpm)->getLocation();
                                if (p.x() < min_qube.x()) min_qube.setX(p.x());
                                if (p.y() < min_qube.y()) min_qube.setY(p.y());
                                if (p.z() < min_qube.z()) min_qube.setZ(p.z());

                                if (p.x() > max_qube.x()) max_qube.setX(p.x());
                                if (p.y() > max_qube.y()) max_qube.setY(p.y());
                                if (p.z() > max_qube.z()) max_qube.setZ(p.z());
                                ++n;
                        }
                }

                if (n > 0) {
                        QVector3D delta = (max_qube - min_qube);
                        impl->m_viewspace_scale = QVector3D(1,1,1)/ delta;
                        impl->m_viewspace_shift = QVector3D(0.5,0.5,0.5);
                }
        }
}

void LandmarkListPainter::do_detach_gl()
{
        impl->m_active_sphere.detach_gl();
        impl->m_normal_sphere.detach_gl();
}

void LandmarkListPainter::do_attach_gl()
{
        impl->m_active_sphere.attach_gl(get_context());
        impl->m_normal_sphere.attach_gl(get_context());
}

void LandmarkListPainter::do_draw(const GlobalSceneState& state)
{
        if (!impl->m_the_list)
                return;

        GlobalSceneState local_state = state;
        for (int i = 0; i < static_cast<int>(impl->m_the_list->size()); ++i) {
                auto lm = (*impl->m_the_list)[i];
                if (lm->has(Landmark::lm_location)) {
                        auto offset = lm->getLocation() * impl->m_viewspace_scale - impl->m_viewspace_shift;
                        local_state.set_offset(offset);
                        if (i == impl->m_active_index) {
                                impl->m_active_sphere.draw(local_state);
                        }else{
                                impl->m_normal_sphere.draw(local_state);
                        }
                }
        }
}

const QString LandmarkListPainter::get_active_landmark_name() const
{
        if (impl->m_active_index < 0 ||
            static_cast<size_t>(impl->m_active_index) >= impl->m_the_list->size())
                return QString();
        return (*impl->m_the_list)[impl->m_active_index]->getName();
}

Landmark& LandmarkListPainter::get_active_landmark()
{
        assert (impl->m_active_index >= 0 &&
                static_cast<size_t>(impl->m_active_index) < impl->m_the_list->size());

        return impl->m_the_list->at(impl->m_active_index);
}

void LandmarkListPainter::set_active_landmark(int idx)
{
        impl->m_active_index = idx;
}

LandmarkListPainterImpl::LandmarkListPainterImpl():
        m_the_list(new LandmarkList),
        m_active_index(-1),
        m_active_sphere(QVector4D(1, 0, 0, 0.9)),
        m_normal_sphere(QVector4D(0, 0.5, 1, 0.8)),
        m_viewspace_scale(1,1,1),
        m_viewspace_shift(0,0,0),
        m_viewspace_is_startup(true)
{
}
