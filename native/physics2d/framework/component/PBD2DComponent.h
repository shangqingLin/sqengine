#pragma once
#include "../../../engine/framework/component/Component.h"
#include "../../phxy/sq-phxy.h"

namespace physics2d
{
    class PBD2DComponent : public Component
    {
    private:
        phxy::PBD *pbd;
        void onNodeTransformChange();

    protected:
        virtual void onAwake() override;

    public:
        PBD2DComponent();
        virtual ~PBD2DComponent();
        void create(float *points, float *mass, int, Vec2 *velocities = nullptr);
        void create(phxy::PBDDef &);
        void setLinearVelocity(int pointIndex, const Vec2 &);
        void move(int pointIndex, const Vec2 &offset);
        void reset();
        void setTuning(phxy::PBDTuning &);
        inline const Vec2 &getPointPosition(int pointIndex) { return (Vec2 &)pbd->getPointPosition(pointIndex); };
        inline int getPointCount() { return pbd->getPointCount(); };
        inline void setMass(int pointIndex, float mass) { pbd->setMass(pointIndex, mass); };
        inline float getMass(int pointIndex) const { return pbd->getMass(pointIndex); };
        inline const Vec2 &getVelocity(int pointIndex) { return (Vec2 &)pbd->getVelocity(pointIndex); };
        inline void setPosition(int pointIndex, const Vec2 &position) { pbd->position(pointIndex, phxy::SqVec2(position.x, position.y)); };
        inline void setPaused(bool paused) { pbd->setPaused(paused); };
        bool rayCast(phxy::SqPBDRayCastOut &output, const phxy::SqRayCastInput &input) const;
        inline void setUserData(int pointIndex, void *data) { pbd->setUserData(pointIndex, data); };
        inline void *getUserData(int pointIndex) { return pbd->getUserData(pointIndex); };
    };
}
