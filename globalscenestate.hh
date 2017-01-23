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

#ifndef GLOBALSCENESTATE_HH
#define GLOBALSCENESTATE_HH

#include "camera.hh"
#include <QVector3D>
#include <QMatrix4x4>
#include <QQuaternion>
#include <QSize>



class GlobalSceneState
{
public:
        GlobalSceneState();

        QMatrix4x4 get_modelview_matrix() const;

        Camera camera;

        QVector4D light_source;
        QMatrix4x4 projection;
        QSize viewport;

};

#endif // GLOBALSCENESTATE_HH
