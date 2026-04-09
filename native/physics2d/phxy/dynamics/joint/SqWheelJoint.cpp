#include "SqWheelJoint.h"
#include "../sim/SqBodySim.h"
#include "../sim/SqJointSim.h"
#include "../../common/math/SqMath.h"
#include "../SqWorld.h"
#include <engine/scene/Node.h>

using namespace phxy;

SqWheelJoint::SqWheelJoint()
{
    type = SqJointType::sq_wheelJoint;
}

void SqWheelJoint::EnableSpring(bool enableSpring)
{
    if (enableSpring != this->enableSpring)
    {
        this->enableSpring = enableSpring;
        this->springImpulse = 0.0f;
    }
}

bool SqWheelJoint::IsSpringEnabled()
{
    return enableSpring;
}

void SqWheelJoint::SetSpringHertz(float hertz)
{
    this->hertz = hertz;
}

float SqWheelJoint::GetSpringHertz()
{
    return hertz;
}

void SqWheelJoint::SetSpringDampingRatio(float dampingRatio)
{
    this->dampingRatio = dampingRatio;
}

float SqWheelJoint::GetSpringDampingRatio()
{
    return dampingRatio;
}

void SqWheelJoint::EnableLimit(bool enableLimit)
{
    if (this->enableLimit != enableLimit)
    {
        lowerImpulse = 0.0f;
        upperImpulse = 0.0f;
        this->enableLimit = enableLimit;
    }
}

bool SqWheelJoint::IsLimitEnabled()
{
    return enableLimit;
}

float SqWheelJoint::GetLowerLimit()
{
    return lowerTranslation;
}

float SqWheelJoint::GetUpperLimit()
{
    return upperTranslation;
}

void SqWheelJoint::SetLimits(float lower, float upper)
{
    if (lower != this->lowerTranslation || upper != this->upperTranslation)
    {
        lowerTranslation = min(lower, upper);
        upperTranslation = max(lower, upper);
        lowerImpulse = 0.0f;
        upperImpulse = 0.0f;
    }
}

void SqWheelJoint::EnableMotor(bool enableMotor)
{
    if (this->enableMotor != enableMotor)
    {
        motorImpulse = 0.0f;
        this->enableMotor = enableMotor;
    }
}

bool SqWheelJoint::IsMotorEnabled()
{
    return enableMotor;
}

void SqWheelJoint::SetMotorSpeed(float motorSpeed)
{
    this->motorSpeed = motorSpeed;
}

float SqWheelJoint::GetMotorSpeed()
{
    return motorSpeed;
}

float SqWheelJoint::GetMotorTorque()
{
    // b2World *world = b2GetWorld(jointId.world0);
    // SqJointSim *this = b2GetJointSimCheckType(jointId, b2_wheelJoint);
    // return world->inv_h * this->wheelJoint.motorImpulse;
    return 0.f;
}

void SqWheelJoint::SetMaxMotorTorque(float torque)
{
    maxMotorTorque = torque;
}

float SqWheelJoint::GetMaxMotorTorque()
{
    return maxMotorTorque;
}

SqVec2 SqWheelJoint::GetWheelJointForce()
{
    // int idA = base->bodyIdA;
    // b2Transform transformA = b2GetBodyTransform(world, idA);

    // SqVec2 localAxisA = b2RotateVector(base->localFrameA.q, (SqVec2){1.0f, 0.0f});
    // SqVec2 axisA = b2RotateVector(transformA.q, localAxisA);
    // SqVec2 perpA = b2LeftPerp(axisA);

    // b2WheelJoint *this = &base->wheelJoint;

    // float perpForce = world->inv_h * this->perpImpulse;
    // float axialForce = world->inv_h * (this->springImpulse + this->lowerImpulse - this->upperImpulse);

    // SqVec2 force = SqVec2::Add(SqVec2::MulSV(perpForce, perpA), SqVec2::MulSV(axialForce, axisA));
    // return force;
    return SqVec2();
}

// float GetWheelJointTorque(b2World *world, SqJointSim *base)
// {
//     return world->inv_h * base->wheelJoint.motorImpulse;
// }

// Linear constraint (point-to-line)
// d = pB - pA = xB + rB - xA - rA
// C = dot(ay, d)
// Cdot = dot(d, cross(wA, ay)) + dot(ay, vB + cross(wB, rB) - vA - cross(wA, rA))
//      = -dot(ay, vA) - dot(cross(d + rA, ay), wA) + dot(ay, vB) + dot(cross(rB, ay), vB)
// J = [-ay, -cross(d + rA, ay), ay, cross(rB, ay)]

// Spring linear constraint
// C = dot(ax, d)
// Cdot = = -dot(ax, vA) - dot(cross(d + rA, ax), wA) + dot(ax, vB) + dot(cross(rB, ax), vB)
// J = [-ax -cross(d+rA, ax) ax cross(rB, ax)]

// Motor rotational constraint
// Cdot = wB - wA
// J = [0 0 -1 0 0 1]

void SqWheelJoint::solvePrepare(const SqStepContext &context)
{
    SqJointSim *sim = getJointSim();
    SqBody *bodyA = world->getBody(sim->bodyAIndex);
    SqBody *bodyB = world->getBody(sim->bodyBIndex);
    SQ_ASSERT(bodyA->isAwake() || bodyB->isAwake());
    SqBodySim *bodySimA = bodyA->getBodySim();
    SqBodySim *bodySimB = bodyB->getBodySim();

    float mA = bodySimA->invMass;
    float iA = bodySimA->invInertia;
    float mB = bodySimB->invMass;
    float iB = bodySimB->invInertia;

    // Compute this anchor frames with world space rotation, relative to center of mass
    frameA.q = SqRot::Mul(bodySimA->transform.q, sim->localFrameA.q);
    frameA.p = SqTransform::transformVector(bodySimA->transform, SqVec2::Sub(sim->localFrameA.p, bodySimA->localCenter));
    frameB.q = SqRot::Mul(bodySimB->transform.q, sim->localFrameB.q);
    frameB.p = SqTransform::transformVector(bodySimB->transform, SqVec2::Sub(sim->localFrameB.p, bodySimB->localCenter));

    // printf("localFrameA %f %f localFrameB %f %f \n", sim->localFrameA.p.x, sim->localFrameA.p.y,sim->localFrameB.p.x,sim->localFrameB.p.y);

    // Compute the initial center delta. Incremental position updates are relative to this.
    deltaCenter = SqVec2::Sub(bodySimB->center, bodySimA->center);

    const SqVec2 &rA = frameA.p;
    const SqVec2 &rB = frameB.p;

    SqVec2 d = SqVec2::Add(deltaCenter, SqVec2::Sub(rB, rA));
    SqVec2 axisA = SqTransform::transformVector(frameA, SqVec2(1.0f, 0.0f));
    SqVec2 perpA = SqVec2::LeftPerp(axisA);

    // perpendicular constraint (keep wheel on line)
    float s1 = SqVec2::Cross(SqVec2::Add(d, rA), perpA);
    float s2 = SqVec2::Cross(rB, perpA);

    float kp = mA + mB + iA * s1 * s1 + iB * s2 * s2;
    this->perpMass = kp > 0.0f ? 1.0f / kp : 0.0f;

    // spring constraint
    float a1 = SqVec2::Cross(SqVec2::Add(d, rA), axisA);
    float a2 = SqVec2::Cross(rB, axisA);

    float ka = mA + mB + iA * a1 * a1 + iB * a2 * a2;
    this->axialMass = ka > 0.0f ? 1.0f / ka : 0.0f;

    this->springSoftness.step(this->hertz, this->dampingRatio, context.h);

    float km = iA + iB;
    this->motorMass = km > 0.0f ? 1.0f / km : 0.0f;

    if (!world->isEnableWarmdStart())
    {
        this->perpImpulse = 0.0f;
        this->springImpulse = 0.0f;
        this->motorImpulse = 0.0f;
        this->lowerImpulse = 0.0f;
        this->upperImpulse = 0.0f;
    }
}

void SqWheelJoint::warmStart(const SqStepContext &context)
{
    SqJointSim *sim = getJointSim();
    SqBody *bodyA = world->getBody(sim->bodyAIndex);
    SqBody *bodyB = world->getBody(sim->bodyBIndex);
    SqBodySim *bodySimA = bodyA->getBodySim();
    SqBodySim *bodySimB = bodyB->getBodySim();

    float mA = bodySimA->invMass;
    float iA = bodySimA->invInertia;
    float mB = bodySimB->invMass;
    float iB = bodySimB->invInertia;

    // dummy state for static bodies
    SqBodySim dummyState;

    SqBodySim *stateA = bodyA->isAwake() ? bodySimA : &dummyState;
    SqBodySim *stateB = bodyB->isAwake() ? bodySimB : &dummyState;

    SqVec2 rA = SqRot::transformVector(stateA->deltaRotation, this->frameA.p);
    SqVec2 rB = SqRot::transformVector(stateB->deltaRotation, this->frameB.p);

    SqVec2 d = SqVec2::Add(SqVec2::Add(SqVec2::Sub(stateB->deltaPosition, stateA->deltaPosition), this->deltaCenter), SqVec2::Sub(rB, rA));
    SqVec2 axisA = SqRot::transformVector(this->frameA.q, SqVec2(1.0f, 0.0f));
    axisA = SqRot::transformVector(stateA->deltaRotation, axisA);
    SqVec2 perpA = SqVec2::LeftPerp(axisA);

    float a1 = SqVec2::Cross(SqVec2::Add(d, rA), axisA);
    float a2 = SqVec2::Cross(rB, axisA);
    float s1 = SqVec2::Cross(SqVec2::Add(d, rA), perpA);
    float s2 = SqVec2::Cross(rB, perpA);

    float axialImpulse = this->springImpulse + this->lowerImpulse - this->upperImpulse;

    SqVec2 P = SqVec2::Add(SqVec2::MulSV(axialImpulse, axisA), SqVec2::MulSV(this->perpImpulse, perpA));
    float LA = axialImpulse * a1 + this->perpImpulse * s1 + this->motorImpulse;
    float LB = axialImpulse * a2 + this->perpImpulse * s2 + this->motorImpulse;

    // printf("warmStart before bodyA %d vA:(%f %f) wA:%f bodyB %d vB(%f %f) wB:%f \n",
    //        bodySimA->bodyIndex, stateA->linearVelocity.x, stateA->linearVelocity.y, stateA->angularVelocity,
    //        bodySimB->bodyIndex, stateB->linearVelocity.x, stateB->linearVelocity.y, stateB->angularVelocity);

    stateA->linearVelocity = SqVec2::MulSub(stateA->linearVelocity, mA, P);
    stateA->angularVelocity -= iA * LA;
    stateB->linearVelocity = SqVec2::MulAdd(stateB->linearVelocity, mB, P);
    stateB->angularVelocity += iB * LB;

    // printf("warmStart after bodyA %d vA:(%f %f) wA:%f bodyB %d vB(%f %f) wB:%f \n",
    //        bodySimA->bodyIndex, stateA->linearVelocity.x, stateA->linearVelocity.y, stateA->angularVelocity,
    //        bodySimB->bodyIndex, stateB->linearVelocity.x, stateB->linearVelocity.y, stateB->angularVelocity);
}

void SqWheelJoint::solve(const SqStepContext &context, bool useBias)
{

    SqJointSim *jointSim = getJointSim();
    SqBody *bodyA = world->getBody(jointSim->bodyAIndex);
    SqBody *bodyB = world->getBody(jointSim->bodyBIndex);
    SqBodySim *bodySimA = bodyA->getBodySim();
    SqBodySim *bodySimB = bodyB->getBodySim();

    float mA = bodySimA->invMass;
    float iA = bodySimA->invInertia;
    float mB = bodySimB->invMass;
    float iB = bodySimB->invInertia;

    // dummy state for static bodies
    SqBodySim dummyState;
    SqBodySim *stateA = bodyA->isAwake() ? bodySimA : &dummyState;
    SqBodySim *stateB = bodyB->isAwake() ? bodySimB : &dummyState;

    SqVec2 vA = stateA->linearVelocity;
    float wA = stateA->angularVelocity;
    SqVec2 vB = stateB->linearVelocity;
    float wB = stateB->angularVelocity;

    // printf("SqWheelJoint before slove bodyA %d vA:(%f %f) wA:%f bodyB %d vB(%f %f) wB:%f \n",
    //        bodySimA->bodyIndex, vA.x, vA.y, wA,
    //        bodySimB->bodyIndex, vB.x, vB.y, wB);

    bool fixedRotation = (iA + iB == 0.0f);

    /**
     * 下面的三条语句相当于计算BodyA和BodyB的锚点在世界坐标系下的位置，这里已经将这个过程拆分了
     * 不是直接使用“世界变换矩阵x锚点”那么简单，但其实底层就是一样的。
     *
     * 世界变换矩阵分为两部分
     * 1、Body上的Transform（外部设置或上一帧）
     * 2、当前帧的的改变（deltaRotation和deltaPosition）
     * 当运算到这里的时候，还没有将上面两个合并在一起。
     *
     * 1、this->frameA.p 和 this->frameB.p 已经在solvePrepare中使用Body上的Transform的旋转矩阵旋转过了
     *    所以：
     *      SqRot::transformVector(stateA->deltaRotation, this->frameA.p)
     *      SqRot::transformVector(stateB->deltaRotation, this->frameB.p);
     *    这里的语句是将deltaRotation继续叠加到上面，最终得出在锚点世界矩阵下的旋转位置
     *
     * 2、接下来就需要计算矩阵对锚点的位移
     *   看下面的解析
     */
    SqVec2 rA = SqRot::transformVector(stateA->deltaRotation, this->frameA.p);
    SqVec2 rB = SqRot::transformVector(stateB->deltaRotation, this->frameB.p);

    /**
     * 2、接下来就需要计算矩阵对锚点的位移
     *   world_position = centerPosition + body transform position + deleta position;
     *
     * 这条语句完成如下：
     * 1、计算锚点在世界坐标系的坐标
     * 2、计算BodyB和BodyA 锚点在世界坐标系的向量
     *
     * 其实下面的语句可以为：
     *  centerB + deltaPositionB + rB = anchorB的世界坐标
     *  centerA + deltaPositionA + RA = anchorA的世界坐标
     *
     *  其中CenterB和CenterA已经在solvePrepare中加上了Body上的Transform的位移了，所以这里只需要加上deltaPosition和旋转后的。
     *
     *  d =  anchorB的世界坐标 - anchorA的世界坐标
     *    =  (centerB + deltaPositionB + rB) - (centerA + deltaPositionA + RA)
     *    = (centerB - centerA) + (deltaPositionB - deltaPositionA)  + (rB - rA)
     *
     *   从而得到如下代码,所以d是两个Body的锚点的距离向量
     */
    SqVec2 d = SqVec2::Add(SqVec2::Add(SqVec2::Sub(stateB->deltaPosition, stateA->deltaPosition), this->deltaCenter), SqVec2::Sub(rB, rA));

    // 以BodyA的x正轴作为BodyB可以滑动的轴。计算旋转q后的轴
    SqVec2 axisA = SqRot::transformVector(this->frameA.q, SqVec2(1.0f, 0.0f)); // Body本身的Transform的旋转
    axisA = SqRot::transformVector(stateA->deltaRotation, axisA);              // 加上deltaRotation的旋转 = 总共需要的旋转

    // printf("useBias %d axisA %f %f stateBd %f %f stateAd %f %f \n", useBias, axisA.x, axisA.y, stateB->deltaPosition.x, stateB->deltaPosition.y, stateA->deltaPosition.x, stateA->deltaPosition.y);

    // 两个Body的锚点投影到轴上的距离是多少，得到两个Body在轴上相距的距离
    float translation = SqVec2::Dot(axisA, d);
    float a1 = SqVec2::Cross(SqVec2::Add(d, rA), axisA);
    float a2 = SqVec2::Cross(rB, axisA);

    // motor constraint
    if (this->enableMotor && fixedRotation == false)
    {
        float Cdot = wB - wA - this->motorSpeed;
        float impulse = -this->motorMass * Cdot;
        float oldImpulse = this->motorImpulse;
        float maxImpulse = context.h * this->maxMotorTorque;
        this->motorImpulse = clamp(this->motorImpulse + impulse, -maxImpulse, maxImpulse);
        // this->motorImpulse = this->motorImpulse + impulse;
        impulse = this->motorImpulse - oldImpulse;

        wA -= iA * impulse;
        wB += iB * impulse;

        // printf("enableMotor impulse %f %f motorMass %f wA %f wB %f \n", impulse, this->motorSpeed, this->motorMass, wA, wB);
    }

    // spring constraint
    if (this->enableSpring)
    {
        // printf("enableSpring\n");
        // This is a real spring and should be applied even during relax
        float C = translation;
        float bias = this->springSoftness.biasRate * C;
        float massScale = this->springSoftness.massScale;
        float impulseScale = this->springSoftness.impulseScale;

        float Cdot = SqVec2::Dot(axisA, SqVec2::Sub(vB, vA)) + a2 * wB - a1 * wA;
        float impulse = -massScale * this->axialMass * (Cdot + bias) - impulseScale * this->springImpulse;
        this->springImpulse += impulse;

        SqVec2 P = SqVec2::MulSV(impulse, axisA);
        float LA = impulse * a1;
        float LB = impulse * a2;

        vA = SqVec2::MulSub(vA, mA, P);
        wA -= iA * LA;
        vB = SqVec2::MulAdd(vB, mB, P);
        wB += iB * LB;
    }

    if (this->enableLimit)
    {
        // 如果开启了两个Body 锚点水平的距离的约束，即两个Body不能超出一定的水平距离
        // translation为当前两个Body的距离，如果translation的距离超出了限制的距离
        // 则这里我们需要将他们的距离拉回来，这里限制的是水平方向的距离

        // printf("translation %f %f %f \n", translation, lowerTranslation, upperTranslation);
        // Lower limit
        {
            float C = translation - this->lowerTranslation;
            float bias = 0.0f;
            float massScale = 1.0f;
            float impulseScale = 0.0f;

            if (C > 0.0f)
            {
                // speculation
                bias = C * context.inv_h;
            }
            else if (useBias)
            {
                bias = jointSim->constraintSoftness.biasRate * C;
                massScale = jointSim->constraintSoftness.massScale;
                impulseScale = jointSim->constraintSoftness.impulseScale;
            }

            float Cdot = SqVec2::Dot(axisA, SqVec2::Sub(vB, vA)) + a2 * wB - a1 * wA;
            float impulse = -massScale * this->axialMass * (Cdot + bias) - impulseScale * this->lowerImpulse;
            float oldImpulse = this->lowerImpulse;
            this->lowerImpulse = max(oldImpulse + impulse, 0.0f);
            impulse = this->lowerImpulse - oldImpulse;

            // printf("lower massScale %f impulseScale %f lowerImpulse %f C %f Cdot %f bias %f  impulse %f lowerTranslation %f \n",
            //        massScale, impulseScale, this->lowerImpulse, C, Cdot,
            //        bias, impulse, this->lowerTranslation);

            SqVec2 P = SqVec2::MulSV(impulse, axisA);
            float LA = impulse * a1;
            float LB = impulse * a2;

            vA = SqVec2::MulSub(vA, mA, P);
            wA -= iA * LA;
            vB = SqVec2::MulAdd(vB, mB, P);
            wB += iB * LB;
        }

        // Upper limit
        // Note: signs are flipped to keep C positive when the constraint is satisfied.
        // This also keeps the impulse positive when the limit is active.
        {
            // sign flipped
            float C = this->upperTranslation - translation;
            float bias = 0.0f;
            float massScale = 1.0f;
            float impulseScale = 0.0f;

            if (C > 0.0f)
            {
                // speculation
                bias = C * context.inv_h;
            }
            else if (useBias)
            {
                bias = jointSim->constraintSoftness.biasRate * C;
                massScale = jointSim->constraintSoftness.massScale;
                impulseScale = jointSim->constraintSoftness.impulseScale;
            }

            // sign flipped on Cdot
            float Cdot = SqVec2::Dot(axisA, SqVec2::Sub(vA, vB)) + a1 * wA - a2 * wB;
            float impulse = -massScale * this->axialMass * (Cdot + bias) - impulseScale * this->upperImpulse;
            float oldImpulse = this->upperImpulse;
            this->upperImpulse = max(oldImpulse + impulse, 0.0f);
            impulse = this->upperImpulse - oldImpulse;

            // printf("upper massScale %f impulseScale %f lowerImpulse %f C %f Cdot %f bias %f  impulse %f upperTranslation %f \n",
            //        massScale, impulseScale, this->lowerImpulse, C, Cdot,
            //        bias, impulse, this->upperTranslation);

            SqVec2 P = SqVec2::MulSV(impulse, axisA);
            float LA = impulse * a1;
            float LB = impulse * a2;

            // sign flipped on applied impulse
            vA = SqVec2::MulAdd(vA, mA, P);
            wA += iA * LA;
            vB = SqVec2::MulSub(vB, mB, P);
            wB -= iB * LB;
        }

        // printf("SqWheelJoint enableLimit bodyA %d vA:(%f %f) aA:%f bodyB %d vB(%f %f) aB:%f\n",
        //        bodySimA->bodyIndex, vA.x, vA.y, wA,
        //        bodySimB->bodyIndex, vB.x, vB.y, wB);
    }

    //  printf("slove 2 vB %f %f wB %f \n",vB.x,vB.y,wB);
    // 线性约束
    {
        SqVec2 perpA = SqVec2::LeftPerp(axisA);

        float bias = 0.0f;
        float massScale = 1.0f;
        float impulseScale = 0.0f;
        if (useBias)
        {
            // 计算rA到rB两个锚点在垂直线上的距离。将点拉在一起
            float C = SqVec2::Dot(perpA, d);
            bias = jointSim->constraintSoftness.biasRate * C;
            massScale = jointSim->constraintSoftness.massScale;
            impulseScale = jointSim->constraintSoftness.impulseScale;
            // printf(" constraintHertz %f  onstraintDampingRatio %f C %f biasRate %f bias %f \n",jointSim->constraintHertz,jointSim->constraintDampingRatio,C,jointSim->constraintSoftness.biasRate,bias);
        }

        // printf("d %f %f perpA %f %f rA %f %f rB %f %f wB %f wA %f vB %f %f vA %f %f \n",
        // 	d.x,d.y,
        // 	perpA.x,perpA.y,
        // 	rA.x,rA.y,
        // 	rB.x,rB.y,
        // 	wB,wA,
        //     vB.x,vB.y,
        //     vA.x,vA.y
        // );

        float s1 = SqVec2::Cross(SqVec2::Add(d, rA), perpA);
        float s2 = SqVec2::Cross(rB, perpA);
        float Cdot = SqVec2::Dot(perpA, SqVec2::Sub(vB, vA)) + s2 * wB - s1 * wA;

        float impulse = -massScale * this->perpMass * (Cdot + bias) - impulseScale * this->perpImpulse;

        // printf("stateB %p massScale %f impulseScale %f perpImpulse %f Cdot %f bias %f  impulse %f \n",stateB,
        // 	massScale,impulseScale,this->perpImpulse,bias,bias,impulse
        // );

        this->perpImpulse += impulse;

        SqVec2 P = SqVec2::MulSV(impulse, perpA);
        float LA = impulse * s1;
        float LB = impulse * s2;

        vA = SqVec2::MulSub(vA, mA, P);
        wA -= iA * LA;
        vB = SqVec2::MulAdd(vB, mB, P);
        wB += iB * LB;

        // printf("SqWheelJoint linear bodyA %d vA:(%f %f) aA:%f bodyB %d vB(%f %f) aB:%f\n",
        //        bodySimA->bodyIndex, vA.x, vA.y, wA,
        //        bodySimB->bodyIndex, vB.x, vB.y, wB);
    }

    stateA->linearVelocity = vA;
    stateA->angularVelocity = wA;
    stateB->linearVelocity = vB;
    stateB->angularVelocity = wB;

    // Node *nodeB = (Node *)bodyB->getUserData();
    // if (nodeB->nativeId == 16  && useBias)
    // {
    //     printf("wheel id %d vB %f %f wB %f \n",
    //            nodeB->nativeId,
    //            stateB->linearVelocity.x, stateB->linearVelocity.y,
    //            stateB->angularVelocity);
    // }

    // printf("wheel vA %f %f wA %f vB %f %f wB %f \n",
    //        stateA->linearVelocity.x, stateA->linearVelocity.y, wA,
    //        stateB->linearVelocity.x, stateB->linearVelocity.y,
    //        stateB->angularVelocity);
}
