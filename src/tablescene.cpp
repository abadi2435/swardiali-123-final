#include "tablescene.h"
#include <iostream>
#include "glwidget.h"

using std::cout;
using std::endl;

static const int MAX_MODELS = 9;

TableScene::TableScene(GLWidget* widget) : Scene(widget)
{
    m_light1Pos = Vector3(10.f, 30.f, -30.f);
}

TableScene::~TableScene(){
        const_cast<QGLContext *>(m_widget->context())->deleteTexture(m_cubeMap);
//        foreach (TransformedModel* tm, m_meteors)
//            glmDelete(tm->model.model);
}

void TableScene::initializeResources()
{
    m_skybox = ResourceLoader::loadSkybox();
    cout << "Loaded skybox..." << endl;

    loadCubeMap();
    cout << "Loaded cube map... " << endl;

    loadTextures();

    loadModels();
}

/**
  Load a cube map for the skybox
 **/
void TableScene::loadCubeMap()
{
    QList<QFile *> fileList;

    fileList.append(new QFile("./textures/hotel2/posx.jpg"));
    fileList.append(new QFile("./textures/hotel2/negx.jpg"));
    fileList.append(new QFile("./textures/hotel2/posy.jpg"));
    fileList.append(new QFile("./textures/hotel2/negy.jpg"));
    fileList.append(new QFile("./textures/hotel2/posz.jpg"));
    fileList.append(new QFile("./textures/hotel2/negz.jpg"));
    m_cubeMap = ResourceLoader::loadCubeMap(fileList);
}


/**
  Load all the textures!
**/
void TableScene::loadTextures() {
    QString filepath;

    filepath = "./models/apples/mpm_F_04_apple_diff.JPG";
    m_textures["apples_diffuse"] = ResourceLoader::loadTexture(filepath);
    if (m_textures["apples_diffuse"] == -1) {cout << "Failed to load " << filepath.toUtf8().constData() << "... " << endl;}
    else {cout << "Loaded " << filepath.toUtf8().constData() << "... " << endl;}

    filepath = "./models/TBL03101.jpg";
    m_textures["table_diffuse"] = ResourceLoader::loadTexture(filepath);
    if (m_textures["table_diffuse"] == -1) {cout << "Failed to load " << filepath.toUtf8().constData() << "... " << endl;}
    else {cout << "Loaded " << filepath.toUtf8().constData() << "... " << endl;}

    filepath = "./models/newspaper/paper1.tga";
    m_textures["newspaper_diffuse"] = ResourceLoader::loadTexture(filepath);
    if (m_textures["newspaper_diffuse"] == -1) {cout << "Failed to load " << filepath.toUtf8().constData() << "... " << endl;}
    else {cout << "Loaded " << filepath.toUtf8().constData() << "... " << endl;}

    filepath = "./models/matchbox/EnglandsGlory.jpg";
    m_textures["matchbox_diffuse"] = ResourceLoader::loadTexture(filepath);
    if (m_textures["matchbox_diffuse"] == -1) {cout << "Failed to load " << filepath.toUtf8().constData() << "... " << endl;}
    else {cout << "Loaded " << filepath.toUtf8().constData() << "... " << endl;}
}

/**
  Load all the models!
**/
void TableScene::loadModels() {

    Model paper = ResourceLoader::loadObjModel("./models/newspaper/newspaper.obj");
    cout << "Loaded newspaper model..." << endl;
    m_paper = TransformedModel(paper, Vector3(0.f, -4.7f, 0.f), Vector3(5.25f, 5.25f, 5.25f), Vector3(0.0,1.0,0.0), -10.f);
    m_allModels.push_back(&m_paper);

    Model table = ResourceLoader::loadObjModel("./models/tbl031.obj");
    cout << "Loaded table model..." << endl;
    m_table = TransformedModel(table, Vector3(0.f, -30.f, 0.f), Vector3(150.25f, 150.25f, 150.25f), Vector3(1.0,0.0,0.0), -90.f);
    m_allModels.push_back(&m_table);

    Model match = ResourceLoader::loadObjModel("./models/matchbox/matchbox.obj");
    cout << "Loaded table model..." << endl;
    m_matchbox = TransformedModel(match, Vector3(0.f, -4.f, 5.f), Vector3(2.25f, 2.25f, 2.25f), Vector3(0.0,1.0,0.0), 40.f);
    m_allModels.push_back(&m_matchbox);

    Model apple = ResourceLoader::loadObjModel("./models/orange.obj");
    cout << "Loaded apple model..." << endl;
    m_apple = TransformedModel(apple, Vector3(0.f, -4.f, 0.f), Vector3(10.25f, 10.25f, 10.25f), Vector3(0.0,1.0,0.0), 0.f);
    m_allModels.push_back(&m_apple);
    m_apple2 = TransformedModel(apple, Vector3(0.f, -4.f, 0.f), Vector3(10.25f, 10.25f, 10.25f), Vector3(0.0,1.0,0.0), 0.f);
    m_allModels.push_back(&m_apple2);
    m_apple3 = TransformedModel(apple, Vector3(0.f, -4.f, 0.f), Vector3(10.25f, 10.25f, 10.25f), Vector3(0.0,1.0,0.0), 0.f);
    m_allModels.push_back(&m_apple3);
}

void TableScene::randomizeModelTransformations() {
//    for (int i = 0; i < m_meteors.size(); i++) {
//        m_meteors[i]->translate = Vector3(-MAX_MODELS + 2*i, 5*randDecimal() - 1, 5*randDecimal() - 1);
//        float scaleFactor = 0.5*randDecimal() + 0.75;
//        m_meteors[i]->scale = Vector3(scaleFactor, scaleFactor, scaleFactor);
//        m_meteors[i]->rotationAxis = Vector3(2*randDecimal() - 1, 2*randDecimal() - 1, 2*randDecimal() - 1);
//        m_meteors[i]->rotationDegrees = 360*randDecimal();
//        m_meteors[i]->dr = randDecimal() * 0.15;
//    }
}

void TableScene::updateModelPositions() {
//    for (int i = 0; i < MAX_MODELS; i++) {
//        m_meteors[i]->rotationDegrees += m_meteors[i]->dr;
//    }
}

float TableScene::randDecimal() {
    return (rand()%1000)/1000.f;
}

void TableScene::moveSpaceShip() {
//    float theta = (m_widget->m_clock.elapsed() % 100000) / (100000.f/(2*M_PI));
//
//    m_apple.translate.x = -10*cos(theta);
//    m_apple.translate.z = 10*sin(theta);
//
//    m_apple2.translate.x = -10*cos(theta - 0.05);
//    m_apple2.translate.z = 10*sin(theta - 0.05);
//
//    m_apple3.translate.x = -10*cos(theta - 0.05);
//    m_apple3.translate.z = 10*sin(theta - 0.05);
//
//    m_apple.rotationDegrees = (theta * 180/M_PI) - 90;
//    m_apple2.rotationDegrees = ((theta - 0.05) * 180/M_PI) - 90;
//    m_apple3.rotationDegrees = ((theta - 0.05) * 180/M_PI) - 90;
}

/**
  Renders the scene.  May be called multiple times by paintGL() if necessary.
**/
void TableScene::renderScene() {

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
    glBindTexture(GL_TEXTURE_2D, m_textures["apples_diffuse"]);
    glActiveTexture(GL_TEXTURE0);this->

    m_widget->m_shaderPrograms["normalmapping"]->bind();
    m_widget->m_shaderPrograms["normalmapping"]->setUniformValue("camera_pos", m_widget->m_camera.getCameraPosition().x, m_widget->m_camera.getCameraPosition().y, m_widget->m_camera.getCameraPosition().z);
    m_widget->m_shaderPrograms["normalmapping"]->setUniformValue("light_pos", m_light1Pos.x, m_light1Pos.y, m_light1Pos.z);
    m_widget->m_shaderPrograms["normalmapping"]->setUniformValue("diffuse_map", GLint(1));
    m_widget->m_shaderPrograms["normalmapping"]->setUniformValue("normal_mapping_active", false);

    glPushMatrix();
    glTranslatef(m_apple.translate.x, m_apple.translate.y, m_apple.translate.z);
    glScalef(m_apple.scale.x, m_apple.scale.y, m_apple.scale.z);
    glRotatef(m_apple.rotationDegrees, m_apple.rotationAxis.x, m_apple.rotationAxis.y, m_apple.rotationAxis.z);
    glCallList(m_apple.model.idx);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(m_apple2.translate.x, m_apple2.translate.y, m_apple2.translate.z);
    glScalef(m_apple2.scale.x, m_apple2.scale.y, m_apple2.scale.z);
    glRotatef(m_apple2.rotationDegrees, m_apple2.rotationAxis.x, m_apple2.rotationAxis.y, m_apple2.rotationAxis.z);
    glCallList(m_apple2.model.idx);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(m_apple3.translate.x, m_apple3.translate.y, m_apple3.translate.z);
    glScalef(m_apple3.scale.x, m_apple3.scale.y, m_apple3.scale.z);
    glRotatef(m_apple3.rotationDegrees, m_apple3.rotationAxis.x, m_apple3.rotationAxis.y, m_apple3.rotationAxis.z);
    glCallList(m_apple3.model.idx);
    glPopMatrix();

    m_widget->m_shaderPrograms["normalmapping"]->release();


    //----------
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, m_textures["newspaper_diffuse"]);
    glActiveTexture(GL_TEXTURE0);this->

    m_widget->m_shaderPrograms["normalmapping"]->bind();
    m_widget->m_shaderPrograms["normalmapping"]->setUniformValue("camera_pos", m_widget->m_camera.getCameraPosition().x, m_widget->m_camera.getCameraPosition().y, m_widget->m_camera.getCameraPosition().z);
    m_widget->m_shaderPrograms["normalmapping"]->setUniformValue("light_pos", m_light1Pos.x, m_light1Pos.y, m_light1Pos.z);
    m_widget->m_shaderPrograms["normalmapping"]->setUniformValue("diffuse_map", GLint(1));
    m_widget->m_shaderPrograms["normalmapping"]->setUniformValue("normal_mapping_active", false);

    glPushMatrix();
    glTranslatef(m_paper.translate.x, m_paper.translate.y, m_paper.translate.z);
    glScalef(m_paper.scale.x, m_paper.scale.y, m_paper.scale.z);
    glRotatef(m_paper.rotationDegrees, m_paper.rotationAxis.x, m_paper.rotationAxis.y, m_paper.rotationAxis.z);
    glCallList(m_paper.model.idx);
    glPopMatrix();

    m_widget->m_shaderPrograms["normalmapping"]->release();
    //-----------


    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, m_textures["table_diffuse"]);
    glActiveTexture(GL_TEXTURE0);this->

    m_widget->m_shaderPrograms["normalmapping"]->bind();
    m_widget->m_shaderPrograms["normalmapping"]->setUniformValue("camera_pos", m_widget->m_camera.getCameraPosition().x, m_widget->m_camera.getCameraPosition().y, m_widget->m_camera.getCameraPosition().z);
    m_widget->m_shaderPrograms["normalmapping"]->setUniformValue("light_pos", m_light1Pos.x, m_light1Pos.y, m_light1Pos.z);
    m_widget->m_shaderPrograms["normalmapping"]->setUniformValue("diffuse_map", GLint(1));
    m_widget->m_shaderPrograms["normalmapping"]->setUniformValue("normal_mapping_active", false);

    glPushMatrix();
    glTranslatef(m_table.translate.x, m_table.translate.y, m_table.translate.z);
    glScalef(m_table.scale.x, m_table.scale.y, m_table.scale.z);
    glRotatef(m_table.rotationDegrees, m_table.rotationAxis.x, m_table.rotationAxis.y, m_table.rotationAxis.z);
    glCallList(m_table.model.idx);
    glPopMatrix();

    m_widget->m_shaderPrograms["normalmapping"]->release();

    //-----------

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, m_textures["matchbox_diffuse"]);
    glActiveTexture(GL_TEXTURE0);this->

    m_widget->m_shaderPrograms["normalmapping"]->bind();
    m_widget->m_shaderPrograms["normalmapping"]->setUniformValue("camera_pos", m_widget->m_camera.getCameraPosition().x, m_widget->m_camera.getCameraPosition().y, m_widget->m_camera.getCameraPosition().z);
    m_widget->m_shaderPrograms["normalmapping"]->setUniformValue("light_pos", m_light1Pos.x, m_light1Pos.y, m_light1Pos.z);
    m_widget->m_shaderPrograms["normalmapping"]->setUniformValue("diffuse_map", GLint(1));
    m_widget->m_shaderPrograms["normalmapping"]->setUniformValue("normal_mapping_active", false);

    glPushMatrix();
    glTranslatef(m_matchbox.translate.x, m_matchbox.translate.y, m_matchbox.translate.z);
    glScalef(m_matchbox.scale.x, m_matchbox.scale.y, m_matchbox.scale.z);
    glRotatef(m_matchbox.rotationDegrees, m_matchbox.rotationAxis.x, m_matchbox.rotationAxis.y, m_matchbox.rotationAxis.z);
    glCallList(m_matchbox.model.idx);
    glPopMatrix();

    m_widget->m_shaderPrograms["normalmapping"]->release();
    //-----------

    // Disable culling, depth testing and cube maps
    glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);
    glBindTexture(GL_TEXTURE_CUBE_MAP,0);
    glDisable(GL_TEXTURE_CUBE_MAP);
}

void TableScene::moveModels() {
    this->updateModelPositions();
    this->moveSpaceShip();
}
