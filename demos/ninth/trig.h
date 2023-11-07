// total trigonometry package:

// the main six trig functions

// sin, cos, tan, arcsin, arccos, arctan

// are implemented with nice interface and SPEED from LUTs






#include <libps.h>

#include <stdio.h>

#include "asssert.h"



#define FIRST_QUADRANT 0
#define SECOND_QUADRANT 1
#define THIRD_QUADRANT 2
#define FOURTH_QUADRANT 3



	// value used to indicate bad value
	// eg when tangent(x) called with PI/2: return is +/- infinity
	// SILLY_VALUE used to flag this to application code
#define SILLY_VALUE 0x7ffffff0



	// NOTE: should REMOVE many of these prototypes
	// AND specify EXACT calling conventions


// rsin and rcos: both get called with standard PSX-format angle
// both deal with -ve, zero and +ve angles
// result is 4096 times too big; >> 12 at END of use
// e.g.

	//  x = (radius * rcos(theta)) >> 12;

// rtan: expects angle between -180 and 180, will clip;
// (this Not problematic);

// again, result is 4096 too big, >>12 at END of use

// Main point for TAN: if angle is 90 + N.180,
// the tangent is +- infinity:
// tangent will return SILLY_VALUE which must be tested for


// e.g.

	// int tangent = rtan(angle);
	// assert(tangent != SILLY_VALUE);
	// y = (x * tangent) >> 12;


int rsin (int angle);	   // result is scaled up by 4096

int rcos (int angle);	   // result is scaled up by 4096

int rtan (int angle);	   // result is scaled up by 4096
	// assert(value != SILLY_VALUE);



int rasin3 (int opposite, int hypoteneuse);
int rasin4 (int value);

int racos3 (int adjacent, int hypoteneuse);
int racos4 (int value);

int ratan3 (int opposite, int adjacent);



	


#if 0		// NO reason to extern these; hide instead
	// the LUTs
extern int SinCosTable[1025];
extern int TangentLUT[4097];
extern int ArcSinTable[4097];
extern int ArcCosTable[4097];
extern int ArcTangentTable[256];
#endif