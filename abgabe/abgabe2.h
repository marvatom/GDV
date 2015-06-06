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
    virtual void sizeChanged(unsigned int width, unsigned int height);

protected:
    QVector<Face> storedFaces;
    QMatrix4x4 transformMatrix;

    virtual QMatrix4x4 createPrtojectionMatrix();
    Varying shadeVertex(Vertex& vertex);
    Varying viewportTransform(Varying& var);
    void rasterizeFace(GdvCanvas& canvas, VaryingTuple& varTup);
    void drawVertex(int x, int y, Fragment& f, GdvCanvas& canvas);
    void sortVaryingTuple(VaryingTuple& varTup);
    bool inView(int x, int y);
    Tuple3<float> calculateWeights(VaryingTuple& triangle, int x, int y);
    virtual Varying interpolate(Tuple3<float> weights, VaryingTuple& triangle, Varying& point);// = 0;
    virtual Fragment shadePixel(Varying& point);// = 0;

    QVector<float> depthBuffer;
    bool zBuffering = true;
};

#endif // FILLEDRENDERER_H
