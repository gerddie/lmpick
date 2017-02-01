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

#include <QStringList>
#include "drawable.hh"


Drawable::Drawable()
{

}

Drawable::~Drawable()
{

}

void Drawable::draw(const GlobalSceneState &state)
{
        do_draw(state);
}

void Drawable::attach_gl(QOpenGLContext *context)
{
        m_context = context;
        if (!m_shader_prefix_set) {
                QString slversion((const char*)context->functions()->glGetString(GL_SHADING_LANGUAGE_VERSION));
                QStringList v = slversion.split(".");

                int major = v.at(0).toInt();
                int minor = v.at(1).toInt();

                if (major > 3 || (major == 3 && minor > 2)) {
                        m_shader_prefix = ":/shaders/shaders_330/";
                        m_shader_prefix_set = true;
                } else {
                        m_shader_prefix = ":/shaders/shaders_120/";
                        m_shader_prefix_set = true;
                }
                qDebug() << "OGLSL version: " << slversion;
        }
        do_attach_gl();
}

void Drawable::compile_and_link(QOpenGLShaderProgram& program, const QString& vtx_prog, const QString& frag_pgrm)
{
        QString vtx_prog_full = m_shader_prefix + vtx_prog;
        QString frag_prog_full = m_shader_prefix + frag_pgrm;

        if (!program.addShaderFromSourceFile(QOpenGLShader::Vertex, vtx_prog_full))
                qWarning() << "Error compiling '" << vtx_prog_full << "' view will be clobbered\n";

        if (!program.addShaderFromSourceFile(QOpenGLShader::Fragment, frag_prog_full))
                qWarning() << "Error compiling '" << frag_prog_full <<  "', view will be clobbered\n";

        if (!program.link())
                qWarning() << "Error linking (" << vtx_prog_full << "," << frag_prog_full << ")', view will be clobbered\n";
}

void Drawable::detach_gl()
{
        do_detach_gl();
}


QOpenGLContext *Drawable::get_context() const
{
        return m_context;
}


bool Drawable::m_shader_prefix_set = false;
QString Drawable::m_shader_prefix;
