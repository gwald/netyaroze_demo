/*
 * LIBOBJC.C
 *
 * Copyright (C) 1993-1997 by Sony Computer Entertainment of America, Inc.
 * All rights Reserved
 * ----------------------------------------------------------------------------
 */

/*
 * A generic object handler data structure has been defined as part of
 * Object.h and all objects in the world are handled using this data
 * structure.  Most of the routines contained in this file work on this
 * data structure.  This file contains routines to
 *
 * - initialize a given object
 * - to rotate and set the coordinates of an object
 * - draw an object
 * - handle position fifo to find status of an object
 * - compute collision of objects with other objects and walls in the world.
 * - check if automatic poly division to be done for a given object
 * ----------------------------------------------------------------------------
 */

#include "libmain.h"
#include "libobjc.h"

/*
 * Initializes stuff in object structure for a given object
 * ----------------------------------------------------------------------------
 */

void InitObjc(OBJECT *objc, GsCOORDINATE2 *super, short x, short y, short z)
{
	/* Initialize the position FIFO */
	objc->PosFIFOTop = -1;
	
	/* Link TMD data to a OBJect structure */
	GsLinkObject4((unsigned long)(objc->Model->ModelPtr+3), &objc->Object, 0);
	
	/* Initialize object handling structure */
	GsInitCoordinate2(super, &objc->Pos);
	
	/* Set the object position */
	objc->Pos.coord.t[0] = (long)x;
	objc->Pos.coord.t[1] = (long)y;
	objc->Pos.coord.t[2] = (long)z;
	
	/* Initialize the 3D object */
	objc->Object.attribute = (GsFOG);
	objc->Object.coord2 = &objc->Pos;
	
	/* Update object coordinates */
	HandleSetCoordinate(objc);
	
	/* Calculate the minimum/maximum x/y/z values of the binding box
	 * depending upon the inclination of the object wrt the z,y,x axes.
	 */
	objc->MinX = objc->Pos.coord.t[0] - objc->DifX;
	objc->MaxX = objc->Pos.coord.t[0] + objc->DifX;
	objc->MinY = objc->Pos.coord.t[1] - objc->DifY;
	objc->MaxY = objc->Pos.coord.t[1] + objc->DifY;
	objc->MinZ = objc->Pos.coord.t[2] - objc->DifZ;
	objc->MaxZ = objc->Pos.coord.t[2] + objc->DifZ;
	
	/* Initialize the speed of the robot */
	objc->SpeedForward = 0;
	objc->SpeedX = 0;
	objc->SpeedY = 0;
	objc->SpeedZ = 0;
	
	/* Initialize variables used to rotate the object */
	objc->SpeedAngleY = 0;
	objc->SpeedAngleYPlaySfx = 0;
	
	/* Initialize timing parameters */
	objc->ChangeStateCounter       = 0;
	objc->ChangeStateThreshold     = 0;
	objc->ChangeParameterCounter   = 0;
	objc->ChangeParameterThreshold = 0;
	objc->AuxilliaryVarOne         = 0;
	objc->AuxilliaryVarTwo         = 0;
	return;
}

/*
 * Set the coordinate of the given object
 * ----------------------------------------------------------------------------
 */

void HandleSetCoordinate(OBJECT *objc)
{
	MATRIX tmp;
	
	/* Calculate Matrix from object parameters */
	tmp = GsIDMATRIX;
	tmp.t[0] = objc->Pos.coord.t[0];
	tmp.t[1] = objc->Pos.coord.t[1];
	tmp.t[2] = objc->Pos.coord.t[2];
	
	/* Rotate, set the matrix and clear the flag for change in parameter */
	RotMatrix(&objc->Angle, &tmp);
	objc->Pos.coord = tmp;
	objc->Pos.flg = 0;
	return;
}

/*
 * Add an object to the world - Ordering Table.
 * ----------------------------------------------------------------------------
 */

void DrawObject(OBJECT *objc, GsOT *ot)
{
	MATRIX tmp;
	
	/* Quit if the object is not alive - i.e. it does not exist */
	if (!(objc->State & STATE_ALIVE))
		return;
	
	/* Enter the object in the Ordering Table */
	GsGetLw(objc->Object.coord2, &tmp);
	GsSetLightMatrix(&tmp);
	GsGetLs(objc->Object.coord2, &tmp);
	GsSetLsMatrix(&tmp);
	GsSortObject4(&objc->Object, ot, 14-OT_LENGTH, getScratchAddr(0));
	return;
}

/*
 * Saves the new position of the object in the position FIFO and
 * determines the objects state depending upon its status in last
 * given number of frames frames.
 * ----------------------------------------------------------------------------
 */

short HandlePosFIFO(OBJECT *objc)
{
	SVECTOR *pos;
	short i, top;
	short state = (objc->State & (~STATE_XYZ_MASK));
	
	/* Quit if the object is not alive - i.e. it does not exist */
	if (!(objc->State & STATE_ALIVE))
		return(state);
	
	/* Initialize all auto variables to calculate the position FIFO */
	pos = objc->PosFIFO;
	top = objc->PosFIFOTop + 1;
	
	/* Check if the FIFO is full.  If it is already full, loose the
	 * position of the object for the oldest frame.
	 */
	if (top >= POSFIFO_LEN) {
		for (i = 1 ; i < top; i++) {
			pos[i-1].vx = pos[i].vx;
			pos[i-1].vy = pos[i].vy;
			pos[i-1].vz = pos[i].vz;
		}
		top --; 
	}
	
	/* Save the current position of the object at the top
	 * of the FIFO queue
	 */
	pos[top].vx = objc->Pos.coord.t[0];
	pos[top].vy = objc->Pos.coord.t[1];
	pos[top].vz = objc->Pos.coord.t[2];
	objc->PosFIFOTop = top;
	
	/* Calculate the object's state from the FIFO.  It computes
	 * whether the object was changing position along any of the
	 * x,y,z axes.
	 */
	for (i = 0 ; i < top ; i++) {
		if (pos[i].vx != pos[i+1].vx) state |= STATE_X_CHANGING;
		if (pos[i].vy != pos[i+1].vy) state |= STATE_Y_CHANGING;
		if (pos[i].vz != pos[i+1].vz) state |= STATE_Z_CHANGING;
	}
	return(state);
}

/*
 * Check the collision of the given object with another object, the following
 * functions computes the collision by comparing the maximum extents of the
 * object in it's new position with the given object's extent and returns
 * a flag showing the collision in x/y/z directions.
 * ----------------------------------------------------------------------------
 */

short HandleObjcCollision(OBJECT *objc, OBJECT *bloc, short *dx, short *dy, short *dz)
{
	short minx, miny, minz;
	short maxx, maxy, maxz;
	short curx, cury, curz;
	short tmpx, tmpy, tmpz;
	short where = COLLISION_CLEAR;
	
	/* return if the blocking object is same as the given object */
	if (objc == bloc)
		return(where);
	
	/* Quit if the object is not alive - i.e. it does not exist */
	if (!(objc->State & STATE_ALIVE))
		return(where);
	
	/* Quit if the blocking object is not alive - i.e. it does not exist */
	if (!(bloc->State & STATE_ALIVE))
		return(where);
	
	/* Initialize all auto vraibles to be used to compute collision */
	minx = bloc->MinX - objc->DifX, maxx = bloc->MaxX + objc->DifX;
	miny = bloc->MinY - objc->DifY, maxy = bloc->MaxY + objc->DifY;
	minz = bloc->MinZ - objc->DifZ, maxz = bloc->MaxZ + objc->DifZ;

	/* Check for collison if the object is on the
	 * minimum x side of the blocking object
	 */
	tmpx = minx - objc->Pos.coord.t[0];
	if ((tmpx >= 0) && (tmpx < (*dx))) {
		where |= NEAR_MINX;
		tmpy = ((*dy) * tmpx) / (*dx);
		tmpz = ((*dz) * tmpx) / (*dx);
		cury = objc->Pos.coord.t[1] + tmpy;
		curz = objc->Pos.coord.t[2] + tmpz;
		if (((cury > miny) && (cury < maxy)) && ((curz > minz) && (curz < maxz))) {
			where |= COLLISION_MINX;
			(*dy) = tmpy;
			(*dz) = tmpz;
			(*dx) = tmpx;
		}
	}
	
	/* Check for collison if the object is on the
	 * maximum x side of the blocking object
	 */
	tmpx = maxx - objc->Pos.coord.t[0];
	if ((tmpx <= 0) && (tmpx > (*dx))) {
		where |= NEAR_MAXX;
		tmpy = ((*dy) * tmpx) / (*dx);
		tmpz = ((*dz) * tmpx) / (*dx);
		cury = objc->Pos.coord.t[1] + tmpy;
		curz = objc->Pos.coord.t[2] + tmpz;
		if (((cury > miny) && (cury < maxy)) && ((curz > minz) && (curz < maxz))) {
			where |= COLLISION_MAXX;
			(*dy) = tmpy;
			(*dz) = tmpz;
			(*dx) = tmpx;
		}
	}
	
	/* Check for collison if the object is on the
	 * minimum y side of the blocking object
	 */
	tmpy = miny - objc->Pos.coord.t[1];
	if ((tmpy >= 0) && (tmpy < (*dy))) {
		where |= NEAR_MINY;
		tmpz = ((*dz) * tmpy) / (*dy);
		tmpx = ((*dx) * tmpy) / (*dy);
		curz = objc->Pos.coord.t[2] + tmpz;
		curx = objc->Pos.coord.t[0] + tmpx;
		if (((curz > minz) && (curz < maxz)) && ((curx > minx) && (curx < maxx))) {
			where |= COLLISION_MINY;
			(*dz) = tmpz;
			(*dx) = tmpx;
			(*dy) = tmpy;
		}
	}
	
	/* Check for collison if the object is on the
	 * maximum y side of the blocking object
	 */
	tmpy = maxy - objc->Pos.coord.t[1];
	if ((tmpy <= 0) && (tmpy > (*dy))) {
		where |=  NEAR_MAXY;
		tmpz = ((*dz) * tmpy) / (*dy);
		tmpx = ((*dx) * tmpy) / (*dy);
		curz = objc->Pos.coord.t[2] + tmpz;
		curx = objc->Pos.coord.t[0] + tmpx;
		if (((curz > minz) && (curz < maxz)) && ((curx > minx) && (curx < maxx))) {
			where |= COLLISION_MAXY;
			(*dz) = tmpz;
			(*dx) = tmpx;
			(*dy) = tmpy;
		}
	}
	
	/* Check for collison if the object is on the
	 * minimum z side of the blocking object
	 */
	tmpz = minz - objc->Pos.coord.t[2];
	if ((tmpz >= 0) && (tmpz < (*dz))) {
		where |= NEAR_MINZ;
		tmpx = ((*dx) * tmpz) / (*dz);
		tmpy = ((*dy) * tmpz) / (*dz);
		curx = objc->Pos.coord.t[0] + tmpx;
		cury = objc->Pos.coord.t[1] + tmpy;
		if (((curx > minx) && (curx < maxx)) && ((cury > miny) && (cury < maxy))) {
			where |= COLLISION_MINZ;
			(*dx) = tmpx;
			(*dy) = tmpy;
			(*dz) = tmpz;
		}
	}
	
	/* Check for collison if the object is on the
	 * maximum z side of the blocking object
	 */
	tmpz = maxz - objc->Pos.coord.t[2];
	if ((tmpz <= 0) && (tmpz > (*dz))) {
		where |=  NEAR_MAXZ;
		tmpx = ((*dx) * tmpz) / (*dz);
		tmpy = ((*dy) * tmpz) / (*dz);
		curx = objc->Pos.coord.t[0] + tmpx;
		cury = objc->Pos.coord.t[1] + tmpy;
		if (((curx > minx) && (curx < maxx)) && ((cury > miny) && (cury < maxy))) {
			where |= COLLISION_MAXZ;
			(*dx) = tmpx;
			(*dy) = tmpy;
			(*dz) = tmpz;
		}
	}
	
	/* return the collision map for the object */
	return(where);
}

/*
 * Check the collision of the object with the walls of the cube.
 * Check the displacement of the object wrt the current distance
 * from the edges of the cube.  If the objects seems to collide
 * with any of the cube edges, calculate the new displacement
 * along x, y and z axes.  This function returns a flag showing
 * which side of the cube the object collided.
 * ----------------------------------------------------------------------------
 */

short HandleWallCollision(OBJECT *objc, OBJECT *wall, short *dx, short *dy, short *dz)
{
	short minx, miny, minz;
	short maxx, maxy, maxz;
	short curx, cury, curz;
	short tmpx, tmpy, tmpz;
	short where = COLLISION_CLEAR;
	
	/* Quit if the object is not alive - i.e. it does not exist */
	if (!(objc->State & STATE_ALIVE))
		return(where);
	
	/* Initialize all auto vraibles to be used to compute collision */
	minx = wall->MinX, maxx = wall->MaxX;
	miny = wall->MinY, maxy = wall->MaxY;
	minz = wall->MinZ, maxz = wall->MaxZ;
	
	/* Check if the collision is with the
	 * minimum x side of the cube
	 */
	tmpx = minx - objc->MinX;
	if (((*dx) != 0) && ((*dx) < tmpx)) {
		where |= NEAR_MINX;
		tmpy = ((*dy) * tmpx) / (*dx);
		tmpz = ((*dz) * tmpx) / (*dx);
		cury = objc->Pos.coord.t[1] + tmpy;
		curz = objc->Pos.coord.t[2] + tmpz;
		if (((cury >= miny) && (cury <= maxy)) && ((curz >= minz) && (curz <= maxz))) {
			where |= COLLISION_MINX;
			(*dy) = tmpy;
			(*dz) = tmpz;
			(*dx) = tmpx;
		}
	}
	
	/* Check if the collision is with the
	 * maximum x side of the cube
	 */
	tmpx = maxx - objc->MaxX;
	if (((*dx) != 0) && ((*dx) > tmpx)) {
		where |= NEAR_MAXX;
		tmpy = ((*dy) * tmpx) / (*dx);
		tmpz = ((*dz) * tmpx) / (*dx);
		cury = objc->Pos.coord.t[1] + tmpy;
		curz = objc->Pos.coord.t[2] + tmpz;
		if (((cury >= miny) && (cury <= maxy)) && ((curz >= minz) && (curz <= maxz))) {
			where |= COLLISION_MAXX;
			(*dy) = tmpy;
			(*dz) = tmpz;
			(*dx) = tmpx;
		}
	}
	
	/* Check if the collision is with the
	 * minimum y side of the cube
	 */
	tmpy = miny - objc->MinY;
	if (((*dy) != 0) && ((*dy) < tmpy)) {
		where |= NEAR_MINY;
 		tmpz = ((*dz) * tmpy) / (*dy);
		tmpx = ((*dx) * tmpy) / (*dy);
		curz = objc->Pos.coord.t[2] + tmpz;
		curx = objc->Pos.coord.t[0] + tmpx;
		if (((curz >= minz) && (curz <= maxz)) && ((curx >= minx) && (curx <= maxx))) {
			where |= COLLISION_MINY;
			(*dz) = tmpz;
			(*dx) = tmpx;
			(*dy) = tmpy;
		}
	}
	
	/* Check if the collision is with the
	 * maximum y side of the cube
	 */
	tmpy = maxy - objc->MaxY;
	if (((*dy) != 0) && ((*dy) > tmpy)) {
		where |= NEAR_MAXY;
 		tmpz = ((*dz) * tmpy) / (*dy);
		tmpx = ((*dx) * tmpy) / (*dy);
		curz = objc->Pos.coord.t[2] + tmpz;
		curx = objc->Pos.coord.t[0] + tmpx;
		if (((curz >= minz) && (curz <= maxz)) && ((curx >= minx) && (curx <= maxx))) {
			where |= COLLISION_MAXY;
			(*dz) = tmpz;
			(*dx) = tmpx;
			(*dy) = tmpy;
		}
	}
	
	/* Check if the collision is with the
	 * minimum z side of the cube
	 */
	tmpz = minz - objc->MinZ;
	if (((*dz) != 0) && ((*dz) < tmpz)) {
		where |= NEAR_MINZ;
		tmpx = ((*dx) * tmpz) / (*dz);
		tmpy = ((*dy) * tmpz) / (*dz);
		curx = objc->Pos.coord.t[0] + tmpx;
		cury = objc->Pos.coord.t[1] + tmpy;
		if (((curx >= minx) && (curx <= maxx)) && ((cury >= miny) && (cury <= maxy))) {
			where |= COLLISION_MINZ;
			(*dx) = tmpx;
			(*dy) = tmpy;
			(*dz) = tmpz;
		}
	}
	
	/* Check if the collision is with the
	 * maximum z side of the cube
	 */
	tmpz = maxz - objc->MaxZ;
	if (((*dz) != 0) && ((*dz) > tmpz)) {
		where |= NEAR_MAXZ;
		tmpx = ((*dx) * tmpz) / (*dz);
		tmpy = ((*dy) * tmpz) / (*dz);
		curx = objc->Pos.coord.t[0] + tmpx;
		cury = objc->Pos.coord.t[1] + tmpy;
		if (((curx >= minx) && (curx <= maxx)) && ((cury >= miny) && (cury <= maxy))) {
			where |= COLLISION_MAXZ;
			(*dx) = tmpx;
			(*dy) = tmpy;
			(*dz) = tmpz;
		}
	}
	
	/* return the collision map for the object */
	return(where);
}

/*
 * Set the automatic division for a blocking object whenever
 * another object gets near it
 * ----------------------------------------------------------------------------
 */

void HandleObjcDivision(OBJECT *objc, OBJECT *bloc)
{
	short minx, miny, minz;
	short maxx, maxy, maxz;
	short curx, cury, curz;
	
	/* Quit if the object is not alive - i.e. it does not exist */
	if (!(objc->State & STATE_ALIVE))
		return;
	
	/* Quit if the blocking object is not alive - i.e. it does not exist */
	if (!(bloc->State & STATE_ALIVE))
		return;
	
	/* Quit if the blocking object is not divisible */
	if (!(bloc->State & STATE_DIVISIBLE))
		return;
	
	/* Calculate if the object is in the visinity of the blocking object */
	minx = bloc->MinX - (objc->DifX + DIV_THRESHOLD);
	maxx = bloc->MaxX + (objc->DifX + DIV_THRESHOLD);
	miny = bloc->MinY - (objc->DifY + DIV_THRESHOLD);
	maxy = bloc->MaxY + (objc->DifY + DIV_THRESHOLD);
	minz = bloc->MinZ - (objc->DifZ + DIV_THRESHOLD);
	maxz = bloc->MaxZ + (objc->DifZ + DIV_THRESHOLD);
	curx = objc->Pos.coord.t[0];
	cury = objc->Pos.coord.t[1];
	curz = objc->Pos.coord.t[2];
	if (((curx >= minx) && (curx <= maxx)) &&
		((cury >= miny) && (cury <= maxy)) &&
		((curz >= minz) && (curz <= maxz))) {
			bloc->Object.attribute |= GsDIV4;	
	}
	return;
}	

/*
 * Set the automatic division for walls of the cube whenever
 * another object gets near it
 * ----------------------------------------------------------------------------
 */

void HandleWallDivision(OBJECT *objc, OBJECT *wall)
{
	short minx, miny, minz;
	short maxx, maxy, maxz;
	short curx, cury, curz;
	
	/* Quit if the object is not alive - i.e. it does not exist */
	if (!(objc->State & STATE_ALIVE))
		return;
	
	/* Calculate if the object is in the visinity of the blocking object */
	minx = wall[0].MinX + (objc->DifX + DIV_THRESHOLD);
	maxx = wall[0].MaxX - (objc->DifX + DIV_THRESHOLD);
	miny = wall[0].MinY + (objc->DifY + DIV_THRESHOLD);
	maxy = wall[0].MaxY - (objc->DifY + DIV_THRESHOLD);
	minz = wall[0].MinZ + (objc->DifZ + DIV_THRESHOLD);
	maxz = wall[0].MaxZ - (objc->DifZ + DIV_THRESHOLD);
	curx = objc->Pos.coord.t[0];
	cury = objc->Pos.coord.t[1];
	curz = objc->Pos.coord.t[2];
	if (curx <= minx) wall[3].Object.attribute |= GsDIV1;
	if (curx >= maxx) wall[1].Object.attribute |= GsDIV1;
	if (cury <= miny) wall[4].Object.attribute |= GsDIV1;
	if (cury >= maxy) wall[5].Object.attribute |= GsDIV1;
	if (curz <= minz) wall[2].Object.attribute |= GsDIV1;
	if (curz >= maxz) wall[0].Object.attribute |= GsDIV1;
	return;
}	

/*
 * End of File
 * ----------------------------------------------------------------------------
 */
