/***************************************************************
*															   *
* Copyright (C) 1995 by Sony Computer Entertainment			   *
*			All rights Reserved								   *
*															   *
*		 L.Evans 		 					 May 97			   *
*															   *
***************************************************************/


/****************************************************************************
					includes
****************************************************************************/


#include <stdio.h>

#include <libps.h>	

#include "dump.h"

#include "pad.h"

#include "2d1.h"

#include "rand.h"

#include "text_str.h"

#include "address.h"

#include "sound.h"

#include "object.h"







/****************************************************************************
					structures and constants
****************************************************************************/



GsIMAGE FireTextureInfo;






int ScreenWidth = 320;		 
int ScreenHeight = 240;



#define PACKETMAX	2048		/* Max GPU packets */
#define PACKETMAX2	(PACKETMAX*24)

static PACKET packetArea[2][PACKETMAX2]; /* GPU PACKETS AREA */
  

#define OT_LENGTH	9		

static GsOT Wot[2];			/* Handler of OT */
static GsOT_TAG wtags[2][1<<OT_LENGTH];



long ProjectionDistance;
GsRVIEW2 TheView;


GsF_LIGHT TheLights[3];

GsFOGPARAM TheFogging;

int OverallLightMode;






int QuitFlag = FALSE;

int frameNumber = 0;

int cpuLoad, gpuLoad;




	  







	// object types
#define CUBE 0
//#define SQUARE 1

#define MAX_CUBES 1
ObjectHandler TheCubes[MAX_CUBES];

//#define MAX_SQUARES 12
//ObjectHandler TheSquares[MAX_SQUARES];






/****************************************************************************
					prototypes
****************************************************************************/


void main (void);

void InitialiseAll (void);

void InitialiseLighting (void);
void InitialiseView (void);

void InitialiseObjects (void);

void CleanupAndExit (void);

void DealWithControllerPad (void);



void HandleAllObjects (void);

void HandleACube (ObjectHandler* object);
void HandleASquare (ObjectHandler* object);

void CheckCollisions (void);
int ObjectsVeryClose (ObjectHandler* first, ObjectHandler* second);


void UpdateObjectCoordinates (VECTOR* twist,
							VECTOR* position, VECTOR* velocity,
							GsCOORDINATE2* coordSystem, MATRIX* matrix);

void UpdateObjectCoordinates2 (SVECTOR* rotationVector,
							VECTOR* translationVector,
							GsCOORDINATE2* coordSystem);

u_short SortSpriteObjectPosition (ObjectHandler* object);




void StoreScreen (void);


void ResetGraphicResolution (int newScreenWidth, int newScreenHeight);



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

	
#define ALL_ONES 0xffffffff

u_long onlyNthBitOn, onlyNthBitOffMask;

#define TURN_NTH_BIT_OFF(argument, sizeInBits, N)			\
	{														\
	onlyNthBitOn = 1 << (N);								\
	onlyNthBitOffMask = ALL_ONES ^ onlyNthBitOn;			\
	argument &= onlyNthBitOn;								\
	}	


		
  
/****************************************************************************
					functions
****************************************************************************/


void main (void)
{
	int	hsync = 0;
	int bufferIndex;			  // buffer index
	u_short zValue;
	MATRIX tmpls;
	ObjectHandler *object;
	int i;
	
	InitialiseAll();

	bufferIndex = GsGetActiveBuff();

	while (QuitFlag == FALSE)
		{		
		FntPrint("frame %d\n", frameNumber);
		FntPrint("hsync %d\n", hsync);
		FntPrint("~cf00cpuLoad %d\n", cpuLoad);
		FntPrint("gpuLoad %d\n", gpuLoad);
		FntPrint("~cfffcube pos %d %d %d\n", 
			TheCubes[0].coord.coord.t[0],
			TheCubes[0].coord.coord.t[1],
			TheCubes[0].coord.coord.t[2]);
	
		RegisterTextStringForDisplay("right pad to move cube", -150, -100);
		RegisterTextStringForDisplay("right pad plus START to rotate cube", -150, -90);
		RegisterTextStringForDisplay("L1 and L2 to change resolution", -150, -70);
		RegisterTextStringForDisplay("select makes controls act quicker", -150, -50);
		RegisterTextStringForDisplay("start and select to quit", -150, -40);			

		frameNumber++;

		if (frameNumber == 3)
			ResetGraphicResolution(640, 480);

		DealWithControllerPad();

		//if (frameNumber < 20)
		//	{
		//	printf("clip rect:-\n"); dumpRECT( &GsDRAWENV.clip);
		//	printf("offsets: %d and %d\n", GsDRAWENV.ofs[0], GsDRAWENV.ofs[1]);
		//	}

		GsSetRefView2(&TheView);

		HandleSound();

		HandleAllObjects();

		GsSetWorkBase( (PACKET*)packetArea[bufferIndex]);

		GsClearOt(0, 0, &Wot[bufferIndex]);

		DisplayTextStrings (&Wot[bufferIndex]);

		for (i = 0; i < MAX_OBJECTS; i++)
			{
			if (ObjectArray[i] != NULL)
				if (ObjectArray[i]->alive == TRUE)
					{
					object = ObjectArray[i];

					if (object->displayFlag == TMD)
						{
						GsGetLs(&(object->coord), &tmpls);	   // local to screen matrix
								   
						GsSetLightMatrix(&tmpls);
									
						GsSetLsMatrix(&tmpls);
									
						GsSortObject4( &(object->handler), 
								&Wot[bufferIndex], 
									3, getScratchAddr(0));
						}
					else if (object->displayFlag == SPRITE)	  // speed-up here: use a fast flag
						{	  // some can use GsSortFastSprite
						zValue = SortSpriteObjectPosition(object);
						GsSortSprite( &object->sprite, &Wot[bufferIndex], zValue);
						}
					else
						{
						assert(FALSE);
						}
					}
			}

		cpuLoad = VSync(1);
		DrawSync(0);
		gpuLoad = VSync(1);
		hsync = VSync(0);		 
		//ResetGraph(1);
		GsSwapDispBuff();
		GsSortClear(0,0,4,&Wot[bufferIndex]);
		GsDrawOt(&Wot[bufferIndex]);
		bufferIndex = GsGetActiveBuff();
		FntFlush(-1);
		}

	CleanupAndExit();
}






void InitialiseAll (void)
{
	PadInit();

	InitialiseRandomNumbers();

	InitialiseTextStrings();

	InitialiseSound();	   

	GsInitGraph(ScreenWidth, ScreenHeight, GsINTER|GsOFSGPU, 1, 0);
	GsDefDispBuff(0,0,0,ScreenHeight);

	GsInit3D();		   

		// sort our only ordering table
	Wot[0].length=OT_LENGTH;	
	Wot[0].org=wtags[0];	   
	Wot[1].length=OT_LENGTH;
	Wot[1].org=wtags[1];

	GsClearOt(0,0,&Wot[0]);
	GsClearOt(0,0,&Wot[1]);

	InitialiseLighting();

	InitialiseView();

		// sort our single texture and single sprite
	ProperInitialiseTexture(FIRE_TEXTURE_ADDRESS, &FireTextureInfo);
		 
		// sort basic text printing
	FntLoad( 960, 256);
	FntOpen( -120, 0, 256, 200, 0, 512);

	InitialiseObjects();
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

		
		// background lighting
	GsSetAmbient(ONE/2, ONE/2, ONE/2);

  

		// distance colour blending ('fogging')
  	TheFogging.dqa = -960;
	TheFogging.dqb = 5120*5120;
	TheFogging.rfc = 0; 
	TheFogging.gfc = 0; 
	TheFogging.bfc = 0;
	GsSetFogParam( &TheFogging);


		// overall lighting conditions
	OverallLightMode = 0;			// doesn't allow fogging  
	GsSetLightMode(OverallLightMode);
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







void InitialiseObjects (void)
{
	int i;

	InitialiseObjectClass();

	for (i = 0; i < MAX_CUBES; i++)
		{
		InitSingleObject(&TheCubes[i]);

		BringObjectToLife(&TheCubes[i], CUBE, 
			CUBE_MODEL_ADDRESS, 0, NONE);

		RegisterObjectIntoObjectArray(&TheCubes[i]);
		}

#if 0
	for (i = 0; i < MAX_SQUARES; i++)
		{
		InitSingleObject(&TheSquares[i]);

		BringObjectToLife(&TheSquares[i], SQUARE, 
			SQUARE_MODEL_ADDRESS, 0, NONE);

		RegisterObjectIntoObjectArray(&TheSquares[i]);
		}
#endif


	 
	LinkAllObjectsToModelsOrSprites();
	
	LinkAllObjectsToTheirCoordinateSystems();



	for (i = 0; i < MAX_CUBES; i++)
		{
		TheCubes[i].alive = FALSE;		// start off dead
		}
	//for (i = 0; i < MAX_SQUARES; i++)
	//	{
	//	TheSquares[i].alive = FALSE;		// start off dead
	//	}

		// bring alive the first cube, position it ahead of camera
	TheCubes[0].alive = TRUE;
	TheCubes[0].position.vz = 500;
		// this for later variable scaling
	SetObjectScaling( &TheCubes[0], ONE, ONE, ONE);
}








void CleanupAndExit (void)
{
	//StoreScreen2 ( (u_long*)SCREEN_SAVE_ADDRESS, 0, 0, ScreenWidth, ScreenHeight);
	StoreScreen();

	ResetGraph(3);

	CleanUpSound();

		// if this program part of a multiple module,
		// can printf to siocons to tell user to
		// invoke a new batch file, etc
}





void DealWithControllerPad (void)
{
	long pad;
	int controlSpeed;
	int pause = 15;
	static int framesSinceLastFlip = 0;

	pad = PadRead();
	framesSinceLastFlip++;

		// <start> and <select> to quit
	if (pad & PADselect && pad & PADstart)
		{
		QuitFlag = TRUE;
		return;
		}

		// sort how fast controls act
	if (pad & PADselect)
		controlSpeed = 10;
	else
		controlSpeed = 1;


	if (pad & PADL1 && pad & PADL2)		// change graphics resolution simply
		{
		if (framesSinceLastFlip > pause)
			{
			if (ScreenWidth == 320 && ScreenHeight == 240)
				ResetGraphicResolution(640, 480);
			else
				ResetGraphicResolution(320, 240);
			framesSinceLastFlip = 0;
			return;
			}
		}

			 


		// moving the main cube
	if ((pad & PADstart) == 0)
		{
		if (pad & PADRleft)
			{
			TheCubes[0].position.vx -= controlSpeed;
			}
		if (pad & PADRright)
			{
			TheCubes[0].position.vx += controlSpeed;
			}
		if (pad & PADRup)
			{
			TheCubes[0].position.vy -= controlSpeed;
			}
		if (pad & PADRdown)
			{
			TheCubes[0].position.vy += controlSpeed;
			}
		if (pad & PADR1)
			{
			TheCubes[0].position.vz -= controlSpeed;
			}
		if (pad & PADR2)
			{
			TheCubes[0].position.vz += controlSpeed;
			}
		}

		// Start: modifier to rotate the cube
	if (pad & PADstart)
		{
		if (pad & PADRleft)
			{
			TheCubes[0].twist.vy -= 32 * controlSpeed;
			}
		if (pad & PADRright)
			{
			TheCubes[0].twist.vy += 32 * controlSpeed;
			}
		if (pad & PADRup)
			{
			TheCubes[0].twist.vx -= 32 * controlSpeed;
			}
		if (pad & PADRdown)
			{
			TheCubes[0].twist.vx += 32 * controlSpeed;
			}
		if (pad & PADR1)
			{
			TheCubes[0].twist.vz -= 32 * controlSpeed;
			}
		if (pad & PADR2)
			{
			TheCubes[0].twist.vz += 32 * controlSpeed;
			}
		return;
		}
}






void HandleAllObjects (void)
{
	ObjectHandler* object;
	int i;

	//CheckCollisions();

	for (i = 0; i < MAX_OBJECTS; i++)
		{
		if (ObjectArray[i] != NULL)
			{
			if (ObjectArray[i]->alive == TRUE)
				{
				object = ObjectArray[i];

					// updating common to all object types
				object->lifeTime++;
				if (object->canFire == TRUE)
					object->framesSinceLastFire++;
				// HandleCommonUpdating(object);

				switch(object->type)
					{
					case CUBE:
						HandleACube(object);
						break;
					//case SQUARE:
					//	HandleASquare(object);
					//	break;
					default:
						assert(FALSE);
					}
				}
			}
		}
}





void HandleACube (ObjectHandler* object)
{
	assert(object->type == CUBE);

	// OLD
	//UpdateObjectCoordinates2 ( &object->rotate,
	//						&object->position, &object->coord);

	UpdateObjectCoordinates (&object->twist,
							&object->position, &object->velocity,
							&object->coord, &object->matrix);

	SortObjectSize(object);

	if (object->movementMomentumFlag == FALSE)
		{
		setVECTOR( &object->velocity, 0, 0, 0);
		}
	if (object->rotationMomentumFlag == FALSE)
		{
		setVECTOR( &object->twist, 0, 0, 0);
		}
}







#if 0
void HandleASquare (ObjectHandler* object)
{
	assert(object->type == SQUARE);

	UpdateObjectCoordinates2 ( &object->rotate,
							&object->position, &object->coord);
}
#endif



		 



#if 0
void CheckCollisions (void)
{
	int i, j;

	for (i = 0; i < MAX_CUBES; i++)
		{
		if (TheCubes[i].alive == TRUE)
			{
			for (j = 0; j < MAX_SQUARES; j++)
				{
				if (TheSquares[j].alive == TRUE)
					{
					if (ObjectsVeryClose(&TheCubes[i], &TheSquares[j]))
						{
						//printf("cube %d and square %d collision\n", i, j);
						TheCubes[i].currentHealth--;
						TheSquares[j].currentHealth--;
						}
					}
				}
			}
		}
}






	// extremely simple and crude collision detection

#define APPROXIMATE_RADIUS 32

int ObjectsVeryClose (ObjectHandler* first, ObjectHandler* second)
{
	if (abs(first->coord.coord.t[0] - second->coord.coord.t[0]) > APPROXIMATE_RADIUS)
		return FALSE;
	if (abs(first->coord.coord.t[1] - second->coord.coord.t[1]) > APPROXIMATE_RADIUS)
		return FALSE;
	if (abs(first->coord.coord.t[2] - second->coord.coord.t[2]) > APPROXIMATE_RADIUS)
		return FALSE;

	return TRUE; 
}

#endif





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

		// update position by actual movement
	position->vx += realMovement.vx;
	position->vy += realMovement.vy;
	position->vz += realMovement.vz;

	
	if (twist->vz != 0)
		{
		zVector.vx = 0;
		zVector.vy = 0;
		zVector.vz = twist->vz;
			
		RotMatrix(&zVector, &zMatrix);

		MulMatrix0(matrix, &zMatrix, matrix);
		}
	else if (twist->vy != 0)
		{
		yVector.vx = 0;
		yVector.vy = twist->vy;
		yVector.vz = 0;
			
		RotMatrix(&yVector, &yMatrix);

		MulMatrix0(matrix, &yMatrix, matrix);
		}
	else if (twist->vx != 0)
		{
		xVector.vx = twist->vx;
		xVector.vy = 0;
		xVector.vz = 0;
			
		RotMatrix(&xVector, &xMatrix);

		MulMatrix0(matrix, &xMatrix, matrix);
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






#define STORING_SCREEN 0

void StoreScreen (void)
{
#if STORING_SCREEN
	u_long* destination;
	int x, y, w, h;
	RECT rect;

	destination = (u_long *) SCREEN_SAVE_ADDRESS;

	x = y = 0;		// top left of frame buffer

	w = ScreenWidth;
	h = ScreenHeight;

	*(destination+0) = 0x00000010;		/* ID */
	*(destination+1) = 0x00000002;		/* FLAG(15bit Direct,No Clut) */
	*(destination+2) = (w*h/2+3)*4;		/* pixel bnum */
	*(destination+3) = ((0 & 0xffff) << 16) | (640 & 0xffff);
						/* pixel DX,DY: at 640, 0 */
	*(destination+4) = ((h & 0xffff) << 16) | (w & 0xffff);
						/* pixel W,H */

	// NO CLUT since 16-bit mode used

	rect.x = x; 
	rect.y = y;
	rect.w = w; 
	rect.h = h;
	DrawSync(0);
	StoreImage(&rect, destination+5);		

	printf("\n\nPress [F10][F4] for dsave, to get screen picture\n"); 
	printf("Dsave[0]: filename %08x %x\n\n\n", destination, (w*h/2+5)*4);

	DrawSync(0);
	VSync(0);
#endif
}






void ResetGraphicResolution (int newScreenWidth, int newScreenHeight)
{
	int interlaceFlag;

		// stop drawing
	DrawSync(0);
	ResetGraph(1);	   

	assert(newScreenWidth >= 256);
	assert(newScreenWidth <= 640);
	assert(newScreenHeight >= 240);
	assert(newScreenHeight <= 512);

	//printf("clip rect before :-\n"); dumpRECT( &GsDRAWENV.clip);
	//printf("offsets before: %d and %d\n", GsDRAWENV.ofs[0], GsDRAWENV.ofs[1]);

	ScreenWidth = newScreenWidth;
	ScreenHeight = newScreenHeight;

	if (ScreenHeight > 256)
		interlaceFlag = GsINTER;
	else
		interlaceFlag = GsNONINTER;

	GsInitGraph(ScreenWidth, ScreenHeight, interlaceFlag|GsOFSGPU, 1, 0);

	if (ScreenHeight > 256)
		GsDefDispBuff(0,0,0,0);
	else
		GsDefDispBuff(0,0,0,ScreenHeight);

	GsSetOrign(ScreenWidth/2, ScreenHeight/2);

	//printf("clip rect after :-\n"); dumpRECT( &GsDRAWENV.clip);
	//printf("offsets after: %d and %d\n", GsDRAWENV.ofs[0], GsDRAWENV.ofs[1]);
}