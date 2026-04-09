#include "PBD.h"
#include "../../../engine/core/math/math.h"
#include <algorithm>
#include <unordered_set>
#include <vector>

using namespace phxy;

PBD::PBD()
{
	m_count = 0;
	m_stretchCount = 0;
	m_bendCount = 0;
	m_stretchConstraints = nullptr;
	m_bendConstraints = nullptr;
	particles = nullptr;
}

PBD::~PBD()
{
	clear();
}

void PBD::clear()
{
	delete[] m_stretchConstraints;
	delete[] m_bendConstraints;
	delete[] particles;

	m_stretchConstraints = nullptr;
	m_bendConstraints = nullptr;
	particles = nullptr;
	m_count = 0;
	m_stretchCount = 0;
	m_bendCount = 0;
}

bool PBD::isValidPointIndex(int index) const
{
	return index >= 0 && index < m_count;
}

void PBD::syncConstraintMassesAndPrecompute()
{
	for (int i = 0; i < m_stretchCount; ++i)
	{
		PBDStretch &c = m_stretchConstraints[i];
		PBDParticle &particle1 = particles[c.i1];
		PBDParticle &particle2 = particles[c.i2];
		c.invMass1 = particle1.m_invMasses;
		c.invMass2 = particle2.m_invMasses;
	}

	for (int i = 0; i < m_bendCount; ++i)
	{
		PBDBend &c = m_bendConstraints[i];

		PBDParticle &particle1 = particles[c.i1];
		PBDParticle &particle2 = particles[c.i2];
		PBDParticle &particle3 = particles[c.i3];

		c.invMass1 = particle1.m_invMasses;
		c.invMass2 = particle2.m_invMasses;
		c.invMass3 = particle3.m_invMasses;

		const SqVec2 &p1 = particle1.m_bindPositions;
		const SqVec2 &p2 = particle2.m_bindPositions;
		const SqVec2 &p3 = particle3.m_bindPositions;

		const SqVec2 e1 = p2 - p1;
		const SqVec2 e2 = p3 - p2;
		const float L1sqr = SqVec2::LengthSquared(e1);
		const float L2sqr = SqVec2::LengthSquared(e2);
		if (L1sqr * L2sqr == 0.0f)
		{
			c.invEffectiveMass = 0.0f;
			c.alpha1 = 0.0f;
			c.alpha2 = 0.0f;
			continue;
		}

		const SqVec2 Jd1 = (-1.0f / L1sqr) * SqVec2::Skew(e1);
		const SqVec2 Jd2 = (1.0f / L2sqr) * SqVec2::Skew(e2);
		const SqVec2 J1 = -Jd1;
		const SqVec2 J2 = Jd1 - Jd2;
		const SqVec2 J3 = Jd2;
		c.invEffectiveMass = c.invMass1 * SqVec2::Dot(J1, J1) + c.invMass2 * SqVec2::Dot(J2, J2) + c.invMass3 * SqVec2::Dot(J3, J3);

		const SqVec2 r = p3 - p1;
		const float rr = SqVec2::LengthSquared(r);
		if (rr == 0.0f)
		{
			c.alpha1 = 0.0f;
			c.alpha2 = 0.0f;
			continue;
		}

		c.alpha1 = SqVec2::Dot(e2, r) / rr;
		c.alpha2 = SqVec2::Dot(e1, r) / rr;
	}
}

void PBD::setLinearVelocity(int pointIndex, const SqVec2 &velocity)
{
	if (!isValidPointIndex(pointIndex))
	{
		return;
	}
	particles[pointIndex].m_vs = velocity;
}

void PBD::move(int pointIndex, const SqVec2 &offset)
{
	SQ_ASSERT(pointIndex >= 0 && pointIndex < m_count);
	SqVec2 &pos = particles[pointIndex].m_ps;
	pos = SqVec2::Add(pos, offset);
}

void PBD::position(int pointIndex, const SqVec2 &position)
{
	SQ_ASSERT(pointIndex >= 0 && pointIndex < m_count);
	PBDParticle &particle = particles[pointIndex];
	particle.m_ps = position;
	particle.m_p0s = position;
	particle.m_vs.x = 0.f;
	particle.m_vs.y = 0.f;
}

void PBD::setMass(int pointIndex, float mass)
{
	SQ_ASSERT(pointIndex >= 0 && pointIndex < m_count);
	particles[pointIndex].m_invMasses = mass > 0.f ? 1.f / mass : 0.f;
	syncConstraintMassesAndPrecompute();
	updateTuning();
}

void PBD::Create(const PBDDef &def)
{
	clear();

	SQ_ASSERT(def.count > 0 && def.vertices != nullptr && def.masses != nullptr);

	m_count = def.count;
	particles = new PBDParticle[m_count];

	for (int i = 0; i < m_count; ++i)
	{
		PBDParticle &particle = particles[i];
		particle.m_bindPositions = def.vertices[i];
		particle.m_ps = particle.m_bindPositions;
		particle.m_p0s = particle.m_bindPositions;

		if (def.velocities)
		{
			particle.m_vs = def.velocities[i];
		}
		else
		{
			particle.m_vs.Set(0.f, 0.f);
		}

		float m = def.masses[i];
		if (m > 0.0f)
		{
			particle.m_invMasses = 1.0f / m;
		}
		else
		{
			particle.m_invMasses = 0.0f;
		}
	}

	std::vector<PBDStretchDef> stretchDefs;
	std::unordered_set<unsigned long long> stretchDedup;
	auto pairKey = [](int a, int b) -> unsigned long long
	{
		const int i1 = std::min(a, b);
		const int i2 = std::max(a, b);
		return (static_cast<unsigned long long>(static_cast<unsigned int>(i1)) << 32) | static_cast<unsigned int>(i2);
	};
	auto pushStretch = [&](int i1, int i2)
	{
		if (!isValidPointIndex(i1) || !isValidPointIndex(i2) || i1 == i2)
		{
			return;
		}

		// 通过index构建一个Key，避免重复对同一对点添加约束
		const unsigned long long key = pairKey(i1, i2);
		if (stretchDedup.find(key) != stretchDedup.end())
		{
			return;
		}

		stretchDedup.insert(key);
		PBDStretchDef c;
		c.i1 = i1;
		c.i2 = i2;
		stretchDefs.push_back(c);
	};

	const bool useCustomStretch = def.stretchConstraints != nullptr && def.stretchCount > 0;
	if (useCustomStretch)
	{
		for (int i = 0; i < def.stretchCount; ++i)
		{
			pushStretch(def.stretchConstraints[i].i1, def.stretchConstraints[i].i2);
		}
	}
	else
	{
		for (int i = 0; i < m_count - 1; ++i)
		{
			pushStretch(i, i + 1);
		}
	}

	m_stretchCount = static_cast<int>(stretchDefs.size());
	m_stretchConstraints = m_stretchCount > 0 ? new PBDStretch[m_stretchCount] : nullptr;

	// 预计算每个距离约束
	for (int i = 0; i < m_stretchCount; ++i)
	{
		PBDStretch &c = m_stretchConstraints[i];
		const int i1 = stretchDefs[i].i1;
		const int i2 = stretchDefs[i].i2;
		c.i1 = i1;
		c.i2 = i2;

		PBDParticle &particle1 = particles[i1];
		PBDParticle &particle2 = particles[i2];
		c.invMass1 = particle1.m_invMasses;
		c.invMass2 = particle2.m_invMasses;
		c.L = SqVec2::Length(particle1.m_ps - particle2.m_ps);
		c.lambda = 0.0f;
		c.damper = 0.0f;
		c.spring = 0.0f;
	}

	std::vector<PBDBendDef> bendDefs;
	std::unordered_set<unsigned long long> bendDedup;
	auto bendKey = [](int i1, int i2, int i3) -> unsigned long long
	{
		const int endMin = std::min(i1, i3);
		const int endMax = std::max(i1, i3);
		return (static_cast<unsigned long long>(static_cast<unsigned int>(i2)) << 40) |
			   (static_cast<unsigned long long>(static_cast<unsigned int>(endMin)) << 20) |
			   static_cast<unsigned int>(endMax);
	};
	auto pushBend = [&](int i1, int i2, int i3)
	{
		if (!isValidPointIndex(i1) || !isValidPointIndex(i2) || !isValidPointIndex(i3))
		{
			return;
		}
		if (i1 == i2 || i2 == i3 || i1 == i3)
		{
			return;
		}

		const unsigned long long key = bendKey(i1, i2, i3);
		if (bendDedup.find(key) != bendDedup.end())
		{
			return;
		}

		bendDedup.insert(key);
		PBDBendDef c;
		c.i1 = i1;
		c.i2 = i2;
		c.i3 = i3;
		bendDefs.push_back(c);
	};

	if (def.bendConstraints != nullptr && def.bendCount > 0)
	{
		// 自定义弯曲约束
		for (int i = 0; i < def.bendCount; ++i)
		{
			pushBend(def.bendConstraints[i].i1, def.bendConstraints[i].i2, def.bendConstraints[i].i3);
		}
	}
	else if (useCustomStretch && def.autoGenerateBendConstraints)
	{
		// 通过临近点生成弯曲约束
		// 临近点由上面的距离约束定义
		std::vector<std::vector<int>> neighbors;
		neighbors.resize(m_count);
		for (int i = 0; i < m_stretchCount; ++i)
		{
			const int a = m_stretchConstraints[i].i1;
			const int b = m_stretchConstraints[i].i2;

			// 记录每个点与哪些点相链的。是一对多关系
			neighbors[a].push_back(b);
			neighbors[b].push_back(a);
		}

		for (int center = 0; center < m_count; ++center)
		{
			auto &list = neighbors[center];
			std::sort(list.begin(), list.end());
			list.erase(std::unique(list.begin(), list.end()), list.end());

			for (int i = 0; i < static_cast<int>(list.size()); ++i)
			{
				for (int j = i + 1; j < static_cast<int>(list.size()); ++j)
				{
					pushBend(list[i], center, list[j]);
				}
			}
		}
	}
	else
	{
		// 生成绳子链式的弯曲约束
		for (int i = 0; i < m_count - 2; ++i)
		{
			pushBend(i, i + 1, i + 2);
		}
	}

	// 预计算所有的弯曲约束
	m_bendCount = static_cast<int>(bendDefs.size());
	m_bendConstraints = m_bendCount > 0 ? new PBDBend[m_bendCount] : nullptr;
	for (int i = 0; i < m_bendCount; ++i)
	{
		PBDBend &c = m_bendConstraints[i];
		c.i1 = bendDefs[i].i1;
		c.i2 = bendDefs[i].i2;
		c.i3 = bendDefs[i].i3;
		c.lambda = 0.0f;
		c.damper = 0.0f;
		c.spring = 0.0f;
		c.invEffectiveMass = 0.0f;

		const SqVec2 &p1 = particles[c.i1].m_ps;
		const SqVec2 &p2 = particles[c.i2].m_ps;
		const SqVec2 &p3 = particles[c.i3].m_ps;
		c.L1 = SqVec2::Length(p1 - p2);
		c.L2 = SqVec2::Length(p2 - p3);
	}

	syncConstraintMassesAndPrecompute();
	m_gravity = def.gravity;
	SetTuning(def.tuning);
}
void PBD::SetTuning(const PBDTuning &tuning)
{
	m_tuning = tuning;
	// Pre-compute spring and damper values based on tuning
	updateTuning();
}

void PBD::updateTuning()
{

	const float bendOmega = 2.0f * Math::PI * m_tuning.bendHertz;

	for (int i = 0; i < m_bendCount; ++i)
	{
		PBDBend &c = m_bendConstraints[i];

		float L1sqr = c.L1 * c.L1;
		float L2sqr = c.L2 * c.L2;

		if (L1sqr * L2sqr == 0.0f)
		{
			c.spring = 0.0f;
			c.damper = 0.0f;
			continue;
		}

		// Flatten the triangle formed by the two edges
		float J2 = 1.0f / c.L1 + 1.0f / c.L2;
		float sum = c.invMass1 / L1sqr + c.invMass2 * J2 * J2 + c.invMass3 / L2sqr;
		if (sum == 0.0f)
		{
			c.spring = 0.0f;
			c.damper = 0.0f;
			continue;
		}

		float mass = 1.0f / sum;

		c.spring = mass * bendOmega * bendOmega;
		c.damper = 2.0f * mass * m_tuning.bendDamping * bendOmega;
	}

	const float stretchOmega = 2.0f * Math::PI * m_tuning.stretchHertz;

	for (int i = 0; i < m_stretchCount; ++i)
	{
		PBDStretch &c = m_stretchConstraints[i];

		float sum = c.invMass1 + c.invMass2;
		if (sum == 0.0f)
		{
			continue;
		}

		float mass = 1.0f / sum;

		c.spring = mass * stretchOmega * stretchOmega;
		c.damper = 2.0f * mass * m_tuning.stretchDamping * stretchOmega;
	}
}

void PBD::Step(float dt, int iterations)
{
	if (paused || dt == 0.0f)
	{
		return;
	}

	const float inv_dt = 1.0f / dt;

	// 计算的是数学中的eˣ
	float d = expf(-dt * m_tuning.damping);

	SqVec2 temp;
	// Apply gravity and damping
	for (int i = 0; i < m_count; ++i)
	{
		PBDParticle &particle = particles[i];
		if (particle.m_invMasses > 0.0f)
		{

			// if (particle.userData)
			// {
			// 	printf(">>>>>>>>>>>>>>before %f %f \n", particle.m_vs.x, particle.m_vs.y);
			// }
			particle.m_vs *= d;
			particle.m_vs += dt * m_gravity;

			// if (particle.userData)
			// {
			// 	printf(">>>>>>>>>>>>>>after %f %f \n", particle.m_vs.x, particle.m_vs.y);
			// }
		}
		// else
		// {
		// 	m_vs[i] = inv_dt * (m_bindPositions[i] - m_p0s[i]);
		// }
	}

	// Apply bending spring
	if (m_tuning.bendingModel == PBDBendingModel::springAngleBendingModel)
	{
		ApplyBendForces(dt);
	}

	for (int i = 0; i < m_bendCount; ++i)
	{
		m_bendConstraints[i].lambda = 0.0f;
	}

	for (int i = 0; i < m_stretchCount; ++i)
	{
		m_stretchConstraints[i].lambda = 0.0f;
	}

	// Update position
	for (int i = 0; i < m_count; ++i)
	{
		PBDParticle &particle = particles[i];
		if (particle.m_invMasses > 0.0f)
		{
			particle.m_ps += dt * particle.m_vs;
		}
	}

	// Solve constraints
	for (int i = 0; i < iterations; ++i)
	{

		// 计算弯曲约束
		if (m_tuning.bendingModel == PBDBendingModel::pbdAngleBendingModel)
		{
			SolveBend_PBD_Angle();
		}
		else if (m_tuning.bendingModel == PBDBendingModel::xpbdAngleBendingModel)
		{
			SolveBend_XPBD_Angle(dt);
		}
		else if (m_tuning.bendingModel == PBDBendingModel::pbdDistanceBendingModel)
		{
			SolveBend_PBD_Distance();
		}
		else if (m_tuning.bendingModel == PBDBendingModel::pbdHeightBendingModel)
		{
			SolveBend_PBD_Height();
		}
		else if (m_tuning.bendingModel == PBDBendingModel::pbdTriangleBendingModel)
		{
			SolveBend_PBD_Triangle();
		}

		// 计算距离约束
		if (m_tuning.stretchingModel == PBDStretchingModel::pbdStretchingModel)
		{
			SolveStretch_PBD();
		}
		else if (m_tuning.stretchingModel == PBDStretchingModel::xpbdStretchingModel)
		{
			SolveStretch_XPBD(dt);
		}
	}

	for (int i = 0; i < m_count; ++i)
	{
		PBDParticle &particle = particles[i];
		// 根据上一个位置和当前位置，计算改变的距离，距离除以时间得出速度
		particle.m_vs = inv_dt * (particle.m_ps - particle.m_p0s);
		particle.m_p0s = particle.m_ps;
	}
}

void PBD::Reset()
{
	// m_position = position;

	for (int i = 0; i < m_count; ++i)
	{
		PBDParticle &particle = particles[i];
		particle.m_p0s = particle.m_bindPositions;
		particle.m_vs.Set(0.f, 0.f);
	}

	for (int i = 0; i < m_bendCount; ++i)
	{
		m_bendConstraints[i].lambda = 0.0f;
	}

	for (int i = 0; i < m_stretchCount; ++i)
	{
		m_stretchConstraints[i].lambda = 0.0f;
	}
}

/**
 * 求解距离约束--使用PBD实现的方式
 */
void PBD::SolveStretch_PBD()
{
	const float stiffness = m_tuning.stretchStiffness;
	SqVec2 d, temp, p1, p2;
	for (int i = 0; i < m_stretchCount; ++i)
	{
		const PBDStretch &c = m_stretchConstraints[i];

		if (c.invMass1 == 0.0f && c.invMass2 == 0.f)
		{
			continue;
		}

		PBDParticle &particle1 = particles[c.i1];
		PBDParticle &particle2 = particles[c.i2];
		p1 = particle1.m_ps;
		p2 = particle2.m_ps;

		SqVec2 d = p2 - p1;
		float L = d.Normalize();

		float sum = c.invMass1 + c.invMass2;
		if (sum == 0.0f)
		{
			continue;
		}

		float s1 = c.invMass1 / sum;
		float s2 = c.invMass2 / sum;

		p1 -= stiffness * s1 * (c.L - L) * d;
		p2 += stiffness * s2 * (c.L - L) * d;

		particle1.m_ps = p1;
		particle2.m_ps = p2;
	}
}

/**
 * 求解距离约束--使用XPBD实现的方式
 */
void PBD::SolveStretch_XPBD(float dt)
{

	SqVec2 dp1, dp2, u, J1, J2, p1, p2;
	for (int i = 0; i < m_stretchCount; ++i)
	{
		PBDStretch &c = m_stretchConstraints[i];

		PBDParticle &particle1 = particles[c.i1];
		PBDParticle &particle2 = particles[c.i2];

		p1 = particle1.m_ps;
		p2 = particle2.m_ps;

		dp1 = p1 - particle1.m_p0s;
		dp2 = p2 - particle2.m_p0s;

		u = p2 - p1;
		float L = u.Normalize();

		J1.Set(-u.x, -u.y);
		J2 = u;

		float sum = c.invMass1 + c.invMass2;
		if (sum == 0.0f)
		{
			continue;
		}

		const float alpha = 1.0f / (c.spring * dt * dt); // 1 / kg
		const float beta = dt * dt * c.damper;			 // kg * s
		const float sigma = alpha * beta / dt;			 // non-dimensional
		float C = L - c.L;

		// This is using the initial velocities
		float Cdot = SqVec2::Dot(J1, dp1) + SqVec2::Dot(J2, dp2);

		float B = C + alpha * c.lambda + sigma * Cdot;
		float sum2 = (1.0f + sigma) * sum + alpha;

		float impulse = -B / sum2;

		p1 += (c.invMass1 * impulse) * J1;
		p2 += (c.invMass2 * impulse) * J2;

		particle1.m_ps = p1;
		particle1.m_ps = p2;
		c.lambda += impulse;
	}
}

/**
 * 求解弯曲约束--使用PBD角度约束实现的方式
 */
void PBD::SolveBend_PBD_Angle()
{
	const float stiffness = m_tuning.bendStiffness;

	SqVec2 temp, d1, d2, Jd1, Jd2, J1, J2, J3, p1, p2, p3;
	for (int i = 0; i < m_bendCount; ++i)
	{
		const PBDBend &c = m_bendConstraints[i];

		if (c.invMass1 == 0.0f && c.invMass2 == 0.f && c.invMass3 == 0.0f)
		{
			continue;
		}

		PBDParticle &particle1 = particles[c.i1];
		PBDParticle &particle2 = particles[c.i2];
		PBDParticle &particle3 = particles[c.i3];

		p1 = particle1.m_ps;
		p2 = particle2.m_ps;
		p3 = particle3.m_ps;

		SqVec2 d1 = p2 - p1;
		SqVec2 d2 = p3 - p2;

		float a = SqVec2::Cross(d1, d2);
		float b = SqVec2::Dot(d1, d2);

		float angle = atan2(a, b);

		float L1sqr, L2sqr;

		if (m_tuning.isometric)
		{
			L1sqr = c.L1 * c.L1;
			L2sqr = c.L2 * c.L2;
		}
		else
		{
			L1sqr = SqVec2::LengthSquared(d1);
			L2sqr = SqVec2::LengthSquared(d2);
		}

		if (L1sqr * L2sqr == 0.0f)
		{
			continue;
		}

		SqVec2 Jd1 = (-1.0f / L1sqr) * SqVec2::Skew(d1);
		SqVec2 Jd2 = (1.0f / L2sqr) * SqVec2::Skew(d2);

		SqVec2 J1 = -Jd1;
		SqVec2 J2 = Jd1 - Jd2;
		SqVec2 J3 = Jd2;

		float sum;
		if (m_tuning.fixedEffectiveMass)
		{
			sum = c.invEffectiveMass;
		}
		else
		{
			sum = c.invMass1 * SqVec2::Dot(J1, J1) + c.invMass2 * SqVec2::Dot(J2, J2) + c.invMass3 * SqVec2::Dot(J3, J3);
		}

		if (sum == 0.0f)
		{
			sum = c.invEffectiveMass;
		}

		if (sum == 0.0f)
		{
			continue;
		}

		float impulse = -stiffness * angle / sum;

		// p1 += (c.invMass1 * impulse) * J1;
		// p2 += (c.invMass2 * impulse) * J2;
		// p3 += (c.invMass3 * impulse) * J3;

		// printf("%f %f %f\n",stiffness , angle,sum);

		if (c.invMass1 > 0.f)
		{
			p1 += (c.invMass1 * impulse) * J1;
		}

		if (c.invMass2 > 0.f)
		{
			p2 += (c.invMass2 * impulse) * J2;
		}

		if (c.invMass3 > 0.f)
		{
			p3 += (c.invMass3 * impulse) * J3;
		}

		particle1.m_ps = p1;
		particle2.m_ps = p2;
		particle3.m_ps = p3;
	}

	// for(int i = 0 ; i < m_count ; ++i){
	// 	printf("check bb %f %f \n",m_ps[i].x,m_ps[i].y);
	// }
}

/**
 * 求解弯曲约束--使用XPBD角度约束实现的方式
 */
void PBD::SolveBend_XPBD_Angle(float dt)
{

	for (int i = 0; i < m_bendCount; ++i)
	{
		PBDBend &c = m_bendConstraints[i];

		PBDParticle &particle1 = particles[c.i1];
		PBDParticle &particle2 = particles[c.i2];
		PBDParticle &particle3 = particles[c.i3];

		SqVec2 p1 = particle1.m_ps;
		SqVec2 p2 = particle2.m_ps;
		SqVec2 p3 = particle3.m_ps;

		SqVec2 dp1 = p1 - particle1.m_p0s;
		SqVec2 dp2 = p2 - particle2.m_p0s;
		SqVec2 dp3 = p3 - particle3.m_p0s;

		SqVec2 d1 = p2 - p1;
		SqVec2 d2 = p3 - p2;

		float L1sqr, L2sqr;

		if (m_tuning.isometric)
		{
			L1sqr = c.L1 * c.L1;
			L2sqr = c.L2 * c.L2;
		}
		else
		{
			L1sqr = SqVec2::LengthSquared(d1);
			L2sqr = SqVec2::LengthSquared(d2);
		}

		if (L1sqr * L2sqr == 0.0f)
		{
			continue;
		}

		float a = SqVec2::Cross(d1, d2);
		float b = SqVec2::Dot(d1, d2);

		float angle = atan2(a, b);

		SqVec2 Jd1 = (-1.0f / L1sqr) * SqVec2::Skew(d1);
		SqVec2 Jd2 = (1.0f / L2sqr) * SqVec2::Skew(d2);

		SqVec2 J1 = -Jd1;
		SqVec2 J2 = Jd1 - Jd2;
		SqVec2 J3 = Jd2;

		float sum;
		if (m_tuning.fixedEffectiveMass)
		{
			sum = c.invEffectiveMass;
		}
		else
		{
			sum = c.invMass1 * SqVec2::Dot(J1, J1) + c.invMass2 * SqVec2::Dot(J2, J2) + c.invMass3 * SqVec2::Dot(J3, J3);
		}

		if (sum == 0.0f)
		{
			continue;
		}

		const float alpha = 1.0f / (c.spring * dt * dt);
		const float beta = dt * dt * c.damper;
		const float sigma = alpha * beta / dt;
		float C = angle;

		// This is using the initial velocities
		float Cdot = SqVec2::Dot(J1, dp1) + SqVec2::Dot(J2, dp2) + SqVec2::Dot(J3, dp3);

		float B = C + alpha * c.lambda + sigma * Cdot;
		float sum2 = (1.0f + sigma) * sum + alpha;

		float impulse = -B / sum2;

		p1 += (c.invMass1 * impulse) * J1;
		p2 += (c.invMass2 * impulse) * J2;
		p3 += (c.invMass3 * impulse) * J3;

		particle1.m_ps = p1;
		particle2.m_ps = p2;
		particle3.m_ps = p3;
		c.lambda += impulse;
	}
}

void PBD::ApplyBendForces(float dt)
{
	// omega = 2 * pi * hz
	const float omega = 2.0f * Math::PI * m_tuning.bendHertz;

	SqVec2 Jd1, Jd2, d1, d2, J1, J2, J3;
	for (int i = 0; i < m_bendCount; ++i)
	{
		const PBDBend &c = m_bendConstraints[i];

		PBDParticle &particle1 = particles[c.i1];
		PBDParticle &particle2 = particles[c.i2];
		PBDParticle &particle3 = particles[c.i3];

		const SqVec2 &p1 = particle1.m_ps;
		const SqVec2 &p2 = particle2.m_ps;
		const SqVec2 &p3 = particle3.m_ps;

		const SqVec2 &v1 = particle1.m_vs;
		const SqVec2 &v2 = particle2.m_vs;
		const SqVec2 &v3 = particle3.m_vs;

		// b2Vec2 d1 = p2 - p1;
		// b2Vec2 d2 = p3 - p2;

		d1.Sub(p2, p1);
		d2.Sub(p3, p2);

		float L1sqr, L2sqr;

		if (m_tuning.isometric)
		{
			L1sqr = c.L1 * c.L1;
			L2sqr = c.L2 * c.L2;
		}
		else
		{
			L1sqr = SqVec2::LengthSquared(d1);
			L2sqr = SqVec2::LengthSquared(d2);
		}

		if (L1sqr * L2sqr == 0.0f)
		{
			continue;
		}

		float a = SqVec2::Cross(d1, d2);
		float b = SqVec2::Dot(d1, d2);

		float angle = atan2(a, b);

		SqVec2 Jd1 = (-1.0f / L1sqr) * SqVec2::Skew(d1);
		SqVec2 Jd2 = (1.0f / L2sqr) * SqVec2::Skew(d2);

		SqVec2 J1 = -Jd1;
		SqVec2 J2 = Jd1 - Jd2;
		SqVec2 J3 = Jd2;

		float sum;
		if (m_tuning.fixedEffectiveMass)
		{
			sum = c.invEffectiveMass;
		}
		else
		{
			sum = c.invMass1 * SqVec2::Dot(J1, J1) + c.invMass2 * SqVec2::Dot(J2, J2) + c.invMass3 * SqVec2::Dot(J3, J3);
		}

		if (sum == 0.0f)
		{
			continue;
		}

		float mass = 1.0f / sum;

		const float spring = mass * omega * omega;
		const float damper = 2.0f * mass * m_tuning.bendDamping * omega;

		float C = angle;
		float Cdot = SqVec2::Dot(J1, v1) + SqVec2::Dot(J2, v2) + SqVec2::Dot(J3, v3);

		float impulse = -dt * (spring * C + damper * Cdot);

		particle1.m_vs += (c.invMass1 * impulse) * J1;
		particle2.m_vs += (c.invMass2 * impulse) * J2;
		particle3.m_vs += (c.invMass3 * impulse) * J3;
	}
}

/**
 * 求解弯曲约束--使用距离约束实现的方式
 */
void PBD::SolveBend_PBD_Distance()
{
	const float stiffness = m_tuning.bendStiffness;

	SqVec2 p1, p2, d, tmp;
	for (int i = 0; i < m_bendCount; ++i)
	{
		const PBDBend &c = m_bendConstraints[i];

		float sum = c.invMass1 + c.invMass3;
		if (sum == 0.0f)
		{
			continue;
		}

		int i1 = c.i1;
		int i2 = c.i3;

		PBDParticle &particle1 = particles[i1];
		PBDParticle &particle2 = particles[i2];

		p1 = particle1.m_ps;
		p2 = particle2.m_ps;

		SqVec2 d = p2 - p1;
		float L = d.Normalize();

		float s1 = c.invMass1 / sum;
		float s2 = c.invMass3 / sum;

		p1 -= stiffness * s1 * (c.L1 + c.L2 - L) * d;
		p2 += stiffness * s2 * (c.L1 + c.L2 - L) * d;

		particle1.m_ps = p1;
		particle2.m_ps = p2;
	}
}

/**
 * 求解弯曲约束--使用PBD Height Based 实现的方式
 */
// Constraint based implementation of:
// P. Volino: Simple Linear Bending Stiffness in Particle Systems
void PBD::SolveBend_PBD_Height()
{
	const float stiffness = m_tuning.bendStiffness;

	SqVec2 p1, p2, p3, d, temp, dHat, J1, J2, J3;
	for (int i = 0; i < m_bendCount; ++i)
	{
		const PBDBend &c = m_bendConstraints[i];

		PBDParticle &particle1 = particles[c.i1];
		PBDParticle &particle2 = particles[c.i2];
		PBDParticle &particle3 = particles[c.i3];

		p1 = particle1.m_ps;
		p2 = particle2.m_ps;
		p3 = particle3.m_ps;

		// Barycentric coordinates are held constant
		SqVec2 d = c.alpha1 * p1 + c.alpha2 * p3 - p2;

		float dLen = SqVec2::Length(d);

		if (dLen == 0.0f)
		{
			continue;
		}

		dHat = (1.0f / dLen) * d;

		SqVec2 J1 = c.alpha1 * dHat;
		SqVec2 J2 = -dHat;
		SqVec2 J3 = c.alpha2 * dHat;

		float sum = c.invMass1 * c.alpha1 * c.alpha1 + c.invMass2 + c.invMass3 * c.alpha2 * c.alpha2;

		if (sum == 0.0f)
		{
			continue;
		}

		float C = dLen;
		float mass = 1.0f / sum;
		float impulse = -stiffness * mass * C;

		p1 += (c.invMass1 * impulse) * J1;
		p2 += (c.invMass2 * impulse) * J2;
		p3 += (c.invMass3 * impulse) * J3;

		particle1.m_ps = p1;
		particle2.m_ps = p2;
		particle3.m_ps = p3;
	}
}

/**
 * 求解弯曲约束--使用PBD Triangle实现的方式
 */
// M. Kelager: A Triangle Bending Constraint Model for PBD
void PBD::SolveBend_PBD_Triangle()
{
	const float stiffness = m_tuning.bendStiffness;
	SqVec2 temp, b0, v, b1, d, db0, dv, db1;
	for (int i = 0; i < m_bendCount; ++i)
	{
		const PBDBend &c = m_bendConstraints[i];

		PBDParticle &particle1 = particles[c.i1];
		PBDParticle &particle2 = particles[c.i2];
		PBDParticle &particle3 = particles[c.i3];

		b0 = particle1.m_ps;
		v = particle2.m_ps;
		b1 = particle3.m_ps;

		float wb0 = c.invMass1;
		float wv = c.invMass2;
		float wb1 = c.invMass3;

		float W = wb0 + wb1 + 2.0f * wv;
		float invW = stiffness / W;

		// SqVec2 d = v - (1.0f / 3.0f) * (b0 + v + b1);
		SqVec2 d = v - 0.33333f * (b0 + v + b1);

		SqVec2 db0 = 2.0f * wb0 * invW * d;
		SqVec2 dv = -4.0f * wv * invW * d;
		SqVec2 db1 = 2.0f * wb1 * invW * d;

		b0 += db0;
		v += dv;
		b1 += db1;

		particle1.m_ps = b0;
		particle2.m_ps = v;
		particle3.m_ps = b1;
	}
}

void PBD::rayCast(SqPBDRayCastOut &output, const SqRayCastInput &input) const
{

	// 查看SqEdgeShape的射线检测的实现，这里使用SqEdgeShape那边的代码实现的射线检测
	const SqVec2 &p1 = input.origin;
	const SqVec2 &d = input.translation;

	for (int i = 0; i < m_stretchCount; ++i)
	{
		const int i1 = m_stretchConstraints[i].i1;
		const int i2 = m_stretchConstraints[i].i2;

		PBDParticle &particle1 = particles[i1];
		PBDParticle &particle2 = particles[i2];

		SqVec2 &v1 = particle1.m_ps;
		SqVec2 &v2 = particle2.m_ps;
		SqVec2 eUnit = v2 - v1;
		float length = eUnit.Normalize();
		if (length == 0.0f)
		{
			continue;
		}

		SqVec2 normal = SqVec2::RightPerp(eUnit);
		float numerator = SqVec2::Dot(normal, SqVec2::Sub(v1, p1));

		float denominator = SqVec2::Dot(normal, d);

		if (denominator == 0.0f)
		{
			continue;
		}

		float t = numerator / denominator;
		if (t < 0.0f || input.maxFraction < t)
		{
			continue;
		}

		SqVec2 p = SqVec2::MulAdd(p1, t, d);

		float s = SqVec2::Dot(SqVec2::Sub(p, v1), eUnit);
		if (s < 0.0f || length < s)
		{
			continue;
		}

		if (numerator > 0.0f)
		{
			normal = SqVec2::Neg(normal);
		}

		output.fraction = t;
		output.point = p;
		output.normal = normal;
		output.hit = true;
		output.pointAIndex = i1;
		output.pointBIndex = i2;
		break;
	}
}

#ifdef PHYSICS_DEBUG
void PBD::Draw(SqDebugDraw *draw) const
{
	SqVec2 point1, point2;

	// printf("=========\n");

	for (int i = 0; i < m_stretchCount; ++i)
	{
		const int i1 = m_stretchConstraints[i].i1;
		const int i2 = m_stretchConstraints[i].i2;
		PBDParticle &particle1 = particles[i1];
		PBDParticle &particle2 = particles[i2];

		point1.x = particle1.m_ps.x;
		point1.y = particle1.m_ps.y;
		point1 = SqTransform::transformPoint(transform, point1);

		point2.x = particle2.m_ps.x;
		point2.y = particle2.m_ps.y;
		point2 = SqTransform::transformPoint(transform, point2);

		// printf("--------------------line %d %f %f %f %f\n",i,point1.x,point1.y,point2.x,point2.y);

		draw->DrawSegmentFcn(point1, point2, SqHexColor::sq_colorBlue, draw->context);
	}

	for (int i = 0; i < m_count; ++i)
	{
		PBDParticle &particle = particles[i];
		const SqHexColor &pc = particle.m_invMasses > 0.0f ? SqHexColor::sq_colorGreen : SqHexColor::sq_colorDarkGray;
		point1.x = particle.m_ps.x;
		point1.y = particle.m_ps.y;
		point1 = SqTransform::transformPoint(transform, point1);
		draw->DrawPointFcn(point1, 0.2f, pc, draw->context);
	}
}
#endif
