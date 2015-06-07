#ifndef ABGABE3A_H
#define ABGABE3A_H

#include "abgabe2.h"

class LightRendererGouard : public FilledRenderer
{
public:
    LightRendererGouard();
    ~LightRendererGouard();

    virtual void meshChanged(const QVector<MeshLoader::Face>& faces);
    virtual void setupGUI(GdvGui& userInterface);
    virtual void render(GdvCanvas& canvas);

protected:
    virtual Varying shadeVertex(Vertex& vertex);
    virtual Varying interpolate(Tuple3<float> weights, VaryingTuple& triangle, Varying& point);
    virtual Fragment shadePixel(Varying& point);

    QVector3D lightPoint;
    float lightIntensity;
    int lightIntensityInt;
    float materialConstant;
    int materialConstantInt;
    int lightSourceDist;
    int lightRotation;
};

#endif // ABGABE3A_H
