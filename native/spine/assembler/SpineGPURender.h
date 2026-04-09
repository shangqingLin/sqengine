#ifndef _SPINE_GPPU_RENDER_H_
#define _SPINE_GPPU_RENDER_H_
#include "SpineRender.h"
#include "../assets/SpineGPURenderCache.h"
#include <spine/SkeletonClipping.h>

/**
 *
 * 在顶点着色器中计算蒙皮动画。这样指数级提高Spine的计算效率，大大节省CPU计算
 * 不过可能会造成DrawCall高，因为不能支持DrawCall合并，只能一个Spine动画一个drawcall
 *
 * 参考：
 *  https://zh.esotericsoftware.com/forum/d/5415-pixijs-runtime-calculates-meshes-and-ffd-anims-on-gpu/2
 *  https://github.com/pixijs/spine/tree/gpupatch
 *
 * 原理
 * 1、将Skin不变的数据那部分存储到GPU Buffer中，在顶点着色器中获取这些数据
 * 2、将动画改变的Skin部分再动态更新
 * 3、顶点着色器需要支持读取浮点数纹理
 */
class SpineGPURender : public SpineRender
{
private:
    SpineGPURenderCacheSkin *cacheSkin;
    char *textureBuffer;
    float *tempSkinBuffer;
    int currentAnimation;
    // 存储Bone变换数据和动画变形数据
    float *boneDeformBuffer;
    std::unordered_map<int, int> deformOffsetMap;
    Texture2d *boneAndDeformTexture;
    int boneAndDeformTextureSize;
    bool hasDeform;
    IndexBuffer *indexBuffer;
    VertexBuffer *vertexBuffer;
    std::vector<Attribute> vfmtPosColor;
    spSkeletonClipping *clipper;

    void switchSkin(SpineComponent *component);
    int deformNeedBufferSize(SpineComponent *component, int boneOffset);
    void updateBoneAndDeformData(SpineComponent *component);

public:
    SpineGPURender();
    virtual ~SpineGPURender();
    virtual void render(SpineComponent *, float dt);
};

#endif
