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

    //calculate the transformation matrix
    transformMatrix = createPrtojectionMatrix() * createViewMatrix();

    for (Face f : storedFaces){
        VaryingTuple varTup;
        for (int i = 0; i < 3; i++){
            Varying tmp = shadeVertex(f[i]);

            Varying var = viewportTransform(tmp);

            varTup[i] = var;

            //test
            canvas.setPixel(var.position.x(), var.position.y(), var.color);
        }

        rasterizeFace(canvas, varTup);
    }

    //flush buffer
    canvas.flipBuffer();
}

Varying FilledRenderer::viewportTransform(Varying &var){
    //scale vartex to screen size
    var.position.setX((var.position.x() + 1) * viewWidth / 2);
    var.position.setY((var.position.y() + 1) * viewHeight / 2);

    return var;
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

Varying FilledRenderer::shadeVertex(Vertex& vertex){
    Varying var;
    var.position = transformMatrix * vertex.position;
    QVector3D transformedPoint = convertFromHomogen(var.position);
    var.position.setX(transformedPoint.x());
    var.position.setY(transformedPoint.y());
    var.position.setZ(transformedPoint.z());
    var.position.setW(1.0);

    var.color = vertex.color;
    return var;
}

void FilledRenderer::rasterizeFace(GdvCanvas& canvas, VaryingTuple& varTup){

}
