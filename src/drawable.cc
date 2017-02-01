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
        do_attach_gl();
}

void Drawable::detach_gl()
{
        do_detach_gl();
}


QOpenGLContext *Drawable::get_context() const
{
        return m_context;
}
