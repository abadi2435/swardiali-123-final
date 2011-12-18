#include "tablescene.h"
#include <iostream>
#include "glwidget.h"

using std::cout;
using std::endl;

static const int MAX_MODELS = 9;

TableScene::TableScene(GLWidget* widget) : Scene(widget)
{
    m_light1Pos = Vector3(-13.f, 31.f, 35.f);
}

TableScene::~TableScene(){
    const_cast<QGLContext *>(m_widget->context())->deleteTexture(m_cubeMap);
    foreach (TransformedModel* tm, m_allModels)
        glmDelete(tm->model.model);
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

    filepath = "./textures/orange_COLOR.png";
    m_textures["orange_diffuse"] = ResourceLoader::loadTexture(filepath);
    if (m_textures["orange_diffuse"] == -1) {cout << "Failed to load " << filepath.toUtf8().constData() << "... " << endl;}
    else {cout << "Loaded " << filepath.toUtf8().constData() << "... " << endl;}

    filepath = "./textures/orange_NRM.png";
    m_textures["orange_normal"] = ResourceLoader::loadTexture(filepath);
    if (m_textures["orange_normal"] == -1) {cout << "Failed to load " << filepath.toUtf8().constData() << "... " << endl;}
    else {cout << "Loaded " << filepath.toUtf8().constData() << "... " << endl;}

    filepath = "./textures/orange_SPEC.png";
    m_textures["orange_specular"] = ResourceLoader::loadTexture(filepath);
    if (m_textures["orange_specular"] == -1) {cout << "Failed to load " << filepath.toUtf8().constData() << "... " << endl;}
    else {cout << "Loaded " << filepath.toUtf8().constData() << "... " << endl;}

    filepath = "./models/cup/cup_texture.jpg";
    m_textures["cup_diffuse"] = ResourceLoader::loadTexture(filepath);
    if (m_textures["cup_diffuse"] == -1) {cout << "Failed to load " << filepath.toUtf8().constData() << "... " << endl;}
    else {cout << "Loaded " << filepath.toUtf8().constData() << "... " << endl;}
}

/**
  Load all the models!
**/
void TableScene::loadModels() {

    Model paper = ResourceLoader::loadObjModel("./models/newspaper/newspaper.obj");
    cout << "Loaded newspaper model..." << endl;
    m_paper = TransformedModel(paper, Vector3(0.f, -4.7f, -2.f), Vector3(5.25f, 5.25f, 5.25f), Vector3(0.0,1.0,0.0), -10.f);
    m_allModels.push_back(&m_paper);

    Model table = ResourceLoader::loadObjModel("./models/tbl031.obj");
    cout << "Loaded table model..." << endl;
    m_table = TransformedModel(table, Vector3(0.f, -30.f, 0.f), Vector3(150.25f, 150.25f, 150.25f), Vector3(1.0,0.0,0.0), -90.f);
    m_allModels.push_back(&m_table);

    Model match = ResourceLoader::loadObjModel("./models/matchbox/matchbox.obj");
    cout << "Loaded matchbox model..." << endl;
    m_matchbox = TransformedModel(match, Vector3(0.f, -4.f, 3.f), Vector3(2.25f, 2.25f, 2.25f), Vector3(0.0,1.0,0.0), -40.f);
    m_allModels.push_back(&m_matchbox);

    Model cup = ResourceLoader::loadObjModel("./models/cup/cup.obj");
    cout << "Loaded cup model..." << endl;
    m_cup = TransformedModel(cup, Vector3(-8.f, -2.f, -3.f), Vector3(3.25f, 3.25f, 3.25f), Vector3(0.0,1.0,0.0), 240.f);
    m_allModels.push_back(&m_cup);

    Model orange = ResourceLoader::loadObjModel("./models/orange.obj");
    cout << "Loaded orange model..." << endl;
    m_orange = TransformedModel(orange, Vector3(8.f, -3.5f, 0.f), Vector3(10.25f, 10.25f, 10.25f), Vector3(0.0,1.0,0.0), 0.f);
    m_allModels.push_back(&m_orange);
    m_orange2 = TransformedModel(orange, Vector3(12.f, -3.5f, 2.f), Vector3(10.25f, 10.25f, 10.25f), Vector3(0.0,1.0,0.0), 0.f);
    m_allModels.push_back(&m_orange2);
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
    glBindTexture(GL_TEXTURE_2D, m_textures["orange_diffuse"]);
    glActiveTexture(GL_TEXTURE0);

    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, m_textures["orange_normal"]);
    glActiveTexture(GL_TEXTURE0);

    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, m_textures["orange_specular"]);
    glActiveTexture(GL_TEXTURE0);

    m_widget->m_shaderPrograms["normalmapping"]->bind();
    m_widget->m_shaderPrograms["normalmapping"]->setUniformValue("camera_pos", m_widget->m_camera.getCameraPosition().x, m_widget->m_camera.getCameraPosition().y, m_widget->m_camera.getCameraPosition().z);
    m_widget->m_shaderPrograms["normalmapping"]->setUniformValue("light_pos", m_light1Pos.x, m_light1Pos.y, m_light1Pos.z);
    m_widget->m_shaderPrograms["normalmapping"]->setUniformValue("diffuse_map", GLint(1));
    m_widget->m_shaderPrograms["normalmapping"]->setUniformValue("normal_map", GLint(2));
    m_widget->m_shaderPrograms["normalmapping"]->setUniformValue("specular_map", GLint(3));
    m_widget->m_shaderPrograms["normalmapping"]->setUniformValue("normal_mapping_active", m_widget->m_useNormalMapping);
    m_widget->m_shaderPrograms["normalmapping"]->setUniformValue("light_color", 1, 0.9, .65, 1);
    m_widget->m_shaderPrograms["normalmapping"]->setUniformValue("ks", GLfloat(0.3));
    m_widget->m_shaderPrograms["normalmapping"]->setUniformValue("ka", GLfloat(0.2));
    m_widget->m_shaderPrograms["normalmapping"]->setUniformValue("kd", GLfloat(0.5));
    m_widget->m_shaderPrograms["normalmapping"]->setUniformValue("shininess", GLfloat(20));

    glPushMatrix();
    glTranslatef(m_orange.translate.x, m_orange.translate.y, m_orange.translate.z);
    glScalef(m_orange.scale.x, m_orange.scale.y, m_orange.scale.z);
    glRotatef(m_orange.rotationDegrees, m_orange.rotationAxis.x, m_orange.rotationAxis.y, m_orange.rotationAxis.z);
    glCallList(m_orange.model.idx);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(m_orange2.translate.x, m_orange2.translate.y, m_orange2.translate.z);
    glScalef(m_orange2.scale.x, m_orange2.scale.y, m_orange2.scale.z);
    glRotatef(m_orange2.rotationDegrees, m_orange2.rotationAxis.x, m_orange2.rotationAxis.y, m_orange2.rotationAxis.z);
    glCallList(m_orange2.model.idx);
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
    m_widget->m_shaderPrograms["normalmapping"]->setUniformValue("light_color", 1, 0.9, .65, 1);
    m_widget->m_shaderPrograms["normalmapping"]->setUniformValue("ks", GLfloat(0.2));
    m_widget->m_shaderPrograms["normalmapping"]->setUniformValue("ka", GLfloat(0.2));
    m_widget->m_shaderPrograms["normalmapping"]->setUniformValue("kd", GLfloat(0.5));
    m_widget->m_shaderPrograms["normalmapping"]->setUniformValue("shininess", GLfloat(1));

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
    m_widget->m_shaderPrograms["normalmapping"]->setUniformValue("light_color", 1, 0.9, .65, 1);
    m_widget->m_shaderPrograms["normalmapping"]->setUniformValue("ks", GLfloat(0.2));
    m_widget->m_shaderPrograms["normalmapping"]->setUniformValue("ka", GLfloat(0.2));
    m_widget->m_shaderPrograms["normalmapping"]->setUniformValue("kd", GLfloat(0.5));
    m_widget->m_shaderPrograms["normalmapping"]->setUniformValue("shininess", GLfloat(5));

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
    m_widget->m_shaderPrograms["normalmapping"]->setUniformValue("light_color", 1, 0.9, .65, 1);
    m_widget->m_shaderPrograms["normalmapping"]->setUniformValue("ks", GLfloat(0.2));
    m_widget->m_shaderPrograms["normalmapping"]->setUniformValue("ka", GLfloat(0.2));
    m_widget->m_shaderPrograms["normalmapping"]->setUniformValue("kd", GLfloat(0.5));
    m_widget->m_shaderPrograms["normalmapping"]->setUniformValue("shininess", GLfloat(3));

    glPushMatrix();
    glTranslatef(m_matchbox.translate.x, m_matchbox.translate.y, m_matchbox.translate.z);
    glScalef(m_matchbox.scale.x, m_matchbox.scale.y, m_matchbox.scale.z);
    glRotatef(m_matchbox.rotationDegrees, m_matchbox.rotationAxis.x, m_matchbox.rotationAxis.y, m_matchbox.rotationAxis.z);
    glCallList(m_matchbox.model.idx);
    glPopMatrix();

    m_widget->m_shaderPrograms["normalmapping"]->release();
    
    //-----------

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, m_textures["cup_diffuse"]);
    glActiveTexture(GL_TEXTURE0);this->

    m_widget->m_shaderPrograms["normalmapping"]->bind();
    m_widget->m_shaderPrograms["normalmapping"]->setUniformValue("camera_pos", m_widget->m_camera.getCameraPosition().x, m_widget->m_camera.getCameraPosition().y, m_widget->m_camera.getCameraPosition().z);
    m_widget->m_shaderPrograms["normalmapping"]->setUniformValue("light_pos", m_light1Pos.x, m_light1Pos.y, m_light1Pos.z);
    m_widget->m_shaderPrograms["normalmapping"]->setUniformValue("diffuse_map", GLint(1));
    m_widget->m_shaderPrograms["normalmapping"]->setUniformValue("normal_mapping_active", false);
    m_widget->m_shaderPrograms["normalmapping"]->setUniformValue("light_color", 1, 0.9, .65, 1);
    m_widget->m_shaderPrograms["normalmapping"]->setUniformValue("ks", GLfloat(0.2));
    m_widget->m_shaderPrograms["normalmapping"]->setUniformValue("ka", GLfloat(0.2));
    m_widget->m_shaderPrograms["normalmapping"]->setUniformValue("kd", GLfloat(0.5));
    m_widget->m_shaderPrograms["normalmapping"]->setUniformValue("shininess", GLfloat(3));

    glPushMatrix();
    glTranslatef(m_cup.translate.x, m_cup.translate.y, m_cup.translate.z);
    glScalef(m_cup.scale.x, m_cup.scale.y, m_cup.scale.z);
    glRotatef(m_cup.rotationDegrees, m_cup.rotationAxis.x, m_cup.rotationAxis.y, m_cup.rotationAxis.z);
    glCallList(m_cup.model.idx);
    glPopMatrix();

    m_widget->m_shaderPrograms["normalmapping"]->release();
    
    //-----------

    // Disable culling, depth testing and cube maps
    glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);
    glBindTexture(GL_TEXTURE_CUBE_MAP,0);
    glDisable(GL_TEXTURE_CUBE_MAP);
}
