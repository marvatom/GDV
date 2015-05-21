#include "abgabe2.h"

FilledRenderer::FilledRenderer()
{

}

FilledRenderer::~FilledRenderer()
{

}

void FilledRenderer::setupGUI(GdvGui &userInterface){
    Abgabe1::setupGUI(userInterface);

}

void FilledRenderer::render(GdvCanvas &canvas){
    //clear buffer and set background color to white
    canvas.clearBuffer(QVector3D(1.0, 1.0, 1.0));

    //sort points with respect to z-coordinate
    qSort(points.begin(), points.end());


    QMatrix4x4 M = createPrtojectionMatrix() * createViewMatrix();
    for (Face f : storedFaces){
        for (int i = 0; i < 3; i++){
            Varyings v = shadeVertex(f[i]);
            QVector4D transformedPointHomog = M * v.position;

            QVector3D transformedPoint = convertFromHomogen(transformedPointHomog);

            //check wether point coordinates are from interval (-1, 1)
            if (transformedPoint.x() < -1 || transformedPoint.x() > 1 ||
                    transformedPoint.y() < -1 || transformedPoint.y() > 1){
                continue;
            }

            scale(transformedPoint);
            drawPixel(transformedPoint, v.color, canvas);
        }
    }

    //flush buffer
    canvas.flipBuffer();
}

void FilledRenderer::meshChanged(const QVector<MeshLoader::Face> &faces){
    storedFaces.clear();

    for (MeshLoader::Face f : faces){
        Face newFace;
        for (int i = 0; i < 3; i++){
            newFace[i].position.setX(f[i].x);
            newFace[i].position.setY(f[i].y);
            newFace[i].position.setZ(f[i].z);
            newFace[i].position.setW(1.0);
            newFace[i].color.setX(f[i].r);
            newFace[i].color.setY(f[i].g);
            newFace[i].color.setZ(f[i].b);
        }
        storedFaces.append(newFace);
    }
}

Varyings FilledRenderer::shadeVertex(Vertex& vertex){
    Varyings var;
    var.color = vertex.color;
    var.position = vertex.position;
    return var;
}

