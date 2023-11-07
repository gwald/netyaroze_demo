#include "rand.h"





void InitialiseRandomNumbers (void)
{
	char time[64];
	int arbitraryValue;

		// attempt to make rand() more random:
		// seed using the seconds 
		// (NOTE: compile-time only: NOT the real time...)
	strcpy(time, __TIME__);
	arbitraryValue = ((int) time[6]) + ((int) time[7]);
	srand(arbitraryValue);
}