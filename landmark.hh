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

#ifndef LANDMARK_HH
#define LANDMARK_HH

#include "camera.hh"
#include <QString>
#include <memory>


class Landmark
{
public:
        typedef std::shared_ptr<Landmark> Pointer;

        Landmark();

        explicit Landmark(const QString& name);

        Landmark(const QString& name, const QVector3D& location, const Camera& best_view);

        bool is_set() const;

        void set_template_image_file(const QString& fname);

        void set(const QVector3D& location, float iso, const Camera& best_view);

        const QString& get_name() const;

        const QString& get_template_filename() const;

        const QVector3D& get_location() const;

        const Camera& get_camera() const;

        float get_iso_value()const;


        void set_location(const QVector3D& loc);

        void set_iso_value(float iso);

        void set_camera(const Camera& camera);

private:
        QString m_name;
        bool m_is_set;
        QString m_template_image_filename;
        QVector3D m_location;
        Camera m_best_view;
        float m_iso_value;


};

typedef Landmark::Pointer PLandmark;

#endif // LANDMARK_HH
