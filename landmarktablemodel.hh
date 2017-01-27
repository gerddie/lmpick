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
private:

        PLandmarkList m_the_list;
};

#endif // LANDMARKTABLEMODEL_HH
