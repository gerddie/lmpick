#include "volumedata.hh"
#include <mia/3d/filter.hh>
#include <mia/3d/imageio.hh>
#include <QOpenGLFramebufferObject>
#include <QOpenGLFramebufferObjectFormat>
#include <QPainter>
#include <cassert>

VolumeData::VolumeData(mia::P3DImage data):
        m_arrayBuf(QOpenGLBuffer::VertexBuffer),
        m_indexBuf(QOpenGLBuffer::IndexBuffer),
        m_volume_tex(0),
        m_image(data),
        m_arrayBuf_2nd_pass(QOpenGLBuffer::VertexBuffer),
        m_indexBuf_2nd_pass(QOpenGLBuffer::IndexBuffer),
        m_voltex_param(-1),
        m_ray_start_param(-1),
        m_ray_end_param(-1)
{
        assert(m_image);

        auto s = m_image->get_size();
        auto v = m_image->get_voxel_size();

        QVector3D size(s.x * v.x, s.y * v.y, s.z * v.z);
        m_max_coord = size.x();
        if (m_max_coord < size.y())
                m_max_coord = size.y();
        if (m_max_coord < size.z())
                m_max_coord = size.z();

        m_scale = size / m_max_coord;
        m_end = 0.5 * m_scale;
        m_start = -m_end;

}

struct VertexData
{
    GLfloat px, py, pz;
    GLfloat nx, ny, nz;
    GLfloat cr, cg, cb;
};

struct PrepVertexData {
        QVector3D v;
        QVector3D t;
};

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

static const QVector2D screenspace_quad[] {
   {0,0}, {0,1}, {1,1}, {1,0}
};

static const unsigned short screenspace_fan_idx[] {
   0, 1, 2, 3
};

static const unsigned short plain_cube_indices[] = {
        0, 2, 1,
        3, 2, 0,

        1, 2, 5,
        5, 2, 6,

        0, 5, 4,
        5, 0, 1,

        3, 0, 4,
        4, 7, 3,

        3, 7, 6,
        6, 2, 3,

        4, 5, 6,
        7, 4, 6

};


static const VertexData cube_vertices[] = {
        // down
        {-1, -1, -1, 0, 0, -1, 1, 0, 0},
        { 1, -1, -1, 0, 0, -1, 1, 0, 0},
        { 1,  1, -1, 0, 0, -1, 1, 0, 0},
        {-1,  1, -1, 0, 0, -1, 1, 0, 0},

        //left
        {-1, -1, -1, -1, 0, 0, 1, 0, 1},
        {-1,  1, -1, -1, 0, 0, 1, 0, 1},
        {-1,  1,  1, -1, 0, 0, 1, 0, 1},
        {-1, -1,  1, -1, 0, 0, 1, 0, 1},

        //right
        { 1, -1, -1,  1, 0, 0, 0, 1, 1},
        { 1,  1, -1,  1, 0, 0, 0, 1, 1},
        { 1,  1,  1,  1, 0, 0, 0, 1, 1},
        { 1, -1,  1,  1, 0, 0, 0, 1, 1},

        //front
        {-1, -1, -1, 0, -1, 0, 1, 1, 1},
        { 1, -1, -1, 0, -1, 0, 1, 1, 1},
        { 1, -1,  1, 0, -1, 0, 1, 1, 1},
        {-1, -1,  1, 0, -1, 0, 1, 1, 1},

        //back
        {-1,  1, -1, 0,  1, 0, 0, 0, 1},
        { 1,  1, -1, 0,  1, 0, 0, 0, 1},
        { 1,  1,  1, 0,  1, 0, 0, 0, 1},
        {-1,  1,  1, 0,  1, 0, 0, 0, 1},

        // up
        { -1, -1, 1, 0, 0,  1, 1, 1, 0},
        {  1, -1, 1, 0, 0,  1, 1, 1, 0},
        {  1,  1, 1, 0, 0,  1, 1, 1, 0},
        { -1,  1, 1, 0, 0,  1, 1, 1, 0}
};

static const unsigned short cube_indices[] = {
        0, 2, 1,
        3, 2, 0,

        4, 6, 5,
        7, 6, 4,

        9, 10, 8,
       10, 11, 8,

        12, 13, 14,
        15, 12, 14,

        16, 18, 17,
        19, 18, 16,

        20, 21, 22,
        23, 20, 22

};


void VolumeData::do_attach_gl(QOpenGLFunctions& ogl)
{

        int error_nr;

        error_nr = glGetError(); if (error_nr)  qWarning() << "Incomming " << error_nr;

        m_vao.create();

        ogl.glActiveTexture(GL_TEXTURE0);
        ogl.glGenTextures(1, &m_volume_tex);


        auto converter = mia::produce_3dimage_filter("convert:repn=ushort");
        auto pimage = converter->filter(m_image);
        const mia::C3DUSImage& img = dynamic_cast<const mia::C3DUSImage&>(*pimage);
        mia::save_image("test_volume.v",pimage);


        ogl.glBindTexture(GL_TEXTURE_3D, m_volume_tex);
        glTexImage3D(GL_TEXTURE_3D, 0, GL_R16UI,
                      m_image->get_size().x, m_image->get_size().y, m_image->get_size().z,
                      0, GL_RED_INTEGER,
                       GL_UNSIGNED_SHORT, &img[0]);


        error_nr = glGetError(); if (error_nr)  qWarning() << "glTexImage3D " << error_nr;

        assert(m_arrayBuf.create());
        assert(m_indexBuf.create());

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
        error_nr = ogl.glGetError(); if (error_nr)  qWarning() << "m_arrayBuf.allocate " << error_nr;

        // Transfer index data to VBO 1
        m_indexBuf.bind();
        m_indexBuf.allocate(plain_cube_indices, sizeof(plain_cube_indices));

        if (!m_prep_program.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/view_cube.glsl"))
                qWarning() << "Error compiling ':/shaders/view_cube.glsl', view will be clobbered\n";

        if (!m_prep_program.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/basic_frag.glsl"))
                qWarning() << "Error compiling ':/s makeCurrent();haders/fshader.glsl', view will be clobbered\n";

        if (!m_prep_program.link())
                qWarning() << "Error linking m_view_program', view will be clobbered\n";;

        if (!m_volume_program.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/volume_2nd_pass_vtx.glsl"))
                qWarning() << "Error compiling ':/shaders/view_cube.glsl', view will be clobbered\n";

        if (!m_volume_program.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/volume_2nd_pss_frag.glsl"))
                qWarning() << "Error compiling ':/s makeCurrent();haders/fshader.glsl', view will be clobbered\n";

        if (!m_volume_program.link())
                qWarning() << "Error linking m_view_program', view will be clobbered\n";;


        m_voltex_param = m_volume_program.uniformLocation("volume");
        if (m_voltex_param == -1)
                qWarning() << "Can't find volume parameter";

        m_ray_start_param = m_volume_program.uniformLocation("ray_start");
        if (m_ray_start_param == -1)
                qWarning() << "Can't find ray_start parameter";

        m_ray_end_param = m_volume_program.uniformLocation("ray_end");
        if (m_ray_end_param == -1)
                qWarning() << "Can't find ray_end parameter";

        // Tell OpenGL programmable pipeline how to locate vertex position data
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

        error_nr = glGetError();
        if (error_nr)
                qWarning() << "Some error Error -1 " << error_nr;

        m_arrayBuf.release();
        m_indexBuf.release();
        m_vao.release();

        error_nr = glGetError();
        if (error_nr)
                qWarning() << "Some error Error " << error_nr; // gl_FragColor = vec4(1,1,0,1);

        m_vao_2nd_pass.create();
        m_vao_2nd_pass.bind();

        m_arrayBuf_2nd_pass.create();
        m_arrayBuf_2nd_pass.bind();
        m_arrayBuf_2nd_pass.setUsagePattern(QOpenGLBuffer::StaticDraw);
        m_arrayBuf_2nd_pass.allocate(screenspace_quad, sizeof(screenspace_quad));

        m_indexBuf_2nd_pass.create();
        m_indexBuf_2nd_pass.bind();
        m_indexBuf_2nd_pass.allocate(screenspace_fan_idx, sizeof(screenspace_fan_idx));

        if (error_nr)
                qWarning() << "Some error Error (2)" << error_nr;

        m_volume_program.bind();
        auto spacing_param = m_volume_program.uniformLocation("step_length");
        assert(spacing_param != -1);
        float step_length = 1.0f / m_max_coord;
        qDebug() << "step_length= " << step_length;

        m_volume_program.setUniformValue(spacing_param, step_length);

        auto iso_value_param = m_volume_program.uniformLocation("iso_value");
        assert(iso_value_param != -1);
        m_volume_program.setUniformValue(iso_value_param, 0.5f);

        auto vertex_location = m_volume_program.attributeLocation("qt_Vertex");
        if (vertex_location >= 0) {
                m_volume_program.enableAttributeArray(vertex_location);
                error_nr = glGetError();
                if (error_nr)
                        qWarning() << "m_volume_program.enableAttributeArray(vertex_location): Error " << error_nr;
                m_volume_program.setAttributeBuffer(vertex_location, GL_FLOAT, 0, 2);
                error_nr = glGetError();
                if (error_nr)
                        qWarning() << "m_volume_program.setAttributeBuffer(vertex_location): Error " << error_nr;
        } else {
                qWarning() << "qt_Vertex not found, rendering will fail";
        }

        m_vao_2nd_pass.release();
}

void VolumeData::detach_gl(QOpenGLFunctions& ogl)
{
        ogl.glDeleteTextures(1, &m_volume_tex);
        m_arrayBuf.destroy();
        m_indexBuf.destroy();
        m_prep_program.release();
}


void VolumeData::do_draw(const GlobalSceneState& state, QOpenGLFunctions& ogl) const
{
        int  error_nr;

        auto modelview = state.get_modelview_matrix();

        static int index= 0;

        ogl.glClearColor(0,0,0,1);
        ogl.glEnable(GL_DEPTH);
        ogl.glEnable(GL_CULL_FACE);
        ogl.glCullFace(GL_BACK);

        m_vao.bind();
        m_prep_program.bind();
        m_arrayBuf.bind();
        m_indexBuf.bind();

        m_prep_program.setUniformValue("qt_mvp", state.projection * modelview);
        m_prep_program.setUniformValue("qt_mv", modelview);

      //  ogl.glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, 0);

        QOpenGLFramebufferObjectFormat fboformat;
        fboformat.setTextureTarget(GL_TEXTURE_2D);
        fboformat.setInternalTextureFormat(GL_FLOAT);
        QOpenGLFramebufferObject fbo_ray_start(state.viewport, GL_TEXTURE_2D);
        QOpenGLFramebufferObject fbo_ray_end(state.viewport, GL_TEXTURE_2D);

        ogl.glDisable(GL_DEPTH);


        fbo_ray_start.bind();
        ogl.glClearColor(0,0,0,1);
        ogl.glClear(GL_COLOR_BUFFER_BIT);
        ogl.glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, 0);
        fbo_ray_start.release();

        glCullFace(GL_FRONT);
        fbo_ray_end.bind();
        ogl.glClearColor(0,0,0,1);
        ogl.glClear(GL_COLOR_BUFFER_BIT);
        ogl.glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, 0);
        fbo_ray_end.release();

        std::ostringstream name;
        name << "ray_start" << index++ << ".png";

        fbo_ray_start.toImage().save(name.str().c_str());

        m_arrayBuf.release();
        m_indexBuf.release();
        m_vao.release();
        m_prep_program.release();

        if (!m_volume_program.bind())
            qWarning() << "Unable to bind m_volume_program\n";


        // now draw the volume
        ogl.glActiveTexture(GL_TEXTURE0);

        //ogl.glEnable(GL_TEXTURE_3D);
        error_nr = glGetError(); if (error_nr)  qWarning() << "ogl.glEnable(GL_TEXTURE_3D);" << error_nr;

        // using an integer valued texture requires this
        ogl.glTexParameterf (GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        ogl.glTexParameterf (GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

        error_nr = glGetError(); if (error_nr)  qWarning() << "ogl.glTexParameterf: GL_TEXTURE_*_FILTER: " << error_nr;

        ogl.glBindTexture(GL_TEXTURE_3D, m_volume_tex);
        error_nr = glGetError(); if (error_nr)  qWarning() << "glBindTexture" << error_nr;

        m_volume_program.setUniformValue(m_voltex_param, 0);
        error_nr = glGetError(); if (error_nr)  qWarning() << "m_volume_program.setUniformValue(volume)" << error_nr;

        ogl.glActiveTexture(GL_TEXTURE0 + 1);
        ogl.glBindTexture(GL_TEXTURE_2D, fbo_ray_start.texture());
        error_nr = glGetError(); if (error_nr)  qWarning() << "ogl.glBindTexture (start)" << error_nr;
        m_volume_program.setUniformValue(m_ray_start_param, 1);
        error_nr = glGetError(); if (error_nr)  qWarning() << "m_volume_program.setUniformValue(ray_start)" << error_nr;

        ogl.glActiveTexture(GL_TEXTURE0 + 2);
        ogl.glBindTexture(GL_TEXTURE_2D, fbo_ray_end.texture());
        error_nr = glGetError(); if (error_nr)  qWarning() << "ogl.glBindTexture (end)" << error_nr;
        m_volume_program.setUniformValue(m_ray_end_param, 2);
        error_nr = glGetError(); if (error_nr)  qWarning() << "m_volume_program.setUniformValue(ray_end)" << error_nr;


        //ogl.glEnable(GL_TEXTURE_2D);
        ogl.glDisable(GL_DEPTH_TEST);
        ogl.glDisable(GL_CULL_FACE);
        error_nr = glGetError(); if (error_nr)  qWarning() << "ogl.glDisable(GL_CULL_FACE);" << error_nr;


        auto iso_value_param = m_volume_program.uniformLocation("iso_value");
        assert(iso_value_param != -1);
        m_volume_program.setUniformValue(iso_value_param, 0.5f);

        ogl.glDisable(GL_CULL_FACE);
        ogl.glDisable(GL_DEPTH_TEST);
        error_nr = glGetError(); if (error_nr)  qWarning() << "ogl.glDisable(GL_DEPTH_TEST);" << error_nr;


        m_vao_2nd_pass.bind();
        m_arrayBuf_2nd_pass.bind();
        m_indexBuf_2nd_pass.bind();

        ogl.glDrawElements(GL_TRIANGLE_FAN, 4, GL_UNSIGNED_SHORT, 0);
        error_nr = glGetError(); if (error_nr)  qWarning() << "ogl.glDrawElements" << error_nr;

        m_indexBuf_2nd_pass.release();
        m_arrayBuf_2nd_pass.release();
        m_vao_2nd_pass.release();
        m_volume_program.release();
        ogl.glDisable(GL_TEXTURE_3D);

}

