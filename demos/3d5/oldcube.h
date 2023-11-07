#ifndef CUBE_H_INCLUDED




#include "object.h"	



 
/****************************************************************************
					structures, constants
****************************************************************************/





	// textured inside or out
#define VISIBLE_FROM_INSIDE 1
#define VISIBLE_FROM_OUTSIDE	2


typedef struct
{
	int id;
	int sideLength;
	int sideFlag;

	RECT textureAreas[6];		// 16 bit textures
	MiniObject *polygons[6];
	VECTOR normals[6];

	GsCOORDINATE2 coord;
} Cube;




 
/****************************************************************************
					prototypes
****************************************************************************/



void InitCube (Cube *cube);


void CreateCube (Cube *cube, int id, int sideLength, 
						int sideFlag, RECT *textureAreas);

void DrawCube (Cube *cube, GsOT *ot);





#define CUBE_H_INCLUDED 1

#endif
