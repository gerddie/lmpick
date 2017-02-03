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

#ifndef LANDMARKLIST_HH
#define LANDMARKLIST_HH

#include <landmark.hh>
#include <map>
#include <memory>

class LandmarkList
{
public:
        typedef std::shared_ptr<LandmarkList> Pointer;

        typedef std::vector<PLandmark>::const_iterator const_iterator;

        LandmarkList() = default;

        explicit LandmarkList(const QString& name);

        bool add(PLandmark landmark);

        bool remove(const QString& name);

        void remove(unsigned  idx, unsigned  count);

        int rename_landmark(const QString& old_name, const QString& new_name);

        bool has(const QString& name) const;

        PLandmark operator [](unsigned i);

        PLandmark operator [](const QString& name);

        const Landmark& at(unsigned  i) const;

        Landmark& at(unsigned i);

        size_t size() const;

        QString get_filename() const;

        QString get_base_dir() const;

        void set_filename(const QString &filename);

        QString get_name() const;

        void set_name(const QString &name);

        const_iterator begin() const;

        const_iterator end() const;

        bool dirty() const;

        void set_dirty_flag(bool d);

        bool has_template_pictures() const;

private:
        QString m_name;
        QString m_filename;

        std::vector<PLandmark> m_list;
        std::map<QString, unsigned> m_index_map;

        bool m_dirty;
};

typedef LandmarkList::Pointer PLandmarkList;

#endif // LANDMARKLIST_HH
