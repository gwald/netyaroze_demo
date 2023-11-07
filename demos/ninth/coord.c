#include "coord.h"




#define setVECTOR(vector, x, y, z)				\
				(vector)->vx = (x), (vector)->vy = (y), (vector)->vz = (z)	






	
int CheckCoordinateSystemIsBase (GsCOORDINATE2* coord)
{
	if (coord->super != WORLD)
		return FALSE;

	if (MatrixIsUnitMatrix( &coord->coord) == FALSE)
		return FALSE;

	return TRUE;
}
	




void CopyCoordinateSystem (GsCOORDINATE2* from, GsCOORDINATE2* to)
{
	assert(from != to);

	to->super = from->super;

	CopyMatrix( &from->coord, &to->coord);

	to->flg = 0;
}





#if 0		   // suspect

	// NOTE: VERY BADLY NAMED FUNCTION

void ConvertPointObjectToWorld (VECTOR* point, GsCOORDINATE2* coord, VECTOR* output)
{
	assert(coord->super == WORLD);

	ApplyMatrixLV(&coord->coord, point, output);	
}





	// NOTE: VERY BADLY NAMED FUNCTION

void ConvertPointWorldToObject (VECTOR* point, GsCOORDINATE2* coord, VECTOR* output)
{
	MATRIX transpose;

	assert(coord->super == WORLD);

#if 0
	TransposeMatrix( &coord->coord, &transpose);	// axes flipped in matrix

	ApplyMatrixLV( &transpose, point, output);
#endif

	output->vx = (point->vx * coord->coord.m[0][0]
	   				+ point->vy * coord->coord.m[0][1]
	   				+ point->vz * coord->coord.m[0][2]) >> 12;
	output->vy = (point->vx * coord->coord.m[1][0]
	   				+ point->vy * coord->coord.m[1][1]
	   				+ point->vz * coord->coord.m[1][2]) >> 12;
	output->vz = (point->vx * coord->coord.m[2][0]
	   				+ point->vy * coord->coord.m[2][1]
	   				+ point->vz * coord->coord.m[2][2]) >> 12;	
}

#endif




	// eg know position or vector in world space,
	// want to find it in object coordinate terms

	// NOTE: Only takes account of rotation
	// NOT displacement as well

void ExpressSuperPointInSub (VECTOR* subPoint, 
			GsCOORDINATE2* subSystem, VECTOR* superPointOutput)
{
	MATRIX* matrix;

	matrix = &subSystem->coord;

	setVECTOR(superPointOutput, 
				((subPoint->vx * matrix->m[0][0]
				+ subPoint->vy * matrix->m[0][1]
				+ subPoint->vz * matrix->m[0][2]) >> 12),

				((subPoint->vx * matrix->m[1][0]
				+ subPoint->vy * matrix->m[1][1]
				+ subPoint->vz * matrix->m[1][2]) >> 12),

				((subPoint->vx * matrix->m[2][0]
				+ subPoint->vy * matrix->m[2][1]
				+ subPoint->vz * matrix->m[2][2]) >> 12) );
}



	
	
	// eg know position or vector in object space,
	// want to find it in world coordinate terms
	
	// NOTE: Only takes account of rotation
	// NOT displacement as well			   

void ExpressSubPointInSuper (VECTOR* superPoint, 
			GsCOORDINATE2* subSystem, VECTOR* subPointOutput)
{
	MATRIX* matrix;

	matrix = &subSystem->coord;

	setVECTOR(subPointOutput, 
				((superPoint->vx * matrix->m[0][0]
				+ superPoint->vy * matrix->m[1][0]
				+ superPoint->vz * matrix->m[2][0]) >> 12),

				((superPoint->vx * matrix->m[0][1]
				+ superPoint->vy * matrix->m[1][1]
				+ superPoint->vz * matrix->m[2][1]) >> 12),

				((superPoint->vx * matrix->m[0][2]
				+ superPoint->vy * matrix->m[1][2]
				+ superPoint->vz * matrix->m[2][2]) >> 12) );
}






#if 0		// suspect
	// output is {screen.vx, screen.vy, screen.vz}; 
void ConvertPointWorldToScreen (VECTOR* position, VECTOR* output)
{
	VECTOR screen;

	PushMatrix();			   // UNSURE ABOUT THIS

		// convert 
	ApplyMatrixLV( &GsWSMATRIX, position, &screen);

	screen.vx += GsWSMATRIX.t[0];
	screen.vy += GsWSMATRIX.t[1];
	screen.vz += GsWSMATRIX.t[2];

	// Note:
	// object (eg if sprite) at normal (non-scaled) size
	// when screen.vz == ProjectionDistance;
	// ie sprite 'visualWidth' = normalWidth * ProjectionDistance / screen.vz; 

	PopMatrix();				 // UNSURE ABOUT THIS

	setVECTOR(output, screen.vx, screen.vy, screen.vz);
}
#endif




	// if <system> coord attached to <start> coord,
	// translate and rotate into a new <end> coord 

#if 0			// suspect
void ConvertCoordSystemObjectToObject (GsCOORDINATE2* system,		
					GsCOORDINATE2* start, GsCOORDINATE2* end)
{
	VECTOR position, newPosition;
	VECTOR xAxis, yAxis, zAxis;
	VECTOR newXaxis, newYaxis, newZaxis;
	MATRIX temp;

	assert(FALSE);				// UNFINISHED

		// check all three different
	assert(system != start);
	assert(system != end);
	assert(start != end);

		// check system attached to start
	assert(system->super == start);

		// check other two directly attached to world
	assert(start->super == WORLD);
	assert(end->super == WORLD);

		// system position: get it into super i.e. <start> coords
	setVECTOR( &position, system->coord.t[0], system->coord.t[1], system->coord.t[2]);
	ApplyMatrixLV( &system->coord, &position, &newPosition);
	setVECTOR( &position, newPosition.vx, newPosition.vy, newPosition.vz);	// in <start>

		// system position: convert to world coords
	ApplyMatrixLV( &start->coord, &position, &newPosition);
	setVECTOR( &position, newPosition.vx, newPosition.vy, newPosition.vz);	  // in WORLD

		// the object-relative axes of <system>: convert to super i.e. start coords
	setVECTOR( &xAxis, system->coord.m[0][0], system->coord.m[0][1], system->coord.m[0][2]);
	setVECTOR( &yAxis, system->coord.m[1][0], system->coord.m[1][1], system->coord.m[1][2]);
	setVECTOR( &zAxis, system->coord.m[2][0], system->coord.m[2][1], system->coord.m[2][2]);
	ApplyMatrixLV( &system->coord, &xAxis, &newXaxis);	   // new axes in <start> system
	ApplyMatrixLV( &system->coord, &yAxis, &newYaxis);
	ApplyMatrixLV( &system->coord, &zAxis, &newZaxis);

		// the object-relative axes of <system>: convert to WORLD coords
	ApplyMatrixLV( &system->coord, &newXaxis, &xAxis);		  // now axes in WORLD
	ApplyMatrixLV( &system->coord, &newYaxis, &yAxis);
	ApplyMatrixLV( &system->coord, &newZaxis, &zAxis);

 		// build matrix of <system> in world coords
	temp.m[0][0] = xAxis.vx;
	temp.m[0][1] = xAxis.vy;
	temp.m[0][2] = xAxis.vz;
	temp.m[1][0] = yAxis.vx;
	temp.m[1][1] = yAxis.vy;
	temp.m[1][2] = yAxis.vz;
	temp.m[2][0] = zAxis.vx;
	temp.m[2][1] = zAxis.vy;
	temp.m[2][2] = zAxis.vz;
	temp.t[0] = position.vx;
	temp.t[1] = position.vy;
	temp.t[2] = position.vz;

		// convert position to <end> coords
	ConvertPointWorldToObject (&position, end, &newPosition);

		// convert object-relative axes to <end> coords
	ConvertPointWorldToObject (&xAxis, end, &newXaxis);
	ConvertPointWorldToObject (&yAxis, end, &newYaxis);
	ConvertPointWorldToObject (&zAxis, end, &newZaxis);

		// finally, attach to new super coordinate system
	system->super = end;
	system->flg = 0;
}
#endif
	




	// rotating around x and y only: theta-Z always zero
	// rotation order: always y then x

void DeriveNewCoordSystemFromRotation (GsCOORDINATE2* original,
			SVECTOR* rotation, GsCOORDINATE2* output)
{
	SVECTOR realRotation;
	MATRIX rotationMatrix;
	SVECTOR xVector, yVector;

		// z component of rotation has no place in cylindrical tunnel
	setVECTOR( &realRotation, rotation->vx, rotation->vy, 0);

	if (realRotation.vx == 0)		 // just a theta-Y rotation
		{
		RotMatrix(&realRotation, &rotationMatrix);
		MulMatrix0(&original->coord, &rotationMatrix, &output->coord);
		}
	else
		{
		if (realRotation.vy == 0)		 // just a theta-X rotation
			{
			RotMatrix(&realRotation, &rotationMatrix);
			MulMatrix0(&original->coord, &rotationMatrix, &output->coord);
			}
		else	 // compound rotation: first by Y, then by X
			{
			setVECTOR( &yVector, 0, realRotation.vy, 0);	   // just y rotation
			RotMatrix(&yVector, &rotationMatrix);
			MulMatrix0(&original->coord, &rotationMatrix, &output->coord);

			setVECTOR( &xVector, realRotation.vx, 0, 0);	  // just x rotation
			RotMatrix(&xVector, &rotationMatrix);
			MulMatrix0(&output->coord, &rotationMatrix, &output->coord);
			}
		}

		// tell GTE that coordinate system has been updated
	output->flg = 0;
}



	
#if 0		// suspect
	
void ConvertMatrixFromSuperToSub (MATRIX* matrix, 
					GsCOORDINATE2* subSystem)
{ 
	VECTOR position, newPosition;
	VECTOR xAxis, yAxis, zAxis, newX, newY, newZ;

	setVECTOR( &position, matrix->t[0],
			matrix->t[1], matrix->t[2]);

	setVECTOR( &xAxis, matrix->m[0][0],
			matrix->m[0][1], matrix->m[0][2]);
	setVECTOR( &yAxis, matrix->m[1][0],
			matrix->m[1][1], matrix->m[1][2]);
	setVECTOR( &zAxis, matrix->m[2][0],
			matrix->m[2][1], matrix->m[2][2]);

	dumpVECTOR(&position);
	dumpVECTOR( &xAxis);
	dumpVECTOR( &yAxis);
	dumpVECTOR( &zAxis);

	ExpressSuperPointInSub (&position, subSystem, &newPosition);

	ExpressSuperPointInSub (&xAxis, subSystem, &newX);
	ExpressSuperPointInSub (&yAxis, subSystem, &newY);
	ExpressSuperPointInSub (&zAxis, subSystem, &newZ);

	dumpVECTOR(&newPosition);
	dumpVECTOR( &newX);
	dumpVECTOR( &newY);
	dumpVECTOR( &newZ);

	matrix->t[0] = newPosition.vx;
	matrix->t[1] = newPosition.vy;
	matrix->t[2] = newPosition.vz;

	matrix->m[0][0] = newX.vx;
	matrix->m[0][1] = newX.vy;
	matrix->m[0][2] = newX.vz;

	matrix->m[1][0] = newY.vx;
	matrix->m[1][1] = newY.vy;
	matrix->m[1][2] = newY.vz;

	matrix->m[2][0] = newZ.vx;
	matrix->m[2][1] = newZ.vy;
	matrix->m[2][2] = newZ.vz;
}
#endif
  

