#include "drawable.hh"

Drawable::Drawable()
{

}

Drawable::~Drawable()
{

}

void Drawable::draw(const GlobalSceneState &state, QOpenGLContext& context) const
{
        do_draw(state, context);
}

void Drawable::attach_gl(QOpenGLContext& context)
{
        do_attach_gl(context);
}
