#ifndef MAINOPENGLVIEW_HH
#define MAINOPENGLVIEW_HH

#include <QOpenGLWidget>

class MainopenGLView : public QOpenGLWidget
{
public:
        MainopenGLView();

private:
        virtual void initializeGL();
        virtual void paintGL();
        virtual void resizeGL(int w, int h);
};

#endif // MAINOPENGLVIEW_HH
