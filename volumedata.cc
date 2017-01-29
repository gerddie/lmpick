/* -*- mia-c++  -*-
 *
 * This file is part of qtlmpick- a tool for landmark picking and
 * visualization in volume data
 * Copyright (c) Genoa 2017,  Gert Wollny
 *
 * qtlmpick is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with MIA; if not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "volumedata.hh"
#include <mia/core/filter.hh>
#include <mia/3d/imageio.hh>
#include <QOpenGLFramebufferObject>
#include <QOpenGLExtraFunctions>
#include <QOpenGLFramebufferObjectFormat>
#include <QOpenGLTexture>
#include <QOpenGLShaderProgram>
#include <QMatrix3x3>
#include <QPainter>
#include <cassert>

using mia::C3DFImage;
using mia::accumulate;

using std::unique_ptr;
using std::transform;
using std::vector;
using std::make_pair;

struct VolumeDataImpl {

        VolumeDataImpl(mia::P3DImage data);
        ~VolumeDataImpl();

        void detach_gl(QOpenGLContext& context);
        void do_draw(const GlobalSceneState& state, QOpenGLContext& context);
        void do_attach_gl(QOpenGLContext& context);

        unique_ptr<C3DFImage> m_image;

        float m_iso_value;
        float m_min;
        float m_max;
        float m_intenisity_scale;
        float m_intenisity_shift;

        QOpenGLBuffer m_arrayBuf;
        QOpenGLBuffer m_indexBuf;

        QOpenGLShaderProgram m_prep_program;
        QOpenGLShaderProgram m_volume_program;
        QOpenGLShaderProgram m_blit_program;

        QOpenGLTexture m_volume_tex;

        QVector3D m_scale;
        float m_max_coord;
        QOpenGLVertexArrayObject m_vao;

        QOpenGLBuffer m_arrayBuf_2nd_pass;
        QOpenGLBuffer m_indexBuf_2nd_pass;
        QOpenGLVertexArrayObject m_vao_2nd_pass;

        GLint m_voltex_param;
        GLint m_ray_start_param;
        GLint m_ray_end_param;
        QVector3D m_gradient_delta;
        GLint m_iso_value_param;
        GLint m_volume_blit_texture_param;

        int m_width;
        int m_height;
        vector<QVector4D> m_tex_coordinates;
        QVector3D m_physical_size;
};

/* convert the input image to a float valued picture that
 * optimally uses the intensity range [0,1]; */
struct GetFloat01Picture: public mia::TFilter<mia::C3DFImage *> {

        GetFloat01Picture(float& minv, float& maxv, float& scale, float& shift):
                m_min_val(minv),
                m_max_val(maxv),
                m_scale(scale),
                m_shift(shift)
        {
        }

        template <typename T>
        mia::C3DFImage *operator() (const mia::T3DImage<T>& input) {
                //should test that there are more than one
                auto mm = std::minmax_element(input.begin(), input.end());
                m_min_val =  *mm.first;
                m_max_val =  *mm.second;
                if (*mm.second != *mm.first) {
                        m_scale = 1.0f /(*mm.second - *mm.first);
                }else{
                        m_scale = 1.0f;
                }
                m_shift = *mm.first;

                C3DFImage *result = new C3DFImage(input.get_size(), input);
                std::transform(input.begin(), input.end(), result->begin(),
                               [this](T x){return (x - m_shift) * m_scale;});
                return result;
        }
private:
        float& m_min_val;
        float& m_max_val;
        float& m_scale;
        float& m_shift;
};

VolumeDataImpl::VolumeDataImpl(mia::P3DImage data):
        m_iso_value(0.7),
        m_arrayBuf(QOpenGLBuffer::VertexBuffer),
        m_indexBuf(QOpenGLBuffer::IndexBuffer),
        m_volume_tex(QOpenGLTexture::Target3D),
        m_arrayBuf_2nd_pass(QOpenGLBuffer::VertexBuffer),
        m_indexBuf_2nd_pass(QOpenGLBuffer::IndexBuffer),
        m_voltex_param(-1),
        m_ray_start_param(-1),
        m_ray_end_param(-1),
        m_volume_blit_texture_param(-1)
{

        GetFloat01Picture scaler(m_min, m_max, m_intenisity_scale, m_intenisity_shift);
        m_image.reset(accumulate(scaler,*data));

        // we want at least 255 steps
        if (m_max - m_min < 255){
                m_max = 255 + m_min;
                m_intenisity_scale = 1.0 / 255;
        }

        auto s = m_image->get_size();
        auto v = m_image->get_voxel_size();

        m_physical_size = QVector3D(s.x * v.x, s.y * v.y, s.z * v.z);
        m_max_coord = m_physical_size.x();
        if (m_max_coord < m_physical_size.y())
                m_max_coord = m_physical_size.y();
        if (m_max_coord < m_physical_size.z())
                m_max_coord = m_physical_size.z();


        m_gradient_delta = QVector3D(1,1,1)/m_physical_size;
        m_scale = m_physical_size / m_max_coord;
}



void compile_and_link(QOpenGLShaderProgram& program, const QString& vtx_prog, const QString& frag_pgrm)
{
        if (!program.addShaderFromSourceFile(QOpenGLShader::Vertex, vtx_prog))
                qWarning() << "Error compiling '" << vtx_prog << "' view will be clobbered\n";

        if (!program.addShaderFromSourceFile(QOpenGLShader::Fragment, frag_pgrm))
                qWarning() << "Error compiling '" << frag_pgrm <<  "', view will be clobbered\n";

        if (!program.link())
                qWarning() << "Error linking (" <<vtx_prog << "," << frag_pgrm << ")', view will be clobbered\n";;
}

VolumeDataImpl::~VolumeDataImpl()
{

}

VolumeData::VolumeData(mia::P3DImage data)
{
        assert(data);
        impl = new VolumeDataImpl(data);
}

VolumeData::~VolumeData()
{
        delete impl;
}

float VolumeData::get_iso_value() const
{
        return impl->m_iso_value / impl->m_intenisity_scale + impl->m_intenisity_shift;
}

void VolumeData::set_iso_value(float iso)
{
        impl->m_iso_value = impl->m_intenisity_scale * (iso - impl->m_intenisity_shift);
}

QVector3D VolumeData::get_viewspace_shift() const
{
        return QVector3D(1,1,1) * impl->m_scale;
}

std::pair<bool, QVector3D> VolumeData::get_surface_coordinate(const QPoint& location) const
{
        qDebug() << "location:" << location << " in(" << impl->m_width << ":" << impl->m_height <<")";
        QVector3D result(-1, -1, -1);
        bool found = false;
        if (location.x() < impl->m_width && location.y() < impl->m_height) {
                QVector4D t = impl->m_tex_coordinates[impl->m_width * (impl->m_height - location.y() - 1) + location.x()];
                qDebug() << "Tex=" << t;
                if (t.w() > 0) {
                        result =  QVector3D(t.x(), t.y(), t.z()) * impl->m_physical_size;
                        found = true;
                }
        }
        return make_pair(found, result);
}

QVector3D VolumeData::get_viewspace_scale() const
{
        return QVector3D(2,2,2) / impl->m_physical_size * impl->m_scale;
}

std::pair<int, int> VolumeData::get_intensity_range() const
{
        int range_min = static_cast<int>(impl->m_min);
        int range_max = static_cast<int>(impl->m_max);
        return make_pair(range_min, range_max);
}

void VolumeData::detach_gl(QOpenGLContext& context)
{
        impl->detach_gl(context);
}

void VolumeData::do_draw(const GlobalSceneState& state, QOpenGLContext& context) const
{
        impl->do_draw(state, context);
}
void VolumeData::do_attach_gl(QOpenGLContext& context)
{
        impl->do_attach_gl(context);
}

struct PrepVertexData {
        QVector3D v;
        QVector3D t;
};

// cube definition for the 3D space (1st pass)

static const PrepVertexData plain_cube_vertices[] = {
        {{-1, -1, -1}, {0,0,0}},
        {{ 1, -1, -1}, {1,0,0}},
        {{ 1,  1, -1}, {1,1,0}},
        {{-1,  1, -1}, {0,1,0}},
        {{ -1, -1, 1}, {0,0,1}},
        {{  1, -1, 1}, {1,0,1}},
        {{  1,  1, 1}, {1,1,1}},
        {{ -1,  1, 1}, {0,1,1}}
};

static const unsigned short plain_cube_indices[] = {
        0, 2, 1,    3, 2, 0,
        1, 2, 5,    5, 2, 6,
        0, 5, 4,    5, 0, 1,
        3, 0, 4,    4, 7, 3,
        3, 7, 6,    6, 2, 3,
        4, 5, 6,    7, 4, 6
};

// rectangle screenspace definition (2nd pass)

static const QVector2D screenspace_quad[] {
   {0,0}, {0,1}, {1,1}, {1,0}
};

static const unsigned short screenspace_fan_idx[] {
   0, 1, 2, 3
};


#define OGL_ERRORTEST(text) { \
        int error_nr = glGetError(); \
        if (error_nr)  qWarning() << "VolumeData:" << text <<":"<< error_nr; \
}

void VolumeDataImpl::do_attach_gl(QOpenGLContext& context)
{
        OGL_ERRORTEST("VolumeData::do_attach_gl: Incoming error:");

        auto ogl = context.functions();
        m_vao.create();

        // obtain properly intensity scaled picture
        const C3DFImage& img = *m_image;

        // create the texture
        ogl->glActiveTexture(GL_TEXTURE0);
        m_volume_tex.setFormat(QOpenGLTexture::R32F);
        m_volume_tex.setMinMagFilters(QOpenGLTexture::Nearest, QOpenGLTexture::Linear);
        m_volume_tex.setSize(m_image->get_size().x, m_image->get_size().y, m_image->get_size().z);
        m_volume_tex.allocateStorage();
        OGL_ERRORTEST("m_volume_tex.allocateStorage()");

        m_volume_tex.setData(0, 0, QOpenGLTexture::Red, QOpenGLTexture::Float32, &img[0]);
        OGL_ERRORTEST("m_volume_tex.setData");

        // set the interpolation mode
        ogl->glTexParameterf (GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        ogl->glTexParameterf (GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        m_arrayBuf.create();
        m_indexBuf.create();
        m_vao.bind();

        // Initializes cube geometry and transfers it to VBOs
        m_arrayBuf.bind();
        m_arrayBuf.setUsagePattern(QOpenGLBuffer::StaticDraw);

        std::vector<PrepVertexData> vertices(8);
        transform(plain_cube_vertices, &plain_cube_vertices[8], vertices.begin(),
                        [this](const PrepVertexData& v){
                             PrepVertexData r = v;
                             r.v *= m_scale;
                             return r;}
        );


        m_arrayBuf.allocate(&vertices[0], 8 * sizeof(PrepVertexData));
        OGL_ERRORTEST("m_arrayBuf.allocate ");

        // Transfer index data to VBO 1
        m_indexBuf.bind();
        m_indexBuf.allocate(plain_cube_indices, sizeof(plain_cube_indices));

        compile_and_link(m_prep_program, ":/shaders/shaders/volume_1st_pass_vtx.glsl", ":/shaders/shaders/volume_1st_pass_frag.glsl");
        compile_and_link(m_volume_program, ":/shaders/shaders/volume_2nd_pass_vtx.glsl", ":/shaders/shaders/volume_2nd_pass_frag.glsl");
        compile_and_link(m_blit_program, ":/shaders/shaders/volume_2nd_pass_vtx.glsl", ":/shaders/shaders/volume_blit_frag.glsl");

        m_voltex_param = m_volume_program.uniformLocation("volume");
        if (m_voltex_param == -1)
                qWarning() << "Can't find volume parameter";

        m_ray_start_param = m_volume_program.uniformLocation("ray_start");
        if (m_ray_start_param == -1)
                qWarning() << "Can't find ray_start parameter";

        m_ray_end_param = m_volume_program.uniformLocation("ray_end");
        if (m_ray_end_param == -1)
                qWarning() << "Can't find ray_end parameter";

        int vertexLocation = m_prep_program.attributeLocation("qt_Vertex");
        if (vertexLocation == -1)
                qWarning() << "vertex loction attribute not found";
        m_prep_program.enableAttributeArray(vertexLocation);
        m_prep_program.setAttributeBuffer(vertexLocation, GL_FLOAT, 0, 3, sizeof(PrepVertexData));

        int texLocation = m_prep_program.attributeLocation("qt_Texture");
        if (texLocation == -1)
                qWarning() << "tex loction attribute not found";
        m_prep_program.enableAttributeArray(texLocation);
        m_prep_program.setAttributeBuffer(texLocation, GL_FLOAT, sizeof(QVector3D), 3, sizeof(PrepVertexData));



        m_arrayBuf.release();
        m_indexBuf.release();
        m_vao.release();

        m_vao_2nd_pass.create();
        m_vao_2nd_pass.bind();

        m_arrayBuf_2nd_pass.create();
        m_arrayBuf_2nd_pass.bind();
        m_arrayBuf_2nd_pass.setUsagePattern(QOpenGLBuffer::StaticDraw);
        m_arrayBuf_2nd_pass.allocate(screenspace_quad, sizeof(screenspace_quad));

        m_indexBuf_2nd_pass.create();
        m_indexBuf_2nd_pass.bind();
        m_indexBuf_2nd_pass.allocate(screenspace_fan_idx, sizeof(screenspace_fan_idx));

        m_volume_program.bind();
        auto spacing_param = m_volume_program.uniformLocation("step_length");
        m_volume_program.setUniformValue(spacing_param, m_gradient_delta);

        m_volume_blit_texture_param = m_blit_program.uniformLocation("image");

        m_iso_value_param = m_volume_program.uniformLocation("iso_value");

        auto vertex_location = m_volume_program.attributeLocation("qt_Vertex");
        if (vertex_location >= 0) {
                m_volume_program.enableAttributeArray(vertex_location);
                m_volume_program.setAttributeBuffer(vertex_location, GL_FLOAT, 0, 2);
        } else {
                qWarning() << "qt_Vertex not found, rendering will fail";
        }
        m_vao_2nd_pass.release();
}

void VolumeDataImpl::detach_gl(QOpenGLContext& context)
{
        m_volume_tex.destroy();
        m_arrayBuf.destroy();
        m_indexBuf.destroy();
        m_prep_program.release();
}


// todo: Consider pre-allocating the FBOs in the attach_gl() function
void VolumeDataImpl::do_draw(const GlobalSceneState& state, QOpenGLContext& context)
{
        auto modelview = state.get_modelview_matrix();
        auto& ogl = *context.functions();

        m_width = state.viewport.width();
        m_height = state.viewport.height();

        m_tex_coordinates.resize(m_width * m_height);

        // first pass: draw cube to fbo's to obtain ray texture start and end

        ogl.glClearColor(0,0,0,1);
        ogl.glEnable(GL_DEPTH_TEST);
        ogl.glEnable(GL_CULL_FACE);
        ogl.glCullFace(GL_BACK);

        m_vao.bind();
        m_prep_program.bind();
        m_arrayBuf.bind();
        m_indexBuf.bind();

        m_prep_program.setUniformValue("qt_mvp", state.projection * modelview);
        m_prep_program.setUniformValue("qt_mv", modelview);

        QOpenGLFramebufferObjectFormat fbformat;
        fbformat.setTextureTarget(GL_TEXTURE_2D);
        fbformat.setInternalTextureFormat(GL_RGBA32F);
        QOpenGLFramebufferObject fbo_ray_start(state.viewport, fbformat);
        QOpenGLFramebufferObject fbo_ray_end(state.viewport, fbformat);


        fbo_ray_start.bind();
        ogl.glClearColor(0,0,0,0);
        ogl.glClear(GL_COLOR_BUFFER_BIT);
        ogl.glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, 0);
        fbo_ray_start.release();

        glCullFace(GL_FRONT);
        fbo_ray_end.bind();
        ogl.glClearColor(0,0,0,0);
        ogl.glClear(GL_COLOR_BUFFER_BIT);
        ogl.glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, 0);
        fbo_ray_end.release();

        m_arrayBuf.release();
        m_indexBuf.release();
        m_vao.release();
        m_prep_program.release();

        // Second pass, render to another separate surface
        //
        QOpenGLFramebufferObject fbo_volume(state.viewport, fbformat);
        fbo_volume.bind();

        glDepthFunc(GL_ALWAYS);
        ogl.glDisable(GL_CULL_FACE);

        if (!m_volume_program.bind())
            qWarning() << "Unable to bind m_volume_program\n";

        // enable the volume texture
        ogl.glActiveTexture(GL_TEXTURE0);
        m_volume_tex.bind();
        m_volume_program.setUniformValue(m_voltex_param, 0);

        // enable the ray endpoint textures
        ogl.glActiveTexture(GL_TEXTURE0 + 1);
        ogl.glBindTexture(GL_TEXTURE_2D, fbo_ray_start.texture());
        m_volume_program.setUniformValue(m_ray_start_param, 1);


        ogl.glActiveTexture(GL_TEXTURE0 + 2);
        ogl.glBindTexture(GL_TEXTURE_2D, fbo_ray_end.texture());
        m_volume_program.setUniformValue(m_ray_end_param, 2);

        // set iso-value; todo: use changable param
        m_volume_program.setUniformValue(m_iso_value_param, m_iso_value);

        // set corrected light source
        auto light_source_param = m_volume_program.uniformLocation("light_source");
        auto inv_normal = modelview.transposed();

        // this is really kind of stupid, but the 3x3 Maatrix doesn't implement multiplication with
        // a vector
        QVector4D l(state.light_source.x(), state.light_source.y(), state.light_source.z(), 0.0);
        QVector4D ls = inv_normal * l;
        m_volume_program.setUniformValue(light_source_param, ls.toVector3D());

        // bind buffers and draw
        m_vao_2nd_pass.bind();
        m_arrayBuf_2nd_pass.bind();
        m_indexBuf_2nd_pass.bind();


        // consider putting this all into its own FBO class
        auto glex = context.extraFunctions();

        // get FBO
        GLuint space_coord_rb;

        // attach a new renderbuffer for writing the texture coordinates
        glex->glGenRenderbuffers(1, &space_coord_rb);
        glex->glBindRenderbuffer(GL_RENDERBUFFER, space_coord_rb);
        glex->glRenderbufferStorage(GL_RENDERBUFFER, GL_RGBA, state.viewport.width(), state.viewport.height());
        glex->glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1,
                                        GL_RENDERBUFFER, space_coord_rb);


        // Set both buffers to write and clear
        GLenum buffers[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
        glex->glDrawBuffers(2, buffers);


        glClear(GL_COLOR_BUFFER_BIT);

        // render the volume data
        ogl.glDrawElements(GL_TRIANGLE_FAN, 4, GL_UNSIGNED_SHORT, 0);


        // grab the texture coordinates to have them for landmark picking
        glex->glReadBuffer(GL_COLOR_ATTACHMENT1);

        // finish rendering before reading back
        // this is no high-speed game, we can wait for glReadPixles
        ogl.glFinish();
        ogl.glReadPixels(0,0,state.viewport.width(), state.viewport.height(), GL_RGBA, GL_FLOAT, &m_tex_coordinates[0]);

        // detach and release the render buffer
        // should not be needed, since the fbo is destroyed when leaving the function ...
        glex->glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1,
                                        GL_RENDERBUFFER, 0);
        glex->glDeleteRenderbuffers(1, &space_coord_rb);

        m_volume_program.release();
        fbo_volume.release();

        ogl.glActiveTexture(GL_TEXTURE1);
        ogl.glBindTexture(GL_TEXTURE_2D, 0);
        ogl.glActiveTexture(GL_TEXTURE2);
        ogl.glBindTexture(GL_TEXTURE_2D, 0);


        // now blit it to the output surface (normally the screen)
        ogl.glActiveTexture(GL_TEXTURE0);
        ogl.glBindTexture(GL_TEXTURE_2D, fbo_volume.texture());

        m_blit_program.bind();
        m_blit_program.setUniformValue(m_volume_blit_texture_param, 0);

        ogl.glDrawElements(GL_TRIANGLE_FAN, 4, GL_UNSIGNED_SHORT, 0);

        m_vao_2nd_pass.release();
        m_indexBuf_2nd_pass.release();
        m_arrayBuf_2nd_pass.release();
}
