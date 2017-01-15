attribute highp vec4 qt_Vertex;
uniform highp mat4 qt_mvp;
uniform highp mat4 qt_mv;
varying highp vec4 color;

void main(void)
{
    gl_Position = qt_mvp * qt_Vertex;
    color = 0.5 * (qt_Vertex + vec4(1,1,1,1));
}
