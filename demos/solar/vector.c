/************************************************************
 *															*
 *						vector.c							*
 *				miscellaneous vector routines				*
 *															*
 *	Copyright (C) 1996 Sony Computer Entertainment Inc.		*
 *	All Rights Reserved										*
 *															*
 ***********************************************************/





#include <libps.h>

#include "asssert.h"

#include "vector.h"



	// assigns random values to vector argument
	// such that it ends up of overall size ONE
void RandomVector (VECTOR* vector)
{
	int soFar, theRest;			  // magnitude so far, what is left
	
		// set randomly
	vector->vx = rand() % ONE;
	vector->vy = rand() % ONE;
	vector->vz = rand() % ONE;
	
	ScaleVectorToUnit(vector);
}



void RandomVectorOfGivenSize (VECTOR* vector, u_long size)
{
	int soFar, theRest;			  // magnitude so far, what is left
	
		// set randomly
	vector->vx = rand() % ONE;
	vector->vy = rand() % ONE;
	vector->vz = rand() % ONE;
	
	ScaleVectorToSize(vector, size);
}





void ScaleVectorToUnit (VECTOR* vector)
{
	int length, square;

	square = pow(vector->vx, 2) + pow(vector->vy, 2) 
					+ pow(vector->vz, 2); 

	length = pow(square, 0.5);
	assert(length != 0);	   // never allow division by zero

	vector->vx *= ONE;
	vector->vy *= ONE;
	vector->vz *= ONE;

	vector->vx /= length;
	vector->vy /= length;
	vector->vz /= length;
}




void ScaleVectorToSize (VECTOR* vector, u_long size)
{
	int length, square;

	square = pow(vector->vx, 2) + pow(vector->vy, 2) 
					+ pow(vector->vz, 2); 

	length = pow(square, 0.5);
	assert(length != 0);	   // never allow division by zero

	vector->vx *= size;
	vector->vy *= size;
	vector->vz *= size;

	vector->vx /= length;
	vector->vy /= length;
	vector->vz /= length;
}




	// assign to <output> any vector of size ONE
	// that is perpendicular to vector <input>
void AnyUnitPerpendicularVector (VECTOR* input, VECTOR* output)
{	 
		// the switching on rand value probably unnecessary ...
	switch (rand() % 3)
		{
		case 0:
		if (input->vz != 0)
			{
		
				// set x and y randomly
			output->vx = rand() % ONE;
			output->vy = rand() % ONE;
	
			output->vz = ((-(input->vx) * output->vx) 
							+ (-(input->vy) * output->vy)) 
											/ input->vz;

			ScaleVectorToUnit(output);
			}
		else		 // never allow division by zero
			{
			output->vx = ONE;
			output->vy = 0;
			output->vz = 0;
			}
		break;
		case 1:
		if (input->vy != 0)
			{
		
				// set x and z randomly
			output->vx = rand() % ONE;
			output->vz = rand() % ONE;
	
			output->vy = ((-(input->vx) * output->vx) 
							+ (-(input->vz) * output->vz)) 
											/ input->vy;

			ScaleVectorToUnit(output);
			}
		else		// never allow division by zero
			{
			output->vx = 0;
			output->vy = ONE;
			output->vz = 0;
			}
		break;
		case 2:
		if (input->vx != 0)
			{
		
				// set z and y randomly
			output->vz = rand() % ONE;
			output->vy = rand() % ONE;
	
			output->vx = ((-(input->vz) * output->vz) 
							+ (-(input->vy) * output->vy)) 
											/ input->vx;

			ScaleVectorToUnit(output);
			}
		else		  // never allow division by zero
			{
			output->vx = 0;
			output->vy = 0;
			output->vz = ONE;
			}
		break;
		default:
			printf("SHOULD NOT GET HERE\n");
			exit(1);
		}
}




	// output vector is perpendicular to first and second
void FindNormalVector (VECTOR* first, VECTOR* second, VECTOR* output)
{
	// NOT YET
}
