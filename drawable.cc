#include "drawable.hh"

Drawable::Drawable()
{

}

Drawable::~Drawable()
{

}

void Drawable::draw(const GlobalSceneState &state, QOpenGLFunctions& ogl) const
{
        do_draw(state, ogl);
}

void Drawable::attach_gl()
{
        do_attach_gl();
}
