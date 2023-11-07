/************************************************************
 *															*
 *						Tuto1.c								*
 *						3-D object manipulation				*
 *			   											    *															*
 *				LPGE     08/01/97							*		
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




#define PACKETMAX 2048		/* Max GPU packets */
#define PACKETMAX2 (PACKETMAX*24)



	// texture for floor
#define MAIN_TEXTURE_ADDRESS	0x800a0000	
GsIMAGE MainTextureInfo;
 
#define STARS_TEXTURE_ADDRESS 0x800b0000
GsIMAGE StarsTextureInfo;

#define CIRCLE_TEXTURE_ADDRESS 0x800b9000
GsIMAGE CircleTextureInfo;

#define DOTS_TEXTURE_ADDRESS 0x800ba000
GsIMAGE DotsTextureInfo;

#define FIRE_TEXTURE_ADDRESS 0x800bb000
GsIMAGE FireTextureInfo;

#define MULTIPLE_TEXTURE_ADDRESS 0x800d0000
GsIMAGE MultipleTextureInfo;




#define SHIP_MODEL_ADDRESS 0x80096000
#define CUBE_MODEL_ADDRESS 0x80097000
#define SQUARE_MODEL_ADDRESS 0x80094000
#define MULTIPLE_MODEL_ADDRESS 0x800c0000


#define OT_LENGTH	9		


		
static GsOT Wot[2];			/* Handler of OT */
static GsOT_TAG wtags[2][1<<OT_LENGTH]; 


	 
		
static GsRVIEW2 view;
int ViewPointX, ViewPointZ;

int ProjectionDistance;	

	// for subdividing squares on plane
int VeryCloseDistance;
int SimpleClipDistance;

int ViewMode;

#define VIEW_1 1

#define MAX_VIEW_MODE (VIEW_1)

u_long framesSinceLastViewSwitch;
u_long ViewChangePauseTime;	

int ViewpointZDistance, ViewpointYDistance, ViewpointXDistance;



GsF_LIGHT TheLights[3];	




static PACKET packetArea[2][PACKETMAX2]; /* GPU PACKETS AREA */


 

GsFOGPARAM fogparam;

GsLINE line;

GsLINE ArrowLines[3];

GsBOXF box[2];

short lightmode;



#define	GRID_SIZE 128

#define WORLD_MAX_X	20
#define WORLD_MAX_Y	10
#define WORLD_MAX_Z 20




	

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
#define FLAT_BASE 5
#define TETRAHEDRON 6
#define TIE_FIGHTER 7







ObjectHandler PlayersShip;

#define MAX_BULLETS 50
ObjectHandler TheBullets[MAX_BULLETS];

	

#define MAX_CUBES 20
ObjectHandler TheCubes[MAX_CUBES];


#define MAX_SQUARES 400			  // MUST be divisible by 4
ObjectHandler TheSquares[MAX_SQUARES];


#define MAX_FLAT_BASES 12
ObjectHandler FlatBases[MAX_FLAT_BASES];


#define MAX_TETRAHEDRONS 64
ObjectHandler Tetrahedrons[MAX_TETRAHEDRONS];


#define MAX_TIE_FIGHTERS 24
ObjectHandler TieFighters[MAX_TIE_FIGHTERS];








	// array of pointers for graphical world clipping
ObjectHandler* DisplayedObjects[MAX_OBJECTS];



   

 	 



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






			 
	 

VECTOR scalingVector;		// for the squares -> smoother walls

		// z value
u_long FurthestBack = (1<<OT_LENGTH)-1;


	



GsSPRITE TestSprite1, TestSprite2;





int SectionLength;
int TunnelSize;			// width and height, i.e. x and y extent
int NumberTunnelSections;
int TunnelSpeed;

	// intrinsic size of square (ten.tmd, SQUARE_MODEL_ADDRESS)
#define SQUARE_SIZE 128

		  




/****************************************************************************
					prototypes
****************************************************************************/


void main (void);

u_short SortSpriteObjectPosition (ObjectHandler* object);




int PerformWorldClipping (ObjectHandler** firstArray, ObjectHandler** secondArray);





void CheckForEndOfLevel (void);


void DealWithControllerPad(void);
void PlayerFiresAShot (void);
void EnemyShipFiresAShot (ObjectHandler* enemy);
  


void InitialiseAll(void);


void InitialiseTestSprite1 (void);
void InitialiseTestSprite2 (void);

void InitialiseObjects (void);

void InitialiseTunnel (void);
void CreateTheTunnel (void);

void UpdateTheTunnel (void);

	  	  


void HandleAllObjects (void);


void HandleThePlayer (ObjectHandler* object);

void HandleABullet (ObjectHandler* object);

void HandleACube (ObjectHandler* object);
void SortCubeSpriteDimming (ObjectHandler* object);

void HandleAFlatBase (ObjectHandler* object);

void HandleATetrahedron (ObjectHandler* object);

void HandleATieFighter (ObjectHandler* object);





void UpdateObjectCoordinates (VECTOR* twist,
							VECTOR* position, VECTOR* velocity,
							GsCOORDINATE2* coordSystem, MATRIX* matrix);
void UpdateObjectCoordinates2 (SVECTOR* rotationVector,
							VECTOR* translationVector,
							GsCOORDINATE2* coordSystem);
void UpdateObjectCoordinates3 (SVECTOR* rotation, VECTOR* twist,
							VECTOR* position, VECTOR* velocity,
							GsCOORDINATE2* coordSystem);



void HandleFlatBaseClass (void);




void InitialiseView(void);
void UpdateTheView (void);

void InitialiseLighting(void);





int InitialiseTexture(long addr);
long* InitialiseModel(long* adrs);

void ProperInitialiseTexture (long address, GsIMAGE* imageInfo);


  	   
	  


void HandlePlayersDeath (void);
void HandleEnemysDeath (ObjectHandler* object);

int CheckCollisions (void); 

int ObjectsVeryClose (ObjectHandler* first, ObjectHandler* second);

void ExplodeObject (ObjectHandler* object);

void DummyGuiLoopForTextPrinting (int numberOfStrings, int quitKey);

void ResetAll (void);

void HandleLevelTransition (void);




void HandleSphereClass (void);
void HandleSpheresBehaviour (ObjectHandler* sphere);


int CountNumberOfSpheres (void);
int FindNextFreeSphereID (void);


int CountNumberOfLivingTypedObjects (int type);





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




#if 0
	// this would be unreadable if done within functions
	// NOTE: very slow, but very easy to optimise
#define TURN_NTH_BIT_OFF(argument, sizeInBits, N)						\
	{																	\
	u_long onlyNthBitOn, allButNthBitOn, allOnes = 0;					\
	int i;																\
																		\
	for (i = 0; i < (sizeInBits); i++)									\
		allOnes |= (1 << i);											\
																		\
	onlyNthBitOn = 1 << (N);											\
	allButNthBitOn = allOnes ^ onlyNthBitOn;							\
	(argument) &= allButNthBitOn;										\
	}
#endif

int allOnes;

#define TURN_NTH_BIT_OFF(argument, sizeInBits, N)						\
	{																	\
	u_long onlyNthBitOn, allButNthBitOn;								\
																		\
	onlyNthBitOn = 1 << (N);											\
	allButNthBitOn = allOnes ^ onlyNthBitOn;							\
	(argument) &= allButNthBitOn;										\
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
	u_short zValue;


	InitialiseAll();

   

	side = GsGetActiveBuff();

	while(1)
		{
		if (QuitFrameNumber == frameNumber)
			break;
		//printf("0\n");

		//if (frameNumber % 100 == 0)
		//	CheckForEndOfLevel();			

		FntPrint("frame: %d\n", frameNumber);
		FntPrint("player health: %d\n", PlayersShip.currentHealth);	 
		FntPrint("player score: %d\n", PlayersShip.meritRating);				
	
		FntPrint("ship pos: \n%d %d %d\n", 
			PlayersShip.position.vx, 
			PlayersShip.position.vy,
			PlayersShip.position.vz);
			 
		x = PlayersShip.position.vx / GRID_SIZE;   
		y = PlayersShip.position.vy / GRID_SIZE;
		z = PlayersShip.position.vz / GRID_SIZE;
		//printf("1\n");
		
		frameNumber++;

		DealWithControllerPad();
		//printf("2\n");			

		GsSetWorkBase((PACKET*)packetArea[side]);

		GsClearOt(0,0,&Wot[side]);
		//printf("3\n");

		HandleAllObjects();
		//printf("4\n");
		
		UpdateTheTunnel();
		//printf("4.5\n");

		GsSetRefView2(&view);
		//printf("5\n");	 
		
		numberToDisplay = PerformWorldClipping(ObjectArray, DisplayedObjects);
		FntPrint("num to display: %d\n", numberToDisplay);
		//printf("6\n");

		FntPrint("vx %d, proj %d\n", view.vpz, ProjectionDistance);


			// testing
		//GsSortSprite( &TestSprite1, &Wot[side], 2);
		//GsSortSprite( &TestSprite2, &Wot[side], 2);
	
		for (i = 0; i < numberToDisplay; i++) 
			{		
			object = DisplayedObjects[i];

			if (object->displayFlag == TMD)
				{
				GsGetLs(&(object->coord), &tmpls);
				   
				GsSetLightMatrix(&tmpls);
					
				GsSetLsMatrix(&tmpls);
					
				GsSortObject4( &(object->handler), 
						&Wot[side], 
							3, getScratchAddr(0));
				}
			else
				{
				zValue = SortSpriteObjectPosition( DisplayedObjects[i]);
				GsSortSprite( &DisplayedObjects[i]->sprite, &Wot[side], zValue);
				}	 
			}
		//printf("7\n");

		hsync = VSync(0);		 // USED TO BE VSYNC(2);
								// VSync(2) probably closer match to real frame rate
		ResetGraph(1);
		GsSwapDispBuff();
		GsSortClear(0,0,4,&Wot[side]);
		GsDrawOt(&Wot[side]);
		side ^= 1;
		FntFlush(-1);
		//printf("8\n");
		}

	ResetGraph(3);
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

#if 0
	if (frameNumber % 15 == 0)
		{
		GsSPRITE* sp = &object->sprite;
		printf("real\n");
		dumpVECTOR(&object->position);
		printf("screen\n");
		dumpVECTOR(&screen);
		printf("visual x and y: %d and %d\n", visualX, visualY);
		printf("sprite x and y: %d %d\n", object->sprite.x, object->sprite.y);
		printf("scale: %d %d\n", object->sprite.scalex, object->sprite.scaley);
		dumpSPRITE(sp);
		}
#endif

	return zValue;			   // NOT YET CORRECT
}


	



	// look at objects in first array,
	// if they're close to view point,
	// put them into second array

	#if 0
		// NEEDS BIG REDOING (at present does no effective clipping at all)
	#endif
int PerformWorldClipping (ObjectHandler** firstArray, ObjectHandler** secondArray)
{
	int numberDisplayedObjects = 0;
	int i;
	ObjectHandler *object, **pointer;
	int viewXoffset, viewYoffset;

#if 1		// NEEDS UPDATING: use ViewpointXDistance also
	viewXoffset = ViewpointZDistance * rsin(PlayersShip.rotate.vy) >> 12;
	viewYoffset = ViewpointZDistance * rcos(PlayersShip.rotate.vy) >> 12;
#endif
		
		// find position of viewpoint in world coordinates,
		// clip everything into a square around that						
	ViewPointX = PlayersShip.position.vx + viewXoffset;
	ViewPointZ = PlayersShip.position.vz + viewYoffset;

	SimpleClipDistance = FindClipDistanceFromViewMode();
	
	for (i = 0; i < MAX_OBJECTS; i++)
		{
		if (firstArray[i] != NULL)
			if (firstArray[i]->alive == TRUE)
				{
				#if 0			// OLD
				if (abs(firstArray[i]->position.vx - ViewPointX) < (SimpleClipDistance * GRID_SIZE)
					&& abs(firstArray[i]->position.vz - ViewPointZ) < (SimpleClipDistance * GRID_SIZE) )
						{
						secondArray[numberDisplayedObjects] = firstArray[i];
						numberDisplayedObjects++;
						}
				#endif
				secondArray[numberDisplayedObjects] = firstArray[i];
				numberDisplayedObjects++;
				}
		}
		
	return numberDisplayedObjects;	
}	




int FindClipDistanceFromViewMode (void)
{
	int distance;

	switch(ViewMode)
		{
		case VIEW_1:	distance = 5; 	break;
		default:
			assert(FALSE);				// bad ViewMode value
		}

	return distance;
}

		   


	 

	 	


void DealWithControllerPad (void)
{
	long	pad;
	static int framesSinceLastModeToggle = 0;
	int speedMultiplier;
	int i;
	static int quitCount = 0;
	int quitTime = 30;
  
	pad = PadRead(0);

		// quit program
	if (pad & PADstart && pad & PADselect)
		{
		QuitFrameNumber = frameNumber + 1;
		}	


	if (pad & PADselect)
		speedMultiplier = 10;
	else
		speedMultiplier = 1;
		

		// select & L2: step slowly
	if (pad & PADselect && pad & PADL2)
		{
		int frameCount;	 
		frameCount = VSync(-1);
		while (frameCount + 25 > VSync(-1))
			{
			;
			}
		return;
		}



		// get nice effect if reduce projection, then increase again:
		// zoom off into distance & warped tunnel, then return
#if 1		// mucking about with the viewpoint
	#if 0				 // no more printing
	if ((frameNumber + 30) % 60 == 0)
		{
		printf("projection: %d\n", ProjectionDistance);
		printf("vpx: %d, vpy: %d, vpz: %d\n", view.vpx, view.vpy, view.vpz);
		}
	#endif
	if (pad & PADR2)
		{
		if (pad & PADLleft)
			view.vpz -= 5;
		if (pad & PADLright)
			view.vpz += 5;
		/***
		if (pad & PADLup)
			view.vpy -= 5;
		if (pad & PADLdown)
			view.vpy += 5;
		if (pad & PADRup)
			view.vpx -= 5;
		if (pad & PADRdown)
			view.vpx += 5;
		***/
		if (pad & PADLup)
			{
			ProjectionDistance -= 5;
			GsSetProjection(ProjectionDistance);
			}
		if (pad & PADLdown)
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
				// movement: world-relative (player's ship never tilts)
				// strictly limited in all directions
			if (pad & PADL1)
				PlayersShip.position.vz -= PlayersShip.movementSpeed * speedMultiplier;
			if (pad & PADL2)
				PlayersShip.position.vz += PlayersShip.movementSpeed * speedMultiplier;
			if (pad & PADLup)
				PlayersShip.position.vy -= PlayersShip.movementSpeed * speedMultiplier;
			if (pad & PADLdown)
				PlayersShip.position.vy += PlayersShip.movementSpeed * speedMultiplier;
			if (pad & PADLleft)
				PlayersShip.position.vx -= PlayersShip.movementSpeed * speedMultiplier;
			if (pad & PADLright)
				PlayersShip.position.vx += PlayersShip.movementSpeed * speedMultiplier;

			KeepWithinRange(PlayersShip.position.vx, -30, 45);
			KeepWithinRange(PlayersShip.position.vy, -45, 45);
			KeepWithinRange(PlayersShip.position.vz, 0, SectionLength);

				// firing
			if (pad & PADR1)
				{
				if (PlayersShip.framesSinceLastFire > PlayersShip.firingRate)
					{
					PlayerFiresAShot();
					PlayersShip.framesSinceLastFire = 0;
					}
				}
			PlayersShip.framesSinceLastFire++;

			break;
		default:
			assert(FALSE);
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
		//printf("player firing a shot\n");

		TheBullets[bulletID].alive = TRUE;
		TheBullets[bulletID].lifeTime = 0;
		TheBullets[bulletID].allegiance = PLAYER;			
		
			// send in direction of ship at time of fire
		TheBullets[bulletID].velocity.vx 
				= ((PlayersShip.shotSpeed * PlayersShip.coord.coord.m[2][0]) / 4096);
		TheBullets[bulletID].velocity.vy = 0;
		TheBullets[bulletID].velocity.vz 
				= -((PlayersShip.shotSpeed * PlayersShip.coord.coord.m[2][2]) / 4096);
		   		
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
	   	//printf("enemy firing a shot\n");

		TheBullets[bulletID].alive = TRUE;
		TheBullets[bulletID].lifeTime = 0;
		TheBullets[bulletID].allegiance = ENEMY;			
		
			// send in direction of ship at time of fire
		TheBullets[bulletID].velocity.vx 
				= ((enemy->shotSpeed * enemy->coord.coord.m[2][0]) / 4096);
		TheBullets[bulletID].velocity.vy = 0;
		TheBullets[bulletID].velocity.vz 
				= -((enemy->shotSpeed * enemy->coord.coord.m[2][2]) / 4096);
		   		
		TheBullets[bulletID].movementMomentumFlag = TRUE; 
		
					// emitted from nose of ship
		TheBullets[bulletID].position.vx = enemy->position.vx;
		TheBullets[bulletID].position.vy = enemy->position.vy;
		TheBullets[bulletID].position.vz = enemy->position.vz;
		}
}



 



void InitialiseAll (void)
{	
	int i;


	PadInit(0);


	for (i = 0; i < 32; i++)	
		allOnes |= 1 << i;


		// scale TMD of square to right size
	scalingVector.vx = 7659;
	scalingVector.vy = 7059;
	scalingVector.vz = 7779;

	//printf("iseAll: 1\n");



	GsInitGraph(SCREEN_WIDTH,SCREEN_HEIGHT,GsINTER|GsOFSGPU,1,0);
	if( SCREEN_HEIGHT<480 )
		GsDefDispBuff(0,0,0,SCREEN_HEIGHT);
	else
		GsDefDispBuff(0,0,0,0);

	GsInit3D();		   
	//printf("iseAll: 2\n");

	Wot[0].length=OT_LENGTH;	
	Wot[0].org=wtags[0];	   
	Wot[1].length=OT_LENGTH;
	Wot[1].org=wtags[1];

	GsClearOt(0,0,&Wot[0]);
	GsClearOt(0,0,&Wot[1]);

		
	//printf("iseAll: 3\n");
	


	InitialiseView();
	//printf("iseAll: 4\n");			
	InitialiseLighting();		   


		// get permanent record of each TIM file
	ProperInitialiseTexture(MAIN_TEXTURE_ADDRESS, &MainTextureInfo);
	ProperInitialiseTexture(STARS_TEXTURE_ADDRESS, &StarsTextureInfo);
	ProperInitialiseTexture(CIRCLE_TEXTURE_ADDRESS, &CircleTextureInfo);
	ProperInitialiseTexture(DOTS_TEXTURE_ADDRESS, &DotsTextureInfo);
	ProperInitialiseTexture(FIRE_TEXTURE_ADDRESS, &FireTextureInfo);
	ProperInitialiseTexture(MULTIPLE_TEXTURE_ADDRESS, &MultipleTextureInfo);
	   



	//InitialiseTestSprite1();
	//InitialiseTestSprite2();
   

	//printf("iseAll: 5\n");	

	 
	

	//printf("iseAll: 6\n");

	InitialiseObjects();

	InitialiseTunnel();
		 
	MainMode = MOVE_SHIP;

	FntLoad( 960, 256);
	FntOpen( 0, 0, 256, 200, 0, 512);		  // 64, 32 as first two args before

	//printf("end of InitialiseAll\n");
}






	     

void InitialiseTestSprite1 (void)
{
	InitGsSprite( &TestSprite1);

	TestSprite1.tpage = GetTPage(1, 0, 960, 0);		// 8-bit texture
	TestSprite1.attribute |= 1<<24;			// 8-bit texture
	TestSprite1.x = -50;
	TestSprite1.y = -50;
	TestSprite1.w = 32;
	TestSprite1.h = 32;
	TestSprite1.u = 0;
	TestSprite1.v = 150;
	TestSprite1.cx = 0;
	TestSprite1.cy = 487;
}


void InitialiseTestSprite2 (void)
{
	InitGsSprite( &TestSprite2);

	TestSprite2.tpage = GetTPage(1, 0, 640, 0);		// 8-bit texture
	TestSprite2.attribute |= 1<<24;			// 8-bit texture
	TestSprite2.x = 50;
	TestSprite2.y = -50;
	TestSprite2.w = 128;
	TestSprite2.h = 128;
	TestSprite2.u = 0;
	TestSprite2.v = 64;
	TestSprite2.cx = 0;
	TestSprite2.cy = 485;
}




 


   



void InitialiseObjects (void)
{
	int i;
	int x, z;
	int model;

	 

	InitialiseObjectClass();
	//printf("1\n");





	InitSingleObject(&PlayersShip);

	BringObjectToLife (&PlayersShip, PLAYER, 
					SHIP_MODEL_ADDRESS, 0, NONE);	
	RegisterObjectIntoObjectArray(&PlayersShip);

	PlayersShip.position.vx = PlayersShip.position.vy = PlayersShip.position.vz = 0;
	PlayersShip.rotate.vy = ONE/2;

	PlayersShip.initialHealth = 1000;
	PlayersShip.currentHealth = 1000;
	PlayersShip.allegiance = PLAYER;

	PlayersShip.movementSpeed = 5;
	PlayersShip.rotationSpeed = ONE/64;
	PlayersShip.firingRate = 12;
	PlayersShip.framesSinceLastFire = 0;
	PlayersShip.shotSpeed = 75;
	//PlayersShip.movementMomentumFlag = TRUE;

	
	//printf("2\n");
		

	for (i = 0; i < MAX_BULLETS; i++)
		{
		InitSingleObject(&TheBullets[i]);

		BringObjectToLife(&TheBullets[i], BULLET, 
			CUBE_MODEL_ADDRESS, 0, NONE);

		TheBullets[i].displayFlag = SPRITE;
		TheBullets[i].imageInfo = &FireTextureInfo;

		RegisterObjectIntoObjectArray(&TheBullets[i]);
		}
	//printf("3\n");



	for (i = 0; i < MAX_CUBES; i++)
		{
		InitSingleObject(&TheCubes[i]);

		BringObjectToLife(&TheCubes[i], CUBE, 
			CUBE_MODEL_ADDRESS, 0, NONE);

		TheCubes[i].displayFlag = SPRITE;
		TheCubes[i].imageInfo = &MultipleTextureInfo;
		TheCubes[i].scaleX = 2 * ONE;
		TheCubes[i].scaleY = 2 * ONE;

		RegisterObjectIntoObjectArray(&TheCubes[i]);
		}
	//printf("4\n");



	for (i = 0; i < MAX_FLAT_BASES; i++)
		{
		InitSingleObject(&FlatBases[i]);

		BringObjectToLife(&FlatBases[i], FLAT_BASE, 
			MULTIPLE_MODEL_ADDRESS, 8, NONE);

		FlatBases[i].initialHealth = 10;
		FlatBases[i].currentHealth = 10;
		FlatBases[i].allegiance = ENEMY;

		FlatBases[i].firingRate = 12;
		FlatBases[i].framesSinceLastFire = 0;
		FlatBases[i].shotSpeed = 75;

		RegisterObjectIntoObjectArray(&FlatBases[i]);
		}
	//printf("4.1\n");


	for (i = 0; i < MAX_TETRAHEDRONS; i++)
		{
		InitSingleObject(&Tetrahedrons[i]);

		BringObjectToLife(&Tetrahedrons[i], TETRAHEDRON, 
			MULTIPLE_MODEL_ADDRESS, 0, NONE);

		Tetrahedrons[i].initialHealth = 10;
		Tetrahedrons[i].currentHealth = 10;
		Tetrahedrons[i].allegiance = ENEMY;

		Tetrahedrons[i].movementSpeed = 5;

		RegisterObjectIntoObjectArray(&Tetrahedrons[i]);
		}
	//printf("4.2\n");

	   
	for (i = 0; i < MAX_TIE_FIGHTERS; i++)
		{
		InitSingleObject(&TieFighters[i]);

		BringObjectToLife(&TieFighters[i], TIE_FIGHTER, 
			MULTIPLE_MODEL_ADDRESS, 6, NONE);

		TieFighters[i].initialHealth = 10;
		TieFighters[i].currentHealth = 10;
		TieFighters[i].allegiance = ENEMY;

		TieFighters[i].movementSpeed = 5;
		TieFighters[i].firingRate = 12;
		TieFighters[i].framesSinceLastFire = 0;
		TieFighters[i].shotSpeed = 75;

		RegisterObjectIntoObjectArray(&TieFighters[i]);
		}
	//printf("4.3\n");


		
	for (i = 0; i < MAX_SQUARES; i++)
		{
		InitSingleObject(&TheSquares[i]);

		BringObjectToLife(&TheSquares[i], SQUARE, 
			SQUARE_MODEL_ADDRESS, 0, NONE);

		RegisterObjectIntoObjectArray(&TheSquares[i]);
		}
	//printf("5.1\n");

 
   
			



	LinkAllObjectsToModelsOrSprites();
	//printf("6\n");
	
	LinkAllObjectsToTheirCoordinateSystems();
	//printf("7\n");




	for (i = 0; i < MAX_BULLETS; i++)
		{
		TheBullets[i].alive = FALSE;		// start off dead
		}
	for (i = 0; i < MAX_CUBES; i++)
		{
		TheCubes[i].alive = FALSE;		// start off dead

			// don't want to take all of this TIM, only a small portion
		TheCubes[i].sprite.w /= 4;
		TheCubes[i].sprite.h /= 4;
		TheCubes[i].sprite.u += 32;
		TheCubes[i].sprite.v += 64;

			// start off at random rotation
		TheCubes[i].sprite.rotate = (rand() % 360) * 4096;
		}
	for (i = 0; i < MAX_SQUARES; i++)
		{
		TheSquares[i].alive = FALSE;		// start off dead
		}
	for (i = 0; i < MAX_FLAT_BASES; i++)
		{
		FlatBases[i].alive = FALSE;		// start off dead
		}
	for (i = 0; i < MAX_TETRAHEDRONS; i++)
		{
		Tetrahedrons[i].alive = FALSE;		// start off dead
		}
	for (i = 0; i < MAX_TIE_FIGHTERS; i++)
		{
		TieFighters[i].alive = FALSE;		// start off dead
		}
	
	//printf("8\n");
}



	 

void InitialiseTunnel (void)
{
	SectionLength = SQUARE_SIZE;
	TunnelSize = SQUARE_SIZE;		   
	NumberTunnelSections = 20;
	TunnelSpeed = 8;

		// if remainder is zero, we keep track in sink with player on plane (z=0)
	assert(SectionLength % TunnelSpeed == 0);

	CreateTheTunnel();
}	 





void CreateTheTunnel (void)
{
	int i, z, squareID = 0;

	//printf("11.1\n");

	for (i = 0; i < NumberTunnelSections; i++)
		{
		z = i * SectionLength;

		if (squareID + 3 >= MAX_SQUARES)
			{
			printf("not enough squares left\n");
			return;
			}
		
		TheSquares[squareID].alive = TRUE;
		TheSquares[squareID].position.vx = TunnelSize/2;
		TheSquares[squareID].position.vy = 0;
		TheSquares[squareID].position.vz = z;
		TheSquares[squareID].rotate.vz = ONE/4;
		UpdateObjectCoordinates2 (&TheSquares[squareID].rotate,
							&TheSquares[squareID].position,
							&TheSquares[squareID].coord);
		SetObjectScaling( &TheSquares[squareID], 
					ONE * TunnelSize / SQUARE_SIZE, 
					ONE, ONE * SectionLength / (SQUARE_SIZE));
		SortObjectSize(&TheSquares[squareID]);
			
		squareID++;

		TheSquares[squareID].alive = TRUE;
		TheSquares[squareID].position.vx = -TunnelSize/2;
		TheSquares[squareID].position.vy = 0;
		TheSquares[squareID].position.vz = z;
		TheSquares[squareID].rotate.vz = 3 * ONE/4;
		UpdateObjectCoordinates2 (&TheSquares[squareID].rotate,
							&TheSquares[squareID].position,
							&TheSquares[squareID].coord);
			// scaling: /2 in z to make sections half-size present/absent
		SetObjectScaling( &TheSquares[squareID], 
					ONE * TunnelSize / SQUARE_SIZE, 
					ONE, ONE * SectionLength / (SQUARE_SIZE));
		SortObjectSize(&TheSquares[squareID]);
			
		squareID++;

		TheSquares[squareID].alive = TRUE;
		TheSquares[squareID].position.vx = 0;
		TheSquares[squareID].position.vy = TunnelSize/2;
		TheSquares[squareID].position.vz = z;
		TheSquares[squareID].rotate.vz = ONE/2;
		UpdateObjectCoordinates2 (&TheSquares[squareID].rotate,
							&TheSquares[squareID].position,
							&TheSquares[squareID].coord);
		SetObjectScaling( &TheSquares[squareID], 
					ONE * TunnelSize / SQUARE_SIZE, 
					ONE, ONE * SectionLength / (SQUARE_SIZE));
		SortObjectSize(&TheSquares[squareID]);
			
		squareID++;

		TheSquares[squareID].alive = TRUE;
		TheSquares[squareID].position.vx = 0;
		TheSquares[squareID].position.vy = -TunnelSize/2;
		TheSquares[squareID].position.vz = z;
		UpdateObjectCoordinates2 (&TheSquares[squareID].rotate,
							&TheSquares[squareID].position,
							&TheSquares[squareID].coord);
		SetObjectScaling( &TheSquares[squareID], 
					ONE * TunnelSize / SQUARE_SIZE, 
					ONE, ONE * SectionLength / (SQUARE_SIZE));
		SortObjectSize(&TheSquares[squareID]);
	
		squareID++;
		}

	//printf("11.2\n");
}





   

void UpdateTheTunnel (void)
{
	int i;
	int test, count = 0, aliveCount = 0;
	int tunnelLength = NumberTunnelSections * SectionLength;
	int veryFarBack = -(SectionLength * 2);
	int distance;

		// firstly: move the tunnel towards player
	for (i = 0; i < MAX_SQUARES; i++)
		{
		if (TheSquares[i].alive == TRUE)
			{
			TheSquares[i].coord.coord.t[2] -= TunnelSpeed;

				// if well behind player, re-attach at furthest end of tunnel
			if (TheSquares[i].coord.coord.t[2] <= veryFarBack)
				{
				TheSquares[i].coord.coord.t[2] += tunnelLength;
				}

			TheSquares[i].coord.flg = 0;
			}
		} 

		// now sort subdivision for squares closest to player's ship
	for (i = 0; i < MAX_SQUARES; i += 4)
		{
		if (TheSquares[i].alive == TRUE)
			{
			aliveCount += 4;
			distance = TheSquares[i].coord.coord.t[2] - PlayersShip.coord.coord.t[2];
			if (distance <= 0)
				{
				TheSquares[i].handler.attribute |= GsDIV1;
				TheSquares[i+1].handler.attribute |= GsDIV1;
				TheSquares[i+2].handler.attribute |= GsDIV1;
				TheSquares[i+3].handler.attribute |= GsDIV1;
				count += 4;
				}
			else
				{
				TURN_NTH_BIT_OFF(TheSquares[i].handler.attribute, 32, 9)
				TURN_NTH_BIT_OFF(TheSquares[i+1].handler.attribute, 32, 9)
				TURN_NTH_BIT_OFF(TheSquares[i+2].handler.attribute, 32, 9)
				TURN_NTH_BIT_OFF(TheSquares[i+3].handler.attribute, 32, 9)
				}
			}
		}
			
		// DEBUG 
	if (frameNumber % 30 == 0)
		{
		if (count == aliveCount)
			{
			printf("all living squares are being subdivided\n");
			}
		printf("count: %d\n", count);
		printf("aliveCount: %d\n", aliveCount);
		}
}


 



void HandleAllObjects (void)
{
	ObjectHandler* object;
	int i;
	int x, y, z;

	x = PlayersShip.position.vx;
	y = PlayersShip.position.vy;
	z = PlayersShip.position.vz;

	CheckCollisions();

	HandleFlatBaseClass();

	for (i = 0; i < MAX_OBJECTS; i++)
		{
		if (ObjectArray[i] != NULL)
			{
			if (ObjectArray[i]->alive == TRUE)
				{
				object = ObjectArray[i];
				object->lifeTime++;

				switch(object->type)
					{
					case PLAYER:
						HandleThePlayer(object);

						if (object->currentHealth < 1)
							{
							HandlePlayersDeath();
							goto endOfHandleAllObjects;
							}
						break;
					case BULLET:
						HandleABullet(object);
						break;
					case CUBE:
						HandleACube(object);
						break;
					case SQUARE:
						// don't need to do anything for SQUARE: handled by UpdateTheTunnel
						break;
					case FLAT_BASE:
						HandleAFlatBase(object);
						break;
					case TETRAHEDRON:
						HandleATetrahedron(object);
						break;
					case TIE_FIGHTER:
						HandleATieFighter(object);
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




void HandleThePlayer (ObjectHandler* object)
{
	assert(object->type == PLAYER);

	UpdateObjectCoordinates2 (&object->rotate, 
		&object->position, &object->coord);
							
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
}








void HandleABullet (ObjectHandler* object)
{
	int gridX, gridY, gridZ;

	assert(object->type == BULLET);

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
		TURN_NTH_BIT_OFF(object->sprite.attribute, 32, 31);
		}

	if (gridX < 0 || gridX >= WORLD_MAX_X
		|| gridY < 0 || gridY >= WORLD_MAX_Y
		|| gridZ < 0 || gridZ >= WORLD_MAX_Z)
		{
		object->alive = FALSE;
		object->lifeTime = 0;
		TURN_NTH_BIT_OFF(object->sprite.attribute, 32, 31);
		}
}





void HandleACube (ObjectHandler* object)
{
	int gridX, gridY, gridZ;

	assert(object->type == CUBE);
		
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

		// make it twist a lot (10 degrees per frame)
	object->sprite.rotate += 10 * 4096;

		// kill off after short time
	if (object->lifeTime > 25)
		{
		object->alive = FALSE;
		object->lifeTime = 0;
		TURN_NTH_BIT_OFF(object->sprite.attribute, 32, 31);
		}

		// fade out colours
	if (object->lifeTime % 5 == 0)
		{
		SortCubeSpriteDimming(object);
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
		TURN_NTH_BIT_OFF(object->sprite.attribute, 32, 31);
		}
}



 
	 
 
void SortCubeSpriteDimming (ObjectHandler* object)
{
	// rgb all 128 at start, end up at zero over 25 frames
	// eg reduce each by 5 per frame
	
	object->sprite.r = 128 - (5 * object->lifeTime);
	object->sprite.g = 128 - (5 * object->lifeTime);
	object->sprite.b = 128 - (5 * object->lifeTime);	 
	
		// also reduce slightly (back to ONE at 25 frames)
	object->scaleX = (2 * ONE) - (object->lifeTime * 160);		// 4096 / 25 is roughly 160
	object->scaleY = (2 * ONE) - (object->lifeTime * 160); 		  
}
	
	



void HandleAFlatBase (ObjectHandler* object)
{
	assert(object->type == FLAT_BASE);
}
void HandleATetrahedron (ObjectHandler* object)
{
	assert(object->type == TETRAHEDRON);
}
void HandleATieFighter (ObjectHandler* object)
{
	assert(object->type == TIE_FIGHTER);
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






#define FLAT_BASE_RATE 60

void HandleFlatBaseClass (void)
{
	int i;


#if 0			// NOT YET
	if (frameNumber % FLAT_BASE_RATE == 0)
		{
		id = FindNextFreeFlatBaseId();

		if (id == -1)			 // none available
			return;
		
		FlatBases[id].alive = TRUE;
		FlatBases[id].currentHealth = FlatBases[id].initialHealth;


		}
#endif
}



  





void InitialiseView( void )
{
	ProjectionDistance = 312;			// used to be 192

	GsSetProjection(ProjectionDistance);  

	ViewpointZDistance = -220;
	ViewpointYDistance = 0;
	ViewpointXDistance = 0;

		// squares on plane clipping
	VeryCloseDistance = 2;
	SimpleClipDistance = 5;

	ViewMode = VIEW_1;
	framesSinceLastViewSwitch = 0;
	ViewChangePauseTime = 15;

	view.vrx = 0; view.vry = 0; view.vrz = 0; 

	view.vpx = ViewpointXDistance; 
	view.vpy = ViewpointYDistance; 
	view.vpz = ViewpointZDistance;
	
	view.rz = 0;
	//view.super = &PlayersShip.coord;
	view.super = WORLD;

	GsSetRefView2(&view);
}




void UpdateTheView (void)
{
	switch(ViewMode)
		{
		case VIEW_1:
			ViewpointZDistance = 220;
			ViewpointYDistance = -40;
			ViewpointXDistance = 0;
			ProjectionDistance = 192;
			break;
		default:
			assert(FALSE);
		}

	view.vpx = ViewpointXDistance;
	view.vpy = ViewpointYDistance; 
	view.vpz = ViewpointZDistance;

	GsSetProjection(ProjectionDistance);

	// GsSetRefView is always called every frame 
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
		// crude reward setup
	PlayersShip.meritRating 
		+= (object->initialHealth * (object->strategyFlag + 1));

	ExplodeObject(object);
}





	// collisions: enemy ships, player's ship, cubic bullets
int CheckCollisions (void)
{
	ObjectHandler* object;
	int i, j;

	for (i = 0; i < MAX_BULLETS; i++)
		{
		if (TheBullets[i].alive == TRUE && TheBullets[i].allegiance == ENEMY)
			{
			if (ObjectsVeryClose(&TheBullets[i], &PlayersShip))
				{
				printf("Player and bullet collision\n");
				PlayersShip.currentHealth--;
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
				
				// emitted from centre of exploding object
			TheCubes[cubeID].position.vx = object->position.vx;
			TheCubes[cubeID].position.vy = object->position.vy;
			TheCubes[cubeID].position.vz = object->position.vz;

			whichAxis = i/2;
			if (i%2 == 0)
				sign = -1;
			else
				sign = 1;

			TheCubes[cubeID].movementMomentumFlag = TRUE;

				// m: first row is object's own x axis in world coord terms
				// and so forth..

				// set velocity to 8 (12-9 ie 3, 2 ** 3 = 8)
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
		
		hsync = VSync(2);
		ResetGraph(1);
		GsSwapDispBuff();
		GsSortClear(0,0,4,&Wot[side]);
		GsDrawOt(&Wot[side]);
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
	//PositionSomewhereNotInAWall(&PlayersShip);
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
			
	  	
	   



int CountNumberOfLivingTypedObjects (int type)
{
	int count = 0;
	int i;

	for (i = 0; i < MAX_OBJECTS; i++)
		{
		if (ObjectArray[i] != NULL)
			{
			if (ObjectArray[i]->alive == TRUE
				&& ObjectArray[i]->type == type)
					{
					count++;
					}
			}
		}

	return count;
}


   	

	


