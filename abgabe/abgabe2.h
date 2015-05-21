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
    QMatrix4x4 transformMatrix;

    Varyings shadeVertex(Vertex& vertex);
    Varyings* viewportTransform(Varyings& var);
};

#endif // FILLEDRENDERER_H
