#version 120
attribute highp vec4 qt_Vertex;
attribute highp vec4 qt_Texture;
uniform highp mat4 qt_mvp;
uniform highp mat4 qt_mv;
varying highp vec4 color;

void main(void)
{
    gl_Position = qt_mvp * qt_Vertex;
    color = qt_Texture;
}
