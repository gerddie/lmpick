#version 130
uniform sampler3D volume;
uniform sampler2D ray_start;
uniform sampler2D ray_end;

uniform float step_length;
uniform float iso_value;

varying vec2 tex2dcoord;

void main(void)
{
        // first just draw the start - end
        vec4 start = texture2D(ray_start, tex2dcoord);
        vec4 end = texture2D(ray_end, tex2dcoord);

        vec3 dir = (end - start).xyz;
        float l = length(dir);

        vec4 result = vec4(1,0,0,1);


        result = vec4(1,1,0,1);

        if (l >= step_length) {
                vec3 test = 0.5 * (start + end).xyz;
                result = texture3D(volume, test);
                normalize(dir);
                vec3 step = step_length * dir;
                int n = int((l + step_length) / step_length);

                vec3 x = start.xyz;
                for (int a = 0; a < n; ++a) {
                        vec4 color = texture3D(volume, start.xyz + a * dir);
                        if( color.r > iso_value) {
                                // evaluate gradient and draw
                                result = color;
                                break;
                        }

                }

        }

        gl_FragColor = result;

}
