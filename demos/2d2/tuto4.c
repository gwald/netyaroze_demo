/************************************************************
 *															*
 *						Tuto4.c								*
 *			   											    *															*
 *				LPGE     27/01/97							*		
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




#define SCE_TEXTURE_ADDRESS 0x80090000
GsIMAGE SCETextureInfo;

#define WRITING_TEXTURE_ADDRESS 0x800a0000
GsIMAGE WritingTextureInfo;

#define PSX_TEXTURE_ADDRESS 0x800b0000
GsIMAGE PSXTextureInfo;

   

	

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

		 	 
   		  

GsSPRITE TestSprite1, TestSprite2;





typedef struct
{	
	int id;
	int alive;

	GsIMAGE* imageInfo;
	GsSPRITE sprite;
	int zValue;
	
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

void ChangeAllObjectsRGB (int rChange, int gChange, int bChange);

void BounceThirdObjectAround (void);

void DealWithTwoBackgrounds (void);


void UpdateAllObjects (void);


void PrintClut (int x, int y, int bitMode);
		  


/****************************************************************************
					 macros
****************************************************************************/



#define setVECTOR( vector, x, y, z)			\
	(vector)->vx = x, (vector)->vy = y, (vector)->vz = z


#define KeepWithinRange(quantity, min, max)					\
{															\
	if ((quantity) < (min))									\
		(quantity) = (min);									\
	else if ((quantity) > (max))							\
		(quantity) = (max);									\
}




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

		/****
		FntPrint("frame: %d\n", frameNumber);
		FntPrint("rgb: %d %d %d\n",
			ObjectArray[0].sprite.r,
			ObjectArray[0].sprite.g,
			ObjectArray[0].sprite.b); 
		****/				

		frameNumber++;

		DealWithControllerPad();

		BounceThirdObjectAround();

		DealWithTwoBackgrounds();

		UpdateAllObjects();

		GsSetWorkBase((PACKET*)packetArea[side]);

		GsClearOt(0,0,&Wot[side]);

#if 0
			// test sprites
		GsSortSprite( &TestSprite1, &Wot[side], 3);
		GsSortSprite( &TestSprite2, &Wot[side], 3);
#endif

		for (i = 0; i < MAX_OBJECTS; i++)
			{
			object = &ObjectArray[i];
			if (object->alive == TRUE)
				{
				GsSortSprite( &object->sprite, &Wot[side], object->zValue);
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
	long pad;
	static int framesSinceLastModeToggle = 0;
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

#if 0
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
#endif



		// rotation 
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

#if 0				// scaling
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
#endif



#if 0
		// six Rpad controls: change rgb of sprites
	if (pad & PADRup)
		{
		ChangeAllObjectsRGB (speedMultiplier, 0, 0);
		return;
		}
	if (pad & PADRdown)
		{
		ChangeAllObjectsRGB (-speedMultiplier, 0, 0);
		return;
		}
	if (pad & PADRleft)
		{
		ChangeAllObjectsRGB (0, speedMultiplier, 0);
		return;
		}
	if (pad & PADRright)
		{
		ChangeAllObjectsRGB (0, -speedMultiplier, 0);
		return;
		}
	if (pad & PADR1)
		{
		ChangeAllObjectsRGB (0, 0, speedMultiplier);
		return;
		}
	if (pad & PADR2)
		{
		ChangeAllObjectsRGB (0, 0, -speedMultiplier);
		return;
		}
#endif
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

	ProperInitialiseTexture(SCE_TEXTURE_ADDRESS, &SCETextureInfo);
	ProperInitialiseTexture(PSX_TEXTURE_ADDRESS, &PSXTextureInfo);
	ProperInitialiseTexture(WRITING_TEXTURE_ADDRESS, &WritingTextureInfo);




	printf("iseAll: 6\n");

	InitialiseObjects();

	CreateSomeObjects();

		 
	FntLoad( 960, 256);
	FntOpen( 0, 80, 256, 200, 0, 512);
	
	

#if 0
	PrintClut (0, 480, 4);
	PrintClut (0, 481, 8);
	PrintClut (0, 482, 4);
	PrintClut (0, 484, 4);
	PrintClut (0, 485, 4);
	PrintClut (0, 486, 4);	
#endif	  
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
	
   

 	// find coords wrt tpage top left points
void FindTopLeftOfTexturePage (GsIMAGE* imageInfo, 
									int* x, int* y, int* u, int* v)
{
	int testX, testY;
	static int debugCount3 = 0, debugCount4 = 0;

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
	object->zValue = 0;

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

	object = GetNextFreeObject();
	assert(object != NULL);
	MakeLivingObjectOfTimFile(object, &PSXTextureInfo, 0, 0);
	printf("id: %d\n", object->id);
	object->zValue = 20;

	object = GetNextFreeObject();
	assert(object != NULL);
	MakeLivingObjectOfTimFile(object, &SCETextureInfo, 0, 0);
	printf("id: %d\n", object->id);
	object->zValue = 20;

	object = GetNextFreeObject();
	assert(object != NULL);
	MakeLivingObjectOfTimFile(object, &WritingTextureInfo, 0, 0);
	printf("id: %d\n", object->id);
	object->velocity.vx = 1;
	object->velocity.vy = 1;
	object->sprite.r = 96;
	object->sprite.g = 96;
	object->sprite.b = 96;
	object->sprite.attribute |= 1 << 28;
	object->sprite.attribute |= 1 << 30;

	ObjectArray[1].alive = FALSE;
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




void ChangeAllObjectsRGB (int rChange, int gChange, int bChange)
{
	int i;

	for (i = 0; i < MAX_OBJECTS; i++)
		{
		if (ObjectArray[i].sprite.r + rChange <= 255
			&& ObjectArray[i].sprite.r + rChange >= 0)
				ObjectArray[i].sprite.r += rChange;

		if (ObjectArray[i].sprite.g + gChange <= 255
			&& ObjectArray[i].sprite.g + gChange >= 0)
				ObjectArray[i].sprite.g += gChange;

		if (ObjectArray[i].sprite.b + bChange <= 255
			&& ObjectArray[i].sprite.b + bChange >= 0)
				ObjectArray[i].sprite.b += bChange;
		}
}

	



#define WRITING_MIN_X -40
#define WRITING_MAX_X 40
#define WRITING_MIN_Y -115
#define WRITING_MAX_Y 115

void BounceThirdObjectAround (void)
{
	ObjectHandler* object = &ObjectArray[2];
	
		// basic bounce-off-rectangle-walls movement
	if (object->position.vx + object->velocity.vx <= WRITING_MIN_X)
		object->velocity.vx = -object->velocity.vx;

	if (object->position.vx + object->velocity.vx >= WRITING_MAX_X)
		object->velocity.vx = -object->velocity.vx;

	if (object->position.vy + object->velocity.vy <= WRITING_MIN_Y)
		object->velocity.vy = -object->velocity.vy;

	if (object->position.vy + object->velocity.vy >= WRITING_MAX_Y)
		object->velocity.vy = -object->velocity.vy;		
}





#define CYCLE_PERIOD 360

void DealWithTwoBackgrounds (void)
{
	static int cyclePoint;
	int halfCycle, phase;
	ObjectHandler* object;

	cyclePoint = frameNumber % CYCLE_PERIOD;
	halfCycle = cyclePoint % (CYCLE_PERIOD / 2);
	phase = cyclePoint * 2 / CYCLE_PERIOD;

	if (cyclePoint == 0)
		{
		ObjectArray[0].alive = TRUE;
		ObjectArray[1].alive = FALSE;
		}
	else if (cyclePoint == CYCLE_PERIOD/2)
		{
		ObjectArray[0].alive = FALSE;
		ObjectArray[1].alive = TRUE;
		}

	switch(phase)
		{
		case 0:
			object = &ObjectArray[0];
			break;
		case 1:
			object = &ObjectArray[1];
			break;
		default:
			assert(FALSE);
		}

		// rotating
	object->sprite.rotate += ONE;

		// shrinking
	object->scaleX = ONE - (halfCycle * ONE / CYCLE_PERIOD);
	object->scaleY = ONE - (halfCycle * ONE / CYCLE_PERIOD);
	
#if 0
		// brightness reduction over the cycle
	object->sprite.r = 128 - (halfCycle * 128 / CYCLE_PERIOD);
	object->sprite.g = 128 - (halfCycle * 128 / CYCLE_PERIOD);
	object->sprite.b = 128 - (halfCycle * 128 / CYCLE_PERIOD);
#endif
}





void UpdateAllObjects (void)
{
	int i;
	GsSPRITE* sprite;
	VECTOR* position;
	VECTOR* velocity;
	ObjectHandler* object;

	for (i = 0; i < MAX_OBJECTS; i++)
		{
		sprite = &ObjectArray[i].sprite;
		position = &ObjectArray[i].position;
		velocity = &ObjectArray[i].velocity;
		object = &ObjectArray[i];

		position->vx += velocity->vx;
		position->vy += velocity->vy;

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
			StoreImage( &rect, (unsigned long *) clutBuffer2);
			for (i = 0; i < 16; i++)
				{
				printf("%d\n", clutBuffer2[i]);
				}
			break;
		case 8:
			setRECT( &rect, x, y, 256, 1);
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