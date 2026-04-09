#pragma once

#include "../../../core/math/Vec2.h"
#include "../../../core/core.h"

/**
 *  粒子发射器
 *  粒子发射流程全部在CPU端完成
 */

enum class ParticleFlag : int
{
    PARTICLE_FLAG_ACTIVE = 1,
    PARTICLE_FLAG_STARTED = 4
};
SQ_ENUM_CONVERSION_OPERATOR(ParticleFlag);

struct Particle2D
{
    int flag{0};
    float lifetime_percent = 0.f;
    float accuLifetime = 0.f;
    int index;
    void addFlag(ParticleFlag);
    void removeFlag(ParticleFlag);
    void clearFlag();
    bool hasFlag(ParticleFlag);
};

struct Particle2DTransmitterDefine
{
    int amount = 8;
    float randomness = 0.f;
    float randomSeed = -1.f;

    // 单位为毫秒
    float lifeTime = 0.f;
    float lifetime_randomness = 1.f;
    float amoutRatio = 1.0f;
    bool emitting = true;
    bool oneShot = false;
};

class Particle2DTransmitter
{
protected:
    

    float prev_time = 0.0;
    float currentTime = 0.0;

    /**
     * 当前激活了多少个粒子
     */
    int activeCount = 0;
    Particle2D *particles;
    Particle2DTransmitterDefine define;

    virtual void onUpdateParticle(Particle2D*) = 0;
    virtual void onEmitParticle(Particle2D*) = 0;
public:
    Particle2DTransmitter();
    virtual ~Particle2DTransmitter();

    virtual void initialize(Particle2DTransmitterDefine& );
    virtual void setRandomSeed(float seed);
    void setRandomness(unsigned int seed);

    /**
     * 改变Ammout是一个非常重度的操作
     */
    virtual void setAmount(int amount);
    void setAmountRatio(float);
    void setLifeTime(int);
    void setEmit(bool b);
    void setOneShot(bool b);
    bool update(float dt);
    inline int getActiveCount() { return activeCount; };
    inline const Particle2D *getParticles() { return particles; };
};
