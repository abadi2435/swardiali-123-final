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

    m_useNormalMapping = true;
    m_drawDepthMap = false;
    m_focalLength = 5.0;
    m_zfocus = .5;
    m_useDepthOfField = false;
    m_flyMode = false;
    m_displayHelp = false;
    m_paused = false;

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

    loadDepthCubeMap();
    cout << "Loaded blurry depth cube map..." << endl;

    loadDepthCubeMapFocused();
    cout << "Loaded focused depth cube map..." << endl;

    createShaderPrograms();
    cout << "Loaded shader programs..." << endl;

    createFramebufferObjects(width(), height());
    cout << "Loaded framebuffer objects..." << endl;

    m_activeScene = new TableScene(this);
    m_inactiveScene = new SpaceScene(this);
    m_activeScene->initializeResources();
    m_inactiveScene->initializeResources();

    cout<< "Loaded active scene..." << endl;

    cout << " --- Finish Loading Resources ---" << endl;
}



/**
  Load a pure white cube box for rendering the background blurry
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
  Load a pure white cube box for rendering the background in focus.
 **/
void GLWidget::loadDepthCubeMapFocused()
{
    QList<QFile *> fileList;
    QFile* black = new QFile("./textures/astra/black.jpeg");
    fileList.append(black);
    fileList.append(black);
    fileList.append(black);
    fileList.append(black);
    fileList.append(black);
    fileList.append(black);
    m_depthCubeMapFocused = ResourceLoader::loadCubeMap(fileList);
}

/**
  Create shader programs.
 **/
void GLWidget::createShaderPrograms()
{
    const QGLContext *ctx = context();
    m_shaderPrograms["normalmapping"] = ResourceLoader::newShaderProgram(ctx, "./shaders/normalmapping.vert",                                                                         "./shaders/normalmapping.frag");

    m_shaderPrograms["dblur"] = ResourceLoader::newFragShaderProgram(ctx, "./shaders/dblur.frag");

    m_shaderPrograms["depth"] = ResourceLoader::newShaderProgram(ctx, "./shaders/depth.vert",
                                                                 "./shaders/depth.frag");
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
    glLoadIdentity();  //  void drawFloor();
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
    m_activeScene->renderDepthScene();                                                                                     //TODO: change this!
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
        m_activeScene->renderScene();
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
            m_shaderPrograms["dblur"]->setUniformValue("focalLength", (float) m_focalLength);

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

    if (!m_paused) {
        m_activeScene->moveModels();
    }

    if (m_flyMode) {
        m_camera.theta -= 0.001;
    }

    // Display the FPS
    paintText();
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

    if (event->button() ==  2) {
        QImage image = m_framebufferObjects["fbo_0"]->toImage();
        QRgb color = image.pixel(m_prevMousePos.x, m_prevMousePos.y);
        if (qGreen((float) ((float)color))/255.0 > 0.2f) {                                                           //if depth away is close to 0, black and sharp.. clicking sould stay sharp
            m_zfocus = qRed((float) ((float)color))/255.0;
        }
    }
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
    case Qt::Key_F12:
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
            m_zfocus +=.005;
            if (m_zfocus > 1){
                m_zfocus = 1;
            }
            break;
        }
    case Qt::Key_Down:
        {
            m_zfocus -=.005;
            if (m_zfocus < 0){
                m_zfocus = 0;
            }
            break;
        }
    case Qt::Key_Left:
        {
            m_focalLength += 0.1;
            if (m_focalLength > 12) {
                m_focalLength = 12;
            }
            break;
        }
    case Qt::Key_Right:
        {
            m_focalLength -= 0.1;
            if (m_focalLength < .1) {
                m_focalLength = 0.1;
            }
            break;
        }
   case Qt::Key_B:
        {
            m_useDepthOfField = !m_useDepthOfField;
            break;
        }
    case Qt::Key_F:
        {
            m_flyMode = !m_flyMode;
            break;
        }
    case Qt::Key_H:
        {
            m_displayHelp = !m_displayHelp;
            break;
        }
    case Qt::Key_P:
        {
            m_paused = !m_paused;
            break;
        }
    case Qt::Key_S:
        Scene* temp = m_activeScene;
        m_activeScene = m_inactiveScene;
        m_inactiveScene = temp;
        break;
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

    if (m_displayHelp) {
        // QGLWidget's renderText takes xy coordinates, a string, and a font
        renderText(10, 20, "FPS: " + QString::number((int) (m_prevFps)), m_font);
        renderText(10, 35, "F12: Save screenshot", m_font);
        renderText(10, 50, "N: Toggle normal/specular mapping: " + QString::number((int) (m_useNormalMapping)), m_font);
        renderText(10, 65, "D: Draw depth map on/off", m_font);
        renderText(10, 80, "B: Toggle depth of field", m_font);
        renderText(10, 95, "Left/Right: Change depth of field size = " + QString::number(m_focalLength), m_font);
        renderText(10, 110, "Up/Down: Change focal distance = " + QString::number(m_zfocus), m_font);
        renderText(10, 125, "F: Toggle fly mode", m_font);
        renderText(10, 140, "P: Pause/Unpause", m_font);
        renderText(10, 155, "S: Toggle Scene", m_font);
    } else {
        renderText(10, 20, "Press H for help.", m_font);
    }

}
