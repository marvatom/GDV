#ifndef TYPES3D_H
#define TYPES3D_H

#include "QVector4D"
#include "QVector3D"
#include "interfaces/Tuple3.h"

struct Vertex{
    QVector4D position;
    QVector3D color;
};

typedef Tuple3<Vertex> Face;

struct Varyings{
    QVector4D position;
    QVector3D color;
};

typedef Tuple3<Varyings> VaryingTuple;
#endif // TYPES3D_H

