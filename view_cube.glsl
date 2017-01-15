attribute highp vec4 qt_Vertex;
attribute highp vec4 qt_Normal;
attribute highp vec4 qt_Color;
attribute highp vec3 ld;

uniform highp mat4 qt_mvp;
uniform highp mat4 qt_mv;
varying highp vec4 color;

void main(void)
{
    gl_Position = qt_mvp * qt_Vertex;
    vec4 n = qt_mv * qt_Normal;

    color = qt_Color * dot(ld * n.xyz);

}
