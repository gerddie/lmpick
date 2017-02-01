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

#include "octaeder.hh"
#include <cassert>
struct VertexData
{
    GLfloat px, py, pz;
    GLfloat nx, ny, nz;
    GLfloat cr, cg, cb;
};


static const VertexData vertices[] = {
        { .8, 0, 0, /**/ 1, 0, 0, /**/1, 0, 0},
        { 0, .8, 0, /**/ 0, 1, 0, /**/1, 0.5, 0},
        { 0, 0, .8, /**/ 0, 0, 1, /**/1, 0, 0.5},
        {-.8, 0, 0, /**/-1, 0, 0, /**/0, 0, 1},
        { 0,-.8, 0, /**/ 0,-1, 0, /**/0, 0.5, 1},
        { 0, 0,-.8, /**/ 0, 0,-1, /**/0.5, 0, 1}
};

static const unsigned short indices[] = {
   1, 2, 0, 4, 5, 3, 3, 4, 2, 2, 3, 1, 5, 0

};

Octaeder::Octaeder():
        m_arrayBuf(QOpenGLBuffer::VertexBuffer),
        m_indexBuf(QOpenGLBuffer::IndexBuffer)
{
}

void Octaeder::do_attach_gl()
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

        compile_and_link(m_program, "view.glsl", "basic_frag.glsl");

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

void Octaeder::do_detach_gl()
{
        m_arrayBuf.destroy();
        m_indexBuf.destroy();
        m_program.release();
}

void Octaeder::do_draw(const GlobalSceneState& state)
{
        auto& ogl = *get_context()->functions();
        m_vao.bind();
        auto modelview = state.get_modelview_matrix();

        m_program.bind();
        m_arrayBuf.bind();
        m_indexBuf.bind();

        auto normal_matrix = modelview.normalMatrix();

        m_program.setUniformValue("qt_mvp", state.projection * modelview);
        m_program.setUniformValue("qt_mv", normal_matrix);
        m_program.setUniformValue("qt_LightDirection", state.light_source);

        ogl.glEnable(GL_DEPTH_TEST);
        ogl.glDepthFunc(GL_LESS);

        ogl.glEnable(GL_CULL_FACE);
        ogl.glCullFace(GL_BACK);




        // Draw cube geometry using indices from VBO 1
        ogl.glDrawElements(GL_TRIANGLE_STRIP, 14, GL_UNSIGNED_SHORT, 0);

        m_arrayBuf.release();
        m_indexBuf.release();
        m_vao.release();
}
