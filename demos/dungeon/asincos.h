#include <libps.h>

#include "asssert.h"



	// arcsin: first angle either 0-90 or 270-360
	// second is 90-270
void rasin (int value, int* firstAngle, int* secondAngle);

	// arccos: first angle 0-180, second 180-360
void racos (int value, int* firstAngle, int* secondAngle);	
	



extern int ArcSinTable[];

extern int ArcCosTable[];