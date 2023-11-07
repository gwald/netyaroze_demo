/************************************************************
 *															*
 *						Tuto17.c							*
 *						3-D object manipulation				*
 *			   											    *															*
 *				LPGE     13/12/96							*		
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

#define CIRCLE_TEXTURE_ADDRESS 0x800b8000
GsIMAGE CircleTextureInfo;

#define DOTS_TEXTURE_ADDRESS 0x800b9000
GsIMAGE DotsTextureInfo;

#define FIRE_TEXTURE_ADDRESS 0x800ba000
GsIMAGE FireTextureInfo;

#define MULTIPLE_TEXTURE_ADDRESS 0x800d0000
GsIMAGE MultipleTextureInfo;




#define SHIP_MODEL_ADDRESS 0x80096000
#define CUBE_MODEL_ADDRESS 0x80097000
#define SQUARE_MODEL_ADDRESS 0x80094000
#define SPHERE_MODEL_ADDRESS 0x800c0000


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
#define VIEW_2 2
#define VIEW_3 3
#define VIEW_4 4
#define VIEW_5 5
#define VIEW_6 6
#define VIEW_7 7

#define MAX_VIEW_MODE (VIEW_7)

u_long framesSinceLastViewSwitch;
u_long ViewChangePauseTime;	

int ViewpointZDistance, ViewpointYDistance;



GsF_LIGHT TheLights[3];	




static PACKET packetArea[2][PACKETMAX2]; /* GPU PACKETS AREA */


 

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










	



GsSPRITE TestSprite1, TestSprite2;

		  




/****************************************************************************
					prototypes
****************************************************************************/


void main (void);

u_short SortSpriteObjectPosition (ObjectHandler* object);




int PerformWorldClipping (ObjectHandler** firstArray, ObjectHandler** secondArray);

int FindClipDistanceFromViewMode (void);

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
void InitialiseTestSprite1 (void);
void InitialiseTestSprite2 (void);
void InitialiseBlipSprites (void);

void InitialiseObjects (void);

	  

void InitialiseShipAccordingToStrategy (ObjectHandler* object);



void HandleAllObjects (void);

void DealWithSquareAttribute (ObjectHandler* object);
int FindSubdivisionDistanceFromViewMode (void);

void SortCubeSpriteDimming (ObjectHandler* object);



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
void UpdateTheView (void);
void InitialiseLighting(void);




int InitialiseTexture(long addr);
long* InitialiseModel(long* adrs);

void ProperInitialiseTexture (long address, GsIMAGE* imageInfo);


  	   
	   
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
	int i;																\
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


	printf("IN MAIN\n");
	InitialiseAll();


   

	side = GsGetActiveBuff();

	while(1)
		{
		if (QuitFrameNumber == frameNumber)
			break;

		//if (frameNumber % 100 == 0)
		//	CheckForEndOfLevel();			

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
		
		//FntPrint("enemy: %d %d %d\n",
		//	TheShips[0].position.vx, TheShips[0].position.vy, TheShips[0].position.vz);				

		frameNumber++;

		DealWithControllerPad();			

		GsSetWorkBase((PACKET*)packetArea[side]);

		GsClearOt(0,0,&Wot[side]);

		UpdateBackground();
		HandleAllObjects();

		GsSetRefView2(&view);	 

		numberToDisplay = PerformWorldClipping(ObjectArray, DisplayedObjects);
		FntPrint("num to display: %d\n", numberToDisplay);

			// background star field
		GsSortSprite( &Background1, &Wot[side], FurthestBack);
		GsSortSprite( &Background2, &Wot[side], FurthestBack);

			// testing
		//GsSortSprite( &TestSprite1, &Wot[side], 2);
		//GsSortSprite( &TestSprite2, &Wot[side], 2);

			// scanner: circle, axes, blips
		GsSortFastSprite( &OvalSprite, &Wot[side], 3);

		SortBlipSprites(&Wot[side]);
	
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

		hsync = VSync(0);		 // USED TO BE VSYNC(2);
								// VSync(2) probably closer match to real frame rate
		ResetGraph(1);
		GsSwapDispBuff();
		GsSortClear(0,0,4,&Wot[side]);
		GsDrawOt(&Wot[side]);
		side ^= 1;
		FntFlush(-1);
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

#if 0
	if (frameNumber % 60 == 0)
		{
		printf("real\n");
		dumpVECTOR(&object->position);
		printf("screen\n");
		dumpVECTOR(&screen);
		printf("visual x and y: %d and %d\n", visualX, visualY);
		printf("sprite x and y: %d %d\n", object->sprite.x, object->sprite.y);
		printf("scale: %d %d\n", object->sprite.scalex, object->sprite.scaley);
		}
#endif

	PopMatrix();

		// 3D clipping
	if (screen.vz < ProjectionDistance/2)
		object->sprite.attribute |= GsDOFF;			 // display off
	else
		TURN_NTH_BIT_OFF(object->handler.attribute, 32, 31)		// ensure 31st bit is OFF

	zValue = (u_short) (screen.vz >> 6);	 // NOTE: need to adjust this

	return zValue;			   // NOT YET CORRECT
}


	



	// look at objects in first array,
	// if they're close to view point,
	// put them into second array
int PerformWorldClipping (ObjectHandler** firstArray, ObjectHandler** secondArray)
{
	int numberDisplayedObjects = 0;
	int i;
	ObjectHandler *object, **pointer;
	int viewXoffset, viewYoffset;

	viewXoffset = ViewpointZDistance * rsin(PlayersShip.rotate.vy) >> 12;
	viewYoffset = ViewpointZDistance * rcos(PlayersShip.rotate.vy) >> 12;
		
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
				if (abs(firstArray[i]->position.vx - ViewPointX) < (SimpleClipDistance * GRID_SIZE)
					&& abs(firstArray[i]->position.vz - ViewPointZ) < (SimpleClipDistance * GRID_SIZE) )
						{
						secondArray[numberDisplayedObjects] = firstArray[i];
						numberDisplayedObjects++;
						}
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
		case VIEW_2:	distance = 4; 	break;
		case VIEW_3:	distance = 4; 	break;
		case VIEW_4:	distance = 4; 	break;
		case VIEW_5:	distance = 4; 	break;
		case VIEW_6:	distance = 10; 	break;
		case VIEW_7:	distance = 15; 	break;
		default:
			assert(FALSE);				// bad ViewMode value
		}

	return distance;
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
					{
					printf("b1  x %d, b2 x %d\n", Background1.x, Background2.x);
					assert(FALSE);
					}
				}
			else					 // right, clockwise
				{
				if (Background1.x == -SCREEN_WIDTH/2)
					Background2.x = Background1.x + SCREEN_WIDTH;
				else if (Background2.x == -SCREEN_WIDTH/2)
					Background1.x = Background2.x + SCREEN_WIDTH;
				else
					{
					printf("b1  x %d, b2 x %d\n", Background1.x, Background2.x);
					assert(FALSE);
					}
				}
			break;
		}

		// move both
	Background1.x -= twist * SCREEN_WIDTH / 1024;
	Background2.x -= twist * SCREEN_WIDTH / 1024;
}







int ScannerRange = 5 * GRID_SIZE;
VECTOR worldZaxis, worldXaxis;	   // world's axes expressed in player's coord system

void SortBlipSprites (GsOT* ot)
{
	int i;
	int wx, wz;		// from player to ship, in world coord system
	int ox, oz;		// from player to ship, in player's coord system
	int xInCircle, zInCircle;
	int cyclePoint;
	
		// cycle: cyclically change r value in red blips -> pulsing
	cyclePoint = frameNumber % 30; 

	worldZaxis.vx = rsin(PlayersShip.rotate.vy);
	worldZaxis.vy = 0;
	worldZaxis.vz = -rcos(PlayersShip.rotate.vy);

	worldXaxis.vx = worldZaxis.vz;
	worldXaxis.vy = 0;
	worldXaxis.vz = -worldZaxis.vx;

		// firstly, detect ships
	for (i = 0; i < MAX_SHIPS; i++)
		{
		if (TheShips[i].alive != TRUE)
			continue;

			// position vector to enemy ship from player, in world coord. system
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

			BlipSprites[i].r = 16 + (5 * cyclePoint);

			GsSortFastSprite( &BlipSprites[i], ot, 1);
			}
		}




#if 0		// no more spheres
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
#endif
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
	int i;
	static int quitCount = 0;
	int quitTime = 30;
  
	pad = PadRead(0);

		// quit program
	if (pad & PADstart && pad & PADselect)
		{
		QuitFrameNumber = frameNumber + 1;
		}	

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

				// view mode toggling
			if (pad & PADR1 && framesSinceLastViewSwitch > ViewChangePauseTime)
				{
				if (ViewMode == MAX_VIEW_MODE)
					ViewMode = VIEW_1;
				else
					ViewMode++;
				framesSinceLastViewSwitch = 0;
				UpdateTheView();
				}
			if (pad & PADR2 && framesSinceLastViewSwitch > ViewChangePauseTime)
				{
				if (ViewMode == VIEW_1)
					ViewMode = MAX_VIEW_MODE;
				else
					ViewMode--;
				framesSinceLastViewSwitch = 0;
				UpdateTheView();
				}
			framesSinceLastViewSwitch++;

			if (pad & PADL1)				// return to normal view
				{
				ViewMode = VIEW_1;
				UpdateTheView();
				framesSinceLastViewSwitch = 0;
				}

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
		printf("firing a shot\n");

		TheBullets[bulletID].alive = TRUE;
		TheBullets[bulletID].lifeTime = 0;
		TheBullets[bulletID].allegiance = PLAYER;			
		
			// send in direction of ship at time of fire
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
	int i;


	printf("iseAll: 0\n");
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


#if 0			   // OLD
	InitialiseTexture(MAIN_TEXTURE_ADDRESS);
	InitialiseTexture(STARS_TEXTURE_ADDRESS);
	printf("iseAll: 4.1\n");
	InitialiseTexture(CIRCLE_TEXTURE_ADDRESS);
	InitialiseTexture(DOTS_TEXTURE_ADDRESS);
	printf("iseAll: 4.2\n");
	InitialiseBackgroundSprite( &Background1);
	InitialiseBackgroundSprite( &Background2);
	InitialiseOvalSprite();
	InitialiseBlipSprites();

	InitialiseTexture(FIRE_TEXTURE_ADDRESS);
	InitialiseTexture(FIRE_2_TEXTURE_ADDRESS);
#endif


	ProperInitialiseTexture(MAIN_TEXTURE_ADDRESS, &MainTextureInfo);
	ProperInitialiseTexture(STARS_TEXTURE_ADDRESS, &StarsTextureInfo);
	ProperInitialiseTexture(CIRCLE_TEXTURE_ADDRESS, &CircleTextureInfo);
	ProperInitialiseTexture(DOTS_TEXTURE_ADDRESS, &DotsTextureInfo);
	ProperInitialiseTexture(FIRE_TEXTURE_ADDRESS, &FireTextureInfo);
	ProperInitialiseTexture(MULTIPLE_TEXTURE_ADDRESS, &MultipleTextureInfo);


		// star fields
	InitialiseBackgroundSprite( &Background1);
	InitialiseBackgroundSprite( &Background2);
		// 'circle' scanner
	InitialiseOvalSprite();
		// 'dots'
	InitialiseBlipSprites();


	InitialiseTestSprite1();
	InitialiseTestSprite2();
   

	//printf("iseAll: 5\n");	

	 
	

	//printf("iseAll: 6\n");

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
	//OvalSprite.attribute |= 1 << 30;		// semi-trans on
	//OvalSprite.attribute |= 1 << 29;		// semi-trans type
	OvalSprite.attribute |= 1<<24;			// 8-bit texture
	OvalSprite.x = -SCREEN_WIDTH/2 + 8;
	OvalSprite.y = -SCREEN_HEIGHT/2 + 19;
	OvalSprite.w = 48;
	OvalSprite.h = 48;
	OvalSprite.cx = 0;
	OvalSprite.cy = 482;
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
	//printf("1\n");





	InitSingleObject(&PlayersShip);

	BringObjectToLife (&PlayersShip, PLAYER, 
					SHIP_MODEL_ADDRESS, 0, NONE);	
	RegisterObjectIntoObjectArray(&PlayersShip);

	PositionSomewhere(&PlayersShip);

	PlayersShip.initialHealth = 1000;
	PlayersShip.currentHealth = 1000;
	PlayersShip.allegiance = PLAYER;

	PlayersShip.movementSpeed = 22;
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
	//printf("5\n");




#if 0				// no more spheres
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
	//printf("5.01\n");
#endif





		// this dependent on world size
	for (i = 0; i < MAX_SQUARES; i++)
		{
		InitSingleObject(&TheSquares[i]);

		BringObjectToLife(&TheSquares[i], SQUARE, 
			SQUARE_MODEL_ADDRESS, 0, NONE); 
	
		x = i / WORLD_MAX_X;			
		z = i % WORLD_MAX_Z;

		TheSquares[i].position.vx = x * GRID_SIZE;
		TheSquares[i].position.vy = 0;
		TheSquares[i].position.vz = z * GRID_SIZE;

		TheSquares[i].rotate.vx = ONE/2;

		UpdateObjectCoordinates2(&TheSquares[i].rotate,  
							&TheSquares[i].position, &TheSquares[i].coord);

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
	for (i = 0; i < MAX_SPHERES; i++)
		{
		TheSpheres[i].alive = FALSE;		// start off dead
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

	//HandleSphereClass();	// no more spheres

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

							// make it twist a lot (10 degrees per frame)
						object->sprite.rotate += 10 * 4096;

							// kill off after short time
						if (object->lifeTime > 25)
							{
							object->alive = FALSE;
							object->lifeTime = 0;
							}

#if 1			// NOT YET WORKING PROPERLY: need to reload after death
							// reduce CLUT over time
						if (object->lifeTime % 5 == 0)
							{
							SortCubeSpriteDimming(object);
							}
#endif

							
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
						DealWithSquareAttribute(object);
						#if 0						
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
						#endif
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





void DealWithSquareAttribute (ObjectHandler* object)
{					
	VeryCloseDistance = FindSubdivisionDistanceFromViewMode();

	if (abs(object->position.vx - ViewPointX) < (VeryCloseDistance * GRID_SIZE)
			&& abs(object->position.vz - ViewPointZ) < (VeryCloseDistance * GRID_SIZE))
		{
		object->handler.attribute |= GsDIV1;
		//printf("attribute set to subdivide, is now %d\n", object->handler.attribute);
		}
	else
		{
		TURN_NTH_BIT_OFF(object->handler.attribute, 32, 31) 
		}
}



int FindSubdivisionDistanceFromViewMode (void)
{
	int distance;

	switch(ViewMode)
		{
		case VIEW_1:	distance = 2; 	break;
		case VIEW_2:	distance = 2; 	break;
		case VIEW_3:	distance = 2; 	break;
		case VIEW_4:	distance = 2; 	break;
		case VIEW_5:	distance = 2; 	break;
		case VIEW_6:	distance = 2; 	break;
		case VIEW_7:	distance = 5; 	break;
		default:
			assert(FALSE);				// bad ViewMode value
		}

	return distance;
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






	
	// this does world-relative rotation and object-relative movement
	// specific to object with a floor
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
	ProjectionDistance = 192;

	GsSetProjection(ProjectionDistance);  

	ViewpointZDistance = 220;
	ViewpointYDistance = -40;

		// squares on plane clipping
	VeryCloseDistance = 2;
	SimpleClipDistance = 5;

	ViewMode = VIEW_1;
	framesSinceLastViewSwitch = 0;
	ViewChangePauseTime = 15;

	view.vrx = 0; view.vry = 0; view.vrz = 0; 
	view.vpx = 0; view.vpy = ViewpointYDistance; view.vpz = ViewpointZDistance;
	view.rz = 0;
	view.super = &PlayersShip.coord;

	GsSetRefView2(&view);
}




void UpdateTheView (void)
{
	switch(ViewMode)
		{
		case VIEW_1:
			ViewpointZDistance = 220;
			ViewpointYDistance = -40;
			ProjectionDistance = 192;
			break;
		case VIEW_2: 
			ViewpointZDistance = 220;
			ViewpointYDistance = 0;
			ProjectionDistance = 192;
			break;
		case VIEW_3: 
			ViewpointZDistance = 220;
			ViewpointYDistance = 20;
			ProjectionDistance = 192;
			break;
		case VIEW_4: 
			ViewpointZDistance = 155;
			ViewpointYDistance = 0;
			ProjectionDistance = 192;
			break;
		case VIEW_5: 
			ViewpointZDistance = 155;
			ViewpointYDistance = -340;
			ProjectionDistance = 192;
			break;
		case VIEW_6: 
			ViewpointZDistance = 155;
			ViewpointYDistance = -1600;
			ProjectionDistance = 192;
			break;
		case VIEW_7: 
			ViewpointZDistance = 80;
			ViewpointYDistance = -25;
			ProjectionDistance = 32;
			break;
		default:
			assert(FALSE);
		}

	view.vpy = ViewpointYDistance; view.vpz = ViewpointZDistance;

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





#define SPHERE_CREATION_RATE 250
#define MAX_NUMBER_SPHERES 3


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
	if ( (CountNumberOfSpheres() < MAX_NUMBER_SPHERES) 
		&& (rand() % SPHERE_CREATION_RATE == 0) )
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


	


