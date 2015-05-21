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
        bool faceInScreen = true; //true if all three vertices are in the visible area (screen)
        for (int i = 0; i < 3; i++){
            Varyings tmp = shadeVertex(f[i]);

            Varyings* varPtr = viewportTransform(tmp);

            if (varPtr == NULL){
                faceInScreen = false;
                continue;
            }

            varTup[i] = *varPtr;

            //test
            canvas.setPixel(varPtr->position.x(), varPtr->position.y(), varPtr->color);
        }

        //TODO: process varTup
    }

    //flush buffer
    canvas.flipBuffer();
}

Varyings* FilledRenderer::viewportTransform(Varyings &var){
    QVector3D transformedPoint = convertFromHomogen(var.position);

    //check wether point coordinates are from interval (-1, 1)
    if (transformedPoint.x() < -1 || transformedPoint.x() > 1 ||
            transformedPoint.y() < -1 || transformedPoint.y() > 1){
        return NULL;
    }

    scale(transformedPoint);

    var.position.setX(transformedPoint.x());
    var.position.setY(transformedPoint.y());
    var.position.setZ(transformedPoint.z());
    var.position.setW(1.0);

    return &var;
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
    var.position = transformMatrix * vertex.position;


    var.color = vertex.color;
    return var;
}

