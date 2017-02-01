#version 330

uniform sampler2D image;
varying vec2 tex2dcoord;

void main(void)
{
        vec4 color = texture2D(image, tex2dcoord);

        if (color.w > 0.0) {
                gl_FragColor.rgb = color.rgb * (1 - color.w);
                gl_FragColor.a = 1.0;
                gl_FragDepth = color.w;
        }else
                discard;
}
