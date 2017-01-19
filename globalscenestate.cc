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

GlobalSceneState::GlobalSceneState():
        camera_location(0,0,-550),
        rotation_center(0, 0, 0),
        light_source(-1,-1,-20, 0),
        rotation(1,0,0,0),
        zoom(1.0),
        viewport(0,0)
{
        light_source.normalize();
}

QMatrix4x4 GlobalSceneState::get_modelview_matrix() const
{
        QMatrix4x4 modelview;

        modelview.setToIdentity();
        modelview.translate(camera_location);
        modelview.rotate(rotation);
        modelview.translate(rotation_center);

        return modelview;
}

