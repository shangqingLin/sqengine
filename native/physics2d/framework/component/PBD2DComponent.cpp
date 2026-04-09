#include "PBD2DComponent.h"
#include "../PhysicsSystem.h"
#include "../../../engine/2d/components/Transform2DComponent.h"

using namespace physics2d;

PBD2DComponent::PBD2DComponent() : Component(),
                                   pbd(nullptr)
{
}

void PBD2DComponent::onAwake()
{
    pbd = PhysicsSystem::getInstance()->getWorld()->createPBD();
    node->on(NodeEventType::TRASNFORM_CHANGE, std::bind(&PBD2DComponent::onNodeTransformChange, this));
    onNodeTransformChange();
}

void PBD2DComponent::onNodeTransformChange()
{
    float units = 1.0f / PhysicsSystem::getInstance()->getUnitsPerMeter();
    const Mat3 &worldMat = node->getComponent<Transform2DComponent>()->getWorldTransform();

    float rx, ry;
    worldMat.getRotation(rx, ry);
    rx = cos(rx);
    ry = sin(ry);

    phxy::SqTransform transform;
    worldMat.getTranslation(transform.p.x, transform.p.y);
    transform.p.x *= units;
    transform.p.y *= units;

    transform.q.c = rx;
    transform.q.s = ry;
    pbd->setTransform(transform);
}

void PBD2DComponent::create(float *points, float *mass, int num, Vec2 *velocities)
{
    phxy::PBDDef def;
    def.vertices = (phxy::SqVec2 *)points;
    def.count = num;
    def.masses = mass;
    def.velocities = (phxy::SqVec2 *)velocities;
    def.gravity.x = 0.f;
    def.gravity.y = -9.8f;
    pbd->Create(def);
}

void PBD2DComponent::create(phxy::PBDDef &define)
{
    pbd->Create(define);
}

void PBD2DComponent::setTuning(phxy::PBDTuning &tuning)
{
    pbd->SetTuning(tuning);
}

void PBD2DComponent::setLinearVelocity(int pointIndex, const Vec2 &velocity)
{
    pbd->setLinearVelocity(pointIndex, phxy::SqVec2(velocity.x, velocity.y));
}

void PBD2DComponent::move(int pointIndex, const Vec2 &offset)
{
    pbd->move(pointIndex, phxy::SqVec2(offset.x, offset.y));
}

void PBD2DComponent::reset()
{
    pbd->Reset();
}

bool PBD2DComponent::rayCast(phxy::SqPBDRayCastOut &output, const phxy::SqRayCastInput &input) const
{
    output.hit = false;
    pbd->rayCast(output, input);
    return output.hit;
}

PBD2DComponent::~PBD2DComponent()
{
    if (pbd)
    {
        node->off(NodeEventType::TRASNFORM_CHANGE, std::bind(&PBD2DComponent::onNodeTransformChange, this));
        PhysicsSystem::getInstance()->getWorld()->destroyPBD(pbd);
        pbd = nullptr;
    }
}
