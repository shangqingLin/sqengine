#pragma once
#include "../../common/math/SqTransform.h"
#include "../../common/math/SqVec2.h"
#include "../../common/collection/SqArray.h"
#include "../../common/collection/SqIdPool.h"
#include "../../common/collection/SqHashSet.h"

#include "SqContact.h"
namespace phxy
{
    class SqShape;
    class SqWorld;
    class SqNarrowPhase;
    class SqContactSim;
    class SqManifold;
    class SqBody;

    class SqContactManager
    {
    private:
        SqWorld *world;
        SqNarrowPhase *narrowPhase;
        SqArray<SqContact> contacts;
        SqIdPool contactIdPool;
        /**
         * 以ShapeA和ShapeB作为组合键标记这两个Shape是否有Contact了没
         */
        SqHashSet pairSet;

        bool updateContact(SqContactSim *contactSim, SqShape *shapeA, const SqTransform &transformA, SqVec2 centerOffsetA,
                           SqShape *shapeB, const SqTransform &transformB, SqVec2 centerOffsetB);

    public:
        friend class SqNarrowPhase;
        friend class SqWorldDraw;
        
        SqContactManager(SqWorld *, SqNarrowPhase *);
        bool hasContact(SqShape *, SqShape *);
        void createContact(SqShape *, SqShape *);
        void removeContact(SqContact *contact, bool wakeBodies);
        void removeContactById(int contactId, bool wakeBodies);
        void removeContact(int bodyIndex, bool wakeBodies);
        void removeContact(int bodyAIndex,int bodyBIndex, bool wakeBodies);
        inline SqContact *getContact(int contactId) { return contacts.get(contactId); };
        SqManifold computeManifold(SqShape *shapeA, SqTransform &transformA, SqShape *shapeB, SqTransform &transformB);
    };

}