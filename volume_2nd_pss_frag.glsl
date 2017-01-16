#version 330
uniform sampler3D volume;
uniform sampler2D ray_start;
uniform sampler2D ray_end;

varying vec2 tex2dcoord;

void main(void)
{
        // first just draw the start - end
        vec4 start = texture2D(ray_start, tex2dcoord);
        vec4 end = texture2D(ray_end, tex2dcoord);

        gl_FragColor = start +end;
}
