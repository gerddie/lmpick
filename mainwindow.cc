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
#include "ui_mainwindow.h"

#include <QFileDialog>
#include <mia/3d/imageio.hh>
#include <sstream>

MainWindow::MainWindow(QWidget *parent) :
        QMainWindow(parent),
        ui(new Ui::MainWindow)
{
        ui->setupUi(this);
        m_glview = findChild<MainopenGLView*>();
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
            VolumeData::Pointer obj = std::make_shared<VolumeData>(volume);
            m_glview->setVolume(obj);
        }


}
