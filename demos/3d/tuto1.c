/************************************************************
 *															*
 *						tuto1.c								*
 *			   											    *															*
 *				L Evans     31/01/97						*		
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



   


#define SCREEN_WIDTH 320			 
#define	SCREEN_HEIGHT 240




#define PACKETMAX (2048*24)   /* Max GPU packets */
static PACKET packetArea[2][PACKETMAX]; /* GPU PACKETS AREA */




#define SQUARE_MODEL_ADDRESS 0x80090000
#define SHIP_MODEL_ADDRESS 0x80091000
#define MULTIPLE_MODEL_ADDRESS 0x80094000


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
#define CUBE 2
#define SPHERE 3
#define VENUS 4

ObjectHandler TheShip; 

#define MAX_SQUARES 256
ObjectHandler TheSquares[MAX_SQUARES];

#define MAX_CUBES 64
ObjectHandler TheCubes[MAX_CUBES];

#define MAX_SPHERES 32
ObjectHandler TheSpheres[MAX_SPHERES];

ObjectHandler TheVenus; 	 




#define SQUARE_SIZE 128



int SimpleClipDistance;

	// array of pointers for graphical world clipping
ObjectHandler* DisplayedObjects[MAX_OBJECTS];


/****************************************************************************
					prototypes
****************************************************************************/


void main (void);	   


int PerformWorldClipping (ObjectHandler** firstArray, ObjectHandler** secondArray);


int DealWithControllerPad(void);


void InitialiseAll(void);
void InitialiseObjects (void);
		  
		  
		  
void HandleAllObjects (void);

void HandleTheShip (ObjectHandler* object);



void UpdateObjectCoordinates (SVECTOR* rotationVector,
							VECTOR* translationVector,
							GsCOORDINATE2* coordSystem);
							
							
								
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
	int hsync;
	ObjectHandler* object;
	u_short zValue;
	int numberToDisplay;

	InitialiseAll();
	
	side = GsGetActiveBuff();
	for	(;;)
		{
		FntPrint("frame: %d\n", frameNumber);
		FntPrint("hsync: %d\n", hsync);

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
	ObjectHandler *object, **pointer;

	SimpleClipDistance = SQUARE_SIZE * 8;
	
	for (i = 0; i < MAX_OBJECTS; i++)
		{
		if (firstArray[i] != NULL)
			if (firstArray[i]->alive == TRUE)
				{
				#if 0
				if (firstArray[i]->coord.coord.t[2] < SimpleClipDistance)
						{
						secondArray[numberDisplayedObjects] = firstArray[i];
						numberDisplayedObjects++;
						}
				#endif
				secondArray[numberDisplayedObjects] = firstArray[i];
				numberDisplayedObjects++;
				}
		}
		
	return numberDisplayedObjects;	
}			



int DealWithControllerPad (void)
{
	long pad;
	int speedMultiplier;
  
	pad = PadRead();

		// quit program
	if (pad & PADstart && pad & PADselect)
		return 0;
		
		// how fast do we turn
	if (pad & PADselect)
		speedMultiplier = 10;
	else
		speedMultiplier = 1;	
						
		// world-relative rotations
	if (pad & PADL1)
		TheShip.rotate.vz -= TheShip.rotationSpeed * speedMultiplier;
	if (pad & PADL2)
		TheShip.rotate.vz += TheShip.rotationSpeed * speedMultiplier;
	if (pad & PADLleft)
		TheShip.rotate.vy -= TheShip.rotationSpeed * speedMultiplier;
	if (pad & PADLright)
		TheShip.rotate.vy += TheShip.rotationSpeed * speedMultiplier;
	if (pad & PADLup)
		TheShip.rotate.vx -= TheShip.rotationSpeed * speedMultiplier;
	if (pad & PADLdown)
		TheShip.rotate.vx += TheShip.rotationSpeed * speedMultiplier;	   

		// move object closer to/further from screen
	if (pad & PADR1)
		TheView.vpz += 50 * speedMultiplier;
	if (pad & PADR2)
		TheView.vpz -= 50 * speedMultiplier;

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
	if (SCREEN_HEIGHT < 480)
		GsDefDispBuff(0, 0, 0, SCREEN_HEIGHT);
	else
		GsDefDispBuff(0, 0, 0, 0);

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
		 
		// set up the basic screen-printing font
	FntLoad( 960, 256);			 
	FntOpen( 44, 96, 256, 200, 0, 512);		  
}







void InitialiseObjects (void)
{
	InitialiseObjectClass();


	InitSingleObject(&TheShip);

	BringObjectToLife ( &TheShip, SHIP, 
					SHIP_MODEL_ADDRESS, 0, NONE);

	TheShip.rotationSpeed = ONE/128;

	RegisterObjectIntoObjectArray( &TheShip);

	LinkAllObjectsToModelsOrSprites();
	LinkAllObjectsToTheirCoordinateSystems();
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
					case SQUARE:	   // not yet
					case CUBE:
					case SPHERE:
					case VENUS:
					default:	
						assert(FALSE);
					}
				}
			}
		}
}





void HandleTheShip (ObjectHandler* object)
{
	assert(object->type == SHIP);

	UpdateObjectCoordinates ( &object->rotate,
							&object->position,
							&object->coord);
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






void InitialiseView (void)
{
	ProjectionDistance = 192;			
	GsSetProjection(ProjectionDistance);  

	TheView.vrx = 0; 
	TheView.vry = 0; 
	TheView.vrz = 0; 
	TheView.vpx = 0; 
	TheView.vpy = 0; 
	TheView.vpz = -1000;
	TheView.rz = 0;
	TheView.super = WORLD;
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

