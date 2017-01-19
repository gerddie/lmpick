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

#ifndef DRAWABLE_HH
#define DRAWABLE_HH

#include "globalscenestate.hh"
#include <QOpenGLFunctions>
#include <QOpenGLVertexArrayObject>
#include <memory>

class Drawable {
public:
        typedef std::shared_ptr<Drawable> Pointer;

        Drawable();
        virtual ~Drawable();

        void draw(const GlobalSceneState& state, QOpenGLContext& context) const;
        void attach_gl(QOpenGLContext& context);
        virtual void detach_gl(QOpenGLContext& context) = 0;
private:
        virtual void do_attach_gl(QOpenGLContext& context) = 0;
        virtual void do_draw(const GlobalSceneState& state, QOpenGLContext& context) const = 0;


};



#endif // DRAWABLE_HH
