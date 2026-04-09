
#include "constants-define.h"


float sq_lengthUnitsPerMeter = 1.0f;

void sqSetLengthUnitsPerMeter( float lengthUnits )
{
	sq_lengthUnitsPerMeter = lengthUnits;
}

float sqGetLengthUnitsPerMeter( void )
{
	return sq_lengthUnitsPerMeter;
}