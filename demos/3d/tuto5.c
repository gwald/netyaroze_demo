/************************************************************
 *															*
 *						tuto5.c								*
 *			   											    *															*
 *				L Evans     12/02/97						*		
 *															*
 *	Copyright (C) 1996 Sony Computer Entertainment Inc.		*
 *	All Rights Reserved										*
 *															*
 ***********************************************************/
		
		
		
			 
#include <stdio.h>
#include <rand.h>
#include <libps.h>
#include "pad.h"
#include "tmd.h"
#include "asssert.h"
#include "object.h"




	// NOTE: the square tiles of ten.tmd are almost EXACTLY 128 by 128
	// if get a fully 2d tiling texture, it will look perfect
   


#define SCREEN_WIDTH 320			 
#define	SCREEN_HEIGHT 240




#define PACKETMAX (2048*24)   /* Max GPU packets */
static PACKET packetArea[2][PACKETMAX]; /* GPU PACKETS AREA */




#define SQUARE_MODEL_ADDRESS 0x80090000
#define SHIP_MODEL_ADDRESS 0x80091000
#define CUBE_MODEL_ADDRESS 0x80093000


#define WAVE_TEXTURE_ADDRESS 0x80092000
GsIMAGE WaveTextureInfo;

#define ASCII_TEXTURE_ADDRESS 0x80094000
GsIMAGE AsciiTextureInfo;




#define OT_LENGTH	9		


static GsOT Wot[2];			/* Handler of OT */
static GsOT_TAG wtags[2][1<<OT_LENGTH]; 



static GsRVIEW2 TheView;


int ProjectionDistance;	




GsF_LIGHT TheLights[3];




GsFOGPARAM Fogging;

int LightMode;



int frameNumber = 0;




	// object types
#define SHIP 0
#define SQUARE 1
#define CUBE 2


ObjectHandler TheShip; 

#define MAX_SQUARES 256
ObjectHandler TheSquares[MAX_SQUARES];


#define MAX_CUBES 1
ObjectHandler TheCubes[MAX_CUBES];


 	 



	// the floor
#define SQUARE_SIZE 128

#define FLOOR_X 4
#define FLOOR_Y 4
#define FLOOR_Z 4


#define WORLD_X (FLOOR_X * SQUARE_SIZE)
#define WORLD_Y (FLOOR_Y * SQUARE_SIZE)
#define WORLD_Z (FLOOR_Z * SQUARE_SIZE)





	// limits on player movement
#define SHIP_MIN_X SQUARE_SIZE/2
#define SHIP_MIN_Y SQUARE_SIZE/2
#define SHIP_MIN_Z SQUARE_SIZE/2
#define SHIP_MAX_X (FLOOR_X * SQUARE_SIZE) - SQUARE_SIZE/2
#define SHIP_MAX_Y (FLOOR_Y * SQUARE_SIZE) - SQUARE_SIZE/2
#define SHIP_MAX_Z (FLOOR_Z * SQUARE_SIZE) - SQUARE_SIZE/2



	// graphical world clipping

ObjectHandler* DisplayedObjects[MAX_OBJECTS];




		// graphical text handling (textures, not FntPrint)
	// (not yet bothered with scaling and colouring, but it's easy to add)
GsSPRITE LetterSprites[26];
GsSPRITE NumberSprites[10];

#define MAX_TEXT_STRINGS 50
#define MAX_STRING_LENGTH 50

typedef struct
{
	int x, y;
	int scaler;
	int zValue;
	int length;
	char data[MAX_STRING_LENGTH];
} TextString;

TextString AllStrings[MAX_TEXT_STRINGS];
int NumberOfStrings;




/****************************************************************************
					prototypes
****************************************************************************/


void main (void);	   


int PerformWorldClipping (ObjectHandler** firstArray, ObjectHandler** secondArray);
void FindViewPoint (VECTOR* output);


int DealWithControllerPad(void);


void InitialiseAll(void);
void InitialiseObjects (void);
void CreateTheCube (void);
		  
		  
		  
void HandleAllObjects (void);


void HandleTheShip (ObjectHandler* object);
void HandleASquare (ObjectHandler* object);
void HandleACube (ObjectHandler* object);

void HandleAllSquares (void);

void MarkAllCubes (void);



void UpdateObjectCoordinates (SVECTOR* rotationVector,
				VECTOR* translationVector,
				GsCOORDINATE2* coordSystem);

void UpdateObjectCoordinates2 (VECTOR* twist,
				VECTOR* position, VECTOR* velocity,
				GsCOORDINATE2* coordSystem, MATRIX* matrix);
							
							
								
void InitialiseView(void);
void InitialiseLighting(void);	

		
long* InitialiseModel(long* adrs);



void ProperInitialiseTexture (long address, GsIMAGE* imageInfo);	
void LinkSpriteToImageInfo (GsSPRITE* sprite, GsIMAGE* imageInfo);

u_short SortSpriteObjectPosition (ObjectHandler* object);
void FindObjectScreenPosition (ObjectHandler* object, VECTOR* output,
				int* twoDScaling);




void InitialiseTextStrings (void);
void DisplayTextStrings (GsOT* orderingTable);
GsSPRITE* GetSpriteForCharacter (char character);
void RegisterTextStringForDisplay (char* string, int x, int y, int scale, int zValue);







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




int allOnes;

#define TURN_NTH_BIT_OFF(argument, sizeInBits, N)						\
	{																	\
	u_long onlyNthBitOn, allButNthBitOn;								\
																		\
	onlyNthBitOn = 1 << (N);											\
	allButNthBitOn = allOnes ^ onlyNthBitOn;							\
	(argument) &= allButNthBitOn;										\
	}




#define setVECTOR(vector, x, y, z)				\
				(vector)->vx = (x), (vector)->vy = (y), (vector)->vz = (z)



		  	  

/****************************************************************************
					 functions
****************************************************************************/



void main (void)
{
	int	i;
	int side;			  // buffer index
	MATRIX	tmpls, tmplw;
	int hsync = 0;
	ObjectHandler* object;
	u_short zValue;
	int numberToDisplay;

	InitialiseAll();
	
	side = GsGetActiveBuff();
	for	(;;)
		{
		//printf("MAIN: 1\n");
		FntPrint("frame: %d\n", frameNumber);
		FntPrint("hsync: %d\n", hsync);
		FntPrint("ship pos:\n");
		FntPrint("%d %d %d\n", 
			TheShip.position.vx, TheShip.position.vy, TheShip.position.vz);

		frameNumber++;

		if (DealWithControllerPad() == 0)	   // quitting
			break;

		//printf("MAIN: 2\n");

		if (frameNumber % 20 == 0)
			{
			// matrices: when updated in this way,
			// ship matrix VERY explicit:
			// rows show object's own axes expressed in world coordinate system
			// columns show world axes expressed in object's own coordinate system
			// given these, object-relative movements and 3D navigation are nice and easy
			//printf("ship matrix\n");
			//dumpMATRIX( &TheShip.coord.coord);
			}
								
		GsSetWorkBase((PACKET*)packetArea[side]);
		//printf("MAIN: 3\n");

		GsClearOt(0,0,&Wot[side]);

		HandleAllObjects();
		//printf("MAIN: 4\n");
		
		GsSetRefView2(&TheView); 
		//printf("MAIN: 5\n");
	
		numberToDisplay = PerformWorldClipping(ObjectArray, DisplayedObjects);
		FntPrint("num to display: %d\n", numberToDisplay); 

		FntPrint("use L1 and right pad\nto move the cubes\n");
		//printf("MAIN: 6\n");

		MarkAllCubes();

		//printf("MAIN: 7\n");

		for (i = 0; i < numberToDisplay; i++) 
			{		
			object = DisplayedObjects[i];

			if (object->displayFlag == TMD)
				{
				GsGetLs(&(object->coord), &tmpls);
				   
				GsSetLightMatrix(&tmpls);
					
				GsSetLsMatrix(&tmpls);
					
				GsSortObject4 (&(object->handler), 
						&Wot[side], 
							3, getScratchAddr(0));
				}
			else
				{
				zValue = SortSpriteObjectPosition (object);
				GsSortSprite ( &object->sprite, &Wot[side], zValue);
				}	 
			}
		//printf("MAIN: 8\n");

		DisplayTextStrings( &Wot[side]);
		//printf("MAIN: 9\n");
					
		hsync = VSync(0);	 // leave this line out for super fast frame rate

		ResetGraph(1);
		GsSwapDispBuff();
		GsSortClear(0,0,4,&Wot[side]);
		GsDrawOt(&Wot[side]);
		side ^= 1;
		FntFlush(-1);
		//printf("MAIN: 10\n");
		}

		// program cleanup
	ResetGraph(3);
}

	 


	// look at objects in first array,
	// if they're close to view point,
	// put them into second array

	// AT PRESENT, no clipping (all objects included)

int PerformWorldClipping (ObjectHandler** firstArray, ObjectHandler** secondArray)
{
	int numberDisplayedObjects = 0;
	int i;
	VECTOR *position;
	VECTOR viewPoint;
	int SimpleClipDistance;

		// this is set arbitrarily
	SimpleClipDistance = SQUARE_SIZE * 6;

	FindViewPoint( &viewPoint);
	
	for (i = 0; i < MAX_OBJECTS; i++)
		{
		if (firstArray[i] != NULL)
			if (firstArray[i]->alive == TRUE)
				{
				position = &(firstArray[i]->position);
				
				if (abs(position->vx - viewPoint.vx) < SimpleClipDistance
					&& abs(position->vy - viewPoint.vy) < SimpleClipDistance
					&& abs(position->vz - viewPoint.vz) < SimpleClipDistance)
					{
					secondArray[numberDisplayedObjects] = firstArray[i];
					numberDisplayedObjects++;
					}
				}
		}
		
	return numberDisplayedObjects;	
}			





	// This ASSUMES that only view.vpz != 0 .....
	// not hard to change ... 
void FindViewPoint (VECTOR* output)
{
	VECTOR viewOffset;

	setVECTOR( &viewOffset, (TheShip.coord.coord.m[2][0] * TheView.vpz) >> 12,
							(TheShip.coord.coord.m[2][1] * TheView.vpz) >> 12,
							(TheShip.coord.coord.m[2][2] * TheView.vpz) >> 12 );

	setVECTOR( output, TheShip.position.vx + viewOffset.vx, 
						TheShip.position.vy + viewOffset.vy, 
						TheShip.position.vz + viewOffset.vz);
}





int DealWithControllerPad (void)
{
	long pad;
	int speedMultiplier;
	int i;
  
	pad = PadRead();

		// quit program
	if (pad & PADstart && pad & PADselect)
		return 0;

		// pause
	if (pad & PADstart)
		{
		while (pad & PADstart)
			{
			pad = PadRead();
			}
		}
		
		// how fast do the controls act
	if (pad & PADselect)
		speedMultiplier = 5;
	else
		speedMultiplier = 1;	


	if (pad & PADL1)
		{
		if (pad & PADRright)
			{
			for (i = 0; i < MAX_CUBES; i++)
				TheCubes[i].position.vx += 20 * speedMultiplier;
			return 1;
			}
		if (pad & PADRleft)
			{
			for (i = 0; i < MAX_CUBES; i++)
				TheCubes[i].position.vx -= 20 * speedMultiplier;
			return 1;
			}
		if (pad & PADRup)
			{
			for (i = 0; i < MAX_CUBES; i++)
				TheCubes[i].position.vy -= 20 * speedMultiplier;
			return 1;
			}
		if (pad & PADRdown)
			{
			for (i = 0; i < MAX_CUBES; i++)
				TheCubes[i].position.vy += 20 * speedMultiplier;
			return 1;
			}
		if (pad & PADR1)
			{
			for (i = 0; i < MAX_CUBES; i++)
				TheCubes[i].position.vz += 20 * speedMultiplier;
			return 1;
			}
		if (pad & PADR2)
			{
			for (i = 0; i < MAX_CUBES; i++)
				TheCubes[i].position.vz -= 20 * speedMultiplier;
			return 1;
			}
		}


		// moving the ship: forward and back
		// ie object relative movement
		// ie movement only along object's own z axis
	if (pad & PADLup)
		TheShip.velocity.vz -= TheShip.movementSpeed * speedMultiplier;
	if (pad & PADLdown)
		TheShip.velocity.vz += TheShip.movementSpeed * speedMultiplier;
				
  		// rotating the ship
	if (pad & PADR1)
		{
		TheShip.twist.vz += TheShip.rotationSpeed * speedMultiplier;
		return 1;
		}
	if (pad & PADR2)
		{
		TheShip.twist.vz -= TheShip.rotationSpeed * speedMultiplier;
		return 1;
		}
	if (pad & PADRup)
		{
		TheShip.twist.vx += TheShip.rotationSpeed * speedMultiplier;
		return 1;
		}
	if (pad & PADRdown)
		{
		TheShip.twist.vx -= TheShip.rotationSpeed * speedMultiplier;
		return 1;
		}
	if (pad & PADRleft)
		{
		TheShip.twist.vy -= TheShip.rotationSpeed * speedMultiplier;
		return 1;
		}
	if (pad & PADRright)
		{
		TheShip.twist.vy += TheShip.rotationSpeed * speedMultiplier;
		return 1;
		}

	return 1;
}





void InitialiseAll (void)
{	
	int i;

	PadInit();			// init controller pad
	//printf("InitialiseAll: 1\n");

		// number used for TURN_NTH_BIT_OFF macro
	for (i = 0; i < 32; i++)
		allOnes |= 1 << i;
	//printf("InitialiseAll: 2\n");

	GsInitGraph(SCREEN_WIDTH, SCREEN_HEIGHT, GsINTER|GsOFSGPU, 1, 0);
	GsDefDispBuff(0, 0, 0, SCREEN_HEIGHT);
	//printf("InitialiseAll: 3\n");

	GsInit3D();	
	SetVideoMode(MODE_NTSC);   
	assert(GetVideoMode() == MODE_NTSC);	
	//printf("InitialiseAll: 4\n");

	Wot[0].length = OT_LENGTH;	
	Wot[0].org = wtags[0];	   
	Wot[1].length = OT_LENGTH;
	Wot[1].org = wtags[1];

	GsClearOt(0, 0, &Wot[0]);
	GsClearOt(0, 0, &Wot[1]);
	//printf("InitialiseAll: 5\n");

	ProperInitialiseTexture(WAVE_TEXTURE_ADDRESS, &WaveTextureInfo);
	ProperInitialiseTexture(ASCII_TEXTURE_ADDRESS, &AsciiTextureInfo);

	InitialiseTextStrings();
	//printf("InitialiseAll: 6\n");

	InitialiseView();		
	InitialiseLighting();		   

	InitialiseObjects();
	//printf("InitialiseAll: 7\n");

	CreateTheCube();
		 
		// set up the basic screen-printing font
	FntLoad( 960, 256);			 
	FntOpen( 0, 40, 256, 200, 0, 512);	
	//printf("InitialiseAll: 8\n");	  
}







void InitialiseObjects (void)
{
	int i;

	InitialiseObjectClass();




   

	InitSingleObject(&TheShip);

	BringObjectToLife ( &TheShip, SHIP, 
					SHIP_MODEL_ADDRESS, 0, NONE);

	TheShip.rotationSpeed = ONE/128;
	TheShip.movementSpeed = 1;
	TheShip.movementMomentumFlag = TRUE;


		// flip ship upside down for right-looking orientation
	TheShip.rotate.vx = ONE/2;
	setVECTOR( &TheShip.position, WORLD_X/2, WORLD_Y/2, WORLD_Z/2);
	UpdateObjectCoordinates (&TheShip.rotate, 
		&TheShip.position, &TheShip.coord);
	setVECTOR( &TheShip.rotate, 0, 0, 0);
	

	RegisterObjectIntoObjectArray( &TheShip);



	for (i = 0; i < MAX_SQUARES; i++)
		{
		InitSingleObject(&TheSquares[i]);

		BringObjectToLife(&TheSquares[i], SQUARE, 
			SQUARE_MODEL_ADDRESS, 0, NONE);

		RegisterObjectIntoObjectArray(&TheSquares[i]);
		}


	for (i = 0; i < MAX_CUBES; i++)
		{
		InitSingleObject(&TheCubes[i]);

		BringObjectToLife(&TheCubes[i], CUBE, 
			CUBE_MODEL_ADDRESS, 0, NONE);

		setVECTOR( &TheCubes[i].position, (FLOOR_X * SQUARE_SIZE/2) + (i*SQUARE_SIZE/2),
			(FLOOR_Y * SQUARE_SIZE/2) + (i*SQUARE_SIZE/2),
			(FLOOR_Z * SQUARE_SIZE/2) + (i*SQUARE_SIZE/2) );

		SetObjectScaling( &TheCubes[i], ONE/4, ONE/4, ONE/4);

		RegisterObjectIntoObjectArray(&TheCubes[i]);
		}

   

		


	LinkAllObjectsToModelsOrSprites();
	LinkAllObjectsToTheirCoordinateSystems();


	for (i = 0; i < MAX_SQUARES; i++)
		{
		TheSquares[i].alive = FALSE;
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
	for (x = 0; x < FLOOR_X; x++)	
		{
		for (z = 0; z < FLOOR_Z; z++)
			{			 
			xPos = SQUARE_SIZE/2 + x * SQUARE_SIZE;
			zPos = SQUARE_SIZE/2 + z * SQUARE_SIZE;

			square = &TheSquares[squareID];

			square->alive = TRUE;

			setVECTOR( &square->position, xPos, yPos, zPos);

			setVECTOR( &square->rotate, ONE/2, 0, 0);

			UpdateObjectCoordinates (&square->rotate, 
				&square->position, &square->coord);

			setVECTOR( &square->rotate, 0, 0, 0); 

			squareID++;
			}
		}

	for (x = 0; x < FLOOR_X; x++)	
		{
		for (z = 0; z < FLOOR_Z; z++)
			{			 
			xPos = SQUARE_SIZE/2 + x * SQUARE_SIZE;
			zPos = SQUARE_SIZE/2 + z * SQUARE_SIZE;

			square = &TheSquares[squareID];

			square->alive = TRUE;

			setVECTOR( &square->position, xPos, 0, zPos);

			UpdateObjectCoordinates (&square->rotate, 
				&square->position, &square->coord);

			squareID++;
			}
		}




	xPos = FLOOR_X * SQUARE_SIZE;
	for (y = 0; y < FLOOR_Y; y++)	
		{
		for (z = 0; z < FLOOR_Z; z++)
			{			 
			yPos = SQUARE_SIZE/2 + y * SQUARE_SIZE;
			zPos = SQUARE_SIZE/2 + z * SQUARE_SIZE;

			square = &TheSquares[squareID];

			square->alive = TRUE;

			setVECTOR( &square->position, xPos, yPos, zPos);

			setVECTOR( &square->rotate, 0, 0, ONE/4);

			UpdateObjectCoordinates (&square->rotate, 
				&square->position, &square->coord);

			setVECTOR( &square->rotate, 0, 0, 0);

			squareID++;
			}
		}

	for (y = 0; y < FLOOR_Y; y++)	
		{
		for (z = 0; z < FLOOR_Z; z++)
			{			 
			yPos = SQUARE_SIZE/2 + y * SQUARE_SIZE;
			zPos = SQUARE_SIZE/2 + z * SQUARE_SIZE;

			square = &TheSquares[squareID];

			square->alive = TRUE;

			setVECTOR( &square->position, 0, yPos, zPos);

			setVECTOR( &square->rotate, 0, 0, 3*ONE/4);

			UpdateObjectCoordinates (&square->rotate, 
				&square->position, &square->coord);
					
			setVECTOR( &square->rotate, 0, 0, 0);

			squareID++;
			}
		}



	zPos = FLOOR_Z * SQUARE_SIZE;
	for (y = 0; y < FLOOR_Y; y++)	
		{
		for (x = 0; x < FLOOR_X; x++)
			{			 
			yPos = SQUARE_SIZE/2 + y * SQUARE_SIZE;
			xPos = SQUARE_SIZE/2 + x * SQUARE_SIZE;

			square = &TheSquares[squareID];

			square->alive = TRUE;

			setVECTOR( &square->position, xPos, yPos, zPos);

			setVECTOR( &square->rotate, 3*ONE/4, 0, 0);

			UpdateObjectCoordinates (&square->rotate, 
				&square->position, &square->coord);

			setVECTOR( &square->rotate, 0, 0, 0);

			squareID++;
			}
		}

	for (y = 0; y < FLOOR_Y; y++)	
		{
		for (x = 0; x < FLOOR_X; x++)
			{			 
			yPos = SQUARE_SIZE/2 + y * SQUARE_SIZE;
			xPos = SQUARE_SIZE/2 + x * SQUARE_SIZE;

			square = &TheSquares[squareID];

			square->alive = TRUE;

			setVECTOR( &square->position, xPos, yPos, 0);

			setVECTOR( &square->rotate, ONE/4, 0, 0);

			UpdateObjectCoordinates (&square->rotate, 
				&square->position, &square->coord);
					
			setVECTOR( &square->rotate, 0, 0, 0);

			squareID++;
			}
		}
}

			



void HandleAllObjects (void)
{
	int i;
	ObjectHandler* object;

	for (i = 0; i < MAX_OBJECTS; i++)
		{
		if (ObjectArray[i] != NULL)
			{
			if (ObjectArray[i]->alive == TRUE)
				{
				object = ObjectArray[i];

					// updating common to all object types
				object->lifeTime++;

				switch(object->type)
					{
					case SHIP:
						HandleTheShip(object);
						break;
					case SQUARE:	 
						HandleASquare(object);
						break; 
					case CUBE:
						HandleACube(object);
						break; 
					default:	
						assert(FALSE);
					}
				}
			}
		}

	HandleAllSquares();
}



	  


void HandleTheShip (ObjectHandler* object)
{
	VECTOR previousPosition;
	
	assert(object->type == SHIP);

	setVECTOR( &previousPosition, object->position.vx,
				object->position.vy, object->position.vz);

	UpdateObjectCoordinates2 (&object->twist,
		&object->position, &object->velocity,
		&object->coord, &object->matrix);

	if (object->movementMomentumFlag == FALSE)
		{
		setVECTOR( &object->velocity, 0, 0, 0);
		}

	if (object->rotationMomentumFlag == FALSE)
		{
		setVECTOR( &object->twist, 0, 0, 0);
		}

	if (object->position.vx < SHIP_MIN_X || object->position.vx > SHIP_MAX_X
		|| object->position.vy < SHIP_MIN_Y || object->position.vy > SHIP_MAX_Y
		|| object->position.vz < SHIP_MIN_Z || object->position.vz > SHIP_MAX_Z)
			{
			setVECTOR( &object->position, previousPosition.vx,
				previousPosition.vy, previousPosition.vz);
			setVECTOR( &object->velocity, 0, 0, 0); 
			}
}




		
	// because the squares do not move we don't need to update
	// the coordinate system; 
	// this whole function can (at present) be dropped
void HandleASquare (ObjectHandler* object)
{
	assert(object->type == SQUARE);
}






void HandleACube (ObjectHandler* object)
{
	assert(object->type == CUBE);

	UpdateObjectCoordinates (&object->rotate,
							&object->position,
							&object->coord);

	SortObjectSize(object);
}





void HandleAllSquares (void)
{
	int i;
	int VeryCloseDistance = SQUARE_SIZE * 2;
	VECTOR* position;
	VECTOR viewPoint;
	GsDOBJ2* handler;

	FindViewPoint( &viewPoint);

	for (i = 0; i < MAX_SQUARES; i++)
		{
		if (TheSquares[i].alive == TRUE)
			{
			position = &TheSquares[i].position;
			handler = &TheSquares[i].handler;

			if (abs(position->vx - viewPoint.vx) < VeryCloseDistance
				&& abs(position->vy - viewPoint.vy) < VeryCloseDistance
				&& abs(position->vz - viewPoint.vz) < VeryCloseDistance)
				{
				handler->attribute |= GsDIV1;
				}
			else
				{
				TURN_NTH_BIT_OFF(handler->attribute, 32, 31)
				}
			}
		}
}





void MarkAllCubes (void)
{
	int i;
	int twoDScaling;
	VECTOR screenPosition;

	for (i = 0; i < MAX_CUBES; i++)
		{
		FindObjectScreenPosition (&TheCubes[i], &screenPosition,
				&twoDScaling);

		if (twoDScaling > 0)
			{
				// -100 to make it more likely to be sorted
			RegisterTextStringForDisplay ("X", screenPosition.vx, 
						screenPosition.vy, twoDScaling, (screenPosition.vz/64)-100);
			if (i == 0)
				{
				printf("screen pos: \n");
				dumpVECTOR( &screenPosition);
				printf("twoDScaling: %d\n", twoDScaling);
				printf("cube itself at\n");
				dumpVECTOR( &TheCubes[0].position);
				}
			}
		else
			printf("twoDScaling <= 0\n");
		}
}



	// this function performs world-relative movement and rotation
void UpdateObjectCoordinates (SVECTOR* rotationVector,
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









	// this function does object-relative movement and rotation
void UpdateObjectCoordinates2 (VECTOR* twist,
							VECTOR* position, VECTOR* velocity,
							GsCOORDINATE2* coordSystem, MATRIX* matrix)
{	
	VECTOR realMovement;
	MATRIX xMatrix, yMatrix, zMatrix;
	SVECTOR xVector, yVector, zVector;

		// find the object-local velocity in super coordinate terms
	ApplyMatrixLV(matrix, velocity, &realMovement);

		// update position by actual movement
	position->vx += realMovement.vx;
	position->vy += realMovement.vy;
	position->vz += realMovement.vz;

	xVector.vx = twist->vx;
	xVector.vy = 0;
	xVector.vz = 0;
	yVector.vx = 0;
	yVector.vy = twist->vy;
	yVector.vz = 0;
	zVector.vx = 0;
	zVector.vy = 0;
	zVector.vz = twist->vz;

	RotMatrix(&xVector, &xMatrix);
	RotMatrix(&yVector, &yMatrix);
	RotMatrix(&zVector, &zMatrix);

		// to get world-relative rotations instead:
		// use same function with order of first two arguments switched

	MulMatrix0(matrix, &xMatrix, matrix);
	MulMatrix0(matrix, &yMatrix, matrix);
	MulMatrix0(matrix, &zMatrix, matrix);

	coordSystem->coord = *matrix;
	
		// set position absolutely	
	coordSystem->coord.t[0] = position->vx;
	coordSystem->coord.t[1] = position->vy;
	coordSystem->coord.t[2] = position->vz;

		// tell GTE that coordinate system has been updated
	coordSystem->flg = 0;
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
	TheView.vpz = 192;
	TheView.rz = 0;

	TheView.super = &TheShip.coord;

	GsSetRefView2(&TheView);
}






void InitialiseLighting (void)
{
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

 		
		// ambient lighting
	GsSetAmbient(ONE/4, ONE/4, ONE/4);		


		// fogging: not used yet
	Fogging.dqa = 0;
	Fogging.dqb = 0;
	Fogging.rfc = 0;
	Fogging.gfc = 0;
	Fogging.bfc = 0;
	GsSetFogParam( &Fogging);


		// overall light mode
	LightMode = 0;
	GsSetLightMode(LightMode);
}
 




long* InitialiseModel (long* adrs)
{
	long *dop;

	dop = adrs;
	dop++;
	GsMapModelingData(dop);
	dop++;
	dop++;
	return dop;
}





	// the second argument is a global declared next to the #define for
	// the TIM file main memory address;
	// it allows endless sprites to be easily linked to it later on
void ProperInitialiseTexture (long address, GsIMAGE* imageInfo)
{
	RECT rect;

		// +4 because we need to skip head of TIM file
	GsGetTimInfo( (u_long *)(address+4), imageInfo);

	rect.x = imageInfo->px;	
	rect.y = imageInfo->py;		
	rect.w = imageInfo->pw;	
	rect.h = imageInfo->ph;	

	LoadImage(	&rect, imageInfo->pixel);

		// if image needs a CLUT, load it up 
		// (pmode: 8 or 9 ==> texture is 4-bit or 8-bit)
	if ( (imageInfo->pmode>>3)&0x01) 
		{
		rect.x = imageInfo->cx;	
		rect.y = imageInfo->cy;	
		rect.w = imageInfo->cw;	
		rect.h = imageInfo->ch;	

		LoadImage( &rect, imageInfo->clut);
		}

		// wait for loading to finish
	DrawSync(0);
}




  
	 

#define SIXTEEN_BIT_MASK (1 << 25)
#define EIGHT_BIT_MASK (1 << 24)

void LinkSpriteToImageInfo (GsSPRITE* sprite, GsIMAGE* imageInfo)
{
	int widthCompression;	  // on frame buffer
	int tPageType;
	int texturePageX, texturePageY, xOffset, yOffset;

	InitGsSprite(sprite);
		
	FindTopLeftOfTexturePage(imageInfo, 
		&texturePageX, &texturePageY, &xOffset, &yOffset);

	switch(imageInfo->pmode)
		{
		case 0:		   // 16-bit
			sprite->attribute |= SIXTEEN_BIT_MASK;		
			widthCompression = 1;
			tPageType = 2;	
			break;
		case 8:		   	 // 4-bit
			widthCompression = 4;
			tPageType = 0;
			sprite->cx = imageInfo->cx;
			sprite->cy = imageInfo->cy;	
			break;
		case 9:		   // 8-bit
			sprite->attribute |= EIGHT_BIT_MASK;		
			widthCompression = 2;
			tPageType = 1;
			sprite->cx = imageInfo->cx;
			sprite->cy = imageInfo->cy;		
			break;
		default:
			printf("Only 4, 8 and 16 bit modes supported\n");
			assert(FALSE);		// other modes not supported
		}

	sprite->tpage 
			= GetTPage(tPageType, 0, texturePageX, texturePageY);

	sprite->w = imageInfo->pw * widthCompression;
	sprite->h = imageInfo->ph;

		// set centre of sprite as point for rotation, scaling, etc
	sprite->mx = sprite->w / 2;
	sprite->my = sprite->h / 2;

	sprite->u = xOffset;
	sprite->v = yOffset;
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





void FindObjectScreenPosition (ObjectHandler* object, VECTOR* output,
				int* twoDScaling)
{
	VECTOR screen;
	int visualX, visualY;

	PushMatrix();

	ApplyMatrixLV( &GsWSMATRIX, &object->position, &screen);

	screen.vx += GsWSMATRIX.t[0];
	screen.vy += GsWSMATRIX.t[1];
	screen.vz += GsWSMATRIX.t[2];

		// assign: screen xyz
	setVECTOR( output, screen.vx, screen.vy, screen.vz);
	
	if (screen.vz > 0)
			// scaling for x and y: distance and visual size
		*twoDScaling = ONE * ProjectionDistance / screen.vz; 
	else
		*twoDScaling = -1;		// indicating no value

	PopMatrix();
}







void InitialiseTextStrings (void)
{
	int i;
	
	for (i = 0; i < 26; i++)
		{
		LinkSpriteToImageInfo( &LetterSprites[i], &AsciiTextureInfo);
		LetterSprites[i].w = LetterSprites[i].h = 8;
		if (i < 15)
			{
			LetterSprites[i].u += 8 + (i * 8);
			LetterSprites[i].v += 16;
			}
		else
			{
			LetterSprites[i].u += ((i-15) * 8);
			LetterSprites[i].v += 24;
			}
		}
	for (i = 0; i < 10; i++)
		{
		LinkSpriteToImageInfo( &NumberSprites[i], &AsciiTextureInfo);
		NumberSprites[i].w = NumberSprites[i].h = 8;
		NumberSprites[i].u += (i * 8);
		NumberSprites[i].v += 8;
		}

	for (i = 0; i < MAX_TEXT_STRINGS; i++)
		{
		AllStrings[i].x = 0;
		AllStrings[i].y = 0;
		AllStrings[i].scaler = ONE;
		AllStrings[i].zValue = 0;
		AllStrings[i].length = 0;
		AllStrings[i].data[0] = '\n';
		}

	NumberOfStrings = 0;
}





void DisplayTextStrings (GsOT* orderingTable)
{
	int i, j;
	TextString* thisOne;
	GsSPRITE* sprite;
	int x, y;
	int zPosition;
	int scale;

	for (i = 0; i < MAX_TEXT_STRINGS; i++)
		{
		if (AllStrings[i].length != 0)
			{
			thisOne = &AllStrings[i];
			x = thisOne->x;
			y = thisOne->y;
			zPosition = (thisOne->zValue >= 0 ? thisOne->zValue : 0);
			scale = thisOne->scaler;

			if (i == 0)
				{
				printf("zValue: %d\n", thisOne->zValue);
				printf("scale: %d\n", thisOne->scaler);
				printf("x %d y %d\n", thisOne->x, thisOne->y);
				}

			for (j = 0; j < thisOne->length; j++)
				{
				sprite = GetSpriteForCharacter(thisOne->data[j]);

				if (sprite != NULL)
					{
					sprite->x = x;
					sprite->y = y;

					sprite->scalex = scale;
					sprite->scaley = scale;

					GsSortSprite(sprite, orderingTable, zPosition); 
					}
				x += 8;
				}

				// mark string as EMPTY
			thisOne->length = 0;
			}
		}

	NumberOfStrings = 0;
}






GsSPRITE* GetSpriteForCharacter (char character)
{
	GsSPRITE* result;
	int index;

	if ( ((character >= 48 && character <= 57)
		|| (character >= 65 && character <= 90) 
		|| (character >= 97 && character <= 122)) == FALSE)
			return NULL;

		// make all one case
	if (character >= 65 && character <= 90)
		character += 32;

	if (character >= 97 && character <= 122)
		{
		index = ((int) character) - 97;
		result = &LetterSprites[index];
		}
	else
		{
		index = ((int) character) - 48;
		result = &NumberSprites[index];
		}

	return result;
}





void RegisterTextStringForDisplay (char* string, int x, int y, int scale, int zValue)
{
	int i, id = -1;
	TextString* thisOne;

	for (i = 0; i < MAX_TEXT_STRINGS; i++)
		{
		if (AllStrings[i].length == 0)
			{
			id = i;
			break;
			}
		}

	assert(id != -1);
	thisOne = &AllStrings[id];

	thisOne->x = x;
	thisOne->y = y;
	thisOne->scaler = scale;
	thisOne->zValue = zValue;
	thisOne->length = strlen(string);
	assert(thisOne->length > 0);
	assert(thisOne->length < MAX_STRING_LENGTH);

	strcpy( thisOne->data, string);

	NumberOfStrings++;
	assert(NumberOfStrings < MAX_TEXT_STRINGS);
}


