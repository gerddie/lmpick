#ifndef VOLUMEDATA_HH
#define VOLUMEDATA_HH

#include "drawable.hh"
#include <mia/3d/image.hh>
#include <QOpenGLTexture>
#include <QOpenGLBuffer>
#include <QOpenGLShaderProgram>
#include <QOpenGLTexture>

class VolumeData : public Drawable
{
public:
        VolumeData(mia::P3DImage data);

        virtual void attach_gl() override;
        virtual void detach_gl() override;
private:
        virtual void do_draw(const GlobalSceneState& state, QOpenGLFunctions& ogl) const override;

        mutable QOpenGLBuffer m_arrayBuf;
        mutable QOpenGLBuffer m_indexBuf;

        mutable QOpenGLShaderProgram m_program;
        mutable QOpenGLTexture m_volume_texture;

        mia::P3DImage m_image;
        QVector3D m_start;
        QVector3D m_end;
        QVector3D m_scale;
};

#endif // VOLUMEDATA_HH
