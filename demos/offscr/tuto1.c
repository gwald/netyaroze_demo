/************************************************************
 *															*
 *						tuto1.c								*
 *			   											    *															*
 *				L Evans     07/02/97						*		
 *															*
 *	Copyright (C) 1996 Sony Computer Entertainment Inc.		*
 *	All Rights Reserved										*
 *															*
 ***********************************************************/
		
		
// runtime retexturing of sprites/3D models

//	Simplest way: can copy one texture's pixel (and clut) data over
// 	another's (using MoveImage), hence can retexture a TMD
//	or GsSPRITE fairly often, very easily;
//	see part of main: 
//				if (frameNumber == 240)	   { /* etc */ }


// more flexible method:
// actually draw onto parts of the frame buffer that are
// neither display or drawing buffers:
// adjust the clip and offset fields of GsDRAWENV, register changes
// with PutDrawEnv, draw sprites onto the pixels where 
// texture is itself.
// this is shown in this example, need to call OverWriteCluts
// and turn on the call to GsSortFastSprite in the second chunk
// of the program's main loop
// --- REASON why it looks shabby: writing to frame buffer
// will ALWAYS be in 16 bit mode, whereas the square TMD expects 
// a 4-bit texture, HENCE the distortion.

// find a TMD expecting a 16-bit texture and all will be well ....


// NOTE: the 2d clipping practised by libgs seems intrinsically slow:
// if you make the clip rectangle of GsDRAWENV smaller (eg 64 by 64)
// the frame rate drops dramatically...
// hence set larger clip area, although this uses up more frame buffer



			 
#include <stdio.h>
#include <rand.h>
#include <libps.h>
#include "pad.h"
#include "tmd.h"
#include "asssert.h"
#include "object.h"



   


#define SCREEN_WIDTH 320			 
#define	SCREEN_HEIGHT 240




#define PACKETMAX (2048*24)   /* Max GPU packets */
static PACKET packetArea[2][PACKETMAX]; /* GPU PACKETS AREA */

static PACKET OtherPacketArea[2][PACKETMAX];




#define SQUARE_MODEL_ADDRESS 0x80090000
#define SHIP_MODEL_ADDRESS 0x80091000
#define SPHERE_MODEL_ADDRESS 0x80096000


#define WAVE_TEXTURE_ADDRESS 0x80092000
GsIMAGE WaveTextureInfo;

#define FIRE_TEXTURE_ADDRESS 0x80093000
GsIMAGE FireTextureInfo;

#define TILE_TEXTURE_ADDRESS 0x80094000
GsIMAGE TileTextureInfo;

#define TILE2_TEXTURE_ADDRESS 0x80095000
GsIMAGE Tile2TextureInfo;

#define DONGLE_TEXTURE_ADDRESS 0x800b0000
GsIMAGE DongleTextureInfo;




#define OT_LENGTH	9		


static GsOT Wot[2];			/* Handler of OT */
static GsOT_TAG wtags[2][1<<OT_LENGTH]; 

static GsOT Otherot[2];			/* Handler of OT */
static GsOT_TAG Othertags[2][1];



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

#define FLOOR_X 1
#define FLOOR_Z 1

#define FLOOR_HEIGHT 128




	// limits on player movement
#define SHIP_MIN_X (-FLOOR_HEIGHT/2)
#define SHIP_MIN_Y (-FLOOR_HEIGHT * 2)
#define SHIP_MIN_Z (-FLOOR_HEIGHT/2)
#define SHIP_MAX_X (31 * SQUARE_SIZE/2)
#define SHIP_MAX_Y (FLOOR_HEIGHT/2)
#define SHIP_MAX_Z (31 * SQUARE_SIZE/2)



	// graphical world clipping

ObjectHandler* DisplayedObjects[MAX_OBJECTS];




GsBOXF Rectangle;		// for simplest drawing

GsSPRITE TheSprite;


/****************************************************************************
					prototypes
****************************************************************************/


void main (void);	   


int PerformWorldClipping (ObjectHandler** firstArray, ObjectHandler** secondArray);
void FindViewPoint (VECTOR* output);


int DealWithControllerPad(void);


void InitialiseAll(void);
void InitialiseObjects (void);
void CreateTheFloor (void);
		  
		  
		  
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


void OverWriteCluts (GsIMAGE* overWrite, GsIMAGE* overWritten);







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
	RECT clippingArea;		// in drawing environment
	u_short offsets[2];

	InitialiseAll();

	//OverWriteCluts( &TileTextureInfo, &WaveTextureInfo);
	
	side = GsGetActiveBuff();
	for	(;;)
		{
		if (frameNumber == 240)
			{
			RECT rect;

			setRECT( &rect, 320, 256, 64, 64);
			MoveImage( &rect, 640, 0);

			setRECT( &rect, 0, 488, 16, 1);
			MoveImage( &rect, 0, 480);

			DrawSync(0);
			}


		FntPrint("frame: %d\n", frameNumber);
		FntPrint("hsync: %d\n", hsync);
		FntPrint("ship pos:\n");
		FntPrint("sprite: %d %d\n", TheSprite.x, TheSprite.y);

		if (frameNumber == 0)
			{
				// print initial settings of DRAWENV   
			dumpRECT( &GsDRAWENV.clip);	
			printf("offsets: %hd %hd\n",
					GsDRAWENV.ofs[0], GsDRAWENV.ofs[1]);
			}

		printf("MAIN: 0000\n");

		frameNumber++;

		if (DealWithControllerPad() == 0)	   // quitting
			break;
											
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
			
			// check test sprite intact
		GsSortFastSprite( &TheSprite, &Wot[side], 0);
					
		hsync = VSync(0);	 // leave this line out for super fast frame rate

		ResetGraph(1);
		GsSwapDispBuff();
		GsSortClear(64, 64, 128, &Wot[side]);
		GsDrawOt(&Wot[side]);
		side = GsGetActiveBuff();
		FntFlush(-1);	  


			// this part of main loop: drawing to off-screen portion
			// of frame buffer for dynamic sprites and model textures
		GsClearOt(0,0,&Otherot[side]);
		GsSetWorkBase((PACKET*)OtherPacketArea[side]);
			// store clip and offsets of DRAWENV   
		clippingArea = GsDRAWENV.clip;	
		offsets[0] = GsDRAWENV.ofs[0];		
		offsets[1] = GsDRAWENV.ofs[1];
			// set new clip and offsets: draw to OUTSIDE double buffers
		setRECT(&GsDRAWENV.clip, 320, 0, 256, 256);
		GsDRAWENV.ofs[0] = 320;
		GsDRAWENV.ofs[1] = 0;
 			// register into system
		PutDrawEnv(&GsDRAWENV);

			// draw our sprite
			// turn this off for TMD's original texture
		GsSortSprite( &TheSprite, &Otherot[side], 0);

		GsDrawOt(&Otherot[side]);

		//DrawSync(0);

			// restore previous clip and offsets
		GsDRAWENV.clip = clippingArea;	
		GsDRAWENV.ofs[0] = offsets[0];	
		GsDRAWENV.ofs[1] = offsets[1];
	 			// register into system
		PutDrawEnv(&GsDRAWENV);	
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

	if (pad & PADRup)
		{
		TheSprite.y -= speedMultiplier;
		return 1;
		}
	if (pad & PADRdown)
		{
		TheSprite.y += speedMultiplier;
		return 1;
		}
	if (pad & PADRleft)
		{
		TheSprite.x -= speedMultiplier;
		return 1;
		}
	if (pad & PADRright)
		{
		TheSprite.x += speedMultiplier;
		return 1;
		}

		// moving the ship
	if (pad & PADL1)
		{
		TheShip.position.vz += speedMultiplier;
		return 1;
		}
	if (pad & PADL2)
		{
		TheShip.position.vz -= speedMultiplier;
		return 1;
		}
	if (pad & PADLup)
		{
		TheShip.position.vy -= speedMultiplier;
		return 1;
		}
	if (pad & PADLdown)
		{
		TheShip.position.vy += speedMultiplier;
		return 1;
		}
	if (pad & PADLleft)
		{
		TheShip.position.vx -= speedMultiplier;
		return 1;
		}
	if (pad & PADLright)
		{
		TheShip.position.vx += speedMultiplier;
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
	#if 0
	SetVideoMode(MODE_PAL);   
	assert(GetVideoMode() == MODE_PAL);
	#endif
	{
	int videoMode;

	videoMode = GetVideoMode();
	switch(videoMode)
		{
		case MODE_NTSC:	printf("mode is NTSC\n"); break;
		case MODE_PAL:	printf("mode is PAL\n"); break;
		default: printf("bad value from GetVideoMode\n"); exit(1);
		}
	}

	Wot[0].length = OT_LENGTH;	
	Wot[0].org = wtags[0];	   
	Wot[1].length = OT_LENGTH;
	Wot[1].org = wtags[1];

	GsClearOt(0, 0, &Wot[0]);
	GsClearOt(0, 0, &Wot[1]);

	Otherot[0].length = 1;	
	Otherot[0].org = Othertags[0];	   
	Otherot[1].length = 1;
	Otherot[1].org = Othertags[1];

	GsClearOt(0, 0, &Otherot[0]);
	GsClearOt(0, 0, &Otherot[1]);

	ProperInitialiseTexture(WAVE_TEXTURE_ADDRESS, &WaveTextureInfo);
	ProperInitialiseTexture(FIRE_TEXTURE_ADDRESS, &FireTextureInfo);
	ProperInitialiseTexture(TILE_TEXTURE_ADDRESS, &TileTextureInfo);
	ProperInitialiseTexture(TILE2_TEXTURE_ADDRESS, &Tile2TextureInfo);
	ProperInitialiseTexture(DONGLE_TEXTURE_ADDRESS, &DongleTextureInfo);

	LinkSpriteToImageInfo ( &TheSprite, &FireTextureInfo);
	TheSprite.x = 80;
	TheSprite.y = 70;
	
	
	InitialiseView();		
	InitialiseLighting();		   

	InitialiseObjects();

	CreateTheFloor();
		 
		// set up the basic screen-printing font
	FntLoad( 960, 256);			 
	FntOpen( -120, 40, 256, 200, 0, 512);		
	
		// set up very simplest red rectangle
	Rectangle.attribute = 0;
	Rectangle.x = 0;
	Rectangle.y = 0;
	Rectangle.w = 50;
	Rectangle.h = 50;
	Rectangle.r = 128;
	Rectangle.g = 0;
	Rectangle.b = 0;    
}







void InitialiseObjects (void)
{
	int i;

	InitialiseObjectClass();




   

	InitSingleObject(&TheShip);

	BringObjectToLife ( &TheShip, SHIP, 
					SPHERE_MODEL_ADDRESS, 0, NONE);

	TheShip.rotationSpeed = ONE/128;
	TheShip.movementSpeed = 1;
	TheShip.movementMomentumFlag = TRUE;


		// flip ship upside down for right-looking orientation
	TheShip.rotate.vx = ONE/2;
	UpdateObjectCoordinates (&TheShip.rotate, 
		&TheShip.position, &TheShip.coord);
	setVECTOR( &TheShip.rotate, 0, 0, 0);
   
	setVECTOR( &TheShip.position, SQUARE_SIZE*2, 0, SQUARE_SIZE*2);
	

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
 



void CreateTheFloor (void)
{
	int x, z;
	int xPos, zPos;
	int squareID = 0;
	ObjectHandler* square;

	for (x = 0; x < FLOOR_X; x++)	
		{
		for (z = 0; z < FLOOR_Z; z++)
			{			 
			xPos = x * SQUARE_SIZE;
			zPos = z * SQUARE_SIZE;

			square = &TheSquares[squareID];

			square->alive = TRUE;

			setVECTOR( &square->position, xPos, FLOOR_HEIGHT, zPos);

			setVECTOR( &square->rotate, ONE/2, 0, 0);

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

	HandleAllSquares();

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

	/*****
	TheView.vrx = 0; 
	TheView.vry = 0; 
	TheView.vrz = 0; 
	TheView.vpx = 0; 
	TheView.vpy = 0; 
	TheView.vpz = 192;
	TheView.rz = 0;

	TheView.super = &TheShip.coord;
	*****/

	TheView.vrx = SQUARE_SIZE * 2; 
	TheView.vry = 0; 
	TheView.vrz = SQUARE_SIZE * 2; 
	TheView.vpx = SQUARE_SIZE * 2; 
	TheView.vpy = 0; 
	TheView.vpz = (SQUARE_SIZE * 2) + 300;
	TheView.rz = 0;

	TheView.super = WORLD;

	GsSetRefView2(&TheView);
}






void InitialiseLighting (void)
{
#if 0
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
#endif

 		
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





void OverWriteCluts (GsIMAGE* overWrite, GsIMAGE* overWritten)
{
	RECT rect;

	assert(overWrite->pmode == overWritten->pmode);

	/**/
	setRECT( &rect, overWritten->cx, overWritten->cy,
			overWritten->cw, overWritten->ch);
	printf("----\n");
	dumpRECT( &rect);
	printf("----\n");
			
	LoadImage( &rect, overWrite->clut);
	/**/

	//setRECT( &rect, 0, 484, 16, 1);
	//MoveImage( &rect, 0, 480);

	DrawSync(0); 
}
