/************************************************************
 *															*
 *			   											    *															*
 *				L Evans     May 97							*		
 *															*
 *	Copyright (C) 1996 Sony Computer Entertainment Inc.		*
 *	All Rights Reserved										*
 *															*
 ***********************************************************/

	
#include <libps.h>

#include <stdio.h>

#include "asssert.h"

#include "general.h"

#include "2d1.h"

#include "timer.h"

#include "main.h"






#define SCREEN_WIDTH 320			 
#define	SCREEN_HEIGHT 240
		
#define PACKETMAX2 (2048*72)			  /* Max GPU packets */
static PACKET packetArea[2][PACKETMAX2]; /* GPU PACKETS AREA */

#define SINGLE_MODEL_ADDRESS 0x800c0000
#define FIRE_TEXTURE_ADDRESS 0x800b0000

#define OT_LENGTH	9		
static GsOT Wot[2];			/* Handler of OT */
static GsOT_TAG wtags[2][1<<OT_LENGTH]; 

static GsRVIEW2 view;
int ProjectionDistance;	

GsF_LIGHT TheLights[3];	

typedef struct
{			
	int alive;
	GsDOBJ2	handler;
	VECTOR position;		// position
	SVECTOR rotate;			// angle
	VECTOR scaler;			// dimension scaling
	GsCOORDINATE2 coord;	 // coordinate system	
} ObjectHandler;


#define MAX_OBJECTS 100
ObjectHandler ArrayOfObjects[MAX_OBJECTS];

int NumberOfObjects = 1;




GsIMAGE FireImageInfo;
GsSPRITE FireSprite;

#define FIRE_SPRITE_LEFT_LIMIT -150
#define FIRE_SPRITE_RIGHT_LIMIT 130

#define BASIC_FIRE_SPRITE_SPEED 6

int FireSpriteMovementSpeed;
int FireSpriteDX;


	
volatile u_char *bb0, *bb1;	   // low-level pad buffers: never need to touch
  	 
	 // constants for interface	to controller pad
#define PADLup     (1<<12)
#define PADLdown   (1<<14)
#define PADLleft   (1<<15)
#define PADLright  (1<<13)
#define PADRup     (1<< 4)
#define PADRdown   (1<< 6)
#define PADRleft   (1<< 7)
#define PADRright  (1<< 5)
#define PADi       (1<< 9)
#define PADj       (1<<10)
#define PADk       (1<< 8)
#define PADl       (1<< 3)
#define PADm       (1<< 1)
#define PADn       (1<< 2)
#define PADo       (1<< 0)
#define PADh       (1<<11)
#define PADL1      PADn
#define PADL2      PADo
#define PADR1      PADl
#define PADR2      PADm
#define PADstart   PADh
#define PADselect  PADk




int frameNumber = 0;

/****************************************************************************
					prototypes
****************************************************************************/

void main (void);	   
int DealWithControllerPad(void);
void InitialiseAll(void);
void SetUpTheObject (void);
void SetUpTheArrayOfObjects (void);
void UpdateObjectCoordinates (SVECTOR* rotationVector,
							GsCOORDINATE2* coordSystem);
void SortObjectPosition (ObjectHandler *object);
void SortObjectScaling (ObjectHandler *object);
void PadInit (void);
u_long PadRead(void);

void BringAliveAnotherObject (void);
void KillOffAnObject (void);

void SetUpTheSprite (void);
void HandleTheFireSprite (GsOT *ot);


/****************************************************************************
					 functions
****************************************************************************/

void main (void)
{
	int side;			  // buffer index
	MATRIX	tmpls;
	ObjectHandler *object;
	int i;
	char stringBuffer[64];

	InitialiseAll();
	
	side = GsGetActiveBuff();
	
	for(;;)
		{
		if (DealWithControllerPad() == 0)	   // quitting
			break;
					
		GsSetWorkBase((PACKET*)packetArea[side]);

		GsClearOt(0,0,&Wot[side]);

		GsSetRefView2(&view); 

		FntPrint("frame %d\n", frameNumber);
		FntPrint("average hsync %d\n", AverageHsync);
		FntPrint("Number objects %d\n", NumberOfObjects);
		FntPrint("predicted frame rate %d\n", PredictedFrameRate);

		GetStringDescribingFrameRateStrategy(stringBuffer);
		FntPrint("frame rate strategy :-\n%s\n", stringBuffer);

		if (AutoTimerSortItOutFlag == TRUE)
			FntPrint("Timer: auto sort it out\n");
		else
			FntPrint("Timer: set manually\n");

		for (i = 0; i < MAX_OBJECTS; i++)
			{
			if (ArrayOfObjects[i].alive == TRUE)
				{
				object = &ArrayOfObjects[i];

				GsGetLs(&(object->coord), &tmpls);
				GsSetLightMatrix(&tmpls);		
				GsSetLsMatrix(&tmpls);		
				GsSortObject4( &(object->handler), &Wot[side], 
							3, getScratchAddr(0));
				}
			}

		HandleTheFireSprite( &Wot[side]);
		
		#if 0		// OLD: presumes game at/close to 60fps
		DrawSync(0);			
		hsync = VSync(0);
		#endif
		HandleGameTiming();	 
				
		GsSwapDispBuff();
		GsSortClear(0,0,4,&Wot[side]);
		GsDrawOt(&Wot[side]);
		side ^= 1;
		FntFlush(-1);

		frameNumber++;
		}

		// program cleanup
	ResetGraph(3);
}

int DealWithControllerPad (void)
{
	long pad;
	int pause = 15;
	static int framesSinceLastChange = 0;
	int controlSpeed;
	
	pad = PadRead();
	framesSinceLastChange++;

	if (pad & PADstart && pad & PADselect)		// quit program
		return 0;	

	if (pad & PADstart)			// pause
		{
		while (pad & PADstart)
			{
			pad = PadRead();
			}
		}
		
	if (pad & PADselect)		  // how fast controls act
		controlSpeed = 10;
	else
		controlSpeed = 1;	

	if (pad & PADRup)	  // draw more shuttles
		{
		if (framesSinceLastChange > pause)
			{
			if (NumberOfObjects < MAX_OBJECTS)
				{
				BringAliveAnotherObject();
				framesSinceLastChange = 0;
				}
			}
		}
	else if (pad & PADRdown)	  // draw less shuttles
		{
		if (framesSinceLastChange > pause)
			{
			if (NumberOfObjects > 1)
				{
				KillOffAnObject();
				framesSinceLastChange = 0;
				}
			}
		}

	if (pad & PADL1)	 // toggle self-sorting	
		{
		if (framesSinceLastChange > pause)
			{
			if (AutoTimerSortItOutFlag == TRUE)
				AutoTimerSortItOutFlag = FALSE;
			else
				{
				assert(AutoTimerSortItOutFlag == FALSE);
				AutoTimerSortItOutFlag = TRUE;
				}
			framesSinceLastChange = 0;
			}
		}

	return 1;
}

void InitialiseAll (void)
{	
	PadInit();				 // init controller pad

		// main graphical initialisations
	GsInitGraph(SCREEN_WIDTH,SCREEN_HEIGHT,GsINTER|GsOFSGPU,1,0);
	GsDefDispBuff(0,0,0,SCREEN_HEIGHT);
	GsInit3D();		   

	Wot[0].length=OT_LENGTH;	
	Wot[0].org=wtags[0];	   
	Wot[1].length=OT_LENGTH;
	Wot[1].org=wtags[1];

	GsClearOt(0,0,&Wot[0]);
	GsClearOt(0,0,&Wot[1]);	

	ProjectionDistance = 192;			// screen to viewpoint distance
	GsSetProjection(ProjectionDistance);  
		
	view.vrx = 0; 		  // viewpoint
	view.vry = 0; 
	view.vrz = 0; 
	view.vpx = 0; 
	view.vpy = 0; 
	view.vpz = -5000;
	view.rz = 0;
	view.super = WORLD;
	GsSetRefView2(&view);

		// sort basic text printing
	FntLoad( 960, 256);
	FntOpen( -120, -50, 256, 200, 0, 512);
		
		// three flat lights
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
	GsSetAmbient(ONE/2, ONE/2, ONE/2);	   
	
	SetUpTheArrayOfObjects();	 
	
	SetUpTheSprite(); 

	InitialiseTimer();
}

void SetUpTheArrayOfObjects (void)
{
	int i;
	ObjectHandler *object;
	u_long *pointer;

		// set pointer to top of TMD in main memory
	pointer = (u_long *) SINGLE_MODEL_ADDRESS;
	pointer++;			// skip TMD header	 
	GsMapModelingData(pointer);			 // Map TMD offset addresses to real addresses
	pointer += 2;	  // skip some more

	for (i = 0; i < MAX_OBJECTS; i++)
		{
		object = &ArrayOfObjects[i];

		if (i == 0)
			object->alive = TRUE;
		else
			object->alive = FALSE;

		object->position.vx = (-2000) + (i * 1250);
		object->position.vy = 0;
		object->position.vz = 0;

		object->rotate.vx = 0;
		object->rotate.vy = 0;
		object->rotate.vz = 0;

		object->scaler.vx = ONE/2;
		object->scaler.vy = ONE/2;
		object->scaler.vz = ONE/2;

		GsInitCoordinate2(WORLD, &object->coord);

		object->handler.coord2 = &(object->coord);

			// link TMD data to object handler	
		GsLinkObject4( (u_long)pointer, &object->handler, 0);

		SortObjectPosition(object);
		SortObjectScaling(object);
		}
}

void UpdateObjectCoordinates (SVECTOR* rotationVector,
							GsCOORDINATE2* coordSystem)
{
	MATRIX tempMatrix;

	tempMatrix.t[0] = coordSystem->coord.t[0];
	tempMatrix.t[1] = coordSystem->coord.t[1];
	tempMatrix.t[2] = coordSystem->coord.t[2];
	
	RotMatrix(rotationVector, &tempMatrix);	   // get rotation matrix from rotation vector
		
	coordSystem->coord = tempMatrix;	// assign new matrix to coordinate system
	   	
	coordSystem->flg = 0;  // tell GTE that coordinate system has been updated
}

void SortObjectPosition (ObjectHandler *object)
{
	object->coord.coord.t[0] = object->position.vx;
	object->coord.coord.t[1] = object->position.vy;
	object->coord.coord.t[2] = object->position.vz;

	object->coord.flg = 0;
}

void SortObjectScaling (ObjectHandler *object)
{
	ScaleMatrix( &object->coord.coord, &object->scaler);

	object->coord.flg = 0;
}

void PadInit (void)
{
	GetPadBuf(&bb0, &bb1);
}

u_long PadRead(void)
{
	return(~(*(bb0+3) | *(bb0+2) << 8 | *(bb1+3) << 16 | *(bb1+2) << 24));
}		




void BringAliveAnotherObject (void)
{
	assert(NumberOfObjects >= 1);
	assert(NumberOfObjects < MAX_OBJECTS);

	//printf("Bringing object %d to life\n", NumberOfObjects+1);

	ArrayOfObjects[NumberOfObjects].alive = TRUE;
	NumberOfObjects++;
}



void KillOffAnObject (void)
{
  	assert(NumberOfObjects > 1);
	assert(NumberOfObjects < MAX_OBJECTS);

	//printf("Killing off object %d\n", NumberOfObjects);

	ArrayOfObjects[NumberOfObjects-1].alive = FALSE;
	NumberOfObjects--;
}





void SetUpTheSprite (void)
{
	ProperInitialiseTexture(FIRE_TEXTURE_ADDRESS, &FireImageInfo);
	LinkSpriteToImageInfo( &FireSprite, &FireImageInfo);

	FireSprite.x = -100;
	FireSprite.y = 80;

	FireSpriteMovementSpeed = BASIC_FIRE_SPRITE_SPEED;
	FireSpriteDX = FireSpriteMovementSpeed;
}




void HandleTheFireSprite (GsOT *ot)
{
		// under auto-timer, adjust sprite movement speed
		// with actual frame rate, so that no matter what the actual
		// fps, the sprite appears to move at reasonably constant speed
	if (AutoTimerSortItOutFlag == TRUE)
		{
		int maxFrameRate;
		int videoMode;

		videoMode = GetVideoMode();
		switch(videoMode)
			{
			case MODE_PAL: 
				maxFrameRate = MAX_FRAME_RATE_UNDER_PAL; 
				break;
			case MODE_NTSC: 
				maxFrameRate = MAX_FRAME_RATE_UNDER_NTSC; 
				break;
			default:	
				assert(FALSE);
			}

		if (PredictedFrameRate == 0)
			FireSpriteMovementSpeed = BASIC_FIRE_SPRITE_SPEED;
		else
			FireSpriteMovementSpeed 
				= (BASIC_FIRE_SPRITE_SPEED * maxFrameRate)
									 / PredictedFrameRate;
		}

	if (FireSprite.x + FireSpriteDX >= FIRE_SPRITE_RIGHT_LIMIT)
		{
		FireSpriteDX = -FireSpriteMovementSpeed;
		}
	else if (FireSprite.x + FireSpriteDX <= FIRE_SPRITE_LEFT_LIMIT)
		{
		FireSpriteDX = FireSpriteMovementSpeed;
		}

	FireSprite.x += FireSpriteDX;

	GsSortFastSprite( &FireSprite, ot, 0);
}


  




