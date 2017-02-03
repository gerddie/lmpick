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
#include "qruntimeexeption.hh"

#include <QFileDialog>
#include <QMessageBox>
#include <QInputDialog>
#include <QCloseEvent>
#include <QSortFilterProxyModel>
#include <QScrollBar>

#include <mia/3d/imageio.hh>
#include <sstream>

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

        std::vector<QQuaternion> q{{1, 0, 0, 0},
                                   {1, 1, 0, 0},
                                   {1, 0, 1, 0},
                                   {1, 0, 0, 1},
                                   {1, -1, 0, 0},
                                   {1, 0, -1, 0}};
        const char n[][2] = {"a", "b", "c", "d", "e", "f"};

        for(int i = 0; i < 6; ++i) {
                Camera c(QVector3D(0,0,-250), q[i],1.0);
                result->add(PLandmark(new Landmark(n[i], v[i], 64, c)));
        }
        // don't want prompt for saving by default for test data
        result->set_dirty_flag(false);
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
        ui(new Ui::MainWindow),
        m_landmark_lm(new LandmarkTableModel(this)),
        m_volume_name(tr("(none)")),
        m_snapshot_serial_number(0)
{
        ui->setupUi(this);
        m_glview = findChild<MainopenGLView*>();
        m_iso_slider = findChild<QSlider*>("isoValueSlider");
        m_landmark_tv = findChild<LandmarkTableView *>("LandmarkTV");
        m_template_view = findChild<QLabel*>("graphicsView");
        assert(m_iso_slider);
        connect(m_iso_slider, &QSlider::valueChanged, m_glview, &MainopenGLView::set_volume_isovalue);

        assert(m_landmark_tv);

        QAction *separator = new QAction(this);
        separator->setSeparator(true);
        m_landmark_tv->addAction(ui->action_Edit);
        m_landmark_tv->addAction(ui->action_Add);
        m_landmark_tv->addAction(separator);
        m_landmark_tv->addAction(ui->action_Clear);
        m_landmark_tv->addAction(ui->action_Clear_all_locations);
        separator = new QAction(this);
        separator->setSeparator(true);
        m_landmark_tv->addAction(separator);
        m_landmark_tv->addAction(ui->action_Delete);

        m_glview->setLandmarkModel(m_landmark_lm);
#ifdef INITIAL_TESTING
        m_current_landmarklist = create_debug_list();

        m_current_volume = create_debug_volume();
        m_glview->setVolume(m_current_volume);
#else
        m_current_landmarklist = make_shared<LandmarkList>("unnamed");
#endif
        m_glview->setLandmarkList(m_current_landmarklist);
        m_landmark_lm->setLandmarkList(m_current_landmarklist);

        m_landmark_sort_proxy = new QSortFilterProxyModel();
        m_landmark_sort_proxy->setSourceModel(m_landmark_lm);
        m_landmark_sort_proxy->setDynamicSortFilter(true);

        m_landmark_tv->setModel(m_landmark_sort_proxy);
        m_landmark_tv->sortByColumn(0, Qt::SortOrder::AscendingOrder);
        m_landmark_tv->resizeColumnsToContents();

        connect(m_landmark_tv->selectionModel(), &QItemSelectionModel::currentRowChanged,
                this, &MainWindow::landmarkSelectionChanged);

        connect(m_glview, &MainopenGLView::isovalue_changed, this, &MainWindow::isovalue_changed);

        m_title_template = windowTitle();

        connect(m_glview, &MainopenGLView::availabledata_changed, this, &MainWindow::availabledata_changed);

        availabledata_changed();
}

void MainWindow::landmarkSelectionChanged(const QModelIndex& idx, const QModelIndex& other_idx)
{
        Q_UNUSED(other_idx);
        auto mapped_index = m_landmark_sort_proxy->mapToSource(idx);
        m_glview->selected_landmark_changed(mapped_index.row());
        const Landmark& lm = m_current_landmarklist->at(mapped_index.row());
        if (lm.has(Landmark::lm_picfile)) {
                QString imagefile = m_current_landmarklist->get_base_dir() + "/" + lm.get_template_filename();
                auto i = m_image_cache.find(imagefile);
                if (i == m_image_cache.end()) {
                        QPixmap image(imagefile);
                        if (image.isNull())
                                qDebug() << "Error loading "<< imagefile ;
                        m_image_cache[imagefile] = image;
                        m_template_view->setPixmap(image);
                        qDebug() << "Set label pixmap: " << imagefile << "\n";
                }else
                        m_template_view->setPixmap(i->second);
                 m_template_view->show();
        }else
                 m_template_view->hide();

}

void MainWindow::availabledata_changed()
{
        bool dirty = m_current_landmarklist ? m_current_landmarklist->dirty() : false;
        QString new_title = m_title_template.arg(m_volume_name).
                            arg(m_landmarks_name.isEmpty() ? tr("(none)") : m_landmarks_name)
                            .arg((dirty ? "*" : ""));

        // reset the maximum size of the table view

        m_landmark_tv->resizeColumnsToContents();
        int width = 1 + m_landmark_tv->verticalHeader()->width();
        for(int column = 0; column < 2; ++column)
                width +=  m_landmark_tv->columnWidth(column)+1;
        width += m_landmark_tv->verticalScrollBar()->width();

        m_landmark_tv->setMaximumWidth(width);
        m_landmark_tv->setMinimumWidth(width);
        setWindowTitle(new_title);
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
        filetypes << "(";
        for (auto i: file_types)
                filetypes << "*." << i.c_str() << " ";
        filetypes << ")";
        QString filename = QFileDialog::getOpenFileName(this, "Open volume data set", ".", filetypes.str().c_str());
        if (!filename.isEmpty()) {
                try {
                        auto volume = mia::load_image3d(filename.toStdString());
                        m_current_volume = std::make_shared<VolumeData>(volume);
                        auto intensity_range = m_current_volume->get_intensity_range();
                        m_glview->setVolume(m_current_volume);
                        m_iso_slider->setRange(intensity_range.first+1, intensity_range.second);
                        m_iso_slider->setValue((intensity_range.second - intensity_range.first) / 2);
                        QFileInfo fileInfo(filename);
                        m_volume_name = fileInfo.fileName();
                        availabledata_changed();
                }
                catch (std::exception& x) {
                        QMessageBox box(QMessageBox::Information, "Error loading volume data", x.what(),
                                        QMessageBox::Ok);
                        box.exec();
                }
        }
}

void MainWindow::on_action_Add_triggered()
{
        QString prompt(tr("Name:"));

        while (true) {
                bool ok;
                QString name = QInputDialog::getText(this, tr("Add Landmark"),
                                                     prompt, QLineEdit::Normal,
                                                     QString(), &ok);
                if (!ok)
                        break;

                if (!name.isEmpty()) {
                        if (!m_current_landmarklist->has(name)) {
                                PLandmark new_lm = make_shared<Landmark>(name);
                                m_landmark_lm->addLandmark(new_lm);
                                availabledata_changed();
                                break;
                        }else{
                                prompt =QString(tr("Name (") + name + tr(" is already in list):"));
                        }
                }else{
                        prompt =QString(tr("Name (must not be empty):"));
                }
        }
}

void MainWindow::on_action_Open_landmarkset_triggered()
{
        QString filename = QFileDialog::getOpenFileName(this, tr("Open landmark list"),
                                                        ".", "(MIA landmark list *.lmx)");

        if (!filename.isEmpty()) {
                try {
                        m_current_landmarklist = read_landmarklist(filename);
                        m_glview->setLandmarkList(m_current_landmarklist);
                        m_current_landmarklist->set_dirty_flag(false);
                        QFileInfo fileInfo(filename);
                        m_landmarks_name = fileInfo.fileName();
                        availabledata_changed();
                }
                catch (QRuntimeExeption& x) {
                        QMessageBox box(QMessageBox::Information, "Error loading landmarks", x.qwhat(),
                                        QMessageBox::Ok);
                        box.exec();
                }
        }
}

void MainWindow::isovalue_changed()
{
        m_iso_slider->setValue( m_current_volume->get_iso_value() );
}

void MainWindow::on_actionSave_landmark_set_As_triggered()
{
        auto fileName = QFileDialog::getSaveFileName(this, "Save landmark list as", ".", "(MIA landmark list *.lmx)");

        if (!fileName.isEmpty() ) {
                try {
                        if (write_landmarklist(fileName, *m_current_landmarklist))
                                m_current_landmarklist->set_dirty_flag(false);
                        QFileInfo fi(fileName);
                        m_landmarks_name = fi.fileName();
                        m_current_landmarklist->set_filename(fileName);
                        availabledata_changed();
                }
                catch (QRuntimeExeption& x) {
                        QMessageBox box(QMessageBox::Information, "Error saving landmarks", x.qwhat(),
                                        QMessageBox::Ok);
                        box.exec();
                }
        }
}

void MainWindow::closeEvent(QCloseEvent *event)
{
        bool ask = true;
        while (m_current_landmarklist->dirty() && ask) {

                QMessageBox ask_saving(QMessageBox::Question,
                                       tr("Save data before closing application"),
                                       tr("Landmark set not saved, save now?"),
                                       QMessageBox::Yes | QMessageBox::No| QMessageBox::Cancel);

                auto btn = ask_saving.exec();
                switch (btn) {
                case QMessageBox::Yes:
                        if (m_landmarks_name.isEmpty()) {
                                on_actionSave_landmark_set_As_triggered();
                        }else{
                                on_action_Save_Landmark_set_triggered();
                        }
                        continue;
                case QMessageBox::No:
                        ask = false;
                        event->accept();
                        break;
                case QMessageBox::Cancel:
                        ask = false;
                        event->ignore();
                        break;
                default:
                        ask = false;
                }
        }
}

void MainWindow::on_action_Save_Landmark_set_triggered()
{
        auto filename = m_current_landmarklist->get_filename();
        if (!filename.isEmpty()) {
                try {
                        if (write_landmarklist(filename, *m_current_landmarklist))
                                m_current_landmarklist->set_dirty_flag(false);
                        availabledata_changed();
                }
                catch (QRuntimeExeption& x) {
                        QMessageBox box(QMessageBox::Information, tr("Error saving landmarks"), x.qwhat(),
                                        QMessageBox::Ok);
                        box.exec();
                }
                catch (std::runtime_error& x) {
                        QMessageBox box(QMessageBox::Information, tr("Error saving landmarks"), x.what(),
                                        QMessageBox::Ok);
                        box.exec();
                }
        } else {
                on_actionSave_landmark_set_As_triggered();
        }
}

void MainWindow::on_action_Edit_triggered()
{
        auto sm = m_landmark_tv->selectionModel();

        auto current_selection = sm->selection();
        if (current_selection.isEmpty())
                return;

        auto selection = current_selection.at(0).indexes().at(0);
        auto mapped_index = m_landmark_sort_proxy->mapToSource(selection);
        int active_index = mapped_index.row();
        if (active_index < 0)
                return;

        QString active_landmark = m_current_landmarklist->at(active_index).get_name();
        bool ok = true;
        int idx = -1;
        while (ok && idx == -1) {
                auto new_name = QInputDialog::getText(this, tr("Change landmark name from %1").arg(active_landmark),
                                      tr("New name:"), QLineEdit::Normal, active_landmark, &ok);
                if (ok) {
                        idx = m_landmark_lm->renameLandmark(mapped_index, active_landmark, new_name);
                        qDebug() << "new index:" << idx;
                }
        }
        if (ok && idx > 0) {
                auto select_index = m_landmark_lm->index(idx, 0);
                auto mapped_index = m_landmark_sort_proxy->mapFromSource(select_index);
                m_landmark_tv->selectRow(mapped_index.row());
                m_glview->selected_landmark_changed(idx);
                availabledata_changed();
        }
}

void MainWindow::on_action_TakeSnapshot_triggered()
{
        if (m_snapshot_name_prototype.isEmpty()) {
                auto out_dir = QFileDialog::getExistingDirectory(this, tr("Get shapshot output directory"));
                QFileInfo out_dir_name(out_dir);
                if (!out_dir_name.isWritable()) {
                        // error message
                }
                // craate file name base
                m_snapshot_name_prototype = out_dir + "/shapshot_%1.png";

        }

        QString out_file = m_snapshot_name_prototype.arg(m_snapshot_serial_number++);
        m_glview->snapshot(out_file);
}

void MainWindow::on_action_CreateTemplate_triggered()
{
        while (true) {
                auto out_dir = QFileDialog::getExistingDirectory(this, tr("Select template output directory"));

                // cancel pressed
                if (out_dir.isEmpty())
                        break;

                QFileInfo out_dir_name(out_dir);
                if (!out_dir_name.isWritable()) {
                        qWarning() << "Directory '" << out_dir << "' is red only";
                        continue;
                }

                try {
                        QString lm_picfile_name_template("/template_%1.png");
                        for (unsigned i = 0; i < m_current_landmarklist->size(); ++i) {
                                Landmark& lm = m_current_landmarklist->at(i);
                                if (lm.has(Landmark::lm_camera) &&
                                    lm.has(Landmark::lm_iso_value) &&
                                    lm.has(Landmark::lm_location)) {
                                        QString lm_picfile_name = lm_picfile_name_template.arg(lm.get_name());
                                        QString snapshot_name = out_dir + "/" + lm_picfile_name;
                                        m_glview->set_volume_isovalue(lm.get_iso_value());
                                        m_glview->selected_landmark_changed(i);
                                        m_glview->snapshot(snapshot_name);
                                        lm.set_template_image_file(lm_picfile_name);
                                }else{
                                        qDebug() << "No snapshot for" << lm.get_name();
                                }
                        }

                        write_landmarklist(out_dir + "/template.lmx", *m_current_landmarklist);
                        break;
                }
                catch (QRuntimeExeption& x) {
                        QMessageBox box(QMessageBox::Information, tr("Error saving landmarks"), x.qwhat(),
                                        QMessageBox::Ok);
                        box.exec();
                }
                catch (std::runtime_error& x) {
                        QMessageBox box(QMessageBox::Information, tr("Error saving landmarks"), x.what(),
                                        QMessageBox::Ok);
                        box.exec();
                }
        }
}
