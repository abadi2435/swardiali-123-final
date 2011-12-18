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
    virtual void renderDepthScene();

    //meteor-specific methods:
    virtual void randomizeModelTransformations();
    virtual void updateModelPositions();
    virtual float randDecimal();

private:
    QVector<TransformedModel> m_meteors; // vector of all the object models (and their transformations)
    int m_numModels;
};

#endif // SPACESCENE_H
