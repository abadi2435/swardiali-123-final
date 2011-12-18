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

    virtual void moveModels();
    virtual void moveSpaceShip();

private:
    QVector<TransformedModel*> m_meteors; // vector of all the object models (and their transformations)

    TransformedModel m_spaceship, m_spaceship2, m_spaceship3;
};

#endif // SPACESCENE_H
