/************************************************************
 *															*
 *						Tuto1.c								*
 *						3-D object manipulation				*
 *			   											    *															*
 *				LPGE     11/02/97							*		
 *															*
 *	Copyright (C) 1996 Sony Computer Entertainment Inc.		*
 *	All Rights Reserved										*
 *															*
 ***********************************************************/




/****************************************************************************
					includes
****************************************************************************/
	  


#include <stdio.h>

#include <rand.h>

#include <libps.h>

#include "pad.h"

#include "tmd.h"

#include "asssert.h"

#include "object.h"

#include "dump.h"

#include "rand.h"

#include "sound.h"

#include "text_str.h"

#include "address.h"

#include "tuto0.h"





/****************************************************************************
					constants, structures, globals
****************************************************************************/


typedef struct 
{
	int x, y, z;
	int w, h, d;   // width, height, depth
} CUBOID;


#define setCUBOID(cuboid, X, Y, Z, W, H, D)		\
	(cuboid)->x = (X),							\
	(cuboid)->y = (Y),							\
	(cuboid)->z = (Z),							\
	(cuboid)->w = (W),							\
	(cuboid)->h = (H),							\
	(cuboid)->d = (D)





int ScreenWidth, ScreenHeight;



#define PACKETMAX	2048		/* Max GPU packets */
#define PACKETMAX2	(PACKETMAX*24)
PACKET packetArea[2][PACKETMAX2]; /* GPU PACKETS AREA */






#define OT_LENGTH	9		
		
static GsOT Wot[2];			
static GsOT_TAG wtags[2][1<<OT_LENGTH]; 

 
	 
		
GsRVIEW2 TheView;	
int ProjectionDistance;	


GsF_LIGHT TheLights[3];	
int AmbientRed, AmbientGreen, AmbientBlue;
int BackRed, BackGreen, BackBlue;
short OverallLightMode;

GsFOGPARAM TheFogging;	







int nModels;

long* dop5;







#define	SQUARE_SIZE		128

#define FLOOR_X	9
#define FLOOR_Y	9
#define FLOOR_Z 9

#define	WALL		0x01



char worldmaps[FLOOR_X][FLOOR_Y][FLOOR_Z];

//GsCOORDINATE2 World[FLOOR_X][FLOOR_Y][FLOOR_Z];









int frameNumber = 0;

int QuitFlag = FALSE;





	// object types
#define PLAYERS_SHIP 0
#define ENEMY 1
#define SQUARE 2


ObjectHandler TheShip;


#define MAX_SHIPS 12
ObjectHandler TheShips[MAX_SHIPS];


#define MAX_SQUARES 920
ObjectHandler TheSquares[MAX_SQUARES];





	// graphical world clipping
ObjectHandler* DisplayedObjects[MAX_OBJECTS];


 	 



	// simple enemy strategies
#define WAITING 0
#define STATIONARY_TRACK 1
#define SEEKING 2

#define NUMBER_ENEMY_STRATEGIES 3

  
 		 

	// how the world landscape is generated 
#define RANDOM_WORLD 0
#define SET_WORLD 1
int WorldType;




VECTOR scalingVector;		// for the squares -> smoother walls



/****************************************************************************
					prototypes
****************************************************************************/

void main (void);

int PerformWorldClipping (ObjectHandler** firstArray, 
								ObjectHandler** secondArray);


void FindViewPoint (VECTOR* output);

void FindViewDirection (VECTOR* output);

void ScaleVectorToUnit (VECTOR* vector);


void DealWithControllerPad (void);



void InitialiseAll (void);
void InitialiseObjects (void);
void InitialiseShipAccordingToStrategy (ObjectHandler* object);

void CreateTheCube (void);

void CleanupAndExit (void);

void HandleAllObjects (void);

void HandleTheShip (ObjectHandler* object);
void HandleAnEnemy (ObjectHandler* object);
void HandleASquare (ObjectHandler* object);



void UpdateObjectCoordinates (VECTOR* twist,
							VECTOR* position, VECTOR* velocity,
							GsCOORDINATE2* coordSystem, MATRIX* matrix);
void UpdateObjectCoordinates2 (SVECTOR* rotationVector,
							VECTOR* translationVector,
							GsCOORDINATE2* coordSystem);



void UpdateModelCoords (SVECTOR* rotationVector,
									GsCOORDINATE2* coordSystem);


int CollisionWithWalls (VECTOR* position, VECTOR* movement);	

int NotSafe (CUBOID* rectangle);




void InitialiseView (void);
void InitialiseLighting (void);
void SortVideoMode (void);




void GenerateWorld (void);
void GenerateRandomWorld (void);
void GenerateSetWorld (void);

void CreateSquaresForWorld (void);




void PositionSomewhereNotInAWall (ObjectHandler* object);


void HandleEnemyBehaviour (ObjectHandler* object);

void HandleWaitingShipsBehaviour (ObjectHandler* object);
void HandleStationaryTrackingShipsBehaviour (ObjectHandler* object);
void HandleSeekingShipBehaviour (ObjectHandler* object);

	  

void CheckCollisions (void); 

int ObjectsVeryClose (ObjectHandler* first, ObjectHandler* second);


u_short SortSpriteObjectPosition (ObjectHandler* object);


int FindNextFreeObjectSubtypeID (int subType);
ObjectHandler* FindNextFreeObjectSubtype (int subType);



/****************************************************************************
					 macros
****************************************************************************/


#define min(a,b) ( ((a) > (b)) ? (b) : (a))

#define max(a,b) ( ((a) > (b)) ? (a) : (b))

#define KeepWithinRange(quantity, min, max)					\
{															\
	if ((quantity) < (min))									\
		(quantity) = (min);									\
	else if ((quantity) > (max))							\
		(quantity) = (max);									\
}


#define setVECTOR(vector, x, y, z)				\
				(vector)->vx = (x), (vector)->vy = (y), (vector)->vz = (z)	

	


#define TURN_NTH_BIT_OFF(argument, sizeInBits, N)			\
	{														\
	u_long onlyNthBitOn;									\
															\
	onlyNthBitOn = 1 << (N);								\
															\
	argument ^= onlyNthBitOn;								\
	}





/****************************************************************************
					 functions
****************************************************************************/

void main (void)
{
	int	i;
	int	hsync = 0;
	int bufferIndex;			 
	MATRIX	tmpls;
	ObjectHandler* object;
	int x, y, z;
	int numberToDisplay;
	u_short zValue;
	VECTOR viewPoint;
	int cpuLoad = 0, gpuLoad = 0;

	InitialiseAll();

	bufferIndex = GsGetActiveBuff();

	while (QuitFlag == FALSE)
		{
		FntPrint("Num models: %d\nHsync = %d\n", nModels, hsync);
		FntPrint("frame %d\n", frameNumber); 

		FntPrint("ship: %d %d %d\n", 
			TheShip.position.vx, 
			TheShip.position.vy,
			TheShip.position.vz);
			 
		x = TheShip.position.vx / SQUARE_SIZE;	
		y = TheShip.position.vy / SQUARE_SIZE;
		z = TheShip.position.vz / SQUARE_SIZE;
		//FntPrint("ship grid: %d %d %d\n", x, y, z);	
		
		//FntPrint("cpu: %d, gpu: %d\n", cpuLoad, gpuLoad);	 
		
		frameNumber++;

		DealWithControllerPad();			

		GsSetRefView2( &TheView);

		GsSetWorkBase( (PACKET*)packetArea[bufferIndex]);

		GsClearOt(0, 0, &Wot[bufferIndex]);

		HandleAllObjects();

		FindViewPoint( &viewPoint);
		FntPrint("VP: %d %d %d\n", viewPoint.vx, 
							viewPoint.vy, viewPoint.vz);

		numberToDisplay 
					= PerformWorldClipping(ObjectArray, DisplayedObjects);
		FntPrint("num to display: %d\n", numberToDisplay);

		FntPrint("FOG: %d %d\n", TheFogging.dqa, TheFogging.dqb);

		for (i = 0; i < numberToDisplay; i++)
			{
			object = DisplayedObjects[i];

			if (object->displayFlag == TMD)
				{
				GsGetLs(&(object->coord), &tmpls);
						   
				GsSetLightMatrix(&tmpls);
							
				GsSetLsMatrix(&tmpls);
							
				GsSortObject4( &(object->handler), 
						&Wot[bufferIndex], 
							3, getScratchAddr(0));
				}
			else
				{
				zValue = SortSpriteObjectPosition(object);
				GsSortSprite( &object->sprite, &Wot[bufferIndex], zValue);
				}
			}

#if 0		// 'profiling': DrawSync seems to slow things down alot 
		cpuLoad = VSync(1);
		DrawSync(0);
		gpuLoad = VSync(1);
#endif

		hsync = VSync(0);	  
		
		
		//ResetGraph(1);		// seems unnecesary


		GsSwapDispBuff();
		GsSortClear(BackRed, BackGreen, BackBlue, &Wot[bufferIndex]);
		GsDrawOt(&Wot[bufferIndex]);
		bufferIndex ^= 1;
		FntFlush(-1);
		}

	CleanupAndExit();
}


	 

	// look at objects in first array,
	// if they're close to view point,
	// put them into second array

	// NOTE: better clipping (more of it) would result from taking the ship's view
	// i.e. since the overall view is attached to the ship, express all objects
	// in ship's coordinate system, now much easier to see if objects should be
	// clipped: draw none outside given XY (screen) limits, and none that are
	// less than (ProjectionDistance/2) ahead in z axis

int PerformWorldClipping (ObjectHandler** firstArray, ObjectHandler** secondArray)
{
	int numberDisplayedObjects = 0;
	int i;
	VECTOR viewPoint, viewDirection;
	int SimpleClipDistance;
	int SubdivisionDistance;
	static int debugCount = 0;

	FindViewPoint( &viewPoint);
	FindViewDirection( &viewDirection);

	SimpleClipDistance = SQUARE_SIZE * 4;
	SubdivisionDistance = SQUARE_SIZE * 2;
	
	for (i = 0; i < MAX_OBJECTS; i++)
		{
		if (firstArray[i] != NULL)
			if (firstArray[i]->alive == TRUE)
				{
				if (abs(firstArray[i]->coord.coord.t[0] - viewPoint.vx) < SimpleClipDistance
					&& abs(firstArray[i]->coord.coord.t[1] - viewPoint.vy) < SimpleClipDistance
					&& abs(firstArray[i]->coord.coord.t[2] - viewPoint.vz) < SimpleClipDistance)
					{
					secondArray[numberDisplayedObjects] = firstArray[i];
					if (abs(firstArray[i]->coord.coord.t[0] - viewPoint.vx) < SubdivisionDistance
						&& abs(firstArray[i]->coord.coord.t[1] - viewPoint.vy) < SubdivisionDistance
						&& abs(firstArray[i]->coord.coord.t[2] - viewPoint.vz) < SubdivisionDistance)
                            {         // NO MORE
                            //secondArray[numberDisplayedObjects]->handler.attribute |= GsDIV1;
                            //if (debugCount % 100 == 0)
                            //    printf("subd ON %d\n", secondArray[numberDisplayedObjects]->handler.attribute);
							}
					else
                            {        // NO MORE
                            //TURN_NTH_BIT_OFF(secondArray[numberDisplayedObjects]->handler.attribute, 32, 9);
                            //if (debugCount % 100 == 0)
                            //    printf("subd OFF %d\n", secondArray[numberDisplayedObjects]->handler.attribute);
							}
					numberDisplayedObjects++;
					debugCount++;
					}
				}
		}
		
	return numberDisplayedObjects;	
}		




void FindViewPoint (VECTOR* output)
{
	if (TheView.super == WORLD)
		{
		setVECTOR( output, TheView.vpx, TheView.vpy, TheView.vpz);
		}
	else if (TheView.super == &TheShip.coord)
		{			// assuming that ONLY TheView.vpz != 0 ....
		VECTOR zDisplacement;

		setVECTOR( &zDisplacement, ((TheShip.coord.coord.m[0][2] * TheView.vpz) >> 12),
							((TheShip.coord.coord.m[1][2] * TheView.vpz) >> 12),
							((TheShip.coord.coord.m[2][2] * TheView.vpz) >> 12) );

		setVECTOR( output, (TheShip.position.vx + zDisplacement.vx), 
						(TheShip.position.vy + zDisplacement.vy), 
						(TheShip.position.vz + zDisplacement.vz) );
		}
}




void FindViewDirection (VECTOR* output)
{
	if (TheView.super == WORLD)
		{
		setVECTOR(output, TheView.vrx - TheView.vpx, 
							TheView.vry - TheView.vpy,
							TheView.vrz - TheView.vpz);

		ScaleVectorToUnit(output);
		}
	else if (TheView.super == &TheShip.coord)
		{			// assuming that ONLY TheView.vpz != 0 ....
					// and view where reference point at centre of the ship
		setVECTOR(output, ((TheShip.coord.coord.m[0][2] * TheView.vpz) >> 12),
							((TheShip.coord.coord.m[1][2] * TheView.vpz) >> 12),
							((TheShip.coord.coord.m[2][2] * TheView.vpz) >> 12) );

		ScaleVectorToUnit(output);
		}
}




void ScaleVectorToUnit (VECTOR* vector)
{
	int length, square;

	square = pow(vector->vx, 2) + pow(vector->vy, 2) 
					+ pow(vector->vz, 2); 

	length = pow(square, 0.5);
	assert(length != 0);	   // never allow division by zero

	vector->vx *= ONE;
	vector->vy *= ONE;
	vector->vz *= ONE;

	vector->vx /= length;
	vector->vy /= length;
	vector->vz /= length;
}


	  

void DealWithControllerPad (void)
{
	long pad;
	int controlSpeed;
  
	pad = PadRead();
	
	if (pad & PADselect)
		controlSpeed = 5;
	else
		controlSpeed = 1;

	if (pad & PADL1)
		{
		if (pad & PADRup)
			{
			TheFogging.dqa -= 20 * controlSpeed;
			GsSetFogParam( &TheFogging);
			return;
			}
		if (pad & PADRdown)
			{
			TheFogging.dqa += 20 * controlSpeed;
			GsSetFogParam( &TheFogging);
			return;
			}
		if (pad & PADRleft)
			{
			TheFogging.dqb -= 200 * controlSpeed;
			GsSetFogParam( &TheFogging);
			return;
			}
		if (pad & PADRright)
			{
			TheFogging.dqb += 200 * controlSpeed;
			GsSetFogParam( &TheFogging);
			return;
			}
		}

		// movement
	if (pad & PADLup)
		TheShip.velocity.vz -= TheShip.movementSpeed * controlSpeed;
	if (pad & PADLdown)
		TheShip.velocity.vz += TheShip.movementSpeed * controlSpeed;

		// rotation
	if (pad & PADRup)
		TheShip.twist.vx += TheShip.rotationSpeed * controlSpeed;
	if (pad & PADRdown)
		TheShip.twist.vx -= TheShip.rotationSpeed * controlSpeed;
	if (pad & PADR2)
		TheShip.twist.vz -= TheShip.rotationSpeed * controlSpeed;
	if (pad & PADR1)
		TheShip.twist.vz += TheShip.rotationSpeed * controlSpeed;
	if (pad & PADRleft)
		TheShip.twist.vy -= TheShip.rotationSpeed * controlSpeed;
	if (pad & PADRright)
		TheShip.twist.vy += TheShip.rotationSpeed * controlSpeed;


		// L1 and L2: step slowly
	if (pad & PADL1 && pad & PADL2)
		{
		int frameCount;	 
		frameCount = VSync(-1);
		while (frameCount + 25 > VSync(-1))
			{
			;
			}
		}

		// quit program
	if (pad & PADstart && pad & PADselect)
		{
		QuitFlag = TRUE;
		}

		// pause
	if (pad & PADstart)
		{
		while (pad & PADstart)
			{
			pad = PadRead();
			}
		}
}

 

 




void InitialiseAll (void)
{
	PadInit();

	InitialiseRandomNumbers();

	InitialiseTextStrings();

	InitialiseSound();

		// scale TMD of square to right size
	scalingVector.vx = 7659;
	scalingVector.vy = 7059;
	scalingVector.vz = 7779;

	ScreenWidth = 320;
	ScreenHeight = 240;

	GsInitGraph(ScreenWidth, ScreenHeight, GsINTER|GsOFSGPU, 1, 0);
   	GsDefDispBuff(0, 0, 0, ScreenHeight);

	GsInit3D();		   

	SortVideoMode();

	Wot[0].length = OT_LENGTH;	
	Wot[0].org = wtags[0];	   
	Wot[1].length = OT_LENGTH;
	Wot[1].org = wtags[1];

	GsClearOt(0, 0 ,&Wot[0]);
	GsClearOt(0, 0, &Wot[1]);

	InitialiseView();
		
	InitialiseLighting();		   

	InitialiseTexture(SQUARE_TEXTURE_ADDRESS);
	
	dop5 = InitialiseTMD( (long*) SQUARE_MODEL_ADDRESS);
	
	InitialiseObjects();

	WorldType = SET_WORLD;
	GenerateWorld();

	FntLoad(960, 256);
	FntOpen(-40, 0, 256, 200, 0, 512);	  
}





void InitialiseObjects (void)
{
	int i;

	InitialiseObjectClass();
	



	InitSingleObject(&TheShip);

	BringObjectToLife (&TheShip, PLAYERS_SHIP, 
					SHIP_MODEL_ADDRESS, 0, NONE);

	//PositionSomewhereNotInAWall(&TheShip);
	setVECTOR( &TheShip.position, SQUARE_SIZE/2, SQUARE_SIZE/2, SQUARE_SIZE/2); 

	TheShip.movementSpeed = 20;
	TheShip.rotationSpeed = 55;
	//TheShip.movementMomentumFlag = TRUE;

	RegisterObjectIntoObjectArray(&TheShip);
		

  
	
	for (i = 0; i < MAX_SHIPS; i++)
		{
		InitSingleObject(&TheShips[i]);

		BringObjectToLife(&TheShips[i], ENEMY, 
			SHIP_MODEL_ADDRESS, 0, NONE);

		TheShips[i].initialHealth = 1;
		TheShips[i].currentHealth = 1; 

			// set strategy randomly
		TheShips[i].strategyFlag = rand() % (NUMBER_ENEMY_STRATEGIES-1);

		InitialiseShipAccordingToStrategy(&TheShips[i]);

		PositionSomewhereNotInAWall(&TheShips[i]);

		TheShips[i].allegiance = ENEMY;

		RegisterObjectIntoObjectArray(&TheShips[i]);
		}


	
	for (i = 0; i < MAX_SQUARES; i++)
		{
		InitSingleObject(&TheSquares[i]);

		BringObjectToLife(&TheSquares[i], SQUARE, 
			SQUARE_MODEL_ADDRESS, 0, NONE);

		RegisterObjectIntoObjectArray(&TheSquares[i]);
		}




	LinkAllObjectsToModelsOrSprites();
	LinkAllObjectsToTheirCoordinateSystems();


	for (i = 0; i < MAX_SHIPS; i++)
		{
		TheShips[i].alive = FALSE;
		}
	for (i = 0; i < MAX_SQUARES; i++)
		{
		TheSquares[i].alive = FALSE;
		}
}





void InitialiseShipAccordingToStrategy (ObjectHandler* object)
{
	object->movementSpeed = 32;
	object->rotationSpeed = 45;		

	switch(object->strategyFlag)
		{
		case WAITING:	 
			object->specialTimeLimit = 400 + (100 * (rand() % 4));
			object->firingRate = 6;
			break;
		case STATIONARY_TRACK:
			object->firingRate = 5;	
			break;	
		case SEEKING:
			object->firingRate = 7;
			break;
		default:
			assert(FALSE);
		}
}






	// make six large walls, each from square tiles

void CreateTheCube (void)
{
	int x, y, z;
	int xPos, yPos, zPos;
	int squareID = 0;
	ObjectHandler* square;

	yPos = FLOOR_Y * SQUARE_SIZE;
	for (x = 0; x < FLOOR_X; x++)		   // this block: the floor
		{
		for (z = 0; z < FLOOR_Z; z++)
			{			 
			xPos = SQUARE_SIZE/2 + x * SQUARE_SIZE;
			zPos = SQUARE_SIZE/2 + z * SQUARE_SIZE;

			square = &TheSquares[squareID];

			square->alive = TRUE;

			setVECTOR( &square->position, xPos, yPos, zPos);

			setVECTOR( &square->rotate, ONE/2, 0, 0);

			UpdateObjectCoordinates2 (&square->rotate, 
				&square->position, &square->coord);
			ScaleMatrix(&square->coord.coord, &scalingVector);
			square->coord.flg = 0;

			setVECTOR( &square->rotate, 0, 0, 0); 

			squareID++;
			}
		}

	for (x = 0; x < FLOOR_X; x++)			 // this block: the ceiling
		{
		for (z = 0; z < FLOOR_Z; z++)
			{			 
			xPos = SQUARE_SIZE/2 + x * SQUARE_SIZE;
			zPos = SQUARE_SIZE/2 + z * SQUARE_SIZE;

			square = &TheSquares[squareID];

			square->alive = TRUE;

			setVECTOR( &square->position, xPos, 0, zPos);

			UpdateObjectCoordinates2 (&square->rotate, 
				&square->position, &square->coord);
				
			ScaleMatrix(&square->coord.coord, &scalingVector);
			square->coord.flg = 0;

			squareID++;
			}
		}




	xPos = FLOOR_X * SQUARE_SIZE;
	for (y = 0; y < FLOOR_Y; y++)				// this block: the right wall
		{
		for (z = 0; z < FLOOR_Z; z++)
			{			 
			yPos = SQUARE_SIZE/2 + y * SQUARE_SIZE;
			zPos = SQUARE_SIZE/2 + z * SQUARE_SIZE;

			square = &TheSquares[squareID];

			square->alive = TRUE;

			setVECTOR( &square->position, xPos, yPos, zPos);

			setVECTOR( &square->rotate, 0, 0, ONE/4);

			UpdateObjectCoordinates2 (&square->rotate, 
				&square->position, &square->coord);

			ScaleMatrix(&square->coord.coord, &scalingVector);
			square->coord.flg = 0;

			setVECTOR( &square->rotate, 0, 0, 0);

			squareID++;
			}
		}

	for (y = 0; y < FLOOR_Y; y++)			   // this block: the left wall
		{
		for (z = 0; z < FLOOR_Z; z++)
			{			 
			yPos = SQUARE_SIZE/2 + y * SQUARE_SIZE;
			zPos = SQUARE_SIZE/2 + z * SQUARE_SIZE;

			square = &TheSquares[squareID];

			square->alive = TRUE;

			setVECTOR( &square->position, 0, yPos, zPos);

			setVECTOR( &square->rotate, 0, 0, 3*ONE/4);

			UpdateObjectCoordinates2 (&square->rotate, 
				&square->position, &square->coord);

			ScaleMatrix(&square->coord.coord, &scalingVector);
			square->coord.flg = 0;
					
			setVECTOR( &square->rotate, 0, 0, 0);

			squareID++;
			}
		}



	zPos = FLOOR_Z * SQUARE_SIZE;
	for (y = 0; y < FLOOR_Y; y++)				// this block: the far wall
		{
		for (x = 0; x < FLOOR_X; x++)
			{			 
			yPos = SQUARE_SIZE/2 + y * SQUARE_SIZE;
			xPos = SQUARE_SIZE/2 + x * SQUARE_SIZE;

			square = &TheSquares[squareID];

			square->alive = TRUE;

			setVECTOR( &square->position, xPos, yPos, zPos);

			setVECTOR( &square->rotate, 3*ONE/4, 0, 0);

			UpdateObjectCoordinates2 (&square->rotate, 
				&square->position, &square->coord);

			ScaleMatrix(&square->coord.coord, &scalingVector);
			square->coord.flg = 0;

			setVECTOR( &square->rotate, 0, 0, 0);

			squareID++;
			}
		}

	for (y = 0; y < FLOOR_Y; y++)					   // this block: the near wall
		{
		for (x = 0; x < FLOOR_X; x++)
			{			 
			yPos = SQUARE_SIZE/2 + y * SQUARE_SIZE;
			xPos = SQUARE_SIZE/2 + x * SQUARE_SIZE;

			square = &TheSquares[squareID];

			square->alive = TRUE;

			setVECTOR( &square->position, xPos, yPos, 0);

			setVECTOR( &square->rotate, ONE/4, 0, 0);

			UpdateObjectCoordinates2 (&square->rotate, 
				&square->position, &square->coord);

			ScaleMatrix(&square->coord.coord, &scalingVector);
			square->coord.flg = 0;
					
			setVECTOR( &square->rotate, 0, 0, 0);

			squareID++;
			}
		}
	//printf("squareID: %d\n", squareID);
}




void CleanupAndExit (void)
{
	//StoreScreen();

	ResetGraph(3);

	CleanUpSound();

		// if this program part of a multiple module,
		// can printf to siocons to tell user to
		// invoke a new batch file, etc
}

 



void HandleAllObjects (void)
{
	ObjectHandler* object;
	int i;

	CheckCollisions();

	for (i = 0; i < MAX_OBJECTS; i++)
		{
		if (ObjectArray[i] != NULL)
			{
			if (ObjectArray[i]->alive == TRUE)
				{
				object = ObjectArray[i];
				object->lifeTime++;

				switch(object->type)
					{
					case PLAYERS_SHIP:
						HandleTheShip(object);
						break;
					case ENEMY:
						HandleAnEnemy(object);
						break;
					case SQUARE:
						//HandleASquare(object);	 // nothing needs doing ...
						// EXCEPT handling subdivision for very close ones ..........
						break;
					default:
						assert(FALSE);
					}
				}
			}
		}
}





void HandleTheShip (ObjectHandler* object)
{
	assert(object->type == PLAYERS_SHIP);

	UpdateObjectCoordinates(&object->twist, &object->position, 
								&object->velocity, &object->coord, &object->matrix);
							
	SortObjectSize(object);

	KeepWithinRange(object->position.vx, -SQUARE_SIZE/2, FLOOR_X * SQUARE_SIZE -SQUARE_SIZE/2);
	KeepWithinRange(object->position.vy, -SQUARE_SIZE/2, FLOOR_Y * SQUARE_SIZE -SQUARE_SIZE/2);
	KeepWithinRange(object->position.vz, -SQUARE_SIZE/2, FLOOR_Z * SQUARE_SIZE -SQUARE_SIZE/2);

		// momentum or not? 
	if (object->movementMomentumFlag == FALSE)
		{
		object->velocity.vx = 0;
		object->velocity.vy = 0;
		object->velocity.vz = 0;
		}
	if (object->rotationMomentumFlag == FALSE)
		{
		object->twist.vx = 0;
		object->twist.vy = 0;
		object->twist.vz = 0;
		}
}
   




void HandleAnEnemy (ObjectHandler* object)
{
	assert(object->type == ENEMY);

	HandleEnemyBehaviour(object);

	UpdateObjectCoordinates(&object->twist, &object->position, 
				&object->velocity, &object->coord, &object->matrix);

	SortObjectSize(object);

	KeepWithinRange(object->position.vx, -SQUARE_SIZE/2, FLOOR_X * SQUARE_SIZE -SQUARE_SIZE/2);
	KeepWithinRange(object->position.vy, -SQUARE_SIZE/2, FLOOR_Y * SQUARE_SIZE -SQUARE_SIZE/2);
	KeepWithinRange(object->position.vz, -SQUARE_SIZE/2, FLOOR_Z * SQUARE_SIZE -SQUARE_SIZE/2);

		// momentum or not? 
	if (object->movementMomentumFlag == FALSE)
		{
		object->velocity.vx = 0;
		object->velocity.vy = 0;
		object->velocity.vz = 0;
		}
	if (object->rotationMomentumFlag == FALSE)
		{
		object->twist.vx = 0;
		object->twist.vy = 0;
		object->twist.vz = 0;
		}
}




void HandleASquare (ObjectHandler* object)
{
	assert(object->type == SQUARE);
}



	// this function does object-relative movement and rotation
void UpdateObjectCoordinates (VECTOR* twist,
							VECTOR* position, VECTOR* velocity,
							GsCOORDINATE2* coordSystem, MATRIX* matrix)
{	
	VECTOR realMovement;
	MATRIX xMatrix, yMatrix, zMatrix;
	SVECTOR xVector, yVector, zVector;
	
		// find the object-local velocity in super coordinate terms
	ApplyMatrixLV(matrix, velocity, &realMovement);

	if (CollisionWithWalls(position, &realMovement) == FALSE)  
		{
			// update position by actual movement
		position->vx += realMovement.vx;
		position->vy += realMovement.vy;
		position->vz += realMovement.vz;
		}
	else
		{
		velocity->vx = 0;
		velocity->vy = 0;
		velocity->vz = 0;
		}

	if (twist->vx != 0)
		{
		xVector.vx = twist->vx;
		xVector.vy = 0;
		xVector.vz = 0;
			
		RotMatrix(&xVector, &xMatrix);

		MulMatrix0(matrix, &xMatrix, matrix);
		}
	else if (twist->vy != 0)
		{
		yVector.vx = 0;
		yVector.vy = twist->vy;
		yVector.vz = 0;
			
		RotMatrix(&yVector, &yMatrix);

		MulMatrix0(matrix, &yMatrix, matrix);
		}
	else if (twist->vz != 0)
		{
		zVector.vx = 0;
		zVector.vy = 0;
		zVector.vz = twist->vz;
			
		RotMatrix(&zVector, &zMatrix);

		MulMatrix0(matrix, &zMatrix, matrix);
		}
	 
	coordSystem->coord = *matrix;
	
		// set position absolutely	
	coordSystem->coord.t[0] = position->vx;
	coordSystem->coord.t[1] = position->vy;
	coordSystem->coord.t[2] = position->vz;

		// tell GTE that coordinate system has been updated
	coordSystem->flg = 0;
}

	  





	// this does world-relative movement and rotation
void UpdateObjectCoordinates2 (SVECTOR* rotationVector,
							VECTOR* translationVector,
							GsCOORDINATE2* coordSystem)
{
	MATRIX tempMatrix;

		// get rotation matrix from rotation vector
	RotMatrix(rotationVector, &tempMatrix);

		// assign new matrix to coordinate system
	coordSystem->coord = tempMatrix;
	
		// set position by absolute coordinates	
	coordSystem->coord.t[0] = translationVector->vx;
	coordSystem->coord.t[1] = translationVector->vy;
	coordSystem->coord.t[2] = translationVector->vz;

		// tell GTE that coordinate system has been updated
	coordSystem->flg = 0;
}







void UpdateModelCoords (SVECTOR* rotationVector,
									GsCOORDINATE2* coordSystem)
{
	MATRIX tempMatrix;

	tempMatrix.t[0] = coordSystem->coord.t[0];
	tempMatrix.t[1] = coordSystem->coord.t[1];
	tempMatrix.t[2] = coordSystem->coord.t[2];      

		// get rotation matrix from rotation vector
	RotMatrix(rotationVector, &tempMatrix);

		// assign new matrix to coordinate system
	coordSystem->coord = tempMatrix;
}





	// dimension of collision detection
int CollisionWidth = SQUARE_SIZE/8; 

int CollisionWithWalls (VECTOR* position, VECTOR* movement)
{
	CUBOID cuboid;

	setCUBOID(&cuboid, position->vx + movement->vx, 
					position->vy + movement->vy, 
						position->vz + movement->vz, 
						CollisionWidth, CollisionWidth, CollisionWidth);

	if (NotSafe(&cuboid))
		return TRUE;

	return FALSE;
}
	

	 




int NotSafe (CUBOID* cuboid)
{
	int x1, y1, z1;
	int x2, y2, z2;
	int gx1, gy1, gz1;
	int gx2, gy2, gz2;
	
	x1 = (cuboid->x - (cuboid->w/2));	  
	y1 = (cuboid->y - (cuboid->h/2));
	z1 = (cuboid->z - (cuboid->d/2));
	x2 = (cuboid->x + (cuboid->w/2));
	y2 = (cuboid->y + (cuboid->h/2));
	z2 = (cuboid->z + (cuboid->d/2));

	//printf("x1 %d x2 %d\n", x1, x2);
	//printf("y1 %d y2 %d\n", y1, y2);
	//printf("z1 %d z2 %d\n", z1, z2);

	if (x1 < -SQUARE_SIZE/2 + CollisionWidth)
		return TRUE;
	if (y1 < -SQUARE_SIZE/2 + CollisionWidth)
		return TRUE;
	if (z1 < -SQUARE_SIZE/2 + CollisionWidth)
		return TRUE;
	if (x2 >= ((FLOOR_X * SQUARE_SIZE) -SQUARE_SIZE/2 - CollisionWidth))
		return TRUE;
	if (y2 >= ((FLOOR_Y * SQUARE_SIZE) -SQUARE_SIZE/2 - CollisionWidth))
		return TRUE;
	if (z2 >= ((FLOOR_Z * SQUARE_SIZE) -SQUARE_SIZE/2 - CollisionWidth))
		return TRUE;

		// adding SQUARE_SIZE/2: adjustment for real positions of squares
		// which are offset from grid cube centres
		// this DODGY: needs redoing properly
	gx1 = (x1 + SQUARE_SIZE/2) / SQUARE_SIZE;
	gy1 = (y1 + SQUARE_SIZE/2) / SQUARE_SIZE;
	gz1 = (z1 + SQUARE_SIZE/2) / SQUARE_SIZE;
	gx2 = (x2 + SQUARE_SIZE/2) / SQUARE_SIZE;
	gy2 = (y2 + SQUARE_SIZE/2) / SQUARE_SIZE;
	gz2 = (z2 + SQUARE_SIZE/2) / SQUARE_SIZE;


	if (worldmaps[gx1][gy1][gz1] & WALL
		|| worldmaps[gx1][gy1][gz2] & WALL
		|| worldmaps[gx1][gy2][gz1] & WALL
		|| worldmaps[gx1][gy2][gz2] & WALL
		|| worldmaps[gx2][gy1][gz1] & WALL
		|| worldmaps[gx2][gy1][gz2] & WALL
		|| worldmaps[gx2][gy2][gz1] & WALL
		|| worldmaps[gx2][gy2][gz2] & WALL)
		return TRUE;

	return FALSE;
}







void InitialiseView (void)
{
	ProjectionDistance = 192;			

	GsSetProjection(ProjectionDistance);  

	TheView.vrx = 0; 
	TheView.vry = 0; 
	TheView.vrz = 0; 

	TheView.vpx = 0; 
	TheView.vpy = 0; 
	TheView.vpz = 220;
	
	TheView.rz = 0;

	TheView.super = &TheShip.coord;

	GsSetRefView2(&TheView);
}





void InitialiseLighting (void)
{
		// colour for clearing ordering table: back colour
	BackRed = BackGreen = BackBlue = 0;		// black


		// three flat light sources
	TheLights[0].vx = ONE; TheLights[0].vy = 0; TheLights[0].vz = 0;
	TheLights[0].r = 128; TheLights[0].g = 0; TheLights[0].b = 0;
	GsSetFlatLight(0, &TheLights[0]);

	TheLights[1].vx = 0; TheLights[1].vy = ONE; TheLights[1].vz = 0;
	TheLights[1].r = 0; TheLights[1].g = 128; TheLights[1].b = 0;
	GsSetFlatLight(1, &TheLights[1]);

	TheLights[2].vx = 0; TheLights[2].vy = 0; TheLights[2].vz = ONE;
	TheLights[2].r = 0; TheLights[2].g = 0; TheLights[2].b = 128;
	GsSetFlatLight(2, &TheLights[2]);

		
		// background lighting
	AmbientRed = AmbientGreen = AmbientBlue = ONE/4;
	GsSetAmbient(AmbientRed, AmbientGreen, AmbientBlue);


  
		// distance colour blending
  	TheFogging.dqa = -960;			// two parameters: related to near and far
	TheFogging.dqb = 5120*5120;
	TheFogging.rfc = 0; 		  // the distance colour itself
	TheFogging.gfc = 0; 
	TheFogging.bfc = 0;
	GsSetFogParam( &TheFogging);


		// overall lighting conditions
	OverallLightMode = 1;			 
	GsSetLightMode(OverallLightMode);
}




void SortVideoMode (void)
{
	int currentMode;

	currentMode = GetVideoMode();

	printf("\n\n");
	switch(currentMode)
		{
		case MODE_PAL:
			printf("currently in video mode PAL\n");
			break;
		case MODE_NTSC:
			printf("currently in video mode NTSC\n");
			break;
		default:
			assert(FALSE);
		}

	printf("If everything is black and white\n");
	printf("or there are lines at bottom or top of screen\n");
	printf("Then you need to change video mode\n\n\n");
}


	






void GenerateWorld (void)
{
	if (WorldType == SET_WORLD)
		GenerateSetWorld();
	else if (WorldType == RANDOM_WORLD)
		GenerateRandomWorld();
	else
		assert(FALSE);

	CreateSquaresForWorld();
}







void GenerateRandomWorld (void)
{
	int	x, y, z;
	int	cellContent;
	int wallPercentage = 25;
	int numberWallsSoFar = 0;

		// this chunk below:
		// for each block in world, set to WALL or nothing
	for (x = 0; x < FLOOR_X; x+=3 ) 
		{		
		for (y = 0; y < FLOOR_Y; y+=3)
			{
			for (z = 0; z < FLOOR_Z; z+=3) 
				{
				//cellContent = (rand()%150==0) ? WALL : 0;
					// 2700 = 27 * 100: 100 for percentage, 27 for loop step (3) cubed
				if ( ((rand() % 3) == 0) 
					&& numberWallsSoFar < FLOOR_X*FLOOR_Y*FLOOR_Z*wallPercentage/2700)
						{
						cellContent = WALL;
						numberWallsSoFar++;
						//printf("making a block, centre %d %d %d\n", x+1, y+1, z+1);
						}
				else
					cellContent = 0;

				worldmaps[x][y][z] = cellContent;
				worldmaps[x+1][y][z] = cellContent;
				worldmaps[x+2][y][z] = cellContent;
				worldmaps[x][y+1][z] = cellContent;
				worldmaps[x+1][y+1][z] = cellContent;
				worldmaps[x+2][y+1][z] = cellContent;
				worldmaps[x][y+2][z] = cellContent;
				worldmaps[x+1][y+2][z] = cellContent;
				worldmaps[x+2][y+2][z] = cellContent;
				worldmaps[x][y][z+1] = cellContent;
				worldmaps[x+1][y][z+1] = cellContent;
				worldmaps[x+2][y][z+1] = cellContent;
				worldmaps[x][y+1][z+1] = cellContent;
				worldmaps[x+1][y+1][z+1] = cellContent;
				worldmaps[x+2][y+1][z+1] = cellContent;
				worldmaps[x][y+2][z+1] = cellContent;
				worldmaps[x+1][y+2][z+1] = cellContent;
				worldmaps[x+2][y+2][z+1] = cellContent;
				worldmaps[x][y][z+2] = cellContent;
				worldmaps[x+1][y][z+2] = cellContent;
				worldmaps[x+2][y][z+2] = cellContent;
				worldmaps[x][y+1][z+2] = cellContent;
				worldmaps[x+1][y+1][z+2] = cellContent;
				worldmaps[x+2][y+1][z+2] = cellContent;
				worldmaps[x][y+2][z+2] = cellContent;
				worldmaps[x+1][y+2][z+2] = cellContent;
				worldmaps[x+2][y+2][z+2] = cellContent;
				}
			}
		}


#if 0
		// set WORLD coordinate grid; this used in calc_model_data
		// as array of fixed reference frames
	for (x=0; x < FLOOR_X; x++) 
		{
		for (y=0; y < FLOOR_Y; y++) 
			{
			for (z=0; z < FLOOR_Z; z++) 
				{
				GsInitCoordinate2(WORLD, &World[x][y][z]);
				World[x][y][z].coord.t[0] = x*SQUARE_SIZE;
				World[x][y][z].coord.t[1] = y*SQUARE_SIZE;
				World[x][y][z].coord.t[2] = z*SQUARE_SIZE;
				}
			}
		}
#endif
}







void GenerateSetWorld (void)
{
	int x, y, z;

	if (FLOOR_X != 9 || FLOOR_Y != 9 || FLOOR_Z != 9)	
		assert(FALSE);

	for (x = 0; x < FLOOR_X; x++) 
		{		
		for (y = 0; y < FLOOR_Y; y++)
			{
			for (z = 0; z < FLOOR_Z; z++) 
				{
				if ( ((x >= 1 && x <= 3) || (x >= 5 && x <= 7))
					&& ((y >= 1 && y <= 3) || (y >= 5 && y <= 7))
					&& ((z >= 1 && z <= 3) || (z >= 5 && z <= 7)) ) 
					worldmaps[x][y][z] = WALL;
				else
					worldmaps[x][y][z] = 0;
				}
			}
		}


#if 0
		// set WORLD coordinate grid; this used in calc_model_data
		// as array of fixed reference frames
	for (x = 0; x < FLOOR_X; x++) 
		{
		for (y = 0; y < FLOOR_Y; y++) 
			{
			for (z = 0; z < FLOOR_Z; z++) 
				{
				GsInitCoordinate2(WORLD, &World[x][y][z]);
				World[x][y][z].coord.t[0] = x * SQUARE_SIZE;
				World[x][y][z].coord.t[1] = y * SQUARE_SIZE;
				World[x][y][z].coord.t[2] = z * SQUARE_SIZE;
				}
			}
		}
#endif
}






	// flesh out our description of the world by squares to show solid surfaces
void CreateSquaresForWorld (void)
{
	int x, y, z;
	ObjectHandler* square;
	int squareID = -1;

	CreateTheCube();

	squareID = FindNextFreeObjectSubtypeID(SQUARE);
	for (x = 0; x < FLOOR_X; x++)
		{
		for (y = 0; y < FLOOR_Y; y++)
			{
			for (z = 0; z < FLOOR_Z; z++)
				{
				if (worldmaps[x][y][z] == WALL)
					{
					if ( (x-1 < 0) || (worldmaps[x-1][y][z] != WALL) )	 // left wall
						{
						square = &TheSquares[squareID];
						assert(square != NULL);
						assert(square->alive == FALSE);

						square->alive = TRUE;
						setVECTOR(&square->position, (x * SQUARE_SIZE - SQUARE_SIZE/2), 
								y * SQUARE_SIZE, z * SQUARE_SIZE);
						setVECTOR(&square->rotate, 0, 0, ONE/4);

						UpdateObjectCoordinates2 (&square->rotate, 
							&square->position, &square->coord);

						ScaleMatrix(&square->coord.coord, &scalingVector);
						square->coord.flg = 0;

						setVECTOR(&square->rotate, 0, 0, 0);

						squareID++;
						}
					if ( (x+1 >= FLOOR_X) || (worldmaps[x+1][y][z] != WALL) )	// right wall
						{
						square = &TheSquares[squareID];
						assert(square != NULL);
						assert(square->alive == FALSE);

						square->alive = TRUE;
						setVECTOR(&square->position, (x * SQUARE_SIZE + SQUARE_SIZE/2), 
								y * SQUARE_SIZE, z * SQUARE_SIZE);
						setVECTOR(&square->rotate, 0, 0, 3*ONE/4);

						UpdateObjectCoordinates2 (&square->rotate, 
							&square->position, &square->coord);

						ScaleMatrix(&square->coord.coord, &scalingVector);
						square->coord.flg = 0;

						setVECTOR(&square->rotate, 0, 0, 0);
												
						squareID++;
						}
					if ( (y-1 < 0) || (worldmaps[y-1][y][z] != WALL) )	// ceiling
						{
						square = &TheSquares[squareID];
						assert(square != NULL);
						assert(square->alive == FALSE);

						square->alive = TRUE;
						setVECTOR(&square->position, x * SQUARE_SIZE, 
								(y * SQUARE_SIZE - SQUARE_SIZE/2), z * SQUARE_SIZE);
						setVECTOR(&square->rotate, ONE/2, 0, 0);

						UpdateObjectCoordinates2 (&square->rotate, 
							&square->position, &square->coord);

						ScaleMatrix(&square->coord.coord, &scalingVector);
						square->coord.flg = 0;

						setVECTOR(&square->rotate, 0, 0, 0);
												
						squareID++;
						}
					if ( (y+1 >= FLOOR_Y) || (worldmaps[y+1][y][z] != WALL) )		// floor
						{
						square = &TheSquares[squareID];
						assert(square != NULL);
						assert(square->alive == FALSE);

						square->alive = TRUE;
						setVECTOR(&square->position, x * SQUARE_SIZE, 
								(y * SQUARE_SIZE + SQUARE_SIZE/2), z * SQUARE_SIZE);
						setVECTOR(&square->rotate, 0, 0, 0);

						UpdateObjectCoordinates2 (&square->rotate, 
							&square->position, &square->coord);

						ScaleMatrix(&square->coord.coord, &scalingVector);
						square->coord.flg = 0;

						setVECTOR(&square->rotate, 0, 0, 0);
			
						squareID++;
						}
					if ( (z-1 < 0) || (worldmaps[z-1][y][z] != WALL) )   // back wall
						{
						square = &TheSquares[squareID];
						assert(square != NULL);
						assert(square->alive == FALSE);

						square->alive = TRUE;
						setVECTOR(&square->position, x * SQUARE_SIZE, 
								y * SQUARE_SIZE, (z * SQUARE_SIZE - SQUARE_SIZE/2) );
						setVECTOR(&square->rotate, 3*ONE/4, 0, 0);

						UpdateObjectCoordinates2 (&square->rotate, 
							&square->position, &square->coord);

						ScaleMatrix(&square->coord.coord, &scalingVector);
						square->coord.flg = 0;

						setVECTOR(&square->rotate, 0, 0, 0);
												
						squareID++;
						}
					if ( (z+1 >= FLOOR_Z) || (worldmaps[z+1][y][z] != WALL) )	 // front wall
						{
						square = &TheSquares[squareID];
						assert(square != NULL);
						assert(square->alive == FALSE);

						square->alive = TRUE;
						setVECTOR(&square->position, x * SQUARE_SIZE, 
								y * SQUARE_SIZE, (z * SQUARE_SIZE + SQUARE_SIZE/2) );
						setVECTOR(&square->rotate, ONE/4, 0, 0);

						UpdateObjectCoordinates2 (&square->rotate, 
							&square->position, &square->coord);

						ScaleMatrix(&square->coord.coord, &scalingVector);
						square->coord.flg = 0;

						setVECTOR(&square->rotate, 0, 0, 0);
												
						squareID++;
						}
					}
				}
			}
		}
} 


   

  
	   



	// random positioning
void PositionSomewhereNotInAWall (ObjectHandler* object)
{
	int gridX, gridY, gridZ;
	int xOffset, yOffset, zOffset;
	int success = FALSE;

if (WorldType == SET_WORLD)
	{
	for (;;)
		{			// get random grid-square
		gridX = (rand() % FLOOR_X);
		gridY = (rand() % FLOOR_Y);
		gridZ = (rand() % FLOOR_Z);

		//printf("testing: %d %d %d\n", gridX, gridY, gridZ);

		if (worldmaps[gridX][gridY][gridZ] != WALL)
			{
			success = TRUE;
			break;
			}
		}

	assert(success == TRUE);			// check

		// offset within grid square
	xOffset = 0;
	yOffset = 0;
	zOffset = 0;

	object->position.vx = (gridX * SQUARE_SIZE) + xOffset;
	object->position.vy = (gridY * SQUARE_SIZE) + yOffset;
	object->position.vz = (gridZ * SQUARE_SIZE) + zOffset;
	}
else if (WorldType == RANDOM_WORLD)
	{
	for (;;)
		{			// get random grid-square
		gridX = 1 + (3 * (rand() % (FLOOR_X/3)));
		gridY = 1 + (3 * (rand() % (FLOOR_Y/3)));
		gridZ = 1 + (3 * (rand() % (FLOOR_Z/3)));

		//printf("testing: %d %d %d\n", gridX, gridY, gridZ);

		if (worldmaps[gridX][gridY][gridZ] != WALL
			&& worldmaps[gridX-1][gridY][gridZ] != WALL
			&& worldmaps[gridX+1][gridY][gridZ] != WALL
			&& worldmaps[gridX][gridY+1][gridZ] != WALL
			&& worldmaps[gridX-1][gridY+1][gridZ] != WALL
			&& worldmaps[gridX+1][gridY+1][gridZ] != WALL
			&& worldmaps[gridX][gridY-1][gridZ] != WALL
			&& worldmaps[gridX-1][gridY-1][gridZ] != WALL
			&& worldmaps[gridX+1][gridY-1][gridZ] != WALL

			&& worldmaps[gridX][gridY][gridZ-1] != WALL
			&& worldmaps[gridX-1][gridY][gridZ-1] != WALL
			&& worldmaps[gridX+1][gridY][gridZ-1] != WALL
			&& worldmaps[gridX][gridY+1][gridZ-1] != WALL
			&& worldmaps[gridX-1][gridY+1][gridZ-1] != WALL
			&& worldmaps[gridX+1][gridY+1][gridZ-1] != WALL
			&& worldmaps[gridX][gridY-1][gridZ-1] != WALL
			&& worldmaps[gridX-1][gridY-1][gridZ-1] != WALL
			&& worldmaps[gridX+1][gridY-1][gridZ-1] != WALL

			&& worldmaps[gridX][gridY][gridZ+1] != WALL
			&& worldmaps[gridX-1][gridY][gridZ+1] != WALL
			&& worldmaps[gridX+1][gridY][gridZ+1] != WALL
			&& worldmaps[gridX][gridY+1][gridZ+1] != WALL
			&& worldmaps[gridX-1][gridY+1][gridZ+1] != WALL
			&& worldmaps[gridX+1][gridY+1][gridZ+1] != WALL
			&& worldmaps[gridX][gridY-1][gridZ+1] != WALL
			&& worldmaps[gridX-1][gridY-1][gridZ+1] != WALL
			&& worldmaps[gridX+1][gridY-1][gridZ+1] != WALL)
			{
			success = TRUE;
			break;
			}
		}

	assert(success == TRUE);			// check

		// offset within grid square
	xOffset = 0;
	yOffset = 0;
	zOffset = 0;

	object->position.vx = (gridX * SQUARE_SIZE) + xOffset;
	object->position.vy = (gridY * SQUARE_SIZE) + yOffset;
	object->position.vz = (gridZ * SQUARE_SIZE) + zOffset;
	}
else
	assert(FALSE);
}

	  



	// what actions does ship do?
void HandleEnemyBehaviour (ObjectHandler* object)
{
	object->framesSinceLastFire++;

	switch(object->strategyFlag)
		{
		case WAITING:	 			
			HandleWaitingShipsBehaviour(object);
			break;
		case STATIONARY_TRACK:				
			HandleStationaryTrackingShipsBehaviour(object);
			break;
		case SEEKING:
			HandleSeekingShipBehaviour(object);
			break;
		default:
			assert(FALSE);
		}
}









#define WAITING_PROXIMITY 200

	// waiting ships: do nothing until woken up, become seekers;
	// activated by proximity of player
	// will activate anyway after enough time
void HandleWaitingShipsBehaviour (ObjectHandler* object)
{
	int x, y, z;

		// when player's ship comes close,
		// activate the sleeper -> active pursuer

	x = TheShip.position.vx - object->position.vx;
	y = TheShip.position.vy - object->position.vy;
	z = TheShip.position.vz - object->position.vz;

	if (abs(x) < WAITING_PROXIMITY 
		&& abs(y) < WAITING_PROXIMITY
		&& abs(z) < WAITING_PROXIMITY)
		{
		//printf("activating sleeper\n");
		object->strategyFlag = SEEKING;
		}

	if (object->lifeTime > object->specialTimeLimit)
		{
		//printf("activating sleeper\n");
		object->strategyFlag = SEEKING;
		}
}







#define NEAR_ENOUGH 50

#define VERY_CLOSE 25

#define TRACKING_PROXIMITY 300

	// just turns towards player, and fires when player is close

	// navigation: if player in object's yz plane, just use	nose up/down
	// to point at it; else, tilt on own z axis until it is in object's yz plane
void HandleStationaryTrackingShipsBehaviour (ObjectHandler* object)
{
	VECTOR worldVector, objectVector;
		
		// vector from object to player in world coordinate terms
	worldVector.vx = TheShip.position.vx - object->position.vx;
	worldVector.vy = TheShip.position.vy - object->position.vy;
	worldVector.vz = TheShip.position.vz - object->position.vz;

		// now find the same vector in object coordinate terms
		// basically: the object matrix contains descriptions of the 
		// world's axes in terms of object's own coordinates
	objectVector.vx = -(worldVector.vx * object->matrix.m[0][0]
						+ worldVector.vy * object->matrix.m[1][0]
						+ worldVector.vz * object->matrix.m[2][0]) / ONE;
	objectVector.vy = (worldVector.vx * object->matrix.m[0][1]
						+ worldVector.vy * object->matrix.m[1][1]
						+ worldVector.vz * object->matrix.m[2][1]) / ONE;
	objectVector.vz = (worldVector.vx * object->matrix.m[0][2]
						+ worldVector.vy * object->matrix.m[1][2]
						+ worldVector.vz * object->matrix.m[2][2]) / ONE;

	

	if ( objectVector.vz < 0
		&& ( ((objectVector.vx * objectVector.vx) + (objectVector.vy * objectVector.vy)) 
			< (NEAR_ENOUGH * NEAR_ENOUGH)) )
				{
				if (abs(objectVector.vz) < TRACKING_PROXIMITY * 4)	 // if close up
					{
					//printf("pointing right way, close up enough\n");
					if (object->framesSinceLastFire > object->firingRate)			// rapid fire
						{
						//EnemyShipFiresAShot(object);
						object->framesSinceLastFire = 0;
						}
					}
				else
					{
					//printf("pointing right way, too far away\n");
					}
	   			}
	else		
		{
		if (abs(objectVector.vx) <= VERY_CLOSE)		// player in object's yz plane
			{
			//printf("enemy in yz plane\n");
			if (objectVector.vy <= 0)
				{
				//printf("vy <= 0 ie above me, so vx-- ie nose up\n");
				object->twist.vx -= object->rotationSpeed;
				}
			else
				{
				//printf("vy > 0 ie below me, so vx++ ie nose down\n");
				object->twist.vx += object->rotationSpeed;
				}
			}
		else
			{
			//printf("not in yz plane\n");
			if (objectVector.vy == 0)		// directly to right or left
				{
				//printf("directly to left or right, vy is Zero\n");
				if (objectVector.vx < 0)	   // if on left, turn anticlockwise
					{
					//printf("to left, so vz++\n");
					object->twist.vz += object->rotationSpeed;
					}
				else if (objectVector.vx > 0)  // if on right, turn clockwise
					{
					//printf("to right, so vz--\n");
					object->twist.vz -= object->rotationSpeed;
					}
				else
					{
					//printf("directly behind me, tilting nose up\n");
					//printf("IE: not in yz plane, BUT vx AND vy are zero\n");
					object->twist.vx -= object->rotationSpeed;	// nose up/down
					}
				}
			else
				{
				//printf("tilt by quadrant\n");
				if ( (objectVector.vx > 0 && objectVector.vy > 0)
					|| (objectVector.vx < 0 && objectVector.vy < 0) )
					{		  // turn clockwise
					//printf("down-right or up-left, so clockwise\n");
					object->twist.vz += object->rotationSpeed;
					} 
				else
					{
					//printf("down-left or up-right, so ANTIclockwise\n");
					object->twist.vz -= object->rotationSpeed;
					}
				}
			} 
		}
}






	// will turn towards as stat_track, but move if too far away, pursue
void HandleSeekingShipBehaviour (ObjectHandler* object)
{
	VECTOR worldVector, objectVector;
		
		// vector from object to player in world coordinate terms
	worldVector.vx = TheShip.position.vx - object->position.vx;
	worldVector.vy = TheShip.position.vy - object->position.vy;
	worldVector.vz = TheShip.position.vz - object->position.vz;

		// now find the same vector in object coordinate terms
		// basically: the object matrix contains descriptions of the 
		// world's axes in terms of object's own coordinates
	objectVector.vx = -(worldVector.vx * object->matrix.m[0][0]
						+ worldVector.vy * object->matrix.m[1][0]
						+ worldVector.vz * object->matrix.m[2][0]) / ONE;
	objectVector.vy = (worldVector.vx * object->matrix.m[0][1]
						+ worldVector.vy * object->matrix.m[1][1]
						+ worldVector.vz * object->matrix.m[2][1]) / ONE;
	objectVector.vz = (worldVector.vx * object->matrix.m[0][2]
						+ worldVector.vy * object->matrix.m[1][2]
						+ worldVector.vz * object->matrix.m[2][2]) / ONE;

	

	if ( objectVector.vz < 0
		&& ( ((objectVector.vx * objectVector.vx) + (objectVector.vy * objectVector.vy)) 
			< (NEAR_ENOUGH * NEAR_ENOUGH)) )
				{
				if (abs(objectVector.vz) < TRACKING_PROXIMITY * 4)	 // if close up
					{
					//printf("pointing right way, close up enough\n");
					if (object->framesSinceLastFire > object->firingRate)			
						{
						//EnemyShipFiresAShot(object);
						object->framesSinceLastFire = 0;
						}
					}
				else
					{
					//printf("pointing right way, too far away\n");
					object->velocity.vz -= object->movementSpeed;
					}
	   			}
	else		
		{
		if (abs(objectVector.vx) <= VERY_CLOSE)		// player in object's yz plane
			{
			//printf("enemy in yz plane\n");
			if (objectVector.vy <= 0)
				{
				//printf("vy <= 0 ie above me, so vx-- ie nose up\n");
				object->twist.vx -= object->rotationSpeed;
				}
			else
				{
				//printf("vy > 0 ie below me, so vx++ ie nose down\n");
				object->twist.vx += object->rotationSpeed;
				}
			}
		else
			{
			//printf("not in yz plane\n");
			if (objectVector.vy == 0)		// directly to right or left
				{
				//printf("directly to left or right, vy is Zero\n");
				if (objectVector.vx < 0)	   // if on left, turn anticlockwise
					{
					//printf("to left, so vz++\n");
					object->twist.vz += object->rotationSpeed;
					}
				else if (objectVector.vx > 0)  // if on right, turn clockwise
					{
					//printf("to right, so vz--\n");
					object->twist.vz -= object->rotationSpeed;
					}
				else
					{
					//printf("directly behind me, tilting nose up\n");
					//printf("IE: not in yz plane, BUT vx AND vy are zero\n");
					object->twist.vx -= object->rotationSpeed;	// nose up/down
					}
				}
			else
				{
				//printf("tilt by quadrant\n");
				if ( (objectVector.vx > 0 && objectVector.vy > 0)
					|| (objectVector.vx < 0 && objectVector.vy < 0) )
					{		  // turn clockwise
					//printf("down-right or up-left, so clockwise\n");
					object->twist.vz += object->rotationSpeed;
					} 
				else
					{
					//printf("down-left or up-right, so ANTIclockwise\n");
					object->twist.vz -= object->rotationSpeed;
					}
				}
			} 
		}
}


							
			


	 

	   


	// collisions: enemy ships, player's ship, cubic bullets
void CheckCollisions (void)
{
	register int i;

	for (i = 0; i < MAX_SHIPS; i++)
		{
		if (TheShips[i].alive == TRUE)
			{
			if (ObjectsVeryClose(&TheShips[i], &TheShip))
				{
					// not yet taking action....
				printf("Player and Enemy collision\n");
				}
			}
		}
}


	  



#define OBJECT_SIZE 32

int ObjectsVeryClose (ObjectHandler* first, ObjectHandler* second)
{
	if (abs(first->position.vx - second->position.vx) > OBJECT_SIZE)
		return FALSE;
	if (abs(first->position.vy - second->position.vy) > OBJECT_SIZE)
		return FALSE;
	if (abs(first->position.vz - second->position.vz) > OBJECT_SIZE)
		return FALSE;

	return TRUE; 
}




	// sort out the conversion of coordinates from local->world->screen			 
	// for objects that are displayed by GsSPRITE rather than GsDOBJ2
u_short SortSpriteObjectPosition (ObjectHandler* object)
{
	VECTOR screen;
	int visualX, visualY;
	u_short zValue;

	PushMatrix();

	ApplyMatrixLV( &GsWSMATRIX, &object->position, &screen);

	screen.vx += GsWSMATRIX.t[0];
	screen.vy += GsWSMATRIX.t[1];
	screen.vz += GsWSMATRIX.t[2];

	if (screen.vz == 0)			   // prevent division by zero
		return 0;	   // right at the front

	visualX = object->sprite.w * ProjectionDistance / screen.vz;	
	visualY = object->sprite.h * ProjectionDistance / screen.vz;	

	object->sprite.x = ((screen.vx * ProjectionDistance / screen.vz) - visualX/2);
	object->sprite.y = ((screen.vy * ProjectionDistance / screen.vz) - visualY/2);

	object->sprite.scalex = object->scaleX * ProjectionDistance / screen.vz; 
	object->sprite.scaley = object->scaleY * ProjectionDistance / screen.vz; 

	PopMatrix();

		// 3D clipping
	if (screen.vz < ProjectionDistance/2)
		object->sprite.attribute |= GsDOFF;			 // display off
	else
		TURN_NTH_BIT_OFF(object->sprite.attribute, 32, 31)		// ensure 31st bit is OFF

	zValue = (u_short) (screen.vz >> 6);	 // NOTE: need to adjust this
											// just a blagg to make it look right

	return zValue;			   // NOT YET CORRECT
}

		   




int FindNextFreeObjectSubtypeID (int subType)
{
	int id = -1;
	int i;
	ObjectHandler* whichArray;
	int arrayLength;

	switch(subType)
		{
		case PLAYERS_SHIP:
			assert(FALSE);	  // not meaningful value for this function
			break;
		case ENEMY:
			whichArray = TheShips;
			arrayLength = MAX_SHIPS;
			break;
		case SQUARE:
			whichArray = TheSquares;
			arrayLength = MAX_SQUARES;
			break;
		default:
			assert(FALSE);
		}

	for (i = 0; i < arrayLength; i++)
		{
		if (whichArray[i].alive == FALSE)
			{
			id = i;
			break;
			}
		}

	return id;
}





ObjectHandler* FindNextFreeObjectSubtype (int subType)
{
	int i;
	ObjectHandler* whichArray;
	int arrayLength;
	ObjectHandler* freeObject = NULL;

	switch(subType)
		{
		case PLAYERS_SHIP:
			assert(FALSE);	  // not meaningful value for this function
			break;
		case ENEMY:
			whichArray = TheShips;
			arrayLength = MAX_SHIPS;
			break;
		case SQUARE:
			whichArray = TheSquares;
			arrayLength = MAX_SQUARES;
			break;
		default:
			assert(FALSE);
		}

	for (i = 0; i < arrayLength; i++)
		{
		if (whichArray[i].alive == FALSE)
			{
			freeObject = &whichArray[i];
			break;
			}
		}

	return freeObject;
}







