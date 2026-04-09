#include "SpineComponent.h"
#include "SkeletonSystem.h"
#include <engine/assets/AssetManager.h>
#include <engine/core/base/config.h>
#include <engine/scene/graphics/Model.h>
#include "MeshInstanceSpine.h"
#include "../assembler/SpineCPURender.h"
#include "../assembler/SpineGPURender.h"

SpineComponent::SpineComponent() : isPuase(false),
                                   spine(nullptr),
                                   skeleton(nullptr),
                                   animationState(nullptr),
                                   animationStateData(nullptr),
                                   currentAnimation(-1),
                                   speed(1.f),
                                   loop(false),
                                   frameDuration(1.f / 30),
                                   currentFrameDt(0),
                                   spineRender(NULL)
{

    // model = new Model();
    // markForUpdateRenderData();
}

void SpineComponent::setRenderType(int type)
{
    if (renderType == type)
        return;
    renderType = type;

    if (spineRender)
    {
        delete spineRender;
        spineRender = NULL;
    }

    if (renderType == 1)
    {
        spineRender = new SpineCPURender();
    }
    else if (renderType == 2)
    {
        spineRender = new SpineGPURender();
    }
}

void SpineComponent::onEnable()
{
    SkeletonSystem::getInstance()->addComponent(this);
}

void SpineComponent::onDisable()
{
    SkeletonSystem::getInstance()->removeComponent(this);
}

void SpineComponent::setSpine(Spine *spine)
{
    if (this->spine == spine)
    {
        return;
    }
    clearSpine();
    this->spine = spine;
    if (spine)
    {
        skeleton = spSkeleton_create(spine->getSkeletonData());
        animationStateData = spAnimationStateData_create(spine->getSkeletonData());
        animationState = spAnimationState_create(animationStateData);

        if (currentAnimation != -1)
            play(currentAnimation);
    }

    // markForUpdateRenderData();
}

void SpineComponent::setPlaySpeed(float speed)
{
    if (speed)
        this->speed = speed;
    // animationState->timeScale = speed;
}

void SpineComponent::setFrame(int frame)
{
    this->frameDuration = 1.0f / frame;
}

void SpineComponent::play(int animIndex, bool loop)
{
    isPuase = false;
    currentAnimation = animIndex;
    this->loop = loop;
    if (spine)
    {
        const spAnimation *animation = spine->getAnimation(currentAnimation);
        SQ_ASSERT(animation);
        spAnimationState_setAnimation(animationState, 0, (spAnimation *)animation, loop);
    }
}

// bool SpineComponent::canRender()
// {
//     // printf("=====canRender %p %p %p \n",getMaterial(),spine,spineRender);

//     return getMaterial() && spine && spineRender;
// }

void SpineComponent::stop()
{
    if (!this->isPuase)
    {
        this->isPuase = true;
    }
}

void SpineComponent::puase()
{
    if (!this->isPuase)
    {
        this->isPuase = true;
    }
}

void SpineComponent::resume()
{
    if (this->isPuase)
    {
        this->isPuase = true;
    }
}

void SpineComponent::setSkin(int skinIndex)
{
    spSkeletonData *skData = spine->getSkeletonData();
    if (skinIndex < skData->skinsCount)
    {
        spSkeleton_setSkin(skeleton, skData->skins[skinIndex]);
    }
}

spSkin *SpineComponent::getSkin()
{
    return skeleton->skin ? skeleton->skin : spine->getSkeletonData()->defaultSkin;
}

void SpineComponent::update(float dt)
{
    // printf("?????????????????? %d %p %d %p\n",isPuase,spine,currentAnimation,spineRender);

    if (!spine || isPuase || !spineRender)
        return;

    currentFrameDt += dt * speed;

    // printf("====== %f %f %f %f\n",currentFrameDt,frameDuration,dt,speed);

    if (currentFrameDt < frameDuration)
    {
        return;
    }

    spineRender->render(this, currentFrameDt);
    currentFrameDt = 0;

    if (!loop && currentAnimation != -1)
    {
        spTrackEntry *entry = spAnimationState_getCurrent(animationState, 0);
        float currentTime = spTrackEntry_getAnimationTime(entry);

        // printf("time %f \n",currentTime);

        if (currentTime >= entry->animation->duration)
        {
            isPuase = true;
        }
    }
}

// void SpineComponent::_render(Batcher2D *batch)
// {
//     Transform2DComponent *transform = node->getComponent<Transform2DComponent>();
//     for (int i = 0; i < model->meshInstances.size(); ++i)
//     {
//         MeshInstanceSpine *instance = dynamic_cast<MeshInstanceSpine *>(model->meshInstances[i]);
//         batch->commitMeshInstace(this, instance, instance->textures, transform, nullptr);
//     }
// }

// void SpineComponent::_onMaterialModified()
// {
//     UIRenderComponent::_onMaterialModified();
//     for (int i = 0; i < model->meshInstances.size(); ++i)
//     {
//         MeshInstance *instance = model->meshInstances[i];
//         instance->setMaterial(getMaterial());
//     }
// }

void SpineComponent::clearSpine()
{
    if (skeleton)
    {
        spSkeleton_dispose(skeleton);
        skeleton = nullptr;
    }

    if (animationStateData)
    {
        spAnimationStateData_dispose(animationStateData);
        animationStateData = nullptr;
    }

    if (animationState)
    {
        spAnimationState_dispose(animationState);
        animationState = nullptr;
    }
    spine = nullptr;
}

SpineComponent::~SpineComponent()
{
    SkeletonSystem::getInstance()->removeComponent(this);
    clearSpine();
    if (spineRender)
    {
        delete spineRender;
        spineRender = NULL;
    }
}

void processSpineComponent(bridge::ComponentType type, char nodeOp, char op, ArrayBuffer &buffer, Node *node)
{
    if (nodeOp == 1)
    {
        node->addComponent<SpineComponent>();
        return;
    }
    else if (nodeOp == 2)
    {
        node->removeComponent<SpineComponent>();
        return;
    }

    SpineComponent *component = node->getComponent<SpineComponent>();
    if (op < 50)
    {
        // processUIRenderComponent(component,type, nodeOp, op, buffer, node);
        return;
    }

    switch (op)
    {
    case 51:
    {
        int &assetId = *buffer.popp<int>();
        if (assetId == -1)
        {
            component->setSpine(nullptr);
        }
        else
        {
            Spine *spine = static_cast<Spine *>(AssetManager::getInstance()->findById(assetId));
            component->setSpine(spine);
        }
        break;
    }
    case 52:
    {
        component->setPlaySpeed(*buffer.popp<float>());
        break;
    }
    case 53:
    {
        component->play(*buffer.popp<int>(), *buffer.popp<char>());
        break;
    }
    case 54:
        component->stop();
        break;
    case 55:
        component->puase();
        break;
    case 56:
        component->resume();
        break;
    case 57:
        component->setFrame(*buffer.popp<int>());
        break;
    case 58:
        component->setRenderType(*buffer.popp<char>());
        break;
    }
}