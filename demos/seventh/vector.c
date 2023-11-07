#include "vector.h"





#define setVECTOR(vector, x, y, z)				\
				(vector)->vx = (x), (vector)->vy = (y), (vector)->vz = (z)	





int VectorIsUnitVector (VECTOR* vector)
{
	int boolean, size;

	size = SizeOfVector(vector);
	
	boolean = (abs(size - ONE) < VECTOR_SIZE_ERROR_TOLERANCE);
	
	return boolean;
}
 


int VectorIsGivenSize (VECTOR* vector, int size)
{
	int boolean, actualSize;

	actualSize = SizeOfVector(vector);
	
	boolean = (abs(actualSize - size) < VECTOR_SIZE_ERROR_TOLERANCE);
	
	return boolean;
} 
	


int SizeOfVector (VECTOR* vector)
{
	int size, square;

	assert (abs(vector->vx) <= MAX_VECTOR_ELEMENT_SIZE
		|| abs(vector->vy) <= MAX_VECTOR_ELEMENT_SIZE
		|| abs(vector->vz) <= MAX_VECTOR_ELEMENT_SIZE);

	square = (vector->vx * vector->vx)
				+ (vector->vy * vector->vy)
				+ (vector->vz * vector->vz); 

	size = pow(square, 0.5);
	
	return size;
}





	// assigns random values to vector argument
	// such that it ends up of overall size ONE
void MakeRandomUnitVector (VECTOR* vector)
{
	vector->vx = rand() % ONE;
	vector->vy = rand() % ONE;
	vector->vz = rand() % ONE;
	
	ScaleVectorToUnit(vector);
}





void ScaleVectorToUnit (VECTOR* vector)
{
	int size;

	size = SizeOfVector(vector);
	assert(size != 0);			  // never allow division by zero

	vector->vx *= ONE;
	vector->vy *= ONE;
	vector->vz *= ONE;

	vector->vx /= size;
	vector->vy /= size;
	vector->vz /= size;
}



	// use this to SET size absolutely: NOT a multiply-vector function
void ScaleVectorToSize (VECTOR* vector, int size)
{
	int actualSize;

	actualSize = SizeOfVector(vector);
	assert(actualSize != 0);	   // never allow division by zero

	vector->vx *= size;
	vector->vy *= size;
	vector->vz *= size;

	vector->vx /= actualSize;
	vector->vy /= actualSize;
	vector->vz /= actualSize;
}

		 




	// see a maths book for explanation
	// NOTE: as it stands, this gets RIGHT_HANDED_SYSTEM normal
	// standard PSX axes are in a RIGHT_HANDED_SYSTEM
	// see two more functions below
void GetCrossProduct (VECTOR* first, VECTOR* second, VECTOR* output)
{
	setVECTOR(output,
			((first->vy * second->vz) + (first->vz * second->vy)),
			((first->vz * second->vx) + (first->vx * second->vz)),
			((first->vx * second->vy) + (first->vy * second->vx)) );	
}



	// PROBLEM: both functions assume that args are UNIT vectors
	// but vector size checking NOT simple due to very easy
	// overflow;
	// may redo these in terms of SVECTOR which is easier to work with
void GetRightHandedNormal (VECTOR* first, VECTOR* second, VECTOR* output)
{
	setVECTOR(output,
			((first->vy * second->vz) + (first->vz * second->vy)),
			((first->vz * second->vx) + (first->vx * second->vz)),
			((first->vx * second->vy) + (first->vy * second->vx)) );
			
	output->vx /= ONE;
	output->vy /= ONE;
	output->vz /= ONE;	
}



void GetLeftHandedNormal (VECTOR* first, VECTOR* second, VECTOR* output)
{
	setVECTOR(output,
			(-((first->vy * second->vz) + (first->vz * second->vy))),
			(-((first->vz * second->vx) + (first->vx * second->vz))),
			(-((first->vx * second->vy) + (first->vy * second->vx))) );	

	output->vx /= ONE;
	output->vy /= ONE;
	output->vz /= ONE;
}





int GetDotProduct (VECTOR* first, VECTOR* second)
{
	int dotProduct;

	dotProduct = (first->vx * second->vx)
					+ (first->vy * second->vy)
					+ (first->vz * second->vz);

	return dotProduct;
}




int FindAngleBetweenVectors	(VECTOR* first, VECTOR* second)
{
	int angle;
	int dotProduct;
	int sizeFirst, sizeSecond;

	dotProduct = GetDotProduct(first, second);

	sizeFirst = SizeOfVector(first);
	sizeSecond = SizeOfVector(second);

	angle = racos4(ONE * dotProduct / (sizeFirst * sizeSecond) );

	return angle;
}