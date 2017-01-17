#version 130
uniform sampler3D volume;
uniform sampler2D ray_start;
uniform sampler2D ray_end;

uniform vec3 step_length;
uniform float iso_value;
uniform vec3 light_source;
uniform mat4 qt_mv;

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

                vec3 nf = adir  / step_length.x;
                float max_nf = max(max(nf.x, nf.y), nf.z);
                int n = int(max_nf);
                vec3 step = dir / n;

                vec3 r = vec3(0,0,0);

                for (int a = 0; a < n; ++a)  {
                        vec3 x = start.xyz + a * step;
                        vec4 color = texture3D(volume, x);
                        if (color.r > iso_value) {
                                float cx = (texture3D(volume, vec3(x.x + step_length.x, x.y, x.z)).r -
                                            texture3D(volume, vec3(x.x - step_length.x, x.y, x.z)).r)/ step_length.x;
                                float cy = (texture3D(volume, vec3(x.x, x.y + step_length.y, x.z)).r -
                                            texture3D(volume, vec3(x.x, x.y - step_length.y, x.z)).r)/ step_length.y;
                                float cz = (texture3D(volume, vec3(x.x, x.y, x.z + step_length.z)).r -
                                            texture3D(volume, vec3(x.x, x.y, x.z - step_length.z)).r)/ step_length.z;
                                vec3 normal = normalize(vec3(cx, cy, cz));

                                vec4 n = qt_mv * vec4(normal.x, normal.y, normal.z, 1);
                                r = n.xyz;
                                break;
                        }
                }
                result = vec4(r.r, r.g, r.b, 1);

        }

        gl_FragColor = result;

}
