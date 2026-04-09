#include "Character2DComponent.h"
#include "./Collision2DComponent.h"
#include "../PhysicsSystem.h"
#include "../../../engine/framework/Application.h"

#include <cmath>
#include <functional>
using namespace physics2d;

struct CastResult
{
    phxy::SqVec2 point;
    phxy::SqVec2 normal;
    int bodyId;
    float fraction;
    bool hit;
};

static float CastCallback(phxy::SqShape *shape, phxy::SqVec2 point, phxy::SqVec2 normal, float fraction, void *context)
{
    CastResult *result = (CastResult *)context;
    result->point = point;
    result->normal = normal;
    result->bodyId = shape->getBody();
    result->fraction = fraction;
    result->hit = true;
    return fraction;
}

bool physics2d::PlaneResultFcn(phxy::SqShape *shape, const phxy::SqPlaneResult *planeResult, void *context)
{
    Character2DComponent *self = static_cast<Character2DComponent *>(context);
    float maxPush = FLT_MAX;
    bool clipVelocity = true;
    // ShapeUserData *userData = static_cast<ShapeUserData *>((void *)b2Shape_GetUserData(shapeId));
    // if (userData != nullptr)
    // {
    //     maxPush = userData->maxPush;
    //     clipVelocity = userData->clipVelocity;
    // }

    if (self->m_planeCount < Character2DComponent::m_planeCapacity)
    {
        self->m_planes[self->m_planeCount] = {planeResult->plane, maxPush, 0.0f, clipVelocity};
        self->m_planeCount += 1;
    }

    return true;
}

Character2DComponent::Character2DComponent()
    : Component(),
      state(0),
      m_onGround(false)
{
    m_velocity = {0.0f, 0.0f};
    world = PhysicsSystem::getInstance()->getWorld()->getPyxhWorld();
    m_capsule.setCapsule(phxy::SqVec2(-1.0f, 0.0f), phxy::SqVec2(1.0, 0.0f), .5f);
}

void Character2DComponent::onEnable()
{
    Application::getInstance()->timer.frameLoop(1, this, &Character2DComponent::update);
}

void Character2DComponent::onDisable()
{
    Application::getInstance()->timer.off(this, &Character2DComponent::update);
}

void Character2DComponent::update(float dt)
{

    dt *= 0.001;

    float speed = phxy::SqVec2::Length(m_velocity);
    if (speed < m_minSpeed)
    {
        // 小于m_minSpeed时则清空上一帧速度，如果接下来state为0，则就会停止状态了
        m_velocity.x = 0.0f;
        m_velocity.y = 0.0f;
    }
    else if (m_onGround)
    {
        float control = speed < m_stopSpeed ? m_stopSpeed : speed;

        // 如果在地上，则应用摩擦力，也就是需要减去的速度
        // 这里隐含一个逻辑：当没有move和jump，则还会滑动一段距离，这个摩擦就是控制滑动多久就停止下来
        float drop = control * m_friction * dt;

        // 可以看到当前的速度小于摩擦速度时，则newSpeed为0，将速度都设置为0从而静止了
        float newSpeed = Math::max(0.0f, speed - drop);
        m_velocity = phxy::SqVec2::MulSV(newSpeed / speed, m_velocity);

        // 在地面的时候不用重力，否则在斜面会滑动下来
        m_velocity.y = 0.f;
    }

    // 移动。如果同时激活Left和Right，则throttle为0则不会移动
    float throttle = 0.f;
    if (state & toNumber(CharacterState::MOVE_LEFT))
    {
        throttle += -1;
    }

    if (state & toNumber(CharacterState::MOVE_RIGHT))
    {
        throttle += 1;
    }

    phxy::SqVec2 desiredVelocity = {m_speed * throttle, 0.0f};
    float desiredSpeed;
    phxy::SqVec2 desiredDirection = phxy::SqVec2::GetLengthAndNormalize(&desiredSpeed, desiredVelocity);
    if (desiredSpeed > m_speed)
    {
        desiredSpeed = m_speed;
    }

    printf("====== speed %f \n", desiredSpeed);

    // 如果当前的速度还没有达到设置的m_speed的速度,我们我们需要一点一点将速度加到m_speed的速度
    // 即模拟加速度，加到m_speed速度的时候就不再加
    float currentSpeed = phxy::SqVec2::Dot(m_velocity, desiredDirection); // 当的速度
    float addSpeed = desiredSpeed - currentSpeed;                         // 期望的m_speed速度与当前的速度还差多少

    // 大于0表示还没有达到m_speed。addSpeed表示还差多少速度才到达m_speed
    if (addSpeed > 0)
    {
        // 执行加速度
        float steer = m_onGround ? 1.0f : m_airSteer;
        float accelSpeed = steer * m_accelerate * m_speed * dt; // 计算一个加速度
        if (accelSpeed > addSpeed)
        {
            accelSpeed = addSpeed;
        }

        m_velocity = phxy::SqVec2::MulAdd(m_velocity, accelSpeed, desiredDirection);
    }

    if (!m_onGround)
    {
        m_velocity.y -= m_gravity * dt;
    }

    /*
    检测地面。垂直向下的方向。
    为了实现小于pogoHeight高度的地方能够自动走过去，在检测地面的时候使用一个没有体积的线段进行射线检测，
    而不是使用角色的碰撞体，然后将角色的碰撞体抬高pogoHeight的高度，否则角色的碰撞体肯定检测前面有东西造成走不过去。
    */

    phxy::SqVec2 origin = m_transform.transformPoint(m_capsule.center1);

    CastResult castResult = {};
    phxy::SqVec2 segmentOffset = {0.75f * m_capsule.radius, 0.0f};

    phxy::SqVec2 segmentPoints[2];
    segmentPoints[0] = phxy::SqVec2::Sub(origin, segmentOffset);
    segmentPoints[1] = phxy::SqVec2::Add(origin, segmentOffset);
    phxy::SqShapeProxy proxy = phxy::sqMakeProxy(segmentPoints, 2, 0.0f);
    float rayLength = pogoHeight + m_capsule.radius;
    phxy::SqVec2 translation = {0.0f, -rayLength};
    world->castShape(&proxy, translation, groundFilter, CastCallback, &castResult);

    if (m_onGround == false)
    {
        m_onGround = castResult.hit && m_velocity.y <= 0.01f;
    }
    else
    {
        m_onGround = castResult.hit;
    }

    // printf("character %f %d %d\n", m_velocity.y, m_onGround, castResult.hit);

    if (castResult.hit == false)
    {
        m_pogoVelocity = 0.0f;
    }
    else
    {

        // 将角色碰撞体调整到距离地面pogoHeight高度。
        // 这里使用弹簧调整高度，使得看上去有点缓冲舒服的感觉
        float pogoCurrentLength = castResult.fraction * rayLength;
        float offset = pogoCurrentLength - rayLength; // 还差多少才到pogoHeight高度

        // printf("hit offset %f %f %f\n",castResult.fraction,rayLength,pogoCurrentLength);

        // offse为绑定在弹簧那头的物体的位移
        // 如果offset小于0，表示挤压了，弹簧向外推
        // 如果offset大于0，表示拉伸了，弹簧向里面拉
        m_pogoVelocity = phxy::sqSpringDamper(m_pogoHertz, m_pogoDampingRatio, offset, m_pogoVelocity, dt);

        // 为碰撞的东西应用一个力，给碰撞的东西一点反应
        // b2Body_ApplyForce(castResult.bodyId, {0.0f, -50.0f}, castResult.point, true);
    }

#ifdef PHYSICS_DEBUG
    PhysicsDraw *debugDraw = PhysicsSystem::getInstance()->getWorld()->getDebugDraw();
    if (debugDraw)
    {
        phxy::SqVec2 delta = castResult.hit ? phxy::SqVec2::MulSV(castResult.fraction, translation) : translation;
        Color eColor;
        eColor.fromHEX(phxy::sq_colorBlue);
        debugDraw->DrawSegment(origin, phxy::SqVec2::Add(origin, delta), eColor);
    }
#endif

    // phxy::SqVec2 target = m_transform.p + dt * m_velocity + dt * m_pogoVelocity * phxy::SqVec2{0.0f, 1.0f};
    phxy::SqVec2 v = phxy::SqVec2::MulSV(dt, m_velocity);
    phxy::SqVec2 damper = phxy::SqVec2::MulSV(dt * m_pogoVelocity, phxy::SqVec2{0.0f, 1.0f});
    phxy::SqVec2 target = phxy::SqVec2::Add(m_transform.p, v);
    target = phxy::SqVec2::Add(target, damper);

    // 下面的逻辑检查角色除了垂直向下的方向的其他方向，避免穿墙

    m_transform.p = target;

    float tolerance = 0.01f;
    tolerance *= tolerance;

    // printf("====target %f %f m_velocity %f %f \n", target.x, target.y, m_velocity.x, m_velocity.y);

    // 迭代多次，不断调整位置，不让抖动和穿透
    // box2D为实现这个角色移动器内部针对性加了方法，如b2SolvePlanes、b2World_CollideMover、b2World_CastMover
    for (int iteration = 0; iteration < 5; ++iteration)
    {
        m_planeCount = 0;

        phxy::SqCapsuleShape mover;
        mover.center1 = m_transform.transformPoint(m_capsule.center1);
        mover.center2 = m_transform.transformPoint(m_capsule.center2);
        mover.radius = m_capsule.radius;

        world->collideMover(&mover, groundFilter, &PlaneResultFcn, this);

        phxy::SqPlaneSolverResult result = phxy::sqSolvePlanes(phxy::SqVec2::Sub(target, m_transform.p), m_planes, m_planeCount);

        float fraction = world->castMover(&mover, result.translation, groundFilter);

        // printf("fraction %f \n",fraction);

        phxy::SqVec2 delta = phxy::SqVec2::MulSV(fraction, result.translation);
        m_transform.p = phxy::SqVec2::Add(m_transform.p, delta);

        if (phxy::SqVec2::LengthSquared(delta) < tolerance)
        {
            break;
        }
    }

    m_velocity = phxy::sqClipVector(m_velocity, m_planes, m_planeCount);
    syncPhysicsToTransform();

#ifdef PHYSICS_DEBUG
    if (debugDraw)
    {
        phxy::SqVec2 p1 = m_transform.transformPoint(m_capsule.center1);
        phxy::SqVec2 p2 = m_transform.transformPoint(m_transform, m_capsule.center2);
        Color eColor;
        eColor.fromHEX(m_onGround ? phxy::sq_colorOrange : phxy::sq_colorAquamarine);
        debugDraw->DrawSolidCapsule(p1, p2, m_capsule.radius, eColor);
    }
#endif
}

void Character2DComponent::syncPhysicsToTransform()
{
    Transform2DComponent *transform = node->getComponent<Transform2DComponent>();
    Mat3 &current = transform->getWorldTransform();
    const phxy::SqVec2 &pos = m_transform.p;
    const phxy::SqRot &rot = m_transform.q;
    float rx, ry;
    current.getRotation(rx, ry);
    float s = sin(rx);
    if (Math::equlas(s, rot.s) && Math::equlas(current.data[4], pos.x) && Math::equlas(current.data[5], pos.y))
    {
        return;
    }

    Mat3 mat;
    mat.data[0] = rot.c;
    mat.data[1] = rot.s;
    mat.data[2] = -rot.s;
    mat.data[3] = rot.c;
    mat.data[4] = pos.x;
    mat.data[5] = pos.y;
    transform->setWorldTransform(mat);
}

void Character2DComponent::jump()
{
    if (!m_onGround)
    {
        return;
    }
    m_onGround = false;
    m_velocity.y = m_jumpSpeed;
}

void Character2DComponent::moveLeft(bool enable)
{
    int move = toNumber(CharacterState::MOVE_LEFT);
    if (enable)
    {
        state |= move;
    }
    else
    {
        state &= ~move;
    }
}

void Character2DComponent::moveRight(bool enable)
{
    int move = toNumber(CharacterState::MOVE_RIGHT);
    if (enable)
    {
        state |= move;
    }
    else
    {
        state &= ~move;
    }
}

void Character2DComponent::setFilter(unsigned int categoryBits, unsigned int maskBits)
{
    groundFilter.categoryBits = categoryBits;
    groundFilter.maskBits = maskBits;
}

Character2DComponent::~Character2DComponent()
{
    Application::getInstance()->timer.off(this, &Character2DComponent::update);
}
