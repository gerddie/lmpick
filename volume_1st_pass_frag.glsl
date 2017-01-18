#version 120
varying highp vec4 color;

void main(void)
{
    gl_FragColor = vec4(color.rgb, 1.0/gl_FragCoord.z);
}
