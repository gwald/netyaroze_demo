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
	int id;
	int found;			// boolean
	int i;

	found = -1;
	for (i = 0; i < MAX_OBJECTS; i++)
		{
		if (ObjectArray[i] == NULL)
			{
			found = 1;
			id = i;
			break;
			}
		}

	if (found == -1)	   // failure
		return -1;
	else
		return id;
}




int CountNumberObjectSlotsLeft (void)
{
	int count = 0;
	int i;

	for (i = 0; i < MAX_OBJECTS; i++)
		{
		if (ObjectArray[i] != NULL)
			{
				count++;
			}
		}
	
	return count;
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





	// all objects must be set up and registered in global array
	// before this function is called
void LinkAllObjectsToModels (void)
{
	ObjectHandler* object;
	int i;

	for (i = 0; i < MAX_OBJECTS; i++)
		{
		if (ObjectArray[i] != NULL)
			{
			if (ObjectArray[i]->alive == TRUE)
				{ 
				if (ObjectArray[i]->modelAddress != 0)
					{
					object = ObjectArray[i];
					
					printf("linking object type %d to model number %d\n",
						object->type, object->whichModel);
					printf("model at memory %u\n", object->modelAddress);

					LinkObjectHandlerToTmdObject( &(object->handler), 
						object->whichModel, object->modelAddress);
					printf("after linking\n");
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
				printf("object super coord obj. id: %d\n",
					object->superCoordinateObjectID);	

				if (superCoordinateObjectID == NONE)  // set to own coord system
					{
					object->handler.coord2 
						= &(object->coord);
					printf("linking to own coords\n");
					}
				else if (superCoordinateObjectID == THE_WORLD)
					{
					object->handler.coord2 = WORLD;
					printf("linking to world\n");
					}
				else
					{
					object->handler.coord2 
						= &(ObjectArray[superCoordinateObjectID]->coord);
					printf("linking to object %d\n", superCoordinateObjectID);
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
	object->id = NONE;
	object->alive = FALSE;
	object->type = NONE;
	object->subType = NONE;
	object->which = NONE;

	object->modelAddress = 0;
	object->whichModel = NONE;

	object->scalingFlag = FALSE;
	object->scalingVector.vx = ONE;
	object->scalingVector.vy = ONE;
	object->scalingVector.vz = ONE;

	object->movementTypeFlag = DYNAMIC;
	   
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

	object->specialMovement = NONE;

	object->lifeTime = 0;
	object->specialTimeLimit = 0;

	object->initialHealth = 0;
	object->currentHealth = 0;

	object->strategyFlag = NONE;
	object->currentActionFlag = NONE;
	object->numberFramesOfThisAction = 0;
		
	object->meritRating = 0;

	object->allegiance = NONE;

	object->firingRate = 0;
	object->framesSinceLastFire = 0;	
}


   


	// here, we Highlight the selected object by turning off the
	// light calculations

	// CAN HIGHLIGHT BY ANY METHOD WHATSOEVER
void HighlightObject (ObjectHandler* object)
{
	object->handler.attribute |= GsLOFF;
}



	// this would be unreadable if done within functions
	// NOTE: very slow, but very easy to optimise
#define TURN_NTH_BIT_OFF(argument, sizeInBits, N)						\
	{																	\
	u_long onlyNthBitOn, allButNthBitOn, allOnes = 0;					\
	int i;																\
																		\
	for (i = 0; i < (sizeInBits); i++)									\
		allOnes |= (1 << i);											\
																		\
	onlyNthBitOn = 1 << (N);											\
	allButNthBitOn = allOnes ^ onlyNthBitOn;							\
	(argument) &= allButNthBitOn;										\
	}



	// restore light calculations to Unhighligh
void UnHighlightObject (ObjectHandler* object)
{
	TURN_NTH_BIT_OFF(object->handler.attribute, 32, 6)
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





   
