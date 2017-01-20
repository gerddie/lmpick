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
  This shader implements volume iso surface rendering by using ray casting.
  It does no depth testing but it writes the depth information, hence it should
  always be drawn first.

  The inputs are:

   volume: the 3D texture used as input for the volume rendering

   ray_start:  the 2D texture that contains the ray start texture coordinates
               and the ray start depth information.

   ray_end:    the 2D texture that contains the ray end texture coordinates
               and the ray end depth information.

   step_length: a 3D vector contaning the step length in 3D that makes sure that
                each step passes over at most one pixel

   iso_value:   the texture intensity value that is used to extract the iso-surface

   light_source: the light direction vector used for shading

   qt_mv:       the model-view matrix to correct the normals

*/

/** \todo:
     * pass in the current z-buffer information to discard pixels that would overdraw
     * add a base color as uniform value to color the output
*/

#version 120
uniform sampler3D volume;
uniform sampler2D ray_start;
uniform sampler2D ray_end;

uniform vec3 step_length;
uniform float iso_value;
uniform highp vec4 light_source;
uniform mat4 qt_mv;

varying vec2 tex2dcoord;


void main(void)
{
        // obtain start and end position of the ray
        vec4 start = texture2D(ray_start, tex2dcoord);
        vec4 end = texture2D(ray_end, tex2dcoord);

        // early exit if the z-value is inf
        if (start.w == 0.0 && end.w == 0.0) {
                discard;
        }

        // obtain drawing direction
        vec3 dir = (end - start).xyz;
        vec3 adir = abs(dir);

        // if the length of all drawing line components is smaller
        // as the corresponding step length then discard the fragment
        if (adir.x < step_length.x && adir.y < step_length.y && adir.z < step_length.z) {
                discard;
        }

        // calculate the actually used step length
        vec3 nf = adir  / step_length;
        float max_nf = max(max(nf.x, nf.y), nf.z);
        int n = int(max_nf);
        vec3 step = dir / n;

        // iterate along the ray, front to back
        bool hit = false;
        float old_iso = -1;

        for (int a = 0; a < n ; ++a)  {
                vec3 x = start.xyz + a * step;
                vec4 color = texture3D(volume, x);

                // if we cross the iso-boundary draw the pixel
                if (color.r > iso_value) {

                        // optimize the actual iso-value crossing coodinate
                        float rel = (iso_value - old_iso) / (color.r - old_iso);
                        float f = a - 1 + rel;
                        x = start.xyz +  f * step;

                        // evalute the normal by using centered finite differences
                        float cx = (texture3D(volume, vec3(x.x - step_length.x, x.y, x.z)).r -
                                    texture3D(volume, vec3(x.x + step_length.x, x.y, x.z)).r)/ step_length.x;
                        float cy = (texture3D(volume, vec3(x.x, x.y - step_length.y, x.z)).r -
                                    texture3D(volume, vec3(x.x, x.y + step_length.y, x.z)).r)/ step_length.y;
                        float cz = (texture3D(volume, vec3(x.x, x.y, x.z - step_length.z)).r -
                                    texture3D(volume, vec3(x.x, x.y, x.z + step_length.z)).r)/ step_length.z;
                        vec3 normal = normalize(vec3(cx, cy, cz));

                        // evaaluate the light inetensity
                        float li = -dot(normal, light_source.xyz);

                        // evaluate the output z position (note that these are stored as
                        // inverses of the actual values).
                        float depth =  1.0 / (1.0/start.w + f * (1.0/end.w - 1.0/start.w)  / n);

                        // todo: add a base color here
                        // Store depth in the alpha component off the output color.
                        gl_FragData[0] = vec4(li,li,li, depth);

                        // output texture coordinate to second render target
                        // if attached, set alpha to one. This can later be use to check
                        // whether we have a valid value stored here.
                        gl_FragData[1] = vec4(x.xyz, 1);

                        //
                        //gl_FragDepth =

                        // exit the loop and indicate that a pixel was drawn
                        hit = true;
                        break;
                }else {
                        // remember intensity value for later interpolation
                        old_iso = color.r;
                }
        }
        // if not it the iso-value, then discard the fragment
        if (!hit)
                discard;
}
