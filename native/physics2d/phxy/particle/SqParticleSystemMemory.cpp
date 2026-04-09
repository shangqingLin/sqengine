#include "SqParticleSystemMemory.h"
#include "SqParticleSystem.h"
#include "../common/SqMemory.h"

using namespace phxy;

SqParticleSystemMemory::SqParticleSystemMemory(SqParticleSystem *system) : system(system)
{
}

SqParticleSystemMemory::~SqParticleSystemMemory()
{

    freeBuffer(&particleIdToBufferIndexMap);
    freeBuffer(&m_positionBuffer);
    freeBuffer(&m_weightBuffer);
    freeBuffer(&m_accumulationBuffer);
    freeBuffer(&m_colorBuffer.data);
    freeBuffer(&m_staticPressureBuffer.data);
    freeBuffer(&m_accumulation2Buffer.data);
    freeBuffer(&m_depthBuffer.data);
    freeBuffer(&m_groupBuffer.data);
    freeBuffer(&m_userDataBuffer.data);
    freeBuffer(&m_expirationTimeBuffer.data);
    freeBuffer(&m_sortExpirationTimeBuffer.data);
    freeBuffer(&m_forceBuffer.data);
    // freeBuffer(&m_bodyContactCountBuffer.data);
    // freeBuffer(&m_consecutiveContactStepsBuffer.data);
}

template <typename T>
void SqParticleSystemMemory::freeBuffer(T **b)
{
    if (*b == NULL)
        return;

    SqFree(*b, sizeof(**b) * particleCapacity);
    *b = NULL;
}

// Reallocate a buffer
template <typename T>
T *SqParticleSystemMemory::reallocateBuffer(T *oldBuffer, int oldCapacity, int newCapacity)
{
    return (T *)SqGrowAlloc(oldBuffer, sizeof(T) * oldCapacity, sizeof(T) * newCapacity);
}

// Reallocate a buffer
template <typename T>
void SqParticleSystemMemory::reallocateLazyBuffer(LazyBuffer<T> *buffer, int oldCapacity, int newCapacity)
{
    SQ_ASSERT(newCapacity > oldCapacity);
    if (buffer->data)
    {
        buffer->data = reallocateBuffer(buffer->data, oldCapacity, newCapacity);
    }
}

template <typename T>
T *SqParticleSystemMemory::requestBuffer(T *buffer)
{
    if (!buffer)
    {
        if (particleCapacity == 0)
        {
            resize(sq_minParticleSystemBufferCapacity);
        }
        buffer = (T *)SqAlloc(sizeof(T) * particleCapacity);
        SQ_ASSERT(buffer);
        memset((void *)buffer, 0, sizeof(T) * particleCapacity);
    }
    return buffer;
}

void SqParticleSystemMemory::requestUserDataBuffer()
{
    if (!m_userDataBuffer.data)
    {
        m_userDataBuffer.data = requestBuffer(m_userDataBuffer.data);
    }
}

void SqParticleSystemMemory::requestLifeTimeBuffer()
{
    if (!m_expirationTimeBuffer.data)
    {
        m_expirationTimeBuffer.data = requestBuffer(m_expirationTimeBuffer.data);
        m_sortExpirationTimeBuffer.data = requestBuffer(m_sortExpirationTimeBuffer.data);
        for (int i = 0; i < m_count; ++i)
        {
            m_sortExpirationTimeBuffer.data[i] = i;
        }
    }
}

void SqParticleSystemMemory::requestGroupBuffer()
{
    if (!m_groupBuffer.data)
    {
        m_groupBuffer.data = requestBuffer(m_groupBuffer.data);
    }
}

void SqParticleSystemMemory::requestColorBuffer()
{
    if (!m_colorBuffer.data)
    {
        m_colorBuffer.data = requestBuffer(m_colorBuffer.data);
    }
}

void SqParticleSystemMemory::requestAccumulation2Buffer()
{
    if (!m_accumulation2Buffer.data)
    {
        m_accumulation2Buffer.data = requestBuffer(m_accumulation2Buffer.data);
    }
}

void SqParticleSystemMemory::requestStaticPressureBuffer()
{
    if (!m_staticPressureBuffer.data)
    {
        m_staticPressureBuffer.data = requestBuffer(m_staticPressureBuffer.data);
    }
}

void SqParticleSystemMemory::resetAccumulationBuffer()
{
    if (m_staticPressureBuffer.data)
    {
        memset(m_accumulationBuffer, 0, sizeof(*m_accumulationBuffer) * m_count);
    }
}

void SqParticleSystemMemory::requestDepthBuffer()
{
    if (!m_depthBuffer.data)
    {
        m_depthBuffer.data = requestBuffer(m_depthBuffer.data);
    }
}

void SqParticleSystemMemory::destroyLifeTimeBuffer()
{
    if (m_expirationTimeBuffer.data)
    {
        freeBuffer(&m_expirationTimeBuffer.data);
        freeBuffer(&m_sortExpirationTimeBuffer.data);
        m_expirationTimeBuffer.data = nullptr;
        m_sortExpirationTimeBuffer.data = nullptr;
    }
}

void SqParticleSystemMemory::resetAccumulation2Buffer()
{
    if (m_accumulation2Buffer.data)
    {
        memset((void *)m_accumulation2Buffer.data, 0, sizeof(SqVec2) * particleCapacity);
    }
}

void SqParticleSystemMemory::resetStaticPressureBuffer()
{
    if (m_staticPressureBuffer.data)
    {
        memset((void *)m_staticPressureBuffer.data, 0, sizeof(float) * particleCapacity);
    }
}

void SqParticleSystemMemory::requestForceBuffer()
{
    if (!m_forceBuffer.data)
    {
        m_forceBuffer.data = requestBuffer(m_forceBuffer.data);
    }
}

void SqParticleSystemMemory::resetForceBuffer()
{
    if (m_forceBuffer.data)
    {
        memset((void *)m_forceBuffer.data, 0, sizeof(SqVec2) * particleCapacity);
    }
}

void SqParticleSystemMemory::resize(int particleNum)
{
    if (particleNum < particleCapacity)
        return;
    int oldCapacity = particleCapacity;
    particleCapacity = particleNum;
    particleIdToBufferIndexMap = reallocateBuffer(particleIdToBufferIndexMap, oldCapacity, particleCapacity);
    m_positionBuffer = reallocateBuffer(m_positionBuffer, oldCapacity, particleCapacity);
    m_weightBuffer = reallocateBuffer(m_weightBuffer, oldCapacity, particleCapacity);
    m_accumulationBuffer = reallocateBuffer(m_accumulationBuffer, oldCapacity, particleCapacity);

    reallocateLazyBuffer(&m_staticPressureBuffer, oldCapacity, particleCapacity);
    reallocateLazyBuffer(&m_forceBuffer, oldCapacity, particleCapacity);
    reallocateLazyBuffer(&m_accumulation2Buffer, oldCapacity, particleCapacity);
    reallocateLazyBuffer(&m_depthBuffer, oldCapacity, particleCapacity);
    reallocateLazyBuffer(&m_colorBuffer, oldCapacity, particleCapacity);
    reallocateLazyBuffer(&m_groupBuffer, oldCapacity, particleCapacity);
    reallocateLazyBuffer(&m_userDataBuffer, oldCapacity, particleCapacity);
    reallocateLazyBuffer(&m_expirationTimeBuffer, oldCapacity, particleCapacity);
    reallocateLazyBuffer(&m_sortExpirationTimeBuffer, oldCapacity, particleCapacity);
}

int SqParticleSystemMemory::addParticle(int particleId)
{
    if (m_count == particleCapacity)
    {
        // 空间不够，重新分配空间，每次按照当前粒子个数的2倍分配
        int capacity = m_count ? 2 * m_count : sq_minParticleSystemBufferCapacity;
        resize(capacity);
    }

    particleIdToBufferIndexMap[m_count] = particleId;
    m_positionBuffer[m_count].zero();
    m_weightBuffer[m_count] = 0;

    if (m_colorBuffer.data)
    {
        m_colorBuffer.data[m_count] = SqParticleColor();
    }

    if (m_sortExpirationTimeBuffer.data)
    {
        m_sortExpirationTimeBuffer.data[m_count] = m_count;
    }

    if (m_depthBuffer.data)
    {
        m_depthBuffer.data[m_count] = 0.f;
    }

    if (m_staticPressureBuffer.data)
    {
        m_staticPressureBuffer.data[m_count] = 0.f;
    }

    if (m_forceBuffer.data)
    {
        m_forceBuffer.data[m_count].zero();
    }

    if (m_userDataBuffer.data)
    {
        m_userDataBuffer.data[m_count] = nullptr;
    }

    if (m_groupBuffer.data)
    {
        m_groupBuffer.data[m_count] = nullptr;
    }

    if (m_accumulationBuffer)
    {
        m_accumulationBuffer[m_count] = 0.f;
    }

    if (m_accumulation2Buffer.data)
    {
        m_accumulation2Buffer.data[m_count] = SqVec2();
    }

    return m_count++;
}

int SqParticleSystemMemory::findParticle(int index) const
{
    SQ_ASSERT(index < m_count);
    return particleIdToBufferIndexMap[index];
}

void SqParticleSystemMemory::removeParticle(int *indicesState)
{
    int newIndex = 0;
    int particleCount = m_count;
    for (int i = 0; i < particleCount; ++i)
    {
        if (indicesState[i])
        {
            if (newIndex != i)
            {
                SqParticle *particle = system->findById(particleIdToBufferIndexMap[i]);
                particle->particleBufferIndex = newIndex;

                particleIdToBufferIndexMap[newIndex] = particleIdToBufferIndexMap[i];
                m_positionBuffer[newIndex] = m_positionBuffer[i];
                m_weightBuffer[newIndex] = m_weightBuffer[i];

                if (m_forceBuffer.data)
                {
                    m_forceBuffer.data[newIndex] = m_forceBuffer.data[i];
                }

                if (m_staticPressureBuffer.data)
                {
                    m_staticPressureBuffer.data[newIndex] = m_staticPressureBuffer.data[i];
                }

                if (m_depthBuffer.data)
                {
                    m_depthBuffer.data[newIndex] = m_depthBuffer.data[i];
                }

                if (m_colorBuffer.data)
                {
                    m_colorBuffer.data[newIndex] = m_colorBuffer.data[i];
                }

                if (m_groupBuffer.data)
                {
                    m_groupBuffer.data[newIndex] = m_groupBuffer.data[i];
                }

                if (m_userDataBuffer.data)
                {
                    m_userDataBuffer.data[newIndex] = m_userDataBuffer.data[i];
                }

                if (m_expirationTimeBuffer.data)
                {
                    m_expirationTimeBuffer.data[newIndex] = m_expirationTimeBuffer.data[i];
                }

                if (m_accumulation2Buffer.data)
                {
                    m_accumulation2Buffer.data[newIndex] = m_accumulation2Buffer.data[i];
                }
            }
            indicesState[i] = newIndex;
            ++newIndex;
        }
        else
        {
            // printf("++++remove Particle %d \n",particleIdToBufferIndexMap[i]);
            indicesState[i] = -1;
            --m_count;
        }
    }

    if (m_sortExpirationTimeBuffer.data)
    {
        newIndex = 0;
        for (int i = 0; i < particleCount; ++i)
        {
            int newParticleIndex = indicesState[m_sortExpirationTimeBuffer.data[i]];
            if (newParticleIndex != -1)
            {
                m_sortExpirationTimeBuffer.data[newIndex++] = newParticleIndex;
            }
        }
    }
}