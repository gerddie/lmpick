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

#include "globalscenestate.hh"
#include <cassert>
#include <cmath>

GlobalSceneState::GlobalSceneState():
        camera(QVector3D(0,0,-250), QQuaternion(1, 0, 0, 0), 1.0),
        light_source(-1,-1,-20),
        viewport(0,0)
{
        light_source.normalize();
}

QMatrix4x4 GlobalSceneState::get_modelview_matrix() const
{
        QMatrix4x4 modelview;

        modelview.setToIdentity();
        modelview.translate(camera.get_position());
        modelview.rotate(camera.get_rotation());
        modelview.translate(m_offset);

        return modelview;
}

void GlobalSceneState::set_offset(const QVector3D& ofs)
{
        m_offset = ofs;
}

void GlobalSceneState::delete_offset()
{
        m_offset = QVector3D(0,0,0);
}
