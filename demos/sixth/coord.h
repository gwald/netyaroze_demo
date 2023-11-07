#include <libps.h>

#include <stdio.h>

#include "asssert.h"

#include "matrix.h"




int CheckCoordinateSystemIsBase (GsCOORDINATE2* coord);

void CopyCoordinateSystem (GsCOORDINATE2* from, GsCOORDINATE2* to);

void ConvertPointObjectToWorld (VECTOR* point, 
							GsCOORDINATE2* coord, VECTOR* output);

void ConvertPointWorldToObject (VECTOR* point, 
							GsCOORDINATE2* coord, VECTOR* output);

void ConvertPointWorldToScreen (VECTOR* position, VECTOR* output);

void ConvertCoordSystemObjectToObject (GsCOORDINATE2* system,		
					GsCOORDINATE2* start, GsCOORDINATE2* end);