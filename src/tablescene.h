#ifndef TABLESCENE_H
#define TABLESCENE_H

#include "scene.h"

class TableScene : public Scene
{
public:
    TableScene(GLWidget* widget);
    ~TableScene();

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

    TransformedModel m_apple, m_apple2, m_apple3, m_paper, m_table, m_matchbox;
};

#endif // TABLESCENE_H
