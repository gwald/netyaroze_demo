#include "vector.h"

#include "trig.h"




#if 0
#define setVECTOR(vector, x, y, z)				\
				(vector)->vx = (x), (vector)->vy = (y), (vector)->vz = (z)	
#endif




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
		&& abs(vector->vy) <= MAX_VECTOR_ELEMENT_SIZE
		&& abs(vector->vz) <= MAX_VECTOR_ELEMENT_SIZE);

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



	// scaling arg: unit is ONE (4096)
	// takes no account of previous vector size
void ScaleVector (VECTOR* vector, int relativeScale)
{
	vector->vx *= relativeScale;
	vector->vy *= relativeScale;
	vector->vz *= relativeScale;

	vector->vx /= ONE;
	vector->vy /= ONE;
	vector->vz /= ONE;
}




void MakeScaledVector (VECTOR* input, VECTOR* output, int relativeScale)
{
	output->vx = input->vx * relativeScale;
	output->vy = input->vy * relativeScale;
	output->vz = input->vz * relativeScale;

	output->vx /= ONE;
	output->vy /= ONE;
	output->vz /= ONE;
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





#if 0			   // not ready
int AngleBetweenVectors	(VECTOR* first, VECTOR* second)
{
	int dotProduct;

	dotProduct = (first->vx * second->vx)
					+ (first->vy * second->vy)
					+ (first->vz * second->vz);

#if 0
	cosAngle 
		= dotProduct / (SizeOfVector(first) * SizeOfVector(second));
#endif
}
#endif






	// chop vector into two pieces;
	// firstPortion expresses the ratio of fristpart to original,
	// scaled up by 4096
void SplitVectorInTwo (VECTOR* original, int firstPortion, 
					VECTOR* firstPart, VECTOR* secondPart)
{
	int secondPortion;

	assert(firstPortion >= 0 && firstPortion <= ONE);
	secondPortion = ONE - firstPortion;

	setVECTOR(firstPart,
			original->vx,
			original->vy,
			original->vz);
	ScaleVector(firstPart, firstPortion);

	setVECTOR(secondPart,
			original->vx,
			original->vy,
			original->vz);
	ScaleVector(secondPart, secondPortion);
}





void ResolveVector (VECTOR* original, VECTOR* resolver,
						VECTOR* parallelPart, VECTOR* orthogonalPart)
{
	int sizeOfResolverSquared;
	int dotProduct;
	int smallest, scaleFactor;

	//printf("\n ResolveVector \n");
	//printf("original, resolver\n");
	//dumpVECTOR(original);
   	//dumpVECTOR(resolver);

	sizeOfResolverSquared = (resolver->vx * resolver->vx)
							+ (resolver->vy * resolver->vy)
							+ (resolver->vz * resolver->vz);
	assert(sizeOfResolverSquared > 0);
	//printf("sizeOfResolverSquared: %d\n", sizeOfResolverSquared); 
		
   	dotProduct = (original->vx * resolver->vx)
				+ (original->vy * resolver->vy)
				+ (original->vz * resolver->vz); 
	//printf("dotProduct: %d\n", dotProduct);

		// scaling to prevent integer overflow
	if (abs(dotProduct) > abs(sizeOfResolverSquared))
		smallest = abs(sizeOfResolverSquared);
	else
		smallest = abs(dotProduct);

		 if (smallest > 1000000)
		scaleFactor = 200000;
	else if (smallest > 100000)
		scaleFactor = 20000;
	else if (smallest > 10000)
		scaleFactor = 2000;
	else if (smallest > 1000)
		scaleFactor = 200;
	else if (smallest > 100)
		scaleFactor = 20;
	else if (smallest > 10)
		scaleFactor = 2;
	else
		scaleFactor = 1;
		

	dotProduct /= scaleFactor;
	sizeOfResolverSquared /= scaleFactor;

	//printf("AFTER SCALE: sizeOfResolverSquared: %d\n", sizeOfResolverSquared);  
	//printf("dotProduct: %d\n", dotProduct);

	setVECTOR(parallelPart,
			((resolver->vx * dotProduct) / sizeOfResolverSquared),
			((resolver->vy * dotProduct) / sizeOfResolverSquared),
			((resolver->vz * dotProduct) / sizeOfResolverSquared) );

	setVECTOR(orthogonalPart, 
				original->vx - parallelPart->vx,
				original->vy - parallelPart->vy,
				original->vz - parallelPart->vz);
	//printf("parallelPart, orthogonalPart\n");
	//dumpVECTOR(parallelPart);
	//dumpVECTOR(orthogonalPart);
}