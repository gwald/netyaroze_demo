/************************************************************
 *															*
 *						object.h							*
 *			   											    *															*
 *				LPGE     29/10/96							*		
 *															*
 *	Copyright (C) 1996 Sony Computer Entertainment Inc.		*
 *	All Rights Reserved										*
 *															*
 ***********************************************************/


#include <libps.h>

#include "tmd.h"
#include "asssert.h"

#ifndef TRUE
	#define FALSE 0
	#define TRUE 1
#endif

#ifndef NONE
	#define NONE 1001
#endif

#define THE_WORLD 1002		// special value for superCoordinateObjectID


#define MAX_OBJECTS		128



typedef struct
{
	int id;
	int alive;		 // boolean
	int type;		 
	int which;	
		
	GsDOBJ2	handler;
	u_long modelAddress;
	int whichModel;

	int scalingFlag;
	int scaleX, scaleY, scaleZ;			// scaling in 3D: volume control

	VECTOR position;		// position
	VECTOR velocity;		
	int movementMomentumFlag;

	SVECTOR rotate;			// angle
	VECTOR twist;			// change of angle
	int rotationMomentumFlag;

	GsCOORDINATE2 coord;	 // coordinate system
	int superCoordinateObjectID;

	int inOrbit;		// boolean
	int centralBodyID;		// id of body around which object orbits
	int angle;		   // within circle
	int angleIncrement;	  // speed of orbit: amount of increment per frame
	int whichWay;		  // CLOCKWISE or ANTICLOCKWISE
	int radius;			  // distance from central body
	VECTOR firstVector;		  // these two lie on the plane of orbit
	VECTOR secondVector;	  // second is perpendicular to first

	int specialMovement;		// flag to indicate arbitrary movement patterns		
} ObjectHandler;



extern ObjectHandler* ObjectArray[MAX_OBJECTS];




		// prototypes for object.c



/***************	first section: class functions				*************************/

void InitialiseObjectClass (void);
int FindNextLowestObjectID (int objectID);
int FindNextHighestObjectID (int objectID);
int FindNextUnusedObjectID (void);
int CountNumberOfLivingObjects (void);
void LinkAllObjectsToModels (void);
void LinkAllObjectsToTheirCoordinateSystems (void);

/***************	second section: individual object functions			***********************/
		
void BringObjectToLife (ObjectHandler* object, int type,
		u_long modelAddress, int whichModel, 
			int superCoordinateObjectID);
void KillAnObject (ObjectHandler* object);
void RegisterObjectIntoObjectArray (ObjectHandler* object);
void RemoveObjectFromObjectArray (ObjectHandler* object);
void InitSingleObject (ObjectHandler* object);
void HighlightObject (ObjectHandler* object);
void UnHighlightObject (ObjectHandler* object);


