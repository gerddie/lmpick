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

/*
  This shader implements the volume renderer 2nd pass vertex shader
  It scales vertex to the viewport and passen it original on as
  texture corordinate to be used to get the start and end coordinates
  for the ray to be cast.
*/

#version 120
attribute vec2 qt_Vertex;
varying vec2 tex2dcoord;

void main(void)
{
    gl_Position = vec4(2 * qt_Vertex.x - 1, 2 * qt_Vertex.y - 1, 0, 1);
    tex2dcoord = qt_Vertex;
}
