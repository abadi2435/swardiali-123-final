#ifndef SCENE_H
#define SCENE_H

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
class GLWidget;

/*! Store a Model with its transformations. */
struct TransformedModel {
    Model model; //! The object model
    Vector3 translate, scale; //! The scale and translate vectors
    Vector3 rotationAxis; //! The axis of rotation
    float rotationDegrees; //! The number of degrees to rotate
    float dr;

    /*! Default constructor */
    TransformedModel() {}

    /*! Copy constructor */
    TransformedModel(const TransformedModel& tm) :
            model(tm.model), translate(tm.translate), scale(tm.scale), rotationAxis(tm.rotationAxis), rotationDegrees(tm.rotationDegrees) {}

    /*! Constructor */
    TransformedModel(Model model, Vector3 translate, Vector3 scale, Vector3 rotationAxis, float degrees) :
            model(model), translate(translate), scale(scale), rotationAxis(rotationAxis), rotationDegrees(degrees){}
};

class Scene
{
public:
    Scene(GLWidget* widget);
    ~Scene();

    // Initialization code
    virtual void initializeResources() {};
    virtual void loadCubeMap() {};

    virtual void loadTextures() {};
    virtual void loadModels() {};

    virtual void renderScene() {};
    virtual void renderDepthScene();

protected:
    QVector<TransformedModel*> m_allModels;

    Vector3 m_light1Pos;
    GLWidget* m_widget;
    GLuint m_cubeMap; // cubeMap texture ID
    GLuint m_skybox; // skybox call list ID
    QHash<QString, GLuint> m_textures; // hash map of all the texture IDs
};

#endif // SCENE_H
