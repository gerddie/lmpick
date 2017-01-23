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

};

LandmarkListPainter::LandmarkListPainter()
{
        impl = new LandmarkListPainterImpl();

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
                local_state.set_offset(lm->get_location() * impl->m_viewspace_scale);
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
        m_viewspace_scale(1,1,1)
{
#if 1
        std::vector<QVector3D> v{{0.8,0,0}, {0,0.8,0}, {0,0,0.8},
                            {-0.8,0,0}, {0,-0.8,0}, {0,0,-0.8}};
        Camera c;
        const char n[][2] = {"a", "b", "c", "d", "e", "f"};

        for(int i = 0; i < 6; ++i) {
                m_the_list->add(PLandmark(new Landmark(n[i], v[i], c)));
        }
        m_active_index = 2;
#endif
}
