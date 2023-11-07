/************************************************************
 *															*
 *						object.c							*
 *			   											    *															*
 *				LPGE     29/10/96							*		
 *															*
 *	Copyright (C) 1996 Sony Computer Entertainment Inc.		*
 *	All Rights Reserved										*
 *															*
 ***********************************************************/






// 	vital to keep separate the overall object-handling list
//  (an array of pointers) from the actual objects themselves;
//	this way we can keep object-handling code unchanged, while
// 	introducing new sub-arrays, e.g. TheShuttles, TheShips, etc;
//	enables us to refer to objects either by the global handling-list
//	or more directly, by 'the 3rd shuttle' i.e. TheShuttles[2]


// Note: one optimisation here is to store the free IDs in a stack
// (e.g. a simply linked list) of integers; this will make the
// functions 'find next free id' and 'free up an id' super fast
// but this isn't a significant optimisation unless lots of objects
// are being created/destroyed every frame

// Another optimisation is to keep the object array in maximally compacted state:
// ie so that all the existing objects are at the front, 
// hence all 'maplist'-type functions can stop walking the list
// the instant they find a non-existent object


		 


#include "object.h"



	// the array of object references
ObjectHandler* ObjectArray[MAX_OBJECTS];


MiniObject* MiniObjectArray[MAX_MINI_OBJECTS];


/**************				macros			***************************/



#define ALL_ONES 0xffffffff

u_long onlyNthBitOn, onlyNthBitOffMask;

#define TURN_NTH_BIT_OFF(argument, sizeInBits, N)			\
	{														\
	onlyNthBitOn = 1 << (N);								\
	onlyNthBitOffMask = ALL_ONES ^ onlyNthBitOn;			\
	argument &= onlyNthBitOn;								\
	}

 

/***************	first section: class functions				*************************/


		  


void InitialiseObjectClass (void)
{
	int i;

	for (i = 0; i < MAX_OBJECTS; i++)
		{
		ObjectArray[i] = NULL;
		}
}






	// find id of first existing object with lower id 
int FindNextLowestObjectID (int objectID)
{
	int nextLowestID;

 	nextLowestID = objectID-1;
	for (;;)
		{
		if (nextLowestID < 0)
			nextLowestID = MAX_OBJECTS-1;

		if (ObjectArray[nextLowestID] != NULL)
			{
			if (ObjectArray[nextLowestID]->alive == TRUE)
				break;
			}
		nextLowestID--;
		}

	return nextLowestID;
}


	// find id of first existing object with higher id
int FindNextHighestObjectID (int objectID)
{
	int nextHighestID;

	nextHighestID = objectID+1;
	for (;;)
		{
		if (nextHighestID >= MAX_OBJECTS)
			nextHighestID = 0;

		if (ObjectArray[nextHighestID] != NULL)
			{
			if (ObjectArray[nextHighestID]->alive == TRUE)
				break;
			}
		nextHighestID++;
		}

	return nextHighestID;
}




int FindNextUnusedObjectID (void)
{
	int id = -1;
	int i;

	for (i = 0; i < MAX_OBJECTS; i++)
		{
		if (ObjectArray[i] == NULL)
			{
			id = i;
			break;
			}
		}

	return id;
}



		




	
int CountNumberOfLivingObjects (void)
{
	int count = 0;
	int i;

	for (i = 0; i < MAX_OBJECTS; i++)
		{
		if (ObjectArray[i] != NULL)
			{
			if (ObjectArray[i]->alive == TRUE)
				count++;
			}
		}
	
	return count;
}




int CountNumberOfLivingTypedObjects (int type)
{
	int count = 0;
	int i;

	for (i = 0; i < MAX_OBJECTS; i++)
		{
		if (ObjectArray[i] != NULL)
			{
			if (ObjectArray[i]->alive == TRUE
				&& ObjectArray[i]->type == type)
					{
					count++;
					}
			}
		}

	return count;
}






	// all objects must be set up and registered in global array
	// before this function is called
void LinkAllObjectsToModelsOrSprites (void)
{
	ObjectHandler* object;
	int i;

	for (i = 0; i < MAX_OBJECTS; i++)
		{
		if (ObjectArray[i] != NULL)
			{
			if (ObjectArray[i]->alive == TRUE)
				{ 
				if (ObjectArray[i]->displayFlag == TMD)
					{
					object = ObjectArray[i];
					
					//printf("linking object type %d to model number %d\n",
						//object->type, object->whichModel);
					//printf("model at memory %u\n", object->modelAddress);
					
					LinkObjectHandlerToTmdObject( &(object->handler), 
						object->whichModel, object->modelAddress);
					//printf("after linking\n");
					}
				else if (ObjectArray[i]->displayFlag == SPRITE)
					{
					object = ObjectArray[i];

					LinkSpriteToImageInfo ( &object->sprite, object->imageInfo);
					}
				}
			}
		}
}



	// all objects must be set up and registered in global array
	// before this function is called

void LinkAllObjectsToTheirCoordinateSystems (void)
{
	ObjectHandler* object;
	int superCoordinateObjectID;
	int i;

	for (i = 0; i < MAX_OBJECTS; i++)
		{
		if (ObjectArray[i] != NULL)
			{
			if (ObjectArray[i]->alive == TRUE)
				{ 
				object = ObjectArray[i];

				superCoordinateObjectID = object->superCoordinateObjectID;
				//printf("object super coord obj. id: %d\n",
					//object->superCoordinateObjectID);	

				if (superCoordinateObjectID == NONE)  // set to own coord system
					{
					object->handler.coord2 
						= &(object->coord);
					//printf("linking to own coords\n");
					}
				else if (superCoordinateObjectID == THE_WORLD)
					{
					object->handler.coord2 = WORLD;
					//printf("linking to world\n");
					}
				else
					{
					object->handler.coord2 
						= &(ObjectArray[superCoordinateObjectID]->coord);
					//printf("linking to object %d\n", superCoordinateObjectID);
					}
				} 
			}
		}
}

	






/***************	second section: individual object functions			***********************/








void BringObjectToLife (ObjectHandler* object, int type,
		u_long modelAddress, int whichModel, 
			int superCoordinateObjectID)
{
	object->alive = TRUE;
	object->type = type;

	object->modelAddress = modelAddress;
	object->whichModel = whichModel;

		// note: use NONE for own independent coordinate system
	object->superCoordinateObjectID = superCoordinateObjectID;
}




void KillAnObject (ObjectHandler* object)
{
	object->alive = FALSE;
} 




void RegisterObjectIntoObjectArray (ObjectHandler* object)
{
	int nextFreeID;

	nextFreeID = FindNextUnusedObjectID();
	assert(nextFreeID != -1);
	assert(nextFreeID >= 0);
	assert(nextFreeID < MAX_OBJECTS);
	assert(ObjectArray[nextFreeID] == NULL);

	ObjectArray[nextFreeID] = object;
	object->id = nextFreeID;
}




void RemoveObjectFromObjectArray (ObjectHandler* object)
{
	int objectID;

	objectID = object->id;
	assert(objectID >= 0);
	assert(objectID < MAX_OBJECTS);
	assert(ObjectArray[objectID] != NULL);

	ObjectArray[objectID] = NULL;
	object->id = NONE;
}
	


	   




void InitSingleObject (ObjectHandler* object)
{
	int i;

	object->id = NONE;
	object->alive = FALSE;
	object->type = NONE;
	object->subType = NONE;
	object->which = NONE;

	object->displayFlag = TMD;		// by default

	object->imageInfo = NULL;
	InitGsSprite( &object->sprite);
	object->scaleX = ONE;
	object->scaleY = ONE;

	object->modelAddress = 0;
	object->whichModel = NONE;

	object->scalingFlag = FALSE;
	object->scalingVector.vx = ONE;
	object->scalingVector.vy = ONE;
	object->scalingVector.vz = ONE;
		   
		// start off stationary with no momentum
	object->position.vx = 0;
	object->position.vy = 0;
	object->position.vz = 0;
	object->velocity.vx = 0;
	object->velocity.vy = 0;
	object->velocity.vz = 0;
	object->movementMomentumFlag = FALSE;
	object->movementSpeed = 0;

	object->rotate.vx = 0;
	object->rotate.vy = 0;
	object->rotate.vz = 0;
	object->twist.vx = 0;
	object->twist.vy = 0;
	object->twist.vz = 0;
	object->rotationMomentumFlag = FALSE;
	object->rotationSpeed = 0;

		// this for initialisation, rather than link to WORLD coords
	GsInitCoordinate2(WORLD, &(object->coord) );
	object->handler.coord2 = &(object->coord);
	object->superCoordinateObjectID = NONE;

	object->matrix.m[0][0] = ONE;
	object->matrix.m[0][1] = 0;
	object->matrix.m[0][2] = 0;
	object->matrix.m[1][0] = 0;
	object->matrix.m[1][1] = ONE;
	object->matrix.m[1][2] = 0;
	object->matrix.m[2][0] = 0;
	object->matrix.m[2][1] = 0;
	object->matrix.m[2][2] = ONE;
	object->matrix.t[0] = 0;
	object->matrix.t[1] = 0;
	object->matrix.t[2] = 0;

	object->inOrbit = FALSE;
	object->centralBodyID = NONE;
	object->angle = 0;
	object->angleIncrement = 0;
	object->whichWay = NONE;
	object->radius = 0;
	object->firstVector.vx = 0;
	object->firstVector.vy = 0;
	object->firstVector.vz = 0;		
	object->secondVector.vx = 0;
	object->secondVector.vy = 0;
	object->secondVector.vz = 0;

	object->lifeTime = 0;
	object->specialTimeLimit = 0;
	object->secondPeriod = 0;
	object->thirdPeriod = 0;

	object->initialHealth = 0;
	object->currentHealth = 0;

	object->allegiance = NONE;

	object->canFire = FALSE;
	object->firingRate = 0;
	object->framesSinceLastFire = 0;
	object->shotSpeed = 0;	
	object->shotType = -1;

	object->actionState = -1;
	
	for (i = 0; i < 12; i++)
		object->objectIdList[i] = -1;
}


	   	
   
   	 

	// set axis-wise scaling parameters
	// ONE is normal (no-scale) size
	// this is RELATIVE scaling only
void SetObjectScaling (ObjectHandler* object, int scaleX,
						int scaleY, int scaleZ)
{
	object->scalingFlag = TRUE;

	object->scalingVector.vx = scaleX;
	object->scalingVector.vy = scaleY;
	object->scalingVector.vz = scaleZ;
}
						 



	/****
	 scale object in 3 ways
	 NOTE: with a hierarchical coordinate system,
	 any scaling of object X will affect all objects whose coord.
	 systems are children of X's coordinate systems
	****/
void SortObjectSize (ObjectHandler* object)
{
	if (object->scalingFlag == FALSE)
		return;
  
	ScaleMatrix(&object->coord.coord, &object->scalingVector);

		// tell graphics system that coordinate system is changed
	object->coord.flg = 0;
}




	// quicker to directly assign to attribute
	// but would lose all other attribute effects
void SetObjectSubdivision (ObjectHandler* object, int setting)
{
	TURN_NTH_BIT_OFF(object->handler.attribute, 32, 9);
	TURN_NTH_BIT_OFF(object->handler.attribute, 32, 10);
	TURN_NTH_BIT_OFF(object->handler.attribute, 32, 11);

	switch (setting)
		{
		case 0:
			break;
		case 1:
			object->handler.attribute |= GsDIV1;
			break;
		case 2:
			object->handler.attribute |= GsDIV2;
			break;
		case 3:
			object->handler.attribute |= GsDIV3;
			break;
		case 4:
			object->handler.attribute |= GsDIV4;
			break;
		case 5:
			object->handler.attribute |= GsDIV5;
			break;
		default:
			assert(FALSE);
		}

	//printf("setting: %d\n", setting);
	//printf("obj attribute: %d\n", object->handler.attribute);
}




	
	 

/**************		Section for mini objects 	***********************/





void InitialiseMiniObjectClass (void)
{
	int i;

	for (i = 0; i < MAX_MINI_OBJECTS; i++)
		{
		MiniObjectArray[i] = NULL;
		}
}






void InitMiniObject (MiniObject *miniObject)
{
	miniObject->id = -1;
	miniObject->alive = -1;
	miniObject->type = -1;

	GsInitCoordinate2(WORLD, &miniObject->coord);
	miniObject->handler.coord2 = WORLD;
	
	//GsInitCoordinate2(WORLD, &miniObject->coord);
	//miniObject->handler.coord = &miniObject->coord;
}  



void RegisterMiniObjectIntoMiniObjectArray (MiniObject *miniObject)
{
	int nextFreeID;

	nextFreeID = FindNextUnusedMiniObjectID();
	assert(nextFreeID != -1);
	assert(nextFreeID >= 0);
	assert(nextFreeID < MAX_MINI_OBJECTS);
	assert(MiniObjectArray[nextFreeID] == NULL);

	MiniObjectArray[nextFreeID] = miniObject;
	miniObject->id = nextFreeID;
}



int FindNextUnusedMiniObjectID (void)
{
	int id = -1;
	int i;

	for (i = 0; i < MAX_MINI_OBJECTS; i++)
		{
		if (MiniObjectArray[i] == NULL)
			{
			id = i;
			break;
			}
		}

	return id;
}






	// quicker to directly assign to attribute
	// but would lose all other attribute effects
void SetMiniObjectSubdivision (MiniObject* object, int setting)
{
	TURN_NTH_BIT_OFF(object->handler.attribute, 32, 9);
	TURN_NTH_BIT_OFF(object->handler.attribute, 32, 10);
	TURN_NTH_BIT_OFF(object->handler.attribute, 32, 11);

	switch (setting)
		{
		case 0:
			break;
		case 1:
			object->handler.attribute |= GsDIV1;
			break;
		case 2:
			object->handler.attribute |= GsDIV2;
			break;
		case 3:
			object->handler.attribute |= GsDIV3;
			break;
		case 4:
			object->handler.attribute |= GsDIV4;
			break;
		case 5:
			object->handler.attribute |= GsDIV5;
			break;
		default:
			assert(FALSE);
		}

	//printf("setting: %d\n", setting);
	//printf("obj attribute: %d\n", object->handler.attribute);
}
