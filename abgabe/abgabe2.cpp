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
            //canvas.setPixel(var.position.x(), var.position.y(), var.color);
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

    //floor x and y to int
    var.position.setX((int) var.position.x());
    var.position.setY((int) var.position.y());

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
    sortVaryingTuple(varTup);

    //handle special cases
    if (varTup[0].position.y() == varTup[1].position.y() && varTup[0].position.y() == varTup[2].position.y()){
        int min = INT_MAX;
        int max = INT_MIN;
        int y = varTup[0].position.y();
        for (int i = 0; i < 3; i++){
            if (varTup[i].position.x() < min) min = varTup[i].position.x();
            if (varTup[i].position.x() > max) max = varTup[i].position.x();
        }
        for (int i = min; i <= max; i++){
            if (!inView(i, y)){
                continue;
            }
            QVector3D color = varTup[0].color;
            drawVertex(i, y, color, canvas);
        }
        return;
    }
    if (varTup[0].position.x() == varTup[1].position.x() && varTup[0].position.x() == varTup[2].position.x()){
        int min = varTup[0].position.y();
        int max = varTup[2].position.y();
        int x = varTup[0].position.x();
        for (int i = min; i <= max; i++){
            if (!inView(x, i)){
                continue;
            }
            QVector3D color = varTup[0].color;
            drawVertex(x, i, color, canvas);
        }
        return;
    }

    //Standard triangle - first subtriangle
    if (varTup[1].position.y() - varTup[0].position.y() == 0){
        int min = varTup[0].position.x();
        int max = varTup[1].position.x();
        if (max < min){
            int tmp = min;
            min = max;
            max = tmp;
        }
        int y = varTup[0].position.y();
        for (int x = min; x <= max; x++){
            if (!inView(x, y)){
                continue;
            }
            QVector3D color = varTup[0].color;
            drawVertex(x, y, color, canvas);
        }
    }else{
        int m1x = varTup[1].position.x() - varTup[0].position.x();
        int m1b = varTup[1].position.x() * varTup[0].position.y() - varTup[0].position.x() * varTup[1].position.y();
        int m1y = varTup[1].position.y() - varTup[0].position.y();
        int m2x = varTup[2].position.x() - varTup[0].position.x();
        int m2b = varTup[2].position.x() * varTup[0].position.y() - varTup[0].position.x() * varTup[2].position.y();
        int m2y = varTup[2].position.y() - varTup[0].position.y();
        for (int y = varTup[0].position.y(); y <= varTup[1].position.y(); y++){
            float x12 = (y * m1x - m1b) / m1y;
            float x13 = (y * m2x - m2b) / m2y;
            if (x13 < x12){
                float tmp = x12;
                x12 = x13;
                x13 = tmp;
            }
            int m1 = floor(x12);
            int m2 = ceil(x13);
            for (int x = m1; x <= m2; x++){
                if (!inView(x, y)){
                    continue;
                }
                QVector3D color = varTup[0].color;
                drawVertex(x, y, color, canvas);
            }
        }
    }

    //Standard triangle - second subtriangle
    if (varTup[2].position.y() == varTup[1].position.y()){
        int min = varTup[1].position.x();
        int max = varTup[2].position.x();
        if (max < min){
            int tmp = min;
            min = max;
            max = tmp;
        }
        int y = varTup[1].position.y();
        for (int x = min; x <= max; x++){
            if (!inView(x, y)){
                continue;
            }
            QVector3D color = varTup[0].color;
            drawVertex(x, y, color, canvas);
        }
    }else{
        int m2x = varTup[2].position.x() - varTup[0].position.x();
        int m2b = varTup[2].position.x() * varTup[0].position.y() - varTup[0].position.x() * varTup[2].position.y();
        int m2y = varTup[2].position.y() - varTup[0].position.y();
        int m3x = varTup[2].position.x() - varTup[1].position.x();
        int m3b = varTup[2].position.x() * varTup[1].position.y() - varTup[1].position.x() * varTup[2].position.y();
        int m3y = varTup[2].position.y() - varTup[1].position.y();
        for (int y = varTup[1].position.y() + 1; y <= varTup[2].position.y(); y++){
            float x23 = (y * m3x - m3b) / m3y;
            float x13 = (y * m2x - m2b) / m2y;
            if (x13 < x23){
                float tmp = x23;
                x23 = x13;
                x13 = tmp;
            }
            int m1 = floor(x23);
            int m2 = ceil(x13);
            for (int x = m1; x <= m2; x++){
                if (!inView(x, y)){
                    continue;
                }
                QVector3D color = varTup[0].color;
                drawVertex(x, y, color, canvas);
            }
        }
    }

}

void FilledRenderer::sortVaryingTuple(VaryingTuple &varTup){
    for (int j = 0; j < 2; j++){
        for(int i = 1; i < 3-j; i++){
            if (varTup[i-1].position.y() > varTup[i].position.y()){
                Varying tmp = varTup[i-1];
                varTup[i-1] = varTup[i];
                varTup[i] = tmp;
            }
        }
    }
}

void FilledRenderer::drawVertex(int x, int y,  QVector3D color, GdvCanvas &canvas){
    if (x < 0 || x > viewWidth - 1 || y < 0 || y > viewHeight - 1){
        return;
    }

    canvas.setPixel(x, y, color);
}

bool FilledRenderer::inView(int x, int y){
    if (x < 0 || x >= viewWidth || y < 0 || y >= viewHeight){
        return false;
    }
    return true;
}
