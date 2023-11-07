#include <libps.h>

#include <stdio.h>

#include "asssert.h"




   // arcsin: first angle either 0-90 or 270-360
	// second is 90-270
void rasin (int value, int* smallerAngle, int* largerAngle);
int rasin2 (int value);



	// arccos: first angle 0-180, second 180-360
void racos (int value, int* smallerAngle, int* largerAngle);
int racos2 (int value);
	

	



extern int ArcSinTable[];

extern int ArcCosTable[];