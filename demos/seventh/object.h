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


#ifndef OBJECT_H_INCLUDED

#include <libps.h>

#include "asssert.h"

#include "tmd.h"

#include "2d1.h"

#include "general.h"
	


	// display flag: by GsSPRITE or by GsDOBJ2
#define TMD 0
#define SPRITE 1


#define MAX_OBJECTS 1024

#define MAX_MINI_OBJECTS 1024



	// value for object alive field
#define UNDEAD 2



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

	int lifeTime;
	int specialTimeLimit;
	int secondPeriod;
	int thirdPeriod;
	
	int initialHealth;
	int currentHealth;
	
	int allegiance;	
	
	int canFire;
	int firingRate;
	int framesSinceLastFire;
	int shotSpeed;
	int shotType;
	
	int actionState;
	
	int objectIdList[12];
	
	int collisionRadius;
	int xWidth, yWidth, zWidth;
	int collisionReactionType;	
	
	int tunnelSection;		
} ObjectHandler;



extern ObjectHandler* ObjectArray[MAX_OBJECTS];



typedef struct
{
	int id;
	int alive;		 // boolean
	int type;		 	
		
	GsDOBJ2	handler;
	u_long modelAddress;
	int whichModel;

	int scalingFlag;
	VECTOR scalingVector;			// scaling in 3D: volume control

	VECTOR position;		// position

	GsCOORDINATE2 coord;	 // coordinate system		
} MiniObject;


extern MiniObject* MiniObjectArray[MAX_MINI_OBJECTS];




		// prototypes for object.c



/***************	first section: class functions				*************************/

void InitialiseObjectClass (void);

int FindNextLowestObjectID (int objectID);
int FindNextHighestObjectID (int objectID);

int FindNextUnusedObjectID (void);

int CountNumberOfLivingObjects (void);
int CountNumberOfLivingTypedObjects (int type);

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

void SetObjectScaling (ObjectHandler* object, int scaleX,
						int scaleY, int scaleZ);
void SortObjectSize (ObjectHandler* object);




/*********			section for mini objects 			*********/



void InitialiseMiniObjectClass (void);


void InitMiniObject (MiniObject* object);


void BringMiniObjectToLife (MiniObject* object, int type,
		u_long modelAddress, int whichModel);


void RegisterMiniObjectIntoMiniObjectArray (MiniObject* object);
int FindNextUnusedMiniObjectID (void);


void LinkAllMiniObjectsToModels (void);


	   



#define OBJECT_H_INCLUDED 1

#endif