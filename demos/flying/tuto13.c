/************************************************************
 *															*
 *						Tuto13.c							*
 *						3-D object manipulation				*
 *			   											    *															*
 *				LPGE     09/12/96							*		
 *															*
 *	Copyright (C) 1996 Sony Computer Entertainment Inc.		*
 *	All Rights Reserved										*
 *															*
 ***********************************************************/




// changes from tuto12:

// making a 3D object displayable via sprite rather than by 
// three D model

// commented out the 2D business
// 2D business: directly loading into frame buffer at runtime
// CLUT manipulations -> change texture brightness, oscillate colour
// also, create own pictures by mathematical functions
// without need to create TIM file
	  


#include <stdio.h>
#include <rand.h>
#include <libps.h>
#include "sincos.h"
#include "tangent.h"
#include "asincos.h"
#include "atan.h"
#include "pad.h"
#include "tmd.h"
#include "asssert.h"
#include "object.h"
#include "dump.h"







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


#define SCREEN_WIDTH 320				// screen width and height
#define	SCREEN_HEIGHT 240



#define PACKETMAX	2048		/* Max GPU packets */
#define OBJMAX		1200		/* Max Objects */	  // USED TO BE 256
#define PACKETMAX2	(PACKETMAX*24)

	// texture for floor
#define MAIN_TEXTURE_ADDRESS	0x800a0000	/* Top Address of texture data (TIM FORMAT)*/

 
#define SKY_TEXTURE_ADDRESS 0x800b0000

#define CIRCLE_TEXTURE_ADDRESS 0x800b8000
#define DOTS_TEXTURE_ADDRESS 0x800b9000
#define FIRE_TEXTURE_ADDRESS 0x800ba000


#define SHIP_MODEL_ADDRESS 0x80096000
#define CUBE_MODEL_ADDRESS 0x80097000
#define SQUARE_MODEL_ADDRESS 0x80094000
#define SPHERE_MODEL_ADDRESS 0x800c0000


#define OT_LENGTH	9		


static GsOT	Sot[2];			/* Handler of OT */
static GsOT_TAG	stags[2][16];		
static GsOT Wot[2];			/* Handler of OT */
static GsOT_TAG wtags[2][1<<OT_LENGTH]; 


	 
		
static GsRVIEW2 view;

int ProjectionDistance = 192;	

int ViewAdjustment = 20;		


static GsF_LIGHT TheLights[3];		


int ReferenceDistance, ViewpointZDistance, ViewpointYDistance;

static PACKET packetArea[2][PACKETMAX2]; /* GPU PACKETS AREA */

static int nModels;

 

GsFOGPARAM fogparam;

GsLINE line;

GsLINE ArrowLines[3];

GsBOXF box[2];

short lightmode;



#define	GRID_SIZE		128

#define WORLD_MAX_X	20
#define WORLD_MAX_Y	10
#define WORLD_MAX_Z 20


#define	WALL		0x01



	

#define MOVE_SHIP 0
int MainMode;



#ifndef TRUE
	#define TRUE 1
	#define FALSE 0
#endif


int frameNumber = 0;
int QuitFrameNumber = -1;




// object types
#define PLAYER 0
#define BULLET 1
#define ENEMY 2
#define CUBE 3
#define SQUARE 4
#define SPHERE 5
#define FIRE 6




ObjectHandler PlayersShip;

#define MAX_BULLETS 50
ObjectHandler TheBullets[MAX_BULLETS];


#define MAX_SHIPS 6
ObjectHandler TheShips[MAX_SHIPS];


#define MAX_CUBES 20
ObjectHandler TheCubes[MAX_CUBES];


#define MAX_SQUARES 400
ObjectHandler TheSquares[MAX_SQUARES];


#define MAX_SPHERES 24
ObjectHandler TheSpheres[MAX_SPHERES];

#define SPHERE_RADIUS 22


ObjectHandler SingleFire;



	// array of pointers for graphical world clipping
ObjectHandler* DisplayedObjects[MAX_OBJECTS];



	// sprites for the circular-awareness drawing
GsSPRITE OvalSprite;
#define OVAL_HALF_WIDTH 22
#define OVAL_HALF_HEIGHT 17
#define OVAL_CENTRE_X 25
#define OVAL_CENTRE_Y 25

GsSPRITE BlipSprites[MAX_SHIPS];
 	 



	// simple enemy strategies
#define WAITING 0
#define STATIONARY_TRACK 1
#define SEEKING 2
#define INACTIVE 3

#define NUMBER_ENEMY_STRATEGIES 3

  


	// actions
#define SWIVEL_LEFT 0			 
#define SWIVEL_RIGHT 1			 
#define SWIVEL_UP 2				
#define SWIVEL_DOWN 3			
#define TILT_CLOCKWISE 4		
#define TILT_ANTICLOCKWISE 5	
#define MOVE_FORWARDS 6
#define MOVE_BACKWARDS 7
#define FIRING 8




	


	// basic text handling
#define MAX_TEXT_STRINGS 20
#define MAX_STRING_LENGTH 50

char TextStrings[MAX_TEXT_STRINGS][MAX_STRING_LENGTH];	


int LevelNumber = 1;

int NumberEnemiesLeft = MAX_SHIPS;





			 
	 

VECTOR scalingVector;		// for the squares -> smoother walls


	// star field sprites
GsSPRITE Background1, Background2;

	// z value
u_long FurthestBack = (1<<OT_LENGTH)-1;




	// for subdividing squares on plane
int VeryCloseDistance = 2;
int SimpleClipDistance = 5;



GsSPRITE TestSprite1, TestSprite2;

		  




/****************************************************************************
					prototypes
****************************************************************************/


void main (void);

u_short SortSingleFirePosition (void);

void OscillateFloorTexture (void);

void WritePixel (u_long* pixel, int stp, u_long r, u_long g, u_long b);

void CreateTestTexturePage (void);






int PerformWorldClipping (ObjectHandler** firstArray, ObjectHandler** secondArray);

void UpdateBackground (void);

void SortBlipSprites (GsOT* ot);

void CheckForEndOfLevel (void);
int CountNumberOfEnemiesLeft (void);


void DealWithControllerPad(void);
void PlayerFiresAShot (void);
void EnemyShipFiresAShot (ObjectHandler* enemy);
  


void InitialiseAll(void);

void InitialiseBackgroundSprite (GsSPRITE* sprite);
void InitialiseOvalSprite (void);
void InitialiseBlipSprites (void);

void InitialiseObjects (void);

void SortOutFireSprite (void);

void InitialiseShipAccordingToStrategy (ObjectHandler* object);



void HandleAllObjects (void);



void UpdateObjectCoordinates (VECTOR* twist,
							VECTOR* position, VECTOR* velocity,
							GsCOORDINATE2* coordSystem, MATRIX* matrix);
void UpdateObjectCoordinates2 (SVECTOR* rotationVector,
							VECTOR* translationVector,
							GsCOORDINATE2* coordSystem);
void UpdateObjectCoordinates3 (SVECTOR* rotation, VECTOR* twist,
							VECTOR* position, VECTOR* velocity,
							GsCOORDINATE2* coordSystem);
void UpdateObjectCoordinates4 (SVECTOR* rotation, VECTOR* twist,
							VECTOR* position, VECTOR* velocity,
							GsCOORDINATE2* coordSystem);
			


void InitialiseView(void);
void InitialiseLighting(void);




int InitialiseTexture(long addr);
long* InitialiseModel(long* adrs);


  	   
	   
void PositionSomewhereNotInAWall (ObjectHandler* object);
void PositionSomewhere (ObjectHandler* object);


void HandleEnemyBehaviour (ObjectHandler* object);

void HandleWaitingShipsBehaviour (ObjectHandler* object);
void HandleStationaryTrackingShipsBehaviour (ObjectHandler* object);
void HandleSeekingShipBehaviour (ObjectHandler* object);





void HandlePlayersDeath (void);
void HandleEnemysDeath (ObjectHandler* object);

int CheckCollisions (void); 

int ObjectsVeryClose (ObjectHandler* first, ObjectHandler* second);

void ExplodeObject (ObjectHandler* object);

void DummyGuiLoopForTextPrinting (int numberOfStrings, int quitKey);

void ResetAll (void);

void HandleLevelTransition (void);



   
void GetPlusZAxisFromMatrix (MATRIX* matrix, VECTOR* vector);
void GetPlusYAxisFromMatrix (MATRIX* matrix, VECTOR* vector);
void GetPlusXAxisFromMatrix (MATRIX* matrix, VECTOR* vector);


void GetMinusZAxisFromMatrix (MATRIX* matrix, VECTOR* vector);
void GetMinusYAxisFromMatrix (MATRIX* matrix, VECTOR* vector);
void GetMinusXAxisFromMatrix (MATRIX* matrix, VECTOR* vector);



void HandleSphereClass (void);
void HandleSpheresBehaviour (ObjectHandler* sphere);


int CountNumberOfSpheres (void);
int FindNextFreeSphereID (void);
	




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





/****************************************************************************
					 functions
****************************************************************************/








void main( void )
{
	int	i;
	int	hsync = 0;
	GsDOBJ2 *op;
	int side;			  // buffer index
	MATRIX	tmpls, tmplw;
	ObjectHandler** pointer;
	ObjectHandler* object;
	int x, y, z;
	int numberToDisplay;
	u_short singleZ;


	printf("IN MAIN\n");
	InitialiseAll();


   

	side = GsGetActiveBuff();

	while(1)
		{
		if (QuitFrameNumber == frameNumber)
			break;

		// crude brightness change
		//OscillateFloorTexture();

		//if (frameNumber % 100 == 0)
		//	CheckForEndOfLevel();			

		FntPrint("Num models: %d\nHsync = %d\n",nModels,hsync );
		FntPrint("frame: %d\n", frameNumber);
		FntPrint("player health: %d\n", PlayersShip.currentHealth);	 
		FntPrint("player score: %d\n", PlayersShip.meritRating);
		FntPrint("level: %d\n", LevelNumber);

		FntPrint("enemies left: %d\n", NumberEnemiesLeft);
		if (frameNumber % 20 == 0)
			{
			NumberEnemiesLeft = CountNumberOfEnemiesLeft();
			}

		 
				
	
		FntPrint("ship pos: \n%d %d %d\n", 
			PlayersShip.position.vx, 
			PlayersShip.position.vy,
			PlayersShip.position.vz);
			 
		x = PlayersShip.position.vx / GRID_SIZE;	// /GRID_SIZE
		y = PlayersShip.position.vy / GRID_SIZE;
		z = PlayersShip.position.vz / GRID_SIZE;
		FntPrint("ship grid: %d %d %d\n", x, y, z);		 
		
		FntPrint("enemy: %d %d %d\n",
			TheShips[0].position.vx, TheShips[0].position.vy, TheShips[0].position.vz);				

		frameNumber++;

		DealWithControllerPad();
		
		nModels = 0;			

		GsSetRefView2(&view);

		GsSetWorkBase((PACKET*)packetArea[side]);

		GsClearOt(0,0,&Wot[side]);
		GsClearOt(0,0,&Sot[side]);

		UpdateBackground();
		HandleAllObjects();

		numberToDisplay = PerformWorldClipping(ObjectArray, DisplayedObjects);
		FntPrint("num to display: %d\n", numberToDisplay);

			// background star field
		GsSortSprite( &Background1, &Wot[side], FurthestBack);
		GsSortSprite( &Background2, &Wot[side], FurthestBack);

			// test sprite
		//GsSortSprite( &TestSprite1, &Wot[side], 3);

			// scanner: circle, axes, blips
		GsSortFastSprite( &OvalSprite, &Wot[side], 3);

			// object displayed by sprite
		singleZ = SortSingleFirePosition();
		GsSortSprite( &SingleFire.sprite, &Wot[side], 2);

		SortBlipSprites(&Wot[side]);
	
		for (i = 0; i < numberToDisplay; i++) 
			{		
			object = DisplayedObjects[i];

			GsGetLs(&(object->coord), &tmpls);
			   
			GsSetLightMatrix(&tmpls);
				
			GsSetLsMatrix(&tmpls);
				
			GsSortObject4( &(object->handler), 
					&Wot[side], 
						3, getScratchAddr(0));	 
			}


		
#if 0
		pointer = &(ObjectArray[0]);
		for (i = 0; i < MAX_OBJECTS; i++) 
			{
			if (*pointer != NULL)
				{
				if ( (*pointer)->alive == TRUE)
					{
					object = *pointer;

					GsGetLs(&(object->coord), &tmpls);
			   
					GsSetLightMatrix(&tmpls);
				
					GsSetLsMatrix(&tmpls);
				
					GsSortObject4( &(object->handler), 
						&Wot[side], 
							3, getScratchAddr(0));
					}	 
				}
			pointer++;
			}
#endif

			// NOTE: here we sort out proper 3D self-viewer
		//draw_world_maps( &Sot[side] );
		//NewDrawWorldMaps(&Sot[side]);

		hsync = VSync(0);		 // USED TO BE VSYNC(2);
								// VSync(2) probably closer match to real frame rate
		ResetGraph(1);
		GsSwapDispBuff();
		GsSortClear(0,0,4,&Wot[side]);
		GsDrawOt(&Wot[side]);
		GsDrawOt(&Sot[side]);
		side ^= 1;
		FntFlush(-1);
		}

	ResetGraph(3);
}





#define NORMAL_WIDTH 32
#define NORMAL_HEIGHT 32

u_short SortSingleFirePosition (void)
{
	VECTOR screen;
	int visualX, visualY;
	u_short zValue;

	PushMatrix();

	ApplyMatrixLV( &GsWSMATRIX, &SingleFire.position, &screen);

	screen.vx += GsWSMATRIX.t[0];
	screen.vy += GsWSMATRIX.t[1];
	screen.vz += GsWSMATRIX.t[2];

	visualX = NORMAL_WIDTH * ProjectionDistance / screen.vz;
	visualY = NORMAL_HEIGHT * ProjectionDistance / screen.vz;

	SingleFire.sprite.x = ((screen.vx * ProjectionDistance / screen.vz) - visualX/2);
	SingleFire.sprite.y = ((screen.vy * ProjectionDistance / screen.vz) - visualY/2);

	SingleFire.sprite.scalex = ONE * visualX / NORMAL_WIDTH;
	SingleFire.sprite.scaley = ONE * visualY / NORMAL_HEIGHT;

	if (frameNumber % 60 == 0)
		{
		printf("real\n");
		dumpVECTOR(&SingleFire.position);
		printf("screen\n");
		dumpVECTOR(&screen);
		printf("visual x and y: %d and %d\n", visualX, visualY);
		printf("sprite x and y: %d %d\n", SingleFire.sprite.x, SingleFire.sprite.y);
		printf("scale: %d %d\n", SingleFire.sprite.scalex, SingleFire.sprite.scaley);
		}

	PopMatrix();

		// 3D clipping
	if (screen.vz < ProjectionDistance/2)
		SingleFire.sprite.attribute |= GsDOFF;			 // display off
	else
		SingleFire.sprite.attribute = 0;		// ensure 31st bit is OFF

	zValue = (u_short) screen.vz;	 // NOTE: need to adjust this

	return zValue;			   // NOT YET CORRECT
}







u_long originalCLUT[16];
u_long newCLUT[16];



	// note: for the floor texture (MAIN_TEXTURE), CLUT entries 0-7 are nonzero
void OscillateFloorTexture (void)
{
	int cyclePoint = (frameNumber % 240) / 16;
	RECT where;
	int i;
	int r, g, b;

	setRECT( &where, 0, 480, 16, 1);	// 16-colour CLUT at 0, 480 

	if (frameNumber == 0)
		{
		StoreImage( &where, originalCLUT);
		DrawSync(0);

#if 0		// find which entries non-zero
		for (i = 0; i < 16; i++)
			{
			if (originalCLUT[i] != 0)
				{
				printf("entry %d not zero\n", i);
				printf("rgb: %d %d %d\n",
					 ((originalCLUT[i] >> 10) & 31L),
					 ((originalCLUT[i] >> 5) & 31L),
					 (originalCLUT[i] & 31L) );
				}
			}
#endif

		return;
		}

	else
		{

		//if (frameNumber % 60 == 0)	 // just do it
		// NOTE: can load 16 by 1 rectangle every frame
		// with no noticeable change in frame rate
			{
			for (i = 0; i < 16; i++)
				newCLUT[i] = originalCLUT[i];

			LoadImage( &where, newCLUT);
			}

		#if 0		// OLD
		if (frameNumber % 10 != 0)
			return;
		else
			{
			for (i = 0; i < 16; i++)
				{
				r = (originalCLUT[i] >> 10) & 31L;
				g = (originalCLUT[i] >> 5) & 31L;
				b = originalCLUT[i] & 31L;

				assert(r >= 0 && r < 32);
				assert(g >= 0 && g < 32);
				assert(b >= 0 && b < 32);

				assert(cyclePoint >= 0 && cyclePoint < 16);

				// increasing and reducing g and b 
				if (cyclePoint >= 0 && cyclePoint < 4)
					{
					g += 2 * cyclePoint;
					b -= 2 * cyclePoint;
					}
				else if (cyclePoint >= 4 && cyclePoint < 8)
					{
					g += 2 * (8 - cyclePoint);
					b -= 2 * (8 - cyclePoint);
					}
				else if (cyclePoint >= 8 && cyclePoint < 12)
					{
					g -= 2 * cyclePoint;
					b += 2 * cyclePoint;
					}
				else if (cyclePoint >= 12 && cyclePoint < 16)
					{
					g -= 2 * (16 - cyclePoint);
					b += 2 * (16 - cyclePoint);
					}

					// clip
				if (g < 0) g = 0;
				if (g > 31) g = 31;
				if (b < 0) b = 0;
				if (b > 31) b = 31;

				newCLUT[i] = (r << 10) | (g << 5) | b;
				}

			LoadImage( &where, newCLUT);
			}
		#endif
		}
}

	

	

	// writes stp and rgb into single u_long variable
void WritePixel (u_long* pixel, int stp, u_long r, u_long g, u_long b)
{
	assert(r < 32);
	assert(g < 32);
	assert(b < 32);
	assert(stp == 0 || stp == 1);

	*pixel = (stp << 15) | (r << 10) | (g << 5) | b;	
		
	assert(*pixel < 65536);
}





u_long array[64 * 64];
u_long array2[16 * 64];


	// load simple pattern to tpage, create test sprite
void CreateTestTexturePage (void)
{
	RECT rect;
	int x, y;
	u_long r, g, b;
	int stp;
	u_long tempCLUT[16];
	int which; 		// which colour in 16 colour CLUT

	setRECT( &rect, 960, 128, 64, 64);

	for (x = 0; x < 64; x++)
		{
		r = g = b = 24;
		stp = 0;

		switch(x % 3)
			{
			case 0: r = 16; break;
			case 1: g = 16; break;
			case 2: b = 16; break;
			}

		for (y = 0; y < 64; y++)
			{
			WritePixel(&array[(x*64)+y], stp, r, g, b);
			}
		}
	
	LoadImage( &rect, array);
	DrawSync(0);



	InitGsSprite(&TestSprite1);

	TestSprite1.tpage = GetTPage(2, 0, 960, 0);		   // 16-bit
	TestSprite1.attribute |= 1 << 25; 				   //16-bit flag
	TestSprite1.x = -80;
	TestSprite1.y = -80;
	TestSprite1.w = 64;
	TestSprite1.h = 64;
	TestSprite1.u = 0;
	TestSprite1.v = 128;
}	
		
	 
	




int PerformWorldClipping (ObjectHandler** firstArray, ObjectHandler** secondArray)
{
	int numberDisplayedObjects = 0;
	int i;
	ObjectHandler *object, **pointer;
	int x, z;
								
	x = PlayersShip.position.vx;
	z = PlayersShip.position.vz;
	
	for (i = 0; i < MAX_OBJECTS; i++)
		{
		if (firstArray[i] != NULL)
			if (firstArray[i]->alive == TRUE)
				{
				if (abs(firstArray[i]->position.vx - x) < (SimpleClipDistance * GRID_SIZE)
					&& abs(firstArray[i]->position.vz - z) < (SimpleClipDistance * GRID_SIZE) )
						{
						secondArray[numberDisplayedObjects] = firstArray[i];
						numberDisplayedObjects++;
						}
				}
		}
		
	return numberDisplayedObjects;	
}	



	// use two full-screen GsSPRITEs to make a star field
	// that moves as player moves
void UpdateBackground (void)
{
	int angle, twist;

	angle = PlayersShip.rotate.vy;
	twist = PlayersShip.twist.vy;

	if (twist == 0)
		return;

	//printf("angle: %d, twist: %d\n", angle, twist);
	//dumpRECT( &Background1);
	//dumpRECT( &Background2);

	switch(angle)
		{
		case 0: case 1024: case 2048: case 3072:	   // dead-on
			if (twist < 0)			 // left, anticlockwise
				{
				if (Background1.x == -SCREEN_WIDTH/2)
					Background2.x = Background1.x - SCREEN_WIDTH;
				else if (Background2.x == -SCREEN_WIDTH/2)
					Background1.x = Background2.x - SCREEN_WIDTH;
				else
					assert(FALSE);
				}
			else					 // right, clockwise
				{
				if (Background1.x == -SCREEN_WIDTH/2)
					Background2.x = Background1.x + SCREEN_WIDTH;
				else if (Background2.x == -SCREEN_WIDTH/2)
					Background1.x = Background2.x + SCREEN_WIDTH;
				else
					assert(FALSE);
				}
			break;
		}

		// move both
	Background1.x -= twist * SCREEN_WIDTH / 1024;
	Background2.x -= twist * SCREEN_WIDTH / 1024;

	//printf("after updating\n");
	//dumpRECT( &Background1);
	//dumpRECT( &Background2);
}







int ScannerRange = 5 * GRID_SIZE;
VECTOR worldZaxis, worldXaxis;	   // world's axes expressed in player's coord system

void SortBlipSprites (GsOT* ot)
{
	int i;
	int wx, wz;		// from player to ship, in world coord system
	int ox, oz;		// from player to ship, in player's coord system
	int xInCircle, zInCircle;

	worldZaxis.vx = rsin(PlayersShip.rotate.vy);
	worldZaxis.vy = 0;
	worldZaxis.vz = -rcos(PlayersShip.rotate.vy);

	worldXaxis.vx = worldZaxis.vz;
	worldXaxis.vy = 0;
	worldXaxis.vz = -worldZaxis.vx;

	//dumpVECTOR(&worldXaxis);
	//dumpVECTOR(&worldZaxis);

		// firstly, detect ships
	for (i = 0; i < MAX_SHIPS; i++)
		{
		if (TheShips[i].alive != TRUE)
			continue;

		wx = TheShips[i].position.vx - PlayersShip.position.vx;
		wz = TheShips[i].position.vz - PlayersShip.position.vz;

		//printf("wx %d, wz %d\n", wx, wz);

			// now find ships' position in object's coord system
		ox = ((wx * worldXaxis.vx) + (wz * worldZaxis.vx)) / ONE;
		oz = ((wx * worldXaxis.vz) + (wz * worldZaxis.vz)) / ONE;

		//printf("ox %d, oz %d\n", ox, oz);

		if ( ((ox * ox) + (oz * oz)) < (ScannerRange * ScannerRange) )  // if in range
			{
			xInCircle = ox * OVAL_HALF_WIDTH / ScannerRange;
			zInCircle = oz * OVAL_HALF_HEIGHT / ScannerRange;
				
			//printf("xInCircle %d, zInCircle %d\n", xInCircle, zInCircle);		

			BlipSprites[i].x = -SCREEN_WIDTH/2 + OVAL_CENTRE_X + 8 + xInCircle - 2;
			BlipSprites[i].y = -SCREEN_HEIGHT/2 + OVAL_CENTRE_Y + 19 - zInCircle - 2;

			GsSortFastSprite( &BlipSprites[i], ot, 1);
			}
		}

		// secondly, detect spheres
	for (i = 0; i < MAX_SPHERES; i++)
		{
		if (TheSpheres[i].alive != TRUE)
			continue;

		wx = TheSpheres[i].position.vx - PlayersShip.position.vx;
		wz = TheSpheres[i].position.vz - PlayersShip.position.vz;

		//printf("wx %d, wz %d\n", wx, wz);

			// now find ships' position in object's coord system
		ox = ((wx * worldXaxis.vx) + (wz * worldZaxis.vx)) / ONE;
		oz = ((wx * worldXaxis.vz) + (wz * worldZaxis.vz)) / ONE;

		//printf("ox %d, oz %d\n", ox, oz);

		if ( ((ox * ox) + (oz * oz)) < (ScannerRange * ScannerRange) )  // if in range
			{
			xInCircle = ox * OVAL_HALF_WIDTH / ScannerRange;
			zInCircle = oz * OVAL_HALF_HEIGHT / ScannerRange;
				
			//printf("xInCircle %d, zInCircle %d\n", xInCircle, zInCircle);		

			BlipSprites[i].x = -SCREEN_WIDTH/2 + OVAL_CENTRE_X + 8 + xInCircle - 2;
			BlipSprites[i].y = -SCREEN_HEIGHT/2 + OVAL_CENTRE_Y + 19 - zInCircle - 2;

			GsSortFastSprite( &BlipSprites[i], ot, 1);
			}
		}
}
		   





void CheckForEndOfLevel (void)
{
	int i;
	int boolean = TRUE;

	for (i = 0; i < MAX_SHIPS; i++)
		{
		if (TheShips[i].alive == TRUE)
			{
			boolean = FALSE;
			break;
			}
		}
	
	if (boolean == TRUE)
		HandleLevelTransition();
}




int CountNumberOfEnemiesLeft (void)
{
	int count = 0;
	int i;

	for (i = 0; i < MAX_SHIPS; i++)
		{
		if (TheShips[i].alive == TRUE)
			{
			count++;
			}
		}

	return count;
}



				




void DealWithControllerPad (void)
{
	long	pad;
	static int framesSinceLastModeToggle = 0;
	int speedMultiplier = 1;
  
	pad = PadRead(0);

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


#if 0
	if (pad & PADselect)
		speedMultiplier = 5;
	else
		speedMultiplier = 1;
#endif

	if (pad & PADR1)
		{
		if (pad & PADLleft)
			SingleFire.position.vx -= SingleFire.movementSpeed * speedMultiplier;
		if (pad & PADLright)
			SingleFire.position.vx += SingleFire.movementSpeed * speedMultiplier;
		if (pad & PADLup)
			SingleFire.position.vy -= SingleFire.movementSpeed * speedMultiplier;
		if (pad & PADLdown)
			SingleFire.position.vy += SingleFire.movementSpeed * speedMultiplier;
		if (pad & PADL1)
			SingleFire.position.vz -= SingleFire.movementSpeed * speedMultiplier;
		if (pad & PADL2)
			SingleFire.position.vz += SingleFire.movementSpeed * speedMultiplier;
		return;
		}

#if 0		// mucking about with the viewpoint
	if ((frameNumber + 30) % 60 == 0)
		{
		printf("projection: %d\n", ProjectionDistance);
		printf("vpy: %d, vpz: %d\n", view.vpy, view.vpz);
		}
	if (pad & PADselect)
		{
		if (pad & PADLleft)
			view.vpz -= 5;
		if (pad & PADLright)
			view.vpz += 5;
		if (pad & PADLup)
			view.vpy -= 5;
		if (pad & PADLdown)
			view.vpy += 5;
		if (pad & PADL1)
			{
			ProjectionDistance -= 5;
			GsSetProjection(ProjectionDistance);
			}
		if (pad & PADL2)
			{
			ProjectionDistance += 5;
			GsSetProjection(ProjectionDistance);
			}
		return;
		}
#endif



		// actions depend on mode
	switch(MainMode)
		{
		case MOVE_SHIP:
				// movement
			if (pad & PADLup)
				PlayersShip.velocity.vz -= PlayersShip.movementSpeed * speedMultiplier;
			if (pad & PADLdown)
				PlayersShip.velocity.vz += PlayersShip.movementSpeed * speedMultiplier;

				// firing
			if (pad & PADL2)
				{
				if (PlayersShip.framesSinceLastFire > PlayersShip.firingRate)
					{
					PlayerFiresAShot();
					PlayersShip.framesSinceLastFire = 0;
					}
				}
			PlayersShip.framesSinceLastFire++;

				// rotation
			if (pad & PADRleft)
				PlayersShip.twist.vy -= PlayersShip.rotationSpeed * speedMultiplier;
			if (pad & PADRright)
				PlayersShip.twist.vy += PlayersShip.rotationSpeed * speedMultiplier;

				// angle clipping
			if (PlayersShip.rotate.vy < 0)
				PlayersShip.rotate.vy += 4096;
			else if (PlayersShip.rotate.vy > 4095)
				PlayersShip.rotate.vy -= 4096;
			break;
		default:
			assert(FALSE);
		}

	if (pad & PADR1)
		{
		if (pad & PADLleft)
			SingleFire.position.vx -= SingleFire.movementSpeed * speedMultiplier;
		if (pad & PADLright)
			SingleFire.position.vx += SingleFire.movementSpeed * speedMultiplier;
		if (pad & PADLup)
			SingleFire.position.vy -= SingleFire.movementSpeed * speedMultiplier;
		if (pad & PADLdown)
			SingleFire.position.vy += SingleFire.movementSpeed * speedMultiplier;
		if (pad & PADL1)
			SingleFire.position.vz -= SingleFire.movementSpeed * speedMultiplier;
		if (pad & PADL2)
			SingleFire.position.vz += SingleFire.movementSpeed * speedMultiplier;
		}

		// pause
	if (pad & PADstart)
		{
		while (pad & PADstart)
			{
			pad = PadRead(0);
			}
		}
}





VECTOR bulletsVelocityWrtSelf;

void PlayerFiresAShot (void)
{
	int bulletID = -1;
	int i;	

		// find next available cube
	for (i = 0; i < MAX_BULLETS; i++)
		{
		if (TheBullets[i].alive == FALSE)
			{
			bulletID = i;
			break;
			}
		}

	if (bulletID == -1)		// no free shots
		return;
	else
		{
		TheBullets[bulletID].alive = TRUE;
		TheBullets[bulletID].lifeTime = 0;
		TheBullets[bulletID].allegiance = PLAYER;

			// start spinning
		TheBullets[bulletID].rotationMomentumFlag = TRUE;
		TheBullets[bulletID].twist.vx = rand() % 20;
		TheBullets[bulletID].twist.vy = rand() % 20;
			
		
			// send in direction of ship at time of fire
#if 0		  // old 3D version
		bulletsVelocityWrtSelf.vx = 0;
		bulletsVelocityWrtSelf.vy = 0;
		bulletsVelocityWrtSelf.vz = -30;

		ApplyMatrixLV(&PlayersShip.matrix, 
				&bulletsVelocityWrtSelf, &(TheBullets[bulletID].velocity) );
#endif

		TheBullets[bulletID].velocity.vx 
				= PlayersShip.shotSpeed * -rsin(PlayersShip.rotate.vy) >> 12;
		TheBullets[bulletID].velocity.vy = 0;
		TheBullets[bulletID].velocity.vz 
				= PlayersShip.shotSpeed	* -rcos(PlayersShip.rotate.vy) >> 12;

		   		
		TheBullets[bulletID].movementMomentumFlag = TRUE; 
		
					// emitted from nose of ship
		TheBullets[bulletID].position.vx = PlayersShip.position.vx;
		TheBullets[bulletID].position.vy = PlayersShip.position.vy;
		TheBullets[bulletID].position.vz = PlayersShip.position.vz;
		}
}









void EnemyShipFiresAShot (ObjectHandler* enemy)
{
	int bulletID = -1;
	int i;	

	assert(FALSE);				// needs updating

		// find next available cube
	for (i = 0; i < MAX_BULLETS; i++)
		{
		if (TheBullets[i].alive == FALSE)
			{
			bulletID = i;
			break;
			}
		}

	if (bulletID == -1)		// no free shots
		return;
	else
		{
		TheBullets[bulletID].alive = TRUE;
		TheBullets[bulletID].lifeTime = 0;
		TheBullets[bulletID].allegiance = ENEMY;

			// start spinning
		TheBullets[bulletID].rotationMomentumFlag = TRUE;
		TheBullets[bulletID].twist.vx = rand() % 20;
		TheBullets[bulletID].twist.vy = rand() % 20;
	
			// send in direction of ship at time of fire
		bulletsVelocityWrtSelf.vx = 0;
		bulletsVelocityWrtSelf.vy = 0;
		bulletsVelocityWrtSelf.vz = -30;

		ApplyMatrixLV(&enemy->matrix, 
			&bulletsVelocityWrtSelf, &(TheBullets[bulletID].velocity) );
		TheBullets[bulletID].movementMomentumFlag = TRUE;

			// emitted from nose of ship
		TheBullets[bulletID].position.vx = enemy->position.vx;
		TheBullets[bulletID].position.vy = enemy->position.vy;
		TheBullets[bulletID].position.vz = enemy->position.vz;
		}
}



 





 

	 
	 







void InitialiseAll (void)
{
	printf("iseAll: 0\n");
	PadInit(0);

		// scale TMD of square to right size
	scalingVector.vx = 7659;
	scalingVector.vy = 7059;
	scalingVector.vz = 7779;

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

	Sot[0].length=4;
	Sot[0].org=stags[0];
	Sot[1].length=4;
	Sot[1].org=stags[1];

	GsClearOt(0,0,&Wot[0]);
	GsClearOt(0,0,&Wot[1]);
	GsClearOt(0,0,&Sot[0]);
	GsClearOt(0,0,&Sot[1]);
		
	printf("iseAll: 3\n");

	InitialiseView();
	printf("iseAll: 4\n");			
	InitialiseLighting();		   


	InitialiseTexture(MAIN_TEXTURE_ADDRESS);
	InitialiseTexture(SKY_TEXTURE_ADDRESS);
	printf("iseAll: 4.1\n");
	InitialiseTexture(CIRCLE_TEXTURE_ADDRESS);
	InitialiseTexture(DOTS_TEXTURE_ADDRESS);
	printf("iseAll: 4.2\n");
	InitialiseBackgroundSprite( &Background1);
	InitialiseBackgroundSprite( &Background2);
	InitialiseOvalSprite();
	InitialiseBlipSprites();

	InitialiseTexture(FIRE_TEXTURE_ADDRESS);
	printf("iseAll: 5\n");	


#if 0
	printf("before CreateTestTexturePage\n");
	CreateTestTexturePage();
	printf("after CreateTestTexturePage\n");
#endif	
	
	 
	

	printf("iseAll: 6\n");

	InitialiseObjects();
		 
	MainMode = MOVE_SHIP;

	FntLoad( 960, 256);
	FntOpen( 0, 0, 256, 200, 0, 512);		  // 64, 32 as first two args before
}







void InitialiseBackgroundSprite (GsSPRITE* sprite)
{
	InitGsSprite(sprite);

	sprite->tpage = GetTPage(0, 0, 640, 256);
	sprite->x = -SCREEN_WIDTH/2;
	sprite->y = -SCREEN_HEIGHT/2;
	sprite->w = 256;
	sprite->h = 256;
	sprite->scalex = ONE * SCREEN_WIDTH/256;	   // stretch to width 320 -> now full screen
	sprite->cx = 0;
	sprite->cy = 481;
}









void InitialiseOvalSprite (void)
{
	InitGsSprite( &OvalSprite);

	OvalSprite.tpage = GetTPage(1, 0, 960, 0);		// 8-bit texture
	OvalSprite.attribute |= 1 << 30;		// semi-trans on
	OvalSprite.attribute |= 1 << 29;		// semi-trans type
	OvalSprite.attribute |= 1<<24;			// 8-bit texture
	OvalSprite.x = -SCREEN_WIDTH/2 + 8;
	OvalSprite.y = -SCREEN_HEIGHT/2 + 19;
	OvalSprite.w = 48;
	OvalSprite.h = 48;
	OvalSprite.cx = 0;
	OvalSprite.cy = 482;
}



 



	// sprites for showing objects on the scanner
void InitialiseBlipSprites (void)
{
	int i;

	for (i = 0; i < MAX_SHIPS; i++)
		{
		InitGsSprite( &BlipSprites[i]);
		BlipSprites[i].tpage = GetTPage(0, 0, 960, 0);
		BlipSprites[i].w = 2;
		BlipSprites[i].h = 2;
		BlipSprites[i].u = 11;		
		BlipSprites[i].v = 52;	   // 52 plus 0 -> red dot
		BlipSprites[i].cx = 0;
		BlipSprites[i].cy = 483;
		}
}




void InitialiseObjects (void)
{
	int i;
	int x, z;
	int model;




	InitialiseObjectClass();
	printf("1\n");
	//printf("slots left: %d\n", CountNumberObjectSlotsLeft() );





	InitSingleObject(&PlayersShip);

	BringObjectToLife (&PlayersShip, PLAYER, 
					SHIP_MODEL_ADDRESS, 0, NONE);	
	RegisterObjectIntoObjectArray(&PlayersShip);
	//printf("slots left: %d\n", CountNumberObjectSlotsLeft() );

	//PositionSomewhere(&PlayersShip);
	PlayersShip.position.vx = 4 * GRID_SIZE;
	PlayersShip.position.vy = -GRID_SIZE/2;
	PlayersShip.position.vz = 4 * GRID_SIZE;

	

	PlayersShip.initialHealth = 1000;
	PlayersShip.currentHealth = 1000;
	PlayersShip.allegiance = PLAYER;

	PlayersShip.movementSpeed = 22;
	PlayersShip.rotationSpeed = ONE/64;
	PlayersShip.firingRate = 4;
	PlayersShip.framesSinceLastFire = 0;
	PlayersShip.shotSpeed = 30;
	//PlayersShip.movementMomentumFlag = TRUE;

	
	printf("2\n");
		

	for (i = 0; i < MAX_BULLETS; i++)
		{
		//printf("slots left: %d\n", CountNumberObjectSlotsLeft() );
		//printf("i is %d\n", i);
		InitSingleObject(&TheBullets[i]);

		BringObjectToLife(&TheBullets[i], BULLET, 
			CUBE_MODEL_ADDRESS, 0, NONE);

		SetObjectScaling(&TheBullets[i], ONE>>5, ONE>>5, ONE>>5);

		RegisterObjectIntoObjectArray(&TheBullets[i]);
		}
	printf("3\n");



	for (i = 0; i < MAX_CUBES; i++)
		{
		InitSingleObject(&TheCubes[i]);

		BringObjectToLife(&TheCubes[i], CUBE, 
			CUBE_MODEL_ADDRESS, 0, NONE);

		SetObjectScaling(&TheCubes[i], ONE>>3, ONE>>3, ONE>>3);

		RegisterObjectIntoObjectArray(&TheCubes[i]);
		}
	printf("4\n");


	  
		 




	for (i = 0; i < MAX_SHIPS; i++)
		{
		InitSingleObject(&TheShips[i]);

		BringObjectToLife(&TheShips[i], ENEMY, 
			SHIP_MODEL_ADDRESS, 0, NONE);		
				
		TheShips[i].initialHealth = 1;
		TheShips[i].currentHealth = 1; 

			// set strategy
		TheShips[i].strategyFlag = INACTIVE; //rand() % (NUMBER_ENEMY_STRATEGIES-2);	// none inactive

		InitialiseShipAccordingToStrategy(&TheShips[i]);

		PositionSomewhere(&TheShips[i]);
		TheShips[i].allegiance = ENEMY;

		RegisterObjectIntoObjectArray(&TheShips[i]);
		}
	printf("5\n");


	for (i = 0; i < MAX_SPHERES; i++)
		{
		if (i % 3 == 2)		 // model: 0, 1 or 3
			model = 3;
		else
			model = i % 3;

		InitSingleObject(&TheSpheres[i]);

		BringObjectToLife(&TheSpheres[i], SPHERE, 
			SPHERE_MODEL_ADDRESS, model, NONE);		 

		TheSpheres[i].movementSpeed = 4;	
			
		SetObjectScaling(&TheSpheres[i], ONE/4, ONE/4, ONE/4);
		
		RegisterObjectIntoObjectArray(&TheSpheres[i]);
		}
	printf("5\n");





		// this dependent on world size
	for (i = 0; i < MAX_SQUARES; i++)
		{
		InitSingleObject(&TheSquares[i]);

		BringObjectToLife(&TheSquares[i], SQUARE, 
			SQUARE_MODEL_ADDRESS, 0, NONE); 
	
		x = i / WORLD_MAX_X;			
		z = i % WORLD_MAX_Z;

		//printf("x %d, z %d\n", x, z);

		TheSquares[i].position.vx = x * GRID_SIZE;
		TheSquares[i].position.vy = 0;
		TheSquares[i].position.vz = z * GRID_SIZE;

		TheSquares[i].rotate.vx = ONE/2;

		UpdateObjectCoordinates2(&TheSquares[i].rotate,  
							&TheSquares[i].position, &TheSquares[i].coord);

		RegisterObjectIntoObjectArray(&TheSquares[i]);
		}
	printf("5.1\n");



		// SingleFire: object represented by sprite not TMD
	InitSingleObject(&SingleFire);

	BringObjectToLife(&SingleFire, FIRE, 
			SPHERE_MODEL_ADDRESS, 0, NONE);		 

	SingleFire.movementSpeed = 10;
	SingleFire.rotationSpeed = 40;
	
	SingleFire.position.vx = 4 * GRID_SIZE;
	SingleFire.position.vy = -GRID_SIZE/2;
	SingleFire.position.vz = 6 * GRID_SIZE;	

	SortOutFireSprite();
				
	//RegisterObjectIntoObjectArray(SingleFire);
	printf("5.2\n");


	LinkAllObjectsToModels();
	printf("6\n");

	LinkAllObjectsToTheirCoordinateSystems();
	printf("7\n");



	for (i = 0; i < MAX_BULLETS; i++)
		{
		TheBullets[i].alive = FALSE;		// start off dead
		}
	for (i = 0; i < MAX_CUBES; i++)
		{
		TheCubes[i].alive = FALSE;		// start off dead
		}
	for (i = 0; i < MAX_SPHERES; i++)
		{
		TheSpheres[i].alive = FALSE;		// start off dead
		}
	printf("8\n");
}






void SortOutFireSprite (void)
{
	SingleFire.sprite.tpage = GetTPage(0, 0, 960, 0);
	SingleFire.sprite.u = 0;
	SingleFire.sprite.v = 84;
	SingleFire.sprite.cx = 0;
	SingleFire.sprite.cy = 484;
	SingleFire.sprite.w = 32;
	SingleFire.sprite.h = 32;
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
		case INACTIVE:
			break;
		default:
			assert(FALSE);
		}
}




 



void HandleAllObjects (void)
{
	ObjectHandler* object;
	int gridX, gridY, gridZ;
	int i;
	int x, y, z;

	x = PlayersShip.position.vx;
	y = PlayersShip.position.vy;
	z = PlayersShip.position.vz;

	CheckCollisions();

	HandleSphereClass();

	for (i = 0; i < MAX_OBJECTS; i++)
		{
		//printf("HandleAllObjects: i: %d\n", i);
		if (ObjectArray[i] != NULL)
			{
			if (ObjectArray[i]->alive == TRUE)
				{
				object = ObjectArray[i];
				object->lifeTime++;

				switch(object->type)
					{
					case PLAYER:
						#if 0
						UpdateObjectCoordinates(&object->twist, &object->position, 
								&object->velocity, &object->coord, &object->matrix);
						#endif
						UpdateObjectCoordinates3 (&object->rotate, &object->twist,
							&object->position, &object->velocity,
							&object->coord);
							
					   	object->rotate.vx += object->twist.vx;
						object->rotate.vy += object->twist.vy;
						object->rotate.vz += object->twist.vz;
						
						SortObjectSize(object);

						KeepWithinRange(object->position.vx, 0, (WORLD_MAX_X-1) * GRID_SIZE);
						//KeepWithinRange(object->position.vy, 0, -(WORLD_MAX_Y-1) * GRID_SIZE);
						KeepWithinRange(object->position.vz, 0, (WORLD_MAX_Z-1) * GRID_SIZE);

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

						if (object->currentHealth < 1)
							{
							HandlePlayersDeath();
							goto endOfHandleAllObjects;
							}
								
#if 0	  // could use this to display arrows telling player where nearest enemy is
							// this chunk: 
							// expresses the position vector to the enemy ship
							// in Object's Own coordinate system:
							// this makes 3D navigation, pursuit, evasion, etc much easier
						if (frameNumber % 20 == 0)
							{
							VECTOR worldVector, playerVector;

							worldVector.vx = TheShips[0].position.vx - PlayersShip.position.vx;
							worldVector.vy = TheShips[0].position.vy - PlayersShip.position.vy;
							worldVector.vz = TheShips[0].position.vz - PlayersShip.position.vz;

								// minus sign: related to the fact that the TMD iof the ship
								// points toward -z not + z
							playerVector.vx = -(worldVector.vx * PlayersShip.matrix.m[0][0]
											+ worldVector.vy * PlayersShip.matrix.m[1][0]
											+ worldVector.vz * PlayersShip.matrix.m[2][0]) / ONE;

							playerVector.vy = (worldVector.vx * PlayersShip.matrix.m[0][1]
											+ worldVector.vy * PlayersShip.matrix.m[1][1]
											+ worldVector.vz * PlayersShip.matrix.m[2][1]) / ONE;

							playerVector.vz = (worldVector.vx * PlayersShip.matrix.m[0][2]
											+ worldVector.vy * PlayersShip.matrix.m[1][2]
											+ worldVector.vz * PlayersShip.matrix.m[2][2]) / ONE;
												
							printf("world vector player->enemy:\n");
							dumpVECTOR(&worldVector);
							printf("object vector player->enemy:\n");
							dumpVECTOR(&playerVector);
							}
#endif
						break;
					case BULLET:
						UpdateObjectCoordinates2(&object->rotate,  
							&object->position, &object->coord);

						SortObjectSize(object);

							// update angle
						object->rotate.vx += object->twist.vx;
						object->rotate.vy += object->twist.vy;
						object->rotate.vz += object->twist.vz;

							// update position
						object->position.vx += object->velocity.vx;
						object->position.vy += object->velocity.vy;
						object->position.vz += object->velocity.vz;

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

							// if in a wall, kill off
						gridX = object->position.vx / GRID_SIZE;	// GRID_SIZE
						gridY = object->position.vy / GRID_SIZE;
						gridZ = object->position.vz / GRID_SIZE;

							// kill off after short time
						if (object->lifeTime > 50)
							{
							object->alive = FALSE;
							object->lifeTime = 0;
							}

						if (gridX < 0 || gridX >= WORLD_MAX_X
							|| gridY < 0 || gridY >= WORLD_MAX_Y
							|| gridZ < 0 || gridZ >= WORLD_MAX_Z)
							{
							object->alive = FALSE;
							object->lifeTime = 0;
							}
						break;
					case CUBE:
						UpdateObjectCoordinates2(&object->rotate,  
							&object->position, &object->coord);

						SortObjectSize(object);

							// update angle
						object->rotate.vx += object->twist.vx;
						object->rotate.vy += object->twist.vy;
						object->rotate.vz += object->twist.vz;

							// update position
						object->position.vx += object->velocity.vx;
						object->position.vy += object->velocity.vy;
						object->position.vz += object->velocity.vz;

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

							// kill off after short time
						if (object->lifeTime > 25)
							{
							object->alive = FALSE;
							object->lifeTime = 0;
							}

							
						gridX = object->position.vx / GRID_SIZE;	  // GRID_SIZE
						gridY = object->position.vy / GRID_SIZE;
						gridZ = object->position.vz / GRID_SIZE; 

						if (gridX < 0 || gridX >= WORLD_MAX_X
							|| gridY < 0 || gridY >= WORLD_MAX_Y
							|| gridZ < 0 || gridZ >= WORLD_MAX_Z)
							{
							object->alive = FALSE;
							object->lifeTime = 0;
							}
						break;
					case ENEMY:
						HandleEnemyBehaviour(object);

						//object->rotate.vx += object->twist.vx;
						//object->rotate.vy += object->twist.vy;
						//object->rotate.vz += object->twist.vz;

						UpdateObjectCoordinates(&object->twist, &object->position, 
								&object->velocity, &object->coord, &object->matrix);

						SortObjectSize(object);

						KeepWithinRange(object->position.vx, 0, (WORLD_MAX_X-1) * GRID_SIZE);
						//KeepWithinRange(object->position.vy, 0, -(WORLD_MAX_Y-1) * GRID_SIZE);
						KeepWithinRange(object->position.vz, 0, (WORLD_MAX_Z-1) * GRID_SIZE);

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
								
						if (object->currentHealth < 1)
							HandleEnemysDeath(object);
						break;
					case SPHERE:
						HandleSpheresBehaviour(object);
						
						UpdateObjectCoordinates4 (&object->rotate, &object->twist,
							&object->position, &object->velocity,
							&object->coord);
							
					   	object->rotate.vx += object->twist.vx;
						object->rotate.vy += object->twist.vy;
						object->rotate.vz += object->twist.vz;
						
						SortObjectSize(object);

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
						break;
					case SQUARE:										
						if (abs(object->position.vx - x) < (VeryCloseDistance * GRID_SIZE)
							&& abs(object->position.vz - z) < (VeryCloseDistance * GRID_SIZE))
							{
							object->handler.attribute |= GsDIV1;
							//printf("attribute set to subdivide, is now %d\n", object->handler.attribute);
							}
						else
							{
							object->handler.attribute = 0; 
							}
						break;
					default:
						assert(FALSE);
					}
				}
			}
		}

endOfHandleAllObjects:
	;
}



   



VECTOR realMovement;
MATRIX xMatrix, yMatrix, zMatrix;
SVECTOR xVector, yVector, zVector;



	// this function does object-relative movement and rotation
void UpdateObjectCoordinates (VECTOR* twist,
							VECTOR* position, VECTOR* velocity,
							GsCOORDINATE2* coordSystem, MATRIX* matrix)
{	
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






MATRIX tempMatrix;

	// this does world-relative movement and rotation
void UpdateObjectCoordinates2 (SVECTOR* rotationVector,
							VECTOR* translationVector,
							GsCOORDINATE2* coordSystem)
{
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




VECTOR realMovement;


	// this does world-relative rotation and object-relative movement
void UpdateObjectCoordinates3 (SVECTOR* rotation, VECTOR* twist,
							VECTOR* position, VECTOR* velocity,
							GsCOORDINATE2* coordSystem)
{ 
#if 0
		// copy the translating vector (position)
	tempMatrix.t[0] = coordSystem->coord.t[0];
	tempMatrix.t[1] = coordSystem->coord.t[1];
	tempMatrix.t[2] = coordSystem->coord.t[2];
#endif

		// get rotation matrix from rotation vector
	RotMatrix(rotation, &tempMatrix);

		// find the velocity in world coordinate terms
	ApplyMatrixLV(&tempMatrix, velocity, &realMovement);
	   
		// assign new matrix to coordinate system
	coordSystem->coord = tempMatrix;
	
		// update position by actual movement
	position->vx += realMovement.vx;
	position->vy += realMovement.vy;
	position->vz += realMovement.vz;
  
		// set position absolutely	
	coordSystem->coord.t[0] = position->vx;
	coordSystem->coord.t[1] = position->vy;
	coordSystem->coord.t[2] = position->vz;

		// tell GTE that coordinate system has been updated
	coordSystem->flg = 0;
}






	
	// this does world-relative rotation and object-relative movement
	// specific to bouncing sphere
void UpdateObjectCoordinates4 (SVECTOR* rotation, VECTOR* twist,
							VECTOR* position, VECTOR* velocity,
							GsCOORDINATE2* coordSystem)
{ 
		// get rotation matrix from rotation vector
	RotMatrix(rotation, &tempMatrix);

		// find the velocity in world coordinate terms
	ApplyMatrixLV(&tempMatrix, velocity, &realMovement);
	   
		// assign new matrix to coordinate system
	coordSystem->coord = tempMatrix;
	
		// update position by actual movement
	position->vx += realMovement.vx;
	position->vy += realMovement.vy;
	position->vz += realMovement.vz;
		
		// reflecting off the floor
	if (position->vy + SPHERE_RADIUS >= 0)
		{
		position->vy -= 2 * (position->vy + SPHERE_RADIUS);
		velocity->vy = -velocity->vy;
		}
  
		// set position absolutely	
	coordSystem->coord.t[0] = position->vx;
	coordSystem->coord.t[1] = position->vy;
	coordSystem->coord.t[2] = position->vz;

		// tell GTE that coordinate system has been updated
	coordSystem->flg = 0;
}













void InitialiseView( void )
{
	GsSetProjection(ProjectionDistance);  

	ReferenceDistance = 50;
	ViewpointZDistance = 220;
	ViewpointYDistance = -40;

	view.vrx = 0; view.vry = 0; view.vrz = 0; //ReferenceDistance;
	view.vpx = 0; view.vpy = ViewpointYDistance; view.vpz = ViewpointZDistance;
	view.rz = 0;
	view.super = &PlayersShip.coord;

	GsSetRefView2(&view);
}





void InitialiseLighting(void)
{
#if 1
	TheLights[0].vx = 1; TheLights[0].vy= 1; TheLights[0].vz= 1;

	TheLights[0].r = TheLights[0].g = TheLights[0].b = 0x80;

	GsSetFlatLight(0, &TheLights[0]);


	TheLights[1].vx = -1; TheLights[1].vy= -1; TheLights[1].vz= 1;

	TheLights[1].r = TheLights[1].g = TheLights[1].b = 0x80;

	GsSetFlatLight(1, &TheLights[1]);
#endif



#if 0
	TheLights[0].vx = ONE; TheLights[0].vy= 0; TheLights[0].vz= 0;
	TheLights[0].r = 128; TheLights[0].g = 0; TheLights[0].b = 0;

	GsSetFlatLight(0, &TheLights[0]);


	TheLights[1].vx = 0; TheLights[1].vy= ONE; TheLights[1].vz= 0;
	TheLights[1].r = 0; TheLights[1].g = 128; TheLights[0].b = 0;

	GsSetFlatLight(1, &TheLights[1]);


	TheLights[2].vx = 0; TheLights[2].vy= 0; TheLights[2].vz= ONE;
	TheLights[2].r = 0; TheLights[2].g = 0; TheLights[2].b = 128;

	GsSetFlatLight(2, &TheLights[2]);
#endif



	GsSetAmbient(ONE/4, ONE/4, ONE/4);




	lightmode = 0;	// used to be 1  
	GsSetLightMode(lightmode);
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



long* InitialiseModel( long* adrs )
{
	long *dop;

	dop=adrs;
	dop++;
	GsMapModelingData(dop);
	dop++;
	dop++;
	return( dop );
}






	
	 



	// random positioning
void PositionSomewhereNotInAWall (ObjectHandler* object)
{
	int gridX, gridY, gridZ;
	int xOffset, yOffset, zOffset;
	int success = FALSE;
 
		// set position randomly		
	object->position.vx = (rand() % WORLD_MAX_X) * GRID_SIZE;
	object->position.vy = (rand() % WORLD_MAX_Y) * GRID_SIZE;
	object->position.vz = (rand() % WORLD_MAX_Z) * GRID_SIZE;
}



void PositionSomewhere (ObjectHandler* object)
{
	object->position.vx = (rand() % WORLD_MAX_X) * GRID_SIZE;
	object->position.vy = -GRID_SIZE/2;
	object->position.vz = (rand() % WORLD_MAX_Z) * GRID_SIZE;
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
		case INACTIVE:		// do nowt
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

	assert(FALSE);			// needs updating

		// when player's ship comes close,
		// activate the sleeper -> active pursuer

	x = PlayersShip.position.vx - object->position.vx;
	y = PlayersShip.position.vy - object->position.vy;
	z = PlayersShip.position.vz - object->position.vz;

	if (abs(x) < WAITING_PROXIMITY 
		&& abs(y) < WAITING_PROXIMITY
		&& abs(z) < WAITING_PROXIMITY)
		{
		printf("activating sleeper\n");
		object->strategyFlag = SEEKING;
		}

	if (object->lifeTime > object->specialTimeLimit)
		{
		printf("activating sleeper\n");
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

	assert(FALSE);			// needs updating
		
		// vector from object to player in world coordinate terms
	worldVector.vx = PlayersShip.position.vx - object->position.vx;
	worldVector.vy = PlayersShip.position.vy - object->position.vy;
	worldVector.vz = PlayersShip.position.vz - object->position.vz;

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

#if 0		 // DEBUG
	if (frameNumber % 20 == 0)
		{
		printf("position vectors of player from enemy\n");
		printf("first world, then object\n");
		dumpVECTOR( &worldVector);
		dumpVECTOR( &objectVector);
		}

	printf("vector to player:\n");
	dumpVECTOR(&objectVector);
#endif	
	
	

	if ( objectVector.vz < 0
		&& ( ((objectVector.vx * objectVector.vx) + (objectVector.vy * objectVector.vy)) 
			< (NEAR_ENOUGH * NEAR_ENOUGH)) )
				{
				if (abs(objectVector.vz) < TRACKING_PROXIMITY * 4)	 // if close up
					{
					//printf("pointing right way, close up enough\n");
					if (object->framesSinceLastFire > object->firingRate)			// rapid fire
						{
						EnemyShipFiresAShot(object);
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
		
	assert(FALSE);			// needs updating
		
		// vector from object to player in world coordinate terms
	worldVector.vx = PlayersShip.position.vx - object->position.vx;
	worldVector.vy = PlayersShip.position.vy - object->position.vy;
	worldVector.vz = PlayersShip.position.vz - object->position.vz;

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

#if 0		 // DEBUG
	if (frameNumber % 20 == 0)
		{
		printf("position vectors of player from enemy\n");
		printf("first world, then object\n");
		dumpVECTOR( &worldVector);
		dumpVECTOR( &objectVector);
		}

	printf("vector to player:\n");
	dumpVECTOR(&objectVector);
#endif	
	
	

	if ( objectVector.vz < 0
		&& ( ((objectVector.vx * objectVector.vx) + (objectVector.vy * objectVector.vy)) 
			< (NEAR_ENOUGH * NEAR_ENOUGH)) )
				{
				if (abs(objectVector.vz) < TRACKING_PROXIMITY * 4)	 // if close up
					{
					//printf("pointing right way, close up enough\n");
					if (object->framesSinceLastFire > object->firingRate)			
						{
						EnemyShipFiresAShot(object);
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


							
			



			// STUB ONLY
void HandlePlayersDeath (void)
{
	//printf("you have died\n");
	ExplodeObject(&PlayersShip);
	
	//QuitFrameNumber = frameNumber + 70;

	sprintf(TextStrings[0], "You have died\n");
	sprintf(TextStrings[1], "Your score: %d\n", PlayersShip.meritRating);
	sprintf(TextStrings[2], "Press start\nto play again\n");

	DummyGuiLoopForTextPrinting(3, PADstart);

	ResetAll();

	PlayersShip.currentHealth = 10;
	PlayersShip.alive = TRUE;
}




void HandleEnemysDeath (ObjectHandler* object)
{
	PlayersShip.meritRating 
		+= (object->initialHealth * (object->strategyFlag + 1));

	ExplodeObject(object);
}





	// collisions: enemy ships, player's ship, cubic bullets
int CheckCollisions (void)
{
	ObjectHandler* object;
	int i, j;

	for (i = 0; i < MAX_SHIPS; i++)
		{
		if (TheShips[i].alive == TRUE)
			{
			if (ObjectsVeryClose(&TheShips[i], &PlayersShip))
				{
					// should kill both really
				printf("Player and Enemy collision\n");
				PlayersShip.currentHealth--;
				TheShips[i].currentHealth--;
				}
			}
		}

	for (i = 0; i < MAX_SPHERES; i++)
		{
		if (TheSpheres[i].alive == TRUE)
			{
			if (ObjectsVeryClose(&TheSpheres[i], &PlayersShip))
				{
					// should kill both really
				printf("Player and sphere collision\n");
				PlayersShip.currentHealth--;
				TheSpheres[i].alive = FALSE;
				}
			}
		}

	for (i = 0; i < MAX_BULLETS; i++)
		{
		if (TheBullets[i].alive == TRUE && TheBullets[i].allegiance == ENEMY)
			{
			if (ObjectsVeryClose(&TheBullets[i], &PlayersShip))
				{
					// should kill both really
				printf("Player and bullet collision\n");
				PlayersShip.currentHealth--;
				}
			}
		} 

	for (i = 0; i < MAX_SHIPS; i++)
		{
		if (TheShips[i].alive == TRUE)
			{
			for (j = 0; j < MAX_BULLETS; j++)
				{
				if (TheBullets[j].alive == TRUE && TheBullets[j].allegiance == PLAYER)
					{
					if (ObjectsVeryClose(&TheBullets[j], &TheShips[i]))
						{
							// should kill both really
						printf("ship and bullet collision\n");
						printf("cube %d, ship %d\n", j, i);
						TheShips[i].currentHealth--;
						}
					}
				}
			}
		}

	for (i = 0; i < MAX_SPHERES; i++)
		{
		if (TheSpheres[i].alive == TRUE)
			{
			for (j = 0; j < MAX_BULLETS; j++)
				{
				if (TheBullets[j].alive == TRUE && TheBullets[j].allegiance == PLAYER)
					{
					if (ObjectsVeryClose(&TheBullets[j], &TheSpheres[i]))
						{
							// should kill both really
						printf("sphere and bullet collision\n");
						printf("cube %d, sphere %d\n", j, i);
						TheSpheres[i].alive = FALSE;
						}
					}
				}
			}
		}
}


	  



#define OBJECT_SIZE 32	   // very rough and ready

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





void ExplodeObject (ObjectHandler* object)
{
	int cubeID;
	int whichAxis, sign;
	int i, j;
	
	object->alive = FALSE;

		// create 6 cubes; send off in directions: along axes
	for (i = 0; i < 6; i++)
		{
		cubeID = -1;	

			// find next available cube
		for (j = 0; j < MAX_CUBES; j++)
			{
			if (TheCubes[j].alive == FALSE)
				{
				cubeID = j;
				break;
				}
			}

		if (cubeID == -1)		// no free cubes
			continue;
		else
			{
			TheCubes[cubeID].alive = TRUE;
			TheCubes[cubeID].lifeTime = 0;

				// start spinning alot
			TheCubes[cubeID].rotationMomentumFlag = TRUE;
			TheCubes[cubeID].twist.vx = rand() % 80;
			TheCubes[cubeID].twist.vy = rand() % 80;
				
				// emitted from centre of exploding object
			TheCubes[cubeID].position.vx = object->position.vx;
			TheCubes[cubeID].position.vy = object->position.vy;
			TheCubes[cubeID].position.vz = object->position.vz;

			whichAxis = i/2;
			if (i%2 == 0)
				sign = 1;
			else
				sign = -1;

				// send along an axis of the object
				// NOTE: this goes badly askew when ship not parallel to xz plane
			TheCubes[cubeID].movementMomentumFlag = TRUE;
			TheCubes[cubeID].velocity.vx = sign * (object->coord.coord.m[whichAxis][0] >> 9);
			TheCubes[cubeID].velocity.vy = sign * (object->coord.coord.m[whichAxis][1] >> 9);
			TheCubes[cubeID].velocity.vz = sign * (object->coord.coord.m[whichAxis][2] >> 9); 
			}
		}
}



   	

	// a gui loop with two main functions:
	// prints text strings
	// waits for single key to quit

void DummyGuiLoopForTextPrinting (int numberOfStrings, int quitKey)
{
	int	i;
	int	hsync = 0;
	GsDOBJ2 *op;
	int side;			  // buffer index
	int localFrameCounter = 0;
	long pad;

	side = GsGetActiveBuff();

	for (;;)
		{
		localFrameCounter++;

		pad = PadRead(0);

		if (pad & quitKey && localFrameCounter > 60)
			break;		 
	  
		GsSetWorkBase((PACKET*)packetArea[side]);
		GsClearOt(0,0,&Wot[side]);
		GsClearOt(0,0,&Sot[side]);
		
		hsync = VSync(2);
		ResetGraph(1);
		GsSwapDispBuff();
		GsSortClear(0,0,4,&Wot[side]);
		GsDrawOt(&Wot[side]);
		GsDrawOt(&Sot[side]);
		side ^= 1;

		for (i = 0; i < numberOfStrings; i++)
			{
			FntPrint(TextStrings[i]);
			}
		
		FntFlush(-1);
		}	
}





	// Note: should make this dependent on which level:
	// more ships, make them faster, fier more rapidly,
	// change their sizes, strategy distributions, etc etc
void ResetAll (void)
{
	int i;


	assert(FALSE);			// needs updating



	BringObjectToLife (&PlayersShip, PLAYER, 
					SHIP_MODEL_ADDRESS, 0, NONE);
	PositionSomewhereNotInAWall(&PlayersShip);
	PlayersShip.initialHealth = 1000;
	PlayersShip.currentHealth = 1000;
	PlayersShip.allegiance = PLAYER;

	PlayersShip.movementSpeed = 2;
	PlayersShip.rotationSpeed = 55;
	PlayersShip.firingRate = 4;
	PlayersShip.framesSinceLastFire = 0;
	PlayersShip.movementMomentumFlag = TRUE;


		

	for (i = 0; i < MAX_BULLETS; i++)
		{
		InitSingleObject(&TheBullets[i]);

		BringObjectToLife(&TheBullets[i], BULLET, 
			CUBE_MODEL_ADDRESS, 0, NONE);

		SetObjectScaling(&TheBullets[i], ONE>>5, ONE>>5, ONE>>5);
		}



	for (i = 0; i < MAX_CUBES; i++)
		{
		InitSingleObject(&TheCubes[i]);

		BringObjectToLife(&TheCubes[i], CUBE, 
			CUBE_MODEL_ADDRESS, 0, NONE);

		SetObjectScaling(&TheCubes[i], ONE>>3, ONE>>3, ONE>>3);
		}


	  
	for (i = 0; i < MAX_SHIPS; i++)
		{
		InitSingleObject(&TheShips[i]);

		BringObjectToLife(&TheShips[i], ENEMY, 
			SHIP_MODEL_ADDRESS, 0, NONE);

		TheShips[i].initialHealth = 1;
		TheShips[i].currentHealth = 1; 

			// set strategy randomly
		TheShips[i].strategyFlag = rand() % (NUMBER_ENEMY_STRATEGIES-1);

		PositionSomewhereNotInAWall(&TheShips[i]);
		TheShips[i].allegiance = ENEMY;

		TheShips[i].position.vy += 20;
		}



  


	for (i = 0; i < MAX_BULLETS; i++)
		{
		TheBullets[i].alive = FALSE;		// start off dead
		}
	for (i = 0; i < MAX_CUBES; i++)
		{
		TheCubes[i].alive = FALSE;		// start off dead
		}



	InitialiseView();
	InitialiseLighting();


	MainMode = MOVE_SHIP;
}







void HandleLevelTransition (void)
{
	int levelBonus;

	levelBonus = LevelNumber * 50;
	LevelNumber++;			 

	PlayersShip.meritRating += levelBonus;

	sprintf(TextStrings[0], "Finished level %d\n", LevelNumber-1);
	sprintf(TextStrings[1], "level bonus: %d\n", levelBonus);
	sprintf(TextStrings[2], "Your score: %d\n", PlayersShip.meritRating);
	sprintf(TextStrings[3], "Press start for\nthe next level\n");

	DummyGuiLoopForTextPrinting(4, PADstart);

	ResetAll();
}
			






	// NOTE: these below work, but Simpler method if using
	// UpdateObjectCoordinates (object-relative move and rotate):
	// the matrix itself just is the three object-relative axes:
	// the Columns are the x, y and z axes respectively.
	   
void GetPlusZAxisFromMatrix (MATRIX* matrix, VECTOR* vector)
{
	VECTOR relativeMovement;

	relativeMovement.vx = 0;
	relativeMovement.vy = 0;
	relativeMovement.vz = ONE;

	ApplyMatrixLV(matrix, &relativeMovement, vector);
}

void GetPlusYAxisFromMatrix (MATRIX* matrix, VECTOR* vector)
{
	VECTOR relativeMovement;

	relativeMovement.vx = 0;
	relativeMovement.vy = ONE;
	relativeMovement.vz = 0;

	ApplyMatrixLV(matrix, &relativeMovement, vector);
}


void GetPlusXAxisFromMatrix (MATRIX* matrix, VECTOR* vector)
{
	VECTOR relativeMovement;

	relativeMovement.vx = ONE;
	relativeMovement.vy = 0;
	relativeMovement.vz = 0;

	ApplyMatrixLV(matrix, &relativeMovement, vector);
}



void GetMinusZAxisFromMatrix (MATRIX* matrix, VECTOR* vector)
{
	VECTOR relativeMovement;

	relativeMovement.vx = 0;
	relativeMovement.vy = 0;
	relativeMovement.vz = -ONE;

	ApplyMatrixLV(matrix, &relativeMovement, vector);
}

void GetMinusYAxisFromMatrix (MATRIX* matrix, VECTOR* vector)
{
	VECTOR relativeMovement;

	relativeMovement.vx = 0;
	relativeMovement.vy = -ONE;
	relativeMovement.vz = 0;

	ApplyMatrixLV(matrix, &relativeMovement, vector);
}


void GetMinusXAxisFromMatrix (MATRIX* matrix, VECTOR* vector)
{
	VECTOR relativeMovement;

	relativeMovement.vx = -ONE;
	relativeMovement.vy = 0;
	relativeMovement.vz = 0;

	ApplyMatrixLV(matrix, &relativeMovement, vector);
}





#define SPHERE_CREATION_RATE 100


void HandleSphereClass (void)
{
	int id;
	ObjectHandler* sphere;
	int angle;

#if 0			  // DEBUG
	if (frameNumber % 15 == 0)
		{
		printf("sphere 0 position\n");
		dumpVECTOR( &TheSpheres[0].position);
		}
#endif

		// from time to time, create a new sphere
	if ( (CountNumberOfSpheres() < MAX_SPHERES / 5) && (rand() % SPHERE_CREATION_RATE == 0) )
		{
		//printf("creating new sphere\n");
		
		id = FindNextFreeSphereID();
		assert(id != -1);

		sphere = &TheSpheres[id];
		sphere->alive = TRUE;

		PositionSomewhere(sphere);
			// start it off at the top of its bounce
		sphere->position.vy = -GRID_SIZE - SPHERE_RADIUS;

			// set it moving on xz plane
		angle = rand() % ONE;
		sphere->velocity.vx = sphere->movementSpeed * rsin(angle) >> 12;
		sphere->velocity.vy = 0;
		sphere->velocity.vz = sphere->movementSpeed * rcos(angle) >> 12;
		sphere->movementMomentumFlag = TRUE;
		}	
}





#define GRAVITATIONAL_ACCELERATION 1
	// how often it happens, in frames
#define GRAVITATIONAL_FREQUENCY	2	

void HandleSpheresBehaviour (ObjectHandler* sphere)
{
	int x, z;
		
	x = sphere->position.vx;
	z = sphere->position.vz;

		// if over edge of world, bounce back
	if (x < SPHERE_RADIUS)
		{
		sphere->position.vx -= 2 * (x - SPHERE_RADIUS);
		//sphere->position.vx += rand() % 5;		// small noise to avoid oscillations
		sphere->velocity.vx = -sphere->velocity.vx;
		}
	else if (x > (WORLD_MAX_X * GRID_SIZE)-SPHERE_RADIUS)
		{
		sphere->position.vx -= 2 * (x + SPHERE_RADIUS - (WORLD_MAX_X * GRID_SIZE));
		//sphere->position.vx -= rand() % 5;		// small noise to avoid oscillations
		sphere->velocity.vx = -sphere->velocity.vx;
		}

	if (z < SPHERE_RADIUS) 
		{
		sphere->position.vz -= 2 * (z - SPHERE_RADIUS);
		//sphere->position.vz += rand() % 5;		// small noise to avoid oscillations
		sphere->velocity.vz = -sphere->velocity.vz;
		}
	else if (z > (WORLD_MAX_Z * GRID_SIZE)-SPHERE_RADIUS )
		{
		sphere->position.vz -= 2 * (z + SPHERE_RADIUS - (WORLD_MAX_Z * GRID_SIZE));
		//sphere->position.vz -= rand() % 5;		// small noise to avoid oscillations
		sphere->velocity.vz = -sphere->velocity.vz;
		}

	if (frameNumber % GRAVITATIONAL_FREQUENCY == 0)
		sphere->velocity.vy += GRAVITATIONAL_ACCELERATION;
}
	
	

	// how many are active?
int CountNumberOfSpheres (void)
{
	int i;	  
	int count = 0;

	for (i = 0; i < MAX_SPHERES; i++)
		{
		if (TheSpheres[i].alive == TRUE)
			count++;
		}
	
	return count;
}



int FindNextFreeSphereID (void)
{
	int id = -1;
	int i;

	for (i = 0; i < MAX_SPHERES; i++)
		{
		if (TheSpheres[i].alive != TRUE)
			{
			id = i;
			break;
			}
		}
	
	return id;
}


	

