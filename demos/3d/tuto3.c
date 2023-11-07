/************************************************************
 *															*
 *						tuto3.c								*
 *			   											    *															*
 *				L Evans     05/02/97						*		
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


#define WAVE_TEXTURE_ADDRESS 0x80092000
GsIMAGE WaveTextureInfo;




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


ObjectHandler TheShip; 

#define MAX_SQUARES 256
ObjectHandler TheSquares[MAX_SQUARES];


 	 



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

void HandleAllSquares (void);



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
		FntPrint("frame: %d\n", frameNumber);
		FntPrint("hsync: %d\n", hsync);
		FntPrint("ship pos:\n");
		FntPrint("%d %d %d\n", 
			TheShip.position.vx, TheShip.position.vy, TheShip.position.vz);

		frameNumber++;

		if (DealWithControllerPad() == 0)	   // quitting
			break;

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

		GsClearOt(0,0,&Wot[side]);

		HandleAllObjects();
		
		GsSetRefView2(&TheView); 
	
		numberToDisplay = PerformWorldClipping(ObjectArray, DisplayedObjects);
		FntPrint("num to display: %d\n", numberToDisplay); 

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
					
		hsync = VSync(0);	 // leave this line out for super fast frame rate

		ResetGraph(1);
		GsSwapDispBuff();
		GsSortClear(0,0,4,&Wot[side]);
		GsDrawOt(&Wot[side]);
		side ^= 1;
		FntFlush(-1);
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

		// number used for TURN_NTH_BIT_OFF macro
	for (i = 0; i < 32; i++)
		allOnes |= 1 << i;

	GsInitGraph(SCREEN_WIDTH, SCREEN_HEIGHT, GsINTER|GsOFSGPU, 1, 0);
	GsDefDispBuff(0, 0, 0, SCREEN_HEIGHT);

	GsInit3D();	
	SetVideoMode(MODE_PAL);   
	assert(GetVideoMode() == MODE_PAL);

	Wot[0].length = OT_LENGTH;	
	Wot[0].org = wtags[0];	   
	Wot[1].length = OT_LENGTH;
	Wot[1].org = wtags[1];

	GsClearOt(0, 0, &Wot[0]);
	GsClearOt(0, 0, &Wot[1]);

	ProperInitialiseTexture(WAVE_TEXTURE_ADDRESS, &WaveTextureInfo);

	InitialiseView();		
	InitialiseLighting();		   

	InitialiseObjects();

	CreateTheCube();
		 
		// set up the basic screen-printing font
	FntLoad( 960, 256);			 
	FntOpen( 0, 40, 256, 200, 0, 512);		  
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

