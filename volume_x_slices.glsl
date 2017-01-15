
attribute highp vec4 vertex;
attribute highp vec3 tex_in;

uniform vec3 GradientDelta;
uniform highp float vdx;
uniform highp float tdx;
uniform highp mat4 qt_mvp;

varying highp vec3 qt_TexCoord0;
varying highp vec3 qt_TexCoord_dx;
varying highp vec3 qt_TexCoord_dy;
varying highp vec3 qt_TexCoord_dz;

void main(void)
{
    vec4 v = vec4(vertex.x + vdx, vertex.y, vertex.z, 1);
    gl_Position = qt_mvp * v;

    qt_TexCoord0 = tex_in;
    qt_TexCoord0.x += tdx;

    qt_TexCoord_dx = vec3(qt_TexCoord0.r + GradientDelta.r, qt_TexCoord0.g, qt_TexCoord0.b);
    qt_TexCoord_dy = vec3(qt_TexCoord0.r, qt_TexCoord0.g  + GradientDelta.g, qt_TexCoord0.b);
    qt_TexCoord_dz = vec3(qt_TexCoord0.r, qt_TexCoord0.g, qt_TexCoord0.b + GradientDelta.b);
}
