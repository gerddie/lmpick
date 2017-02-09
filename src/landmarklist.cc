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
#include <QFileInfo>
#include <QDir>
#include <cassert>

using std::for_each;

LandmarkList::LandmarkList(const QString& name):
        m_name(name),
        m_dirty(false)
{

}

bool LandmarkList::dirty() const
{
        return m_dirty;
}

void LandmarkList::setDirtyFlag(bool d)
{
        m_dirty = d;
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

const Landmark& LandmarkList::at(unsigned  i) const
{
        assert(i < m_list.size());
        return *m_list[i];
}

Landmark& LandmarkList::at(unsigned i)
{
        assert(i < m_list.size());
        return *m_list[i];
}


PLandmark LandmarkList::operator [](unsigned  i)
{
        assert(i < m_list.size());
        return m_list[i];
}

bool LandmarkList::add(PLandmark landmark)
{
        auto i = m_index_map.find(landmark->getName());
        if (i != m_index_map.end())
                return false;
        m_index_map[landmark->getName()] = m_list.size();
        m_list.push_back(landmark);

        m_dirty = true;
        return true;
}

size_t LandmarkList::size() const
{
        return m_list.size();
}

QString LandmarkList::getFilename() const
{
    return m_filename;
}

void LandmarkList::setFilename(const QString &filename)
{
    m_filename = filename;
}

QString LandmarkList::getName() const
{
        return m_name;
}

void LandmarkList::setName(const QString &name)
{
        m_name = name;
}

void LandmarkList::remove(unsigned idx, unsigned count)
{
        unsigned end = idx + count;

        for (unsigned  i = idx; i < end; ++i) {
        m_index_map.erase(m_list[i]->getName());
        }

        m_list.erase(m_list.begin() + idx, m_list.begin() + end);

        for_each(m_index_map.begin(), m_index_map.end(),
                 [idx,count](std::map<QString, unsigned>::value_type& it){
                if (it.second >= idx)
                        it.second -= count;
        });
        m_dirty = true;
}

int LandmarkList::renameLandmark(const QString& old_name, const QString& new_name)
{
        auto i = m_index_map.find(old_name);
        int idx = i->second;
        if (old_name != new_name) {
                // the new name is already used and it is not the old name
                if (has(new_name))
                        return -1;

                m_index_map.erase(i);
                m_list[idx]->set_name(new_name);
                m_index_map[new_name] = idx;
                m_dirty = true;
        }
        return idx;
}

bool LandmarkList::remove(const QString& name)
{
        auto i = m_index_map.find(name);
        if (i == m_index_map.end())
                return false;

        unsigned idx = i->second;
        m_list.erase(m_list.begin() + idx);
        m_index_map.erase(i);

        // correct index map
        for_each(m_index_map.begin(), m_index_map.end(),
                 [idx](std::map<QString, unsigned>::value_type& it){
                if (it.second >= idx)
                        --it.second;
        });
        m_dirty = true;
        return true;
}

bool LandmarkList::has(const QString& name) const
{
        return m_index_map.find(name) != m_index_map.end();
}

LandmarkList::const_iterator LandmarkList::begin() const
{
        return m_list.begin();
}

LandmarkList::const_iterator LandmarkList::end() const
{
        return m_list.end();
}

bool LandmarkList::hasTemplatePictures() const
{
        for (auto lm: m_list) {
                if (lm->has(Landmark::lm_picfile))
                        return true;
        }
        return false;
}

QString LandmarkList::getBaseDir() const
{
        if (m_filename.isEmpty())
                return QString();
        return QFileInfo(m_filename).absoluteDir().absolutePath();
}

void LandmarkList::clearAllLocations()
{
        for (auto lm: m_list) {
                lm->clearFlag(Landmark::lm_location);
        }
        setDirtyFlag(true);
}
