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

   iso_value:   the texture intensity value that is used to extract the iso-surface
0
   light_source: the light direction vector used for shading. This direction must be
                 corrected for the view direction.

Outputs:
    if the cast ray doesn't hit a voxel that has an intensity value larger than the
    iso value, then the fragment is discarded, otherwise the following values are
    written:

    gl_FragData[0]: a 4D vector with light intensity in the color componets rgb, and
                     the z value in the w component.

    gl_FragData[1]: xyz = 3D texture coordinate where the ray stopped, and w=1,
                    if ray hit something.

*/

/** \todo:
     * pass in the current z-buffer information to discard pixels that would overdraw?
       (Since this is textureSizean intermediate step, it could also be done in the final blit step)
     * add a base color as uniform value to color the output
       (This could also be done in the final blit step)
*/

#version 140
uniform sampler3D volume;
uniform sampler2D ray_start;
uniform sampler2D ray_end;

uniform highp float iso_value;
uniform highp vec3 light_source;
uniform highp mat4 qt_mv;

varying highp vec2 tex2dcoord;

const float zNear = 548.0;
const float zFar = 552.0;

// from http://stackoverflow.com/questions/6652253/getting-the-true-z-value-from-the-depth-buffer

float linearDepth(float depthSample)
{
    depthSample = 2.0 * depthSample - 1.0;
    highp float zLinear = 2.0 * zNear * zFar / (zFar + zNear - depthSample * (zFar - zNear));
    return zLinear;
}

// result suitable for assigning to gl_FragDepth
float depthSample(float linearDepth)
{
    highp float nonLinearDepth = (zFar + zNear - 2.0 * zNear * zFar / linearDepth) / (zFar - zNear);
    nonLinearDepth = (nonLinearDepth + 1.0) / 2.0;
    return nonLinearDepth;
}

void main(void)
{
        // obtain start and end position of the ray
        highp vec4 start = texture2D(ray_start, tex2dcoord);
        highp vec4 end = texture2D(ray_end, tex2dcoord);

        // early exit if the z-value is not set
        if (start.w == 1.0 && end.w == 1.0) {
                discard;
        }

        // obtain drawing direction
        highp vec3 dir = (end - start).xyz;
        highp vec3 adir = abs(dir);

        // evaluate the step_length based on the texture size
        // each step should move into another voxel
        ivec3 ts = textureSize(volume, 0);
        vec3 step_length = vec3(1.0/ts.x, 1.0/ts.y, 1.0/ts.z);

        // if the length of all drawing line components is smaller
        // as the corresponding step length then discard the fragment
        if (adir.x < step_length.x && adir.y < step_length.y && adir.z < step_length.z) {
                discard;
        }

        // calculate the actually used step length
        highp vec3 nf = adir  / step_length;
        highp float max_nf =max(max(nf.x, nf.y), nf.z);
        highp vec3 step = dir / max_nf;

        // iterate along the ray, front to back
        bool hit = false;
        highp float old_iso = -1;

        for (highp float a = 0; a < max_nf ; a += 1.0)  {
                highp vec3 x = start.xyz + a * step;
                highp vec4 color = texture3D(volume, x);

                // if we cross the iso-boundary draw the pixel
                if (color.r < iso_value) {
                        old_iso = color.r;
                        continue;
                } else {
                        highp float f = a - 1 + (iso_value - old_iso) / (color.r - old_iso);

                        x = start.xyz +  f * step;

                        // evalute the normal by using centered finite differences
                        highp float gx = (texture3D(volume, vec3(x.x - step_length.x, x.y, x.z)).r -
                                    texture3D(volume, vec3(x.x + step_length.x, x.y, x.z)).r)/ step_length.x / 2.0;

                        highp float gy = (texture3D(volume, vec3(x.x, x.y - step_length.y, x.z)).r -
                                    texture3D(volume, vec3(x.x, x.y + step_length.y, x.z)).r)/ step_length.y / 2.0;

                        highp float gz = (texture3D(volume, vec3(x.x, x.y, x.z - step_length.z)).r -
                                    texture3D(volume, vec3(x.x, x.y, x.z + step_length.z)).r)/ step_length.z / 2.0;

                        highp vec3 normal = normalize(vec3(gx, gy, gz));

                        // evaaluate the light inetensity
                        highp float li = -dot(normal, light_source);

                        // evaluate the output z position (note that these are stored as
                        // inverses of the actual values).
                        highp float start_depth = linearDepth(start.w);
                        highp float end_depth = linearDepth(end.w);
                        highp float fragment_depth = start_depth + f * (end_depth - start_depth) / max_nf;

                        highp float depth = depthSample(fragment_depth);

                        // todo: add a base color here
                        // Store depth in the alpha component off the output color.
                        gl_FragData[0] = vec4(li, li, li, depth);

                        // output texture coordinate to second render target
                        // if attached, set alpha to one. This can later be use to check
                        // whether we have a valid value stored here.
                        gl_FragData[1] = vec4(x.xyz, 1);

                        // exit the loop and indicate that a pixel was drawn
                        hit = true;
                        break;
                }
        }
        //if  not hit the iso-value, then discard the fragment
        if (!hit)
                discard;
}
