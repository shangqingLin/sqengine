#pragma once

#include "../../../engine/assets/RenderTexture.h"
#include "../../../engine/assets/Texture2d.h"
#include "../../../engine/scene/graphics/VertexBuffer.h"
#include "../../../engine/scene/graphics/Mesh.h"
#include "ParticlePhysicsRender.h"

/**
 * 使用SDF渲染圆形粒子的。即使用SDF在着色器中构建Metaball
 * 1、设备需要支持RGBA32F，因为需要使用这种纹理存储数据到着色器中
 * 2、支持RGBA32F 纹理的 RenderTexture
 * 3、WebGL有些平台不支持32位纹理或FBO渲染到32位纹理上的，则不能使用此渲染方式
 */
namespace physics2d
{

    class ParticlePhysics2DComponent;

    /**
     *
     * 一、折射的渲染模式：
     *  所谓的折射其实就是透过水可以看到背景的扭曲就是折射，为了实现这个效果这里渲染分为两个阶段
     *  1、将水的信息离屏幕渲染到一张图片B中
     *  2、将水渲染和场景渲染到一张图A中，标记图A哪个像素是水的，然后在处理这个像素的时候通过扭曲的uv取背景颜色、水的颜色、B图中存储水的信息，进行融合计算得到最终水的颜色
     *
     */
    class ParticlePhysicsSdfRender : public ParticlePhysicsRender
    {
    private:
        Texture2d *posBuffer;
        Texture2d *rowColumnTexture;
        float *rowColumnBuffer;

        Texture2d *sdfBufferTexture;
        float *sdfBuffer;
        unsigned int sdfBufferUpdateMinX, sdfBufferUpdateMinY, sdfBufferUpdateMaxX, sdfBufferUpdateMaxY;

        VertexBuffer *vParticleIndex;
        float viewWidth, viewHeight;
        float viewPosLeftX, viewPosLeftY;
        unsigned int viewportMinX;
        unsigned int viewportMinY;
        unsigned int viewportMaxX;
        unsigned int viewportMaxY;
        bool viewportDiry = false;

        Mesh *mesh;
        
        void setParticleNum(int particleNum);
        void createGridTexture();

        void udpateViewport();

    public:
        friend class ParticlePhysics2DComponent;
        ParticlePhysicsSdfRender(ParticlePhysics2DComponent *);
        virtual ~ParticlePhysicsSdfRender();
        virtual void onUpdateMaterial();
        void setViewportSize(float, float);
        void setViewportPos(float, float);
        virtual void allocate(int particleNum);
        virtual void update();
    };
}