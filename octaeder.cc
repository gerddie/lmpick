#include "octaeder.hh"
#include <cassert>
struct VertexData
{
    GLfloat px, py, pz;
    GLfloat nx, ny, nz;
    GLfloat cr, cg, cb;
};


static const VertexData vertices[] = {
        { 1, 0, 0, /**/ 1, 0, 0, /**/1, 0, 0},
        { 0, 1, 0, /**/ 0, 1, 0, /**/1, 0.5, 0},
        { 0, 0, 1, /**/ 0, 0, 1, /**/1, 0, 0.5},
        {-1, 0, 0, /**/-1, 0, 0, /**/0, 0, 1},
        { 0,-1, 0, /**/ 0,-1, 0, /**/0, 0.5, 1},
        { 0, 0,-1, /**/ 0, 0,-1, /**/0.5, 0, 1}
};

static const unsigned short indices[] = {
   1, 2, 0, 4, 5, 3, 3, 4, 2, 2, 3, 1, 5, 0

};

Octaeder::Octaeder():
        m_arrayBuf(QOpenGLBuffer::VertexBuffer),
        m_indexBuf(QOpenGLBuffer::IndexBuffer)
{
}

void Octaeder::do_attach_gl(QOpenGLContext& context)
{
        // Generate 2 VBOs
        assert(m_arrayBuf.create());
        assert(m_indexBuf.create());
        m_vao.create();
        m_vao.bind();

        // Initializes cube geometry and transfers it to VBOs
        m_arrayBuf.bind();
        m_arrayBuf.setUsagePattern(QOpenGLBuffer::StaticDraw);
        m_arrayBuf.allocate(vertices, 6 * sizeof(VertexData));

        // Transfer index data to VBO 1
        m_indexBuf.bind();
        m_indexBuf.allocate(indices, sizeof(indices));

        if (!m_program.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/view.glsl"))
                qWarning() << "Error compiling ':/shaders/view.glsl', view will be clobbered\n";

        if (!m_program.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/basic_frag.glsl"))
                qWarning() << "Error compiling ':/s makeCurrent();haders/fshader.glsl', view will be clobbered\n";

        if (!m_program.link())
                qWarning() << "Error linking m_view_program', view will be clobbered\n";;


        // Offset for position
        int offset = 0;

        // Tell OpenGL programmable pipeline how to locate vertex position data
        int vertexLocation = m_program.attributeLocation("qt_Vertex");
        if (vertexLocation == -1)
                qWarning() << "vertex loction attribute not found";

        m_program.enableAttributeArray(vertexLocation);
        m_program.setAttributeBuffer(vertexLocation, GL_FLOAT, offset, 3, sizeof(VertexData));


        // Offset for normals
        offset += sizeof(QVector3D);
        int normalLocation = m_program.attributeLocation("qt_Normal");
        if (normalLocation != -1) {
                m_program.enableAttributeArray(normalLocation);
                m_program.setAttributeBuffer(normalLocation, GL_FLOAT, offset, 3, sizeof(VertexData));
        }else
                qWarning() << "'normal' loction attribute not found";

        offset += sizeof(QVector3D);
        int colorLocation = m_program.attributeLocation("qt_Color");
        if (colorLocation == -1)
                qWarning() << "color loction attribute not found";
        m_program.enableAttributeArray(colorLocation);
        m_program.setAttributeBuffer(colorLocation, GL_FLOAT, offset, 3, sizeof(VertexData));

        m_arrayBuf.release();
        m_indexBuf.release();
        m_vao.release();
}

void Octaeder::detach_gl(QOpenGLContext& context)
{
        m_arrayBuf.destroy();
        m_indexBuf.destroy();
        m_program.release();
}

void Octaeder::do_draw(const GlobalSceneState& state, QOpenGLContext& context) const
{
        m_vao.bind();
        auto modelview = state.get_modelview_matrix();
        m_program.setUniformValue("qt_mvp", state.projection * modelview);
        m_program.setUniformValue("qt_mv", modelview);
        m_program.setUniformValue("qt_LightDirection", state.light_source);

        m_program.bind();
        m_arrayBuf.bind();
        m_indexBuf.bind();


        // Draw cube geometry using indices from VBO 1
        context.functions()->glDrawElements(GL_TRIANGLE_STRIP, 14, GL_UNSIGNED_SHORT, 0);

        m_arrayBuf.release();
        m_indexBuf.release();
        m_vao.release();
}
