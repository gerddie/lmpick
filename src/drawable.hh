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

/**

\brief base class for all objects to be drawn in the scene

*/
class Drawable {
public:
        typedef std::shared_ptr<Drawable> Pointer;

        Drawable();
        virtual ~Drawable();

        void draw(const GlobalSceneState& state);
        void attach_gl(QOpenGLContext *context);
        void detach_gl();

protected:
        QOpenGLContext *get_context() const;
private:
        virtual void do_attach_gl() = 0;
        virtual void do_draw(const GlobalSceneState& state) = 0;
        virtual void do_detach_gl() = 0;

        QOpenGLContext *m_context;
};

#endif // DRAWABLE_HH
