#ifndef MAINOPENGLVIEW_HH
#define MAINOPENGLVIEW_HH

#include <QOpenGLWidget>

class RenderingThread;

class MainopenGLView : public QOpenGLWidget
{
public:
        MainopenGLView(QWidget *parent);

        ~MainopenGLView();
private:
        virtual void initializeGL();
        virtual void paintGL();
        virtual void resizeGL(int w, int h);

        RenderingThread *m_rendering;
};

#endif // MAINOPENGLVIEW_HH