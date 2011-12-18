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

#include "src/scene.h"
#include "src/spacescene.h"

class QGLShaderProgram;
class QGLFramebufferObject;



class GLWidget : public QGLWidget
{
    Q_OBJECT

public:
    GLWidget(QWidget *parent = 0);
    ~GLWidget();

    QTime m_clock;

    // Resources
    QHash<QString, QGLShaderProgram *> m_shaderPrograms; // hash map of all shader programs
    QHash<QString, QGLFramebufferObject *> m_framebufferObjects; // hash map of all framebuffer objects
    GLuint m_depthCubeMap; // all white cubeMap texture ID
    GLuint m_depthCubeMapFocused; // all black cubeMap texture ID

    OrbitCamera m_camera;

    bool m_useNormalMapping;
    bool m_drawDepthMap;
    float m_focalLength;
    bool m_useDepthOfField;
    float m_zfocus;

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
    void loadDepthCubeMap();
    void loadDepthCubeMapFocused();
    void createShaderPrograms();
    void createFramebufferObjects(int width, int height);

    // Drawing code
    void applyOrthogonalCamera(float width, float height);
    void applyPerspectiveCamera(float width, float height);
    void renderTexturedQuad(int width, int height, bool flip);
    void paintText();

    void randomizeModelTransformations();
    void updateModelPositions();
    float randDecimal();

private:
    QTimer m_timer;
    int m_prevTime;
    float m_prevFps, m_fps;
    Vector2 m_prevMousePos;

    Scene* m_activeScene;
    Scene* m_inactiveScene;

    QFont m_font; // font for rendering text

};

#endif // GLWIDGET_H
