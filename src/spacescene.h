#ifndef SPACESCENE_H
#define SPACESCENE_H

#include "scene.h"

class SpaceScene : public Scene
{
public:
    SpaceScene(GLWidget* widget);
    ~SpaceScene();

    virtual void initializeResources();

    virtual void loadCubeMap();

    virtual void loadTextures();
    virtual void loadModels();

    virtual void renderScene();

    //meteor-specific methods:
    virtual void randomizeModelTransformations();
    virtual void updateModelPositions();
    virtual float randDecimal();

private:
    QVector<TransformedModel*> m_meteors; // vector of all the object models (and their transformations)

    TransformedModel m_spaceship;
};

#endif // SPACESCENE_H
