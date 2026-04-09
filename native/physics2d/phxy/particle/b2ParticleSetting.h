#pragma once

#include <float.h>

typedef signed char	int8;
typedef signed short int16;
typedef signed int int32;
typedef unsigned char uint8;
typedef unsigned short uint16;
typedef unsigned int uint32;
typedef float float32;
typedef double float64;

#ifdef WIN32
typedef __int64   int64;
typedef unsigned __int64   uint64;
#else // !WIN32
typedef long long int64;
typedef unsigned long long uint64;
#endif


#define B2_NOT_USED(x) ((void)(x))

#define	sq_maxFloat		FLT_MAX

#define sq_maxManifoldPoints	2

#define sq_linearSlop			0.005f

/// 粒子的最小的个数。用于初始化缓冲区的大小。
/// 如果你设置maxCount小于这个则使用maxCount；如果maxCount大于这个则使用b2_minParticleSystemBufferCapacity
#define sq_minParticleSystemBufferCapacity	256

/// The time into the future that collisions against barrier particles will be detected.
#define sq_barrierCollisionTime 2.5f

#define sq_maxParticleIndex	0x7FFFFFFF

/// The default distance between particles, multiplied by the particle diameter.
#define sq_particleStride			0.75f

/// 设置粒子的权重值到达大多少才产生压力
/// 比如你设置为1，则权重需要 w > 1 的时候才才是权重值，否则为0
/// 控制重叠程度？
#define sq_minParticleWeight			0.1f

/// The upper limit for particle pressure.
#define sq_maxParticlePressure		1.75f

/// The upper limit for force between particles.
#define sq_maxParticleForce		0.5f

/// A symbolic constant that stands for particle allocation error.
#define sq_invalidParticleIndex		(-1)

#define sq_maxTriadDistance 2

#define sq_maxTriadDistanceSquared		(sq_maxTriadDistance * sq_maxTriadDistance)
