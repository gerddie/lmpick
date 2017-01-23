#include "landmarklistpainter.hh"
#include "landmarklist.hh"
#include "sphere.hh"


struct LandmarkListPainterImpl {

        LandmarkListPainterImpl();

        PLandmarkList m_the_list;
        int m_active_index;

        Sphere m_active_sphere;
        Sphere m_normal_sphere;
        QVector3D m_viewspace_scale;
        QVector3D m_viewspace_shift;

};

LandmarkListPainter::LandmarkListPainter()
{
        impl = new LandmarkListPainterImpl();

}

LandmarkListPainter::~LandmarkListPainter()
{
        delete impl;
}

void LandmarkListPainter::set_viewspace_correction(const QVector3D& scale, const QVector3D& shift)
{
        qDebug() << "Set new scale " << scale << " and shift "  << shift;
        impl->m_viewspace_scale = scale;
        impl->m_viewspace_shift = shift;
}

void LandmarkListPainter::set_landmark_list(PLandmarkList list)
{
        impl->m_the_list = list;
        impl->m_active_index = -1;
}

void LandmarkListPainter::detach_gl(QOpenGLContext& context)
{
        impl->m_active_sphere.detach_gl(context);
        impl->m_normal_sphere.detach_gl(context);
}

void LandmarkListPainter::do_attach_gl(QOpenGLContext& context)
{
        impl->m_active_sphere.attach_gl(context);
        impl->m_normal_sphere.attach_gl(context);
}

void LandmarkListPainter::do_draw(const GlobalSceneState& state, QOpenGLContext& context) const
{
        GlobalSceneState local_state = state;
        for (int i = 0; i < static_cast<int>(impl->m_the_list->size()); ++i) {
                auto lm = (*impl->m_the_list)[i];
                auto offset = lm->get_location() * impl->m_viewspace_scale - impl->m_viewspace_shift;
                qDebug() << "landmark("<< lm->get_name() <<  "):" << offset;
                local_state.set_offset(offset);
                if (i == impl->m_active_index) {
                        impl->m_active_sphere.draw(local_state, context);
                }else{
                        impl->m_normal_sphere.draw(local_state, context);
                }
        }
}

LandmarkListPainterImpl::LandmarkListPainterImpl():
        m_the_list(new LandmarkList),
        m_active_index(-1),
        m_active_sphere(QVector4D(1, 0.5, 0, 0.7)),
        m_normal_sphere(QVector4D(0, 0.5, 1, 0.7)),
        m_viewspace_scale(1,1,1),
        m_viewspace_shift(0,0,0)
{
#if 1
        std::vector<QVector3D> v{{251.8, 140.8 ,128},
                                 {140.8, 251.8,128},
                                 {140.8,140.8, 239},
                                 {140.8,30,128},
                                 {30,140.8,128},
                                 {140.8,140.8, 17}};
        Camera c;
        const char n[][2] = {"a", "b", "c", "d", "e", "f"};

        for(int i = 0; i < 6; ++i) {
                m_the_list->add(PLandmark(new Landmark(n[i], v[i], c)));
        }
        m_active_index = 1;
#endif
}
