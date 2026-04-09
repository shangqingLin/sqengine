#pragma once

#include "../../engine/framework/Application.h"
#include "../../bindings/nativetojs/NativeToJsObject.h"
#include "../phxy/sq-phxy.h"
#include "PhysicsDraw.h"
#include "../phxy/particle/SqParticleSystem.h"

namespace physics2d
{
        class PhysicsWorld
        {
        private:
                phxy::SqWorld *world;
                phxy::SqParticleSystemWorld *particleWorld;
                bool lockSyncPhysicsToScene = false;
                std::vector<phxy::PBD *> pbds;
// debug相关
#ifdef PHYSICS_DEBUG
                PhysicsDraw *debugDraw = nullptr;
                Node *debugDrawNode = nullptr;
#endif

                bridge::NativeToJsObject nativeToJsObject;

                void removeDebugDraw();
                void syncNodeToPhysics(Node *, phxy::SqBody *);

        public:
                static const int bridgeOp_onCollisionBeginContact = 1;
                static const int bridgeOp_onCollisionEndContact = 2;
                static const int bridgeOp_onCollisionSensorBegin = 3;
                static const int bridgeOp_onCollisionSensorEnd = 4;
                static const int bridgeOp_onParticleBeginContantBody = 5;
                static const int bridgeOp_onParticleEndContantBody = 6;
                static const int bridgeOp_onParticleBeginContantParticle = 7;
                static const int bridgeOp_onParticleEndContantParticle = 8;
                static const int bridgeOp_onParticleBeginContantBody_to_body = 9;
                static const int bridgeOp_onParticleEndContantBody_to_body = 10;
                PhysicsWorld();
                ~PhysicsWorld();
                void syncSceneToPhysics();
                void syncPhysicsToScene();
                inline bool isLockSyncPhysicsToScene() { return lockSyncPhysicsToScene; };
                inline const phxy::SqVec2 &getGravity() const { return world->getGravity(); };
                inline phxy::SqWorld *getPyxhWorld() { return world; };
                int createBody(phxy::SqBodyDef &def);
                phxy::SqBody *getBody(int bodyId);
                void removeBody(int bodyId);
                phxy::SqShape *createShape(int bodyId, const phxy::SqShapeDef &def);
                void setDebugDraw(bool enable, Node *);
                void drawDebug();
#ifdef PHYSICS_DEBUG
                inline PhysicsDraw *getDebugDraw() { return debugDraw; };
#endif
                void beforeStep();
                void step(float dt);
                phxy::SqParticleSystem *CreateParticleSystem(phxy::b2ParticleSystemDef &);
                void destroyParticleSystem(phxy::SqParticleSystem *);
                inline bridge::NativeToJsObject &getNativeToJsObject() { return nativeToJsObject; };

                phxy::PBD *createPBD();
                void destroyPBD(phxy::PBD *);
        };
}
