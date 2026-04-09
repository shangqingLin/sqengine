#include "SqConstraintGraph.h"
#include "./sim/SqBodySim.h"
#include "./sim/SqContactSim.h"
#include "./sim/SqJointSim.h"
#include "../collision/narrowphase/SqContact.h"

using namespace phxy;

#define SQ_OVERFLOW_INDEX (SQ_GRAPH_COLOR_COUNT - 1)


SqConstraintGraph::SqConstraintGraph(SqWorld *world) : world(world)
{
}

/*
void SqConstraintGraph::addContactToGraph(SqContactSim *contactSim, SqContact *contact)
{
    SQ_ASSERT( contactSim->manifold.pointCount > 0 );
	SQ_ASSERT( contactSim->simFlags & SqContactSimFlags::sq_simTouchingFlag );
	SQ_ASSERT( contact->flags & SqContactSimFlags::sq_contactTouchingFlag );

	int colorIndex = SQ_OVERFLOW_INDEX;

	int bodyIdA = contact->edges[0].bodyId;
	int bodyIdB = contact->edges[1].bodyId;
	b2Body* bodyA = b2BodyArray_Get( &world->bodies, bodyIdA );
	b2Body* bodyB = b2BodyArray_Get( &world->bodies, bodyIdB );
	bool staticA = bodyA->setIndex == b2_staticSet;
	bool staticB = bodyB->setIndex == b2_staticSet;
	SQ_ASSERT( staticA == false || staticB == false );

#if B2_FORCE_OVERFLOW == 0
	if ( staticA == false && staticB == false )
	{
		for ( int i = 0; i < SQ_OVERFLOW_INDEX; ++i )
		{
			b2GraphColor* color = graph->colors + i;
			if ( b2GetBit( &color->bodySet, bodyIdA ) || b2GetBit( &color->bodySet, bodyIdB ) )
			{
				continue;
			}

			b2SetBitGrow( &color->bodySet, bodyIdA );
			b2SetBitGrow( &color->bodySet, bodyIdB );
			colorIndex = i;
			break;
		}
	}
	else if ( staticA == false )
	{
		// No static contacts in color 0
		for ( int i = 1; i < SQ_OVERFLOW_INDEX; ++i )
		{
			b2GraphColor* color = graph->colors + i;
			if ( b2GetBit( &color->bodySet, bodyIdA ) )
			{
				continue;
			}

			b2SetBitGrow( &color->bodySet, bodyIdA );
			colorIndex = i;
			break;
		}
	}
	else if ( staticB == false )
	{
		// No static contacts in color 0
		for ( int i = 1; i < SQ_OVERFLOW_INDEX; ++i )
		{
			b2GraphColor* color = graph->colors + i;
			if ( b2GetBit( &color->bodySet, bodyIdB ) )
			{
				continue;
			}

			b2SetBitGrow( &color->bodySet, bodyIdB );
			colorIndex = i;
			break;
		}
	}
#endif

	b2GraphColor* color = graph->colors + colorIndex;
	contact->colorIndex = colorIndex;
	contact->localIndex = color->contactSims.count;

	b2ContactSim* newContact = b2ContactSimArray_Add( &color->contactSims );
	memcpy( newContact, contactSim, sizeof( b2ContactSim ) );

	// todo perhaps skip this if the contact is already awake

	if ( staticA )
	{
		newContact->bodySimIndexA = B2_NULL_INDEX;
		newContact->invMassA = 0.0f;
		newContact->invIA = 0.0f;
	}
	else
	{
		SQ_ASSERT( bodyA->setIndex == b2_awakeSet );
		b2SolverSet* awakeSet = b2SolverSetArray_Get( &world->solverSets, b2_awakeSet );

		int localIndex = bodyA->localIndex;
		newContact->bodySimIndexA = localIndex;

		b2BodySim* bodySimA = b2BodySimArray_Get( &awakeSet->bodySims, localIndex );
		newContact->invMassA = bodySimA->invMass;
		newContact->invIA = bodySimA->invInertia;
	}

	if ( staticB )
	{
		newContact->bodySimIndexB = B2_NULL_INDEX;
		newContact->invMassB = 0.0f;
		newContact->invIB = 0.0f;
	}
	else
	{
		SQ_ASSERT( bodyB->setIndex == b2_awakeSet );
		b2SolverSet* awakeSet = b2SolverSetArray_Get( &world->solverSets, b2_awakeSet );

		int localIndex = bodyB->localIndex;
		newContact->bodySimIndexB = localIndex;

		b2BodySim* bodySimB = b2BodySimArray_Get( &awakeSet->bodySims, localIndex );
		newContact->invMassB = bodySimB->invMass;
		newContact->invIB = bodySimB->invInertia;
	}
}

void SqConstraintGraph::removeContactFromGraph(int bodyIdA, int bodyIdB, int colorIndex, int localIndex)
{

	SQ_ASSERT( 0 <= colorIndex && colorIndex < B2_GRAPH_COLOR_COUNT );
	SqGraphColor* color = colors + colorIndex;

	if ( colorIndex != SQ_OVERFLOW_INDEX )
	{
		// might clear a bit for a static body, but this has no effect
		b2ClearBit( &color->bodySet, bodyIdA );
		b2ClearBit( &color->bodySet, bodyIdB );
	}

	int movedIndex = b2ContactSimArray_RemoveSwap( &color->contactSims, localIndex );
	if ( movedIndex != SQ_NULL_INDEX )
	{
		// Fix index on swapped contact
		b2ContactSim* movedContactSim = color->contactSims.data + localIndex;

		// Fix moved contact
		int movedId = movedContactSim->contactId;
		b2Contact* movedContact = b2ContactArray_Get( &world->contacts, movedId );
		SQ_ASSERT( movedContact->setIndex == b2_awakeSet );
		SQ_ASSERT( movedContact->colorIndex == colorIndex );
		SQ_ASSERT( movedContact->localIndex == movedIndex );
		movedContact->localIndex = localIndex;
	}
}

SqJointSim *SqConstraintGraph::createJointInGraph(SqJoint *joint)
{

    int bodyIdA = joint->edges[0].bodyId;
    int bodyIdB = joint->edges[1].bodyId;
    b2Body *bodyA = b2BodyArray_Get(&world->bodies, bodyIdA);
    b2Body *bodyB = b2BodyArray_Get(&world->bodies, bodyIdB);
    bool staticA = bodyA->setIndex == b2_staticSet;
    bool staticB = bodyB->setIndex == b2_staticSet;

    int colorIndex = b2AssignJointColor(graph, bodyIdA, bodyIdB, staticA, staticB);

    SqJointSim *jointSim = b2JointSimArray_Add(&graph->colors[colorIndex].jointSims);
    memset(jointSim, 0, sizeof(SqJointSim));

    joint->colorIndex = colorIndex;
    joint->localIndex = graph->colors[colorIndex].jointSims.count - 1;
    return jointSim;
}

void SqConstraintGraph::addJointToGraph(SqJointSim *jointSim, SqJoint *joint)
{
    SqJointSim* jointDst = createJointInGraph(joint );
	memcpy( jointDst, jointSim, sizeof( SqJointSim ) );
}

void SqConstraintGraph::removeJointFromGraph(int bodyIdA, int bodyIdB, int colorIndex, int localIndex)
{

	SQ_ASSERT( 0 <= colorIndex && colorIndex < SQ_GRAPH_COLOR_COUNT );
	SqGraphColor* color = colors + colorIndex;

	if ( colorIndex != SQ_OVERFLOW_INDEX )
	{
		// May clear static bodies, no effect
		b2ClearBit( &color->bodySet, bodyIdA );
		b2ClearBit( &color->bodySet, bodyIdB );
	}

	int movedIndex = b2JointSimArray_RemoveSwap( &color->jointSims, localIndex );
	if ( movedIndex != B2_NULL_INDEX )
	{
		// Fix moved joint
		SqJointSim* movedJointSim = color->jointSims.data + localIndex;
		int movedId = movedJointSim->jointId;
		b2Joint* movedJoint = b2JointArray_Get( &world->joints, movedId );
		SQ_ASSERT( movedJoint->setIndex == b2_awakeSet );
		SQ_ASSERT( movedJoint->colorIndex == colorIndex );
		SQ_ASSERT( movedJoint->localIndex == movedIndex );
		movedJoint->localIndex = localIndex;
	}
	*/