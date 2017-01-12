#include "mainopenglview.hh"

MainopenGLView::MainopenGLView()
{
        connect(this, SIGNAL(resized()), this, SLOT(on_resize()));
}

void MainopenGLView::initializeGL()
{

}

void MainopenGLView::paintGL()
{

}

void MainopenGLView::resizeGL(int w, int h)
{

}
