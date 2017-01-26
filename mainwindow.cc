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

#include "mainwindow.hh"
#include "volumedata.hh"
#include "landmarklistio.hh"
#include "ui_mainwindow.h"

#include <QFileDialog>
#include <mia/3d/imageio.hh>
#include <sstream>
#include <QMessageBox>


using std::make_shared;

#ifdef INITIAL_TESTING

static PLandmarkList create_debug_list()
{
        PLandmarkList result = make_shared<LandmarkList>("Test list");
        std::vector<QVector3D> v{{251.8, 140.8 ,128},
                                 {140.8, 251.8,128},
                                 {140.8,140.8, 239},
                                 {140.8,30,128},
                                 {30,140.8,128},
                                 {140.8,140.8, 17}};
        Camera c;
        const char n[][2] = {"a", "b", "c", "d", "e", "f"};

        for(int i = 0; i < 6; ++i) {
                result->add(PLandmark(new Landmark(n[i], v[i], c)));
        }
        return result;
}


static PVolumeData create_debug_volume()
{
        auto img = new mia::C3DFImage(mia::C3DBounds(128,256,128));

        auto i = img->begin();
        for (unsigned int z = 0; z < 128; ++z) {
                float fz = sin (z * M_PI / 127);
                for (unsigned int y = 0; y < 256; ++y) {
                        float fy = sin (y * M_PI / 255) * fz;
                        for (unsigned int x = 0; x < 128; ++x, ++i)  {
                                *i = fy * sin (x * M_PI / 127);
                        }
                }
        }
        img->set_voxel_size(mia::C3DFVector(2.2, 1.1, 2.0));
        return PVolumeData(new VolumeData(mia::P3DImage(img)));
}

#endif

MainWindow::MainWindow(QWidget *parent) :
        QMainWindow(parent),
        ui(new Ui::MainWindow)
{
        ui->setupUi(this);
        m_glview = findChild<MainopenGLView*>();
        m_iso_slider = findChild<QSlider*>("isoValueSlider");
        m_landmark_list_view = findChild<QListView*>("LandmarkListView");
        assert(m_iso_slider);
        connect(m_iso_slider, SIGNAL(valueChanged(int)), m_glview, SLOT(set_volume_isovalue(int)));

        m_glview->addAction(ui->action_Set);
        m_glview->addAction(ui->action_Add);


        assert(m_landmark_list_view);

        m_landmark_list_view->addAction(ui->action_Add);
        m_landmark_list_view->addAction(ui->action_Set);
        m_landmark_list_view->addAction(ui->action_Edit);
        m_landmark_list_view->addAction(ui->action_Clear);
        m_landmark_list_view->addAction(ui->action_Clear_all_locations);

#ifdef INITIAL_TESTING
        m_current_landmarklist = create_debug_list();
        m_glview->setLandmarkList(m_current_landmarklist);

        m_current_volume = create_debug_volume();
        m_glview->setVolume(m_current_volume);
#endif
}



MainWindow::~MainWindow()
{
        delete ui;
}

void MainWindow::on_actionE_xit_triggered()
{
    close();
}

void MainWindow::on_actionOpen_Volume_triggered()
{
        const auto& imageio  = mia::C3DImageIOPluginHandler::instance();
        auto file_types = imageio.get_supported_suffix_set();
        std::ostringstream filetypes;

        QStringList filters;
        filetypes << "(";
        for (auto i: file_types)
                filetypes << "*." << i.c_str() << " ";
        filetypes << ")";
        QFileDialog dialog(this, "Open volume data set", ".", filetypes.str().c_str());
        dialog.setFileMode(QFileDialog::ExistingFile);
        if (dialog.exec()) {
            auto fileNames = dialog.selectedFiles();
            if ( fileNames.length() > 1) {
                    qWarning() << "Open volume data:  selected " << fileNames.length()
                               << "files, only first one will be used";
            }else if (fileNames.empty()) {
                    qWarning() << "Open volume data:  no files selected";
                    return;
            }
            auto volume = mia::load_image3d(fileNames.first().toStdString());
            m_current_volume = std::make_shared<VolumeData>(volume);
            auto intensity_range = m_current_volume->get_intensity_range();
            m_glview->setVolume(m_current_volume);
            m_iso_slider->setRange(intensity_range.first+1, intensity_range.second);
            m_iso_slider->setValue((intensity_range.second - intensity_range.first) / 2);
        }
}

void MainWindow::on_action_Add_triggered()
{
        qDebug() << "add landmark called";
}

void MainWindow::on_action_Open_landmarkset_triggered()
{
    QFileDialog dialog(this, "Open landmark list", ".", "(MIA landmark list *.lmx)");
    dialog.setFileMode(QFileDialog::ExistingFile);

    if (dialog.exec()) {
        auto fileNames = dialog.selectedFiles();
        if ( fileNames.empty()) {
                qWarning() << "Open landmark list:  no files selected";
                return;
        }

        LandmarklistIO io;
        auto lmlist = io.read(fileNames.first());
        if (lmlist) {
                m_current_landmarklist = lmlist;
                m_glview->setLandmarkList(m_current_landmarklist);
        }else{
             QMessageBox box(QMessageBox::Information, "Error loading lndmarks", "Unable to load landmarks",
                             QMessageBox::Ok);
        }
    }
}
