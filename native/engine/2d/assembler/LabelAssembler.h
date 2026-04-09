#pragma once
#include "../components/label/LabelComponent.h"

class LabelAssembler
{
public:
    void fillBuffers(LabelComponent *label, int textureIndex);
    static LabelAssembler *getInstance();
};
