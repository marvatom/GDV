#include "abgabe1.h"
#include <iostream>

Abgabe1::Abgabe1()
{
    //std::cout << "construct()" << std::endl;
}

Abgabe1::~Abgabe1()
{

}

void Abgabe1::setupGUI(GdvGui &userInterface){
    userInterface.addSlider("Camera distance from origin", 0, 100, 10, camDist);
    userInterface.addSlider("Eyes distance from screen", 0, 100, 20, screenDist);

    userInterface.addCheckBox("Perspective projection", true, perspectiveProj);

    userInterface.addSlider("X-Axe roatation", 0, 360, 0, xRot);
    userInterface.addSlider("Y-Axe roatation", 0, 360, 0, yRot);
}

bool Abgabe1::usesOpenGL(){
    return false;
}

void Abgabe1::initialize(){
    camX = 0.0;
    camY = 0.0;
    makeArcballVector(0, 0, arcballP1);
    makeArcballVector(0, 0, arcballP2);
    setCameraVectors();
}

void Abgabe1::setCameraVectors(){
    n.setX(camX);
    n.setY(camY);
    n.setZ(-camDist);
    n.normalize();

    //setup vector v (v is orthonormal to n)
    QVector3D v1(0.0, 1.0, 0.0);
    QVector3D proj((QVector3D::dotProduct(v1, n) / QVector3D::dotProduct(n, n)) * n);
    v = v1 - proj;
    v.normalize();

    //setup the last basis vector u
    u = QVector3D::crossProduct(n, v);

    /*std::cout << "n = " << n.x() << ", " << n.y() << ", " << n.z() << std::endl;
    std::cout << "v = " << v.x() << ", " << v.y() << ", " << v.z() << std::endl;
    std::cout << "u = " << u.x() << ", " << u.y() << ", " << u.z() << std::endl;
    std::cout << QVector3D::dotProduct(n, v) << std::endl;
    std::cout << QVector3D::dotProduct(n, u) << std::endl;
    std::cout << QVector3D::dotProduct(v, u) << std::endl;*/
}

QMatrix4x4 Abgabe1::createViewMatrix(){
    //setCameraVectors();
    QMatrix4x4 MView;
    QMatrix4x4 basisChange(u.x(), u.y(), u.z(), 0.0,
                           v.x(), v.y(), v.z(), 0.0,
                           n.x(), n.y(), n.z(), 0.0,
                           0.0, 0.0, 0.0, 1.0);
    QMatrix4x4 camTranslation(1.0, 0.0, 0.0, -camX,
                              0.0, 1.0, 0.0, -camY,
                              0.0, 0.0, 1.0, -camDist,
                              0.0, 0.0, 0.0, 1.0);

    /*qDebug() << basisChange;
    qDebug() << camTranslation;*/
    MView = basisChange * camTranslation;

    //apply rotations from GUI sliders
    float cosx = qCos(qDegreesToRadians((float) xRot));
    float sinx = qSin(qDegreesToRadians((float) xRot));
    QMatrix4x4 MRotX(1.0, 0.0, 0.0, 0.0,
                     0.0, cosx, sinx, 0.0,
                     0.0, -sinx, cosx, 0.0,
                     0.0, 0.0, 0.0, 1.0);

    MView = MView * MRotX;
    float cosy = qCos(qDegreesToRadians((float) yRot));
    float siny = qSin(qDegreesToRadians((float) yRot));
    QMatrix4x4 MRotY(cosy, 0.0, siny, 0.0,
                     0.0, 1.0, 0.0, 0.0,
                     -siny, 0.0, cosy, 0.0,
                     0.0, 0.0, 0.0, 1.0);

    MView = MView * MRotY;

    if (!arcballSet){
        return MView;
    }

    //apply arcball rotation
    float angelRad = qAcos(QVector3D::dotProduct(arcballP1, arcballP2) /
                           (arcballP1.length() * arcballP2.length()));
    QVector3D arcballU = QVector3D::crossProduct(arcballP1, arcballP2);
    arcballU.normalize();
    arcballU.setZ(-arcballU.z());

    float d = qSqrt(qPow(arcballU.y(), 2) + qPow(arcballU.z(), 2));
    QMatrix4x4 ArcballT(1.0, 0.0, 0.0, -arcballP1.x(),
                        0.0, 1.0, 0.0, -arcballP1.y(),
                        0.0, 0.0, 1.0, -arcballP1.z(),
                        0.0, 0.0, 0.0, 1.0);
    QMatrix4x4 ArcballTInv(1.0, 0.0, 0.0, arcballP1.x(),
                           0.0, 1.0, 0.0, arcballP1.y(),
                           0.0, 0.0, 1.0, arcballP1.z(),
                           0.0, 0.0, 0.0, 1.0);
    QMatrix4x4 ArcballX(1.0, 0.0, 0.0, 0.0,
                        0.0, arcballU.z()/d, -arcballU.y()/d, 0.0,
                        0.0, arcballU.y()/d, arcballU.z()/d, 0.0,
                        0.0, 0.0, 0.0, 1.0);
    QMatrix4x4 ArcballXInv(1.0, 0.0, 0.0, 0.0,
                        0.0, arcballU.z()/d, arcballU.y()/d, 0.0,
                        0.0, -arcballU.y()/d, arcballU.z()/d, 0.0,
                        0.0, 0.0, 0.0, 1.0);
    QMatrix4x4 ArcballY(d, 0.0, -arcballU.x(), 0.0,
                        0.0, 1.0, 0.0, 0.0,
                        arcballU.x(), 0.0, d, 0.0,
                        0.0, 0.0, 0.0, 1.0);
    QMatrix4x4 ArcballYInv(d, 0.0, arcballU.x(), 0.0,
                        0.0, 1.0, 0.0, 0.0,
                        -arcballU.x(), 0.0, d, 0.0,
                        0.0, 0.0, 0.0, 1.0);
    QMatrix4x4 ArcballZ(qCos(angelRad), -qSin(angelRad), 0.0, 0.0,
                        qSin(angelRad), qCos(angelRad), 0.0, 0.0,
                        0.0, 0.0, 1.0, 0.0,
                        0.0, 0.0, 0.0, 1.0);

    //MView = MView * ArcballTInv * ArcballXInv * ArcballYInv * ArcballZ * ArcballY * ArcballX * ArcballT;
    MView = MView * ArcballXInv * ArcballYInv * ArcballZ * ArcballY * ArcballX;
    return MView;
}

QMatrix4x4 Abgabe1::createPrtojectionMatrix(){
    if (perspectiveProj){
        QMatrix4x4 MProj(1.0, 0.0, 0.0, 0.0,
                         0.0, 1.0, 0.0, 0.0,
                         0.0, 0.0, 1.0, 0.0,
                         0.0, 0.0, 1.0/screenDist, 0.0);
        return MProj;
    }else{
        QMatrix4x4 MProj(1.0, 0.0, 0.0, 0.0,
                         0.0, 1.0, 0.0, 0.0,
                         0.0, 0.0, 0.0, 0.0,
                         0.0, 0.0, 0.0, 1.0);
        return MProj;
    }
}

void Abgabe1::render(GdvCanvas &canvas){
    //clear buffer and set background color to white
    canvas.clearBuffer(QVector3D(1.0, 1.0, 1.0));

    //sort points with respect to z-coordinate
    qSort(points.begin(), points.end());

    //DEBUG
    /*MyPoint p;
    p.coord.setX(0.0);
    p.coord.setY(0.0);
    p.coord.setZ(0.0);
    p.r = 1.0;
    p.g = 0.0;
    p.b = 0.0;
    points.append(p);*/

    QMatrix4x4 M = createPrtojectionMatrix() * createViewMatrix();
    for (MyPoint p : points){
        QVector4D point(p.coord.x(), p.coord.y(), p.coord.z(), 1.0);
        QVector4D transformedPointHomog = M * point;
        QVector3D transformedPoint = convertFromHomogen(transformedPointHomog);
        QVector3D color(p.r, p.g, p.b);

        //check wether point coordinates are from interval (-1, 1)
        if (transformedPoint.x() < -1 || transformedPoint.x() > 1 ||
                transformedPoint.y() < -1 || transformedPoint.y() > 1){
            continue;
        }

        scale(transformedPoint);
        drawPixel(transformedPoint, color, canvas);
    }

    //flush buffer
    canvas.flipBuffer();
}

/*Scales the Point (with coordinates from interval (-1, 1)) to screen size.
 * */
void Abgabe1::scale(QVector3D& p){
    if(p.x() < -1 || p.x() > 1 || p.y() < -1 || p.y() > 1)
        std::cout << "ERROR: POINT OUT OF INTERVAL (" << p.x() << ", " << p.y() << ")" << std::endl;

    p.setX((p.x() + 1) * viewWidth / 2);
    p.setY((p.y() + 1) * viewHeight / 2);
}

void Abgabe1::drawPixel(QVector3D &transformedPoint, QVector3D color, GdvCanvas &canvas){
    for (int i = -1; i <= 1; i++)
        for (int j = -1; j <= 1; j++){
            if (transformedPoint.x() + i >= 0 && transformedPoint.x() + i < viewWidth &&
                    transformedPoint.y() + j >= 0 && transformedPoint.y() + j < viewHeight){
                canvas.setPixel(transformedPoint.x() + i, transformedPoint.y() + j, color);
            }
        }
    if (transformedPoint.y() >= 0 && transformedPoint.y() < viewHeight){
        if (transformedPoint.x() - 2 >= 0)
            canvas.setPixel(transformedPoint.x() - 2, transformedPoint.y(), color);
        if (transformedPoint.x() + 2 < viewWidth)
            canvas.setPixel(transformedPoint.x() + 2, transformedPoint.y(), color);
    }
    if (transformedPoint.x() >= 0 && transformedPoint.x() < viewWidth){
        if (transformedPoint.y() - 2 >= 0)
            canvas.setPixel(transformedPoint.x(), transformedPoint.y() - 2, color);
        if (transformedPoint.y() + 2 < viewHeight)
            canvas.setPixel(transformedPoint.x(), transformedPoint.y() + 2, color);
    }
}

QVector3D Abgabe1::convertFromHomogen(QVector4D& v){
    QVector3D v2(v.x() / v.w(), v.y() / v.w(), v.z() / v.w());
    return v2;
}

void Abgabe1::deinitialize(){

}

void Abgabe1::sizeChanged(unsigned int width, unsigned int height){
    viewWidth = width;
    viewHeight = height;
}

void Abgabe1::meshChanged(const QVector<MeshLoader::Face> &faces){
    points.clear();

    for (MeshLoader::Face f : faces){
        //for every triangle store all 3 points
        for (int i = 0; i < 3; i++){
            MyPoint p;
            QVector3D coord(f[i].x, f[i].y, f[i].z);
            p.coord = coord;
            p.r = f[i].r;
            p.g = f[i].g;
            p.b = f[i].b;
            points.append(p);
        }
    }
}

void Abgabe1::textureChanged(QImage texture){

}

void Abgabe1::mousePressed(int x, int y){
    arcballActive = true;
    makeArcballVector(x, y, arcballP1);
    makeArcballVector(x, y, arcballP2);
    std::cout << "Fixing vector P1: " << arcballP1.x() << ", " << arcballP1.y() << ", " << arcballP1.z() << std::endl;
}

void Abgabe1::mouseMoved(int x, int y){
    if (!arcballActive){
        return;
    }
    arcballSet = true;
    makeArcballVector(x, y, arcballP2);
    std::cout << "P2: " << arcballP2.x() << ", " << arcballP2.y() << ", " << arcballP2.z() << std::endl;
}

void Abgabe1::mouseReleased(int x, int y){
    arcballActive = false;
    std::cout << "End of arball rotation" << std::endl;
}

void Abgabe1::makeArcballVector(int screenX, int screenY, QVector3D &vec){
    float x = (2.0 * ((float) screenX) / ((float) viewWidth)) - 1.0;
    float y = (2.0 * ((float) screenY) / ((float) viewHeight)) - 1.0;
    QVector2D v(x, y);
    if (v.length() > 1.0)
        v.normalize();
    float z = -qSqrt(1.0 - qPow(v.x(), 2) - qPow(v.y(), 2));

    vec.setX(v.x());
    vec.setY(v.y());
    vec.setZ(z);
}
