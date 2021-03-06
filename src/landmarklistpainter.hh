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

#ifndef LANDMARKLISTPAINTER_HH
#define LANDMARKLISTPAINTER_HH

#include "landmarklist.hh"
#include "drawable.hh"

class LandmarkListPainter : public Drawable
{
public:
        LandmarkListPainter();
        ~LandmarkListPainter();

        void set_landmark_list(PLandmarkList list);
        void set_viewspace_correction(const QVector3D& scale, const QVector3D& shift);

        Landmark& get_active_landmark();
        void set_active_landmark(int idx);
        const QString get_active_landmark_name() const;

private:
        void do_attach_gl() override;
        void do_draw(const GlobalSceneState& state) override;
        void do_detach_gl() override;

        struct LandmarkListPainterImpl *impl;
};

#endif // LANDMARKLISTPAINTER_HH
