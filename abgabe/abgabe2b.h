#ifndef ABGABE2B_H
#define ABGABE2B_H

#include "abgabe2.h"

class WaterRenderer : public FilledRenderer
{
public:
    WaterRenderer();
    ~WaterRenderer();
    //virtual void setupGUI(GdvGui& userInterface);
    virtual void render(GdvCanvas& canvas);
    virtual void meshChanged(const QVector<MeshLoader::Face>& faces);
    virtual Varying shadeVertex(Vertex& vertex);

protected:
    //Vertex vertices[21,21];
    float iter = 0.0;
    float actY = 0.0;
};

#endif // ABGABE2B_H
