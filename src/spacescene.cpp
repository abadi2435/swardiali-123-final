#include "spacescene.h"
#include <iostream>
#include "glwidget.h"

using std::cout;
using std::endl;

static const int MAX_MODELS = 9;

SpaceScene::SpaceScene(GLWidget* widget) : Scene(widget)
{
    m_light1Pos = Vector3(10.f, 30.f, -30.f);
}

SpaceScene::~SpaceScene(){
        const_cast<QGLContext *>(m_widget->context())->deleteTexture(m_cubeMap);
        foreach (TransformedModel* tm, m_meteors)
            glmDelete(tm->model.model);
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

    filepath = "./models/fighter/ghoul_map_jpg.jpg";
    m_textures["fighter_diffuse"] = ResourceLoader::loadTexture(filepath);
    if (m_textures["fighter_diffuse"] == -1) {cout << "Failed to load " << filepath.toUtf8().constData() << "... " << endl;}
    else {cout << "Loaded " << filepath.toUtf8().constData() << "... " << endl;}
}

/**
  Load all the models!
**/
void SpaceScene::loadModels() {
    Model mesh = ResourceLoader::loadObjModel("./models/meteor.obj");
    cout << "Loaded object mesh..." << endl;

    for (int i = 0; i < MAX_MODELS; i++) {
        TransformedModel* t = new TransformedModel(mesh, Vector3(0.f,0.f,0.f), Vector3(1.f,1.f,1.f), Vector3(1.0,0.0,0.0), 0.f);
        m_allModels.push_back(t);
        m_meteors.push_back(t);
    }
    this->randomizeModelTransformations();

    Model ship = ResourceLoader::loadObjModel("./models/fighter/GhoulOBJ.obj");
    m_spaceship = TransformedModel(ship, Vector3(0.f, 3.f, 0.f), Vector3(0.25f, 0.25f, 0.25f), Vector3(0.0,1.0,0.0), 0.f);
    m_allModels.push_back(&m_spaceship);
    m_spaceship2 = TransformedModel(ship, Vector3(0.f, 3.3f, 0.f), Vector3(0.25f, 0.25f, 0.25f), Vector3(0.0,1.0,0.0), 0.f);
    m_allModels.push_back(&m_spaceship2);
    m_spaceship3 = TransformedModel(ship, Vector3(0.f, 2.7f, 0.f), Vector3(0.25f, 0.25f, 0.25f), Vector3(0.0,1.0,0.0), 0.f);
    m_allModels.push_back(&m_spaceship3);
}

void SpaceScene::randomizeModelTransformations() {
    for (int i = 0; i < m_meteors.size(); i++) {
        m_meteors[i]->translate = Vector3(-MAX_MODELS + 2*i, 5*randDecimal() - 1, 5*randDecimal() - 1);
        float scaleFactor = 0.5*randDecimal() + 0.75;
        m_meteors[i]->scale = Vector3(scaleFactor, scaleFactor, scaleFactor);
        m_meteors[i]->rotationAxis = Vector3(2*randDecimal() - 1, 2*randDecimal() - 1, 2*randDecimal() - 1);
        m_meteors[i]->rotationDegrees = 360*randDecimal();
        m_meteors[i]->dr = randDecimal() * 0.15;
    }
}

void SpaceScene::updateModelPositions() {
    for (int i = 0; i < MAX_MODELS; i++) {
        m_meteors[i]->rotationDegrees += m_meteors[i]->dr;
    }
}

float SpaceScene::randDecimal() {
    return (rand()%1000)/1000.f;
}

void SpaceScene::moveSpaceShip() {
    float theta = (m_widget->m_clock.elapsed() % 100000) / (100000.f/(2*M_PI));

    m_spaceship.translate.x = -10*cos(theta);
    m_spaceship.translate.z = 10*sin(theta);

    //m_widget->m_camera.theta = theta - M_PI/2;

    m_spaceship2.translate.x = -10*cos(theta - 0.05);
    m_spaceship2.translate.z = 10*sin(theta - 0.05);

    m_spaceship3.translate.x = -10*cos(theta - 0.05);
    m_spaceship3.translate.z = 10*sin(theta - 0.05);

    m_spaceship.rotationDegrees = (theta * 180/M_PI) - 90;
    m_spaceship2.rotationDegrees = ((theta - 0.05) * 180/M_PI) - 90;
    m_spaceship3.rotationDegrees = ((theta - 0.05) * 180/M_PI) - 90;
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

    for (int i = 0; i < m_meteors.size(); i++) {
        glPushMatrix();
        glTranslatef(m_meteors[i]->translate.x, m_meteors[i]->translate.y, m_meteors[i]->translate.z);
        glScalef(m_meteors[i]->scale.x, m_meteors[i]->scale.y, m_meteors[i]->scale.z);
        glRotatef(m_meteors[i]->rotationDegrees, m_meteors[i]->rotationAxis.x, m_meteors[i]->rotationAxis.y, m_meteors[i]->rotationAxis.z);
        glCallList(m_meteors[i]->model.idx);
        glPopMatrix();
    }
    m_widget->m_shaderPrograms["normalmapping"]->release();


    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, m_textures["fighter_diffuse"]);
    glActiveTexture(GL_TEXTURE0);this->

    m_widget->m_shaderPrograms["normalmapping"]->bind();
    m_widget->m_shaderPrograms["normalmapping"]->setUniformValue("camera_pos", m_widget->m_camera.getCameraPosition().x, m_widget->m_camera.getCameraPosition().y, m_widget->m_camera.getCameraPosition().z);
    m_widget->m_shaderPrograms["normalmapping"]->setUniformValue("light_pos", m_light1Pos.x, m_light1Pos.y, m_light1Pos.z);
    m_widget->m_shaderPrograms["normalmapping"]->setUniformValue("diffuse_map", GLint(1));
    m_widget->m_shaderPrograms["normalmapping"]->setUniformValue("normal_mapping_active", false);

    glPushMatrix();
    glTranslatef(m_spaceship.translate.x, m_spaceship.translate.y, m_spaceship.translate.z);
    glScalef(m_spaceship.scale.x, m_spaceship.scale.y, m_spaceship.scale.z);
    glRotatef(m_spaceship.rotationDegrees, m_spaceship.rotationAxis.x, m_spaceship.rotationAxis.y, m_spaceship.rotationAxis.z);
    glCallList(m_spaceship.model.idx);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(m_spaceship2.translate.x, m_spaceship2.translate.y, m_spaceship2.translate.z);
    glScalef(m_spaceship2.scale.x, m_spaceship2.scale.y, m_spaceship2.scale.z);
    glRotatef(m_spaceship2.rotationDegrees, m_spaceship2.rotationAxis.x, m_spaceship2.rotationAxis.y, m_spaceship2.rotationAxis.z);
    glCallList(m_spaceship2.model.idx);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(m_spaceship3.translate.x, m_spaceship3.translate.y, m_spaceship3.translate.z);
    glScalef(m_spaceship3.scale.x, m_spaceship3.scale.y, m_spaceship3.scale.z);
    glRotatef(m_spaceship3.rotationDegrees, m_spaceship3.rotationAxis.x, m_spaceship3.rotationAxis.y, m_spaceship3.rotationAxis.z);
    glCallList(m_spaceship3.model.idx);
    glPopMatrix();

    m_widget->m_shaderPrograms["normalmapping"]->release();

    // Disable culling, depth testing and cube maps
    glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);
    glBindTexture(GL_TEXTURE_CUBE_MAP,0);
    glDisable(GL_TEXTURE_CUBE_MAP);
}

void SpaceScene::moveModels() {
    this->updateModelPositions();
    this->moveSpaceShip();
}
