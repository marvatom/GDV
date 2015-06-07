#include "abgabe3b.h"

LightRendererFlat::LightRendererFlat()
{

}

LightRendererFlat::~LightRendererFlat()
{

}


void LightRendererFlat::meshChanged(const QVector<MeshLoader::Face> &faces){
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

        QVector3D a = newFace[0].position.toVector3D();
        QVector3D b= newFace[1].position.toVector3D();
        QVector3D c = newFace[2].position.toVector3D();
        QVector3D normal = QVector3D::crossProduct(b - a, c - a);
        normal.normalize();
        newFace[0].normal = normal;
        newFace[1].normal = normal;
        newFace[2].normal = normal;

        storedFaces.append(newFace);
    }
}

