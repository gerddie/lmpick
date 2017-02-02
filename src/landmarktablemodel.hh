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

#ifndef LANDMARKTABLEMODEL_HH
#define LANDMARKTABLEMODEL_HH

#include <QAbstractTableModel>
#include <landmarklist.hh>

class LandmarkTableModel : public QAbstractTableModel
{
        Q_OBJECT
public:


        explicit LandmarkTableModel(QObject *parent = 0);

        int rowCount(const QModelIndex &parent) const override;
        int columnCount(const QModelIndex &parent) const override;
        QVariant data(const QModelIndex &index, int role) const override;
        QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

        void setLandmarkList(PLandmarkList landmarks);
        PLandmarkList getLandmarkList() const;

        void addLandmark(PLandmark lm);
        int renameLandmark(const QModelIndex &index, const QString& old_name, const QString& new_name);
private:

        PLandmarkList m_the_list;
};

#endif // LANDMARKTABLEMODEL_HH
