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

private:
    QVector<TransformedModel*> m_meteors; // vector of all the object models (and their transformations)

    TransformedModel m_orange, m_orange2, m_paper, m_table, m_matchbox, m_cup;
};

#endif // TABLESCENE_H
