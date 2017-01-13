attribute highp vec3 qt_Vertex;
attribute highp vec3 qt_Color;
attribute highp vec3 qt_Normal;


uniform highp vec3 qt_LightDirection;
uniform highp mat4 qt_mvp;
uniform highp mat4 qt_mv;
varying highp vec3 color;

void main(void)
{
    vec4 n = vec4(qt_Normal.x, qt_Normal.y, qt_Normal.z, 0);
    vec4 v = vec4(qt_Vertex.x, qt_Vertex.y, qt_Vertex.z, 1);
    gl_Position = qt_mvp * v;
    color = qt_Color * (0.5 * dot((qt_mv * n).xyz, qt_LightDirection) + 0.5);
}
