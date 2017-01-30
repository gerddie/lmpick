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

#include "camera.hh"
#include <QMatrix4x4>

Camera::Camera():
        m_rotation(1,0,0,0),
        m_position(0,0,-250),
        m_zoom(1.0)
{
        m_rotation.normalize();
}

Camera::Camera(const QVector3D& position, const QQuaternion& rotation, float zoom):
        m_rotation(rotation),
        m_position(position),
        m_zoom(zoom)
{
}

void Camera::set_rotation(const QQuaternion& r)
{
        m_rotation = r;
}

void Camera::rotate(const QQuaternion& r)
{
        m_rotation  = r * m_rotation;
}

void Camera::set_position(const QVector3D& pos)
{
        m_position = pos;
}

void Camera::set_zoom(float z)
{
        m_zoom = z;
}

void Camera::zoom_in()
{
        if (m_zoom > 0.1)
                m_zoom *= 0.95;
}

void Camera::zoom_out()
{
        if (m_zoom < 10)
                m_zoom *= 1.05;
}

void Camera::set_distance(float dist)
{
        m_position.setZ(dist);
}

const QQuaternion& Camera::get_rotation() const
{
        return m_rotation;
}

const QVector3D& Camera::get_position() const
{
        return m_position;
}

void Camera::move(const QVector3D& delta)
{
        m_position += delta;
}

float Camera::get_zoom() const
{
        return m_zoom;
}

QMatrix4x4 Camera::get_modelview_matrix() const
{
        QMatrix4x4 modelview;

        modelview.setToIdentity();
        modelview.translate(m_position);
        modelview.rotate(m_rotation);

        return modelview;
}
