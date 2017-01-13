varying highp vec3 color;

void main(void)
{
    gl_FragColor = vec4(color.x, color.y, color.z, 1.0);
}
