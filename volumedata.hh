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
        ~VolumeData();

        virtual void detach_gl(QOpenGLContext& context) override;
private:
        virtual void do_draw(const GlobalSceneState& state, QOpenGLContext& context) const override;
        virtual void do_attach_gl(QOpenGLContext& context) override;

        struct VolumeDataImpl *impl;
};

#endif // VOLUMEDATA_HH
