#include "drawable.hh"

Drawable::Drawable()
{

}

Drawable::~Drawable()
{

}

void Drawable::draw(const GlobalSceneState &state, QOpenGLFunctions& ogl) const
{
        m_vao.bind();
        do_draw(state, ogl);
        m_vao.release();
}

void Drawable::attach_gl()
{
        m_vao.create();
        m_vao.bind();
        do_attach_gl();
}
