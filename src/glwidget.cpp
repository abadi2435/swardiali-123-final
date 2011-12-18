#include "glwidget.h"

#include <iostream>
#include <QFileDialog>
#include <QGLFramebufferObject>
#include <QGLShaderProgram>
#include <QMouseEvent>
#include <QTime>
#include <QTimer>
#include <QWheelEvent>
#include "glm.h"

using std::cout;
using std::endl;

extern "C"
{
    extern void APIENTRY glActiveTexture(GLenum);
}

static const int MAX_FPS = 120;
static const int MAX_MODELS = 9;

/**
  Constructor.  Initialize all member variables here.
 **/
GLWidget::GLWidget(QWidget *parent) : QGLWidget(parent),
    m_timer(this), m_prevTime(0), m_prevFps(0.f), m_fps(0.f),
    m_font("Deja Vu Sans Mono", 8, 4)
{
    setFocusPolicy(Qt::StrongFocus);
    setMouseTracking(true);

    m_camera.center = Vector3(0.f, 0.f, 0.f);
    m_camera.up = Vector3(0.f, 1.f, 0.f);
    m_camera.zoom = 3.5f;
    m_camera.theta = M_PI * 1.5f, m_camera.phi = 0.2f;
    m_camera.fovy = 60.f;

    m_light1Pos = Vector3(10.f, 30.f, -30.f);

    m_useNormalMapping = true;
    m_drawDepthMap = false;
    m_focalLength = 10.0;
    m_useDepthOfField = false;
    m_numModels = 9;

    connect(&m_timer, SIGNAL(timeout()), this, SLOT(update()));
}

/**
  Destructor.  Delete any 'new'ed objects here.
 **/
GLWidget::~GLWidget()
{
    foreach (QGLShaderProgram *sp, m_shaderPrograms)
        delete sp;
    foreach (QGLFramebufferObject *fbo, m_framebufferObjects)
        delete fbo;
    glDeleteLists(m_skybox, 1);
    const_cast<QGLContext *>(context())->deleteTexture(m_cubeMap);
    foreach (TransformedModel tm, m_models)
        glmDelete(tm.model.model);
}

/**
  Initialize the OpenGL state and start the drawing loop.
 **/
void GLWidget::initializeGL()
{
    // Set up OpenGL
    glEnable(GL_TEXTURE_2D);

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);

    glDisable(GL_DITHER);

    glDisable(GL_LIGHTING);
    glShadeModel(GL_FLAT);

    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

    // Load resources, including creating shader programs and framebuffer objects
    initializeResources();

    // Start the drawing timer
    m_timer.start(1000.0f / MAX_FPS);
}

/**
  Initialize all resources.
  This includes models, textures, call lists, shader programs, and framebuffer objects.
 **/
void GLWidget::initializeResources()
{
    cout << "Using OpenGL Version " << glGetString(GL_VERSION) << endl << endl;
    // Ideally we would now check to make sure all the OGL functions we use are supported
    // by the video card.  But that's a pain to do so we're not going to.
    cout << "--- Loading Resources ---" << endl;

    m_skybox = ResourceLoader::loadSkybox();
    cout << "Loaded skybox..." << endl;

    loadCubeMap();
    cout << "Loaded cube map... " << m_cubeMap << endl;

    loadDepthCubeMap();
    cout << "Loaded depth cube map..." << m_depthCubeMap << endl;

    loadTextures();

    loadModels();

    createShaderPrograms();
    cout << "Loaded shader programs..." << endl;

    createFramebufferObjects(width(), height());
    cout << "Loaded framebuffer objects..." << endl;

    cout << " --- Finish Loading Resources ---" << endl;
}

/**
  Load a cube map for the skybox
 **/
void GLWidget::loadCubeMap()
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
  Load a pure white cube box for rendering the depths
 **/
void GLWidget::loadDepthCubeMap()
{
    QList<QFile *> fileList;
    QFile* white = new QFile("./textures/astra/white.jpg");
    fileList.append(white);
    fileList.append(white);
    fileList.append(white);
    fileList.append(white);
    fileList.append(white);
    fileList.append(white);
    m_depthCubeMap = ResourceLoader::loadCubeMap(fileList);
}

/**
  Create shader programs.
 **/
void GLWidget::createShaderPrograms()
{
    const QGLContext *ctx = context();
    m_shaderPrograms["normalmapping"] = ResourceLoader::newShaderProgram(ctx, "./shaders/normalmapping.vert",
                                                                         "./shaders/normalmapping.frag");

    m_shaderPrograms["dblur"] = ResourceLoader::newFragShaderProgram(ctx, "./shaders/dblur.frag");

    m_shaderPrograms["depth"] = ResourceLoader::newShaderProgram(ctx, "./shaders/depth.vert",
                                                                 "./shaders/depth.frag");
}

/**
  Load all the textures!
**/
void GLWidget::loadTextures() {
    QString filepath;

    filepath = "./textures/meteor_COLOR.jpg";
    m_textures["obj_diffuse"] = ResourceLoader::loadTexture(filepath);
    if (m_textures["obj_diffuse"] == -1) {cout << "Failed to load " << filepath.toUtf8().constData() << "... " << endl;}
    else {cout << "Loaded " << filepath.toUtf8().constData() << "... " << endl;}

    filepath = "./textures/meteor_NRM.jpg";
    m_textures["obj_normal"] = ResourceLoader::loadTexture(filepath);
    if (m_textures["obj_normal"] == -1) {cout << "Failed to load " << filepath.toUtf8().constData() << "... " << endl;}
    else {cout << "Loaded " << filepath.toUtf8().constData() << "... " << endl;}

    filepath = "./textures/floor_diffuse.jpg";
    m_textures["floor_diffuse"] = ResourceLoader::loadTexture(filepath);
    if (m_textures["floor_diffuse"] == -1) {cout << "Failed to load " << filepath.toUtf8().constData() << "... " << endl;}
    else {cout << "Loaded " << filepath.toUtf8().constData() << "... " << endl;}

    filepath = "./textures/floor_normal.jpg";
    m_textures["floor_normal"] = ResourceLoader::loadTexture(filepath);
    if (m_textures["floor_normal"] == -1) {cout << "Failed to load " << filepath.toUtf8().constData() << "... " << endl;}
    else {cout << "Loaded " << filepath.toUtf8().constData() << "... " << endl;}
}

/**
  Load all the models!
**/
void GLWidget::loadModels() {
    Model mesh = ResourceLoader::loadObjModel("./models/meteor.obj");
    cout << "Loaded object mesh..." << endl;
    for (int i = 0; i < MAX_MODELS; i++) {
        m_models.push_back(TransformedModel(mesh, Vector3(0.f,0.f,0.f), Vector3(1.f,1.f,1.f), Vector3(1.0,0.0,0.0), 0.f));
    }
    this->randomizeModelTransformations();
}

void GLWidget::randomizeModelTransformations() {
    for (int i = 0; i < MAX_MODELS; i++) {
        m_models[i].translate = Vector3(-MAX_MODELS + 2*i, 5*randDecimal() - 1, 5*randDecimal() - 1);
        float scaleFactor = 0.5*randDecimal() + 0.75;
        m_models[i].scale = Vector3(scaleFactor, scaleFactor, scaleFactor);
        m_models[i].rotationAxis = Vector3(2*randDecimal() - 1, 2*randDecimal() - 1, 2*randDecimal() - 1);
        m_models[i].rotationDegrees = 360*randDecimal();
        m_models[i].dr = randDecimal() * 0.15;
    }
}

void GLWidget::updateModelPositions() {
    for (int i = 0; i < MAX_MODELS; i++) {
        m_models[i].rotationDegrees += m_models[i].dr;
    }
}

float GLWidget::randDecimal() {
    return (rand()%1000)/1000.f;
}

/**
  Allocate framebuffer objects.

  @param width: the viewport width
  @param height: the viewport height
 **/
void GLWidget::createFramebufferObjects(int width, int height)
{
    // Allocate the main framebuffer object for rendering the scene to
    // This needs a depth attachment
    m_framebufferObjects["fbo_0"] = new QGLFramebufferObject(width, height, QGLFramebufferObject::Depth,
                                                             GL_TEXTURE_2D, GL_RGB16F_ARB);
    m_framebufferObjects["fbo_0"]->format().setSamples(16);

    m_framebufferObjects["fbo_1"] = new QGLFramebufferObject(width, height, QGLFramebufferObject::Depth,
                                                             GL_TEXTURE_2D, GL_RGB16F_ARB);
    m_framebufferObjects["fbo_1"]->format().setSamples(16);
}

/**
  Called to switch to an orthogonal OpenGL camera.
  Useful for rending a textured quad across the whole screen.

  @param width: the viewport width
  @param height: the viewport height
**/
void GLWidget::applyOrthogonalCamera(float width, float height)
{
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, width, height, 0.f, -1.f, 1.f);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

/**
  Called to switch to a perspective OpenGL camera.

  @param width: the viewport width
  @param height: the viewport height
**/
void GLWidget::applyPerspectiveCamera(float width, float height)
{
    float ratio = ((float) width) / height;
    Vector3 dir(-Vector3::fromAngles(m_camera.theta, m_camera.phi));
    Vector3 eye(m_camera.center - dir * m_camera.zoom);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(m_camera.fovy, ratio, 0.1f, 1000.f);
    gluLookAt(eye.x, eye.y, eye.z, eye.x + dir.x, eye.y + dir.y, eye.z + dir.z,
              m_camera.up.x, m_camera.up.y, m_camera.up.z);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

/**
  Draws the scene to a buffer which is rendered to the screen when this function exits.
 **/
void GLWidget::paintGL()
{
    this->updateModelPositions();

    // Clear the screen
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Update the fps
    int time = m_clock.elapsed();
    m_fps = 1000.f / (time - m_prevTime);
    m_prevTime = time;
    int width = this->width();
    int height = this->height();

    // Render the depth scene to framebuffer0
    m_framebufferObjects["fbo_0"]->bind();
    applyPerspectiveCamera(width, height);
    renderDepthScene();
    m_framebufferObjects["fbo_0"]->release();

    // User can press "d" to toggle drawing the depth map instead of the final scene
    if (m_drawDepthMap) {
        applyOrthogonalCamera(width, height);
        glBindTexture(GL_TEXTURE_2D, m_framebufferObjects["fbo_0"]->texture());
        renderTexturedQuad(width, height, true);
        glBindTexture(GL_TEXTURE_2D, 0);
    } else {

        // Render the normal mapped scene to framebuffer1
        m_framebufferObjects["fbo_1"]->bind();
        applyPerspectiveCamera(width, height);
        renderScene();
        m_framebufferObjects["fbo_1"]->release();

        if (!m_useDepthOfField) {
            applyOrthogonalCamera(width, height);
            glBindTexture(GL_TEXTURE_2D, m_framebufferObjects["fbo_1"]->texture());
            renderTexturedQuad(width, height, true);
            glBindTexture(GL_TEXTURE_2D, 0);
        } else {
            // Bind the depth of field blur shader passing it the window's height and width
            m_shaderPrograms["dblur"]->bind();
            m_shaderPrograms["dblur"]->setUniformValue("height", (float) height);
            m_shaderPrograms["dblur"]->setUniformValue("width", (float) width);

            glActiveTexture(GL_TEXTURE3); // Bind the depth texture to slot 3
            glBindTexture(GL_TEXTURE_2D, m_framebufferObjects["fbo_0"]->texture());
            glActiveTexture(GL_TEXTURE0);

            glActiveTexture(GL_TEXTURE4); // Bind the regular scene texture to slot 4
            glBindTexture(GL_TEXTURE_2D, m_framebufferObjects["fbo_1"]->texture());
            glActiveTexture(GL_TEXTURE0);

            // Pass the depth map and the regular scene to the shader
            m_shaderPrograms["dblur"]->setUniformValue("depthtex", GLint(3));
            m_shaderPrograms["dblur"]->setUniformValue("tex", GLint(4));

            // Draw a quad to the screen
            applyOrthogonalCamera(width,height);
            renderTexturedQuad(width, height, true);

            // Release the shader
            m_shaderPrograms["dblur"]->release();
        }
    }

    // Display the FPS
    paintText();
}

/**
  Renders the scene.  May be called multiple times by paintGL() if necessary.
**/
void GLWidget::renderDepthScene() { //this is for the depth

    glDisable(GL_BLEND);

    // Enable depth testing
    glEnable(GL_DEPTH_TEST);
    glClear(GL_DEPTH_BUFFER_BIT);

    //Enable cube maps and draw the skybox
    glEnable(GL_TEXTURE_CUBE_MAP);
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_depthCubeMap);
    glCallList(m_skybox);

    // Enable culling (back) faces for rendering the dragon
    glEnable(GL_CULL_FACE);

    // Render the dragon with the refraction shader bound
    glActiveTexture(GL_TEXTURE0);
    m_shaderPrograms["depth"]->bind();
    m_shaderPrograms["depth"]->setUniformValue("camPosition", m_camera.getCameraPosition().x, m_camera.getCameraPosition().y, m_camera.getCameraPosition().z);
    m_shaderPrograms["depth"]->setUniformValue("focalLength", m_focalLength);


    for (int i = 0; i < m_numModels; i++) {
        glPushMatrix();
        glTranslatef(m_models[i].translate.x, m_models[i].translate.y, m_models[i].translate.z);
        glScalef(m_models[i].scale.x, m_models[i].scale.y, m_models[i].scale.z);
        glRotatef(m_models[i].rotationDegrees, m_models[i].rotationAxis.x, m_models[i].rotationAxis.y, m_models[i].rotationAxis.z);
        glCallList(m_models[i].model.idx);
        glPopMatrix();
    }

//    // Draw the first object
//    glPushMatrix();
//    glTranslatef(-1.25f,0.f,0.f);
//    glScalef(2.0f, 2.0f, 2.0f);
//    glRotatef(-90.f, 1.0f, 0.f, 0.f);
//    glCallList(m_mesh.idx);
//    glPopMatrix();

    // Draw the second object
    /*glPushMatrix();
    glTranslatef(1.25f,0.f,0.f);
    glScalef(2.0f, 2.0f, 2.0f);
    glRotatef(90.f, 1.0f, 0.f, 0.f);
    glCallList(m_mesh.idx);
    glPopMatrix();*/

    // Draw the floor
    /*glPushMatrix();
    glTranslatef(-10.f, -1.25f, -10.f);
    glScalef(20.f, 0.f, 20.f);
    this->drawFloor();
    glPopMatrix();*/
    m_shaderPrograms["depth"]->release();

    // Disable culling, depth testing and cube maps
    glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST); //why?
    glBindTexture(GL_TEXTURE_CUBE_MAP,0);
    glDisable(GL_TEXTURE_CUBE_MAP);
}


/**
  Renders the scene.  May be called multiple times by paintGL() if necessary.
**/
void GLWidget::renderScene() {

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

    float light_theta = (m_clock.elapsed() % 5000) / (5000.f/(2*M_PI));
    //m_light1Pos.x = 15.f * cos(light_theta);
    //m_light1Pos.y = 15.f + 15.f * sin(light_theta);

    m_shaderPrograms["normalmapping"]->bind();
    m_shaderPrograms["normalmapping"]->setUniformValue("camera_pos", m_camera.getCameraPosition().x, m_camera.getCameraPosition().y, m_camera.getCameraPosition().z);
    m_shaderPrograms["normalmapping"]->setUniformValue("light_pos", m_light1Pos.x, m_light1Pos.y, m_light1Pos.z);
    m_shaderPrograms["normalmapping"]->setUniformValue("diffuse_map", GLint(1));
    m_shaderPrograms["normalmapping"]->setUniformValue("normal_map", GLint(2));
    m_shaderPrograms["normalmapping"]->setUniformValue("normal_mapping_active", m_useNormalMapping);

    for (int i = 0; i < m_numModels; i++) {
        glPushMatrix();
        glTranslatef(m_models[i].translate.x, m_models[i].translate.y, m_models[i].translate.z);
        glScalef(m_models[i].scale.x, m_models[i].scale.y, m_models[i].scale.z);
        glRotatef(m_models[i].rotationDegrees, m_models[i].rotationAxis.x, m_models[i].rotationAxis.y, m_models[i].rotationAxis.z);
        glCallList(m_models[i].model.idx);
        glPopMatrix();
    }

//    glPushMatrix();
//    glTranslatef(-1.25f, 0.f, 0.f);
//    glScalef(2.0f, 2.0f, 2.0f);
//    glRotatef(-90.f, 1.0f, 0.f, 0.f);
//    glCallList(m_mesh.idx);
//    glPopMatrix();

    /*glPushMatrix();
    glTranslatef(1.25f,0.f,0.f);
    glScalef(2.0f, 2.0f, 2.0f);
    glRotatef(90.f, 1.0f, 0.f, 0.f);
    glCallList(m_mesh.idx);
    glPopMatrix();*/

    m_shaderPrograms["normalmapping"]->release();

/*
    //Draw the floor
    glEnable(GL_TEXTURE_2D);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, m_textures["floor_diffuse"]);
    glActiveTexture(GL_TEXTURE0);

    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, m_textures["floor_normal"]);
    glActiveTexture(GL_TEXTURE0);

    m_shaderPrograms["normalmapping"]->bind();
    m_shaderPrograms["normalmapping"]->setUniformValue("cameraPosition", m_camera.getCameraPosition().x, m_camera.getCameraPosition().y, m_camera.getCameraPosition().z);
    m_shaderPrograms["normalmapping"]->setUniformValue("light1Position", m_light1Pos.x, m_light1Pos.y, m_light1Pos.z);
    m_shaderPrograms["normalmapping"]->setUniformValue("diffuseTexture", GLint(1));
    m_shaderPrograms["normalmapping"]->setUniformValue("normalTexture", GLint(2));
    m_shaderPrograms["normalmapping"]->setUniformValue("useNormalMapping", m_useNormalMapping);

    glPushMatrix();
    glTranslatef(-10.f, -1.25f, -10.f);
    glScalef(20.f, 0.f, 20.f);
    this->drawFloor();
    glPopMatrix();

    m_shaderPrograms["normalmapping"]->release(); */

    // Disable culling, depth testing and cube maps
    glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);
    glBindTexture(GL_TEXTURE_CUBE_MAP,0);
    glDisable(GL_TEXTURE_CUBE_MAP);
}

/**
  Draws the floor.
**/
void GLWidget::drawFloor() {
    glBegin(GL_QUADS);
    glNormal3f(0.f, 1.f, 0.f);
    glTexCoord2f(0.f, 0.f);
    glVertex3f(0.f, 0.f, 0.f);

    glNormal3f(0.f, 1.f, 0.f);
    glTexCoord2f(0.f, 1.f);
    glVertex3f(0.f, 0.f, 1.f);

    glNormal3f(0.f, 1.f, 0.f);
    glTexCoord2f(1.f, 1.f);
    glVertex3f(1.f, 0.f, 1.f);

    glNormal3f(0.f, 1.f, 0.f);
    glTexCoord2f(1.f, 0.f);
    glVertex3f(1.f, 0.f, 0.f);

    glEnd();
}

/**
  Called when the mouse is dragged.  Rotates the camera based on mouse movement.
**/
void GLWidget::mouseMoveEvent(QMouseEvent *event)
{
    Vector2 pos(event->x(), event->y());
    if (event->buttons() & Qt::LeftButton || event->buttons() & Qt::RightButton)
    {
        m_camera.mouseMove(pos - m_prevMousePos);
    }
    m_prevMousePos = pos;
}

/**
  Record a mouse press position.
 **/
void GLWidget::mousePressEvent(QMouseEvent *event)
{
    m_prevMousePos.x = event->x();
    m_prevMousePos.y = event->y();
}

/**
  Called when the mouse wheel is turned.  Zooms the camera in and out.
**/
void GLWidget::wheelEvent(QWheelEvent *event)
{
    if (event->orientation() == Qt::Vertical)
    {
        m_camera.mouseWheel(event->delta());
    }
}

/**
  Called when the GLWidget is resized.
 **/
void GLWidget::resizeGL(int width, int height)
{
    // Resize the viewport
    glViewport(0, 0, width, height);

    // Reallocate the framebuffers with the new window dimensions
    foreach (QGLFramebufferObject *fbo, m_framebufferObjects)
    {
        const QString &key = m_framebufferObjects.key(fbo);
        QGLFramebufferObjectFormat format = fbo->format();
        delete fbo;
        m_framebufferObjects[key] = new QGLFramebufferObject(width, height, format);
    }
}

/**
  Draws a textured quad. The texture most be bound and unbound
  before and after calling this method - this method assumes that the texture
  has been bound before hand.

  @param w: the width of the quad to draw
  @param h: the height of the quad to draw
  @param flip: flip the texture vertically
**/
void GLWidget::renderTexturedQuad(int width, int height, bool flip) {
    // Clamp value to edge of texture when texture index is out of bounds
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    // Draw the  quad
    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, flip ? 1.0f : 0.0f);
    glVertex2f(0.0f, 0.0f);
    glTexCoord2f(1.0f, flip ? 1.0f : 0.0f);
    glVertex2f(width, 0.0f);
    glTexCoord2f(1.0f, flip ? 0.0f : 1.0f);
    glVertex2f(width, height);
    glTexCoord2f(0.0f, flip ? 0.0f : 1.0f);
    glVertex2f(0.0f, height);
    glEnd();
}

/**
  Handles any key press from the keyboard
 **/
void GLWidget::keyPressEvent(QKeyEvent *event)
{
    switch(event->key())
    {
        case Qt::Key_S:
        {
            QImage qi = grabFrameBuffer(false);
            QString filter;
            QString fileName = QFileDialog::getSaveFileName(this, tr("Save Image"), "", tr("PNG Image (*.png)"), &filter);
            qi.save(QFileInfo(fileName).absoluteDir().absolutePath() + "/" + QFileInfo(fileName).baseName() + ".png", "PNG", 100);
            break;
        }
        case Qt::Key_N:
        {
            m_useNormalMapping = !m_useNormalMapping;
            break;
        }
        case Qt::Key_D:
        {
            m_drawDepthMap = !m_drawDepthMap;
            break;
        }
        case Qt::Key_Up:
        {
            m_focalLength += 0.1;
            break;
        }
        case Qt::Key_Down:
        {
            m_focalLength -= 0.1;
            break;
        }
        case Qt::Key_B:
        {
            m_useDepthOfField = !m_useDepthOfField;
            break;
        }
    }
}

/**
  Draws text for the FPS and screenshot prompt
 **/
void GLWidget::paintText()
{
    glColor3f(1.f, 1.f, 1.f);

    // Combine the previous and current framerate
    if (m_fps >= 0 && m_fps < 1000)
    {
       m_prevFps *= 0.95f;
       m_prevFps += m_fps * 0.05f;
    }

    // QGLWidget's renderText takes xy coordinates, a string, and a font
    renderText(10, 20, "FPS: " + QString::number((int) (m_prevFps)), m_font);
    renderText(10, 35, "S: Save screenshot", m_font);
    renderText(10, 50, "N: Toggle normal mapping: " + QString::number((int) (m_useNormalMapping)), m_font);
    renderText(10, 65, "D: Draw depth map on/off", m_font);
    renderText(10, 80, "Up/Down: Change focal length = " + QString::number(m_focalLength), m_font);
    renderText(10, 95, "B: Toggle depth of field", m_font);
}
