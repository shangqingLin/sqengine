#include "SqContactSim.h"
#include <string.h>
using namespace phxy;

SqContactSim::SqContactSim()
{
    reset();
}

void SqContactSim::reset()
{
    memset(this, 0, sizeof(SqContactSim));
}