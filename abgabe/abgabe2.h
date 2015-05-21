#ifndef FILLEDRENDERER_H
#define FILLEDRENDERER_H

#include "abgabe1.h"
#include "Types3D.h"

class FilledRenderer : public Abgabe1
{
public:
    FilledRenderer();
    ~FilledRenderer();

    virtual void setupGUI(GdvGui& userInterface);
    virtual void render(GdvCanvas& canvas);
    virtual void meshChanged(const QVector<MeshLoader::Face>& faces);

protected:
    QVector<Face> storedFaces;

    Varyings shadeVertex(Vertex& vertex);
};

#endif // FILLEDRENDERER_H
