/************************************************************
 *															*
 *						Tuto7.c								*
 *						3-D object manipulation				*
 *			   											    *															*
 *				LPGE     19/11/96							*		
 *															*
 *	Copyright (C) 1996 Sony Computer Entertainment Inc.		*
 *	All Rights Reserved										*
 *															*
 ***********************************************************/








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




	// This vector: vy expresses angle theta-y, i.e. angle on plane
static SVECTOR	PVect;	


		
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
#define WORLD_MAX_X	10
#define WORLD_MAX_Y	10
#define WORLD_MAX_Z 10
#define	WEST		112
#define	POSY		28
#define	WALL		0x01



static char worldmaps[WORLD_MAX_X][WORLD_MAX_Y][WORLD_MAX_Z];
static GsCOORDINATE2 World[WORLD_MAX_X][WORLD_MAX_Y][WORLD_MAX_Z];




#define MOVE_VIEW 0
#define MOVE_SHIP 1
#define ALTER_VIEW 2
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


#define MAX_SHIPS 20
ObjectHandler TheShips[MAX_SHIPS];


#define MAX_CUBES 20
ObjectHandler TheCubes[MAX_CUBES];


   
int PlayerBulletSpeedFactor;
int EnemyBulletSpeedFactor;			  

	 



	// simple enemy strategies
#define WAITING 0
#define STATIONARY_TRACK 1
#define SEEKING 2
#define RANDOM 3
#define INACTIVE 4

#define NUMBER_ENEMY_STRATEGIES 5

  


	// more business for strategies


#define SWIVEL_LEFT 0
#define SWIVEL_RIGHT 1
#define MOVE_FORWARDS 2
#define MOVE_BACKWARDS 3
#define FIRING 4

#define NUMBER_RANDOM_ACTIONS 5


	


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


int CompassDirectionToNearestEnemy = NONE;
int ExactDirectionToNearestEnemy = 8192;











/****************************************************************************
					prototypes
****************************************************************************/


void main (void);

void CheckForEndOfLevel (void);
int CountNumberOfEnemiesLeft (void);

void draw_world_maps (GsOT* ot);

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

void UpdateObjectCoordinates (VECTOR* twist,
							VECTOR* position, VECTOR* velocity,
							GsCOORDINATE2* coordSystem, MATRIX* matrix);
void UpdateObjectCoordinates2 (SVECTOR* rotationVector,
							VECTOR* translationVector,
							GsCOORDINATE2* coordSystem);
void UpdateObjectCoordinates3 (SVECTOR* rotation, VECTOR* twist,
							VECTOR* position, VECTOR* velocity,
							GsCOORDINATE2* coordSystem);


int CollisionWithWalls (VECTOR* position, VECTOR* movement);	

int NotSafe (CUBOID* rectangle);




void InitialiseView(void);
void InitialiseLighting(void);




int InitialiseTexture(long addr);
long* InitialiseModel(long* adrs);
void make_world_maps(void);



int calc_world_data(void);
int NewCalculateWorldData (void);
int MinOfFour (int a, int b, int c, int d);
int MaxOfFour (int a, int b, int c, int d);




//int calc_model_data(int n, int px, int py, int fact, int atrb);
int calc_model_data(int n, int px, int py, int pz, int fact, int atrb);
void draw_world_maps( GsOT* ot );
int collision( int x, int y );


void PrintObject (ObjectHandler* object);

void PositionSomewhereNotInAWall (ObjectHandler* object);


void HandleEnemyBehaviour (ObjectHandler* object);

void HandleWaitingShipsBehaviour (ObjectHandler* object);
void HandleStationaryTrackingShipsBehaviour (ObjectHandler* object);
void HandleSeekingShipBehaviour (ObjectHandler* object);
void HandleRandomShipsBehaviour (ObjectHandler* object);




void HandlePlayersDeath (void);
void HandleEnemysDeath (ObjectHandler* object);

int CheckCollisions (void); 

int ObjectsVeryClose (ObjectHandler* first, ObjectHandler* second);

void ExplodeObject (ObjectHandler* object);

void DummyGuiLoopForTextPrinting (int numberOfStrings, int quitKey);

void ResetAll (void);

void HandleLevelTransition (void);

int FindDirectionToNearestLivingEnemy (int* exactDirection);
void PrintCompassDirectionToNearestEnemy (int direction);






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

		//FntPrint("X = %d\nZ = %d\nV = %d\nN = %d\nH = %d\n",view.vpx,view.vpz,PVect.vy,nModels,hsync );
		//FntPrint("X = %d\nZ = %d\nV = %d\n", view.vpx,view.vpz,PVect.vy);
		//FntPrint("Num models: %d\nHsync = %d\n",nModels,hsync );
		FntPrint("frame: %d\n", frameNumber);
		FntPrint("player health: %d\n", PlayersShip.currentHealth);	 
		FntPrint("player score: %d\n", PlayersShip.meritRating);
		FntPrint("level: %d\n", LevelNumber);

		//FntPrint("nearest ship:\n");
		//PrintCompassDirectionToNearestEnemy(DirectionToNearestEnemy);

			// update direction to nearest enemy
		if (frameNumber % 10 == 0 && frameNumber > 5)
			{
			CompassDirectionToNearestEnemy 
				= FindDirectionToNearestLivingEnemy(&ExactDirectionToNearestEnemy);
			}


		FntPrint("enemies left: %d\n", NumberEnemiesLeft);
		if (frameNumber % 20 == 0)
			{
			NumberEnemiesLeft = CountNumberOfEnemiesLeft();
			}

				
	
		FntPrint("ship pos: \n%d %d %d\n", 
			PlayersShip.position.vx, 
			PlayersShip.position.vy,
			PlayersShip.position.vz);
			 
		x = PlayersShip.position.vx / FACT;
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

		hsync = VSync(2);
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







	// draw line to indicate angle of view on plane
void draw_world_maps( GsOT* ot )
{

	line.attribute = 0;
	line.r = line.g = line.b = 128;
	line.x0 = PIH/2-48; line.y0 = -PIV/2+48;
	line.x1 = line.x0 + (rsin( PVect.vy )/256);
	line.y1 = line.y0 - (rcos( PVect.vy )/256);

	setRECT( &box[0], line.x0-16, line.y0-16, 32, 32 );
	box[0].x += ( rsin( PVect.vy )/256 );
	box[0].y -= ( rcos( PVect.vy )/256 );
	box[0].r = 0; box[0].g = 0; box[0].b = 128;

	box[1].attribute = 1<<30;	  // semi-transparency
	setRECT( &box[1], line.x0-32, line.y0-32, 64, 64 );
	box[1].r = 128; box[1].g = 128; box[1].b = 128;

	GsSortLine( &line, ot, 2 );
	GsSortBoxFill( &box[0], ot, 2 );
	GsSortBoxFill( &box[1], ot, 2 );
}







VECTOR shipDirection;
VECTOR nearestEnemyDirection;
VECTOR goingEast, goingWest;

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






#define	SPEED	2


void DealWithControllerPad (void)
{
	int	spd;
	long	pad;
	long	range;
	short	vx, vz;	   // world-relative x, z displacements for this frame
	static int framesSinceLastModeToggle = 0;
  
	pad = PadRead(0);

	//if (frameNumber % 10 == 0)
		//printf("in DealWithControllerPad\nMainMode is %d\n", MainMode);
	
		// actions depend on mode
	switch(MainMode)
		{
		case MOVE_VIEW:
			#if 0		// OLD
			{
			spd = SPEED;
			if( pad & PADRright ) spd = spd*4;

				// Lleft and Lright: change angle on horizontal plane
			if(pad & PADLleft) {
				PVect.vy = (PVect.vy-spd*8)&4095;
			}

			if(pad & PADLright) {
				PVect.vy = (PVect.vy+spd*8)&4095;
			}

				// R2 and L2 (m and o): alter vx
			if((pad & PADm)&&(PVect.vx+spd*8<960)) {
				PVect.vx = (PVect.vx+spd*8);
			}
			if((pad & PADo)&&(PVect.vx-spd*8>-960)) {
				PVect.vx = (PVect.vx-spd*8);
			}

			vz = vx = 0;
			if(pad & PADLup) {
				vz += (rcos( PVect.vy )/64*spd)/16;
				vx += (rsin( PVect.vy )/64*spd)/16;
			}
			if(pad & PADLdown) {
				vz -= (rcos( PVect.vy )/64*spd)/16;
				vx -= (rsin( PVect.vy )/64*spd)/16;
			}
				// R1 and L1: sidestepping: inline conversion to world coords
			if(pad & PADl) {
				vz += (rcos((PVect.vy+1024)&4095)/64*spd)/16;
				vx += (rsin((PVect.vy+1024)&4095)/64*spd)/16;
			}
			if(pad & PADn) {
				vz += (rcos((PVect.vy-1024)&4095)/64*spd)/16;
				vx += (rsin((PVect.vy-1024)&4095)/64*spd)/16;
			}

				// prevent moving into walls
			if( vz>0 ) {
				if(collision(    0, vz+WEST)) vz = 0;
				if(collision(-WEST, vz+WEST)) vz = 0;
				if(collision( WEST, vz+WEST)) vz = 0;
			}
			if( vz<0 ) {
				if(collision(    0, vz-WEST)) vz = 0;
				if(collision(-WEST, vz-WEST)) vz = 0;
				if(collision( WEST, vz-WEST)) vz = 0;
			}
			if( vx>0 ) {
				if(collision( vx+WEST,     0)) vx = 0;
				if(collision( vx+WEST,  WEST)) vx = 0;
				if(collision( vx+WEST, -WEST)) vx = 0;
			}
			if( vx<0 ) {
				if(collision( vx-WEST,     0)) vx = 0;
				if(collision( vx-WEST,  WEST)) vx = 0;
				if(collision( vx-WEST, -WEST)) vx = 0;
			}

			if( vz || vx ) {
				view.vpz += vz;
				view.vpx += vx;
			}

			range = rcos( PVect.vx );
			view.vrz = (view.vpz+rcos( PVect.vy )*range/ONE);
			view.vrx = (view.vpx+rsin( PVect.vy )*range/ONE);
			view.vry = rsin( PVect.vx ) + POSY;
			}
			#endif
			break;
		case MOVE_SHIP:
				// movement
			if (pad & PADLup)
				PlayersShip.velocity.vz -= PlayersShip.movementSpeed;
			if (pad & PADLdown)
				PlayersShip.velocity.vz += PlayersShip.movementSpeed;

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
				PlayersShip.twist.vx += PlayersShip.rotationSpeed;
			if (pad & PADRdown)
				PlayersShip.twist.vx -= PlayersShip.rotationSpeed;
			if (pad & PADRleft)
				PlayersShip.twist.vz += PlayersShip.rotationSpeed;
			if (pad & PADRright)
				PlayersShip.twist.vz -= PlayersShip.rotationSpeed;
			break;
		case ALTER_VIEW:
			if (pad & PADRup)
				{
				ReferenceDistance += 5;
				view.vrx = 0; view.vry = 0; view.vrz = ReferenceDistance;
				view.vpx = 0; view.vpy = ViewpointYDistance; view.vpz = ViewpointZDistance;
				GsSetRefView2(&view);
				}
			if (pad & PADRdown)
				{
				ReferenceDistance -= 5;
				view.vrx = 0; view.vry = 0; view.vrz = ReferenceDistance;
				view.vpx = 0; view.vpy = ViewpointYDistance; view.vpz = ViewpointZDistance;
				GsSetRefView2(&view);
				}
			if (pad & PADLup)
				{
				ViewpointYDistance += 5;
				view.vrx = 0; view.vry = 0; view.vrz = ReferenceDistance;
				view.vpx = 0; view.vpy = ViewpointYDistance; view.vpz = ViewpointZDistance;
				GsSetRefView2(&view);
				}
			if (pad & PADLdown)
				{
				ViewpointYDistance -= 5;
				view.vrx = 0; view.vry = 0; view.vrz = ReferenceDistance;
				view.vpx = 0; view.vpy = ViewpointYDistance; view.vpz = ViewpointZDistance;
				GsSetRefView2(&view);
				}
			if (pad & PADRleft)
				{
				ViewpointZDistance -= 20;
				view.vrx = 0; view.vry = 0; view.vrz = ReferenceDistance;
				view.vpx = 0; view.vpy = ViewpointYDistance; view.vpz = ViewpointZDistance;
				GsSetRefView2(&view);
				}
			if (pad & PADRright)
				{
				ViewpointZDistance += 20;
				view.vrx = 0; view.vry = 0; view.vrz = ReferenceDistance;
				view.vpx = 0; view.vpy = ViewpointYDistance; view.vpz = ViewpointZDistance;
				GsSetRefView2(&view);
				}
			break;
		default:
			assert(FALSE);
		}

#if 0
		// select: toggles main mode
	if (pad & PADselect)
		{
		if (framesSinceLastModeToggle > 10)
			{
			if (MainMode == ALTER_VIEW)	
				MainMode = MOVE_VIEW;
			else
				MainMode++;
			switch(MainMode)
				{
				case MOVE_VIEW:
					view.super = WORLD;
					break;
				case MOVE_SHIP:
					view.super = &PlayersShip.coord;
					break;
				}
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
			// NOTE: this goes badly askew when ship not parallel to xz plane
		TheBullets[bulletID].movementMomentumFlag = TRUE;
		TheBullets[bulletID].velocity.vx = PlayersShip.coord.coord.m[2][0] >> PlayerBulletSpeedFactor;
		TheBullets[bulletID].velocity.vy = PlayersShip.coord.coord.m[2][1] >> PlayerBulletSpeedFactor;
		TheBullets[bulletID].velocity.vz = -PlayersShip.coord.coord.m[2][2] >> PlayerBulletSpeedFactor; 
		
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
			// NOTE: this method goes badly askew when ship not parallel to xz plane
		TheBullets[bulletID].movementMomentumFlag = TRUE;
		TheBullets[bulletID].velocity.vx = enemy->coord.coord.m[2][0] >> EnemyBulletSpeedFactor;
		TheBullets[bulletID].velocity.vy = enemy->coord.coord.m[2][1] >> EnemyBulletSpeedFactor;
		TheBullets[bulletID].velocity.vz = -enemy->coord.coord.m[2][2] >> EnemyBulletSpeedFactor; 
		
					// emitted from nose of ship
		TheBullets[bulletID].position.vx = enemy->position.vx;
		TheBullets[bulletID].position.vy = enemy->position.vy;
		TheBullets[bulletID].position.vz = enemy->position.vz;
		}
}





#if 0

/* 衝突判定 */
int collision (int vx, int vz )
{
	int	wx, wz;

	wx = ((view.vpx+FACT/2)+vx);
	wz = ((view.vpz+FACT/2)+vz);
	if( wx<0 || wz<0 ) return(1);

	wx = wx/FACT; wz = wz/FACT;
	if( wx>=WORLD_MAX_X || wz<0 || wz>=WORLD_MAX_Y ) return(1);

	return( worldmaps[wx][wz] );
}

#endif




/* 視点移動による光源ベクトル補正 */
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


   
void NewMoveLightingSpot (void)
{
	TheLights[0].r = TheLights[0].g = TheLights[0].b = 0x70;
	TheLights[0].vx = PlayersShip.coord.coord.t[0];
	TheLights[0].vy = 4096;			 // from above
	TheLights[0].vz = PlayersShip.coord.coord.t[2];
	GsSetFlatLight(0,&TheLights[0]);

	TheLights[1].r = TheLights[1].g = TheLights[1].b = 0x70;
	TheLights[1].vx = PlayersShip.coord.coord.t[0];
	TheLights[1].vy = -4096;			 // from below
	TheLights[1].vz = PlayersShip.coord.coord.t[2];
	GsSetFlatLight(1,&TheLights[1]);
}





#if 0		// OLD


/* 自分の周囲のモデルデータ生成 */
int calc_world_data (void)
{
	int	n;
	int	x, y;
	int	px, py;
	int	sx, sy, ex, ey;
	int	fact;

	fact = FACT;

	px = (view.vpx+fact/2)/fact;		// grid square coordinates
	py = (view.vpz+fact/2)/fact;


	sx = px - 4; ex = px + 4;		   // box around where view is now
	sy = py - 4; ey = py + 4;

	sx += ( rsin( PVect.vy )/1000 );
	sy += ( rcos( PVect.vy )/1000 );
	ex += ( rsin( PVect.vy )/1000 );
	ey += ( rcos( PVect.vy )/1000 );

	if( sx<0 ) sx = 0;
	if( sy<0 ) sy = 0;
	if( ex>=WORLD_MAX_X ) ex = WORLD_MAX_X-1;
	if( ey>=WORLD_MAX_Y ) ey = WORLD_MAX_Y-1;

#if 0
	printf("OLD calc world data\n");
	printf("sx: %d, sy: %d, ex: %d, ey: %d\n", sx, sy, ex, ey);
#endif

	n = 0;				  // for every square in the around-view box of squares
	for( x=sx; x<=ex; x++ ) {
		for( y=sy; y<=ey; y++ ) {
			if( abs(px-x)<3 && abs(py-y)<3 ) {
				/* 近くのモデルはポリゴン分割する */
				n = calc_model_data( n, x, y, fact, GsDIV1 );
			} else	{
				n = calc_model_data( n, x, y, fact, 0 );
			}
		}
	}
	return( n );
}

#endif





VECTOR p1, p2, p3, p4;
VECTOR radius, easternTgt, westernTgt;	 // 'radius': direction ship points in

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
	int SimpleClipDistance = 4;


#if 1			// OLD: super simple and very poor
	px = PlayersShip.position.vx / FACT;
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


#if 0		// NOT YET
	
#endif	
		

		// DEBUG
	if (frameNumber % 10 == 0)	
		{
		printf("px: %d, py: %d, pz: %d\n", px, py, pz);
		printf("sx: %d, sy: %d, sz: %d\n", sx, sy, sz);
		printf("ex: %d, ey: %d, ez: %d\n", ex, ey, ez);
		}

	
	numberOfModels = 0;				  // for every square in the around-view box of squares
	for (x = sx; x <= ex; x++) 
		{
		for (y = sy; y <= ey; y++) 
			{
			for (z = sz; z < ez; z++)
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

	printf("About to return num.models: %d\n", numberOfModels);

	return(numberOfModels);






#if 0			// OLD
	int numberOfModels;
	int	x, y;
	int px, py;
	int	sx, sy, ex, ey;		  // start x and y; end x and y
	int	fact;				  // distance
	static int ClipDistance = 4 * FACT;	
	int viewX, viewY;

	fact = FACT;

	// NOTE: easy to simplify & optimise the code below for creating
	// rectangle ahead of player's ship for object-world clipping

	radius.vx = rsin(PlayersShip.rotate.vy);
	radius.vy = POSY;
	radius.vz = rcos(PlayersShip.rotate.vy);

	easternTgt.vx = radius.vz;
	easternTgt.vy = POSY;
	easternTgt.vz = -radius.vx;

	westernTgt.vx = -radius.vz;
	westernTgt.vy = POSY;
	westernTgt.vz = radius.vx;

	viewX = PlayersShip.position.vx + ((ViewpointZDistance * rsin(PlayersShip.rotate.vy)) >> 12);
	viewY = PlayersShip.position.vz + ((ViewpointZDistance * rcos(PlayersShip.rotate.vy)) >> 12);
		

		
	p1.vx = viewX + ((-(radius.vx*2) + westernTgt.vx) * ClipDistance >> 12);
	p1.vy = PlayersShip.position.vy;
	p1.vz = viewY + ((-(radius.vz*2) + westernTgt.vz) * ClipDistance >> 12);
	p2.vx = viewX + ((-(radius.vx*2) + easternTgt.vx) * ClipDistance >> 12);   
	p2.vy = PlayersShip.position.vy;
	p2.vz = viewY + ((-(radius.vz*2) + easternTgt.vz) * ClipDistance >> 12);
	p3.vx = viewX + ((radius.vx/2 + westernTgt.vx) * ClipDistance >> 12);
	p3.vy = PlayersShip.position.vy;
	p3.vz = viewY + ((radius.vz/2 + westernTgt.vz) * ClipDistance >> 12);
	p4.vx = viewX + ((radius.vx/2 + easternTgt.vx) * ClipDistance >> 12);
	p4.vy = PlayersShip.position.vy;
	p4.vz = viewY + ((radius.vz/2 + easternTgt.vz) * ClipDistance >> 12);	

	

	sx = MinOfFour(p1.vx, p2.vx, p3.vx, p4.vx)/fact;
	sy = MinOfFour(p1.vz, p2.vz, p3.vz, p4.vz)/fact;
	ex = MaxOfFour(p1.vx, p2.vx, p3.vx, p4.vx)/fact;
	ey = MaxOfFour(p1.vz, p2.vz, p3.vz, p4.vz)/fact;
   
	if( sx<0 ) sx = 0;
	if( sy<0 ) sy = 0;
	if( ex>=WORLD_MAX_X ) ex = WORLD_MAX_X-1;
	if( ey>=WORLD_MAX_Y ) ey = WORLD_MAX_Y-1;

#if 0
	if (frameNumber % 15 == 0)
		{
		printf("NEW calc world data\n");
		printf("sx: %d, ex: %d, sy: %d, ey: %d\n", sx, ex, sy, ey);
		printf("angle vy: %d\n", PlayersShip.rotate.vy);
		printf("radius: ");dumpVECTOR(&radius);
		printf("east tgt: ");dumpVECTOR(&easternTgt);
		printf("west tgt: ");dumpVECTOR(&westernTgt);
		printf("p1 to p4:\n");
		dumpVECTOR(&p1);
		dumpVECTOR(&p2);
		dumpVECTOR(&p3);
		dumpVECTOR(&p4);
		}
#endif

	px = (sx + ex) / 2;
	py = (sy + ey) / 2;

	//printf("px: %d, py: %d\n", px, py);

	numberOfModels = 0;				  // for every square in the around-view box of squares
	for (x = sx; x <= ex; x++) 
		{
		for (y = sy; y <= ey; y++) 
			{
			if( abs(px-x)<4 && abs(py-y)<4 ) 
				{
				numberOfModels = calc_model_data( numberOfModels, x, y, fact, GsDIV1 );
				} 
			else	
				{
				numberOfModels = calc_model_data( numberOfModels, x, y, fact, 0 );
				}

			if (numberOfModels > OBJMAX)
				{
				numberOfModels = OBJMAX;
				printf("Forcibly holding number of models at OBJMAX %d\n", OBJMAX);
				goto end;
				}
			}
		}


end:

	//printf("About to return num.models: %d\n", numberOfModels);

	return(numberOfModels);
#endif
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


	if(( worldmaps[px-1][py][pz]&WALL)|| px==0 ) {	  // wall to left
		GsInitCoordinate2( &World[px][py][pz], &DWorld[n] );
		DWorld[n].coord.t[0] = 0;
		DWorld[n].coord.t[1] = 0;
		DWorld[n].coord.t[2] = 0;
		GsLinkObject4((long)dop4, &Models[n],0);
		Models[n].coord2 = &DWorld[n];
		Models[n].attribute = atrb;
		n++;
	}

	if(( worldmaps[px+1][py][pz]&WALL)|| px==WORLD_MAX_X-1 ) {	   // wall to right
		GsInitCoordinate2( &World[px][py][pz], &DWorld[n] );
		DWorld[n].coord.t[0] = 0;
		DWorld[n].coord.t[1] = 0;
		DWorld[n].coord.t[2] = 0;
		GsLinkObject4((long)dop3, &Models[n],0);
		Models[n].coord2 = &DWorld[n];
		Models[n].attribute = atrb;
		n++;
	}

	if(( worldmaps[px][py-1][pz]&WALL) || py==0 ) {			 // wall above
		GsInitCoordinate2( &World[px][py][pz], &DWorld[n] );
		DWorld[n].coord.t[0] = 0;
		DWorld[n].coord.t[1] = 0;
		DWorld[n].coord.t[2] = 0;
		GsLinkObject4((long)dop5, &Models[n],0);
		Models[n].coord2 = &DWorld[n];
		Models[n].attribute = atrb;
		n++;
	}

	if(( worldmaps[px][py+1][pz]&WALL) || py==WORLD_MAX_Y-1 ) {	 // wall below
		GsInitCoordinate2( &World[px][py][pz], &DWorld[n] );
		DWorld[n].coord.t[0] = 0;
		DWorld[n].coord.t[1] = 0;
		DWorld[n].coord.t[2] = 0;
		GsLinkObject4((long)dop6, &Models[n],0);
		Models[n].coord2 = &DWorld[n];
		Models[n].attribute = atrb;
		n++;
	}

	if(( worldmaps[px][py][pz-1]&WALL) || pz==0 ) {			 // wall to south
		GsInitCoordinate2( &World[px][py][pz], &DWorld[n] );
		DWorld[n].coord.t[0] = 0;
		DWorld[n].coord.t[1] = 0;
		DWorld[n].coord.t[2] = 0;
		GsLinkObject4((long)dop1, &Models[n],0);
		Models[n].coord2 = &DWorld[n];
		Models[n].attribute = atrb;
		n++;
	}

	if(( worldmaps[px][py][pz+1]&WALL) || pz==WORLD_MAX_Z-1 ) {	 // wall to north
		GsInitCoordinate2( &World[px][py][pz], &DWorld[n] );
		DWorld[n].coord.t[0] = 0;
		DWorld[n].coord.t[1] = 0;
		DWorld[n].coord.t[2] = 0;
		GsLinkObject4((long)dop2, &Models[n],0);
		Models[n].coord2 = &DWorld[n];
		Models[n].attribute = atrb;
		n++;
	}

	return( n );



	


#if 0			// OLD
	if( worldmaps[px][py]&WALL ) return(n);

	if(( worldmaps[px-1][py]&WALL)|| px==0 ) {	  // wall to left
		GsInitCoordinate2( &World[px][py], &DWorld[n] );
		DWorld[n].coord.t[0] = 0;
		DWorld[n].coord.t[1] = 0;
		DWorld[n].coord.t[2] = 0;
		GsLinkObject4((long)dop4, &Models[n],0);
		Models[n].coord2 = &DWorld[n];
		Models[n].attribute = atrb;
		n++;
	}

	if(( worldmaps[px+1][py]&WALL)|| px==WORLD_MAX_X-1 ) {	   // wall to right
		GsInitCoordinate2( &World[px][py], &DWorld[n] );
		DWorld[n].coord.t[0] = 0;
		DWorld[n].coord.t[1] = 0;
		DWorld[n].coord.t[2] = 0;
		GsLinkObject4((long)dop3, &Models[n],0);
		Models[n].coord2 = &DWorld[n];
		Models[n].attribute = atrb;
		n++;
	}

	if(( worldmaps[px][py-1]&WALL) || py==0 ) {			 // wall to south
		GsInitCoordinate2( &World[px][py], &DWorld[n] );
		DWorld[n].coord.t[0] = 0;
		DWorld[n].coord.t[1] = 0;
		DWorld[n].coord.t[2] = 0;
		GsLinkObject4((long)dop1, &Models[n],0);
		Models[n].coord2 = &DWorld[n];
		Models[n].attribute = atrb;
		n++;
	}

	if(( worldmaps[px][py+1]&WALL) || py==WORLD_MAX_Y-1 ) {	 // wall to north
		GsInitCoordinate2( &World[px][py], &DWorld[n] );
		DWorld[n].coord.t[0] = 0;
		DWorld[n].coord.t[1] = 0;
		DWorld[n].coord.t[2] = 0;
		GsLinkObject4((long)dop2, &Models[n],0);
		Models[n].coord2 = &DWorld[n];
		Models[n].attribute = atrb;
		n++;
	}

	GsInitCoordinate2(&World[px][py], &DWorld[n] );				// ceiling
	DWorld[n].coord.t[0] = 0;
	DWorld[n].coord.t[1] = - ( fact );
	DWorld[n].coord.t[2] = 0;
	GsLinkObject4((long)dop5, &Models[n],0);
	Models[n].coord2 = &DWorld[n];
	Models[n].attribute = atrb;
	n++;

	GsInitCoordinate2(&World[px][py], &DWorld[n] );				// floor
	DWorld[n].coord.t[0] = 0;
	DWorld[n].coord.t[1] = fact;
	DWorld[n].coord.t[2] = 0;
	GsLinkObject4((long)dop6, &Models[n],0);
	Models[n].coord2 = &DWorld[n];
	Models[n].attribute = atrb;
	n++;

	return( n );
#endif
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

	make_world_maps();
	//nModels = calc_world_data();
	printf("iseAll: 6\n");

	InitialiseObjects();

	//MainMode = MOVE_VIEW;		 
	MainMode = MOVE_SHIP;

	FntLoad( 960, 256);
	FntOpen( 0, 0, 256, 200, 0, 512);		  // 64, 32 as first two args before

	//PlayersShip.movementMomentumFlag = TRUE;

	PlayerBulletSpeedFactor = 7;
	EnemyBulletSpeedFactor = 8;
}





void InitialiseObjects (void)
{
	int i;

	InitialiseObjectClass();
	printf("1\n");

	InitSingleObject(&PlayersShip);

	BringObjectToLife (&PlayersShip, PLAYER, 
					SHIP_MODEL_ADDRESS, 0, NONE);
	RegisterObjectIntoObjectArray(&PlayersShip);

	PositionSomewhereNotInAWall(&PlayersShip);
	PlayersShip.initialHealth = 1000;
	PlayersShip.currentHealth = 1000;
	PlayersShip.allegiance = PLAYER;

	PlayersShip.movementSpeed = 25;
	PlayersShip.rotationSpeed = 55;
	PlayersShip.firingRate = 3;
	PlayersShip.framesSinceLastFire = 0;
	//PlayersShip.movementMomentumFlag = TRUE;

	
	printf("2\n");
		

	for (i = 0; i < MAX_BULLETS; i++)
		{
		InitSingleObject(&TheBullets[i]);

		BringObjectToLife(&TheBullets[i], BULLET, 
			CUBE_MODEL_ADDRESS, 0, NONE);

		SetObjectScaling(&TheBullets[i], ONE>>5, ONE>>5, ONE>>5);
		TheBullets[i].position.vy += 20;

		RegisterObjectIntoObjectArray(&TheBullets[i]);
		}
	printf("3\n");


	for (i = 0; i < MAX_CUBES; i++)
		{
		InitSingleObject(&TheCubes[i]);

		BringObjectToLife(&TheCubes[i], CUBE, 
			CUBE_MODEL_ADDRESS, 0, NONE);

		SetObjectScaling(&TheCubes[i], ONE>>3, ONE>>3, ONE>>3);
		TheCubes[i].position.vy += 20;

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
		case RANDOM:
			object->specialTimeLimit = 10 + (rand() % 10);
			object->firingRate = 8 + (rand() % 6);
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
							
						if (frameNumber % 10 == 0)
							{
							dumpMATRIX(&PlayersShip.matrix);
							}

						#if 0
							// rotate wrt world, move wrt object
						UpdateObjectCoordinates3 (&object->rotate, &object->twist,
							&object->position, &object->velocity,
							&object->coord);
						#endif

						SortObjectSize(object);

						KeepWithinRange(object->position.vx, 0, (WORLD_MAX_X-1)*FACT);
						KeepWithinRange(object->position.vy, 0, (WORLD_MAX_Y-1)*FACT);
						KeepWithinRange(object->position.vz, 0, (WORLD_MAX_Z-1)*FACT);

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
						#if 0
						UpdateObjectCoordinates2(&object->rotate,  
							&object->position, &object->coord);
						#endif
						UpdateObjectCoordinates(&object->twist, &object->position, 
								&object->velocity, &object->coord, &object->matrix);

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
						gridX = object->position.vx / FACT;
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
						#if 0
						UpdateObjectCoordinates2(&object->rotate,  
							&object->position, &object->coord);
						#endif
						UpdateObjectCoordinates(&object->twist, &object->position, 
								&object->velocity, &object->coord, &object->matrix);

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
						gridX = object->position.vx / FACT;
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

						#if 0
							// rotate wrt world, move wrt object
						UpdateObjectCoordinates3 (&object->rotate, &object->twist,
							&object->position, &object->velocity,
							&object->coord);
						#endif
						UpdateObjectCoordinates(&object->twist, &object->position, 
								&object->velocity, &object->coord, &object->matrix);

						SortObjectSize(object);

						KeepWithinRange(object->position.vx, 0, (WORLD_MAX_X-1)*FACT);
						KeepWithinRange(object->position.vy, 0, (WORLD_MAX_Y-1)*FACT);
						KeepWithinRange(object->position.vz, 0, (WORLD_MAX_Z-1)*FACT);

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
  
		// set position absolutely	
	coordSystem->coord.t[0] = position->vx;
	coordSystem->coord.t[1] = position->vy;
	coordSystem->coord.t[2] = position->vz;

		// tell GTE that coordinate system has been updated
	coordSystem->flg = 0;
}




	// dimension of collision detection
#define COLLISION_WIDTH (FACT)

int CollisionWithWalls (VECTOR* position, VECTOR* movement)
{
	int result;
	CUBOID cuboid;

		// THIS FUNCTION VERY SLOW 
		// NEEDS REWRITING BADLY
	return FALSE;


	setCUBOID(&cuboid, position->vx, position->vy, position->vz, 
						COLLISION_WIDTH, COLLISION_WIDTH, COLLISION_WIDTH);
	if (NotSafe(&cuboid))
		return TRUE;

	setCUBOID(&cuboid, position->vx + movement->vx, position->vy, position->vz, 
						COLLISION_WIDTH, COLLISION_WIDTH, COLLISION_WIDTH);
	if (NotSafe(&cuboid))
		return TRUE;

	setCUBOID(&cuboid, position->vx, position->vy + movement->vy, position->vz, 
						COLLISION_WIDTH, COLLISION_WIDTH, COLLISION_WIDTH);
	if (NotSafe(&cuboid))
		return TRUE;

	setCUBOID(&cuboid, position->vx, position->vy, position->vz + movement->vz, 
						COLLISION_WIDTH, COLLISION_WIDTH, COLLISION_WIDTH);
	if (NotSafe(&cuboid))
		return TRUE;

	setCUBOID(&cuboid, position->vx + movement->vx, position->vy + movement->vy, position->vz, 
						COLLISION_WIDTH, COLLISION_WIDTH, COLLISION_WIDTH);
	if (NotSafe(&cuboid))
		return TRUE;

	setCUBOID(&cuboid, position->vx + movement->vx, position->vy, position->vz + movement->vz, 
						COLLISION_WIDTH, COLLISION_WIDTH, COLLISION_WIDTH);
	if (NotSafe(&cuboid))
		return TRUE;

	setCUBOID(&cuboid, position->vx, position->vy + movement->vy, position->vz + movement->vz, 
						COLLISION_WIDTH, COLLISION_WIDTH, COLLISION_WIDTH);
	if (NotSafe(&cuboid))
		return TRUE;


	setCUBOID(&cuboid, position->vx + movement->vx, position->vy + movement->vy, position->vz + movement->vz, 
						COLLISION_WIDTH, COLLISION_WIDTH, COLLISION_WIDTH);
	if (NotSafe(&cuboid))
		return TRUE;

	return FALSE;

	


#if 0			// OLD
	int result;
	RECT rectangle;

	setRECT( &rectangle, position->vx, position->vz, 
						COLLISION_WIDTH, COLLISION_WIDTH);
	if (NotSafe(&rectangle))
		return TRUE;

	setRECT( &rectangle, position->vx + movement->vx, position->vz, 
						COLLISION_WIDTH, COLLISION_WIDTH);
	if (NotSafe(&rectangle))
		return TRUE;

	setRECT( &rectangle, position->vx, position->vz + movement->vz, 
						COLLISION_WIDTH, COLLISION_WIDTH);
	if (NotSafe(&rectangle))
		return TRUE;

	setRECT( &rectangle, position->vx + movement->vx, position->vz + movement->vz, 
						COLLISION_WIDTH, COLLISION_WIDTH);
	if (NotSafe(&rectangle))
		return TRUE;

	return FALSE;
#endif
}
	





int NotSafe (CUBOID* cuboid)
{
	int gridX, gridY, gridZ;
	
	if ( (cuboid->x < FACT/2)
		|| (cuboid->y < FACT/2)
		|| (cuboid->z < FACT/2)
		|| (cuboid->x + cuboid->w + FACT/2 > (WORLD_MAX_X-1)*FACT)
		|| (cuboid->y + cuboid->h + FACT/2 > (WORLD_MAX_Y-1)*FACT)
		|| (cuboid->z + cuboid->d + FACT/2 > (WORLD_MAX_Z-1)*FACT) )
			return TRUE;


	gridX = (cuboid->x) / FACT;
	gridY = (cuboid->y) / FACT;
	gridZ = (cuboid->y) / FACT;

	if ((gridX >= 0 && gridX < WORLD_MAX_X) == FALSE)
		return TRUE;
	if ((gridY >= 0 && gridY < WORLD_MAX_Y) == FALSE)
		return TRUE;
	if ((gridZ >= 0 && gridZ < WORLD_MAX_Z) == FALSE)
		return TRUE;

	if (worldmaps[gridX][gridY][gridZ] & WALL)
		return TRUE;

	#if 0
		// NOTE: should test the other 7 corners also
	#endif

	return FALSE;




#if 0				// OLD
	int gridX, gridY;
	
	if ( (rectangle->x < FACT/2)
		|| (rectangle->y < FACT/2)
		|| (rectangle->x + rectangle->w + FACT/2 > (WORLD_MAX_X-1)*FACT)
		|| (rectangle->y + rectangle->h + FACT/2 > (WORLD_MAX_Y-1)*FACT) )
			return TRUE;

		// top left
	gridX = (rectangle->x) / FACT;
	gridY = (rectangle->y) / FACT;

	if ((gridX >= 0 && gridX < WORLD_MAX_X) == FALSE)
		return TRUE;
	if ((gridY >= 0 && gridY < WORLD_MAX_Y) == FALSE)
		return TRUE;

	if (worldmaps[gridX][gridY] & WALL)
		return TRUE;

		// top right
	gridX = (rectangle->x + rectangle->w) / FACT;
	gridY = (rectangle->y) / FACT;

	if ((gridX >= 0 && gridX < WORLD_MAX_X) == FALSE)
		return TRUE;
	if ((gridY >= 0 && gridY < WORLD_MAX_Y) == FALSE)
		return TRUE;

	if (worldmaps[gridX][gridY] & WALL)
		return TRUE;

		// bottom left
	gridX = (rectangle->x) / FACT;
	gridY = (rectangle->y + rectangle->h) / FACT;

	if ((gridX >= 0 && gridX < WORLD_MAX_X) == FALSE)
		return TRUE;
	if ((gridY >= 0 && gridY < WORLD_MAX_Y) == FALSE)
		return TRUE;

	if (worldmaps[gridX][gridY] & WALL)
		return TRUE;

		// bottom right
	gridX = (rectangle->x + rectangle->w) / FACT;
	gridY = (rectangle->y + rectangle->h) / FACT;

	if ((gridX >= 0 && gridX < WORLD_MAX_X) == FALSE)
		return TRUE;
	if ((gridY >= 0 && gridY < WORLD_MAX_Y) == FALSE)
		return TRUE;

	if (worldmaps[gridX][gridY] & WALL)
		return TRUE;

	return FALSE;
#endif
}







void InitialiseView( void )
{
	GsSetProjection(192);	/* プロジェクション設定 */


	PVect.vx = PVect.vy = PVect.vz = 0;

#if 0
	view.vpx = WORLD_MAX_X/2*FACT; view.vpy = POSY; view.vpz = WORLD_MAX_Y/2*FACT;

  	/* Setting focus point location : 注視点パラメータ設定 */
	view.vrx = 0; view.vry = POSY; view.vrz = 0;
	/* Setting bank of SCREEN : 視点の捻りパラメータ設定 */
	view.rz=0;
	view.super = WORLD;		
#endif

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





/* 平行光源設定 */
void InitialiseLighting(void)
{
	/* ライトID０ 設定 */
	TheLights[0].vx = 1; TheLights[0].vy= 1; TheLights[0].vz= 1;

	TheLights[0].r = TheLights[0].g = TheLights[0].b = 0x80;

	GsSetFlatLight(0, &TheLights[0]);


	/* ライトID１ 設定 */
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





/* テクスチャデータをVRAMにロードする */
int InitialiseTexture(long addr)
{
	RECT rect;
	GsIMAGE tim1;

	/* TIMデータのヘッダからテクスチャのデータタイプの情報を得る */
	GsGetTimInfo((u_long *)(addr+4),&tim1);

	rect.x=tim1.px;	/* テクスチャ左上のVRAMでのX座標 */
	rect.y=tim1.py;	/* テクスチャ左上のVRAMでのY座標 */
	rect.w=tim1.pw;	/* テクスチャ幅 */
	rect.h=tim1.ph;	/* テクスチャ高さ */

	/* VRAMにテクスチャをロードする */
	LoadImage(&rect,tim1.pixel);
	/* Exist Color Lookup Table : カラールックアップテーブルが存在する */
	if((tim1.pmode>>3)&0x01) {
		rect.x=tim1.cx;	/* クラット左上のVRAMでのX座標 */
		rect.y=tim1.cy;	/* クラット左上のVRAMでのY座標 */
		rect.w=tim1.cw;	/* Width of CLUT : クラットの幅 */
		rect.h=tim1.ch;	/* Height of CLUT : クラットの高さ */
		/* クラットをロードする */
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








void make_world_maps( void )
{
	int	x, y, z;
	int	fact;

		// this chunk below:
		// for each block in world, set to WALL or nothing
	fact = FACT;
	for( x=0; x<WORLD_MAX_X; x++ ) 
		{		
		for( y=0; y<WORLD_MAX_Y; y++ )
			{
			for( z=0; z<WORLD_MAX_Z; z++ ) 
				{
				worldmaps[x][y][z] = (rand()%24==0)?WALL:0;
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
				World[x][y][z].coord.t[0] = x*fact;
				World[x][y][z].coord.t[1] = y*fact;
				World[x][y][z].coord.t[2] = z*fact;
				}
			}
		}
}


   

  


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

	for (;;)
		{			// get random grid-square
		gridX = 1 + rand() % (WORLD_MAX_X/3);
		gridY = 1 + rand() % (WORLD_MAX_Y/3);
		gridZ = 1 + rand() % (WORLD_MAX_Z/3);

		if (worldmaps[gridX][gridY][gridZ] != WALL)
			{
			success = TRUE;
			break;
			}
		}

	assert(success == TRUE);			// check

		// offset within grid square
	xOffset = FACT/4 + (rand() % (FACT/2));
	yOffset = FACT/4 + (rand() % (FACT/2));
	zOffset = FACT/4 + (rand() % (FACT/2));

	object->position.vx = (FACT * gridX) + xOffset;
	object->position.vy = (FACT * gridY) + yOffset;
	object->position.vz = (FACT * gridZ) + zOffset;



#if 0
	int gridX, gridY;
	int xOffset, yOffset;
	int success = FALSE;

	for (;;)
		{			// get random grid-square
		gridX = 1 + (3 * rand() % (WORLD_MAX_X/3));
		gridY = 1 + (3 * rand() % (WORLD_MAX_Y/3));

		if (worldmaps[gridX][gridY] != WALL
			&& worldmaps[gridX-1][gridY] != WALL
			&& worldmaps[gridX+1][gridY] != WALL
			&& worldmaps[gridX][gridY-1] != WALL
			&& worldmaps[gridX-1][gridY-1] != WALL
			&& worldmaps[gridX+1][gridY-1] != WALL
			&& worldmaps[gridX][gridY+1] != WALL
			&& worldmaps[gridX-1][gridY+1] != WALL
			&& worldmaps[gridX+1][gridY+1] != WALL)
			{
			success = TRUE;
			break;
			}
		}

	assert(success == TRUE);			// check

		// offset within grid square
	xOffset = FACT/4 + (rand() % (FACT/2));
	yOffset = FACT/4 + (rand() % (FACT/2));

	object->position.vx = (FACT * gridX) + xOffset;
	object->position.vz = (FACT * gridY) + yOffset;
#endif
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
		case RANDOM:
			HandleRandomShipsBehaviour(object);
			break;
		case INACTIVE:		// do nowt
			break;
		default:
			assert(FALSE);
		}
}





void HandleWaitingShipsBehaviour (ObjectHandler* object)
{
	int x, z;

		// when player's ship comes close,
		// activate the sleeper -> active pursuer

	x = PlayersShip.position.vx - object->position.vx;
	z = PlayersShip.position.vz - object->position.vz;

	if (abs(x) < 500 && abs(z) < 500)
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
}






	// will turn towards as stat_track, but move if too far away, pursue
void HandleSeekingShipBehaviour (ObjectHandler* object)
{
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
}





void HandleRandomShipsBehaviour (ObjectHandler* object)
{
	int whichRandomAction;

	if (object->lifeTime % object->specialTimeLimit == 0
		|| object->currentActionFlag == NONE)		// start new action
		{
		whichRandomAction = rand() % NUMBER_RANDOM_ACTIONS;

		switch(whichRandomAction)
			{
			case 0:
				object->currentActionFlag = SWIVEL_LEFT;
				break;
			case 1:
				object->currentActionFlag = SWIVEL_RIGHT;
				break;
			case 2:
				object->currentActionFlag = MOVE_FORWARDS;
				break;
			case 3:
				object->currentActionFlag = MOVE_BACKWARDS;
				break;
			case 4:
				object->currentActionFlag = FIRING;
				break;
			}
		}
	else
		{		   // do action
		switch(object->currentActionFlag)
			{
			case SWIVEL_LEFT:
				object->twist.vy -= object->rotationSpeed << 2;
				break;
			case SWIVEL_RIGHT:
				object->twist.vy += object->rotationSpeed << 2;
				break;
			case MOVE_FORWARDS:
				object->velocity.vz += object->movementSpeed << 2;
				break;
			case MOVE_BACKWARDS:
				object->velocity.vz -= object->movementSpeed << 2;
				break;
			case FIRING:
				if (object->framesSinceLastFire > object->firingRate)			// rapid fire
					{
					EnemyShipFiresAShot(object);
					object->framesSinceLastFire = 0;
					}
				break;
			default:
				assert(FALSE);
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
				//printf("Player and Enemy collision\n");
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
				//printf("Player and bullet collision\n");
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
						//printf("ship and bullet collision\n");
						//printf("cube %d, ship %d\n", j, i);
						TheShips[i].currentHealth--;
						}
					}
				}
			}
		}
}


	  





int ObjectsVeryClose (ObjectHandler* first, ObjectHandler* second)
{
	if (abs(first->position.vx - second->position.vx) > 50)
		return FALSE;
	if (abs(first->position.vy - second->position.vy) > 50)
		return FALSE;
	if (abs(first->position.vz - second->position.vz) > 50)
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

		// regenerate random dungeon
	make_world_maps();

	PlayersShip.velocity.vx = 0;
	PlayersShip.velocity.vy = 0;
	PlayersShip.velocity.vz = 0;
	PlayersShip.twist.vx = 0;
	PlayersShip.twist.vy = 0;
	PlayersShip.twist.vz = 0;
	PlayersShip.rotate.vx = 0;
	PlayersShip.rotate.vy = 0;
	PlayersShip.rotate.vz = 0;

	PositionSomewhereNotInAWall(&PlayersShip);
	PlayersShip.allegiance = PLAYER;
	PlayersShip.position.vy = 20;

	PlayersShip.movementSpeed = 48;
	PlayersShip.rotationSpeed = 60;
	PlayersShip.firingRate = 5;
	PlayersShip.framesSinceLastFire = 0;

		

	for (i = 0; i < MAX_BULLETS; i++)
		{
		BringObjectToLife(&TheBullets[i], BULLET, 
			CUBE_MODEL_ADDRESS, 0, NONE);

		SetObjectScaling(&TheBullets[i], ONE>>5, ONE>>5, ONE>>5);

		TheBullets[i].velocity.vx = 0;
		TheBullets[i].velocity.vy = 0;
		TheBullets[i].velocity.vz = 0;
		TheBullets[i].twist.vx = 0;
		TheBullets[i].twist.vy = 0;
		TheBullets[i].twist.vz = 0;
		TheBullets[i].rotate.vx = 0;
		TheBullets[i].rotate.vy = 0;
		TheBullets[i].rotate.vz = 0;

		TheBullets[i].position.vy = 20;
		}


	for (i = 0; i < MAX_CUBES; i++)
		{
		TheCubes[i].velocity.vx = 0;
		TheCubes[i].velocity.vy = 0;
		TheCubes[i].velocity.vz = 0;
		TheCubes[i].twist.vx = 0;
		TheCubes[i].twist.vy = 0;
		TheCubes[i].twist.vz = 0;
		TheCubes[i].rotate.vx = 0;
		TheCubes[i].rotate.vy = 0;
		TheCubes[i].rotate.vz = 0;

		BringObjectToLife(&TheCubes[i], CUBE, 
			CUBE_MODEL_ADDRESS, 0, NONE);

		SetObjectScaling(&TheCubes[i], ONE>>3, ONE>>3, ONE>>3);
		TheCubes[i].position.vy = 20;
		}




	for (i = 0; i < MAX_SHIPS; i++)
		{
		TheShips[i].velocity.vx = 0;
		TheShips[i].velocity.vy = 0;
		TheShips[i].velocity.vz = 0;
		TheShips[i].twist.vx = 0;
		TheShips[i].twist.vy = 0;
		TheShips[i].twist.vz = 0;
		TheShips[i].rotate.vx = 0;
		TheShips[i].rotate.vy = 0;
		TheShips[i].rotate.vz = 0;

		BringObjectToLife(&TheShips[i], ENEMY, 
			SHIP_MODEL_ADDRESS, 0, NONE);

		TheShips[i].initialHealth = 1;
		TheShips[i].currentHealth = 1;
			// set one of three strategies randomly
		TheShips[i].strategyFlag = rand() % NUMBER_ENEMY_STRATEGIES;
		InitialiseShipAccordingToStrategy(&TheShips[i]);

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

	



int FindDirectionToNearestLivingEnemy (int* exactDirection)
{
	int direction;
	int shipID;
	int x, y, z;
	int relativeX, relativeZ;
	int distanceSquared, minDistanceSquared, firstFlag = TRUE;
	int turnAngle, positionalAngle;
	int simplerAngle;
	int i;

	x = PlayersShip.position.vx;
	y = PlayersShip.position.vy;
	z = PlayersShip.position.vz;
	shipID = -1;
	for (i = 0; i < MAX_SHIPS; i++)
		{
		if (TheShips[i].alive == TRUE)
			{
			distanceSquared 
				= pow((x - TheShips[i].position.vx), 2)
				+ pow((y - TheShips[i].position.vy), 2)
				+ pow((z - TheShips[i].position.vz), 2);

			if (firstFlag == TRUE)
				{
				shipID = i;
				minDistanceSquared = distanceSquared;
				firstFlag = FALSE;
				}
			else
				{
				if (distanceSquared < minDistanceSquared)
					{
					shipID = i;
					minDistanceSquared = distanceSquared;
					}
				}
			}
		}

	if (shipID == -1)
		{
		printf("FOUND NO SHIPS\n");
		direction = NONE;
		}
	else
		{
		relativeX = TheShips[shipID].position.vx - x;
		relativeZ = TheShips[shipID].position.vz - z;

		positionalAngle = SUBratan(relativeX, relativeZ);
		positionalAngle += 2048;
		positionalAngle &= 4095;
		turnAngle = positionalAngle - PlayersShip.rotate.vy;
		for (;;)
			{
			if (turnAngle >= 2049)
				turnAngle -= 4096;
			else if (turnAngle <= -2048)
				turnAngle += 4096;
			else
				break;
			}
		assert(turnAngle <= 2048);
		assert(turnAngle >= -2047);

		*exactDirection = turnAngle;

		simplerAngle = turnAngle >> 8;		// divide by 256

#if 0
		printf("turn angle: %d\n", turnAngle);
		printf("simplerAngle: %d\n", simplerAngle);
#endif 

		switch(simplerAngle)
			{
			case 0: 
			case 1: 
			case -1:
				direction = NORTH_8_FOLD;
				break;
			case 2:
				direction = NORTH_WEST_8_FOLD;
				break;
			case 3: 
			case 4: 
			case 5:
				direction = WEST_8_FOLD;
				break;
			case 6:
				direction = SOUTH_WEST_8_FOLD;
				break;
			case 7: 
			case 8:
			case -7:
			case -8:
				direction = SOUTH_8_FOLD;
				break;
			case -2:
				direction = NORTH_EAST_8_FOLD;
				break;
			case -3:
			case -4:
			case -5:
				direction = EAST_8_FOLD;
				break;
			case -6:
				direction = SOUTH_EAST_8_FOLD;
				break;
			default:
				//printf("DEFAULTING OUT\n");
				assert(FALSE);
			}
		}

	return direction;
}




void PrintCompassDirectionToNearestEnemy (int direction)
{
	if (direction == NONE)
		return;

	switch(direction)
		{
		case NORTH_8_FOLD:
			FntPrint("north\n");
			break;
		case NORTH_WEST_8_FOLD:
			FntPrint("north-west\n");
			break;
		case WEST_8_FOLD:
			FntPrint("west\n");
			break;
		case SOUTH_WEST_8_FOLD:
			FntPrint("south-west\n");
			break;
		case SOUTH_8_FOLD:
			FntPrint("south\n");
			break;
		case SOUTH_EAST_8_FOLD:
			FntPrint("south-east\n");
			break;
		case EAST_8_FOLD:
			FntPrint("east\n");
			break;
		case NORTH_EAST_8_FOLD:
			FntPrint("north-east\n");
			break;
		default:
			assert(FALSE);
		}
}

