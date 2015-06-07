#ifndef ABGABE3B_H
#define ABGABE3B_H

#import "abgabe3a.h"

class LightRendererFlat : public LightRendererGouard
{
public:
    LightRendererFlat();
    ~LightRendererFlat();

    virtual void meshChanged(const QVector<MeshLoader::Face>& faces);
};

#endif // ABGABE3B_H
