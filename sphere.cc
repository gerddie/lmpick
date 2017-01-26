#include "sphere.hh"

#include <QVector3D>
#include <QOpenGLBuffer>
#include <QOpenGLShaderProgram>
#include <QOpenGLVertexArrayObject>
#include <vector>
#include <cassert>

using std::vector;
using std::transform;

struct SphereImpl {

        SphereImpl(float r);

        void attach_gl();

        void create_sphere(float r);

        float m_radius;

        QOpenGLBuffer m_arrayBuf;
        QOpenGLBuffer m_indexBuf;
        QOpenGLShaderProgram m_program;
        QOpenGLVertexArrayObject m_vao;

        int m_light_direction_param;
        int m_view_matrix_param;
        int m_normal_matrix_param;
        int m_base_color_param;
        int m_n_triangles;

        void draw(const GlobalSceneState& state, QOpenGLContext& context, const QVector4D& color);

};


Sphere::Sphere(const QVector4D& color):
        m_base_color(color)
{
        if (!m_instances) {
                impl = new SphereImpl(0.02);
        }
        m_instances++;
}

Sphere::~Sphere()
{
        --m_instances;
        if (!m_instances)
                delete impl;
}

void Sphere::detach_gl(QOpenGLContext& context)
{
        --m_instances_gl_attached;
}

void Sphere::do_attach_gl(QOpenGLContext& context)
{
        if (!m_instances_gl_attached)
                impl->attach_gl();
        ++m_instances_gl_attached;
}

void Sphere::do_draw(const GlobalSceneState& state, QOpenGLContext& context) const
{
        impl->draw(state, context, m_base_color);
}

int Sphere::m_instances = 0;
int Sphere::m_instances_gl_attached = 0;
SphereImpl *Sphere::impl = nullptr;

SphereImpl::SphereImpl(float r):
        m_radius(r),
        m_arrayBuf(QOpenGLBuffer::VertexBuffer),
        m_indexBuf(QOpenGLBuffer::IndexBuffer),
        m_light_direction_param(-1),
        m_view_matrix_param(-1),
        m_normal_matrix_param(-1),
        m_base_color_param(-1),
        m_n_triangles(0)
{
}

struct Triangle {
        Triangle(unsigned short  _a, unsigned short  _b, unsigned short  _c):
                a(_a), b(_b), c(_c){}
        unsigned short a,b,c;
};

struct VNVertex {
        QVector3D v;
        QVector3D n;
};

void SphereImpl::attach_gl()
{

        m_arrayBuf.create();
        m_indexBuf.create();
        m_vao.create();
        m_vao.bind();

        // Initializes cube geometry and transfers it to VBOs
        m_arrayBuf.bind();
        m_indexBuf.bind();

        create_sphere(m_radius);

        if (!m_program.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/shaders/shere_vtx.glsl"))
                qWarning() << "Error compiling ':/shaders/shere_vtx.glsl', view will be clobbered\n";

        if (!m_program.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/shaders/basic_frag.glsl"))
                qWarning() << "Error compiling ':/s makeCurrent();haders/fshader.glsl', view will be clobbered\n";

        if (!m_program.link())
                qWarning() << "Error linking m_view_program', view will be clobbered\n";;


        // Offset for position
        int offset = 0;

        // Tell OpenGL programmable pipeline how to locate vertex position data
        int vertexLocation = m_program.attributeLocation("qt_vertex");
        if (vertexLocation == -1)
                qWarning() << "vertex loction attribute not found";

        m_program.enableAttributeArray(vertexLocation);
        m_program.setAttributeBuffer(vertexLocation, GL_FLOAT, offset, 3, sizeof(VNVertex));


        // Offset for normals
        offset += sizeof(QVector3D);
        int normalLocation = m_program.attributeLocation("qt_normal");
        if (normalLocation != -1) {
                m_program.enableAttributeArray(normalLocation);
                m_program.setAttributeBuffer(normalLocation, GL_FLOAT, offset, 3, sizeof(VNVertex));
        }else
                qWarning() << "'normal' loction attribute not found";

        // get the uniforms locations
        m_light_direction_param = m_program.uniformLocation("qt_light_direction");
        assert(m_light_direction_param != -1);

        m_view_matrix_param = m_program.uniformLocation("qt_view_matrix");
        assert(m_view_matrix_param != -1);

        m_normal_matrix_param = m_program.uniformLocation("qt_normal_matrix");
        assert(m_normal_matrix_param != -1);

        m_base_color_param = m_program.uniformLocation("qt_base_color");
        assert(m_base_color_param != -1);

}


void SphereImpl::create_sphere(float r)
{
        vector<QVector3D> points({{1,0,0}, {0, 1, 0}, {0, 0, 1},
                                  {-1, 0, 0}, {0,-1, 0}, {0, 0,-1}});

        vector<Triangle> triangles({{0,2,1}, {0,4,2}, {0,5,4}, {4,5,3},
                                    {3,2,4}, {2,3,1}, {3,5,1}, {1,5,0}});

        // subdivide the triangles
        const int sub_rounds = 3;
        int point_idx = points.size();
        for (int i = 0; i < sub_rounds; ++i) {

                // subdivide all triangles
                auto nt = triangles.size();
                for (size_t t = 0; t < nt; ++t) {
                        auto triangle =  triangles[t];
                        QVector3D pa = points[triangle.a];
                        QVector3D pb = points[triangle.b];
                        QVector3D pc = points[triangle.c];

                        QVector3D pab = 0.5 * (pa + pb);
                        QVector3D pbc = 0.5 * (pb + pc);
                        QVector3D pca = 0.5 * (pc + pa);

                        pab.normalize();
                        pbc.normalize();
                        pca.normalize();

                        points.push_back(pab);
                        points.push_back(pbc);
                        points.push_back(pca);

                        int ipab = point_idx++;
                        int ipbc = point_idx++;
                        int ipca = point_idx++;

                        // replace original triangle and add the new ones
                        triangles[t] = Triangle(ipab, ipbc, ipca);
                        triangles.push_back(Triangle(triangle.a,  ipab, ipca));
                        triangles.push_back(Triangle(ipab,  triangle.b, ipbc));
                        triangles.push_back(Triangle(ipca,  ipbc, triangle.c));
                }
        }

        vector<VNVertex> vnarray(points.size());
        transform(points.begin(), points.end(), vnarray.begin(), [r](const QVector3D& p){
                VNVertex vn;
                vn.n = -p.normalized();
                vn.v = r * vn.n;
                return vn;
        });
        m_arrayBuf.allocate(&vnarray[0], vnarray.size() * sizeof(VNVertex));
        m_indexBuf.allocate(&triangles[0], triangles.size() * sizeof(Triangle));

        m_n_triangles = triangles.size();

}

void SphereImpl::draw(const GlobalSceneState& state, QOpenGLContext& context, const QVector4D& color)
{
        auto& ogl = *context.functions();
        m_vao.bind();
        QMatrix4x4 modelview = state.get_modelview_matrix();

        m_program.bind();
        m_arrayBuf.bind();
        m_indexBuf.bind();

        m_program.setUniformValue(m_view_matrix_param, state.projection * modelview);
        m_program.setUniformValue(m_normal_matrix_param, modelview.normalMatrix());
        m_program.setUniformValue(m_light_direction_param, state.light_source);
        m_program.setUniformValue(m_base_color_param, color);

        ogl.glEnable(GL_DEPTH_TEST);
        ogl.glDepthFunc(GL_LESS);

        ogl.glEnable(GL_CULL_FACE);
        ogl.glCullFace(GL_BACK);

        ogl.glEnable(GL_BLEND);
        ogl.glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        // Draw cube geometry using indices from VBO 1
        ogl.glDrawElements(GL_TRIANGLES, 3 * m_n_triangles, GL_UNSIGNED_SHORT, 0);

        ogl.glDisable(GL_BLEND);

        m_arrayBuf.release();
        m_indexBuf.release();
        m_vao.release();
}
