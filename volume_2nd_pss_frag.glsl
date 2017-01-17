#version 130
uniform sampler3D volume;
uniform sampler2D ray_start;
uniform sampler2D ray_end;

uniform vec3 step_length;
uniform float iso_value;

varying vec2 tex2dcoord;

void main(void)
{
        // first just draw the start - end
        vec4 start = texture2D(ray_start, tex2dcoord);
        vec4 end = texture2D(ray_end, tex2dcoord);

        vec3 dir = (end - start).xyz;
        vec3 adir = abs(dir);

        vec4 result = vec4(0,0,0,1);


        if (adir.x >= step_length.x || adir.y >= step_length.y || adir.z >= step_length.z) {

                vec3 nf = (adir + step_length) / step_length;

                normalize(dir);
                vec3 step = min(min(step_length.x, step_length.y), step_length.z) * dir;

                vec3 r = vec3(1,0,0);
                vec3 x = start.xyz;
                while  (x.x >= 0 && x.x <= 1.0 && x.y >= 0 && x.y <= 1.0 && x.z >= 0 && x.z <= 1.0)  {
                        vec4 color = texture3D(volume, x);
                        if (color.r > iso_value) {
                                float cx = (texture3D(volume, vec3(x.x + step_length.x, x.y, x.z)).r - color.r)/ step_length.x;
                                float cy = (texture3D(volume, vec3(x.x, x.y + step_length.y, x.z)).r - color.r)/ step_length.y;
                                float cz = (texture3D(volume, vec3(x.x, x.y, x.z + step_length.z)).r - color.r)/ step_length.z;
                                r = normalize(vec3(cx + 1, cy + 1, cz + 1));
                                break;
                        }
                        x = x + 2* step;
                }
                result = vec4(r.r, r.g, r.b, 1);

        }

        gl_FragColor = result;

}
