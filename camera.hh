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

#ifndef CAMERA_HH
#define CAMERA_HH

#include <QVector3D>
#include <QQuaternion>

/**
  \brief OpenGL view defining camera

  This class implements the camera that is used for visualizing
  the graphic scene.

*/

class Camera
{
public:
        /// Default constructor to define the default view
        Camera();

        /**
          Camera
           \param position
           \param rotation
           \param zoom
        */
        Camera(const QVector3D& position, const QQuaternion& rotation, float zoom);


        void set_rotation(const QQuaternion& r);
        const QQuaternion& get_rotation() const;
        void rotate(const QQuaternion& r);

        void set_zoom(float z);
        void zoom_in();
        void zoom_out();
        float get_zoom() const;

        const QVector3D& get_position() const;
        void set_position(const QVector3D& pos);
        void move(const QVector3D& delta);
        void set_distance(float dist);


private:

        QQuaternion m_rotation;
        QVector3D m_position;
        float m_zoom;
};

#endif // CAMERA_HH
