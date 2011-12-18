#include "scene.h"
#include <iostream>

#include "glwidget.h"

using std::cout;
using std::endl;

Scene::Scene(GLWidget* widget)
{
    m_widget = widget;
    initializeResources();
}

Scene::~Scene()
{
    glDeleteLists(m_skybox, 1);
    const_cast<QGLContext *>(m_widget->context())->deleteTexture(m_cubeMap);
}
