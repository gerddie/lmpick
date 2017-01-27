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

#ifndef SPHERE_HH
#define SPHERE_HH

#include "drawable.hh"

class Sphere : public Drawable
{
public:
        explicit Sphere(const QVector4D& color);
        ~Sphere();
        virtual void detach_gl(QOpenGLContext& context);
private:
        virtual void do_attach_gl(QOpenGLContext& context);
        virtual void do_draw(const GlobalSceneState& state, QOpenGLContext& context) const;

        QVector4D m_base_color;
        static int m_instances;
        static int m_instances_gl_attached;
        static struct SphereImpl *impl;
};

#endif // SPHERE_HH
