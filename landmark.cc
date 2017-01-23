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

#include "landmark.hh"

Landmark::Landmark():
        m_is_set(false)
{
}

Landmark::Landmark(const QString& name):
        m_name(name),
        m_is_set(false)
{
}


Landmark::Landmark(const QString& name, const QVector3D& location, const Camera& best_view):
        m_name(name),
        m_is_set(true),
        m_location(location),
        m_best_view(best_view)
{
}

bool Landmark::is_set() const
{
        return m_is_set;
}

void Landmark::set(const QVector3D& location, const Camera& best_view)
{
        m_location = location;
        m_best_view = best_view;
        m_is_set = true;
}

const QString& Landmark::get_name() const
{
        return m_name;
}

const QString& Landmark::get_template_filename() const
{
        return m_template_image_filename;
}

const QVector3D& Landmark::get_location() const
{
        return m_location;
}

const Camera& Landmark::get_camera() const
{
        return m_best_view;
}
