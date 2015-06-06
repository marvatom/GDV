#ifndef ABGABE1_H
#define ABGABE1_H

#include "interfaces/RendererBase.h"

class Abgabe1 : public RendererBase
{
public:
    Abgabe1();
    virtual ~Abgabe1();

    virtual void setupGUI(GdvGui& userInterface);
    virtual bool usesOpenGL();
    virtual void initialize();
    virtual void render(GdvCanvas& canvas);
    virtual void deinitialize();
    virtual void sizeChanged(unsigned int width, unsigned int height);

    virtual void meshChanged(const QVector<MeshLoader::Face>& faces);
    virtual void textureChanged(QImage texture);

    virtual void mousePressed(int x, int y);
    virtual void mouseMoved(int x, int y);
    virtual void mouseReleased(int x, int y);

    void setCameraVectors();

protected:
    QMatrix4x4 createViewMatrix();
    virtual QMatrix4x4 createPrtojectionMatrix();
    QVector3D convertFromHomogen(QVector4D& v);
    void scale(QVector3D& p);
    void drawPixel(QVector3D& transformedPoint, QVector3D color, GdvCanvas& canvas);
    void makeArcballVector(int screenX, int screenY, QVector3D& vec);


    struct MyPoint
    {
        QVector3D coord;
        float r, g, b;

        bool operator <(const MyPoint& x) const {
            return coord.z() < x.coord.z();
        }
    };
    QVector<MyPoint> points;

    unsigned int viewWidth, viewHeight;

    float camX;
    float camY;
    int camDist;
    QVector3D n;
    QVector3D v;
    QVector3D u;

    bool perspectiveProj;
    int screenDist;

    int xRot;
    int yRot;

    bool arcballActive = false;
    bool arcballSet = false;
    QVector3D arcballP1;
    QVector3D arcballP2;
};

#endif // ABGABE1_H
