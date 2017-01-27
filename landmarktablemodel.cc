#include "landmarktablemodel.hh"

LandmarkTableModel::LandmarkTableModel(QObject *parent):
        QAbstractTableModel(parent)
{

}

void LandmarkTableModel::setLandmarkList(PLandmarkList landmarks)
{
        if (m_the_list)
                removeRows(0, m_the_list->size());
        m_the_list = landmarks;
        if (m_the_list)
                insertRows(0,m_the_list->size());
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
        return 4;
}

QVariant LandmarkTableModel::data(const QModelIndex &index, int role) const
{
        if (!index.isValid() || !m_the_list)
                return QVariant();

        if (index.row() >= m_the_list->size() || index.row() < 0)
                return QVariant();

        if (role == Qt::DisplayRole) {
                Landmark lm = m_the_list->at(index.row());
                if (index.column() == 0)
                        return lm.get_name();
                else {
                        if (!lm.is_set())
                                return QVariant();

                        auto p = lm.get_location();
                        switch (index.column()) {
                        case 1: return p.x();
                        case 2: return p.y();
                        case 3: return p.z();
                        default: return QVariant();
                        }
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
                       return "x";

                case 2:
                        return "y";

                case 3:
                        return "z";

                default:
                        return QVariant();
                }
           }
           return QVariant();
}

