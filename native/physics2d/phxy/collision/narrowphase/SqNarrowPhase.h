#pragma once
#include "../../dynamics/SqStepContext.h"
#include "SqContactManager.h"
#include "../../common/collection/SqArray.h"
#include "../../common/collection/SqIdPool.h"
namespace phxy
{
    class SqWorld;
    class SqBody;

    class SqNarrowPhase
    {
    private:
        SqWorld *world;
        SqContactManager contactManager;

        void collideTask(int endIndex, SqStepContext *stepContext);
        void afterCollideProcessContact(SqStepContext &context);

    public:
        friend class SqContactManager;
        friend class SqWorldDraw;
        
        SqNarrowPhase(SqWorld *);
        void createContact(SqShape *, SqShape *);
        bool hasContact(SqShape *, SqShape *);
        inline SqContact *getContact(int contactId) { return contactManager.getContact(contactId); };
        inline void removeContactById(int contactId, bool wakeBody) { contactManager.removeContactById(contactId, wakeBody); };
        inline void removeContact(int bodyIndex, bool wakeBodies) { contactManager.removeContact(bodyIndex, wakeBodies); };
        inline void removeContact(int bodyAIndex, int bodyBIndex, bool wakeBodies) { contactManager.removeContact(bodyAIndex, bodyBIndex, wakeBodies); }
        void collide(SqStepContext &context);
    };

}