/************************************************************
 *															*
 *						Tuto6.c								*
 *			   											    *															*
 *				LPGE     07/02/97							*		
 *															*
 *	Copyright (C) 1996 Sony Computer Entertainment Inc.		*
 *	All Rights Reserved										*
 *															*
 ***********************************************************/




// Here, see runtime clut manipulation
// good way to get animation on the cheap, and
// many other useful 2D effects


// See bottom of file: create your own TIMs in code:
// often easier way to achieve specific 2d effects
// than trying to force paint package to use certain CLUT and
// specify pixels exactly


#include <stdio.h>
#include <rand.h>
#include <libps.h>

#include "pad.h"
#include "asssert.h"
#include "dump.h"



#define FIRE_TEXTURE_ADDRESS 0x80090000
GsIMAGE FireTextureInfo;

#define DOTS_TEXTURE_ADDRESS 0x80091000
GsIMAGE DotsTextureInfo;

#define MASCOTS_TEXTURE_ADDRESS 0x80092000
GsIMAGE MascotsTextureInfo;

#define WHITE_TEXTURE_ADDRESS 0x800a0000
GsIMAGE WhiteTextureInfo;



GsIMAGE StripedOne;

	

	

#define SCREEN_WIDTH 320				// screen width and height
#define	SCREEN_HEIGHT 240



#define PACKETMAX	2048		/* Max GPU packets */
#define PACKETMAX2	(PACKETMAX*24)


	 

#define OT_LENGTH	9		



static GsOT Wot[2];			/* Handler of OT */
static GsOT_TAG wtags[2][1<<OT_LENGTH]; 


	 

static PACKET packetArea[2][PACKETMAX2]; /* GPU PACKETS AREA */

	   
	
	
		

#ifndef TRUE
	#define TRUE 1
	#define FALSE 0
#endif


int frameNumber = 0;
int QuitFrameNumber = -1;

		 	 
   	   



typedef struct
{	
	int id;
	int alive;

	GsIMAGE* imageInfo;
	GsSPRITE sprite;
	
	int scalingFlag;
	int scaleX, scaleY;
		
	VECTOR position;		// position
	VECTOR velocity;		
	int movementMomentumFlag;
	int movementSpeed;

	SVECTOR rotate;			// angle
	VECTOR twist;			// change of angle
	int rotationMomentumFlag;
	int rotationSpeed;

	GsCOORDINATE2 coord;	 // coordinate system

	MATRIX matrix;			// relation to SUPER

		// object-internal timing counters
	int lifeTime;
	int firstPeriod, secondPeriod, thirdPeriod;

} ObjectHandler;

#define MAX_OBJECTS 256

ObjectHandler ObjectArray[MAX_OBJECTS];


 
 
#define CLOCKWISE 0
#define ANTICLOCKWISE 1	



	// clut cycling
int cyclePoint, cyclePhase;
int cyclePoint2, cyclePhase2;	  
int cyclePoint3, cyclePhase3;	



/****************************************************************************
					prototypes
****************************************************************************/


void main (void);

void OscillateFloorTexture (void);

void CreateTestTexturePage (void);


	 
void DealWithControllerPad(void);


void InitialiseAll(void);
		  		  		   		

int InitialiseTexture(long addr);


void ProperInitialiseTexture (long address, GsIMAGE* imageInfo);


void InitGsSprite (GsSPRITE* sprite);

void LinkSpriteToImageInfo (GsSPRITE* sprite, GsIMAGE* imageInfo);

void FindTopLeftOfTexturePage (GsIMAGE* imageInfo, 
									int* x, int* y, int* u, int* v);


void InitialiseObjects (void);

void InitObject (ObjectHandler* object, int id);   	  
		
void InitMatrix (MATRIX* matrix);


ObjectHandler* GetNextFreeObject (void);


void CreateSomeObjects (void);

void MakeLivingObjectOfTimFile (ObjectHandler* object, GsIMAGE* imageInfo, int x, int y);


void MoveAllObjects (VECTOR* movement);

void RotateAllObjects (int amount, int whichWay);

void ScaleAllObjects (int xMultiplier, int yMultiplier);


void UpdateAllObjects (void);

void PrintClut (int x, int y, int bitMode);


void CycleDotsClut (void);
void CycleMascotsClut (void);
void CycleFireClut (void);


int SmallIntToColourPQR (int arg);
void CreateStripedTextureOne (void);

		  


/****************************************************************************
					 macros
****************************************************************************/



#define setVECTOR( vector, x, y, z)			\
	(vector)->vx = x, (vector)->vy = y, (vector)->vz = z


/****************************************************************************
					 functions
****************************************************************************/



void main (void)
{
	int	i;
	int	hsync = 0;
	int side;			  // buffer index
	ObjectHandler* object;

	printf("IN MAIN\n");
	InitialiseAll();


   

	side = GsGetActiveBuff();

	while(1)
		{
		if (QuitFrameNumber == frameNumber)
			break;		

		FntPrint("frame: %d\n", frameNumber);
		FntPrint("hsync: %d\n", hsync);				

		frameNumber++;

		DealWithControllerPad();

		UpdateAllObjects();

		GsSetWorkBase((PACKET*)packetArea[side]);

		GsClearOt(0,0,&Wot[side]);

	   	//CycleDotsClut();
	   	//CycleMascotsClut();
	   	//CycleFireClut();

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
	int speedMultiplier;
	VECTOR movement;
  
	pad = PadRead();

		// select and R1 and R2: step slowly
	if (pad & PADselect && pad & PADR1 && pad & PADR2)
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

	if (pad & PADselect)
		speedMultiplier = 4;
	else
		speedMultiplier = 1;

	if (pad & PADLup)
		{
		setVECTOR( &movement, 0, -speedMultiplier, 0);
		MoveAllObjects( &movement);
		return;
		}
	if (pad & PADLdown)
		{
		setVECTOR( &movement, 0, speedMultiplier, 0);
		MoveAllObjects( &movement);
		return;
		}
	if (pad & PADLleft)
		{
		setVECTOR( &movement, -speedMultiplier, 0, 0);
		MoveAllObjects( &movement);
		return;
		}
	if (pad & PADLright)
		{
		setVECTOR( &movement, speedMultiplier, 0, 0);
		MoveAllObjects( &movement);
		return;
		}
	if (pad & PADL1)
		{
		RotateAllObjects (ONE * speedMultiplier, ANTICLOCKWISE);
		return;
		}
	if (pad & PADR1)
		{
		RotateAllObjects (ONE * speedMultiplier, CLOCKWISE);
		return;
		}
	
	if (pad & PADRup)
		{
		ScaleAllObjects (ONE, ONE * 21 / 20);
		return;
		}
	if (pad & PADRdown)
		{
		ScaleAllObjects (ONE, ONE * 19 / 20);
		return;
		}
	if (pad & PADRleft)
		{
		ScaleAllObjects (ONE * 21 / 20, ONE);
		return;
		}
	if (pad & PADRright)
		{
		ScaleAllObjects (ONE * 19 / 20, ONE);
		return;
		}
}

		


	   

 

	  




void InitialiseAll (void)
{
	printf("iseAll: 0\n");
	PadInit();

	printf("iseAll: 1\n");



	GsInitGraph(SCREEN_WIDTH,SCREEN_HEIGHT,GsINTER|GsOFSGPU,1,0);
	if( SCREEN_HEIGHT<480 )
		GsDefDispBuff(0,0,0,SCREEN_HEIGHT);
	else
		GsDefDispBuff(0,0,0,0);

	GsInit3D();		   
	printf("iseAll: 2\n");

	Wot[0].length=OT_LENGTH;	
	Wot[0].org=wtags[0];	   
	Wot[1].length=OT_LENGTH;
	Wot[1].org=wtags[1];

	GsClearOt(0,0,&Wot[0]);
	GsClearOt(0,0,&Wot[1]);

		
	printf("iseAll: 3\n");



	printf("iseAll: 5\n");


	ProperInitialiseTexture(MASCOTS_TEXTURE_ADDRESS, &MascotsTextureInfo);
	ProperInitialiseTexture(DOTS_TEXTURE_ADDRESS, &DotsTextureInfo);
	ProperInitialiseTexture(FIRE_TEXTURE_ADDRESS, &FireTextureInfo);
	ProperInitialiseTexture(WHITE_TEXTURE_ADDRESS, &WhiteTextureInfo);

	CreateStripedTextureOne();



	printf("iseAll: 6\n");

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

	printf("ProperInitialiseTexture\n");
	dumpIMAGE(imageInfo);
	printf("\n\n");

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

	dumpIMAGE(imageInfo);
		
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

	dumpSPRITE(sprite);
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
	
	for (i = 0; i < MAX_OBJECTS; i++)
		{
		InitObject( &ObjectArray[i], i);
		}
}




void InitObject (ObjectHandler* object, int id)
{
	object->id = id;
	object->alive = FALSE;

	object->imageInfo = NULL;
	InitGsSprite( &object->sprite);

	object->scalingFlag = FALSE;
	object->scaleX = object->scaleY = ONE;

	setVECTOR( &object->position, 0, 0, 0);
	setVECTOR( &object->velocity, 0, 0, 0);
	object->movementMomentumFlag = FALSE;
	object->movementSpeed = 0;

	setVECTOR( &object->rotate, 0, 0, 0);
	setVECTOR( &object->twist, 0, 0, 0);
	object->rotationMomentumFlag = FALSE;
	object->rotationSpeed = 0;

	GsInitCoordinate2( WORLD, &object->coord);

	InitMatrix( &object->matrix);

	object->lifeTime = 0;
	object->firstPeriod = 0;
	object->secondPeriod = 0;
	object->thirdPeriod = 0;
}			 	  


   	  
		
void InitMatrix (MATRIX* matrix)
{
	matrix->t[0] = matrix->t[1] = matrix->t[2] = 0;

	matrix->m[0][0] = matrix->m[1][1] = matrix->m[2][2] = ONE;

	matrix->m[0][1] = matrix->m[0][2] = 0;
	matrix->m[1][0] = matrix->m[1][2] = 0;
	matrix->m[2][0] = matrix->m[2][1] = 0;
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

#if 0
	object = GetNextFreeObject();
	assert(object != NULL);
	MakeLivingObjectOfTimFile(object, &MascotsTextureInfo, -50, -20);
	printf("id: %d\n", object->id);

	object = GetNextFreeObject();
	assert(object != NULL);
	MakeLivingObjectOfTimFile(object, &DotsTextureInfo, -50, 80);
	printf("id: %d\n", object->id);

	object = GetNextFreeObject();
	assert(object != NULL);
	MakeLivingObjectOfTimFile(object, &FireTextureInfo, 20, 80);
	printf("id: %d\n", object->id);

	object = GetNextFreeObject();
	assert(object != NULL);
	MakeLivingObjectOfTimFile(object, &WhiteTextureInfo, 100, 80);
	printf("id: %d\n", object->id);
#endif

	object = GetNextFreeObject();
	assert(object != NULL);
	MakeLivingObjectOfTimFile(object, &StripedOne, 0, 0);
	printf("id: %d\n", object->id);

	//PrintClut (MascotsTextureInfo.cx, MascotsTextureInfo.cy, 8);
   	//PrintClut (DotsTextureInfo.cx, DotsTextureInfo.cy, 4);
	//PrintClut (FireTextureInfo.cx, FireTextureInfo.cy, 4);
	PrintClut (StripedOne.cx, StripedOne.cy, 8);
}






void MakeLivingObjectOfTimFile (ObjectHandler* object, GsIMAGE* imageInfo, int x, int y)
{
	object->alive = TRUE;

	object->imageInfo = imageInfo;
	LinkSpriteToImageInfo (&object->sprite, object->imageInfo);

	object->position.vx = x;
	object->position.vy = y;
}



void MoveAllObjects (VECTOR* movement)
{
	int i;

	for (i = 0; i < MAX_OBJECTS; i++)
		{
		ObjectArray[i].position.vx += movement->vx;
		ObjectArray[i].position.vy += movement->vy;
		}
}




void RotateAllObjects (int amount, int whichWay)
{
	int i;
	int addAngle;

	switch(whichWay)
		{
		case CLOCKWISE:		
			addAngle = amount;		
			break;
		case ANTICLOCKWISE:		
			addAngle = -amount;		
			break;
		default:			
			assert(FALSE);
		}

	for (i = 0; i < MAX_OBJECTS; i++)
		{
		ObjectArray[i].sprite.rotate += addAngle;
		}
}




void ScaleAllObjects (int xMultiplier, int yMultiplier)
{
	int i;

	for (i = 0; i < MAX_OBJECTS; i++)
		{
		ObjectArray[i].scaleX = (ObjectArray[i].scaleX * xMultiplier) >> 12;
		ObjectArray[i].scaleY = (ObjectArray[i].scaleY * yMultiplier) >> 12;
		}
}





void UpdateAllObjects (void)
{
	int i;
	GsSPRITE* sprite;
	VECTOR* position;
	ObjectHandler* object;

	for (i = 0; i < MAX_OBJECTS; i++)
		{
		sprite = &ObjectArray[i].sprite;
		position = &ObjectArray[i].position;
		object = &ObjectArray[i];

		sprite->x = position->vx;
		sprite->y = position->vy;

		sprite->scalex = object->scaleX;
		sprite->scaley = object->scaleY;
		}
}





void PrintClut (int x, int y, int bitMode)
{
	u_short clutBuffer1[256];
	u_short clutBuffer2[16];
	RECT rect;
	int i;

	assert(x >= 0);
	assert(x < 1024);
	assert(y >= 0);
	assert(y < 512);
	assert( (bitMode == 4) || (bitMode == 8) );

	printf("Printing all CLUT values for clut\n");
	printf("At %d %d, bit mode %d\n", x, y, bitMode);

	switch(bitMode)
		{
		case 4:
			setRECT( &rect, x, y, 16, 1);
			for (i = 0; i < 16; i++)
				clutBuffer2[i] = 0;
			StoreImage( &rect, (unsigned long *) clutBuffer2);
			for (i = 0; i < 16; i++)
				{
				printf("%d\n", clutBuffer2[i]);
				}
			break;
		case 8:
			setRECT( &rect, x, y, 256, 1);
			for (i = 0; i < 256; i++)
				clutBuffer1[i] = 0;
			StoreImage( &rect, (unsigned long *) clutBuffer1);
			for (i = 0; i < 256; i++)
				{
				printf("%d\n", clutBuffer1[i]);
				}
			break;
		default:
			assert(FALSE);
		}
}





#define CYCLE_PERIOD (16 * 10)

void CycleDotsClut (void)
{
	static u_short permanentStore[16];
	u_short buffer[16];
	static int firstTimeFlag = TRUE;
	RECT rect;
	int i;

	cyclePoint = frameNumber % CYCLE_PERIOD;
	cyclePhase = cyclePoint * 16 / CYCLE_PERIOD;

	setRECT( &rect, DotsTextureInfo.cx, DotsTextureInfo.cy, 16, 1);

	if (firstTimeFlag == TRUE)
		{
		StoreImage( &rect, (u_long*) permanentStore);
		firstTimeFlag = FALSE;
		}

	#if 0		// OLD: just one cell on at any one time
	for (i = 0; i < 16; i++)
		buffer[i] = 0;
	buffer[cyclePhase] = permanentStore[cyclePhase];
	#endif

		// buffer is permanentStore shifted to the right, with wraparound
	for (i = cyclePhase; i < 16; i++)
		{
		buffer[i] = permanentStore[i - cyclePhase];
		}
	for (i = 0; i < cyclePhase; i++)
		{
		buffer[i] = permanentStore[(16-cyclePhase)+i];
		}

	LoadImage( &rect, (u_long*) buffer);

	DrawSync(0);
}





#define CYCLE_PERIOD2 (256 * 5)

void CycleMascotsClut (void)
{
	static u_short permanentStore[256];
	u_short buffer[256];
	static int firstTimeFlag = TRUE;
	RECT rect;
	int i;

	cyclePoint2 = frameNumber % CYCLE_PERIOD2;
	cyclePhase2 = cyclePoint2 * 256 / CYCLE_PERIOD2;

	setRECT( &rect, MascotsTextureInfo.cx, MascotsTextureInfo.cy, 256, 1);

	if (firstTimeFlag == TRUE)
		{
		StoreImage( &rect, (u_long*) permanentStore);
		firstTimeFlag = FALSE;
		}

	#if 0		// OLD: just one cell on at any one time
	for (i = 0; i < 256; i++)
		buffer[i] = 0;
	buffer[cyclePhase2] = permanentStore[cyclePhase2];
	#endif

		// buffer is permanentStore shifted to the right, with wraparound
	for (i = cyclePhase2; i < 256; i++)
		{
		buffer[i] = permanentStore[i - cyclePhase2];
		}
	for (i = 0; i < cyclePhase2; i++)
		{
		buffer[i] = permanentStore[(256-cyclePhase2)+i];
		}

	LoadImage( &rect, (u_long*) buffer);

	DrawSync(0);
}


	

#define CYCLE_PERIOD3 (16 * 10)

void CycleFireClut (void)
{
	static u_short permanentStore[16];
	u_short buffer[16];
	static int firstTimeFlag = TRUE;
	RECT rect;
	int i;

	cyclePoint3 = frameNumber % CYCLE_PERIOD3;
	cyclePhase3 = cyclePoint3 * 16 / CYCLE_PERIOD3;

	setRECT( &rect, FireTextureInfo.cx, FireTextureInfo.cy, 16, 1);

	if (firstTimeFlag == TRUE)
		{
		StoreImage( &rect, (u_long*) permanentStore);
		firstTimeFlag = FALSE;
		}

		// buffer is permanentStore shifted to the right, with wraparound
	for (i = cyclePhase3; i < 16; i++)
		{
		buffer[i] = permanentStore[i - cyclePhase3];
		}
	for (i = 0; i < cyclePhase3; i++)
		{
		buffer[i] = permanentStore[(16-cyclePhase3)+i];
		}
	



	LoadImage( &rect, (u_long*) buffer);

	DrawSync(0);
}





int SmallIntToColourPQR (int arg)
{
	switch(arg)	
		{
		case 0: return 0; break;
		case 1: return 8; break;
		case 2: return 16; break;
		case 3: return 31; break;
		default: assert(FALSE);
		}
} 



u_short pixels[64 * 64];
u_short clut[256];
u_long *pixelPointer, *clutPointer;

	  

#if 0			// very simple: 16 bit square
				// first 5 bytes red, next are green, last are blue
				// ie 
				// pixels[index] = 16; 		// medium red
				// pixels[index] = 16 << 5; 		// medium green
				// pixels[index] = 16 << 10; 		// medium blue
void CreateStripedTextureOne (void)
{
	int i, j, index;
	RECT pixelRect;

	for (i = 0; i < 64; i++)
		{
		for (j = 0; j < 64; j++)
			{
			index = (64 * i) + j;
			pixels[index] = 16 << 10;
			}
		}

	StripedOne.pmode = 2;			// 16-bit flag

	setRECT( &pixelRect, 640, 0, 64, 64);
	StripedOne.px = pixelRect.x;
	StripedOne.py = pixelRect.y;
	StripedOne.pw = pixelRect.w;
	StripedOne.ph = pixelRect.h;

	pixelPointer = (u_long*) pixels;
	StripedOne.pixel = (u_long*) pixels;	
	LoadImage( &pixelRect, pixelPointer);

	DrawSync(0);
}
#endif


	  






#if 0

void CreateStripedTextureOne (void)
{
	int i, j, index;
	RECT pixelRect, clutRect;

	for (i = 0; i < 64; i++)
		{
		for (j = 0; j < 64; j++)
			{
			index = (64 * j) + i;		 
			pixels[index] = i % 32;   // clut index is horizontal position
			}						  // i.e. vertical stripes
		}

	for (i = 0; i < 32; i++)
		{
		clut[i] = (8 + i / 4) << 5;		
		}

	StripedOne.pmode = 9;			// 8-bit flag

	setRECT( &pixelRect, 640, 0, 32, 64);
	StripedOne.px = pixelRect.x;
	StripedOne.py = pixelRect.y;
	StripedOne.pw = pixelRect.w;
	StripedOne.ph = pixelRect.h;

	pixelPointer = (u_long*) pixels;
	StripedOne.pixel = (u_long*) pixels;	
	LoadImage( &pixelRect, pixelPointer);

	setRECT( &clutRect, 640, 320, 256, 1);
	StripedOne.cx = clutRect.x;
	StripedOne.cy = clutRect.y;
	StripedOne.cw = clutRect.w;
	StripedOne.ch = clutRect.h;

	clutPointer = (u_long*) clut;
	StripedOne.clut = (u_long*) clut;	
	LoadImage( &clutRect, clutPointer);

	DrawSync(0);
}

#endif








char pixels2[64 * 64];


#if 1	  // for 8-bit, use CHAR pixels: each one just 8 bits
			// for 4-bit, squash 2 pixels into one char

void CreateStripedTextureOne (void)
{
	int i, j, index;
	RECT pixelRect, clutRect;

	for (i = 0; i < 64; i++)
		{
		for (j = 0; j < 64; j++)
			{
			index = (64 * j) + i;		 
			pixels2[index] = i;  
			}						  
		}

	for (i = 0; i < 64; i++)
		{
		if ( i < 32)
			clut[i] = i;
		else
			clut[i] = (i - 32) << 5;		
		}

	StripedOne.pmode = 9;			// 8-bit flag

	setRECT( &pixelRect, 640, 0, 32, 64);
	StripedOne.px = pixelRect.x;
	StripedOne.py = pixelRect.y;
	StripedOne.pw = pixelRect.w;
	StripedOne.ph = pixelRect.h;

	pixelPointer = (u_long*) pixels2;
	StripedOne.pixel = (u_long*) pixels2;	
	LoadImage( &pixelRect, pixelPointer);

	setRECT( &clutRect, 640, 320, 256, 1);
	StripedOne.cx = clutRect.x;
	StripedOne.cy = clutRect.y;
	StripedOne.cw = clutRect.w;
	StripedOne.ch = clutRect.h;

	clutPointer = (u_long*) clut;
	StripedOne.clut = (u_long*) clut;	
	LoadImage( &clutRect, clutPointer);

	DrawSync(0);
}

#endif
	
	