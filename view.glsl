/*
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

#version 120

attribute highp vec3 qt_Vertex;
attribute highp vec3 qt_Color;
attribute highp vec3 qt_Normal;


uniform highp vec3 qt_LightDirection;
uniform highp mat4 qt_mvp;
uniform highp mat3 qt_mv;
varying highp vec4 color;

void main(void)
{
    //vec4 n = vec4(qt_Normal.x, qt_Normal.y, qt_Normal.z, 0);
    vec4 v = vec4(qt_Vertex.x, qt_Vertex.y, qt_Vertex.z, 1);
    gl_Position = qt_mvp * v;
    color = vec4( qt_Color * (- 0.9 * dot(qt_mv * qt_Normal, qt_LightDirection) + 0.1), 1.0);
}
