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
        { 0, 1, 0, /**/ 0, 1, 0, /**/0, 1, 0},
        { 0, 0, 1, /**/ 0, 0, 1, /**/0, 0, 1},
        {-1, 0, 0, /**/-1, 0, 0, /**/1, 1, 0},
        { 0,-1, 0, /**/ 0,-1, 0, /**/0, 1, 1},
        { 0, 0,-1, /**/ 0, 0,-1, /**/1, 0, 1}
};

static const unsigned short indices[] = {
   1, 2, 0, 4, 2, 3, 3, 4, 5, 0, 1, 3
};

Octaeder::Octaeder():
        m_arrayBuf(QOpenGLBuffer::VertexBuffer),
        m_indexBuf(QOpenGLBuffer::IndexBuffer)
{
        initializeOpenGLFunctions();

        // Generate 2 VBOs
        assert(m_arrayBuf.create());
        assert(m_indexBuf.create());

        // Initializes cube geometry and transfers it to VBOs
        m_arrayBuf.bind();
        m_arrayBuf.setUsagePattern(QOpenGLBuffer::StaticDraw);
        m_arrayBuf.allocate(vertices, 6 * sizeof(VertexData));

        // Transfer index data to VBO 1
        m_indexBuf.bind();
        m_indexBuf.allocate(indices, sizeof(indices));

}

void Octaeder::draw(QOpenGLShaderProgram& program)
{
        assert(m_arrayBuf.bind());

        // Offset for position
        int offset = 0;

        // Tell OpenGL programmable pipeline how to locate vertex position data
        int vertexLocation = program.attributeLocation("qt_Vertex");
        if (vertexLocation == -1)
                qWarning() << "vertex loction attribute not found";

        program.enableAttributeArray(vertexLocation);
        program.setAttributeBuffer(vertexLocation, GL_FLOAT, offset, 3, sizeof(VertexData));


        // Offset for normals
        offset += sizeof(QVector3D);
        int normalLocation = program.attributeLocation("qt_Normal");
        if (normalLocation != -1) {
                program.enableAttributeArray(normalLocation);
                program.setAttributeBuffer(normalLocation, GL_FLOAT, offset, 3, sizeof(VertexData));
        }else
                qWarning() << "'normal' loction attribute not found";

        offset += sizeof(QVector3D);
        int colorLocation = program.attributeLocation("qt_Color");
        if (colorLocation == -1)
                qWarning() << "color loction attribute not found";

        program.enableAttributeArray(colorLocation);
        program.setAttributeBuffer(colorLocation, GL_FLOAT, offset, 3, sizeof(VertexData));

        m_indexBuf.bind();
        // Draw cube geometry using indices from VBO 1
        glDrawElements(GL_TRIANGLE_STRIP, 12, GL_UNSIGNED_SHORT, 0);
}
