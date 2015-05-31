#ifndef FILLEDRENDERER_H
#define FILLEDRENDERER_H

#include "abgabe1.h"
#include "Types3D.h"
#include "limits.h"
#include "math.h"

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

    Varying shadeVertex(Vertex& vertex);
    Varying viewportTransform(Varying& var);
    void rasterizeFace(GdvCanvas& canvas, VaryingTuple& varTup);
    void drawVertex(int x, int y, QVector3D color, GdvCanvas& canvas);
    void sortVaryingTuple(VaryingTuple& varTup);
    bool inView(int x, int y);
};

#endif // FILLEDRENDERER_H
