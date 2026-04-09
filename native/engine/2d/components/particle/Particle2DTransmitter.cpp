#include "Particle2DTransmitter.h"
#include "./common/math.h"
#include "../../../core/math/Color.h"
#include "../../../core/math/math.h"
#include <stdio.h>
// #include "../../../../core/utils/bit.h"

void Particle2D::addFlag(ParticleFlag flagParam)
{
    flag |= toNumber(flagParam);
}

void Particle2D::removeFlag(ParticleFlag flagParam)
{
    flag &= ~toNumber(flagParam);
}

void Particle2D::clearFlag()
{
    flag = 0.f;
}

bool Particle2D::hasFlag(ParticleFlag flagParam)
{
    return flag & toNumber(flagParam);
}

Particle2DTransmitter::Particle2DTransmitter() 
:particles(nullptr){}

void Particle2DTransmitter::initialize(Particle2DTransmitterDefine& define)
{
    this->define = define;
    int amount = define.amount;
    this->define.amount = 0;
    setAmount(amount);
}

Particle2DTransmitter::~Particle2DTransmitter()
{
    delete[] particles;
    particles = nullptr;
}

void Particle2DTransmitter::setAmount(int amount)
{
    if (define.amount == amount)
        return;
    define.amount = amount;
    if (particles)
    {
        delete[] particles;
        particles = nullptr;
    }
    particles = new Particle2D[amount];
    for(int i = 0; i < amount ; ++i){
        particles[i].index = i;
    }
}

void Particle2DTransmitter::setLifeTime(int time)
{
    define.lifeTime = time;
}

void Particle2DTransmitter::setEmit(bool b)
{
    define.emitting = b;
}

void Particle2DTransmitter::setOneShot(bool b)
{
    define.oneShot = b;
}

void Particle2DTransmitter::setRandomSeed(float seed)
{
    define.randomSeed = seed;
}

void Particle2DTransmitter::setRandomness(unsigned int seed)
{
    define.randomness = seed;
}

void Particle2DTransmitter::setAmountRatio(float ratio)
{
    define.amoutRatio = ratio;
}

bool Particle2DTransmitter::update(float dt)
{

    /*
     * 通过进度的方式计算一帧需要发射多少个粒子，判断指定的粒子是否在当前的进度之内
     * 如果在则发射这个粒子
     *
     * 还需要注意：为了丰富粒子表现，发射的粒子不一定处于激活状态，粒子发射之后还会在start或update中随机激活
     *
     */

    // 超出1的部分直接去掉1，取小数部分，表示已经超过了一次的进度循环
    // float new_phase = Math::fmod(particles.prev_phase + (dt / define.lifeTime), 1.0f);

    currentTime += dt;
    if (currentTime > define.lifeTime)
    {
        currentTime = Math::fmod(currentTime, define.lifeTime);
        if (define.oneShot)
        {
            setEmit(false);
        }
    }

    // printf("++++++++++++++++ prev_phase %f new_phase %f \n", prev_time , currentTime);

    activeCount = 0;
    bool restart = false;
    bool dirty = false;
    float local_delta = dt;
    bool particleDirty = false;
    for (int particleIndex = 0; particleIndex < define.amount; ++particleIndex)
    {
        
        Particle2D& particle = particles[particleIndex];
        particleDirty = false;

        // 上一帧是激活状态，移除掉
        if (particle.hasFlag(ParticleFlag::PARTICLE_FLAG_STARTED))
        {
            particle.removeFlag(ParticleFlag::PARTICLE_FLAG_STARTED);
            particleDirty = true;
        }

        restart = false;
        local_delta = dt;

        // 计算粒子在整个进度哪个地方，到了这个进度才触发粒子
        float restart_phase = float(particleIndex) / float(define.amount);
        float restart_time = restart_phase * define.lifeTime;

        // 用于给粒子发射时机添加随机性的逻辑，目的是打破粒子发射的机械式均匀分布，让粒子生成更自然
        /**
          数学效果,假设：
            total_particles = 1000（粒子总数）
            index = 123（当前粒子索引）
            randomness = 0.5
            生成的 random = 0.327
            则相位扰动量为：
            0.5 * 0.327 * (1/1000) ≈ 0.0001635
            最终 restart_phase 从原来的 0.123 → 0.1231635
         */
        // if (define.randomness > 0.0)
        // {
        //     unsigned int seed = particles.cycle;
        //     if (restart_phase >= new_phase)
        //     {
        //         seed -= (unsigned int)(1);
        //     }
        //     seed *= (unsigned int)(define.amount);
        //     seed += (unsigned int)(particleIndex);
        //     float random = float(hash(seed) % (unsigned int)(65536)) / 65536.0;
        //     restart_phase += define.randomness * random * 1.0 / float(define.amount);
        // }

        if (currentTime > prev_time)
        {
            // 表示当前进度是向前

            // 判断当前的粒子是否在这个进度之内
            if (restart_time >= prev_time && restart_time < currentTime)
            {
                restart = true;
                // 起始时间，粒子很大可能已激活就已经过了一定的时间了，不是一激活都是从0开始的，要看进度
                local_delta = currentTime - restart_time;
            }
        }
        else if (local_delta > 0.0f)
        {
            // 如果ncurrentTime <= prev_time，表示重新进入新一轮的进度了

            if (restart_time >= prev_time)
            {
                // 保证处于最后进度的粒子全部发射
                restart = true;
                local_delta = define.lifeTime - restart_time + currentTime;
            }
            else if (restart_time < currentTime)
            {
                // 或处于开始处的粒子全部发射
                restart = true;
                local_delta = currentTime - restart_time;
            }
        }

        // printf("fuck %d %f \n",particleIndex,restart_time);

        if (restart)
        {

            if (!define.emitting)
            {
                if (particle.hasFlag(ParticleFlag::PARTICLE_FLAG_ACTIVE))
                {
                    dirty = true;
                    particle.clearFlag();
                    particleDirty = true;
                    // printf("+++clear %d act %f rt %f ct %f pt %f \n", particleIndex, particleInner.accuLifetime, restart_time, currentTime, prev_time);
                }
                continue;
            }

            dirty = true;

            // 外部通过amoutRatio控制发射的进度
            unsigned int alt_seed = hash(particleIndex + 1 + define.randomSeed);
            if (rand_from_seed(alt_seed) > define.amoutRatio)
            {
                particle.clearFlag();
                particleDirty = true;
            }
            else
            {

                // if (particleIndex == 0.)
                // printf("reset %d rt %f ct  %f %f at %f \n", particleIndex, restart_time, currentTime, local_delta, particleInner.accuLifetime);
                particle.lifetime_percent = local_delta / define.lifeTime;
                particle.accuLifetime = local_delta;
                particle.addFlag(ParticleFlag::PARTICLE_FLAG_STARTED);
                particle.addFlag(ParticleFlag::PARTICLE_FLAG_ACTIVE);
                ++activeCount;
                particleDirty = true;
            }
        }
        else
        {

            if (particle.hasFlag(ParticleFlag::PARTICLE_FLAG_ACTIVE))
            {

                dirty = true;
                particleDirty = true;
                if (particle.lifetime_percent >= 1.0f)
                {
                    particle.removeFlag(ParticleFlag::PARTICLE_FLAG_STARTED);
                    particle.removeFlag(ParticleFlag::PARTICLE_FLAG_ACTIVE);
                    // printf("++++die %d \n", particleIndex);
                    continue;
                }
                ++activeCount;
                particle.accuLifetime += dt;
                particle.lifetime_percent = particle.accuLifetime / define.lifeTime;
                // if (particleIndex == 0.)
                // printf("update particleIndex %d %f %f \n", particleIndex, particleInner.accuLifetime, particle->lifetime_percent);
            }
        }

        if(particleDirty)
        {
            onUpdateParticle(&particle);
        }

        // printf("== %d %g\n",particleIndex,particle->flag);

        // particle->flag = 1;
        // particle->lifetime = 20.f;
        // particle->velocity.x = 0.f;
    }

    // printf("+++++++++++++++++active number %d %d \n",activeCount,dirty);

    prev_time = currentTime;
    return dirty;
}