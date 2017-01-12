#include "mainwindow.hh"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
        QMainWindow(parent),
        ui(new Ui::MainWindow)
{
        ui->setupUi(this);
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

}
