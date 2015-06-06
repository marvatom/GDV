#include "abgabe2.h"
#include <iostream>

FilledRenderer::FilledRenderer()
{

}

FilledRenderer::~FilledRenderer()
{

}

void FilledRenderer::setupGUI(GdvGui &userInterface){
    Abgabe1::setupGUI(userInterface);

}

QMatrix4x4 FilledRenderer::createPrtojectionMatrix(){
    if (perspectiveProj){
        QMatrix4x4 MProj(1.0, 0.0, 0.0, 0.0,
                         0.0, 1.0, 0.0, 0.0,
                         0.0, 0.0, 1.0, 0.0,
                         0.0, 0.0, 1.0/screenDist, 0.0);
        return MProj;
    }else{
        QMatrix4x4 MProj(1.0, 0.0, 0.0, 0.0,
                         0.0, 1.0, 0.0, 0.0,
                         0.0, 0.0, 1.0, 0.0, //Here is 1.0!!!!! Z-Buffering
                         0.0, 0.0, 0.0, 1.0);
        return MProj;
    }
}

void FilledRenderer::render(GdvCanvas &canvas){
    //clear buffer and set background color to white
    canvas.clearBuffer(QVector3D(1.0, 1.0, 1.0));
    depthBuffer.fill(-INFINITY);

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
    //var.position.setZ(transformedPoint.z()); //Z-Value is not normalized. Z-Buffering
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
        for (int x = min; x <= max; x++){
            if (!inView(x, y)){
                continue;
            }
            Tuple3<float> weights = calculateWeights(varTup, x, y);
            Varying v;
            v.position.setX(x);
            v.position.setY(y);
            v = interpolate(weights, varTup, v);
            Fragment f = shadePixel(v);
            drawVertex(x, y, f, canvas);
            //QVector3D color = varTup[0].color;
            //drawVertex(i, y, color, canvas);
        }
        return;
    }
    if (varTup[0].position.x() == varTup[1].position.x() && varTup[0].position.x() == varTup[2].position.x()){
        int min = varTup[0].position.y();
        int max = varTup[2].position.y();
        int x = varTup[0].position.x();
        for (int y = min; y <= max; y++){
            if (!inView(x, y)){
                continue;
            }
            Tuple3<float> weights = calculateWeights(varTup, x, y);
            Varying v;
            v.position.setX(x);
            v.position.setY(y);
            v = interpolate(weights, varTup, v);
            Fragment f = shadePixel(v);
            drawVertex(x, y, f, canvas);
            //QVector3D color = varTup[0].color;
            //drawVertex(x, i, color, canvas);
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
            Tuple3<float> weights = calculateWeights(varTup, x, y);
            Varying v;
            v.position.setX(x);
            v.position.setY(y);
            v = interpolate(weights, varTup, v);
            Fragment f = shadePixel(v);
            drawVertex(x, y, f, canvas);
            //QVector3D color = varTup[0].color;
            //drawVertex(x, y, color, canvas);
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
                Tuple3<float> weights = calculateWeights(varTup, x, y);
                Varying v;
                v.position.setX(x);
                v.position.setY(y);
                v = interpolate(weights, varTup, v);
                Fragment f = shadePixel(v);
                drawVertex(x, y, f, canvas);
                //QVector3D color = varTup[0].color;
                //drawVertex(x, y, color, canvas);
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
            Tuple3<float> weights = calculateWeights(varTup, x, y);
            Varying v;
            v.position.setX(x);
            v.position.setY(y);
            v = interpolate(weights, varTup, v);
            Fragment f = shadePixel(v);
            drawVertex(x, y, f, canvas);
            //QVector3D color = varTup[0].color;
            //drawVertex(x, y, color, canvas);
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
                Tuple3<float> weights = calculateWeights(varTup, x, y);
                Varying v;
                v.position.setX(x);
                v.position.setY(y);
                v = interpolate(weights, varTup, v);
                Fragment f = shadePixel(v);
                drawVertex(x, y, f, canvas);
                //QVector3D color = varTup[0].color;
                //drawVertex(x, y, color, canvas);
            }
        }
    }

}

Tuple3<float> FilledRenderer::calculateWeights(VaryingTuple &triangle, int x, int y){
    QVector3D a = triangle[0].position.toVector3D();
    QVector3D b = triangle[1].position.toVector3D();
    QVector3D c = triangle[2].position.toVector3D();
    QVector3D p(x, y, 0);

    /*float areaABC = QVector3D::crossProduct(c-a, b-a).length();
    float areaPBC = QVector3D::crossProduct(c-p, b-p).length();
    float areaPCA = QVector3D::crossProduct(c-p, a-p).length();
    float areaPAB = QVector3D::crossProduct(a-p, b-p).length();

    float alpha = areaPBC / areaABC;
    float beta = areaPCA / areaABC;
    //float gama = 1.0 - alpha - beta;
    float gama = areaPAB / areaABC;
    std::cout << alpha << ", " << beta << ", " << gama << std::endl;*/
    QVector3D v0 = b - a, v1 = c - a, v2 = p - a;
    float d00 = QVector3D::dotProduct(v0, v0);
    float d01 = QVector3D::dotProduct(v0, v1);
    float d11 = QVector3D::dotProduct(v1, v1);
    float d20 = QVector3D::dotProduct(v2, v0);
    float d21 = QVector3D::dotProduct(v2, v1);
    float denom = d00 * d11 - d01 * d01;
    float beta = (d11 * d20 - d01 * d21) / denom;
    float gama = (d00 * d21 - d01 * d20) / denom;
    float alpha = 1.0f - beta - gama;

    Tuple3<float> ret;
    ret[0] = alpha;
    ret[1] = beta;
    ret[2] = gama;
    return ret;
}

Varying FilledRenderer::interpolate(Tuple3<float> weights, VaryingTuple &triangle, Varying &point){
    //interpolate color
    point.color.setX(weights[0] * triangle[0].color.x() + weights[1] * triangle[1].color.x() + weights[2] * triangle[2].color.x());
    point.color.setY(weights[0] * triangle[0].color.y() + weights[1] * triangle[1].color.y() + weights[2] * triangle[2].color.y());
    point.color.setZ(weights[0] * triangle[0].color.z() + weights[1] * triangle[1].color.z() + weights[2] * triangle[2].color.z());

    //interpolate depth (Z-coordinate)
    point.position.setZ(weights[0] * triangle[0].position.z() + weights[1] * triangle[1].position.z() + weights[2] * triangle[2].position.z());

    //return the same point with interpolated color
    return point;
}

Fragment FilledRenderer::shadePixel(Varying &point){
    Fragment f;
    f.color = point.color;
    f.depth = point.position.z();
    return f;
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

void FilledRenderer::drawVertex(int x, int y,  Fragment &f, GdvCanvas &canvas){
    if (x < 0 || x > viewWidth - 1 || y < 0 || y > viewHeight - 1){
        return;
    }

    int depthBufferIndex = y * viewWidth + x;
    if (f.depth > depthBuffer[depthBufferIndex]){
        canvas.setPixel(x, y, f.color);
        depthBuffer[depthBufferIndex] = f.depth;
    }

}

bool FilledRenderer::inView(int x, int y){
    if (x < 0 || x >= viewWidth || y < 0 || y >= viewHeight){
        return false;
    }
    return true;
}

void FilledRenderer::sizeChanged(unsigned int width, unsigned int height){
    viewWidth = width;
    viewHeight = height;

    depthBuffer.resize(width * height);
}
