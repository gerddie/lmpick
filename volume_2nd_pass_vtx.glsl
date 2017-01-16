#version 330
attribute vec2 qt_Vertex;
varying vec2 tex2dcoord;

void main(void)
{
    gl_Position = vec4(2 * qt_Vertex.x - 1, 2 * qt_Vertex.y - 1, 0, 1);
    tex2dcoord = qt_Vertex;
}
