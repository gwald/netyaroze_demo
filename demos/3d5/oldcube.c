
#include <libps.h>

#include <stdio.h>

#include "asssert.h"

#include "dump.h"

#include "geom.h"

#include "cube.h"




/****************************************************************************
					macros
****************************************************************************/



#define setVECTOR(vector, x, y, z)				\
				(vector)->vx = (x), (vector)->vy = (y), (vector)->vz = (z)	





/****************************************************************************
					functions
****************************************************************************/


	





void InitCube (Cube *cube)
{
	int i;

	cube->id = -1;
	cube->sideLength = -1;
	cube->sideFlag = -1;

	for (i = 0; i < 6; i++)
		{
		setRECT( &(cube->textureAreas[i]), 0, 0, 0, 0);
		cube->polygons[i] = NULL;
		setVECTOR( &(cube->normals[i]), 0, 0, 0);
		}

	GsInitCoordinate2( WORLD, &cube->coord);
}







void CreateCube (Cube *cube, int id, int sideLength, int sideFlag, 
									RECT *textureAreas)
{
	int i;

	assert(id >= 0);
	assert(sideLength > 0);

	InitCube(cube);

	cube->id = id;
	cube->sideLength = cube->sideLength;
	cube->sideFlag = sideFlag;

		// sort the texture areas
	for (i = 0; i < 6; i++)
		{
		setRECT( &(cube->textureAreas[i]), 
			textureAreas[i].x, textureAreas[i].y,
			textureAreas[i].w, textureAreas[i].h);
		}

		// we know the normals: cube constructed at world origin with world's orientation
	switch(sideFlag)
		{
		case VISIBLE_FROM_INSIDE:
			setVECTOR( &(cube->normals[PLUS_X_Y_PLANE]), 0, 0, -ONE);
			setVECTOR( &(cube->normals[MINUS_X_Y_PLANE]), 0, 0, ONE);
			setVECTOR( &(cube->normals[PLUS_X_Z_PLANE]), 0, -ONE, 0);
			setVECTOR( &(cube->normals[MINUS_X_Z_PLANE]), 0, ONE, 0);
			setVECTOR( &(cube->normals[PLUS_Y_Z_PLANE]), -ONE, 0, 0);
			setVECTOR( &(cube->normals[MINUS_Y_Z_PLANE]), ONE, 0, 0);
			break;
		case VISIBLE_FROM_OUTSIDE:
			setVECTOR( &(cube->normals[PLUS_X_Y_PLANE]), 0, 0, ONE);
			setVECTOR( &(cube->normals[MINUS_X_Y_PLANE]), 0, 0, -ONE);
			setVECTOR( &(cube->normals[PLUS_X_Z_PLANE]), 0, ONE, 0);
			setVECTOR( &(cube->normals[MINUS_X_Z_PLANE]), 0, -ONE, 0);
			setVECTOR( &(cube->normals[PLUS_Y_Z_PLANE]), ONE, 0, 0);
			setVECTOR( &(cube->normals[MINUS_Y_Z_PLANE]), -ONE, 0, 0);
			break;
		default:	
			assert(FALSE);
		}
} 




void DrawCube (Cube *cube, GsOT *ot)
{
	int i;
	MATRIX tmpls;
	MiniObject *polygon;

	for (i = 0; i < 6; i++)
		{
		polygon = cube->polygons[i];

		GsGetLs(&(polygon->coord), &tmpls);	   
								   
		GsSetLightMatrix(&tmpls);
									
		GsSetLsMatrix(&tmpls);
									
		GsSortObject4( &(polygon->handler), 
				ot, 3, getScratchAddr(0));
		}
}
