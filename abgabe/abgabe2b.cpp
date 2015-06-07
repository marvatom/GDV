#include "abgabe2b.h"
#include "Types3D.h"

WaterRenderer::WaterRenderer()
{

}

WaterRenderer::~WaterRenderer()
{

}

/*void WaterRenderer::setupGUI(GdvGui &userInterface){
    FilledRenderer::setupGUI(userInterface);
}*/
void WaterRenderer::render(GdvCanvas &canvas){
    //clear buffer and set background color to white
    canvas.clearBuffer(QVector3D(1.0, 1.0, 1.0));
    /*if (zBuffering){
        depthBuffer.fill(INFINITY);
    }*/

    //calculate the transformation matrix
    transformMatrix = createPrtojectionMatrix() * createViewMatrix();

    for (Face f : storedFaces){
        VaryingTuple varTup;
        for (int i = 0; i < 3; i++){
            actY = qSin(iter);
            iter += 0.1;
            Varying tmp = shadeVertex(f[i]);

            Varying var = viewportTransform(tmp);

            varTup[i] = var;
        }

        /*if (culling){
            QVector3D viewPoint(0.0, 0.0, camDist);
            QVector3D v0 = varTup[0].position.toVector3D();
            QVector3D v1 = varTup[1].position.toVector3D();
            QVector3D v2 = varTup[2].position.toVector3D();
            QVector3D faceNormal = QVector3D::crossProduct(v1 - v0, v2 - v0);
            float testVal = QVector3D::dotProduct(v0 - viewPoint, faceNormal);

            if (testVal >= 0){
                continue;
            }
        }*/

        rasterizeFace(canvas, varTup);
    }

    //flush buffer
    canvas.flipBuffer();
}

void WaterRenderer::meshChanged(const QVector<MeshLoader::Face> &faces){
    //FilledRenderer::meshChanged(faces);
    storedFaces.clear();

    /*Vertex a, b, c;
    a.position.setX(-1.0);
    a.position.setY(1.0);
    a.position.setZ(0.0);
    a.position.setW(1.0);
    a.color.setX(1.0);
    a.color.setY(0.0);
    a.color.setZ(0.0);
    b.position.setX(1.0);
    b.position.setY(1.0);
    b.position.setZ(0.0);
    b.position.setW(1.0);
    b.color.setX(0.0);
    b.color.setY(1.0);
    b.color.setZ(0.0);
    c.position.setX(0.0);
    c.position.setY(-1.0);
    c.position.setZ(0.0);
    c.position.setW(1.0);
    c.color.setX(0.0);
    c.color.setY(0.0);
    c.color.setZ(1.0);
    Face f;
    f[0] = a;
    f[1] = b;
    f[2] = c;
    storedFaces.append(f);*/


    bool diag = true;
    bool nextDiag = true;
    for (float x = -1.0; x <= 0.9; x += 0.1){
        nextDiag = !nextDiag;
        diag = nextDiag;
        for (float y = 1.0; y >= -0.9; y -= 0.1){
            Vertex a, b, c, d;

            a.position = QVector4D(x, y, 0.0, 1.0);
            a.color = QVector3D(0.0, 0.0, 1.0);
            b.position = QVector4D(x + 0.1, y, 0.0, 1.0);
            b.color = QVector3D(0.0, 0.0, 1.0);
            c.position = QVector4D(x, y - 0.1, 0.0, 1.0);
            c.color = QVector3D(0.0, 0.0, 1.0);
            d.position = QVector4D(x + 0.1, y - 0.1, 0.0, 1.0);
            d.color = QVector3D(0.0, 0.0, 1.0);

            if (x == -1.0 && y == 1.0){
                a.color = QVector3D(1.0, 0.0, 0.0);
                b.color = QVector3D(1.0, 0.0, 0.0);
                c.color = QVector3D(1.0, 0.0, 0.0);
            }

            Face f1, f2;
            if (diag){
                f1[0] = a;
                f1[1] = b;
                f1[2] = d;
                f2[0] = a;
                f2[1] = d;
                f2[2] = c;
            }else{
                f1[0] = a;
                f1[1] = b;
                f1[2] = c;
                f2[0] = b;
                f2[1] = d;
                f2[2] = c;
            }
            storedFaces.append(f1);
            storedFaces.append(f2);

            diag = !diag;
        }
    }
}

Varying WaterRenderer::shadeVertex(Vertex &vertex){
    vertex.position.setZ(actY);

    return FilledRenderer::shadeVertex(vertex);
}
