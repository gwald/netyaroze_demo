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




	// coord is standard coord for objects 
	// ie describes how coord is related to its super, the world

void ConvertPointObjectToWorld (VECTOR* point, GsCOORDINATE2* coord, VECTOR* output)
{
	assert(coord->super == WORLD);

	ApplyMatrixLV(&coord->coord, point, output);	
}






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




	// if <system> coord attached to <start> coord,
	// translate and rotate into a new <end> coord 

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
	




	  