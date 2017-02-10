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

#ifndef MAINWINDOW_HH
#define MAINWINDOW_HH

#include "mainopenglview.hh"
#include "landmarktableview.hh"
#include "landmarktablemodel.hh"
#include <QMainWindow>
#include <QSlider>
#include <QTableView>
#include <QSortFilterProxyModel>
#include <QPixmap>
#include <map>


class QLabel;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
        Q_OBJECT

public:
        explicit MainWindow(QWidget *parent = 0);
        ~MainWindow();

public slots:
        void availableDataChanged();

private slots:
        void on_actionE_xit_triggered();

        void on_actionOpen_Volume_triggered();

        void on_action_Add_triggered();

        void on_action_Open_landmarkset_triggered();

        void isovalue_changed();

        void on_actionSave_landmark_set_As_triggered();
        void on_action_Save_Landmark_set_triggered();

        void on_action_Edit_triggered();

        void on_action_TakeSnapshot_triggered();

        void on_action_CreateTemplate_triggered();

        void on_action_About_triggered();

        void on_action_Clear_all_locations_triggered();

        void on_action_Clear_triggered();

        void on_action_Delete_triggered();

protected:
        void closeEvent(QCloseEvent *event) override;

        void landmarkSelectionChanged(const QModelIndex& idx, const QModelIndex& other_idx);
private:
        int  getSelectedLandmarkIndex(QModelIndex *idx) const;


        Ui::MainWindow *ui;
        MainopenGLView *m_glview;
        QSlider *m_iso_slider;
        LandmarkTableView *m_landmark_tv;
        LandmarkTableModel *m_landmark_lm;
        QSortFilterProxyModel *m_landmark_sort_proxy;
        QLabel *m_template_view;

        PVolumeData m_current_volume;
        PLandmarkList m_current_landmarklist;

        QString m_title_template;
        QString m_volume_name;
        QString m_landmarks_name;

        // configurable data
        QString m_snapshot_name_prototype;
        int m_snapshot_serial_number;
        std::map<QString, QPixmap> m_image_cache;
};

#endif // MAINWINDOW_HH
