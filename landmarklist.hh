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

class Landmarklist
{
public:
        Landmarklist() = default;

        explicit Landmarklist(const QString& name);

        bool add(PLandmark landmark);

        bool remove(const QString& name);

        PLandmark operator [](unsigned i);

        PLandmark operator [](const QString& name);

        size_t size() const;

private:
        QString m_name;

        std::vector<PLandmark> m_list;
        std::map<QString, unsigned> m_index_map;
};


#endif // LANDMARKLIST_HH
