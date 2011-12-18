#include "spacescene.h"
#include <iostream>
#include "glwidget.h"

using std::cout;
using std::endl;

static const int MAX_MODELS = 9;

SpaceScene::SpaceScene(GLWidget* widget) : Scene(widget)
{
    m_light1Pos = Vector3(10.f, 30.f, -30.f);
    m_numModels = MAX_MODELS;
}

SpaceScene::~SpaceScene(){
        const_cast<QGLContext *>(m_widget->context())->deleteTexture(m_cubeMap);
        foreach (TransformedModel tm, m_meteors)
            glmDelete(tm.model.model);
}

void SpaceScene::initializeResources()
{
    m_skybox = ResourceLoader::loadSkybox();
    cout << "Loaded skybox..." << endl;

    loadCubeMap();
    cout << "Loaded cube map... " << m_cubeMap << endl;

    loadTextures();

    loadModels();
}

/**
  Load a cube map for the skybox
 **/
void SpaceScene::loadCubeMap()
{
    QList<QFile *> fileList;

    fileList.append(new QFile("./textures/space/posx.jpg"));
    fileList.append(new QFile("./textures/space/negx.jpg"));
    fileList.append(new QFile("./textures/space/posy.jpg"));
    fileList.append(new QFile("./textures/space/negy.jpg"));
    fileList.append(new QFile("./textures/space/posz.jpg"));
    fileList.append(new QFile("./textures/space/negz.jpg"));
    m_cubeMap = ResourceLoader::loadCubeMap(fileList);
}


/**
  Load all the textures!
**/
void SpaceScene::loadTextures() {
    QString filepath;

    filepath = "./textures/meteor_COLOR.jpg";
    m_textures["obj_diffuse"] = ResourceLoader::loadTexture(filepath);
    if (m_textures["obj_diffuse"] == -1) {cout << "Failed to load " << filepath.toUtf8().constData() << "... " << endl;}
    else {cout << "Loaded " << filepath.toUtf8().constData() << "... " << endl;}

    filepath = "./textures/meteor_NRM.jpg";
    m_textures["obj_normal"] = ResourceLoader::loadTexture(filepath);
    if (m_textures["obj_normal"] == -1) {cout << "Failed to load " << filepath.toUtf8().constData() << "... " << endl;}
    else {cout << "Loaded " << filepath.toUtf8().constData() << "... " << endl;}

    filepath = "./textures/meteor_SPEC.jpg";
    m_textures["obj_spec"] = ResourceLoader::loadTexture(filepath);
    if (m_textures["obj_spec"] == -1) {cout << "Failed to load " << filepath.toUtf8().constData() << "... " << endl;}
    else {cout << "Loaded " << filepath.toUtf8().constData() << "... " << endl;}
}

/**
  Load all the models!
**/
void SpaceScene::loadModels() {
    Model mesh = ResourceLoader::loadObjModel("./models/meteor.obj");
    cout << "Loaded object mesh..." << endl;

    for (int i = 0; i < MAX_MODELS; i++) {
        m_meteors.push_back(TransformedModel(mesh, Vector3(0.f,0.f,0.f), Vector3(1.f,1.f,1.f), Vector3(1.0,0.0,0.0), 0.f));
    }
    this->randomizeModelTransformations();
}

void SpaceScene::randomizeModelTransformations() {
    for (int i = 0; i < MAX_MODELS; i++) {
        m_meteors[i].translate = Vector3(-MAX_MODELS + 2*i, 5*randDecimal() - 1, 5*randDecimal() - 1);
        float scaleFactor = 0.5*randDecimal() + 0.75;
        m_meteors[i].scale = Vector3(scaleFactor, scaleFactor, scaleFactor);
        m_meteors[i].rotationAxis = Vector3(2*randDecimal() - 1, 2*randDecimal() - 1, 2*randDecimal() - 1);
        m_meteors[i].rotationDegrees = 360*randDecimal();
        m_meteors[i].dr = randDecimal() * 0.15;
    }
}

void SpaceScene::updateModelPositions() {
    for (int i = 0; i < MAX_MODELS; i++) {
        m_meteors[i].rotationDegrees += m_meteors[i].dr;
    }
}

float SpaceScene::randDecimal() {
    return (rand()%1000)/1000.f;
}

/**
  Renders the scene.  May be called multiple times by paintGL() if necessary.
**/
void SpaceScene::renderDepthScene() { //this is for the depth
    this->updateModelPositions();

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

    for (int i = 0; i < m_numModels; i++) {
        glPushMatrix();
        glTranslatef(m_meteors[i].translate.x, m_meteors[i].translate.y, m_meteors[i].translate.z);
        glScalef(m_meteors[i].scale.x, m_meteors[i].scale.y, m_meteors[i].scale.z);
        glRotatef(m_meteors[i].rotationDegrees, m_meteors[i].rotationAxis.x, m_meteors[i].rotationAxis.y, m_meteors[i].rotationAxis.z);
        glCallList(m_meteors[i].model.idx);
        glPopMatrix();
    }

    m_widget->m_shaderPrograms["depth"]->release();

    // Disable culling, depth testing and cube maps
    glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST); //why?
    glBindTexture(GL_TEXTURE_CUBE_MAP,0);
    glDisable(GL_TEXTURE_CUBE_MAP);
}


/**
  Renders the scene.  May be called multiple times by paintGL() if necessary.
**/
void SpaceScene::renderScene() {

    // Enable depth testing
    glEnable(GL_DEPTH_TEST);
    glClear(GL_DEPTH_BUFFER_BIT);

    // Enable cube maps and draw the skybox
    glEnable(GL_TEXTURE_CUBE_MAP);
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_cubeMap);
    glCallList(m_skybox);

    // Enable culling (back) faces for rendering the dragon
    glEnable(GL_CULL_FACE);

    // Enable 2D texture and draw the dragon
    glEnable(GL_TEXTURE_2D);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, m_textures["obj_diffuse"]);
    glActiveTexture(GL_TEXTURE0);

    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, m_textures["obj_normal"]);
    glActiveTexture(GL_TEXTURE0);

    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, m_textures["obj_spec"]);
    glActiveTexture(GL_TEXTURE0);

    m_widget->m_shaderPrograms["normalmapping"]->bind();
    m_widget->m_shaderPrograms["normalmapping"]->setUniformValue("camera_pos", m_widget->m_camera.getCameraPosition().x, m_widget->m_camera.getCameraPosition().y, m_widget->m_camera.getCameraPosition().z);
    m_widget->m_shaderPrograms["normalmapping"]->setUniformValue("light_pos", m_light1Pos.x, m_light1Pos.y, m_light1Pos.z);
    m_widget->m_shaderPrograms["normalmapping"]->setUniformValue("diffuse_map", GLint(1));
    m_widget->m_shaderPrograms["normalmapping"]->setUniformValue("normal_map", GLint(2));
    m_widget->m_shaderPrograms["normalmapping"]->setUniformValue("specular_map", GLint(3));
    m_widget->m_shaderPrograms["normalmapping"]->setUniformValue("normal_mapping_active", m_widget->m_useNormalMapping);

    for (int i = 0; i < m_numModels; i++) {
        glPushMatrix();
        glTranslatef(m_meteors[i].translate.x, m_meteors[i].translate.y, m_meteors[i].translate.z);
        glScalef(m_meteors[i].scale.x, m_meteors[i].scale.y, m_meteors[i].scale.z);
        glRotatef(m_meteors[i].rotationDegrees, m_meteors[i].rotationAxis.x, m_meteors[i].rotationAxis.y, m_meteors[i].rotationAxis.z);
        glCallList(m_meteors[i].model.idx);
        glPopMatrix();
    }
    m_widget->m_shaderPrograms["normalmapping"]->release();

    // Disable culling, depth testing and cube maps
    glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);
    glBindTexture(GL_TEXTURE_CUBE_MAP,0);
    glDisable(GL_TEXTURE_CUBE_MAP);
}
