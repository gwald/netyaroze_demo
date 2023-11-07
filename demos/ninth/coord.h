#include <libps.h>

#include <stdio.h>

#include "asssert.h"

#include "matrix.h"

#include "dump.h"

#include "object.h"




int CheckCoordinateSystemIsBase (GsCOORDINATE2* coord);

void CopyCoordinateSystem (GsCOORDINATE2* from, GsCOORDINATE2* to);


#if 0		// need redoing
void ConvertPointObjectToWorld (VECTOR* point, 
							GsCOORDINATE2* coord, VECTOR* output);

void ConvertPointWorldToObject (VECTOR* point, 
							GsCOORDINATE2* coord, VECTOR* output);
#endif


void ExpressSubPointInSuper (VECTOR* subPoint, 
			GsCOORDINATE2* subSystem, VECTOR* superPointOutput);

void ExpressSuperPointInSub (VECTOR* superPoint, 
			GsCOORDINATE2* subSystem, VECTOR* subPointOutput);



#if 0		// suspect
void ConvertPointWorldToScreen (VECTOR* position, VECTOR* output);



void ConvertCoordSystemObjectToObject (GsCOORDINATE2* system,		
					GsCOORDINATE2* start, GsCOORDINATE2* end);
#endif


void DeriveNewCoordSystemFromRotation (GsCOORDINATE2* original,
			SVECTOR* rotation, GsCOORDINATE2* output);


#if 0		// suspect
void ConvertMatrixFromSuperToSub (MATRIX* matrix, 
					GsCOORDINATE2* subSystem);
#endif
