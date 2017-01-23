#include "landmarklistpainter.hh"
#include "landmarklist.hh"


struct LandmarkListPainterImpl {

        LandmarkListPainterImpl();

        Landmarklist m_the_list;
        int m_active_index;

};

LandmarkListPainter::LandmarkListPainter()
{
        impl = new LandmarkListPainterImpl();
}

void LandmarkListPainter::detach_gl(QOpenGLContext& context)
{

}

void LandmarkListPainter::do_attach_gl(QOpenGLContext& context)
{

}

void LandmarkListPainter::do_draw(const GlobalSceneState& state, QOpenGLContext& context) const
{

}

LandmarkListPainterImpl::LandmarkListPainterImpl():
        m_active_index(-1)
{

}
