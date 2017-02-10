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

#include "landmarktablemodel.hh"
#include <cassert>

LandmarkTableModel::LandmarkTableModel(QObject *parent):
        QAbstractTableModel(parent)
{
}

void LandmarkTableModel::setLandmarkList(PLandmarkList landmarks)
{

        size_t old_size = m_the_list ? m_the_list->size(): 0;
        size_t new_size = landmarks ? landmarks->size() : 0;

        if (old_size > new_size) {
                beginRemoveRows(QModelIndex(), new_size, old_size);
                m_the_list = landmarks;
                endRemoveRows();
        }else if (new_size > old_size) {
                beginInsertRows(QModelIndex(), old_size , new_size -1);
                m_the_list = landmarks;
                endInsertRows();
        }else
                m_the_list = landmarks;

}

int LandmarkTableModel::rowCount(const QModelIndex &parent) const
{
        Q_UNUSED(parent);
        if (m_the_list)
                return m_the_list->size();
        else return 0;
}

int LandmarkTableModel::columnCount(const QModelIndex &parent) const
{
        Q_UNUSED(parent);
        return 2;
}

QVariant LandmarkTableModel::data(const QModelIndex &index, int role) const
{
        if (!index.isValid() || !m_the_list)
                return QVariant();

        if (static_cast<size_t>(index.row()) >= m_the_list->size() || index.row() < 0)
                return QVariant();

        if (role == Qt::DisplayRole) {
                Landmark lm = m_the_list->at(index.row());
                if (index.column() == 0)
                        return lm.getName();
                else {
                        if (!lm.isSet())
                                return QVariant();

                        if (index.column() == 1) {
                                if (lm.has(Landmark::lm_location)) {
                                        auto p = lm.getLocation();
                                        QString result;
                                        QTextStream ts(&result);
                                        ts.setRealNumberPrecision(4);
                                        ts << "(" << p.x() << ", " << p.y() << ", " << p.z() << ")";
                                        return result;
                                } else {
                                        return tr("  <not set>  ");
                                }
                        }else
                                return QVariant();
                }
        }

        return QVariant();
}

QVariant LandmarkTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
        if (role != Qt::DisplayRole)
                return QVariant();

        if (orientation == Qt::Horizontal) {
                switch (section) {
                case 0:
                        return tr("Name");
                case 1:
                        return tr("Location");
                default:
                        return QVariant();
                }
           }
           return QVariant();
}

void LandmarkTableModel::addLandmark(PLandmark lm)
{
        beginInsertRows(QModelIndex(), m_the_list->size(), m_the_list->size());
        m_the_list->add(lm);
        endInsertRows();
}

void LandmarkTableModel::removeLandmark(int idx)
{
        assert(idx > 0);
        beginRemoveRows(QModelIndex(), idx, idx);
        m_the_list->remove(idx, 1);
        endRemoveRows();
}

int LandmarkTableModel::renameLandmark(const QModelIndex &index, const QString& old_name, const QString& new_name)
{
        int idx = m_the_list->renameLandmark(old_name, new_name);
        emit dataChanged(index, index);
        return idx;
}

PLandmarkList LandmarkTableModel::getLandmarkList() const
{
        return m_the_list;
}
