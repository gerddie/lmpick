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

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
        Q_OBJECT

public:
        explicit MainWindow(QWidget *parent = 0);
        ~MainWindow();

private slots:
        void on_actionE_xit_triggered();

        void on_actionOpen_Volume_triggered();

        void on_action_Add_triggered();

        void on_action_Open_landmarkset_triggered();

        void on_iso_value_changed();

private:
        QWidget *action_source(QAction *action);

        Ui::MainWindow *ui;
        MainopenGLView *m_glview;
        QSlider *m_iso_slider;
        LandmarkTableView *m_landmark_tv;
        LandmarkTableModel *m_landmark_lm;

        PVolumeData m_current_volume;
        PLandmarkList m_current_landmarklist;

};

#endif // MAINWINDOW_HH
