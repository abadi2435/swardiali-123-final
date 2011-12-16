#ifndef GLWIDGET_H
#define GLWIDGET_H

#include <QGLWidget>
#include <QHash>
#include <QString>
#include <QTimer>
#include <QTime>

#include "camera.h"
#include "vector.h"
#include "resourceloader.h"

class QGLShaderProgram;
class QGLFramebufferObject;


class GLWidget : public QGLWidget
{
    Q_OBJECT

public:
    GLWidget(QWidget *parent = 0);
    ~GLWidget();

protected:
    // Overridden QGLWidget methods
    void initializeGL();
    void paintGL();
    void resizeGL(int width, int height);
    void wheelEvent(QWheelEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void keyPressEvent(QKeyEvent *event);

    // Initialization code
    void initializeResources();
    void loadCubeMap();
    void loadDepthCubeMap();
    void createShaderPrograms();
    void createFramebufferObjects(int width, int height);
    void loadTextures();

    // Drawing code
    void applyOrthogonalCamera(float width, float height);
    void applyPerspectiveCamera(float width, float height);
    void renderTexturedQuad(int width, int height, bool flip);
    void renderScene();
    void renderDepthScene();
    void paintText();
    void drawFloor();

private:
    QTimer m_timer;
    QTime m_clock;
    int m_prevTime;
    float m_prevFps, m_fps;
    Vector2 m_prevMousePos;
    OrbitCamera m_camera;

    Vector3 m_light1Pos;

    bool m_useNormalMapping;
    bool m_drawDepthMap;
    float m_focalLength;

    // Resources
    QHash<QString, QGLShaderProgram *> m_shaderPrograms; // hash map of all shader programs
    QHash<QString, QGLFramebufferObject *> m_framebufferObjects; // hash map of all framebuffer objects
    Model m_mesh; // object model
    GLuint m_skybox; // skybox call list ID
    GLuint m_cubeMap; // cubeMap texture ID
    GLuint m_depthCubeMap; // all white cubeMap texture ID

    QHash<QString, GLuint> m_textures; // hash map of all the texture IDs

    QFont m_font; // font for rendering text

};

#endif // GLWIDGET_H
