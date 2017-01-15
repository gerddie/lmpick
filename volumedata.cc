#include "volumedata.hh"
#include <mia/3d/filter.hh>
#include <QOpenGLFramebufferObject>
#include <QOpenGLFramebufferObjectFormat>
#include <QPainter>
#include <cassert>

VolumeData::VolumeData(mia::P3DImage data):
        m_arrayBuf(QOpenGLBuffer::VertexBuffer),
        m_indexBuf(QOpenGLBuffer::IndexBuffer),
        m_volume_texture(QOpenGLTexture::Target3D),
        m_image(data)
{
        assert(m_image);

        auto s = m_image->get_size();
        auto v = m_image->get_voxel_size();

        QVector3D size(s.x * v.x, s.y * v.y, s.z * v.z);
        float max_coord = size.x();
        if (max_coord < size.y())
                max_coord = size.y();
        if (max_coord < size.z())
                max_coord = size.z();

        m_scale = size / max_coord;
        m_end = 0.5 * m_scale;
        m_start = -m_end;

}

struct VertexData
{
    GLfloat px, py, pz;
    GLfloat nx, ny, nz;
    GLfloat cr, cg, cb;
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


void VolumeData::do_attach_gl()
{
        /*
        // prepare the texture
        m_volume_texture.setSize(m_image->get_size().x, m_image->get_size().y, m_image->get_size().z);
        m_volume_texture.setMinMagFilters(QOpenGLTexture::Nearest, QOpenGLTexture::Linear);
        m_volume_texture.setWrapMode(QOpenGLTexture::ClampToEdge);
        m_volume_texture.allocateStorage();
        m_volume_texture.bind();

        auto converter = mia::produce_3dimage_filter("convert:repn=ubyte");
        auto pimage = converter->filter(m_image);
        const mia::C3DUBImage& img = dynamic_cast<const mia::C3DUBImage&>(*pimage);

        m_volume_texture.setData(QOpenGLTexture::Luminance, QOpenGLTexture::UInt8,
                                 &img(0,0,0));

        */
        assert(m_arrayBuf.create());
        assert(m_indexBuf.create());

        // Initializes cube geometry and transfers it to VBOs
        m_arrayBuf.bind();
        m_arrayBuf.setUsagePattern(QOpenGLBuffer::StaticDraw);
        m_arrayBuf.allocate(cube_vertices, sizeof(cube_vertices));

        // Transfer index data to VBO 1
        m_indexBuf.bind();
        m_indexBuf.allocate(cube_indices, sizeof(cube_indices));

        if (!m_program.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/view_cube.glsl"))
                qWarning() << "Error compiling ':/shaders/view_cube.glsl', view will be clobbered\n";

        if (!m_program.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/basic_frag.glsl"))
                qWarning() << "Error compiling ':/s makeCurrent();haders/fshader.glsl', view will be clobbered\n";

        if (!m_program.link())
                qWarning() << "Error linking m_view_program', view will be clobbered\n";;


        // Tell OpenGL programmable pipeline how to locate vertex position data
        int vertexLocation = m_program.attributeLocation("qt_Vertex");
        if (vertexLocation == -1)
                qWarning() << "vertex loction attribute not found";
        m_program.enableAttributeArray(vertexLocation);
        m_program.setAttributeBuffer(vertexLocation, GL_FLOAT, 0, 3, sizeof(VertexData));

        /*
        int normalLocation = m_program.attributeLocation("qt_Normal");
        if (normalLocation == -1)
                qWarning() << "normal loction attribute not found";
        m_program.enableAttributeArray(normalLocation);
        m_program.setAttributeBuffer(normalLocation, GL_FLOAT, 3 * sizeof(float), 3, sizeof(VertexData));

      //  int colorLocation = m_program.attributeLocation("qt_Color");
        if (colorLocation == -1)
                qWarning() << "color loction attribute not found";
        m_program.enableAttributeArray(colorLocation);
        m_program.setAttributeBuffer(colorLocation, GL_FLOAT, 6 * sizeof(float), 3, sizeof(VertexData));
        */



}

void VolumeData::detach_gl()
{
        m_volume_texture.destroy();
        m_arrayBuf.destroy();
        m_indexBuf.destroy();
        m_program.release();
}

void VolumeData::do_draw(const GlobalSceneState& state, QOpenGLFunctions& ogl) const
{
        //        m_volume_texture.bind();


        // draw to two FBOs to get display range
        /*
        QOpenGLFramebufferObjectFormat fboformat;
        fboformat.setInternalTextureFormat(GL_FLOAT);

        QOpenGLFramebufferObject fbo_ray_start(state.viewport, fboformat);
        QOpenGLFramebufferObject fbo_ray_stop(state.viewport, fboformat);

        fbo_ray_start.bind();


        ogl.glEnable(GL_CULL_FACE);
        ogl.glCullFace(GL_BACK);




        fbo_ray_start.release();
        */



        auto modelview = state.get_modelview_matrix();
        m_program.setUniformValue("qt_mvp", state.projection * modelview);
        m_program.setUniformValue("qt_mv", modelview);

        m_program.bind();
        m_arrayBuf.bind();
        m_indexBuf.bind();

        ogl.glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, 0);

        m_arrayBuf.release();
        m_indexBuf.release();
}

