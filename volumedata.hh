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
        typedef std::shared_ptr<VolumeData> Pointer;

        VolumeData(mia::P3DImage data);

        virtual void detach_gl(QOpenGLFunctions& ogl) override;
private:
        virtual void do_draw(const GlobalSceneState& state, QOpenGLFunctions& ogl) const override;
        virtual void do_attach_gl(QOpenGLFunctions& ogl) override;

        mutable QOpenGLBuffer m_arrayBuf;
        mutable QOpenGLBuffer m_indexBuf;

        mutable QOpenGLShaderProgram m_prep_program;
        mutable QOpenGLShaderProgram m_volume_program;
        GLuint m_volume_tex;

        mia::P3DImage m_image;
        QVector3D m_start;
        QVector3D m_end;
        QVector3D m_scale;
        float m_max_coord;
        mutable QOpenGLVertexArrayObject m_vao;

        mutable QOpenGLBuffer m_arrayBuf_2nd_pass;
        mutable QOpenGLBuffer m_indexBuf_2nd_pass;
        mutable QOpenGLVertexArrayObject m_vao_2nd_pass;

        GLint m_voltex_param;
        GLint m_ray_start_param;
        GLint m_ray_end_param;

};

#endif // VOLUMEDATA_HH
