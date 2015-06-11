#include "abgabe3c.h"

TexturedRenderer::TexturedRenderer()
{

}

TexturedRenderer::~TexturedRenderer()
{

}

void TexturedRenderer::setupGUI(GdvGui &userInterface){
    FilledRenderer::setupGUI(userInterface);

    userInterface.addCheckBox("Bilinear Filtering", false, bilinearFilter);
}

void TexturedRenderer::textureChanged(QImage texture){
    storedTexture = texture.copy();

    qDebug() << "TEXTURE: " << storedTexture.width() << ", " << storedTexture.height();
}

void TexturedRenderer::meshChanged(const QVector<MeshLoader::Face> &faces){
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

            newFace[i].textureCoord.setX(f[i].u);
            newFace[i].textureCoord.setY(f[i].v);
        }
        storedFaces.append(newFace);
    }
}

Varying TexturedRenderer::shadeVertex(Vertex &vertex){
    Varying var = FilledRenderer::shadeVertex(vertex);

    var.textureCoord = vertex.textureCoord;

    /*if (var.textureCoord.y() > 1.0f || var.textureCoord.y() < 0.0f){
        qDebug() << var.textureCoord.y();
    }*/

    return var;
}

Varying TexturedRenderer::interpolate(Tuple3<float> weights, VaryingTuple &triangle, Varying &point){
    FilledRenderer::interpolate(weights, triangle, point);

    float textX = weights[0] * triangle[0].textureCoord.x() + weights[1] * triangle[1].textureCoord.x() + weights[2] * triangle[2].textureCoord.x();

    textX = fabs(textX);
    if (textX > 1.0f){
        textX = 1.0f;
    }

    point.textureCoord.setX(textX);
    float textY = weights[0] * triangle[0].textureCoord.y() + weights[1] * triangle[1].textureCoord.y() + weights[2] * triangle[2].textureCoord.y();

    textY = fabs(textY);
    if (textY > 1.0f){
        textY = 1.0f;
    }
    point.textureCoord.setY(textY);

    return point;
}

Fragment TexturedRenderer::shadePixel(Varying &point){
    Fragment f = FilledRenderer::shadePixel(point);

    //qDebug() << "Texture coordinates: " << point.textureCoord.x() << ", " << point.textureCoord.y();

    float textureX = point.textureCoord.x() * (storedTexture.width() - 1);
    float textureY = point.textureCoord.y() * (storedTexture.height() - 1);
    //int x = qRound(textureX);
    //int y = qRound(textureY);
    QVector3D filteredColor = filter(textureX, textureY);
    /*QRgb pixel = storedTexture.pixel(x, y);
    float r = qRed(pixel) / 255.0f;
    float g = qGreen(pixel) / 255.0f;
    float b = qBlue(pixel) / 255.0f;*/

    f.color.setX((f.color.x() + filteredColor.x())/2.0f);
    f.color.setY((f.color.y() + filteredColor.y())/2.0f);
    f.color.setZ((f.color.z() + filteredColor.z())/2.0f);

    return f;
}

QVector3D TexturedRenderer::filter(float textureX, float textureY){
    QVector3D ret;
    int x = 0;
    int y = 0;
    if (bilinearFilter){
        int pos11_x = floor(textureX);
        int pos11_y = floor(textureY);
        float dist11 = sqrt(pow(textureX - pos11_x, 2) + pow(textureY - pos11_y, 2));

        int pos12_x = ceil(textureX);
        int pos12_y = floor(textureY);
        float dist12 = sqrt(pow(textureX - pos12_x, 2) + pow(textureY - pos12_y, 2));

        int pos21_x = floor(textureX);
        int pos21_y = ceil(textureY);
        float dist21 = sqrt(pow(textureX - pos21_x, 2) + pow(textureY - pos21_y, 2));

        int pos22_x = ceil(textureX);
        int pos22_y = ceil(textureY);
        float dist22 = sqrt(pow(textureX - pos22_x, 2) + pow(textureY - pos22_y, 2));

        QRgb textel11 = storedTexture.pixel(pos11_x, pos11_y);
        QRgb textel12 = storedTexture.pixel(pos12_x, pos12_y);
        QRgb textel21 = storedTexture.pixel(pos21_x, pos21_y);
        QRgb textel22 = storedTexture.pixel(pos22_x, pos22_y);

        float distSum = dist11 + dist12 + dist21 + dist22;

        ret.setX((qRed(textel11) * dist11 +  qRed(textel12) * dist12 + qRed(textel21) * dist21 + qRed(textel22) * dist22) / (distSum * 255.0f));
        ret.setY((qGreen(textel11) * dist11 +  qGreen(textel12) * dist12 + qGreen(textel21) * dist21 + qGreen(textel22) * dist22) / (distSum * 255.0f));
        ret.setZ((qBlue(textel11) * dist11 +  qBlue(textel12) * dist12 + qBlue(textel21) * dist21 + qBlue(textel22) * dist22) / (distSum * 255.0f));
        //ret.setX((qRed(textel11) / 255.0f * dist11 +  qRed(textel12) / 255.0f * dist12 + qRed(textel21) / 255.0f * dist21 + qRed(textel22) / 255.0f * dist22) / (distSum));
        //ret.setY((qGreen(textel11) / 255.0f * dist11 +  qGreen(textel12) / 255.0f * dist12 + qGreen(textel21) / 255.0f * dist21 + qGreen(textel22) / 255.0f * dist22) / (distSum));
        //ret.setZ((qBlue(textel11) / 255.0f * dist11 +  qBlue(textel12) / 255.0f * dist12 + qBlue(textel21) / 255.0f * dist21 + qBlue(textel22) / 255.0f * dist22) / (distSum));

    }else{
        x = qRound(textureX);
        y = qRound(textureY);

        QRgb pixel = storedTexture.pixel(x, y);
        ret.setX(qRed(pixel) / 255.0f);
        ret.setY(qGreen(pixel) / 255.0f);
        ret.setZ(qBlue(pixel) / 255.0f);
    }

    return ret;
}
