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

attribute highp vec3 qt_vertex;
attribute highp vec3 qt_normal;


uniform highp vec3 qt_light_direction;
uniform highp mat4 qt_view_matrix;
uniform highp mat3 qt_normal_matrix;
uniform highp vec4 qt_base_color;

varying highp vec4 color;

void main(void)
{
        vec4 v = vec4(qt_vertex.x, qt_vertex.y, qt_vertex.z, 1);
        gl_Position = qt_view_matrix * v;
        float light_intensity = dot(qt_normal_matrix * qt_normal, qt_light_direction);
        color = qt_base_color *  (0.9 * light_intensity + 0.1);
}
