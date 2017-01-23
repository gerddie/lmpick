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

#include "landmarklist.hh"
#include <cassert>

using std::for_each;

LandmarkList::LandmarkList(const QString& name):
        m_name(name)
{

}

PLandmark LandmarkList::operator [](const QString& name)
{
        auto i = m_index_map.find(name);
        if (i != m_index_map.end()) {
                assert(i->second < m_list.size());
                return m_list[i->second];
        }
        return PLandmark();
}

PLandmark LandmarkList::operator [](unsigned  i)
{
        assert(i < m_list.size());
        return m_list[i];
}

bool LandmarkList::add(PLandmark landmark)
{
        auto i = m_index_map.find(landmark->get_name());
        if (i != m_index_map.end())
                return false;
        m_list.push_back(landmark);
        m_index_map[landmark->get_name()] = m_list.size();

        return true;
}

size_t LandmarkList::size() const
{
        return m_list.size();
}

bool LandmarkList::remove(const QString& name)
{
        auto i = m_index_map.find(name);
        if (i == m_index_map.end())
                return false;

        int idx = i->second;
        m_list.erase(m_list.begin() + idx);
        m_index_map.erase(i);

        // correct index map
        for_each(m_index_map.begin(), m_index_map.end(),
                 [idx](std::map<QString, unsigned>::value_type& it){
                if (it.second >= idx)
                        --it.second;
        });
        return true;
}
