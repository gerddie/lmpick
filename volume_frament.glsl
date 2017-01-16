#version 330
uniform sampler3D VolumeTexture;
uniform highp vec4 light_source;
uniform vec3 GradientScale;

varying highp vec3 qt_TexCoord0;
varying highp vec3 qt_TexCoord_dx;
varying highp vec3 qt_TexCoord_dy;
varying highp vec3 qt_TexCoord_dz;


void main(void)
{



        vec4 alpha = texture3D(VolumeTexture, qt_TexCoord0);
        float nx = GradientScale.x * (texture3D(VolumeTexture, qt_TexCoord_dx).a - alpha);
        float ny = GradientScale.y * (texture3D(VolumeTexture, qt_TexCoord_dy).a - alpha);
        float nz = GradientScale.z * (texture3D(VolumeTexture, qt_TexCoord_dz).a - alpha);
        vec3 normal = normalize(vec3(nx, ny, nz));
        float light_intensity =  max(dot(light_source, normal), 0.0);
        gl_FragColor = vec4(light_intensity, light_intensity, light_intensity, alpha);
}

