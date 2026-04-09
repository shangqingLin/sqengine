#pragma once

#include "../2d/components/sprite/SpriteComponent.h"
#include "../2d/components/graphics/Graphics.h"
#include "../2d/components/label/LabelComponent.h"
#include "../2d/components/Transform2DComponent.h"
#include "../2d/components/particle/Particles2DComponent.h"
#include "../core/sqstd/sqstd.h"
#include "../../physics2d/index.h"
#include "BatchStencil.h"
#include "stencil-define.h"

namespace pipeline
{

    /**
     * 创建内置 UBO DescrtiptorSets，并为其填充数据
     */
    class UBOLocalDescrtiptorSets
    {
    private:
        sqstd::Array<DescriptorSet *> cacheLocalDescriptorSets;

    public:
        DescriptorSet *getLocalDescriptorSet();
        void recvoery(DescriptorSet *localDescriptorSet);
        void setLocalTransform(DescriptorSet *localDescriptorSet, Transform2DComponent *transform);
    };

    class Batch
    {
    private:
        BatchStencil batchStencil;
        UBOLocalDescrtiptorSets localSets;
        MeshInstance *currentMeshInstance = nullptr;

        // 缓存中的MeshInstance
        sqstd::Array<MeshInstance *> drawCallBatchPool;

        // 当前正在使用的MeshInstance
        sqstd::Array<MeshInstance *> batches;
        sqstd::Array<MeshInstance *> otherBatchers;

        void updatePrevDrawCall();

        int combineTextureMaterial(Material *, Texture2d *, Mesh *, Layers, sqstd::Array<MeshInstance *> &drawCalls, StencilStage stencilStage);

        void beginStencil(Layers, sqstd::Array<MeshInstance *> &drawCalls, StencilStage stencilStage);
        void walkNode(Node *, sqstd::Array<MeshInstance *> &drawCalls);

    public:
        friend class BatchStencil;
        Batch();

        MeshInstance *createMeshInstance();
        void commitSprite(SpriteComponent *, sqstd::Array<MeshInstance *> &drawCalls);
        void commitGraphics(Graphics *, sqstd::Array<MeshInstance *> &drawCalls);
        void commitLabel(LabelComponent *, sqstd::Array<MeshInstance *> &drawCalls);
        void commitPhysicsParticle(physics2d::ParticlePhysics2DComponent *, sqstd::Array<MeshInstance *> &drawCalls);
        void commitParticle(Particles2DComponent *, sqstd::Array<MeshInstance *> &drawCalls);
        void commitRenderer(RenderComponent *, sqstd::Array<MeshInstance *> &drawCalls);
        void commitOver();
        void reset();
        void walkScene(sqstd::Array<MeshInstance *> &drawCalls);
    };

}
