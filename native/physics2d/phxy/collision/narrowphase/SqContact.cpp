#include "SqContact.h"

using namespace phxy;

SqContactEdge::SqContactEdge()
{
    reset();
}


void SqContactEdge::reset()
{
    bodyIndex = SQ_NULL_INDEX;
    prevKey = SQ_NULL_INDEX;
    nextKey = SQ_NULL_INDEX;
}

SqContact::SqContact()
{
    reset();
}

void SqContact::reset()
{

    setIndex = SQ_NULL_INDEX;
    colorIndex = SQ_NULL_INDEX;
    simIndex = SQ_NULL_INDEX;

    edges[0].reset();
    edges[1].reset();

    shapeA = nullptr;
    shapeB = nullptr;

    islandPrev = SQ_NULL_INDEX;
    islandNext = SQ_NULL_INDEX;
    islandId = SQ_NULL_INDEX;
    isMarkedForIslandSplit = false;

    contactId = SQ_NULL_INDEX;
    flags = 0;
    isMarked = false;
}