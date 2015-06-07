#ifndef TYPES3D_H
#define TYPES3D_H

#include "QVector4D"
#include "QVector3D"
#include "interfaces/Tuple3.h"

struct Vertex{
    QVector4D position;
    QVector3D color;

    QVector3D normal;
};

typedef Tuple3<Vertex> Face;

struct Varying{
    QVector4D position;
    QVector3D color;

    float lightIntensity = 0.0;
};

typedef Tuple3<Varying> VaryingTuple;

struct Fragment{
    QVector3D color;
    float depth;
};

#endif // TYPES3D_H

