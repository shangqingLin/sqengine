#pragma once
#include "Collision2DComponent.h"
namespace physics2d
{
    class ChainShape2DComponent : public Collision2DComponent
    {
    public:
        ChainShape2DComponent();
        virtual ~ChainShape2DComponent();
        void createChain(float *points, int num);
        void createChainEnableLink(float *points, int *ids, int numPoint);
        void insertBefore(int fromId, int id, float x, float y);
        void insertAfter(int fromId, int id, float x, float y);
        void modify(int id, float x, float y);
        void remove(int id);
        void setOneSided(bool b);
    };
}
