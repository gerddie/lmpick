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

#ifndef VOLUMEDATA_HH
#define VOLUMEDATA_HH

#include "drawable.hh"
#include <mia/3d/image.hh>
#include <QOpenGLBuffer>

/**
  \brief Class for rendering an iso-surface from a volume data set

  This class implements the rendering of an iso-surface of a 3D voxel
  data set of intensity values.

  \todo: The Rendering writes depth values, but doesn't handle depth
   already written to the depth buffer. Hence this object should be drawn
   first, and only one can be drawn.


*/
class VolumeData : public Drawable
{
public:
        typedef std::shared_ptr<VolumeData> Pointer;

        VolumeData(mia::P3DImage data);

        ~VolumeData();

        void set_iso_value(float iso);

        float get_iso_value() const;

        std::pair<int, int> get_intensity_range() const;

        std::pair<bool, QVector3D> get_surface_coordinate(const QPoint& location) const;

        virtual void detach_gl(QOpenGLContext& context) override;

        QVector3D get_viewspace_scale() const;

        QVector3D get_viewspace_shift() const;

private:
        virtual void do_draw(const GlobalSceneState& state, QOpenGLContext& context) const override;
        virtual void do_attach_gl(QOpenGLContext& context) override;

        struct VolumeDataImpl *impl;
};

typedef VolumeData::Pointer PVolumeData;

#endif // VOLUMEDATA_HH
