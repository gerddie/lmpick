#version 120
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

        vec3 step = vec3(step_length, step_length, step_length);

        if (abs(dir.z) < step_length) {
                gl_FragColor = vec4(0,0,1,1);
		return; 
	}

        vec3 x = start.xyz;

        for (int a = 0; a < 1024; ++a, x += step) {
                vec4 color = texture3D(volume, x);
                if (color.a >= iso_value) {
                        // evaluate gradient and draw
                        gl_FragColor = vec4(1,1,0,1);
                        return;
                }
        }
        gl_FragColor = vec4(1,0,0,1);

}
