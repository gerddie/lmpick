#version 120
varying highp vec4 color;

void main(void)
{
    gl_FragColor = vec4(color.rgb, gl_FragCoord.z);
}
