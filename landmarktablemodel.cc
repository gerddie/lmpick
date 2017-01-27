#include "landmarktablemodel.hh"

LandmarkTableModel::LandmarkTableModel(QObject *parent):
        QAbstractTableModel(parent)
{
}

void LandmarkTableModel::setLandmarkList(PLandmarkList landmarks)
{

        size_t old_size = m_the_list ? m_the_list->size(): 0;
        size_t new_size = landmarks ? landmarks->size() : 0;

        if (old_size > new_size) {
                beginRemoveRows(QModelIndex(), old_size , new_size);
                m_the_list = landmarks;
                endRemoveRows();
        }else if (new_size > old_size) {
                beginInsertRows(QModelIndex(), old_size , new_size);
                m_the_list = landmarks;
                endInsertRows();
        }
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

        if (static_cast<size_t>(index.row()) >= m_the_list->size() || index.row() < 0)
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

void LandmarkTableModel::addLandmark(PLandmark lm)
{
        beginInsertRows(QModelIndex(), m_the_list->size(), m_the_list->size());
        m_the_list->add(lm);
        endInsertRows();
}

PLandmarkList LandmarkTableModel::getLandmarkList() const
{
        return m_the_list;
}
