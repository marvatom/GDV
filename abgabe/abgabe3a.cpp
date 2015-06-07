#include "abgabe3a.h"

LightRendererGouard::LightRendererGouard()
{
    lightPoint.setX(2.0);
    lightPoint.setY(2.0);
    lightPoint.setZ(2.0);

    lightIntensity = 1.0;
    materialConstant = 1.0;
}

LightRendererGouard::~LightRendererGouard()
{

}

void LightRendererGouard::setupGUI(GdvGui &userInterface){
    FilledRenderer::setupGUI(userInterface);

    userInterface.addSlider("Light intensity", 0, 100, 80, lightIntensityInt);
    userInterface.addSlider("Material constant", 0, 100, 80, materialConstantInt);
    userInterface.addSlider("Light rotation", 0, 360, 0, lightRotation);
    userInterface.addSlider("Light source distance", 0, 10, 2, lightSourceDist);
}

void LightRendererGouard::render(GdvCanvas &canvas){
    lightIntensity = lightIntensityInt / 100.0f;
    materialConstant = materialConstantInt / 100.0f;
    lightPoint.setZ(lightSourceDist);

    float cosy = qCos(qDegreesToRadians((float) lightRotation));
    float siny = qSin(qDegreesToRadians((float) lightRotation));
    QMatrix4x4 MRotY(cosy, 0.0, siny, 0.0,
                     0.0, 1.0, 0.0, 0.0,
                     -siny, 0.0, cosy, 0.0,
                     0.0, 0.0, 0.0, 1.0);
    QVector4D newLightPoint = MRotY * lightPoint.toVector4D();
    lightPoint = newLightPoint.toVector3D();

    FilledRenderer::render(canvas);
}

void LightRendererGouard::meshChanged(const QVector<MeshLoader::Face> &faces){
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

            newFace[i].normal.setX(f[i].nx);
            newFace[i].normal.setY(f[i].ny);
            newFace[i].normal.setZ(f[i].nz);
        }
        storedFaces.append(newFace);
    }
}

Varying LightRendererGouard::shadeVertex(Vertex &vertex){
    Varying var = FilledRenderer::shadeVertex(vertex);

    QVector3D point = vertex.position.toVector3D();

    float d = point.distanceToPoint(lightPoint); //distance to light source
    QVector3D L = lightPoint - point;
    L.normalize();
    float dotProd = QVector3D::dotProduct(vertex.normal, L);
    var.lightIntensity = 0.0;
    if (dotProd > 0){
        float epsilon = 0.1;
        var.lightIntensity = (lightIntensity * materialConstant * dotProd) / (d + epsilon);
    }

    return var;
}

Varying LightRendererGouard::interpolate(Tuple3<float> weights, VaryingTuple &triangle, Varying &point){
    FilledRenderer::interpolate(weights, triangle, point);

    point.lightIntensity = weights[0] * triangle[0].lightIntensity + weights[1] * triangle[1].lightIntensity + weights[2] * triangle[2].lightIntensity;

    return point;
}

Fragment LightRendererGouard::shadePixel(Varying &point){
    Fragment f = FilledRenderer::shadePixel(point);

    f.color.setX(f.color.x() * point.lightIntensity);
    f.color.setY(f.color.y() * point.lightIntensity);
    f.color.setZ(f.color.z() * point.lightIntensity);

    return f;
}

