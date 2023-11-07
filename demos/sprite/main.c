/************************************************************
 *															*
 *			   											    *																	
 *															*
 *	Copyright (C) 1996 Sony Computer Entertainment Inc.		*
 *	All Rights Reserved										*
 *															*
 ***********************************************************/

#include <stdio.h>
#include <rand.h>
#include <libps.h>

#include "pad.h"
#include "asssert.h"
#include "dump.h"

#define FIRE_TEXTURE_ADDRESS 0x80090000			// where held in main RAM
GsIMAGE FireTextureInfo;					   

#define DOTS_TEXTURE_ADDRESS 0x80091000
GsIMAGE DotsTextureInfo;

#define MASCOTS_TEXTURE_ADDRESS 0x80092000
GsIMAGE MascotsTextureInfo;

#define PACKETMAX	2048		/* Max GPU packets */
#define PACKETMAX2	(PACKETMAX*24) 

#define OT_LENGTH	9		

static GsOT Wot[2];			/* Handler of OT */
static GsOT_TAG wtags[2][1<<OT_LENGTH]; 

static PACKET packetArea[2][PACKETMAX2]; /* GPU PACKETS AREA */	

#define TRUE 1
#define FALSE 0

int frameNumber = 0;
int QuitFrameNumber = -1;
	 
typedef struct
{	
	int id;
	int alive;

	GsIMAGE* imageInfo;
	GsSPRITE sprite;
} ObjectHandler;

#define MAX_OBJECTS 256

ObjectHandler ObjectArray[MAX_OBJECTS];
 
#define CLOCKWISE 0
#define ANTICLOCKWISE 1	

/****************************************************************************
					prototypes
****************************************************************************/


void main (void);

void DealWithControllerPad(void);

void InitialiseAll(void);
		  		  		   		
int InitialiseTexture(long addr);
void ProperInitialiseTexture (long address, GsIMAGE* imageInfo);
void InitGsSprite (GsSPRITE* sprite);
void LinkSpriteToImageInfo (GsSPRITE* sprite, GsIMAGE* imageInfo);
void FindTopLeftOfTexturePage (GsIMAGE* imageInfo, 
									int* x, int* y, int* u, int* v);

void InitialiseObjects (void);   	  
ObjectHandler* GetNextFreeObject (void);
void CreateSomeObjects (void);
void MakeLivingObjectOfTimFile (ObjectHandler* object, GsIMAGE* imageInfo, int x, int y);	  

/****************************************************************************
					 functions
****************************************************************************/

void main (void)
{
	int	i;
	int	hsync = 0;
	int side;			  // buffer index
	ObjectHandler* object;

	InitialiseAll();

	side = GsGetActiveBuff();

	while(1)
		{
		if (QuitFrameNumber == frameNumber)
			break;		

		FntPrint("frame: %d\n", frameNumber);				

		frameNumber++;

		DealWithControllerPad();

		GsSetWorkBase((PACKET*)packetArea[side]);

		GsClearOt(0,0,&Wot[side]);

		for (i = 0; i < MAX_OBJECTS; i++)
			{
			object = &ObjectArray[i];
			if (object->alive == TRUE)
				{
				GsSortSprite( &object->sprite, &Wot[side], 0);
				}
			}

		hsync = VSync(0);		 
		ResetGraph(1);
		GsSwapDispBuff();
		GsSortClear(0,0,4,&Wot[side]);
		GsDrawOt(&Wot[side]);
		side ^= 1;
		FntFlush(-1);
		}

	ResetGraph(3);
}
				   	
void DealWithControllerPad (void)
{
	long	pad;
  
	pad = PadRead();

		// quit program
	if (pad & PADstart && pad & PADselect)
		{
		QuitFrameNumber = frameNumber + 1;
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

	GsInitGraph(320, 240, GsINTER|GsOFSGPU, 1, 0);
	GsDefDispBuff(0, 0, 0, 240);

	GsInit3D();		   

	Wot[0].length=OT_LENGTH;	
	Wot[0].org=wtags[0];	   
	Wot[1].length=OT_LENGTH;
	Wot[1].org=wtags[1];

	GsClearOt(0,0,&Wot[0]);
	GsClearOt(0,0,&Wot[1]);

		// transfer textures from main RAM to VRAM
	ProperInitialiseTexture(MASCOTS_TEXTURE_ADDRESS, &MascotsTextureInfo);
	ProperInitialiseTexture(DOTS_TEXTURE_ADDRESS, &DotsTextureInfo);
	ProperInitialiseTexture(FIRE_TEXTURE_ADDRESS, &FireTextureInfo);

	InitialiseObjects();

	CreateSomeObjects();
		 
	FntLoad( 960, 256);
	FntOpen( 30, 50, 256, 200, 0, 512);		  
}
	 
int InitialiseTexture(long addr)
{
	RECT rect;
	GsIMAGE tim1;

	GsGetTimInfo((u_long *)(addr+4),&tim1);

	rect.x=tim1.px;	
	rect.y=tim1.py;		
	rect.w=tim1.pw;	
	rect.h=tim1.ph;	


	LoadImage(&rect,tim1.pixel);

	if((tim1.pmode>>3)&0x01) {
		rect.x=tim1.cx;	
		rect.y=tim1.cy;	
		rect.w=tim1.cw;	
		rect.h=tim1.ch;	

		LoadImage(&rect,tim1.clut);
	}
	DrawSync(0);
	return(0);
}

	// the second argument is a global declared next to the #define for
	// the TIM file main memory address;
	// it allows endless sprites to be easily linked to it later on
void ProperInitialiseTexture (long address, GsIMAGE* imageInfo)
{
	RECT rect;

		// +4 because we need to skip head of TIM file
	GsGetTimInfo( (u_long *)(address+4), imageInfo);

	//dumpIMAGE(imageInfo);

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
		
void InitGsSprite (GsSPRITE* sprite)
{
		// initialise sprite to dummy
	sprite->attribute = 0;
	sprite->x = 0;
	sprite->y = 0;
	sprite->w = 0;
	sprite->h = 0;
	sprite->tpage = 0;
	sprite->u = 0;
	sprite->v = 0;
	sprite->cx = 0;
	sprite->cy = 0;
	sprite->r = 128;
	sprite->g = 128;
	sprite->b = 128;
	sprite->mx = 0;
	sprite->my = 0;
	sprite->scalex = ONE;
	sprite->scaley = ONE;
	sprite->rotate = 0;
}

#define SIXTEEN_BIT_MASK (1 << 25)
#define EIGHT_BIT_MASK (1 << 24)

void LinkSpriteToImageInfo (GsSPRITE* sprite, GsIMAGE* imageInfo)
{
	int widthCompression;	  // on frame buffer
	int tPageType;
	int texturePageX, texturePageY, xOffset, yOffset;

	InitGsSprite(sprite);

	//dumpIMAGE(imageInfo);
		
	FindTopLeftOfTexturePage(imageInfo, 
		&texturePageX, &texturePageY, &xOffset, &yOffset);

	switch(imageInfo->pmode)
		{
		case 2:		   // 16-bit
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

	//dumpSPRITE(sprite);
}
	
 	// find coords wrt tpage top left points
void FindTopLeftOfTexturePage (GsIMAGE* imageInfo, 
									int* x, int* y, int* u, int* v)
{
	int testX, testY;

	testX = imageInfo->px;
	testY = imageInfo->py;

	*u = 0;
	*v = 0;
	
	for (;;)
		{
		if (testX % 320 != 0)
			{
			testX--;
			(*u)++;
			}
		else
			break;
		}

	for (;;)
		{
		if (testY % 256 != 0)
			{
			testY--;
			(*v)++;
			}
		else
			break;
		}

	*x = testX;
	*y = testY;
}	

void InitialiseObjects (void)
{
	int i;
	ObjectHandler *object;
	
	for (i = 0; i < MAX_OBJECTS; i++)
		{
		object = &ObjectArray[i];

		object->id = i;
		object->alive = FALSE;

		object->imageInfo = NULL;
		InitGsSprite( &object->sprite);
		}
}

ObjectHandler* GetNextFreeObject (void)
{
	ObjectHandler* object = NULL;
	int i;

	for (i = 0; i < MAX_OBJECTS; i++)
		{
		if (ObjectArray[i].alive == FALSE)
			{
			object = &ObjectArray[i];
			break;
			}
		}

	return object;
}

void CreateSomeObjects (void)
{
	ObjectHandler* object;

	object = GetNextFreeObject();
	assert(object != NULL);
	MakeLivingObjectOfTimFile(object, &MascotsTextureInfo, -50, -20);
	//printf("id: %d\n", object->id);

	object = GetNextFreeObject();
	assert(object != NULL);
	MakeLivingObjectOfTimFile(object, &DotsTextureInfo, -50, 80);
	//printf("id: %d\n", object->id);

	object = GetNextFreeObject();
	assert(object != NULL);
	MakeLivingObjectOfTimFile(object, &FireTextureInfo, 20, 80);
	//printf("id: %d\n", object->id);
}

void MakeLivingObjectOfTimFile (ObjectHandler* object, GsIMAGE* imageInfo, int x, int y)
{
	object->alive = TRUE;

	object->imageInfo = imageInfo;
	LinkSpriteToImageInfo (&object->sprite, object->imageInfo);

	object->sprite.x = x;
	object->sprite.y = y;
}


  

