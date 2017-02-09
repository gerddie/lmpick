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
        m_is_set(false),
        m_flags(lm_none)
{
}

Landmark::Landmark(const QString& name):
        m_name(name),
        m_is_set(false)
{
        m_flags = lm_name;
}


Landmark::Landmark(const QString& name, const QVector3D& location, float iso, const Camera& best_view):
        m_name(name),
        m_is_set(true),
        m_location(location),
        m_best_view(best_view),
        m_iso_value(iso)
{
        m_flags = lm_name | lm_location | lm_camera | lm_iso_value;
}

bool Landmark::has(EFlags flag) const
{
        return (flag &   m_flags) == flag;
}

bool Landmark::isSet() const
{
        return m_is_set;
}

void Landmark::clearFlag(EFlags flag)
{
        m_flags = static_cast<Landmark::EFlags>(static_cast<int>(m_flags) & ~static_cast<int>(flag));
}

void Landmark::set(const QVector3D& location, float iso, const Camera& best_view)
{
        m_location = location;
        m_iso_value = iso;
        m_best_view = best_view;
        m_is_set = true;

        m_flags = m_flags  | lm_location | lm_camera | lm_iso_value;
}

void  Landmark::setLocation(const QVector3D& loc)
{
        m_location = loc;
        m_flags = m_flags | lm_location;
}

void  Landmark::setCamera(const Camera& camera)
{
        m_best_view = camera;
        m_flags = m_flags |lm_camera;
}

void Landmark::set_name(const QString& new_name)
{
        m_name = new_name;
}

const QString& Landmark::getName() const
{
        return m_name;
}

void Landmark::setTemplateImageFile(const QString& fname)
{
        m_template_image_filename = fname;
        m_flags = m_flags | lm_picfile;
}

const QString& Landmark::getTemplateFilename() const
{
        return m_template_image_filename;
}

const QVector3D& Landmark::getLocation() const
{
        return m_location;
}

const Camera& Landmark::getCamera() const
{
        return m_best_view;
}

void Landmark::setIsoValue(float iso)
{
        m_iso_value = iso;
        m_flags = m_flags | lm_iso_value;
}

float Landmark::getIsoValue() const
{
        return m_iso_value;
}
