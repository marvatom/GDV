#ifndef ABGABE3C_H
#define ABGABE3C_H

#import "abgabe2.h"

class TexturedRenderer : public FilledRenderer
{
public:
    TexturedRenderer();
    ~TexturedRenderer();

    virtual void textureChanged(QImage texture);
    virtual void meshChanged(const QVector<MeshLoader::Face>& faces);
    virtual void setupGUI(GdvGui& userInterface);

protected:
    virtual Fragment shadePixel(Varying& point);
    virtual Varying shadeVertex(Vertex& vertex);
    virtual Varying interpolate(Tuple3<float> weights, VaryingTuple& triangle, Varying& point);

    QVector3D filter(float textureX, float textureY);

    QImage storedTexture;
    bool bilinearFilter = false;
};

#endif // ABGABE3C_H
