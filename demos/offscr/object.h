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
#include <stdio.h>
#include "tmd.h"
#include "asssert.h"
#include "dump.h"

#ifndef TRUE
	#define FALSE 0
	#define TRUE 1
#endif

#ifndef NONE
	#define NONE 1001
#endif

#define THE_WORLD 1002		// special value for superCoordinateObjectID


	// display flag: by GsSPRITE or by GsDOBJ2
#define TMD 0
#define SPRITE 1


	// does object ever move?
#define STATIC 0
#define DYNAMIC 1


	// types of bullet
#define FIRE_SPRITE 0
#define TETRAHEDRON_MODEL 1



#define MAX_FOLLOWERS 24



#define MAX_OBJECTS 1024



typedef struct
{
	int id;
	int alive;		 // boolean
	int type;		 
	int subType;
	int which;	
	
	int displayFlag;		// TMD or SPRITE

	GsIMAGE* imageInfo;
	GsSPRITE sprite;
	int scaleX, scaleY;
		
	GsDOBJ2	handler;
	u_long modelAddress;
	int whichModel;

	int scalingFlag;
	VECTOR scalingVector;			// scaling in 3D: volume control

	int movementTypeFlag;		// STATIC, DYNAMIC

	VECTOR position;		// position
	VECTOR velocity;		
	int movementMomentumFlag;
	int movementSpeed;

	SVECTOR rotate;			// angle
	VECTOR twist;			// change of angle
	int rotationMomentumFlag;
	int rotationSpeed;

	GsCOORDINATE2 coord;	 // coordinate system
	int superCoordinateObjectID;
	MATRIX matrix;			// relation to SUPER

	int inOrbit;		// boolean
	int centralBodyID;		// id of body around which object orbits
	int angle;		   // within circle
	int angleIncrement;	  // speed of orbit: amount of increment per frame
	int whichWay;		  // CLOCKWISE or ANTICLOCKWISE
	int radius;			  // distance from central body
	VECTOR firstVector;		  // these two lie on the plane of orbit
	VECTOR secondVector;	  // second is perpendicular to first

	int specialMovement;		// flag to indicate arbitrary movement patterns

	int lifeTime;
	int specialTimeLimit;
} ObjectHandler;



extern ObjectHandler* ObjectArray[MAX_OBJECTS];




		// prototypes for object.c



/***************	first section: class functions				*************************/

void InitialiseObjectClass (void);
int FindNextLowestObjectID (int objectID);
int FindNextHighestObjectID (int objectID);
int FindNextUnusedObjectID (void);
int CountNumberObjectSlotsLeft (void);
int CountNumberOfLivingObjects (void);
void LinkAllObjectsToModelsOrSprites (void);
void LinkAllObjectsToTheirCoordinateSystems (void);

/***************	second section: individual object functions			***********************/
		
void BringObjectToLife (ObjectHandler* object, int type,
		u_long modelAddress, int whichModel, 
			int superCoordinateObjectID);
void KillAnObject (ObjectHandler* object);
void RegisterObjectIntoObjectArray (ObjectHandler* object);
void RemoveObjectFromObjectArray (ObjectHandler* object);
void InitSingleObject (ObjectHandler* object);
void ReviveObject (ObjectHandler* object);
void HighlightObject (ObjectHandler* object);
void UnHighlightObject (ObjectHandler* object);
void SetObjectScaling (ObjectHandler* object, int scaleX,
						int scaleY, int scaleZ);
void SortObjectSize (ObjectHandler* object);


	
void InitGsSprite (GsSPRITE* sprite);
 	// find coords wrt tpage top left points
void FindTopLeftOfTexturePage (GsIMAGE* imageInfo, 
									int* x, int* y, int* u, int* v);
void LinkObjectToSprite (ObjectHandler* object);



		 