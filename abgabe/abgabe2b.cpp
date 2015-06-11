#include "abgabe2b.h"
#include "Types3D.h"

WaterRenderer::WaterRenderer()
{
    int rows = 20;
    int cols = 20;
    arrayIter = new float**[rows];
    for (int i = 0; i < rows; ++i) {
      arrayIter[i] = new float*[cols];
    }

    for (int i = 0; i < rows; ++i) {
        bool zero = true;
        for (int j = 0; j < cols; ++j) {
            if (zero){
                arrayIter[i][j] = new float(0.0);
            }else{
                arrayIter[i][j] = new float(3.1416);
            }
            zero = !zero;
        }
    }
}

WaterRenderer::~WaterRenderer()
{

}

void WaterRenderer::render(GdvCanvas &canvas){
    int rows = 20;
    int cols = 20;
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            *arrayIter[i][j] += 0.1;
        }
    }

    FilledRenderer::render(canvas);
}

void WaterRenderer::meshChanged(const QVector<MeshLoader::Face> &faces){
    storedFaces.clear();

    for (int y = 0; y <= 18; y += 2){
        for (int x = 0; x <= 18; x += 2){
            Vertex a, b, c, d, e;

            a.position = QVector4D(x / 21.0f - 0.5f, y / 21.0f - 0.5f, 0.0, 1.0);
            a.color = QVector3D(0.0, 0.0, 1.0);
            b.position = QVector4D((x + 2) / 21.0f - 0.5f, y / 21.0f - 0.5f, 0.0, 1.0);
            b.color = QVector3D(0.0, 0.0, 1.0);
            c.position = QVector4D(x / 21.0f - 0.5f, (y + 2) / 21.0f - 0.5f, 0.0, 1.0);
            c.color = QVector3D(0.0, 0.0, 1.0);
            d.position = QVector4D((x + 2) / 21.0f - 0.5f, (y + 2) / 21.0f - 0.5f, 0.0, 1.0);
            d.color = QVector3D(0.0, 0.0, 1.0);

            e.position = QVector4D((x + 1) / 21.0f - 0.5f, (y + 1) / 21.0f - 0.5f, 0.0, 1.0);
            e.color = QVector3D(0.0, 0.0, 1.0);
            e.middle = true;
            e.iter = arrayIter[y / 2][x / 2];

            Face f1, f2, f3, f4;

            f1[0] = a;
            f1[1] = b;
            f1[2] = e;

            f2[0] = b;
            f2[1] = d;
            f2[2] = e;

            f3[0] = d;
            f3[1] = c;
            f3[2] = e;

            f4[0] = c;
            f4[1] = a;
            f4[2] = e;

            storedFaces.append(f1);
            storedFaces.append(f2);
            storedFaces.append(f3);
            storedFaces.append(f4);
        }
    }

}

Varying WaterRenderer::shadeVertex(Vertex &vertex){
    if (vertex.middle){
        float sin = qSin(*vertex.iter);
        vertex.position.setZ(sin);
        vertex.color.setY((sin + 1.0)/2.0);
        //vertex.color.setY(sin);
    }

    return FilledRenderer::shadeVertex(vertex);
}
