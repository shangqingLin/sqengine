#pragma once

/**
 *
 * 基于物理引擎的粒子模拟
 * 1、可以模拟流体
 * 2、可以模拟软体
 */

#include "../../../engine/framework/component/RenderComponent.h"
#include "../../../engine/scene/graphics/VertexBuffer.h"
#include "../assembler/ParticlePhysicsRender.h"

#include "../../phxy/sq-phxy.h"
#include <optional>

namespace physics2d
{

    class ParticlePhysicsRender;
    typedef phxy::SqParticleGroupDef ParticleGroupDef;
    typedef void BodyContactCallback(phxy::SqParticleBodyContact *);
    typedef void BeginParticleContactCallback(phxy::SqParticleContact *);
    typedef void EndParticleContactCallback(int, int);

    struct ParticlePhysicsCreateDefine
    {
        int count{1};
        unsigned int flags{phxy::SqParticleFlag::sq_waterParticle};
        Vec2 position{0.f, 0.f};
        Vec2 velocity{0.f, 0.f};
        unsigned char r{255u};
        unsigned char g{255u};
        unsigned char b{255u};
        unsigned char a{255u};
        int lifetime{0};
    };

    class ParticlePhysics2DComponent : public RenderComponent
    {
    private:
        ParticlePhysicsRender *particleRender;
        phxy::SqParticleSystem *system = NULL;
        int renderType = 0;
        unsigned int listenerState = 0;
        std::optional<std::function<BodyContactCallback>> beginBodyContactCallHandler;
        std::optional<std::function<BodyContactCallback>> endBodyContactCallHandler;
        std::optional<std::function<BeginParticleContactCallback>> beginParticleContactCallHandler;
        std::optional<std::function<EndParticleContactCallback>> endParticleContactCallHandler;

    protected:
        void createParticleSystem();

        virtual void onAwake();
        virtual void onEnable();
        virtual void onDisable();
        virtual void onUpdate();

        virtual bool canRender();
        virtual void _onMaterialModified();

        void onBeginContactBody(phxy::SqParticleBodyContact *);
        void onEndContactBody(phxy::SqParticleBodyContact *);
        void onBeginContactParticle(phxy::SqParticleContact *particleContact);
        void onEndContactParticle(int particleIdA, int particleIdB);
        void checkAnClearEnableListener();

    public:
        friend class PhysicsWorld;
        friend class ParticlePhysicsSdfRender;
        friend class ParticlePhysicsTextureBlur;
        friend class phxy::b2ParticleSystemContactListenerImp;
        ParticlePhysics2DComponent();
        virtual ~ParticlePhysics2DComponent();

        void setUseParticleRenderType(int);
        inline int getParticleRenderType() { return renderType; };

        void addParticleGroup(ParticleGroupDef &);

        /**
         * 重力缩放
         */
        inline void setGravityScale(float scale) { system->SetGravityScale(scale); };

        /**
         * 设置粒子的flag
         */
        inline void setParticleFlags(int particleIndex, unsigned int newMaskFlags) { system->SetParticleFlags(particleIndex, newMaskFlags); };
        inline unsigned int getParticleFlag(int id) { return system->GetParticleFlags(id); };

        void applyLinearImpulseInRang(int firstParticleIndex, int lastParticleIndexr, const Vec2 &force);

        /**
         * 为指定范围内的粒子应用一个力
         */
        void applyForceInRang(int firstParticleIndex, int lastParticleIndexr, const Vec2 &force);

        /**
         * 设置粘性系数。粒子需要实现开启粘性模拟
         */
        void setViscousStrength(float);

        /**
         * 模拟粒子之间的吸附效果。例如谁一滴一滴地分开
         * 否则粒子之间非常松散，详细请看SloveTension的说明
         **/
        void setSurfaceTensionPressureStrength(float);
        void setSurfaceTensionNormalStrength(float);

        /**
         * 创建一个粒子，并返回粒子的ID
         */
        int createParticle(const ParticlePhysicsCreateDefine *const);

        void setParticleRadius(float);
        void setPressureStrength(float v);
        /**
         * 可以预先分配好所需要的粒子，避免不断的内存分配
         */
        void allocate(int count);

        void setDestructionByAge(bool);
        /**
         * 设置某个粒子的生命周期
         * 需要先开启生命周期检测逻辑
         */
        void setParticleLifetime(int particleIndex, float);

        void destroyParticle(int index);
        void destroyOldestParticle(int num);
        void destroyAllParticle();

        void enableListener(bool, char);

        void setBodyContactFilter(uint32_t categoryBits, uint32_t maskBits);

        void setBeginBodyContactCallback(std::function<BodyContactCallback> beginContactCallback);
        void offBeginBodyContactCallback();

        void setEndBodyContactCallback(std::function<BodyContactCallback> endContactCallback);
        void offEndBodyContactCallback();

        void setBeginParticleContactCallback(std::function<BeginParticleContactCallback> callback);
        void offBeginParticleContactCallback();

        void setEndParticleContactCallback(std::function<EndParticleContactCallback> callback);
        void offEndParticleContactCallback();

        inline int getParticleCount() { return system->GetParticleCount(); };
        inline float getParticleLifeTime(int id) { return system->GetParticleLifetime(id); };

        inline const phxy::SqArray<phxy::SqParticleContact> &GetContacts() { return system->GetContacts(); };

        void setViewportSize(float, float);
        void setViewportPos(float, float);

        Vec2 getParticlePosition(int particleId);
        void setPaused(bool);
    };
}
