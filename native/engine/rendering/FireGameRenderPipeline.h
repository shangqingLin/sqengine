#pragma once
#include "RenderPipeline.h"
#include "Batch.h"
#include "../gfx/base/CommandBuffer.h"

namespace pipeline
{

    class FireGameRenderPipeline : public RenderPipeline
    {
    private:
        Batch batch;

        bool enabelWashRender = false;
        bool washFirst = false;
        Material *waterMarkMaterial = nullptr;

        bool prevRenderGameObjectInfo = false;
        bool prevRenderGameObjectInfoBlend = false;
        void renderMeshInstance(MeshInstance *, gfx::CommandBuffer *);
        void beginRenderPass(Camera *, gfx::CommandBuffer *, bool forceRefresh);
        void renderWashEffect(std::vector<Camera *> &, sqstd::Array<MeshInstance *>& drawCalls);

    public:
        virtual void render(std::vector<Camera *> &, PipleRenderContext &context);
        void enableWashRender(bool b, int waterMarkMaterialId);
    };

}
