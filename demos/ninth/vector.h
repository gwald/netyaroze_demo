#include <libps.h>

#include "stdio.h"

#include "stdlib.h"

#include "asssert.h"







#define VECTOR_SIZE_ERROR_TOLERANCE 50


#define MAX_VECTOR_ELEMENT_SIZE 32766




	// prototypes

int VectorIsUnitVector (VECTOR* vector);

int VectorIsGivenSize (VECTOR* vector, int size);

int SizeOfVector (VECTOR* vector);

void MakeRandomUnitVector (VECTOR* vector);

void ScaleVectorToUnit (VECTOR* vector);

	// use this to SET size absolutely: NOT a multiply-vector function
void ScaleVectorToSize (VECTOR* vector, int size);


	// see a maths book for explanation
	// NOTE: as it stands, this gets LEFT_HANDED_SYSTEM normal
	// see two more functions below
void GetCrossProduct (VECTOR* first, VECTOR* second, VECTOR* output);

void GetLeftHandedNormal (VECTOR* first, VECTOR* second, VECTOR* output);

void GetRightHandedNormal (VECTOR* first, VECTOR* second, VECTOR* output);




int GetDotProduct (VECTOR* first, VECTOR* second);

#if 0
int FindAngleBetweenVectors	(VECTOR* first, VECTOR* second);
#endif
