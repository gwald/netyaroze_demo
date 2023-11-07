/************************************************************
 *															*
 *						Tuto9.c								*
 *						3-D object manipulation				*
 *			   											    *															*
 *				LPGE     04/12/96							*		
 *															*
 *	Copyright (C) 1996 Sony Computer Entertainment Inc.		*
 *	All Rights Reserved										*
 *															*
 ***********************************************************/


	  


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
#define TEX1_ADDR	0x800a0000	/* Top Address of texture data (TIM FORMAT)*/

 
#define SKY_TEXTURE_ADDRESS 0x800b0000

#define CIRCLE_TEXTURE_ADDRESS 0x800b8000
#define DOTS_TEXTURE_ADDRESS 0x800b9000


#define SHIP_MODEL_ADDRESS 0x80096000
#define CUBE_MODEL_ADDRESS 0x80097000
#define SQUARE_MODEL_ADDRESS 0x80094000


#define OT_LENGTH	9		


static GsOT	Sot[2];			/* Handler of OT */
static GsOT_TAG	stags[2][16];		
static GsOT Wot[2];			/* Handler of OT */
static GsOT_TAG wtags[2][1<<OT_LENGTH]; 


	 
		
static GsRVIEW2 view;	

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



#define	FACT		128

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




ObjectHandler PlayersShip;

#define MAX_BULLETS 50
ObjectHandler TheBullets[MAX_BULLETS];


#define MAX_SHIPS 1
ObjectHandler TheShips[MAX_SHIPS];


#define MAX_CUBES 20
ObjectHandler TheCubes[MAX_CUBES];


#define MAX_SQUARES 400
ObjectHandler TheSquares[MAX_SQUARES];



	// array of pointers for graphical world clipping
ObjectHandler* DisplayedObjects[MAX_OBJECTS];



	// sprites for the circular-awareness drawing
GsSPRITE CircleSprite;
#define CIRCLE_RADIUS 22
#define CIRCLE_CENTRE_X 25
#define CIRCLE_CENTRE_Y 25

GsGLINE line1, line2;

GsSPRITE ShipSprites[MAX_SHIPS];
 	 



	// simple enemy strategies
#define WAITING 0
#define STATIONARY_TRACK 1
#define SEEKING 2
#define INACTIVE 3

#define NUMBER_ENEMY_STRATEGIES 4

  


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



GsSPRITE Background1, Background2;




	// for subdividing squares on plane
int VeryCloseDistance = 2;
int SimpleClipDistance = 5;



GsSPRITE TestSprite1, TestSprite2;



/****************************************************************************
					prototypes
****************************************************************************/


void main (void);

int PerformWorldClipping (ObjectHandler** firstArray, ObjectHandler** secondArray);

void UpdateBackground (void);

void SortShipSprites (GsOT* ot);

void CheckForEndOfLevel (void);
int CountNumberOfEnemiesLeft (void);


void DealWithControllerPad(void);
void PlayerFiresAShot (void);
void EnemyShipFiresAShot (ObjectHandler* enemy);
  


void InitialiseAll(void);

void InitGsSprite (GsSPRITE* sprite);
void InitialiseBackgroundSprite (GsSPRITE* sprite);
void InitialiseCircleSprite (void);
void InitialiseCircleLines (void);
void InitialiseShipSprites (void);

void InitialiseObjects (void);

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


	printf("IN MAIN\n");
	InitialiseAll();


   

	side = GsGetActiveBuff();

	while(1)
		{
		if (QuitFrameNumber == frameNumber)
			break;

		if (frameNumber % 100 == 0)
			CheckForEndOfLevel();			

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
			 
		x = PlayersShip.position.vx / FACT;	// /FACT
		y = PlayersShip.position.vy / FACT;
		z = PlayersShip.position.vz / FACT;
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
		GsSortSprite( &Background1, &Wot[side], (1<<OT_LENGTH)-1);
		GsSortSprite( &Background2, &Wot[side], (1<<OT_LENGTH)-1);

			// scanner: circle, axes, blips
		GsSortFastSprite( &CircleSprite, &Wot[side], 3);
		GsSortGLine( &line1, &Wot[side], 2);
		GsSortGLine( &line2, &Wot[side], 2);
		SortShipSprites(&Wot[side]);
	
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
				if (abs(firstArray[i]->position.vx - x) < (SimpleClipDistance * FACT)
					&& abs(firstArray[i]->position.vz - z) < (SimpleClipDistance * FACT) )
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







int ScannerRange = 5 * FACT;
VECTOR worldZaxis, worldXaxis;	   // world's axes expressed in player's coord system

void SortShipSprites (GsOT* ot)
{
	int i;
	int wx, wz;		// from player to ship, in world coord system
	int ox, oz;		// from player to ship, in player's coord system
	int xInCircle, zInCircle;

	worldZaxis.vx = rsin(PlayersShip.rotate.vy);
	worldZaxis.vy = 0;
	worldZaxis.vz = -rcos(PlayersShip.rotate.vy);

	worldXaxis.vx = -rcos(PlayersShip.rotate.vy);
	worldXaxis.vy = 0;
	worldXaxis.vz = -rsin(PlayersShip.rotate.vy);

	dumpVECTOR(&worldXaxis);
	dumpVECTOR(&worldZaxis);

	for (i = 0; i < MAX_SHIPS; i++)
		{
		if (TheShips[i].alive != TRUE)
			continue;

		wx = TheShips[i].position.vx - PlayersShip.position.vx;
		wz = TheShips[i].position.vz - PlayersShip.position.vz;

		printf("wx %d, wz %d\n", wx, wz);

		ox = ((wx * worldXaxis.vx) + (wz * worldZaxis.vx)) / ONE;
		oz = ((wx * worldXaxis.vz) + (wz * worldZaxis.vz)) / ONE;

		printf("ox %d, oz %d\n", ox, oz);

		if ( ((ox * ox) + (oz * oz)) < (ScannerRange * ScannerRange) )  // if in range
			{
			xInCircle = ox * CIRCLE_RADIUS / ScannerRange;
			zInCircle = oz * CIRCLE_RADIUS / ScannerRange;
				
			printf("xInCircle %d, zInCircle %d\n", xInCircle, zInCircle);		

			ShipSprites[i].x = -SCREEN_WIDTH/2 + CIRCLE_CENTRE_X + 15 + xInCircle - 1;
			ShipSprites[i].y = -SCREEN_HEIGHT/2 + CIRCLE_CENTRE_Y + 15 - zInCircle - 1;

			GsSortFastSprite( &ShipSprites[i], ot, 1);
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
	int speedMultiplier;
  
	pad = PadRead(0);

	
	if (pad & PADselect)
		speedMultiplier = 5;
	else
		speedMultiplier = 1;


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
				= -PlayersShip.shotSpeed	* rsin(PlayersShip.rotate.vy) >> 12;
		TheBullets[bulletID].velocity.vy = 0;
		TheBullets[bulletID].velocity.vz 
				= PlayersShip.shotSpeed	* rcos(PlayersShip.rotate.vy) >> 12;

		   		
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


	InitialiseTexture(TEX1_ADDR);
	InitialiseTexture(SKY_TEXTURE_ADDRESS);
	printf("iseAll: 4.1\n");
	InitialiseTexture(CIRCLE_TEXTURE_ADDRESS);
	InitialiseTexture(DOTS_TEXTURE_ADDRESS);
	printf("iseAll: 4.2\n");
	InitialiseBackgroundSprite( &Background1);
	InitialiseBackgroundSprite( &Background2);
	InitialiseCircleSprite();
	InitialiseCircleLines();
	InitialiseShipSprites();
	printf("iseAll: 5\n");	
	 
	

	printf("iseAll: 6\n");

	InitialiseObjects();
		 
	MainMode = MOVE_SHIP;

	FntLoad( 960, 256);
	FntOpen( 0, 0, 256, 200, 0, 512);		  // 64, 32 as first two args before
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









void InitialiseCircleSprite (void)
{
	InitGsSprite( &CircleSprite);

	CircleSprite.tpage = GetTPage(0, 3, 960, 0);
	CircleSprite.attribute |= 1 << 30;		// semi-trans on
	CircleSprite.attribute |= 1 << 29;		// semi-trans type
	CircleSprite.x = -SCREEN_WIDTH/2 + 15;
	CircleSprite.y = -SCREEN_HEIGHT/2 + 15;
	CircleSprite.w = 52;
	CircleSprite.h = 50;
	CircleSprite.cx = 0;
	CircleSprite.cy = 482;
}




void InitialiseCircleLines (void)
{
		// first one: y axis in circle
	line1.attribute = 0;
	//line1.attribute |= 1 << 30;
	//line1.attribute |= 1 << 29;
	line1.x0 = -SCREEN_WIDTH/2 + 15 + CIRCLE_RADIUS + 3;
	line1.y0 = -SCREEN_HEIGHT/2 + 15 + 3;
	line1.x1 = -SCREEN_WIDTH/2 + 15 + CIRCLE_RADIUS + 3;
	line1.y1 = -SCREEN_HEIGHT/2 + 15 + (2 * CIRCLE_RADIUS) + 3;
	line1.r0 = 128;	line1.g0 = 0; line1.b0 = 0;			// red
	line1.r1 = 0;	line1.g1 = 0; line1.b1 = 128;		// blue

		// first one: x axis in circle
	line2.attribute = 0;
	//line2.attribute |= 1 << 30;
	//line2.attribute |= 1 << 29;
	line2.x0 = -SCREEN_WIDTH/2 + 15 + 3;
	line2.y0 = -SCREEN_HEIGHT/2 + 15 + CIRCLE_RADIUS + 3;
	line2.x1 = -SCREEN_WIDTH/2 + 15 + (2 * CIRCLE_RADIUS) + 3;
	line2.y1 = -SCREEN_HEIGHT/2 + 15 + CIRCLE_RADIUS + 3;
	line2.r0 = 0;	line2.g0 = 128; line2.b0 = 0;			// green
	line2.r1 = 128;	line2.g1 = 0; line2.b1 = 128;		// blue and red
}





void InitialiseShipSprites (void)
{
	int i;

	for (i = 0; i < MAX_SHIPS; i++)
		{
		InitGsSprite( &ShipSprites[i]);
		ShipSprites[i].tpage = GetTPage(0, 0, 960, 0);
		ShipSprites[i].w = 3;
		ShipSprites[i].h = 3;
		ShipSprites[i].u = 4;		
		ShipSprites[i].v = 58;	   // 52 plus 6 -> red cross
		ShipSprites[i].cx = 0;
		ShipSprites[i].cy = 483;
		}
}




void InitialiseObjects (void)
{
	int i;
	int x, z;




	InitialiseObjectClass();
	printf("1\n");
	printf("slots left: %d\n", CountNumberObjectSlotsLeft() );





	InitSingleObject(&PlayersShip);

	BringObjectToLife (&PlayersShip, PLAYER, 
					SHIP_MODEL_ADDRESS, 0, NONE);
	RegisterObjectIntoObjectArray(&PlayersShip);
	printf("slots left: %d\n", CountNumberObjectSlotsLeft() );

	PositionSomewhere(&PlayersShip);
	//PlayersShip.twist.vy = ONE/2;
	PlayersShip.twist.vx = ONE/2;

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
		printf("slots left: %d\n", CountNumberObjectSlotsLeft() );
		printf("i is %d\n", i);
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
		TheShips[i].strategyFlag = INACTIVE;		//rand() % (NUMBER_ENEMY_STRATEGIES-1);

		InitialiseShipAccordingToStrategy(&TheShips[i]);

		PositionSomewhere(&TheShips[i]);
		TheShips[i].allegiance = ENEMY;

		RegisterObjectIntoObjectArray(&TheShips[i]);
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

		TheSquares[i].position.vx = x * FACT;
		TheSquares[i].position.vy = 0;
		TheSquares[i].position.vz = z * FACT;

		UpdateObjectCoordinates2(&TheSquares[i].rotate,  
							&TheSquares[i].position, &TheSquares[i].coord);

		RegisterObjectIntoObjectArray(&TheSquares[i]);
		}
	printf("5.1\n");



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
	printf("8\n");
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

						KeepWithinRange(object->position.vx, 0, (WORLD_MAX_X-1) * FACT);
						KeepWithinRange(object->position.vy, 0, (WORLD_MAX_Y-1) * FACT);
						KeepWithinRange(object->position.vz, 0, (WORLD_MAX_Z-1) * FACT);

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
						gridX = object->position.vx / FACT;	// FACT
						gridY = object->position.vy / FACT;
						gridZ = object->position.vz / FACT;

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

							
						gridX = object->position.vx / FACT;	  // FACT
						gridY = object->position.vy / FACT;
						gridZ = object->position.vz / FACT; 

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

						KeepWithinRange(object->position.vx, 0, (WORLD_MAX_X-1) * FACT);
						KeepWithinRange(object->position.vy, 0, (WORLD_MAX_Y-1) * FACT);
						KeepWithinRange(object->position.vz, 0, (WORLD_MAX_Z-1) * FACT);

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
					case SQUARE:										
						if (abs(object->position.vx - x) < (VeryCloseDistance * FACT)
							&& abs(object->position.vz - z) < (VeryCloseDistance * FACT))
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
	int gridX, gridY;

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












void InitialiseView( void )
{
	GsSetProjection(192);  

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
	TheLights[0].vx = 1; TheLights[0].vy= 1; TheLights[0].vz= 1;

	TheLights[0].r = TheLights[0].g = TheLights[0].b = 0x80;

	GsSetFlatLight(0, &TheLights[0]);


	TheLights[1].vx = -1; TheLights[1].vy= -1; TheLights[1].vz= 1;

	TheLights[1].r = TheLights[1].g = TheLights[1].b = 0x80;

	GsSetFlatLight(1, &TheLights[1]);



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
	object->position.vx = (rand() % WORLD_MAX_X) * FACT;
	object->position.vy = (rand() % WORLD_MAX_Y) * FACT;
	object->position.vz = (rand() % WORLD_MAX_Z) * FACT;
}



void PositionSomewhere (ObjectHandler* object)
{
	object->position.vx = (rand() % WORLD_MAX_X) * FACT;
	object->position.vy = FACT/2;
	object->position.vz = (rand() % WORLD_MAX_Z) * FACT;
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
}


	  



#define OBJECT_SIZE 32

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



