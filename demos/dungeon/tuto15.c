/************************************************************
 *															*
 *						Tuto15.c							*
 *						3-D object manipulation				*
 *			   											    *															*
 *				LPGE     26/11/96							*		
 *															*
 *	Copyright (C) 1996 Sony Computer Entertainment Inc.		*
 *	All Rights Reserved										*
 *															*
 ***********************************************************/


	   

// change from tuto14:
// attempt to find object-relative positional vectors of other objects
// ie express vector of enemy, from ship,
// in ship's own coordinate system
// not yet successful



#include <stdio.h>
#include <rand.h>
#include <libps.h>
#include "sincos.h"
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


#define PIH 320				// screen width and height
#define	PIV	240



#define PACKETMAX	2048		/* Max GPU packets */
#define OBJMAX		1200		/* Max Objects */	  // USED TO BE 256
#define PACKETMAX2	(PACKETMAX*24)
#define TEX1_ADDR	0x800a0000	/* Top Address of texture data (TIM FORMAT)*/
#define MODEL1_ADDR	0x80090000	/* Top Address of modeling data (TMD FORMAT)*/
#define MODEL2_ADDR	0x80091000	/* Top Address of modeling data (TMD FORMAT)*/
#define MODEL3_ADDR	0x80092000	/* Top Address of modeling data (TMD FORMAT)*/
#define MODEL4_ADDR	0x80093000	/* Top Address of modeling data (TMD FORMAT)*/
#define MODEL5_ADDR	0x80094000	/* Top Address of modeling data (TMD FORMAT)*/
#define MODEL6_ADDR	0x80095000	/* Top Address of modeling data (TMD FORMAT)*/



#define SHIP_MODEL_ADDRESS 0x80096000
#define CUBE_MODEL_ADDRESS 0x80097000


#define OT_LENGTH	9		


static GsOT	Sot[2];			/* Handler of OT */
static GsOT_TAG	stags[2][16];		
static GsOT Wot[2];			/* Handler of OT */
static GsOT_TAG wtags[2][1<<OT_LENGTH]; 
static GsDOBJ2	Models[OBJMAX]; 	/* Array of Object Handler : */
static GsCOORDINATE2 DWorld[OBJMAX];

SVECTOR Rotations[OBJMAX];



	 
		
static GsRVIEW2 view;	

int ViewAdjustment = 20;		


static GsF_LIGHT TheLights[3];		


int ReferenceDistance, ViewpointZDistance, ViewpointYDistance;

static PACKET packetArea[2][PACKETMAX2]; /* GPU PACKETS AREA */

static int nModels;

long* dop1;
long* dop2;
long* dop3;
long* dop4;
long* dop5;
long* dop6;



GsFOGPARAM fogparam;

GsLINE line;

GsLINE ArrowLines[3];

GsBOXF box[2];

short lightmode;



#define	FACT		128
#define WORLD_MAX_X	14
#define WORLD_MAX_Y	14
#define WORLD_MAX_Z 14
#define	WEST		112
#define	POSY		28
#define	WALL		0x01



static char worldmaps[WORLD_MAX_X][WORLD_MAX_Y][WORLD_MAX_Z];
static GsCOORDINATE2 World[WORLD_MAX_X][WORLD_MAX_Y][WORLD_MAX_Z];




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

ObjectHandler PlayersShip;

#define MAX_BULLETS 50
ObjectHandler TheBullets[MAX_BULLETS];


#define MAX_SHIPS 1
ObjectHandler TheShips[MAX_SHIPS];


#define MAX_CUBES 20
ObjectHandler TheCubes[MAX_CUBES];



 	 



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







#define NORTH_8_FOLD 0
#define NORTH_WEST_8_FOLD 1
#define WEST_8_FOLD 2
#define SOUTH_WEST_8_FOLD 3
#define SOUTH_8_FOLD 4
#define SOUTH_EAST_8_FOLD 5
#define EAST_8_FOLD 6
#define NORTH_EAST_8_FOLD 7

		 





int PositionalRebound = 0;
int VelocityRebound = 4;



 
#define RANDOM_WORLD 0
#define SET_WORLD 1
int WorldType;






/****************************************************************************
					prototypes
****************************************************************************/


void main (void);

void CheckForEndOfLevel (void);
int CountNumberOfEnemiesLeft (void);



void NewDrawWorldMaps( GsOT* ot );


void DealWithControllerPad(void);
void PlayerFiresAShot (void);
void EnemyShipFiresAShot (ObjectHandler* enemy);

void move_lighting_spot(void);
void NewMoveLightingSpot (void);


void InitialiseAll(void);
void InitialiseObjects (void);
void InitialiseShipAccordingToStrategy (ObjectHandler* object);

void HandleAllObjects (void);

int CalculateRelativeZ (void);

void UpdateObjectCoordinates (VECTOR* twist,
							VECTOR* position, VECTOR* velocity,
							GsCOORDINATE2* coordSystem, MATRIX* matrix);
void UpdateObjectCoordinates2 (SVECTOR* rotationVector,
							VECTOR* translationVector,
							GsCOORDINATE2* coordSystem);
void UpdateObjectCoordinates3 (SVECTOR* rotation, VECTOR* twist,
							VECTOR* position, VECTOR* velocity,
							GsCOORDINATE2* coordSystem);


void UpdateModelCoords (SVECTOR* rotationVector,
									GsCOORDINATE2* coordSystem);


int CollisionWithWalls (VECTOR* position, VECTOR* movement);	

int NotSafe (CUBOID* rectangle);




void InitialiseView(void);
void InitialiseLighting(void);




int InitialiseTexture(long addr);
long* InitialiseModel(long* adrs);
void GenerateRandomWorld(void);
void GenerateSetWorld (void);



int calc_world_data(void);
int NewCalculateWorldData (void);
int MinOfFour (int a, int b, int c, int d);
int MaxOfFour (int a, int b, int c, int d);





int calc_model_data(int n, int px, int py, int pz, int fact, int atrb);

int collision( int x, int y );


void PrintObject (ObjectHandler* object);

void PositionSomewhereNotInAWall (ObjectHandler* object);


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
				

		frameNumber++;

		DealWithControllerPad();

		if (view.super == WORLD)
			move_lighting_spot();
		else
			NewMoveLightingSpot();

		
		nModels = NewCalculateWorldData();			

		GsSetRefView2(&view);

		GsSetWorkBase((PACKET*)packetArea[side]);

		GsClearOt(0,0,&Wot[side]);
		GsClearOt(0,0,&Sot[side]);


		op = Models;
		for( i=0; i<nModels; i++ ) 
			{
			UpdateModelCoords (&Rotations[i],
									op->coord2);
			GsGetLws(op->coord2,&tmplw, &tmpls);	
			GsSetLightMatrix(&tmplw);	
			GsSetLsMatrix(&tmpls);	   
			GsSortObject4(op,&Wot[side],3,getScratchAddr(0));
			op++;
			}


		HandleAllObjects();

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






	




VECTOR shipDirection;
VECTOR nearestEnemyDirection;
VECTOR goingEast, goingWest;


#if 0		// OLD
	// draw line to indicate angle of ship on plane
void NewDrawWorldMaps( GsOT* ot )
{
	shipDirection.vx = rsin(PlayersShip.rotate.vy);
	shipDirection.vy = 0;
	shipDirection.vz = -rcos(PlayersShip.rotate.vy);

	nearestEnemyDirection.vx = rsin(ExactDirectionToNearestEnemy);
	nearestEnemyDirection.vy = 0;
	nearestEnemyDirection.vz = -rcos(ExactDirectionToNearestEnemy);

	goingEast.vx = nearestEnemyDirection.vz;
	goingEast.vy = 0;
	goingEast.vz = -nearestEnemyDirection.vx;

	goingWest.vx = -nearestEnemyDirection.vz;
	goingWest.vy = 0;
	goingWest.vz = nearestEnemyDirection.vx;
	
	line.attribute = 0;
	line.r = line.g = line.b = 128;

	line.x0 = PIH/2-48; line.y0 = -PIV/2+48;

	line.x1 = line.x0 + ((30 * shipDirection.vx)>>12);
	line.y1 = line.y0 + ((30 * shipDirection.vz)>>12);



	box[1].attribute = 1<<30;	  // semi-transparency
	setRECT( &box[1], line.x0-30, line.y0-29, 64, 64 );
	box[1].r = 128; box[1].g = 128; box[1].b = 128;


	
	ArrowLines[0].r = 255; ArrowLines[0].g = 128; ArrowLines[0].b = 128;
	ArrowLines[0].attribute = 0;
	ArrowLines[0].x0 = 112; ArrowLines[0].y0 = -72;
	ArrowLines[0].x1 = ArrowLines[0].x0 + ((25 * nearestEnemyDirection.vx)>>12);
	ArrowLines[0].y1 = ArrowLines[0].y0 + ((25 * nearestEnemyDirection.vz)>>12);

	ArrowLines[1].r = 255; ArrowLines[1].g = 128; ArrowLines[1].b = 128;
	ArrowLines[1].attribute = 0;
	ArrowLines[1].x0 = ArrowLines[0].x1; ArrowLines[1].y0 = ArrowLines[0].y1;
	ArrowLines[1].x1 = ArrowLines[1].x0 - ((10 * nearestEnemyDirection.vx)>>12) + ((10 * goingWest.vx)>>12);
	ArrowLines[1].y1 = ArrowLines[1].y0 - ((10 * nearestEnemyDirection.vz)>>12) + ((10 * goingWest.vz)>>12);

	ArrowLines[2].r = 255; ArrowLines[2].g = 128; ArrowLines[2].b = 128;
	ArrowLines[2].attribute = 0;
	ArrowLines[2].x0 = ArrowLines[0].x1; ArrowLines[2].y0 = ArrowLines[0].y1;
	ArrowLines[2].x1 = ArrowLines[2].x0 - ((10 * nearestEnemyDirection.vx)>>12) + ((10 * goingEast.vx)>>12);
	ArrowLines[2].y1 = ArrowLines[2].y0 - ((10 * nearestEnemyDirection.vz)>>12) + ((10 * goingEast.vz)>>12);


	GsSortLine( &line, ot, 2 );
	GsSortLine( &ArrowLines[0], ot, 2 );
	GsSortLine( &ArrowLines[1], ot, 2 );
	GsSortLine( &ArrowLines[2], ot, 2 );
	//GsSortBoxFill( &box[0], ot, 2 );
	GsSortBoxFill( &box[1], ot, 2 );
	

#if 0		 // OLD
	line.attribute = 0;
	line.r = line.g = line.b = 128;
	line.x0 = PIH/2-48; line.y0 = -PIV/2+48;
	line.x1 = line.x0 + (rsin( PlayersShip.rotate.vy )/256);
	line.y1 = line.y0 - (rcos( PlayersShip.rotate.vy )/256);

	setRECT( &box[0], line.x0-16, line.y0-16, 32, 32 );
	box[0].x += ( rsin( PlayersShip.rotate.vy )/256 );
	box[0].y -= ( rcos( PlayersShip.rotate.vy )/256 );
	box[0].r = 0; box[0].g = 0; box[0].b = 128;

	box[1].attribute = 1<<30;	  // semi-transparency
	setRECT( &box[1], line.x0-32, line.y0-32, 64, 64 );
	box[1].r = 128; box[1].g = 128; box[1].b = 128;

	GsSortLine( &line, ot, 2 );
	GsSortBoxFill( &box[0], ot, 2 );
	GsSortBoxFill( &box[1], ot, 2 );
#endif
}


#endif






#define	SPEED	2


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
			if (pad & PADRup)
				PlayersShip.twist.vx += PlayersShip.rotationSpeed * speedMultiplier;
			if (pad & PADRdown)
				PlayersShip.twist.vx -= PlayersShip.rotationSpeed * speedMultiplier;
			if (pad & PADRleft)
				PlayersShip.twist.vz += PlayersShip.rotationSpeed * speedMultiplier;
			if (pad & PADRright)
				PlayersShip.twist.vz -= PlayersShip.rotationSpeed * speedMultiplier;

			break;
		default:
			assert(FALSE);
		}

#if 0			// OLD: no more modes
		// select & Lleft: toggles main mode
	if (pad & PADselect && pad & PADLleft)
		{
		if (framesSinceLastModeToggle > 10)
			{
			MainMode = 1 - MainMode;
			framesSinceLastModeToggle = 0;
			}
		}
	framesSinceLastModeToggle++;
#endif


		// select and R2: step slowly
	if (pad & PADselect && pad & PADR2)
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
VECTOR bogus;

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
		bulletsVelocityWrtSelf.vx = 0;
		bulletsVelocityWrtSelf.vy = 0;
		bulletsVelocityWrtSelf.vz = -30;

		ApplyMatrixLV(&PlayersShip.matrix, 
				&bulletsVelocityWrtSelf, &(TheBullets[bulletID].velocity) );
		   		
		TheBullets[bulletID].movementMomentumFlag = TRUE; 
		
					// emitted from nose of ship
		TheBullets[bulletID].position.vx = PlayersShip.position.vx;
		TheBullets[bulletID].position.vy = PlayersShip.position.vy;
		TheBullets[bulletID].position.vz = PlayersShip.position.vz;
		}
}







MATRIX enemyShipMatrix;

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

		RotMatrix(&enemy->rotate, &enemyShipMatrix);
		ApplyMatrixLV(&enemyShipMatrix, 
			&bulletsVelocityWrtSelf, &(TheBullets[bulletID].velocity) );
		TheBullets[bulletID].movementMomentumFlag = TRUE;

			// emitted from nose of ship
		TheBullets[bulletID].position.vx = enemy->position.vx;
		TheBullets[bulletID].position.vy = enemy->position.vy;
		TheBullets[bulletID].position.vz = enemy->position.vz;
		}
}





#if 0

/* è’ìÀîªíË */
int collision (int vx, int vz )
{
	int	wx, wz;

	wx = ((view.vpx+64)+vx);
	wz = ((view.vpz+64)+vz);
	if( wx<0 || wz<0 ) return(1);

	wx = wx/FACT; wz = wz/FACT;
	if( wx>=WORLD_MAX_X || wz<0 || wz>=WORLD_MAX_Y ) return(1);

	return( worldmaps[wx][wz] );
}

#endif





void move_lighting_spot( void )
{
	TheLights[0].r = TheLights[0].g = TheLights[0].b = 0x70;
	TheLights[0].vx = view.vrx - view.vpx;
	TheLights[0].vy = 4096;			 // from above
	TheLights[0].vz = view.vrz - view.vpz;
	GsSetFlatLight(0,&TheLights[0]);

	TheLights[1].r = TheLights[1].g = TheLights[1].b = 0x70;
	TheLights[1].vx = view.vrx - view.vpx;
	TheLights[1].vy = -4096;			 // from below
	TheLights[1].vz = view.vrz - view.vpz;
	GsSetFlatLight(1,&TheLights[1]);
}





VECTOR shipPointingDirection;
   
void NewMoveLightingSpot (void)
{
	GetMinusZAxisFromMatrix( &PlayersShip.matrix, &shipPointingDirection);

	TheLights[0].r = TheLights[0].g = TheLights[0].b = 0x70;
	TheLights[0].vx = shipPointingDirection.vx;
	TheLights[0].vy = shipPointingDirection.vy;
	TheLights[0].vz = shipPointingDirection.vz;
	GsSetFlatLight(0,&TheLights[0]);
}

 


VECTOR p1, p2, p3, p4, p5, p6, p7, p8;
VECTOR radius, easternTgt, westernTgt;	 // 'radius': direction ship points in
VECTOR plusX, plusY, plusZ, minusX, minusY, minusZ;

// NOTE: reason why radius is used with minus signs is because
// objects really point towards -z not towards +z


int NewCalculateWorldData (void)
{	
	int numberOfModels;
	int	x, y, z;
	int px, py, pz;
	int	sx, sy, sz, ex, ey, ez;		  // start x and y; end x and y
	static int ClipDistance = 4 * FACT;
	int viewX, viewY, viewZ;
	int SimpleClipDistance = 3;


#if 1			// OLD: super simple and very poor
	px = PlayersShip.position.vx / FACT;	// /FACT
	py = PlayersShip.position.vy / FACT;
	pz = PlayersShip.position.vz / FACT;

	sx = px - SimpleClipDistance; 
	sy = py - SimpleClipDistance; 
	sz = pz - SimpleClipDistance;

	ex = px + SimpleClipDistance; 
	ey = py + SimpleClipDistance; 
	ez = pz + SimpleClipDistance;

	if( sx<0 ) sx = 0;
	if( sy<0 ) sy = 0;
	if( sz<0 ) sz = 0;
	if( ex>=WORLD_MAX_X ) ex = WORLD_MAX_X-1;
	if( ey>=WORLD_MAX_Y ) ey = WORLD_MAX_Y-1;
	if( ez>=WORLD_MAX_Z ) ez = WORLD_MAX_Z-1;
#endif

 	


#if 0		// NOT YET: proper cuboid world-clipping

		// NOTE: not finished because probably far too slow
		// (not just the /50, but the whole lot)
	GetPlusXAxisFromVector(&PlayersShip.matrix, &plusX);
	GetPlusYAxisFromVector(&PlayersShip.matrix, &plusY);
	GetPlusZAxisFromVector(&PlayersShip.matrix, &plusZ);
	GetMinusXAxisFromVector(&PlayersShip.matrix, &minusX);
	GetMinusYAxisFromVector(&PlayersShip.matrix, &minusY);
	GetMinusZAxisFromVector(&PlayersShip.matrix, &minusZ);	

	viewX = PlayersShip.position.vx + ((plusZ.vx * ViewpointZDistance) / 50);
	viewY = PlayersShip.position.vy + ((plusZ.vy * ViewpointZDistance) / 50);
	viewZ = PlayersShip.position.vz + ((plusZ.vz * ViewpointZDistance) / 50);

	printf("view: %d %d %d\n", viewX, viewY, viewZ);

	p1.vx = viewX + ((plusX.vx * ClipDistance) / 50) + ((plusY.vx * ClipDistance) / 50) + ((plusZ.vx * ClipDistance) / 50) 
#endif	
		

#if 0
		// DEBUG
	if (frameNumber % 10 == 0)	
		{
		printf("px: %d, py: %d, pz: %d\n", px, py, pz);
		printf("sx: %d, sy: %d, sz: %d\n", sx, sy, sz);
		printf("ex: %d, ey: %d, ez: %d\n", ex, ey, ez);
		}
#endif

	
	numberOfModels = 0;				  // for every square in the around-view box of squares
	for (x = sx; x <= ex; x++) 
		{
		for (y = sy; y <= ey; y++) 
			{
			for (z = sz; z <= ez; z++)
				{
				if( abs(px-x)<4 && abs(py-y)<4 && abs(pz-z)<4) 
					{
					numberOfModels = calc_model_data( numberOfModels, x, y, z, FACT, GsDIV1 );
					} 
				else	
					{
					numberOfModels = calc_model_data( numberOfModels, x, y, z, FACT, 0 );
					}

				if (numberOfModels > OBJMAX)
					{
					numberOfModels = OBJMAX;
					printf("Forcibly holding number of models at OBJMAX %d\n", OBJMAX);
					goto end;
					}
				}
			}
		}


end:

	if (frameNumber % 15 == 0)
		printf("About to return num.models: %d\n", numberOfModels);

	return(numberOfModels);
}





int MinOfFour (int a, int b, int c, int d)
{
	int result, t1, t2;

	t1 = min(a,b); t2 = min(c,d);
	result = min(t1, t2);

	return result;
}


int MaxOfFour (int a, int b, int c, int d)
{
	int result, t1, t2;

	t1 = max(a,b); t2 = max(c,d);
	result = max(t1, t2);

	return result;
}






	// calculate models around this single<px,py> square
int calc_model_data (int n, int px, int py, int pz, int fact, int atrb )
{
	if (px < 0 || px >= WORLD_MAX_X
		|| py < 0 || py >= WORLD_MAX_Y
		|| pz < 0 || pz >= WORLD_MAX_Z)
			return n;
				

	if( worldmaps[px][py][pz]&WALL ) return(n);

	if (px==0 || worldmaps[px-1][py][pz]&WALL) {	  // wall to left
		GsInitCoordinate2( &World[px][py][pz], &DWorld[n] );
		DWorld[n].coord.t[0] = -FACT/2;
		DWorld[n].coord.t[1] = 0;
		DWorld[n].coord.t[2] = 0;
		GsLinkObject4((long)dop5, &Models[n],0);
		Models[n].coord2 = &DWorld[n];
		Models[n].attribute = atrb;
		Rotations[n].vx = 0;
		Rotations[n].vy = 0;
		Rotations[n].vz = 3 * ONE/4;
		n++;
	}
   

	if(px==WORLD_MAX_X-1 || worldmaps[px+1][py][pz]&WALL) {	   // wall to right
		GsInitCoordinate2( &World[px][py][pz], &DWorld[n] );
		DWorld[n].coord.t[0] = FACT/2;
		DWorld[n].coord.t[1] = 0;
		DWorld[n].coord.t[2] = 0;
		GsLinkObject4((long)dop5, &Models[n],0);
		Models[n].coord2 = &DWorld[n];
		Models[n].attribute = atrb;
		Rotations[n].vx = 0;
		Rotations[n].vy = 0;
		Rotations[n].vz = ONE/4;
		n++;
	}
	

	if (py==0 || worldmaps[px][py-1][pz]&WALL) {			 // wall above
		GsInitCoordinate2( &World[px][py][pz], &DWorld[n] );
		DWorld[n].coord.t[0] = 0;
		DWorld[n].coord.t[1] = -FACT/2;
		DWorld[n].coord.t[2] = 0;
		GsLinkObject4((long)dop5, &Models[n],0);
		Models[n].coord2 = &DWorld[n];
		Models[n].attribute = atrb;
		Rotations[n].vx = 0;
		Rotations[n].vy = 0;
		Rotations[n].vz = 0;
		n++;
	}
	
	if (py==WORLD_MAX_Y-1 || worldmaps[px][py+1][pz]&WALL) {	 // wall below
		GsInitCoordinate2( &World[px][py][pz], &DWorld[n] );
		DWorld[n].coord.t[0] = 0;
		DWorld[n].coord.t[1] = FACT/2;
		DWorld[n].coord.t[2] = 0;
		GsLinkObject4((long)dop5, &Models[n],0);
		Models[n].coord2 = &DWorld[n];
		Models[n].attribute = atrb;
		Rotations[n].vx = ONE/2;
		Rotations[n].vy = 0;
		Rotations[n].vz = 0;
		n++;
	}
	

	if (pz==0 || worldmaps[px][py][pz-1]&WALL) {			 // wall to south
		GsInitCoordinate2( &World[px][py][pz], &DWorld[n] );
		DWorld[n].coord.t[0] = 0;
		DWorld[n].coord.t[1] = 0;
		DWorld[n].coord.t[2] = -FACT/2;
		GsLinkObject4((long)dop5, &Models[n],0);
		Models[n].coord2 = &DWorld[n];
		Models[n].attribute = atrb;
		Rotations[n].vx = ONE/4;
		Rotations[n].vy = 0;
		Rotations[n].vz = 0;
		n++;
	}
   

	if(pz==WORLD_MAX_Z-1 || worldmaps[px][py][pz+1]&WALL) {	 // wall to north
		GsInitCoordinate2( &World[px][py][pz], &DWorld[n] );
		DWorld[n].coord.t[0] = 0;
		DWorld[n].coord.t[1] = 0;
		DWorld[n].coord.t[2] = FACT/2;
		GsLinkObject4((long)dop5, &Models[n],0);
		Models[n].coord2 = &DWorld[n];
		Models[n].attribute = atrb;
		Rotations[n].vx = 3 * ONE/4;
		Rotations[n].vy = 0;
		Rotations[n].vz = 0;
		n++;
	}

	return( n );
}









void InitialiseAll (void)
{
	printf("iseAll: 0\n");
	PadInit(0);

	printf("iseAll: 1\n");


	GsInitGraph(PIH,PIV,GsINTER|GsOFSGPU,1,0);
	if( PIV<480 )
		GsDefDispBuff(0,0,0,PIV);
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
	printf("iseAll: 5\n");

		
	dop1 = InitialiseModel((long*)MODEL1_ADDR);	
	dop2 = InitialiseModel((long*)MODEL2_ADDR);	
	dop3 = InitialiseModel((long*)MODEL3_ADDR);	
	dop4 = InitialiseModel((long*)MODEL4_ADDR);	
	dop5 = InitialiseModel((long*)MODEL5_ADDR); 
	dop6 = InitialiseModel((long*)MODEL6_ADDR);	
	  
	WorldType = RANDOM_WORLD;

	if (WorldType == SET_WORLD)
		GenerateSetWorld();
	else if (WorldType == RANDOM_WORLD)
		GenerateRandomWorld();
	else
		assert(FALSE);	

	//nModels = calc_world_data();
	printf("iseAll: 6\n");

	InitialiseObjects();
		 
	MainMode = MOVE_SHIP;

	FntLoad( 960, 256);
	FntOpen( 0, 0, 256, 200, 0, 512);		  // 64, 32 as first two args before
}





void InitialiseObjects (void)
{
	int i;

	InitialiseObjectClass();
	printf("1\n");
	printf("slots left: %d\n", CountNumberObjectSlotsLeft() );

	InitSingleObject(&PlayersShip);

	BringObjectToLife (&PlayersShip, PLAYER, 
					SHIP_MODEL_ADDRESS, 0, NONE);
	RegisterObjectIntoObjectArray(&PlayersShip);
	printf("slots left: %d\n", CountNumberObjectSlotsLeft() );

	PositionSomewhereNotInAWall(&PlayersShip);
	PlayersShip.initialHealth = 1000;
	PlayersShip.currentHealth = 1000;
	PlayersShip.allegiance = PLAYER;

	PlayersShip.movementSpeed = 2;
	PlayersShip.rotationSpeed = 55;
	PlayersShip.firingRate = 4;
	PlayersShip.framesSinceLastFire = 0;
	PlayersShip.movementMomentumFlag = TRUE;

	
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

		TheShips[i].strategyFlag = INACTIVE;

#if 0
					// set strategy randomly
		TheShips[i].strategyFlag = rand() % (NUMBER_ENEMY_STRATEGIES-1);
		InitialiseShipAccordingToStrategy(&TheShips[i]);
#endif

		PositionSomewhereNotInAWall(&TheShips[i]);
		TheShips[i].allegiance = ENEMY;

		TheShips[i].position.vy += 20;

		RegisterObjectIntoObjectArray(&TheShips[i]);
		}
	printf("5\n");



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
						UpdateObjectCoordinates(&object->twist, &object->position, 
								&object->velocity, &object->coord, &object->matrix);
							
						object->rotate.vx += object->twist.vx;
						object->rotate.vy += object->twist.vy;
						object->rotate.vz += object->twist.vz;
						
						#if 1	
						if (frameNumber % 15 == 0)
							{
							VECTOR temp;
							int relativeZ;

							GetPlusZAxisFromMatrix(&PlayersShip.matrix,
								&temp);
							//printf("z forward axis\n");
							//dumpVECTOR(&temp);
							GetPlusXAxisFromMatrix(&PlayersShip.matrix,
								&temp);
							//printf("left or right axis\n");
							//dumpVECTOR(&temp);
							GetPlusYAxisFromMatrix(&PlayersShip.matrix,
								&temp);
							//printf("up or down axis\n");
							//dumpVECTOR(&temp);
								
							//printf("matrix itself\n");
							//dumpMATRIX(&PlayersShip.matrix);

							relativeZ = CalculateRelativeZ();
							printf("rel. z to ship: %d\n", relativeZ);
							}
						#endif

						SortObjectSize(object);

						KeepWithinRange(object->position.vx, -FACT/2, WORLD_MAX_X * FACT -FACT/2);
						KeepWithinRange(object->position.vy, -FACT/2, WORLD_MAX_Y * FACT -FACT/2);
						KeepWithinRange(object->position.vz, -FACT/2, WORLD_MAX_Z * FACT -FACT/2);

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

						if (worldmaps[gridX][gridY][gridZ] == WALL)
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

							// if in a wall, kill off
						gridX = object->position.vx / FACT;	  // FACT
						gridY = object->position.vy / FACT;
						gridZ = object->position.vz / FACT; 

						if (worldmaps[gridX][gridY][gridZ] == WALL)
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
					case ENEMY:
						HandleEnemyBehaviour(object);

						object->rotate.vx += object->twist.vx;
						object->rotate.vy += object->twist.vy;
						object->rotate.vz += object->twist.vz;

						UpdateObjectCoordinates(&object->twist, &object->position, 
								&object->velocity, &object->coord, &object->matrix);

						SortObjectSize(object);

						KeepWithinRange(object->position.vx, -FACT/2, WORLD_MAX_X * FACT -FACT/2);
						KeepWithinRange(object->position.vy, -FACT/2, WORLD_MAX_Y * FACT -FACT/2);
						KeepWithinRange(object->position.vz, -FACT/2, WORLD_MAX_Z * FACT -FACT/2);

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
					default:
						assert(FALSE);
					}
				}
			}
		}

endOfHandleAllObjects:
	;
}






int CalculateRelativeZ (void)
{
	VECTOR displacement;
	VECTOR xAxis, yAxis, zAxis;
	int super, denom;
	
	assert(MAX_SHIPS == 1);

	displacement.vx = TheShips[0].position.vx - PlayersShip.position.vx;
	displacement.vy = TheShips[0].position.vy - PlayersShip.position.vy;
	displacement.vz = TheShips[0].position.vz - PlayersShip.position.vz;

	printf("world-rel position vector:\n");
	dumpVECTOR(&displacement);

		// times ONE: scaling to same size as axes
	displacement.vx *= ONE;
	displacement.vy *= ONE;
	displacement.vz *= ONE;

	GetPlusXAxisFromMatrix( &PlayersShip.matrix, &xAxis);
	GetPlusYAxisFromMatrix( &PlayersShip.matrix, &yAxis);
	GetPlusZAxisFromMatrix( &PlayersShip.matrix, &zAxis);

	denom = (((yAxis.vx * xAxis.vy) - (xAxis.vx * yAxis.vy))
			 * ((xAxis.vx * zAxis.vz) - (zAxis.vx * xAxis.vz)))
			 - 
			 (((yAxis.vx * xAxis.vz) - (xAxis.vx * yAxis.vz))
			 * ((xAxis.vx * zAxis.vy) - (zAxis.vx * xAxis.vy)));

	if (denom == 0)
		return -1;

	super = (((yAxis.vx * xAxis.vz) - (xAxis.vx * yAxis.vz))
			 * ((xAxis.vy * displacement.vx) - (displacement.vy * xAxis.vx)))
			 - 
			 (((yAxis.vx * xAxis.vy) - (xAxis.vx * yAxis.vy))
			 * ((xAxis.vz * displacement.vx) - (xAxis.vx * displacement.vz)));

	super *= ONE;		// to make visible

	printf("super: %d, denom: %d\n", super, denom);

	return (super/denom);
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

	if (CollisionWithWalls(position, &realMovement) == FALSE)  
		{
			// update position by actual movement
		position->vx += realMovement.vx;
		position->vy += realMovement.vy;
		position->vz += realMovement.vz;
		}
	else
		{
		#if 0		// no more rebounds
			// send object the other way (rebound)
		position->vx -= PositionalRebound * realMovement.vx;
		position->vy -= PositionalRebound * realMovement.vy;
		position->vz -= PositionalRebound * realMovement.vz;
		velocity->vx = -velocity->vx / VelocityRebound;
		velocity->vy = -velocity->vy / VelocityRebound;
		velocity->vz = -velocity->vz / VelocityRebound;
		#endif
		velocity->vx = 0;
		velocity->vy = 0;
		velocity->vz = 0;
		}

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

		// copy the translating vector (position)
	tempMatrix.t[0] = coordSystem->coord.t[0];
	tempMatrix.t[1] = coordSystem->coord.t[1];
	tempMatrix.t[2] = coordSystem->coord.t[2];

		// get rotation matrix from rotation vector
	RotMatrix(rotation, &tempMatrix);

		// find the velocity in world coordinate terms
	ApplyMatrixLV(&tempMatrix, velocity, &realMovement);
	   
		// assign new matrix to coordinate system
	coordSystem->coord = tempMatrix;
	

	if (CollisionWithWalls(position, &realMovement) == FALSE)	
		{
			// update position by actual movement
		position->vx += realMovement.vx;
		position->vy += realMovement.vy;
		position->vz += realMovement.vz;
		} 
	else
		{
		#if 0		// no more rebounds
			// send object the other way (rebound)
		position->vx -= PositionalRebound * realMovement.vx;
		position->vy -= PositionalRebound * realMovement.vy;
		position->vz -= PositionalRebound * realMovement.vz;
		velocity->vx = -velocity->vx / VelocityRebound;
		velocity->vy = -velocity->vy / VelocityRebound;
		velocity->vz = -velocity->vz / VelocityRebound;
		#endif
		velocity->vx = 0;
		velocity->vy = 0;
		velocity->vz = 0;
		}
  
		// set position absolutely	
	coordSystem->coord.t[0] = position->vx;
	coordSystem->coord.t[1] = position->vy;
	coordSystem->coord.t[2] = position->vz;

		// tell GTE that coordinate system has been updated
	coordSystem->flg = 0;
}




void UpdateModelCoords (SVECTOR* rotationVector,
									GsCOORDINATE2* coordSystem)
{
	tempMatrix.t[0] = coordSystem->coord.t[0];
	tempMatrix.t[1] = coordSystem->coord.t[1];
	tempMatrix.t[2] = coordSystem->coord.t[2];      

		// get rotation matrix from rotation vector
	RotMatrix(rotationVector, &tempMatrix);

		// assign new matrix to coordinate system
	coordSystem->coord = tempMatrix;
}





	// dimension of collision detection
int CollisionWidth = FACT/8; 




int CollisionWithWalls (VECTOR* position, VECTOR* movement)
{
	CUBOID cuboid;

	setCUBOID(&cuboid, position->vx + movement->vx, 
					position->vy + movement->vy, 
						position->vz + movement->vz, 
						CollisionWidth, CollisionWidth, CollisionWidth);

#if 0
	if (frameNumber % 15 == 0)
		{
		printf("CUBOID: %d %d %d\n%d %d %d\n", 
			cuboid.x, cuboid.y, cuboid.z, cuboid.w, cuboid.h, cuboid.d);
		}
#endif

	if (NotSafe(&cuboid))
		return TRUE;

	return FALSE;
}
	




int x1, y1, z1;
int x2, y2, z2;
int gx1, gy1, gz1;
int gx2, gy2, gz2;


int NotSafe (CUBOID* cuboid)
{
	x1 = (cuboid->x - (cuboid->w/2));	  
	y1 = (cuboid->y - (cuboid->h/2));
	z1 = (cuboid->z - (cuboid->d/2));
	x2 = (cuboid->x + (cuboid->w/2));
	y2 = (cuboid->y + (cuboid->h/2));
	z2 = (cuboid->z + (cuboid->d/2));

	//printf("x1 %d x2 %d\n", x1, x2);
	//printf("y1 %d y2 %d\n", y1, y2);
	//printf("z1 %d z2 %d\n", z1, z2);

	if (x1 < -FACT/2 + CollisionWidth)
		return TRUE;
	if (y1 < -FACT/2 + CollisionWidth)
		return TRUE;
	if (z1 < -FACT/2 + CollisionWidth)
		return TRUE;
	if (x2 >= ((WORLD_MAX_X * FACT) -FACT/2 - CollisionWidth))
		return TRUE;
	if (y2 >= ((WORLD_MAX_Y * FACT) -FACT/2 - CollisionWidth))
		return TRUE;
	if (z2 >= ((WORLD_MAX_Z * FACT) -FACT/2 - CollisionWidth))
		return TRUE;

		// adding FACT/2: adjustment for real positions of squares
		// which are offset from grid cube centres
		// this DODGY: needs redoing properly
	gx1 = (x1 + FACT/2) / FACT;
	gy1 = (y1 + FACT/2) / FACT;
	gz1 = (z1 + FACT/2) / FACT;
	gx2 = (x2 + FACT/2) / FACT;
	gy2 = (y2 + FACT/2) / FACT;
	gz2 = (z2 + FACT/2) / FACT;

#if 0
	if (frameNumber % 15 == 0)
		{
		printf("Points: %d %d %d\n%d %d %d\n", x1, y1, z1, x2, y2, z2);
		printf("grid: %d %d %d\n%d %d %d\n", gx1, gy1, gz1, gx2, gy2, gz2);
		}
#endif

	if (worldmaps[gx1][gy1][gz1] & WALL
		|| worldmaps[gx1][gy1][gz2] & WALL
		|| worldmaps[gx1][gy2][gz1] & WALL
		|| worldmaps[gx1][gy2][gz2] & WALL
		|| worldmaps[gx2][gy1][gz1] & WALL
		|| worldmaps[gx2][gy1][gz2] & WALL
		|| worldmaps[gx2][gy2][gz1] & WALL
		|| worldmaps[gx2][gy2][gz2] & WALL)
		return TRUE;

	return FALSE;
}







void InitialiseView( void )
{
	GsSetProjection(192);  

	ReferenceDistance = 50;
	ViewpointZDistance = 220;
	ViewpointYDistance = -40;

#if 1
	view.vrx = 0; view.vry = 0; view.vrz = 0; //ReferenceDistance;
	view.vpx = 0; view.vpy = ViewpointYDistance; view.vpz = ViewpointZDistance;
	view.rz = 0;
	view.super = &PlayersShip.coord;
#endif

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




	lightmode = 1;	  
	GsSetLightMode(lightmode);


		// fogging
	fogparam.dqa = -960;
	fogparam.dqb = 5120*5120;
		// inky-blue/black background colour
	fogparam.rfc = 0; fogparam.gfc = 0; fogparam.bfc = 4;
	GsSetFogParam(&fogparam);
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








void GenerateRandomWorld (void)
{
	int	x, y, z;
	int	cellContent;
	int wallPercentage = 25;
	int numberWallsSoFar = 0;

		// this chunk below:
		// for each block in world, set to WALL or nothing
	for( x=0; x<WORLD_MAX_X; x+=3 ) 
		{		
		for( y=0; y<WORLD_MAX_Y; y+=3 )
			{
			for( z=0; z<WORLD_MAX_Z; z+=3 ) 
				{
				//cellContent = (rand()%150==0) ? WALL : 0;
					// 2700 = 27 * 100: 100 for percentage, 27 for loop step (3) cubed
				if ( ((rand() % 3) == 0) 
					&& numberWallsSoFar < WORLD_MAX_X*WORLD_MAX_Y*WORLD_MAX_Z*wallPercentage/2700)
						{
						cellContent = WALL;
						numberWallsSoFar++;
						printf("making a block, centre %d %d %d\n", x+1, y+1, z+1);
						}
				else
					cellContent = 0;

				worldmaps[x][y][z] = cellContent;
				worldmaps[x+1][y][z] = cellContent;
				worldmaps[x+2][y][z] = cellContent;
				worldmaps[x][y+1][z] = cellContent;
				worldmaps[x+1][y+1][z] = cellContent;
				worldmaps[x+2][y+1][z] = cellContent;
				worldmaps[x][y+2][z] = cellContent;
				worldmaps[x+1][y+2][z] = cellContent;
				worldmaps[x+2][y+2][z] = cellContent;
				worldmaps[x][y][z+1] = cellContent;
				worldmaps[x+1][y][z+1] = cellContent;
				worldmaps[x+2][y][z+1] = cellContent;
				worldmaps[x][y+1][z+1] = cellContent;
				worldmaps[x+1][y+1][z+1] = cellContent;
				worldmaps[x+2][y+1][z+1] = cellContent;
				worldmaps[x][y+2][z+1] = cellContent;
				worldmaps[x+1][y+2][z+1] = cellContent;
				worldmaps[x+2][y+2][z+1] = cellContent;
				worldmaps[x][y][z+2] = cellContent;
				worldmaps[x+1][y][z+2] = cellContent;
				worldmaps[x+2][y][z+2] = cellContent;
				worldmaps[x][y+1][z+2] = cellContent;
				worldmaps[x+1][y+1][z+2] = cellContent;
				worldmaps[x+2][y+1][z+2] = cellContent;
				worldmaps[x][y+2][z+2] = cellContent;
				worldmaps[x+1][y+2][z+2] = cellContent;
				worldmaps[x+2][y+2][z+2] = cellContent;
				}
			}
		}

		// set WORLD coordinate grid; this used in calc_model_data
		// as array of fixed reference frames
	for( x=0; x<WORLD_MAX_X; x++ ) 
		{
		for( y=0; y<WORLD_MAX_Y; y++ ) 
			{
			for( z=0; z<WORLD_MAX_Z; z++ ) 
				{
				GsInitCoordinate2(WORLD, &World[x][y][z]);
				World[x][y][z].coord.t[0] = x*FACT;
				World[x][y][z].coord.t[1] = y*FACT;
				World[x][y][z].coord.t[2] = z*FACT;
				}
			}
		}
}



void GenerateSetWorld (void)
{
	int x, y, z;

	for( x=0; x<WORLD_MAX_X; x++ ) 
		{		
		for( y=0; y<WORLD_MAX_Y; y++ )
			{
			for( z=0; z<WORLD_MAX_Z; z++ ) 
				{
				if ( ((x >= 2 && x <= 5) || (x >= 8 && x <= 11))
					&& ((y >= 2 && y <= 5) || (y >= 8 && y <= 11))
					&& ((z >= 2 && z <= 5) || (z >= 8 && z <= 11)) ) 
					worldmaps[x][y][z] = WALL;
				else
					worldmaps[x][y][z] = 0;
				}
			}
		}

		// set WORLD coordinate grid; this used in calc_model_data
		// as array of fixed reference frames
	for( x=0; x<WORLD_MAX_X; x++ ) 
		{
		for( y=0; y<WORLD_MAX_Y; y++ ) 
			{
			for( z=0; z<WORLD_MAX_Z; z++ ) 
				{
				GsInitCoordinate2(WORLD, &World[x][y][z]);
				World[x][y][z].coord.t[0] = x * FACT;
				World[x][y][z].coord.t[1] = y * FACT;
				World[x][y][z].coord.t[2] = z * FACT;
				}
			}
		}

}


   

  

	// NOTE: incomplete: see object.h for necessary additions
void PrintObject (ObjectHandler* object)
{
	printf("id: %d, alive: %d, type: %d, which: %d\n",
		object->id, object->alive, object->type, object->which);

	dumpOBJ2(&object->handler);
	printf("model: addr %u, which one %d\n",
		object->modelAddress, object->whichModel);

	printf("scaling flag: %d\n", object->scalingFlag);
	dumpVECTOR(&object->scalingVector);

	printf("movementTypeFlag: %d\n", object->movementTypeFlag);

	dumpVECTOR(&object->position);
	dumpVECTOR(&object->velocity);
	printf("movement momentum: %d\n",
		object->movementMomentumFlag);
		
	dumpVECTOR(&object->rotate);
	dumpVECTOR(&object->twist);
	printf("rotation momentum: %d\n",
		object->rotationMomentumFlag); 

	dumpCOORD2(&object->coord);
	printf("super coord object id: %d\n",
		object->superCoordinateObjectID);
	dumpMATRIX(&object->matrix);

	printf("inOrbit: %d, centralBodyID: %d\n, \
		angle: %d, angleIncrement: %d\n",
		object->inOrbit, object->centralBodyID,
		object->angle, object->angleIncrement);
	printf("whichWay: %d, radius: %d\n",
		object->whichWay, object->radius);
	dumpVECTOR(&object->firstVector);
	dumpVECTOR(&object->secondVector);

	printf("specialMovement: %d\n", object->specialMovement);

	printf("initialHealth: %d\n", object->initialHealth);
	printf("currentHealth: %d\n", object->currentHealth);

	printf("strategyFlag: %d\n", object->strategyFlag);

	printf("meritRating: %d\n", object->meritRating);
}





	// random positioning
void PositionSomewhereNotInAWall (ObjectHandler* object)
{
	int gridX, gridY, gridZ;
	int xOffset, yOffset, zOffset;
	int success = FALSE;


if (WorldType == SET_WORLD)
	{
	for (;;)
		{			// get random grid-square
		gridX = (rand() % WORLD_MAX_X);
		gridY = (rand() % WORLD_MAX_Y);
		gridZ = (rand() % WORLD_MAX_Z);

		printf("testing: %d %d %d\n", gridX, gridY, gridZ);

		if (worldmaps[gridX][gridY][gridZ] != WALL)
			{
			success = TRUE;
			break;
			}
		}

	assert(success == TRUE);			// check

		// offset within grid square
	xOffset = 0;//+ (rand() % (FACT/2));
	yOffset = 0;//+ (rand() % (FACT/2));
	zOffset = 0;// + (rand() % (FACT/2));

	object->position.vx = (gridX * FACT) + xOffset;
	object->position.vy = (gridY * FACT) + yOffset;
	object->position.vz = (gridZ * FACT) + zOffset;
	}
else if (WorldType == RANDOM_WORLD)
	{
	for (;;)
		{			// get random grid-square
		gridX = 1 + (3 * (rand() % (WORLD_MAX_X/3)));
		gridY = 1 + (3 * (rand() % (WORLD_MAX_Y/3)));
		gridZ = 1 + (3 * (rand() % (WORLD_MAX_Z/3)));

		printf("testing: %d %d %d\n", gridX, gridY, gridZ);

		if (worldmaps[gridX][gridY][gridZ] != WALL
			&& worldmaps[gridX-1][gridY][gridZ] != WALL
			&& worldmaps[gridX+1][gridY][gridZ] != WALL
			&& worldmaps[gridX][gridY+1][gridZ] != WALL
			&& worldmaps[gridX-1][gridY+1][gridZ] != WALL
			&& worldmaps[gridX+1][gridY+1][gridZ] != WALL
			&& worldmaps[gridX][gridY-1][gridZ] != WALL
			&& worldmaps[gridX-1][gridY-1][gridZ] != WALL
			&& worldmaps[gridX+1][gridY-1][gridZ] != WALL

			&& worldmaps[gridX][gridY][gridZ-1] != WALL
			&& worldmaps[gridX-1][gridY][gridZ-1] != WALL
			&& worldmaps[gridX+1][gridY][gridZ-1] != WALL
			&& worldmaps[gridX][gridY+1][gridZ-1] != WALL
			&& worldmaps[gridX-1][gridY+1][gridZ-1] != WALL
			&& worldmaps[gridX+1][gridY+1][gridZ-1] != WALL
			&& worldmaps[gridX][gridY-1][gridZ-1] != WALL
			&& worldmaps[gridX-1][gridY-1][gridZ-1] != WALL
			&& worldmaps[gridX+1][gridY-1][gridZ-1] != WALL

			&& worldmaps[gridX][gridY][gridZ+1] != WALL
			&& worldmaps[gridX-1][gridY][gridZ+1] != WALL
			&& worldmaps[gridX+1][gridY][gridZ+1] != WALL
			&& worldmaps[gridX][gridY+1][gridZ+1] != WALL
			&& worldmaps[gridX-1][gridY+1][gridZ+1] != WALL
			&& worldmaps[gridX+1][gridY+1][gridZ+1] != WALL
			&& worldmaps[gridX][gridY-1][gridZ+1] != WALL
			&& worldmaps[gridX-1][gridY-1][gridZ+1] != WALL
			&& worldmaps[gridX+1][gridY-1][gridZ+1] != WALL)
			{
			success = TRUE;
			break;
			}
		}

	assert(success == TRUE);			// check

		// offset within grid square
	xOffset = 0;//+ (rand() % (FACT/2));
	yOffset = 0;//+ (rand() % (FACT/2));
	zOffset = 0;// + (rand() % (FACT/2));

	object->position.vx = (gridX * FACT) + xOffset;
	object->position.vy = (gridY * FACT) + yOffset;
	object->position.vz = (gridZ * FACT) + zOffset;
	}
else
	assert(FALSE);
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






#define PROXIMITY 200

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

	if (abs(x) < PROXIMITY && abs(y) < PROXIMITY && abs(z) < PROXIMITY)
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





void HandleStationaryTrackingShipsBehaviour (ObjectHandler* object)
{
#if 0		// VERY OLD
	int x, z;
	int turnAngle, positionalAngle;

		// position constant, point towards player
		// fire if clear line and close range
		
	x = PlayersShip.position.vx - object->position.vx;
	z = PlayersShip.position.vz - object->position.vz;

	positionalAngle = SUBratan(x,z);
	positionalAngle += 2048;
	positionalAngle &= 4095;
	turnAngle = positionalAngle - object->rotate.vy;
			
	if (abs(turnAngle) >= 57)	// 5 degrees
		{
		if (turnAngle >= 0)			 // to the right
			object->twist.vy += (object->rotationSpeed << 2);	  // speedMultiplier
		else						 // to the left
			object->twist.vy -= (object->rotationSpeed << 2);
		}
	else			// small angle
		{
		if (abs(x) < 500 && abs(z) < 500)	 // if close up
			{
			if (object->framesSinceLastFire > object->firingRate)			// rapid fire
				{
				EnemyShipFiresAShot(object);
				object->framesSinceLastFire = 0;
				}
			}
		}
#endif
}






	// will turn towards as stat_track, but move if too far away, pursue
void HandleSeekingShipBehaviour (ObjectHandler* object)
{
#if 0		/// VERY OLD
	int x, z;
	int turnAngle, positionalAngle;
		
	x = PlayersShip.position.vx - object->position.vx;
	z = PlayersShip.position.vz - object->position.vz;

	positionalAngle = SUBratan(x,z);
	positionalAngle += 2048;
	positionalAngle &= 4095;
	turnAngle = positionalAngle - object->rotate.vy;
			
		// if angle too big, turn
	if (abs(turnAngle) >= 114)	// 10 degrees
		{
		if (turnAngle >= 0)			 // to the right
			object->twist.vy += (object->rotationSpeed << 2);	  // speedMultiplier
		else						 // to the left
			object->twist.vy -= (object->rotationSpeed << 2);
		}
	else			// small angle
		{		  
		if (abs(x) > 500 && abs(z) > 500)	   // move toward player
			object->velocity.vz -= object->movementSpeed;
		else if (abs(x) < 250 && abs(z) < 250)	   // move away from player
			object->velocity.vz += object->movementSpeed;

		if (abs(x) < 500 && abs(z) < 500)	 // if close up, fire
			{
			if (object->framesSinceLastFire > object->firingRate)			// rapid fire
				{
				EnemyShipFiresAShot(object);
				object->framesSinceLastFire = 0;
				}
			}
		}
#endif
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

	if (WorldType == SET_WORLD)
		GenerateSetWorld();
	else if (WorldType == RANDOM_WORLD)
		GenerateRandomWorld();
	else
		assert(FALSE);


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

		TheShips[i].strategyFlag = INACTIVE;

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



