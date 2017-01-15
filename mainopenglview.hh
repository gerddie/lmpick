#ifndef MAINOPENGLVIEW_HH
#define MAINOPENGLVIEW_HH

#include "volumedata.hh"
#include <QOpenGLWidget>

class RenderingThread;

class MainopenGLView : public QOpenGLWidget
{
public:
        MainopenGLView(QWidget *parent);

        ~MainopenGLView();

        void setVolume(VolumeData::Pointer volume);
private:
        void initializeGL()override;
        void paintGL()override;
        void resizeGL(int w, int h)override;
        void mouseMoveEvent(QMouseEvent *ev) override;
        void mousePressEvent(QMouseEvent *ev) override;
        void mouseReleaseEvent(QMouseEvent *ev) override;
        void wheelEvent(QWheelEvent *ev) override;
	
        RenderingThread *m_rendering;
};

#endif // MAINOPENGLVIEW_HH
