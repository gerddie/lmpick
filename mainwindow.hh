#ifndef MAINWINDOW_HH
#define MAINWINDOW_HH

#include <mainopenglview.hh>
#include <QMainWindow>


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

private:
        Ui::MainWindow *ui;
        MainopenGLView *m_glview;
};

#endif // MAINWINDOW_HH
