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

/**
  Renders the scene.  May be called multiple times by paintGL() if necessary.
**/
void Scene::renderDepthScene() { //this is for the depth
    glDisable(GL_BLEND);

    // Enable depth testing
    glEnable(GL_DEPTH_TEST);
    glClear(GL_DEPTH_BUFFER_BIT);

    //Enable cube maps and draw the skybox
    glEnable(GL_TEXTURE_CUBE_MAP);
    if (m_widget->m_zfocus > 0.3 || m_widget->m_focalLength < 0.2 ){
                glBindTexture(GL_TEXTURE_CUBE_MAP, m_widget->m_depthCubeMapFocused);
    }
    else {
                glBindTexture(GL_TEXTURE_CUBE_MAP, m_widget->m_depthCubeMap);
    }
    glCallList(m_skybox);

    // Enable culling (back) faces for rendering the dragon
    glEnable(GL_CULL_FACE);

    // Render the dragon with the refraction shader bound
    glActiveTexture(GL_TEXTURE0);
    m_widget->m_shaderPrograms["depth"]->bind();

    m_widget->m_shaderPrograms["depth"]->setUniformValue("camPosition", m_widget->m_camera.getCameraPosition().x, m_widget->m_camera.getCameraPosition().y,m_widget->m_camera.getCameraPosition().z);
    m_widget->m_shaderPrograms["depth"]->setUniformValue("focalLength", m_widget->m_focalLength);
    m_widget->m_shaderPrograms["depth"]->setUniformValue("zfocus", m_widget->m_zfocus);

    for (int i = 0; i < m_allModels.size(); i++) {
        glPushMatrix();
        glTranslatef(m_allModels[i]->translate.x, m_allModels[i]->translate.y, m_allModels[i]->translate.z);
        glScalef(m_allModels[i]->scale.x, m_allModels[i]->scale.y, m_allModels[i]->scale.z);
        glRotatef(m_allModels[i]->rotationDegrees, m_allModels[i]->rotationAxis.x, m_allModels[i]->rotationAxis.y, m_allModels[i]->rotationAxis.z);
        glCallList(m_allModels[i]->model.idx);
        glPopMatrix();
    }

    m_widget->m_shaderPrograms["depth"]->release();

    // Disable culling, depth testing and cube maps
    glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST); //why?
    glBindTexture(GL_TEXTURE_CUBE_MAP,0);
    glDisable(GL_TEXTURE_CUBE_MAP);
}
