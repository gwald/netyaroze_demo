/************************************************************
 *															*
 *						Tuto4.c								*
 *						3-D object manipulation				*
 *			   											    *															*
 *				LPGE     27/11/96							*		
 *															*
 *	Copyright (C) 1996 Sony Computer Entertainment Inc.		*
 *	All Rights Reserved										*
 *															*
 ***********************************************************/


	   

// changes from the dungeon demos:
// now all objects are done via ObjectHandler
// frame rate has suffered but one MASSIVE optimisation
// still beckons: using correct-shaped squares
// rather than continually rotating 'dop5' ie 'ten.tmd'



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




typedef struct
{
	int thetaX, thetaY, thetaZ;
} ANGLE;

#define MAX_SECTIONS 36

int NumberOfSections;
int SectionLength;		// depth forwards
int SectionSize;		// width and height
	
MATRIX TrackMatrix;
GsCOORDINATE2 TrackCoordinates;




ANGLE SectionAngles[MAX_SECTIONS-1];		// first member is angle of second section wrt first




#define PIH 320				// screen width and height
#define	PIV	240



#define PACKETMAX	2048		/* Max GPU packets */
#define OBJMAX		256		/* Max Objects */	  // USED TO BE 256
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
#define WORLD_MAX_X	15
#define WORLD_MAX_Y	15
#define WORLD_MAX_Z 15
#define	WEST		112
#define	POSY		28
#define	WALL		0x01



static char worldmaps[WORLD_MAX_X][WORLD_MAX_Y][WORLD_MAX_Z];
static GsCOORDINATE2 World[WORLD_MAX_X][WORLD_MAX_Y][WORLD_MAX_Z];



ObjectHandler ConsideredObjects[OBJMAX];


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
#define SQUARE 1


ObjectHandler PlayersShip;




#define MAX_SQUARES 256
ObjectHandler TheSquares[MAX_SQUARES];


GsCOORDINATE2 SquaresSuperCoords[MAX_SQUARES];




 	 



	
  

	 

	

	 



		



/****************************************************************************
					prototypes
****************************************************************************/


void main (void);
	   

void NewDrawWorldMaps( GsOT* ot );


void DealWithControllerPad(void);

void NewMoveLightingSpot (void);






void InitialiseAll(void);


void InitialiseTrack (void);
void CreateTheTrackObjects (void);
void CreateTrackSection (int* squareNumber);



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


void UpdateModelCoords (SVECTOR* rotationVector,
									GsCOORDINATE2* coordSystem);


void MoveRelativeToSelf (VECTOR* relativeMovement, MATRIX* matrix);
void RotateRelativeToSelf (VECTOR* relativeTwist, MATRIX* matrix);





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


void PositionSomewhereNotInAWall (ObjectHandler* object);

	 
		 
int ObjectsVeryClose (ObjectHandler* first, ObjectHandler* second);


   
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

		FntPrint("Num models: %d\nHsync = %d\n",nModels,hsync );
		FntPrint("frame: %d\n", frameNumber);
		FntPrint("player health: %d\n", PlayersShip.currentHealth);	 
		FntPrint("player score: %d\n", PlayersShip.meritRating);
				
	
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

		NewMoveLightingSpot();
		
		nModels = NewCalculateWorldData();			

		GsSetRefView2(&view);

		GsSetWorkBase((PACKET*)packetArea[side]);

		GsClearOt(0,0,&Wot[side]);
		GsClearOt(0,0,&Sot[side]);


#if 0		// OLD
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
#endif

		
		for (i = 0; i < nModels; i++)
			{
			object = &ConsideredObjects[i];
			UpdateObjectCoordinates2 ( &object->rotate,
							&object->position,
							&object->coord);
			GsGetLs( &(object->coord), &tmpls);	
			GsSetLightMatrix(&tmpls);	
			GsSetLsMatrix(&tmpls);	   
			GsSortObject4( &(object->handler), &Wot[side], 3, getScratchAddr(0));
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

				// rotation
			if (pad & PADRup)
				PlayersShip.twist.vx += PlayersShip.rotationSpeed * speedMultiplier;
			if (pad & PADRdown)
				PlayersShip.twist.vx -= PlayersShip.rotationSpeed * speedMultiplier;
			if (pad & PADRleft)
				PlayersShip.twist.vy -= PlayersShip.rotationSpeed * speedMultiplier;
			if (pad & PADRright)
				PlayersShip.twist.vy += PlayersShip.rotationSpeed * speedMultiplier;
			if (pad & PADR1)
				PlayersShip.twist.vz += PlayersShip.rotationSpeed * speedMultiplier;
			if (pad & PADR2)
				PlayersShip.twist.vz -= PlayersShip.rotationSpeed * speedMultiplier;

			break;
		default:
			assert(FALSE);
		}

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
// theship.tmd point towards -z not towards +z


	
	// this function below essentially does world-clipping:
	// only the objects within a given cuboid around the player's ship
	// are entered into the ListOfObjectsToBeDisplayed
	// to do this without the cubic worldmaps array:
	// just test each object to see if inside cuboid or not
	// (nested ifs -> quicker rejection)

int NewCalculateWorldData (void)
{	



#if 0		// NO LONGER BOTHERING WITH WALLS AT ALL
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
#endif

	return 0;
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
		GsInitCoordinate2( &World[px][py][pz], &ConsideredObjects[n].coord);
		ConsideredObjects[n].position.vx = -FACT/2;
		ConsideredObjects[n].position.vy = 0;
		ConsideredObjects[n].position.vz = 0;
		GsLinkObject4((long)dop5, &ConsideredObjects[n].handler,0);
		ConsideredObjects[n].handler.coord2 = &ConsideredObjects[n].coord;
		ConsideredObjects[n].handler.attribute = atrb;
		ConsideredObjects[n].rotate.vx = 0;
		ConsideredObjects[n].rotate.vy = 0;
		ConsideredObjects[n].rotate.vz = 3 * ONE/4;
		n++;
	}
   

	if(px==WORLD_MAX_X-1 || worldmaps[px+1][py][pz]&WALL) {	   // wall to right
		GsInitCoordinate2( &World[px][py][pz], &ConsideredObjects[n].coord);
		ConsideredObjects[n].position.vx = FACT/2;
		ConsideredObjects[n].position.vy = 0;
		ConsideredObjects[n].position.vz = 0;
		GsLinkObject4((long)dop5, &ConsideredObjects[n].handler,0);
		ConsideredObjects[n].handler.coord2 = &ConsideredObjects[n].coord;
		ConsideredObjects[n].handler.attribute = atrb;
		ConsideredObjects[n].rotate.vx = 0;
		ConsideredObjects[n].rotate.vy = 0;
		ConsideredObjects[n].rotate.vz = ONE/4;
		n++;
	}
	

	if (py==0 || worldmaps[px][py-1][pz]&WALL) {			 // wall above
		GsInitCoordinate2( &World[px][py][pz], &ConsideredObjects[n].coord);
		ConsideredObjects[n].position.vx = 0;
		ConsideredObjects[n].position.vy = -FACT/2;
		ConsideredObjects[n].position.vz = 0;
		GsLinkObject4((long)dop5, &ConsideredObjects[n].handler,0);
		ConsideredObjects[n].handler.coord2 = &ConsideredObjects[n].coord;
		ConsideredObjects[n].handler.attribute = atrb;
		ConsideredObjects[n].rotate.vx = 0;
		ConsideredObjects[n].rotate.vy = 0;
		ConsideredObjects[n].rotate.vz = 0;
		n++;
	}
	
	if (py==WORLD_MAX_Y-1 || worldmaps[px][py+1][pz]&WALL) {	 // wall below
		GsInitCoordinate2( &World[px][py][pz], &ConsideredObjects[n].coord);
		ConsideredObjects[n].position.vx = 0;
		ConsideredObjects[n].position.vy = FACT/2;
		ConsideredObjects[n].position.vz = 0;
		GsLinkObject4((long)dop5, &ConsideredObjects[n].handler,0);
		ConsideredObjects[n].handler.coord2 = &ConsideredObjects[n].coord;
		ConsideredObjects[n].handler.attribute = atrb;
		ConsideredObjects[n].rotate.vx = ONE/2;
		ConsideredObjects[n].rotate.vy = 0;
		ConsideredObjects[n].rotate.vz = 0;
		n++;
	}
	

	if (pz==0 || worldmaps[px][py][pz-1]&WALL) {			 // wall to south
		GsInitCoordinate2( &World[px][py][pz], &ConsideredObjects[n].coord);
		ConsideredObjects[n].position.vx = 0;
		ConsideredObjects[n].position.vy = 0;
		ConsideredObjects[n].position.vz = -FACT/2;
		GsLinkObject4((long)dop5, &ConsideredObjects[n].handler,0);
		ConsideredObjects[n].handler.coord2 = &ConsideredObjects[n].coord;
		ConsideredObjects[n].handler.attribute = atrb;
		ConsideredObjects[n].rotate.vx = ONE/4;
		ConsideredObjects[n].rotate.vy = 0;
		ConsideredObjects[n].rotate.vz = 0;
		n++;
	}
   

	if(pz==WORLD_MAX_Z-1 || worldmaps[px][py][pz+1]&WALL) {	 // wall to north
		GsInitCoordinate2( &World[px][py][pz], &ConsideredObjects[n].coord);
		ConsideredObjects[n].position.vx = 0;
		ConsideredObjects[n].position.vy = 0;
		ConsideredObjects[n].position.vz = FACT/2;
		GsLinkObject4((long)dop5, &ConsideredObjects[n].handler,0);
		ConsideredObjects[n].handler.coord2 = &ConsideredObjects[n].coord;
		ConsideredObjects[n].handler.attribute = atrb;
		ConsideredObjects[n].rotate.vx = 3 * ONE/4;
		ConsideredObjects[n].rotate.vy = 0;
		ConsideredObjects[n].rotate.vz = 0;
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
	  
	GenerateRandomWorld();	

	//nModels = calc_world_data();
	printf("iseAll: 6\n");

	InitialiseObjects();

	InitialiseTrack();
		 
	MainMode = MOVE_SHIP;

	FntLoad( 960, 256);
	FntOpen( 0, 0, 256, 200, 0, 512);		  // 64, 32 as first two args before
}





void InitialiseTrack (void)
{
	int i;

	NumberOfSections = 16;	// make sure we get first one correct first
	SectionLength = FACT*4;	  
	SectionSize = FACT*2;	

	GsInitCoordinate2(WORLD, &TrackCoordinates);
	
		// this matrix: as if track itself was a 3D object moving round world
		// with self-relative movement and translation
	TrackMatrix.m[0][0] = ONE;
	TrackMatrix.m[0][1] = 0;
	TrackMatrix.m[0][2] = 0;
	TrackMatrix.m[1][0] = 0;
	TrackMatrix.m[1][1] = ONE;
	TrackMatrix.m[1][2] = 0;
	TrackMatrix.m[2][0] = 0;
	TrackMatrix.m[2][1] = 0;
	TrackMatrix.m[2][2] = ONE;
	
		// starting position	
	TrackMatrix.t[0] = 5 * FACT;
	TrackMatrix.t[1] = 5 * FACT;
	TrackMatrix.t[2] = 5 * FACT;

	TrackCoordinates.coord = TrackMatrix;

		// now create track itself
		// this track just a short curve to the right
	for (i = 0; i < NumberOfSections-1; i++)
		{
		SectionAngles[i].thetaX = 0;		
		SectionAngles[i].thetaY = ONE/16;	  // 22.5 degrees	
		SectionAngles[i].thetaZ = 0;	   
		}

	CreateTheTrackObjects();
}

			   
			   
			  
			 


void CreateTheTrackObjects (void)
{
	int i;
	int count;
	VECTOR movementForward, twistSideWays;
	int squareNumber = 0;
	int twistFlag;
	int extraBit;
		
		// firstly check validity of angular description of track
	for (i = 0; i < MAX_SECTIONS-1; i++)
		{
		count = 0;
		if (SectionAngles[i].thetaX != 0)
			count++;
		if (SectionAngles[i].thetaY != 0)
			count++;
		if (SectionAngles[i].thetaZ != 0)
			count++;
		assert(count <= 1);
		}

		// first section
	CreateTrackSection( &squareNumber);


#if 0
		// create first section
		// m[0][0] <-> m[0][2] is xAxis, and so forth
		// i.e. the TrackMatrix specifies x, y and z axes
		// of hypothetical object following central path of track
	TheSquares[0].position.vx = TrackMatrix.t[0]
		+ (TrackMatrix.m[0][0] * (SectionSize / 2) / ONE);
	TheSquares[0].position.vy = TrackMatrix.t[1]
		+ (TrackMatrix.m[0][1] * (SectionSize / 2) / ONE);
	TheSquares[0].position.vz = TrackMatrix.t[2]
		+ (TrackMatrix.m[0][2] * (SectionSize / 2) / ONE);
	TheSquares[0].rotate.vz = ONE/4;


	TheSquares[1].position.vx = TrackMatrix.t[0]
		- (TrackMatrix.m[0][0] * (SectionSize / 2) / ONE);
	TheSquares[1].position.vy = TrackMatrix.t[1]
		- (TrackMatrix.m[0][1] * (SectionSize / 2) / ONE);
	TheSquares[1].position.vz = TrackMatrix.t[2]
		- (TrackMatrix.m[0][2] * (SectionSize / 2) / ONE);
	TheSquares[1].rotate.vz = 3 * ONE/4;


	TheSquares[2].position.vx = TrackMatrix.t[0]
		+ (TrackMatrix.m[1][0] * (SectionSize / 2) / ONE);
	TheSquares[2].position.vy = TrackMatrix.t[1]
		+ (TrackMatrix.m[1][1] * (SectionSize / 2) / ONE);
	TheSquares[2].position.vz = TrackMatrix.t[2]
		+ (TrackMatrix.m[1][2] * (SectionSize / 2) / ONE);
	TheSquares[2].rotate.vz = ONE/2;


	TheSquares[3].position.vx = TrackMatrix.t[0]
		- (TrackMatrix.m[1][0] * (SectionSize / 2) / ONE);
	TheSquares[3].position.vy = TrackMatrix.t[1]
		- (TrackMatrix.m[1][1] * (SectionSize / 2) / ONE);
	TheSquares[3].position.vz = TrackMatrix.t[2]
		- (TrackMatrix.m[1][2] * (SectionSize / 2) / ONE);
#endif


								
								
	for (i = 0; i < NumberOfSections-1; i++)
		{
		if (SectionAngles[i].thetaX != 0)
			{
			extraBit = rsin(SectionAngles[i].thetaX/2) * SectionSize / (2 * ONE);
			twistFlag = TRUE;
			}
		else if (SectionAngles[i].thetaY != 0)
			{
			extraBit = rsin(SectionAngles[i].thetaY/2) * SectionSize / (2 * ONE);
			twistFlag = TRUE;
			}
		else if (SectionAngles[i].thetaZ != 0)
			{
			extraBit = rsin(SectionAngles[i].thetaZ/2) * SectionSize / (2 * ONE);
			twistFlag = TRUE;
			}
		else
			twistFlag = FALSE;

		if (twistFlag)
			{
			movementForward.vx = 0;
			movementForward.vy = 0;
			movementForward.vz = SectionLength/2 + extraBit;	  // SHOULD BE / 2: just TESTING

			MoveRelativeToSelf( &movementForward, &TrackMatrix);

			twistSideWays.vx = SectionAngles[i].thetaX;
			twistSideWays.vy = SectionAngles[i].thetaY;
			twistSideWays.vz = SectionAngles[i].thetaZ;
				
			RotateRelativeToSelf ( &twistSideWays, &TrackMatrix);

			MoveRelativeToSelf( &movementForward, &TrackMatrix);
			}
		else		// straight on
			{
			movementForward.vx = 0;
			movementForward.vy = 0;
			movementForward.vz = SectionLength;

			MoveRelativeToSelf( &movementForward, &TrackMatrix);
			}

	
		#if 0
		if (SectionAngles[i].thetaX != 0 
			|| SectionAngles[i].thetaY != 0
			|| SectionAngles[i].thetaZ != 0)
			{
			movementForward.vx = 0;
			movementForward.vy = 0;
			movementForward.vz = SectionLength;	  // SHOULD BE / 2: just TESTING

			MoveRelativeToSelf( &movementForward, &TrackMatrix);

			twistSideWays.vx = SectionAngles[i].thetaX;
			twistSideWays.vy = SectionAngles[i].thetaY;
			twistSideWays.vz = SectionAngles[i].thetaZ;
				
			RotateRelativeToSelf ( &twistSideWays, &TrackMatrix);

			MoveRelativeToSelf( &movementForward, &TrackMatrix);
			}
		else		// straight on
			{
			movementForward.vx = 0;
			movementForward.vy = 0;
			movementForward.vz = SectionLength;

			MoveRelativeToSelf( &movementForward, &TrackMatrix);
			}
		#endif

		CreateTrackSection( &squareNumber);
		} 

		// turn display off for all other squares
		// DOES NOT WORK for some reason
	for (i = squareNumber; i < MAX_SQUARES; i++)
		{
		TheSquares[i].handler.attribute &= GsDOFF;
		}
}







void CreateTrackSection (int* squareNumber)
{
	int currentSquare;

	currentSquare = *squareNumber;


	TheSquares[currentSquare].position.vx = (SectionSize / 2);
	TheSquares[currentSquare].position.vy = 0;
	TheSquares[currentSquare].position.vz = 0;

	SetObjectScaling( &TheSquares[currentSquare], 
	  	ONE * SectionSize / FACT, ONE, ONE * SectionLength / FACT);

	TheSquares[currentSquare].twist.vz = ONE/4;

	TheSquares[currentSquare].handler.attribute &= GsDIV2;

	//dumpVECTOR(&TheSquares[currentSquare].position);
	SquaresSuperCoords[currentSquare].coord = TrackMatrix;
	TheSquares[currentSquare].coord.super = &SquaresSuperCoords[currentSquare];

	currentSquare++;



	TheSquares[currentSquare].position.vx = -(SectionSize / 2);
	TheSquares[currentSquare].position.vy = 0;
	TheSquares[currentSquare].position.vz = 0;

	SetObjectScaling( &TheSquares[currentSquare], 
	  	ONE * SectionSize / FACT, ONE, ONE * SectionLength / FACT);

	TheSquares[currentSquare].twist.vz = 3 * ONE/4;

	TheSquares[currentSquare].handler.attribute &= GsDIV2;

	//dumpVECTOR(&TheSquares[currentSquare].position);
	SquaresSuperCoords[currentSquare].coord = TrackMatrix;
	TheSquares[currentSquare].coord.super = &SquaresSuperCoords[currentSquare];

	currentSquare++;




	TheSquares[currentSquare].position.vx = 0;
	TheSquares[currentSquare].position.vy = (SectionSize / 2);
	TheSquares[currentSquare].position.vz = 0;

	SetObjectScaling( &TheSquares[currentSquare], 
	  	ONE * SectionSize / FACT, ONE, ONE * SectionLength / FACT);

	TheSquares[currentSquare].twist.vz = ONE/2;

	TheSquares[currentSquare].handler.attribute &= GsDIV2;

	//dumpVECTOR(&TheSquares[currentSquare].position);
	SquaresSuperCoords[currentSquare].coord = TrackMatrix;
	TheSquares[currentSquare].coord.super = &SquaresSuperCoords[currentSquare];

	currentSquare++;




	TheSquares[currentSquare].position.vx = 0;
	TheSquares[currentSquare].position.vy = -(SectionSize / 2);
	TheSquares[currentSquare].position.vz = 0;

	SetObjectScaling( &TheSquares[currentSquare], 
	  	ONE * SectionSize / FACT, ONE, ONE * SectionLength / FACT);

	TheSquares[currentSquare].handler.attribute &= GsDIV2;

	//dumpVECTOR(&TheSquares[currentSquare].position);
	SquaresSuperCoords[currentSquare].coord = TrackMatrix;
	TheSquares[currentSquare].coord.super = &SquaresSuperCoords[currentSquare];

	currentSquare++;





#if 0

		// m[0][0] <-> m[0][2] is xAxis, and so forth
		// i.e. the TrackMatrix specifies x, y and z axes
		// of hypothetical object following central path of track
							   
		// positioning: centre of square is track section centre
		// plus offset along either y or x axis
	TheSquares[currentSquare].position.vx = TrackMatrix.t[0]
		+ (TrackMatrix.m[0][0] * (SectionSize / 2) / ONE);
	TheSquares[currentSquare].position.vy = TrackMatrix.t[1]
		+ (TrackMatrix.m[1][0] * (SectionSize / 2) / ONE);
	TheSquares[currentSquare].position.vz = TrackMatrix.t[2]
		+ (TrackMatrix.m[2][0] * (SectionSize / 2) / ONE);

	SetObjectScaling( &TheSquares[currentSquare], 
	  	ONE * SectionSize / FACT, ONE, ONE * SectionLength / FACT);

	TheSquares[currentSquare].twist.vz = ONE/4;

	TheSquares[currentSquare].handler.attribute &= GsDIV2;

	//dumpVECTOR(&TheSquares[currentSquare].position);

	currentSquare++;





	TheSquares[currentSquare].position.vx = TrackMatrix.t[0]
		- (TrackMatrix.m[0][0] * (SectionSize / 2) / ONE);
	TheSquares[currentSquare].position.vy = TrackMatrix.t[1]
		- (TrackMatrix.m[1][0] * (SectionSize / 2) / ONE);
	TheSquares[currentSquare].position.vz = TrackMatrix.t[2]
		- (TrackMatrix.m[2][0] * (SectionSize / 2) / ONE);

	SetObjectScaling( &TheSquares[currentSquare], 
	  	ONE * SectionSize / FACT, ONE, ONE * SectionLength / FACT);

	TheSquares[currentSquare].twist.vz = 3 * ONE/4;

	TheSquares[currentSquare].handler.attribute &= GsDIV2;

	//dumpVECTOR(&TheSquares[currentSquare].position);

	currentSquare++;






	TheSquares[currentSquare].position.vx = TrackMatrix.t[0]
		+ (TrackMatrix.m[0][1] * (SectionSize / 2) / ONE);
	TheSquares[currentSquare].position.vy = TrackMatrix.t[1]
		+ (TrackMatrix.m[1][1] * (SectionSize / 2) / ONE);
	TheSquares[currentSquare].position.vz = TrackMatrix.t[2]
		+ (TrackMatrix.m[2][1] * (SectionSize / 2) / ONE);

	TheSquares[currentSquare].twist.vz = ONE/2;

	SetObjectScaling( &TheSquares[currentSquare], 
	  	ONE * SectionSize / FACT, ONE, ONE * SectionLength / FACT);

	TheSquares[currentSquare].handler.attribute &= GsDIV2;

	//dumpVECTOR(&TheSquares[currentSquare].position);
	
	currentSquare++;






	TheSquares[currentSquare].position.vx = TrackMatrix.t[0]
		- (TrackMatrix.m[0][1] * (SectionSize / 2) / ONE);
	TheSquares[currentSquare].position.vy = TrackMatrix.t[1]
		- (TrackMatrix.m[1][1] * (SectionSize / 2) / ONE);
	TheSquares[currentSquare].position.vz = TrackMatrix.t[2]
		- (TrackMatrix.m[2][1] * (SectionSize / 2) / ONE);

	SetObjectScaling( &TheSquares[currentSquare], 
	  	ONE * SectionSize / FACT, ONE, ONE * SectionLength / FACT);

	TheSquares[currentSquare].handler.attribute &= GsDIV2;

	//dumpVECTOR(&TheSquares[currentSquare].position);
  
	currentSquare++;
#endif




	*squareNumber += 4;
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

	//PositionSomewhereNotInAWall(&PlayersShip);
	PlayersShip.position.vx = 5 * FACT;
	PlayersShip.position.vy = 5 * FACT;
	PlayersShip.position.vz = 7 * FACT / 2;

	PlayersShip.initialHealth = 1000;
	PlayersShip.currentHealth = 1000;
	PlayersShip.allegiance = PLAYER;

	PlayersShip.movementSpeed = 22;
	PlayersShip.rotationSpeed = 55;
	PlayersShip.firingRate = 4;
	PlayersShip.framesSinceLastFire = 0;
	//PlayersShip.movementMomentumFlag = TRUE;
	PlayersShip.twist.vy = ONE/2;		// turn to face into tunnel

	
	printf("2\n");



	for (i = 0; i < MAX_SQUARES; i++)
		{
		InitSingleObject(&TheSquares[i]);

			// model 5: just a square
		BringObjectToLife(&TheSquares[i], SQUARE, 
			MODEL5_ADDR, 0, NONE);

		RegisterObjectIntoObjectArray(&TheSquares[i]);
		}
	printf("3\n");
		

	   

	LinkAllObjectsToModels();
	printf("6\n");
	LinkAllObjectsToTheirCoordinateSystems();
	printf("7\n");
}

		 


 



void HandleAllObjects (void)
{
	ObjectHandler* object;
	int gridX, gridY, gridZ;
	int i;

	//CheckCollisions();	 // between non-square objects

#if 0
	if (frameNumber % 15 == 0)
		{
		printf("matrix before vy ONE/8 :-\n");
		dumpMATRIX(&temp111);
		printf("matrix after vy ONE/8 :-\n");
		dumpMATRIX(&temp222);
		}
#endif

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
						
						#if 0	
						if (frameNumber % 15 == 0)
							{
							VECTOR temp;

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
							}
						#endif

						SortObjectSize(object);

						#if 0
						KeepWithinRange(object->position.vx, -FACT/2, WORLD_MAX_X * FACT -FACT/2);
						KeepWithinRange(object->position.vy, -FACT/2, WORLD_MAX_Y * FACT -FACT/2);
						KeepWithinRange(object->position.vz, -FACT/2, WORLD_MAX_Z * FACT -FACT/2);
						#endif

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
						#if 0	
							// position and rotation are world-relative
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
						break;
					default:
						{
						printf("dodgy object type: %d\n", object->type);
						assert(FALSE);
						}
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



#if 0	 // NEED NEW COLLISION_DETECTOR: walls of tunnel
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
#endif

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
	

#if 0	 // NEED NEW COLLISION_DETECTOR: walls of tunnel
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
#endif

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






void MoveRelativeToSelf (VECTOR* relativeMovement, MATRIX* matrix)
{
		// find the object-local velocity in super coordinate terms
	ApplyMatrixLV(matrix, relativeMovement, &realMovement);

	matrix->t[0] += realMovement.vx;
	matrix->t[1] += realMovement.vy;
	matrix->t[2] += realMovement.vz;
}




void RotateRelativeToSelf (VECTOR* relativeTwist, MATRIX* matrix)
{
	xVector.vx = relativeTwist->vx;
	xVector.vy = 0;
	xVector.vz = 0;
	yVector.vx = 0;
	yVector.vy = relativeTwist->vy;
	yVector.vz = 0;
	zVector.vx = 0;
	zVector.vy = 0;
	zVector.vz = relativeTwist->vz;

	RotMatrix(&xVector, &xMatrix);
	RotMatrix(&yVector, &yMatrix);
	RotMatrix(&zVector, &zMatrix);

		// to get world-relative rotations instead:
		// use same function with order of first two arguments switched

	MulMatrix0(matrix, &xMatrix, matrix);
	MulMatrix0(matrix, &yMatrix, matrix);
	MulMatrix0(matrix, &zMatrix, matrix);
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
	fogparam.dqa = 0;//-960;
	fogparam.dqb = 0;//5120*5120;
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

	for( x=0; x<WORLD_MAX_X; x++ ) 
		{		
		for( y=0; y<WORLD_MAX_Y; y++ )
			{
			for( z=0; z<WORLD_MAX_Z; z++ ) 
				{
				worldmaps[x][y][z] = 0;
				}
			}
		}
		
		
#if 0			// REMOVE BLOCKS TO MAKE WAY FOR TUNNEL
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
#endif

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

   


	// random positioning
void PositionSomewhereNotInAWall (ObjectHandler* object)
{
	int gridX, gridY, gridZ;
	int xOffset, yOffset, zOffset;
	int success = FALSE;

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



