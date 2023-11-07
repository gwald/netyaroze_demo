/************************************************************
 *															*
 *						Demo.c								*
 *						3-D game demo						*
 *			   											    *															*
 *				L Evans     02/02/97						*		
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



#include "storescr.h"

	// location in memory where screen will get saved
#define SCREEN_SAVE_ADDRESS 0x80090000







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
#define MAIN_TEXTURE_ADDRESS	0x80092000	
GsIMAGE MainTextureInfo;
 

#define FIRE_TEXTURE_ADDRESS 0x80093000
GsIMAGE FireTextureInfo;

#define MULTIPLE_TEXTURE_ADDRESS 0x80099000
GsIMAGE MultipleTextureInfo;

#define MASCOTS_TEXTURE_ADDRESS 0x80120000
GsIMAGE MascotsTextureInfo;

#define ASCII_TEXTURE_ADDRESS 0x80130000
GsIMAGE AsciiTextureInfo;




#define SHIP_MODEL_ADDRESS 0x80091000
#define SQUARE_MODEL_ADDRESS 0x80090000
#define MULTIPLE_MODEL_ADDRESS 0x80094000



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

GsFOGPARAM Fogging;




static PACKET packetArea[2][PACKETMAX2]; /* GPU PACKETS AREA */


 




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



#define QUIT_PROGRAM 0
#define RESET_GAME 1
#define RESET_AFTER_LOSE_LIFE 2
#define MAKE_PLAYER_INTERACTIVE_AGAIN 3
#define TURN_PLAYER_DISPLAY_OFF 4
#define TURN_PLAYER_DISPLAY_ON 5
#define REVERT_PLAYER_FIRING_RATE 6
#define REVERT_PLAYER_SHOT_SPEED 7
#define REVERT_PLAYER_MOVEMENT_SPEED 8
#define STOP_LIGHT_OSCILLATION 9
#define STOP_VIEW_OSCILLATION 10
#define KILL_ALL_SPECIAL_OBJECTS 11

	// status flags for delayed actions
#define ACTIVELY_WAITING 0
#define INERT 1

typedef struct
{
	int status;	
	int frameToStart;
	int typeFlag;
} DelayedAction;	

#define MAX_DELAYED_ACTIONS 20
DelayedAction ActionList[MAX_DELAYED_ACTIONS];

	  



	// object types
#define PLAYER 0
#define BULLET 1
#define ENEMY 2
#define CUBE 3
#define SQUARE 4
#define TETRAHEDRON 5
#define TIE_FIGHTER 6
#define SINGLE_ATTACKER 7
#define INSECT_SHIP 8
#define SPECIAL_OBJECT 9
#define ROBOT 10





#define INITIAL_NUMBER_OF_LIVES 5
int PlayersLives;

int LevelNumber;

int PlayerInteractive;

ObjectHandler PlayersShip;


#define MAX_BULLETS 50
ObjectHandler TheBullets[MAX_BULLETS];
	

#define MAX_CUBES 20
ObjectHandler TheCubes[MAX_CUBES];


#define MAX_SQUARES 400			  // MUST be divisible by 4
ObjectHandler TheSquares[MAX_SQUARES];


#define MAX_TETRAHEDRONS 64
ObjectHandler Tetrahedrons[MAX_TETRAHEDRONS];


#define MAX_TIE_FIGHTERS 24
ObjectHandler TieFighters[MAX_TIE_FIGHTERS];


#define MAX_SINGLE_ATTACKERS 24
ObjectHandler SingleAttackers[MAX_SINGLE_ATTACKERS];


#define MAX_INSECT_SHIPS 36
ObjectHandler InsectShips[MAX_INSECT_SHIPS];


#define MAX_SPECIAL_OBJECTS 6
ObjectHandler SpecialObjects[MAX_SPECIAL_OBJECTS];


ObjectHandler TheRobot;






	// array of pointers for world clipping
ObjectHandler* DisplayedObjects[MAX_OBJECTS];



   

	 	 
	// for sprite-displayed objects: show as 2D rather than 3D

#define HOLD_STILL 57



int EnemyWaveFrequency;

#define TIE_FIGHTER_CLASS 0
#define SINGLE_ATTACKER_CLASS 1
#define INSECT_SHIP_CLASS 2
#define ROBOT_CLASS 3

	// not including robot
#define NUMBER_ENEMY_CLASSES 3


 



	// ways to go round a circle
#define CLOCKWISE 0
#define ANTICLOCKWISE 1
	


	// basic text handling
#define MAX_TEXT_STRINGS 20
#define MAX_STRING_LENGTH 50

char BasicTextStrings[MAX_TEXT_STRINGS][MAX_STRING_LENGTH];	

	

	// graphical text handling (textures, not FntPrint)
	// (not yet bothered with scaling and colouring, but it's easy to add)
GsSPRITE LetterSprites[26];
GsSPRITE NumberSprites[10];


typedef struct
{
	int x, y;
	int scaleX, scaleY;
	int length;
	char data[MAX_STRING_LENGTH];
} TextString;

TextString AllStrings[MAX_TEXT_STRINGS];
int NumberOfStrings;





	// game level
int LevelNumber = 1;






			 
	 

VECTOR scalingVector;		// for the squares -> smoother walls

		// z value
u_long FurthestBack = (1<<OT_LENGTH)-1;


	



GsSPRITE TestSprite1, TestSprite2;





int SectionLength;
int TunnelWidth, TunnelHeight;
int NumberTunnelSections;
int TunnelSpeed;

#define INITIAL_TUNNEL_SPEED 8

int LeastX, MostX, LeastY, MostY, LeastZ, MostZ;

	// intrinsic size of square (ten.tmd, SQUARE_MODEL_ADDRESS)
#define SQUARE_SIZE 128

		  
	// limits on player movement in xy plane
#define PLAYER_MIN_X -65
#define PLAYER_MAX_X 65
#define PLAYER_MIN_Y -50
#define PLAYER_MAX_Y 50
#define PLAYER_MIN_Z 0
#define PLAYER_MAX_Z SectionLength



	// each one is ten units in from tunnel sides
#define TIE_FIGHTER_MIN_X -86
#define TIE_FIGHTER_MAX_X 86
#define TIE_FIGHTER_MIN_Y -54
#define TIE_FIGHTER_MAX_Y 54




	// for tie-fighters
int NumberTieFightersPerWave;

	// wave types
#define STATIC_HELIX 0
#define SWITCHING_HELIX 1
#define DARTING_STATIC_HELIX 2
#define DARTING_SWITCHING_HELIX 3

#define NUMBER_WAVE_TYPES 4


	// for tie-fighter waves
#define SWITCH_PERIOD 45 


	// group darting movements
int DartOn = FALSE;
int DartDirection = -1;
int DartFrameStart = -1;
int DartDuration = -1;



#define ANGULAR_SPEED (ONE/128)
#define RADIUS 48


#define Z_DISTANCE (SQUARE_SIZE * 4)
#define Z_SEPARATION (SQUARE_SIZE)

	// precompiling positions on circle;
	// unsure how much speed difference this makes
int CircularXs[256], CircularYs[256];
int NumberPositions;



	// single attackers
#define RANDOM_JIGGLER 0
#define X_Y_EVADER 1
#define STATIC_CIRCLER 2
#define SWITCHING_CIRCLER 3

#define NUMBER_ATTACKER_SUBTYPES 4



	// insect ships
int NumberInsectShipsPerWave;

#define INSECT_INITIAL_Z (SQUARE_SIZE * 4)
#define INSECT_Z_SEPARATION (SQUARE_SIZE / 2)




	// these globals enable easy speed-ups later in the game as player progresses
int TieFighterMovementSpeed;
int TieFighterAngularSpeed;


int SingleAttackerMovementSpeed;
int SingleAttackerAngularSpeed;
int SingleAttackerFiringRate;
int SingleAttackerShotSpeed;


int InsectShipMovementSpeed;


int RobotCirclingSpeed;
int RobotMovementSpeed;
int RobotRotationSpeed;
int RobotPeriod;
int RobotZPeriod;
int RobotFiringPeriod;

int RobotFrequency;



#define ROBOT_MIN_X -50
#define ROBOT_MAX_X 50
#define ROBOT_MIN_Y -35
#define ROBOT_MAX_Y 35







	// sound business

#define SOUND_ON 1		// compile-time overall switch

#define VH0_ADDRESS 0x800a0000
#define VB0_ADDRESS 0x800b0000


short vab0;


	// value of status flag for each sound channel
#define OFF 0
#define WAITING_TO_START 1
#define PLAYING 2

typedef struct
{	
	int value;
	int statusFlag;
	int frameToStartOn;
	int duration;
	int program, tone, note;
	int leftVolume, rightVolume;
} VoiceChannelHandler;

#define MAX_VOICES 24		// psx standard
VoiceChannelHandler TheVoices[MAX_VOICES];


	


	

	

	// special objects: bonusses and distorters

int SpecialObjectLikelihood;

	// sub types
#define EXTRA_LIFE 0
#define FIRE_QUICKER 1
#define SHOTS_MOVE_FASTER 2
#define SHIP_MOVES_FASTER 3
#define OSCILLATE_LIGHTS 4
#define OSCILLATE_VIEW 5

#define NUMBER_SPECIAL_OBJECT_SUBTYPES 6

	// how long the effect of (temporarily acting) special objects lasts
#define EFFECT_DURATION 480



#define SIZE_OF_SPECIAL_OBJECT_TYPE_TABLE 12
int SpecialObjectTypeTable[SIZE_OF_SPECIAL_OBJECT_TYPE_TABLE]
	= { EXTRA_LIFE,
		FIRE_QUICKER,
		SHOTS_MOVE_FASTER,
		SHIP_MOVES_FASTER,
		OSCILLATE_LIGHTS,
		OSCILLATE_LIGHTS,
		OSCILLATE_LIGHTS,
		OSCILLATE_LIGHTS,
		OSCILLATE_VIEW,
		OSCILLATE_VIEW,
		OSCILLATE_VIEW,
		OSCILLATE_VIEW
	  };
		


	// view oscillations
#define LEFT_TO_RIGHT 0
#define UP_AND_DOWN	1
#define CIRCLING_CLOCKWISE 2
#define CIRCLING_ANTICLOCKWISE 3

int LightOscillationFlag = FALSE, ViewOscillationFlag = FALSE;


int ViewDistortionMode, OldViewDistortionMode = LEFT_TO_RIGHT;
int ViewDistortPeriod = 120;
int ViewHorizontalVelocity, ViewVerticalVelocity, ViewBaseVelocity = 2;
int ViewAngle, ViewAngularSpeed = ONE/128;
int ViewOscillationJustBegun = FALSE;




	// reading files from CD
	// sound stuff ....	   
#define NUMBER_FILES 10


typedef struct {
	char	fname[64];
	void	*addr;
	CdlFILE	finfo;
} FILE_INFO;

	

#if 0		// previous example code
	// OTHER CHOICES FOR MUSIC:
	// gogo.seq, musi.seq
static FILE_INFO dfile[DFILE] = {
	{"\\DATA\\SOUND\\STD0.VH;1", (void *)VH_ADDR, 0},
	{"\\DATA\\SOUND\\STD0.VB;1", (void *)VB_ADDR, 0},
	{"\\DATA\\SOUND\\GOGO.SEQ;1", (void *)SEQ_ADDR, 0},
};
#endif


static FILE_INFO DataFiles[NUMBER_FILES] = {
	{"WAVE09.TIM;1", (void *)MAIN_TEXTURE_ADDRESS, 0 },
	{"FIRE7.TIM;1", (void *)FIRE_TEXTURE_ADDRESS, 0 },
	{"MULTI3.TIM;1", (void *)MULTIPLE_TEXTURE_ADDRESS, 0 },
	{"MASCOTS2.TIM;1", (void *)MASCOTS_TEXTURE_ADDRESS, 0 },
	{"ASCII2.TIM;1", (void *)ASCII_TEXTURE_ADDRESS, 0 },
	{"THESHIP.TMD;1", (void *)SHIP_MODEL_ADDRESS, 0 },
	{"TEN.TMD;1", (void *)SQUARE_MODEL_ADDRESS, 0 },
	{"ENEMY.TMD;1", (void *)MULTIPLE_MODEL_ADDRESS, 0 },
	{"LEWIS.VH;1", (void *)VH0_ADDRESS, 0 },
	{"LEWIS.VB;1", (void *)VB0_ADDRESS, 0 },
};


#if 0			// memory locations
#define MAIN_TEXTURE_ADDRESS 0x80092000	
#define FIRE_TEXTURE_ADDRESS 0x80093000
#define MULTIPLE_TEXTURE_ADDRESS 0x80099000
#define MASCOTS_TEXTURE_ADDRESS 0x80120000
#define ASCII_TEXTURE_ADDRESS 0x80130000
#define SHIP_MODEL_ADDRESS 0x80091000
#define SQUARE_MODEL_ADDRESS 0x80090000
#define MULTIPLE_MODEL_ADDRESS 0x80094000
#define VH0_ADDRESS 0x800a0000
#define VB0_ADDRESS 0x800b0000
#endif









/****************************************************************************
					prototypes
****************************************************************************/


void main (void);

void CheckForEndOfLevel (void);

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

void InitialiseEnemyMovementGlobals (void);

void InitialiseTunnel (void);
void CreateTheTunnel (void);

void UpdateTheTunnel (void);

	  	  


void HandleAllObjects (void);


void HandleThePlayer (ObjectHandler* object);

void HandleABullet (ObjectHandler* object);

void HandleACube (ObjectHandler* object);
void SortCubeSpriteDimming (ObjectHandler* object);

void HandleATetrahedron (ObjectHandler* object);

void HandleATieFighter (ObjectHandler* object);

void HandleSingleAttacker (ObjectHandler* object);

void HandleAnInsectShip (ObjectHandler* object);

void HandleSpecialObject (ObjectHandler* object);

void HandleRobot (ObjectHandler* object);





void HandleTieFighterWaves (void);

							   



void UpdateObjectCoordinates (VECTOR* twist,
							VECTOR* position, VECTOR* velocity,
							GsCOORDINATE2* coordSystem, MATRIX* matrix);
void UpdateObjectCoordinates2 (SVECTOR* rotationVector,
							VECTOR* translationVector,
							GsCOORDINATE2* coordSystem);
void UpdateObjectCoordinates3 (SVECTOR* rotation, VECTOR* twist,
							VECTOR* position, VECTOR* velocity,
							GsCOORDINATE2* coordSystem);
void UpdateObjectCoordinates4 (ObjectHandler* object);




void HandleEnemyClass (void);
int NumberOfEnemiesLeft (void);


void HandleSpecialObjectClass (void);



void CreateSomeTieFighters (void);
void CreateWaveOfTieFighters (void);
void CreateSingleAttacker (void);
void CreateWaveOfInsectShips (void);
void CreateRobot (void);


int FindNextFreeTetrahedronId (void);
int FindNextFreeTieFighterId (void);
int FindNextFreeSingleAttackerId (void);
int FindNextFreeInsectShipId (void);
int FindNextFreeSpecialObjectId (void);


void PositionObjectOnInsideOfTunnel (ObjectHandler* object, int z);

void HandleObjectContinuousFiring (ObjectHandler* object, VECTOR* direction);

void ObjectFiresAShot (ObjectHandler* object, VECTOR* direction);




void InitialiseView(void);

void InitialiseLighting(void);
void SortTheFlatLights (void);

void OscillateLighting (void);
void MinorOscillateLighting (void);
void OscillateView (void);

					


int InitialiseTexture(long addr);
long* InitialiseModel(long* adrs);

void ProperInitialiseTexture (long address, GsIMAGE* imageInfo);

void LinkSpriteToImageInfo (GsSPRITE* sprite, GsIMAGE* imageInfo);


  	   
	  


void HandlePlayersDeath (void);
void HandleEnemysDeath (ObjectHandler* object);

void HandlePlayerEndOfGame (void);

void ResetEntireProgram (void);

void RestartAfterPlayerLosesALife (void);


void HandleLevelUpdating (void);

void CheckCollisions (void); 

int ObjectsVeryClose (ObjectHandler* first, ObjectHandler* second);

void ExplodeObject (ObjectHandler* object);

void DummyGuiLoopForTextPrinting (int numberOfStrings, int quitKey);


 


int CountNumberOfLivingTypedObjects (int type);



void NormaliseVector (VECTOR* vector);


void PrecompileCircularPositions (void);




void FlyingSequence (void);

int PerformBogusWorldClipping (ObjectHandler** firstArray, ObjectHandler** secondArray);

			 


void InitialiseSound (void);

void HandleSound (void);

void TurnOnSound (int whichProgram, int whichTone, int whichNote, int delayFromNow, int howLong,
									int leftVolume, int rightVolume);

void CleanUpSound (void);




int FindPlayersRewardForKillingObject (ObjectHandler* object);



void InitialiseDelayedActionList (void);

void HandleDelayedActionList (void);

void PerformDelayedAction (DelayedAction* action);

void RegisterDelayedAction (int type, int delayFromNow);


void DealWithSpecialObjectAndPlayer (ObjectHandler* special);

 
void RevertPlayerFiringRate (void);
void RevertPlayerShotSpeed (void);
void RevertPlayerMovementSpeed (void);

void StopLightOscillation (void);
void StopViewOscillation (void);


void KillAllSpecialObjects (void);


void InitialiseTextStrings (void);

void DisplayTextStrings (GsOT* orderingTable);

GsSPRITE* GetSpriteForCharacter (char character);

void RegisterTextStringForDisplay (char* string, int x, int y);



void DatafileSearch (void);
   
void DatafileRead (void);



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




#define setVECTOR(vector, x, y, z)				\
				(vector)->vx = (x), (vector)->vy = (y), (vector)->vz = (z)




/****************************************************************************
					 functions
****************************************************************************/






char temp1[64];
char temp2[64];
char temp3[64];
char temp4[64];
char temp5[64];
char temp6[64];
	
void main (void)
{
	int	i;
	int	hsync = 0;
	GsDOBJ2 *op;
	int side;			  // buffer index
	MATRIX	tmpls, tmplw;
	ObjectHandler** pointer;
	ObjectHandler* object;
	int numberToDisplay;
	u_short singleZ;
	u_short zValue;


	InitialiseAll();


	FlyingSequence();

   

	side = GsGetActiveBuff();

	

	while(1)
		{
		HandleDelayedActionList();
		  
		CheckForEndOfLevel();			

		/******
		FntPrint("frame: %d\n", frameNumber);
		FntPrint("player health: %d\n", PlayersShip.currentHealth);
		FntPrint("player lives: %d\n", PlayersLives);	 
		FntPrint("player score: %d\n", PlayersShip.meritRating);
		FntPrint("level: %d\n", LevelNumber);
		******/
		sprintf(temp1, "frame %d\n", frameNumber); 
		RegisterTextStringForDisplay(temp1, 50, 50);
		sprintf(temp2, "health %d\n", PlayersShip.currentHealth); 
		RegisterTextStringForDisplay(temp2, 50, 60);
		sprintf(temp3, "lives %d\n", PlayersLives); 
		RegisterTextStringForDisplay(temp3, 50, 70);
		sprintf(temp4, "score %d\n", PlayersShip.meritRating); 
		RegisterTextStringForDisplay(temp4, 50, 80);
		sprintf(temp5, "level %d\n", LevelNumber); 
		RegisterTextStringForDisplay(temp5, 50, 90);

		
		frameNumber++;

		DealWithControllerPad();
		
		if (LightOscillationFlag == TRUE)
			OscillateLighting();
			
		if (ViewOscillationFlag == TRUE)
			OscillateView();
			
			
		MinorOscillateLighting();		
		
#if SOUND_ON
		HandleSound();	
#endif

		GsSetWorkBase((PACKET*)packetArea[side]);

		GsClearOt(0,0,&Wot[side]);

		HandleAllObjects();
		
		UpdateTheTunnel();

		GsSetRefView2(&view); 
		
		numberToDisplay = PerformWorldClipping(ObjectArray, DisplayedObjects);
		//FntPrint("num to display: %d\n", numberToDisplay);
	
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

		DisplayTextStrings (&Wot[side]);

		hsync = VSync(0);		 

		ResetGraph(1);
		GsSwapDispBuff();
		GsSortClear(0,0,4,&Wot[side]);
		GsDrawOt(&Wot[side]);
		side ^= 1;
		FntFlush(-1);
		}




		// program cleanup

	ResetGraph(3);

#if SOUND_ON
	CleanUpSound();
#endif

		// capture the screen as TIM in memory, ready for download to PC
	StoreScreen ( (u_long*)SCREEN_SAVE_ADDRESS, 0, 0, 320, 240);
}


  
 



	// extremely crude notion of 'level': just clock time

#define LEVEL_TIME 500
		// used to be 1000

void CheckForEndOfLevel (void)
{
	int actualLevel = (frameNumber / LEVEL_TIME) + 1;

	if (LevelNumber < actualLevel)
		{
		LevelNumber++;
		assert(LevelNumber == actualLevel);
		HandleLevelUpdating();
		}
	else
		{
		assert(LevelNumber == actualLevel);
		}
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


	



	// look at objects in first array,
	// if they're close to view point,
	// put them into second array

int PerformWorldClipping (ObjectHandler** firstArray, ObjectHandler** secondArray)
{
	int numberDisplayedObjects = 0;
	int i;
	ObjectHandler *object, **pointer;

	SimpleClipDistance = SQUARE_SIZE * 8;
	
	for (i = 0; i < MAX_OBJECTS; i++)
		{
		if (firstArray[i] != NULL)
			if (firstArray[i]->alive == TRUE)
				{
				if (firstArray[i]->coord.coord.t[2] < SimpleClipDistance)
						{
						secondArray[numberDisplayedObjects] = firstArray[i];
						numberDisplayedObjects++;
						}
				}
		}
		
	return numberDisplayedObjects;	
}			   


	 

	 	


void DealWithControllerPad (void)
{
	long pad;
	int speedMultiplier;
	int i;
	VECTOR firingDirection;
  
	pad = PadRead(0);

		// quit program
	if (pad & PADstart && pad & PADselect)
		{
		RegisterDelayedAction (QUIT_PROGRAM, 1);
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

 		// actions depend on mode
	switch(MainMode)
		{
		case MOVE_SHIP:
			if (PlayerInteractive == TRUE)
				{
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

				KeepWithinRange(PlayersShip.position.vx, PLAYER_MIN_X, PLAYER_MAX_X);
				KeepWithinRange(PlayersShip.position.vy, PLAYER_MIN_Y, PLAYER_MAX_Y);
				KeepWithinRange(PlayersShip.position.vz, PLAYER_MIN_Z, PLAYER_MAX_Z);
					
				if (pad & PADR1)
					{
					firingDirection.vx = PlayersShip.coord.coord.m[2][0];
					firingDirection.vy = PlayersShip.coord.coord.m[2][1];
						// - because of direction ship TMD points in
					firingDirection.vz = -PlayersShip.coord.coord.m[2][2];

					HandleObjectContinuousFiring( &PlayersShip, &firingDirection);
					}
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

 



void InitialiseAll (void)
{	
	int i;
	char time[64];	 // format: aa:bb:cc, hour:min:sec
	int arbitraryValue;


		// unsure about position in main ......
	DatafileSearch();
	DatafileRead();


	PadInit(0);


		// number used for TURN_NTH_BIT_OFF macro
	for (i = 0; i < 32; i++)	
		allOnes |= 1 << i;


		// scale TMD of square to right size
	scalingVector.vx = 7659;
	scalingVector.vy = 7059;
	scalingVector.vz = 7779;



	PrecompileCircularPositions();

	InitialiseDelayedActionList();

	



#if SOUND_ON
	InitialiseSound();	   
#endif

		// attempt to make rand() more random:
		// seed using the seconds (NOTE: compile-time only: NOT the real time...)
	strcpy(time, __TIME__);
	arbitraryValue = ((int) time[6]) + ((int) time[7]);
	srand(arbitraryValue);




	GsInitGraph(SCREEN_WIDTH,SCREEN_HEIGHT,GsINTER|GsOFSGPU,1,0);
	if( SCREEN_HEIGHT<480 )
		GsDefDispBuff(0,0,0,SCREEN_HEIGHT);
	else
		GsDefDispBuff(0,0,0,0);

	GsInit3D();		   

	Wot[0].length=OT_LENGTH;	
	Wot[0].org=wtags[0];	   
	Wot[1].length=OT_LENGTH;
	Wot[1].org=wtags[1];

	GsClearOt(0,0,&Wot[0]);
	GsClearOt(0,0,&Wot[1]);	


	InitialiseView();		
	InitialiseLighting();		   


		// get permanent record of each TIM file
	ProperInitialiseTexture(MAIN_TEXTURE_ADDRESS, &MainTextureInfo);
	ProperInitialiseTexture(FIRE_TEXTURE_ADDRESS, &FireTextureInfo);
	ProperInitialiseTexture(MULTIPLE_TEXTURE_ADDRESS, &MultipleTextureInfo);
	ProperInitialiseTexture(MASCOTS_TEXTURE_ADDRESS, &MascotsTextureInfo);
	ProperInitialiseTexture(ASCII_TEXTURE_ADDRESS, &AsciiTextureInfo);


	InitialiseTextStrings();
	 
	

	InitialiseObjects();

	InitialiseTunnel();
		 
	MainMode = MOVE_SHIP;

	FntLoad( 960, 256);
	FntOpen( 0, 40, 256, 200, 0, 512);		  // 64, 32 as first two args before
}



	     



void InitialiseObjects (void)
{
	int i;
	int x, z;
	int model;

	 

	InitialiseObjectClass();


	InitialiseEnemyMovementGlobals();


	PlayersLives = INITIAL_NUMBER_OF_LIVES;
	LevelNumber = 1;
	PlayerInteractive = TRUE;
	InitSingleObject(&PlayersShip);

	BringObjectToLife (&PlayersShip, PLAYER, 
					SHIP_MODEL_ADDRESS, 0, NONE);	
	RegisterObjectIntoObjectArray(&PlayersShip);

	PlayersShip.position.vx = PlayersShip.position.vy = PlayersShip.position.vz = 0;
	PlayersShip.rotate.vy = ONE/2;
	UpdateObjectCoordinates2 (&PlayersShip.rotate, 
		&PlayersShip.position, &PlayersShip.coord);

	PlayersShip.initialHealth = 12;
	PlayersShip.currentHealth = 12;
	PlayersShip.allegiance = PLAYER;

	PlayersShip.movementSpeed = 5;
	PlayersShip.rotationSpeed = ONE/64;

	PlayersShip.canFire = TRUE;
	PlayersShip.firingRate = 12;
	PlayersShip.framesSinceLastFire = 0;
	PlayersShip.shotSpeed = 75;
	PlayersShip.shotType = FIRE_SPRITE;


	SetObjectScaling( &PlayersShip, ONE/2, ONE/2, ONE/2);
	SortObjectSize(&PlayersShip);

	
		

	for (i = 0; i < MAX_BULLETS; i++)
		{
		InitSingleObject(&TheBullets[i]);

		BringObjectToLife(&TheBullets[i], BULLET, 
			SQUARE_MODEL_ADDRESS, 0, NONE);

		TheBullets[i].displayFlag = SPRITE;
		TheBullets[i].imageInfo = &FireTextureInfo;

		RegisterObjectIntoObjectArray(&TheBullets[i]);
		}



	for (i = 0; i < MAX_CUBES; i++)
		{
		InitSingleObject(&TheCubes[i]);

		BringObjectToLife(&TheCubes[i], CUBE, 
			SQUARE_MODEL_ADDRESS, 0, NONE);

		TheCubes[i].displayFlag = SPRITE;
		TheCubes[i].imageInfo = &MultipleTextureInfo;

		RegisterObjectIntoObjectArray(&TheCubes[i]);
		}





	for (i = 0; i < MAX_TETRAHEDRONS; i++)
		{
		InitSingleObject(&Tetrahedrons[i]);

		BringObjectToLife(&Tetrahedrons[i], TETRAHEDRON, 
			MULTIPLE_MODEL_ADDRESS, 0, NONE);

		Tetrahedrons[i].initialHealth = 1;
		Tetrahedrons[i].currentHealth = 1;
		Tetrahedrons[i].allegiance = ENEMY;

		Tetrahedrons[i].movementSpeed = 5;

		SetObjectScaling( &Tetrahedrons[i], ONE/4, ONE/4, ONE/4);
        SortObjectSize(&Tetrahedrons[i]);

		RegisterObjectIntoObjectArray(&Tetrahedrons[i]);
		}



  	   
	for (i = 0; i < MAX_TIE_FIGHTERS; i++)
		{
		InitSingleObject(&TieFighters[i]);

		BringObjectToLife(&TieFighters[i], TIE_FIGHTER, 
			MULTIPLE_MODEL_ADDRESS, 6, NONE);

		TieFighters[i].initialHealth = 1;
		TieFighters[i].currentHealth = 1;
		TieFighters[i].allegiance = ENEMY;

		TieFighters[i].movementSpeed = TieFighterMovementSpeed;

		SetObjectScaling( &TieFighters[i], ONE/2, ONE/2, ONE/2);
		SortObjectSize( &TieFighters[i]);

		RegisterObjectIntoObjectArray(&TieFighters[i]);
		}



	for (i = 0; i < MAX_INSECT_SHIPS; i++)
		{
		InitSingleObject(&InsectShips[i]);

		BringObjectToLife(&InsectShips[i], INSECT_SHIP, 
			MULTIPLE_MODEL_ADDRESS, 3, NONE);

		InsectShips[i].initialHealth = 1;
		InsectShips[i].currentHealth = 1;
		InsectShips[i].allegiance = ENEMY;

		InsectShips[i].movementSpeed = InsectShipMovementSpeed;

		SetObjectScaling( &InsectShips[i], ONE/4, ONE/4, ONE/4);
		SortObjectSize( &InsectShips[i]);

		RegisterObjectIntoObjectArray(&InsectShips[i]);
		}


	for (i = 0; i < MAX_SINGLE_ATTACKERS; i++)
		{
		InitSingleObject(&SingleAttackers[i]);

		BringObjectToLife(&SingleAttackers[i], SINGLE_ATTACKER, 
			MULTIPLE_MODEL_ADDRESS, 6, NONE);

		SingleAttackers[i].initialHealth = 4;
		SingleAttackers[i].currentHealth = 4;
		SingleAttackers[i].allegiance = ENEMY;

		SingleAttackers[i].movementSpeed = SingleAttackerMovementSpeed;

		SingleAttackers[i].canFire = TRUE;
		SingleAttackers[i].firingRate = SingleAttackerFiringRate;
		SingleAttackers[i].framesSinceLastFire = 0;
		SingleAttackers[i].shotSpeed = SingleAttackerShotSpeed;
		SingleAttackers[i].shotType = FIRE_SPRITE;	//TETRAHEDRON_MODEL;

		SetObjectScaling( &SingleAttackers[i], ONE/2, ONE/2, ONE/2);
		SortObjectSize( &SingleAttackers[i]);

		RegisterObjectIntoObjectArray(&SingleAttackers[i]);
		}



		
	for (i = 0; i < MAX_SQUARES; i++)
		{
		InitSingleObject(&TheSquares[i]);

		BringObjectToLife(&TheSquares[i], SQUARE, 
			SQUARE_MODEL_ADDRESS, 0, NONE);

		RegisterObjectIntoObjectArray(&TheSquares[i]);
		}

 
   
	for (i = 0; i < MAX_SPECIAL_OBJECTS; i++)
		{
		InitSingleObject(&SpecialObjects[i]);

		BringObjectToLife(&SpecialObjects[i], SPECIAL_OBJECT, 
			SQUARE_MODEL_ADDRESS, 0, NONE);	   

		SpecialObjects[i].displayFlag = SPRITE;
		SpecialObjects[i].imageInfo = &MascotsTextureInfo;

		SpecialObjects[i].movementSpeed = 8;

		RegisterObjectIntoObjectArray(&SpecialObjects[i]);
		}




	InitSingleObject(&TheRobot);

	BringObjectToLife(&TheRobot, ROBOT, 
		MULTIPLE_MODEL_ADDRESS, 9, NONE);			

	TheRobot.initialHealth = 10;
	TheRobot.currentHealth = 10;
	TheRobot.allegiance = ROBOT;

	TheRobot.movementSpeed = 5;
	TheRobot.rotationSpeed = ONE/32;

	TheRobot.canFire = TRUE;
	TheRobot.firingRate = 9;
	TheRobot.framesSinceLastFire = 0;
	TheRobot.shotSpeed = 25;
	TheRobot.shotType = FIRE_SPRITE;

	SetObjectScaling( &TheRobot, ONE/12, ONE/12, ONE/12);
	SortObjectSize(&TheRobot);

	RegisterObjectIntoObjectArray(&TheRobot);




	LinkAllObjectsToModelsOrSprites();
	
	LinkAllObjectsToTheirCoordinateSystems();





	for (i = 0; i < MAX_BULLETS; i++)
		{
		TheBullets[i].alive = FALSE;		// start off dead

			// set semi-transparency
        TheBullets[i].sprite.attribute |= GsALON;
		}
	for (i = 0; i < MAX_CUBES; i++)
		{
		TheCubes[i].alive = FALSE;		// start off dead

			// don't want to take all of this TIM, only a small portion
		TheCubes[i].sprite.w /= 4;
		TheCubes[i].sprite.h /= 4;
		TheCubes[i].sprite.u += 32;
		TheCubes[i].sprite.v += 64;

			// focal point for rotation etc should be centre, not top-left
		TheCubes[i].sprite.mx = TheCubes[i].sprite.w/2;
		TheCubes[i].sprite.my = TheCubes[i].sprite.h/2;

			// set high semi-transparency
        TheCubes[i].sprite.attribute |= GsALON;
        TheCubes[i].sprite.attribute |= 1 << 28;
		TheCubes[i].sprite.attribute |= 1 << 29;

        //TheCubes[i].sprite.r = 64;
        //TheCubes[i].sprite.g = 64;
        //TheCubes[i].sprite.b = 64;

			// start off at random rotation
		TheCubes[i].sprite.rotate = (rand() % 360) * 4096;
		}
	for (i = 0; i < MAX_SQUARES; i++)
		{
		TheSquares[i].alive = FALSE;		// start off dead
		}
	for (i = 0; i < MAX_TETRAHEDRONS; i++)
		{
		Tetrahedrons[i].alive = FALSE;		// start off dead
		}
	for (i = 0; i < MAX_TIE_FIGHTERS; i++)
		{
		TieFighters[i].alive = FALSE;		// start off dead
		}
	for (i = 0; i < MAX_SINGLE_ATTACKERS; i++)
		{
		SingleAttackers[i].alive = FALSE;		// start off dead
		}
	for (i = 0; i < MAX_INSECT_SHIPS; i++)
		{
		InsectShips[i].alive = FALSE;		// start off dead
		}
	for (i = 0; i < MAX_SPECIAL_OBJECTS; i++)
		{
		SpecialObjects[i].alive = FALSE;		// start off dead

			// don't want to take all of this TIM, only a small portion
		SpecialObjects[i].sprite.w = 34;
		SpecialObjects[i].sprite.h = 60;
		SpecialObjects[i].sprite.u += i * 34;

			// focal point for rotation etc should be centre, not top-left
		SpecialObjects[i].sprite.mx = TheCubes[i].sprite.w/2;
		SpecialObjects[i].sprite.my = TheCubes[i].sprite.h/2;

			// set high semi-transparency
        SpecialObjects[i].sprite.attribute |= GsALON;
        //SpecialObjects[i].sprite.attribute |= 1 << 28;
		//SpecialObjects[i].sprite.attribute |= 1 << 29;

		SpecialObjects[i].subType = i; // type => action
		}

	TheRobot.alive = FALSE;
}



	 
void InitialiseEnemyMovementGlobals (void)
{
	EnemyWaveFrequency = 100;
	RobotFrequency = 1000;

	TieFighterMovementSpeed = 8;
	TieFighterAngularSpeed = ANGULAR_SPEED;
	NumberTieFightersPerWave = 12;

	SingleAttackerMovementSpeed = 7;
	SingleAttackerAngularSpeed = ANGULAR_SPEED;
	SingleAttackerFiringRate = 20;
	SingleAttackerShotSpeed = 6;

	InsectShipMovementSpeed = 20;
	NumberInsectShipsPerWave = 24;

	RobotCirclingSpeed = ONE/128;
	RobotMovementSpeed = 7;
	RobotRotationSpeed = ONE/32;
	RobotPeriod = 80;
	RobotZPeriod = 80;
	RobotFiringPeriod = 80;

	SpecialObjectLikelihood = 250;
}





void InitialiseTunnel (void)
{
	SectionLength = SQUARE_SIZE;		 
	TunnelWidth = (SQUARE_SIZE * 3) / 2;  
	TunnelHeight = SQUARE_SIZE;
	NumberTunnelSections = 20;
	TunnelSpeed = INITIAL_TUNNEL_SPEED;

	LeastX = -TunnelWidth/2;
	MostX = TunnelWidth/2;
	LeastY = -TunnelHeight/2;
	MostY = TunnelHeight/2;
	LeastZ = -SectionLength;
	MostZ = NumberTunnelSections * SectionLength;

		// if remainder is zero, we keep track in sink with player on plane (z=0)
	assert(SectionLength % TunnelSpeed == 0);

	CreateTheTunnel();
}	 





void CreateTheTunnel (void)
{
	int i, z, squareID = 0;


		// ensure all squares in default state before beginning
		// ==> can reconstruct tunnel again and again
	for (i = 0; i < MAX_SQUARES; i++)
		{
		TheSquares[i].position.vx = 0;
		TheSquares[i].position.vy = 0;
		TheSquares[i].position.vz = 0;
		TheSquares[i].rotate.vx = 0;
		TheSquares[i].rotate.vy = 0;
		TheSquares[i].rotate.vz = 0;
		UpdateObjectCoordinates2 (&TheSquares[squareID].rotate,
							&TheSquares[squareID].position,
							&TheSquares[squareID].coord);
		}

	for (i = 0; i < NumberTunnelSections; i++)
		{
		z = i * SectionLength;

		if (squareID + 3 >= MAX_SQUARES)
			{
			printf("not enough squares left\n");
			return;
			}
		
		TheSquares[squareID].alive = TRUE;
		TheSquares[squareID].position.vx = TunnelWidth/2;
		TheSquares[squareID].position.vy = 0;
		TheSquares[squareID].position.vz = z;
		TheSquares[squareID].rotate.vz = ONE/4;
		UpdateObjectCoordinates2 (&TheSquares[squareID].rotate,
							&TheSquares[squareID].position,
							&TheSquares[squareID].coord);
		SetObjectScaling( &TheSquares[squareID], 
					ONE * TunnelWidth / SQUARE_SIZE, 
					ONE, ONE * SectionLength / (SQUARE_SIZE));
		SortObjectSize(&TheSquares[squareID]);
			
		squareID++;

		TheSquares[squareID].alive = TRUE;
		TheSquares[squareID].position.vx = -TunnelWidth/2;
		TheSquares[squareID].position.vy = 0;
		TheSquares[squareID].position.vz = z;
		TheSquares[squareID].rotate.vz = 3 * ONE/4;
		UpdateObjectCoordinates2 (&TheSquares[squareID].rotate,
							&TheSquares[squareID].position,
							&TheSquares[squareID].coord);
			// scaling: /2 in z to make sections half-size present/absent
		SetObjectScaling( &TheSquares[squareID], 
					ONE * TunnelWidth / SQUARE_SIZE, 
					ONE, ONE * SectionLength / (SQUARE_SIZE));
		SortObjectSize(&TheSquares[squareID]);
			
		squareID++;

		TheSquares[squareID].alive = TRUE;
		TheSquares[squareID].position.vx = 0;
		TheSquares[squareID].position.vy = TunnelHeight/2;
		TheSquares[squareID].position.vz = z;
		TheSquares[squareID].rotate.vz = ONE/2;
		UpdateObjectCoordinates2 (&TheSquares[squareID].rotate,
							&TheSquares[squareID].position,
							&TheSquares[squareID].coord);
		SetObjectScaling( &TheSquares[squareID], 
					ONE * TunnelHeight / SQUARE_SIZE, 
					ONE, ONE * SectionLength / (SQUARE_SIZE));
		SortObjectSize(&TheSquares[squareID]);
			
		squareID++;

		TheSquares[squareID].alive = TRUE;
		TheSquares[squareID].position.vx = 0;
		TheSquares[squareID].position.vy = -TunnelHeight/2;
		TheSquares[squareID].position.vz = z;
		UpdateObjectCoordinates2 (&TheSquares[squareID].rotate,
							&TheSquares[squareID].position,
							&TheSquares[squareID].coord);
		SetObjectScaling( &TheSquares[squareID], 
					ONE * TunnelHeight / SQUARE_SIZE, 
					ONE, ONE * SectionLength / (SQUARE_SIZE));
		SortObjectSize(&TheSquares[squareID]);
	
		squareID++;
		}
}





   

void UpdateTheTunnel (void)
{
	int i;
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
			//aliveCount += 4;
			distance = TheSquares[i].coord.coord.t[2] - PlayersShip.coord.coord.t[2];
			if (distance <= 0)
				{
				TheSquares[i].handler.attribute |= GsDIV1;
				TheSquares[i+1].handler.attribute |= GsDIV1;
				TheSquares[i+2].handler.attribute |= GsDIV1;
				TheSquares[i+3].handler.attribute |= GsDIV1;
				//count += 4;
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

	HandleEnemyClass();

	HandleSpecialObjectClass();

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
					case TETRAHEDRON:
						HandleATetrahedron(object);
						break;
					case TIE_FIGHTER:
						HandleATieFighter(object);
						break;
					case SINGLE_ATTACKER:
						HandleSingleAttacker(object);
						break;
					case INSECT_SHIP:
						HandleAnInsectShip(object);
						break;
					case SPECIAL_OBJECT:
						HandleSpecialObject(object);
						break;
					case ROBOT:
						HandleRobot(object);
						break;
					default:
						assert(FALSE);
					}
				}
			}
		}

	HandleTieFighterWaves();

endOfHandleAllObjects:
	;
}




void HandleThePlayer (ObjectHandler* object)
{
	assert(object->type == PLAYER);

	UpdateObjectCoordinates4 (object);
}







#define BULLET_LIFETIME 60

void HandleABullet (ObjectHandler* object)
{
	assert(object->type == BULLET);

	UpdateObjectCoordinates4 (object);

		// kill off after short time
	if (object->lifeTime > BULLET_LIFETIME)
		{
		object->alive = FALSE;
		object->lifeTime = 0;
		}

	if (object->position.vx <= LeastX || object->position.vx >= MostX
		|| object->position.vy <= LeastY || object->position.vy >= MostY
		|| object->position.vz <= LeastZ || object->position.vz >= MostZ)
		{
		object->alive = FALSE;
		object->lifeTime = 0;
		}
}






#define CUBE_LIFETIME 24

void HandleACube (ObjectHandler* object)
{
	assert(object->type == CUBE);
	
	UpdateObjectCoordinates4 (object);

		// make it twist a lot (10 degrees per frame)
	object->sprite.rotate += 10 * 4096;

		// kill off after short time
	if (object->lifeTime > CUBE_LIFETIME)
		{
		object->alive = FALSE;
		object->lifeTime = 0;
		}

		// fade out colours
	if (object->lifeTime % 3 == 0)
		{
		SortCubeSpriteDimming(object);
		}
						 
	if (object->position.vx <= LeastX || object->position.vx >= MostX
		|| object->position.vy <= LeastY || object->position.vy >= MostY
		|| object->position.vz <= LeastZ || object->position.vz >= MostZ)
		{
		object->alive = FALSE;
		object->lifeTime = 0;
		}
}



 
	 
 
void SortCubeSpriteDimming (ObjectHandler* object)
{
	// rgb all 128 at start, end up at 32 over 24 frames
	// eg reduce each by 4 per frame
	
	object->sprite.r = 128 - (4 * object->lifeTime);
	object->sprite.g = 128 - (4 * object->lifeTime);
	object->sprite.b = 128 - (4 * object->lifeTime);	 
	
		// also reduce in size slightly (back to ONE at 25 frames)
	object->scaleX = (ONE) - (object->lifeTime * 160);		// 4096 / 25 is roughly 160
	object->scaleY = (ONE) - (object->lifeTime * 160); 		  
}
	
	


   




#define TETRAHEDRON_LIFETIME 60

void HandleATetrahedron (ObjectHandler* object)
{
	assert(object->type == TETRAHEDRON);

	UpdateObjectCoordinates4 (object);
		
		// kill off after short time
	if (object->lifeTime > TETRAHEDRON_LIFETIME)
		{
		object->alive = FALSE;
		object->lifeTime = 0;
		}
						 
	if (object->position.vx <= LeastX || object->position.vx >= MostX
		|| object->position.vy <= LeastY || object->position.vy >= MostY
		|| object->position.vz <= LeastZ || object->position.vz >= MostZ)
		{
		object->alive = FALSE;
		object->lifeTime = 0;
		}
}






void HandleATieFighter (ObjectHandler* object)
{
	int i;
	
	assert(object->type == TIE_FIGHTER);

	switch(object->specialMovement)
		{
		case STATIC_HELIX:
		case SWITCHING_HELIX:
		case DARTING_STATIC_HELIX:
		case DARTING_SWITCHING_HELIX:
			switch(object->whichWay)
				{
				case CLOCKWISE:
					object->angle += object->angleIncrement;
					break;
				case ANTICLOCKWISE:
					object->angle -= object->angleIncrement;
					break;
				default:	
					assert(FALSE);
				}

			object->coord.coord.t[0] 
				= object->firstVector.vx + ((object->radius * rcos(object->angle)) >> 12);
			object->coord.coord.t[1] 
				= object->firstVector.vy + ((object->radius * rsin(object->angle)) >> 12);
			KeepWithinRange(object->coord.coord.t[0], TIE_FIGHTER_MIN_X, TIE_FIGHTER_MAX_X);
			KeepWithinRange(object->coord.coord.t[1], TIE_FIGHTER_MIN_Y, TIE_FIGHTER_MAX_Y);
			object->coord.coord.t[2] -= object->movementSpeed;
			object->coord.flg = 0;
			break;
		default:	
			assert(FALSE);
		}

	if (object->coord.coord.t[2] < -SectionLength)
		{
		object->alive = FALSE;
		}

	if (object->currentHealth <= 0)
		{
		HandleEnemysDeath(object);	   
		}
}
	

   	   	   



void HandleSingleAttacker (ObjectHandler* object)
{
	int waitingDistance = 2 * SQUARE_SIZE;
	VECTOR firingDirection;
	int whichJiggle;
	int x, y;

	assert(object->type == SINGLE_ATTACKER);

	if (object->position.vz >= waitingDistance)
		{
		object->position.vz -= object->movementSpeed;
		}


		// main movement chunk
		{
		firingDirection.vx = PlayersShip.position.vx - object->position.vx;
		firingDirection.vy = PlayersShip.position.vy - object->position.vy;
		firingDirection.vz = PlayersShip.position.vz - object->position.vz;

		HandleObjectContinuousFiring(object, &firingDirection);

		switch(object->subType)
			{
			case RANDOM_JIGGLER:		// 4 directions, actionState stores which
				if (object->lifeTime % object->specialTimeLimit == 0)
					{
					whichJiggle = rand() % 4;	   // choose a new one
					object->actionState = whichJiggle;
					}
				else
					whichJiggle = object->actionState;

				switch(whichJiggle)
					{
					case 0:
						object->position.vx -= object->movementSpeed; 
						break;
					case 1:
						object->position.vx += object->movementSpeed;
						break;
					case 2:
						object->position.vy -= object->movementSpeed;
						break;
					case 3:
						object->position.vy += object->movementSpeed;
						break;
					default:
						assert(FALSE);
					}

				KeepWithinRange(object->position.vx, PLAYER_MIN_X, PLAYER_MAX_X);
				KeepWithinRange(object->position.vy, PLAYER_MIN_Y, PLAYER_MAX_Y);
				break;
			case X_Y_EVADER:
				x = PlayersShip.coord.coord.t[0] - object->coord.coord.t[0];
				y = PlayersShip.coord.coord.t[1] - object->coord.coord.t[1];

					// if very close in xy terms, jiggle randomly
				if (abs(x) < object->movementSpeed && abs(y) < object->movementSpeed)
					{
					whichJiggle = rand() % 4;
						
					switch(whichJiggle)
						{
						case 0:
							object->position.vx -= object->movementSpeed; 
							break;
						case 1:
							object->position.vx += object->movementSpeed;
							break;
						case 2:
							object->position.vy -= object->movementSpeed;
							break;			
						case 3:
							object->position.vy += object->movementSpeed;
							break;
						default:
							assert(FALSE);
						}
					}
				else		 // otherwise, always move away
					{
					if (x > 0)
						object->position.vx -= object->movementSpeed;
					else
						object->position.vx += object->movementSpeed;

					if (y > 0)
						object->position.vy -= object->movementSpeed;
					else
						object->position.vy += object->movementSpeed;		
					}

				KeepWithinRange(object->position.vx, PLAYER_MIN_X, PLAYER_MAX_X);
				KeepWithinRange(object->position.vy, PLAYER_MIN_Y, PLAYER_MAX_Y);
				break;
			case STATIC_CIRCLER:
				switch(object->whichWay)
					{
					case CLOCKWISE:
						object->angle += object->angleIncrement;
						break;
					case ANTICLOCKWISE:
						object->angle -= object->angleIncrement;
						break;
					default:	
						assert(FALSE);
					}

				object->position.vx 
					= object->firstVector.vx + ((object->radius * rcos(object->angle)) >> 12);
				object->position.vy 
					= object->firstVector.vy + ((object->radius * rsin(object->angle)) >> 12);
				KeepWithinRange(object->position.vx, TIE_FIGHTER_MIN_X, TIE_FIGHTER_MAX_X);
				KeepWithinRange(object->position.vy, TIE_FIGHTER_MIN_Y, TIE_FIGHTER_MAX_Y);
				break;
			case SWITCHING_CIRCLER:
				switch(object->whichWay)
					{
					case CLOCKWISE:
						object->angle += object->angleIncrement;
						break;
					case ANTICLOCKWISE:
						object->angle -= object->angleIncrement;
						break;
					default:	
						assert(FALSE);
					}

				object->position.vx 
					= object->firstVector.vx + ((object->radius * rcos(object->angle)) >> 12);
				object->position.vy 
					= object->firstVector.vy + ((object->radius * rsin(object->angle)) >> 12);
				KeepWithinRange(object->position.vx, TIE_FIGHTER_MIN_X, TIE_FIGHTER_MAX_X);
				KeepWithinRange(object->position.vy, TIE_FIGHTER_MIN_Y, TIE_FIGHTER_MAX_Y);

					// periodically switch direction of circling
				if (object->lifeTime % object->specialTimeLimit == 0)
					{
					object->whichWay = 1 - object->whichWay;
						// make the timing not entirely predictable
					object->specialTimeLimit = 30 + (rand() % 60);
					}
				break;
			default:
				assert(FALSE);
			}
		}

	UpdateObjectCoordinates4 (object);

	if (object->currentHealth <= 0)
		{
		HandleEnemysDeath(object);
		}
}




void HandleAnInsectShip (ObjectHandler* object)
{
	assert(object->type == INSECT_SHIP);

	object->coord.coord.t[2] -= object->movementSpeed;
	object->coord.flg = 0;
								 
	if (object->coord.coord.t[2] < -SectionLength)
		object->alive = FALSE;

	if (object->currentHealth < 1)
		HandleEnemysDeath(object);
}



void HandleSpecialObject (ObjectHandler* object)
{
	int newX, newY;

	assert(object->type == SPECIAL_OBJECT);

		// for when object being displayed as stationary 2D sprite
	if (object->actionState == HOLD_STILL)
		return;

		// make it spin a little (1 degrees per frame)
	switch(object->whichWay)
		{
		case CLOCKWISE:			
			object->sprite.rotate += 1 * 4096;		
			break;
		case ANTICLOCKWISE:		
			object->sprite.rotate -= 1 * 4096;		
			break;
		default:				
			assert(FALSE);
		}

	while (object->sprite.rotate >= 4096 * 360)
		object->sprite.rotate -= 4096 * 360;
	while (object->sprite.rotate < 0)
		object->sprite.rotate += 4096 * 360;	

	newX = object->position.vx + object->velocity.vx;
	newY = object->position.vy + object->velocity.vy;

		// bounce off surfaces of virtual enclosing box
	if (newX < PLAYER_MIN_X || newX > PLAYER_MAX_X)
		{
		object->velocity.vx = -object->velocity.vx;
		}
	if (newY < PLAYER_MIN_Y || newY > PLAYER_MAX_Y)
		{
		object->velocity.vy = -object->velocity.vy;
		}

	UpdateObjectCoordinates4 (object);

		// beyond edge of tunnel, kill off
	if (object->coord.coord.t[2] < -SectionLength)
		object->alive = FALSE;
}





void HandleRobot (ObjectHandler* object)
{
	int cyclePoint;
	int zCyclePoint, zPhase;
	int firingPoint, firingPhase;		// either fire or circle fast
	MATRIX tempMatrix;
	int direction;
	VECTOR firingDirection;
	
	assert(object->type == ROBOT);

		// firing: two phases in loop:
		// first phase, fire fast and circle slowly; second phase, just circle fast
	firingPoint = object->lifeTime % object->thirdPeriod;
	firingPhase = (firingPoint * 2) / object->thirdPeriod;
	if (firingPhase == 0)
		{
		firingDirection.vx = PlayersShip.position.vx - object->position.vx;
		firingDirection.vy = PlayersShip.position.vy - object->position.vy;
		firingDirection.vz = PlayersShip.position.vz - object->position.vz;

		HandleObjectContinuousFiring(object, &firingDirection);
		object->angleIncrement = RobotCirclingSpeed;
		}
	else
		object->angleIncrement = RobotCirclingSpeed * 2;



	cyclePoint = object->lifeTime % object->specialTimeLimit;

	if (cyclePoint == 0)
		{
		object->whichWay = 1 - object->whichWay;

			// set direction for circle centre moving
		direction = rand() % 4;
		switch(direction)
			{
			case 0: object->secondVector.vx = 1; break;
			case 1: object->secondVector.vx = -1; break;
			case 2: object->secondVector.vy = 1; break;
			case 3: object->secondVector.vy = -1; break;
			default:			assert(FALSE);
			}
		}

	object->position.vx = object->firstVector.vx + 
							((object->radius * rcos(object->angle)) >> 12);
	object->position.vy = object->firstVector.vy + 
							((object->radius * rsin(object->angle)) >> 12);

	KeepWithinRange(object->position.vx, ROBOT_MIN_X, ROBOT_MAX_X);
	KeepWithinRange(object->position.vy, ROBOT_MIN_Y, ROBOT_MAX_Y);

	

		// update angle
	switch(object->whichWay)
		{
		case CLOCKWISE:	object->angle += object->angleIncrement; break;
		case ANTICLOCKWISE:	object->angle -= object->angleIncrement; break;
		default:		assert(FALSE);
		}

		// move centre of circle
	object->firstVector.vx += object->secondVector.vx;
	object->firstVector.vy += object->secondVector.vy;
	KeepWithinRange(object->firstVector.vx, ROBOT_MIN_X, ROBOT_MAX_X);
	KeepWithinRange(object->firstVector.vy, ROBOT_MIN_Y, ROBOT_MAX_Y);

	
		// in z direction, it moves back and forth and waits 
	zCyclePoint = object->lifeTime % object->secondPeriod;
	zPhase = (zCyclePoint * 4) / object->secondPeriod;
	switch(zPhase)
		{
		case 0: 								break;		 
		case 1:		object->position.vz -= 5; 	break;
		case 2:									break;
		case 3:		object->position.vz += 5; 	break;
		default:				assert(FALSE);
		}  
	
		// set position by absolute coordinates	
   	object->coord.coord.t[0] = object->position.vx;
	object->coord.coord.t[1] = object->position.vy;
	object->coord.coord.t[2] = object->position.vz;

	   	// tell GTE that coordinate system has been updated
	object->coord.flg = 0;

	   	// beyond edge of tunnel, kill off
	if (object->coord.coord.t[2] < -SectionLength)
		object->alive = FALSE;

	if (object->currentHealth < 1)
		HandleEnemysDeath(object);
}

	




void HandleTieFighterWaves (void)
{
	int i;
	int id = -1;
	ObjectHandler* fighter;
	
	for (i = 0; i < MAX_TIE_FIGHTERS; i++)
		{
		if (TieFighters[i].alive == TRUE)
			{
			id = i;
			break;
			}
		}
	
	if (id == -1)
		return;
		
	fighter = &TieFighters[id]; 
		
	switch(fighter->specialMovement)
		{
		case STATIC_HELIX:	 // nowt to be done
			return;
			break;
		case SWITCHING_HELIX:
			if (fighter->lifeTime % fighter->specialTimeLimit == 0)
				{	   
					// switch all circling directions
				for (i = 0; i < MAX_TIE_FIGHTERS; i++)
					{
					if (TieFighters[i].alive == TRUE)
						{
						TieFighters[i].whichWay = 1 - TieFighters[i].whichWay;
						}
					}

					// reset period of switching
				fighter->specialTimeLimit = SWITCH_PERIOD + (rand() % SWITCH_PERIOD);
				}
			break;
		case DARTING_STATIC_HELIX:
			if (DartOn == FALSE && ((fighter->lifeTime % fighter->specialTimeLimit) == 0))
				{
				DartOn = TRUE;
				DartDirection = rand() % 4;
				DartFrameStart = frameNumber+1;
				DartDuration = 8 + (rand() % 8);
				return;
				}
			else
				{
				if (DartOn == TRUE)
					{
					if (frameNumber == DartFrameStart + DartDuration)
						{
						DartOn = FALSE;
						return;
						}
					else
						{
							// make them all dart to one side
						for (i = 0; i < MAX_TIE_FIGHTERS; i++)
							{
							if (TieFighters[i].alive == TRUE)
								{
								switch(DartDirection)
									{
									case 0:	
										TieFighters[i].firstVector.vx -= 2;
										break;
									case 1:	
										TieFighters[i].firstVector.vx += 2;
										break;
									case 2:	
										TieFighters[i].firstVector.vy -= 2;
										break;
									case 3:	
										TieFighters[i].firstVector.vy += 2;
										break;
									default:
										assert(FALSE);
									}
								TieFighters[i].coord.flg = 0;
								}
							}
						}
					}
				}
			break;
		case DARTING_SWITCHING_HELIX:
			if (fighter->lifeTime % fighter->specialTimeLimit == 0)
				{
					// switch all circling directions
				for (i = 0; i < MAX_TIE_FIGHTERS; i++)
					{
					if (TieFighters[i].alive == TRUE)
						{
						TieFighters[i].whichWay = 1 - TieFighters[i].whichWay;
						}
					}
					// reset period of switching
				fighter->specialTimeLimit = SWITCH_PERIOD + (rand() % SWITCH_PERIOD);
				}

			if (DartOn == FALSE && ((fighter->lifeTime % fighter->specialTimeLimit) == 0))
				{
				DartOn = TRUE;
				DartDirection = rand() % 4;
				DartFrameStart = frameNumber+1;
				DartDuration = 8 + (rand() % 8);
				return;
				}
			else
				{
				if (DartOn == TRUE)
					{
					if (frameNumber == DartFrameStart + DartDuration)
						{
						DartOn = FALSE;
						return;
						}
					else
						{
							// make them all dart to one side
						for (i = 0; i < MAX_TIE_FIGHTERS; i++)
							{
							if (TieFighters[i].alive == TRUE)
								{
								switch(DartDirection)
									{
									case 0:	
										TieFighters[i].firstVector.vx -= 2;
										break;
									case 1:	
										TieFighters[i].firstVector.vx += 2;
										break;
									case 2:	
										TieFighters[i].firstVector.vy -= 2;
										break;
									case 3:	
										TieFighters[i].firstVector.vy += 2;
										break;
									default:
										assert(FALSE);
									}
								TieFighters[i].coord.flg = 0;
								}
							}
						}
					}
				}
			break;
		default:
			assert(FALSE);
		} 
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





	// no rotation, world-relative movement
void UpdateObjectCoordinates4 (ObjectHandler* object)
{
	object->position.vx += object->velocity.vx;
	object->position.vy += object->velocity.vy;
	object->position.vz += object->velocity.vz; 
	
	if (object->movementMomentumFlag != TRUE)
		{
		setVECTOR( &object->velocity, 0, 0, 0);		  // no momentum
		}

		// set position by absolute coordinates	
   	object->coord.coord.t[0] = object->position.vx;
	object->coord.coord.t[1] = object->position.vy;
	object->coord.coord.t[2] = object->position.vz;

	   	// tell GTE that coordinate system has been updated
	object->coord.flg = 0;
}








void HandleEnemyClass (void)
{
	int whichEnemyType;

	if ( (frameNumber % RobotFrequency == 0) && (TheRobot.alive == FALSE) )
		{
		CreateRobot();
		}

	if (frameNumber % EnemyWaveFrequency == 0 && frameNumber > 0)
		{
		if (NumberOfEnemiesLeft() == 0)
			{
			whichEnemyType = rand() % NUMBER_ENEMY_CLASSES;
			//printf("whichEnemyType: %d\n", whichEnemyType);

			switch(whichEnemyType)
				{
				case TIE_FIGHTER_CLASS:		 
					CreateWaveOfTieFighters();
					break;
				case SINGLE_ATTACKER_CLASS:
					CreateSingleAttacker();
					break;
				case INSECT_SHIP_CLASS:
					CreateWaveOfInsectShips();
					break;
				default:
					assert(FALSE);
				}

				// could make sound specific to wave type -> audio signal warning
			#if 0 
				TurnOnSound (53, 0, 60, 1, 20, 127, 127);
			#endif
			}
		}
}




int NumberOfEnemiesLeft (void)
{
	int numberTieFighters, numberSingleAttackers, numberInsectShips;

	numberTieFighters = CountNumberOfLivingTypedObjects(TIE_FIGHTER);
	numberSingleAttackers = CountNumberOfLivingTypedObjects(SINGLE_ATTACKER);
	numberInsectShips = CountNumberOfLivingTypedObjects(INSECT_SHIP);

	return (numberTieFighters + numberSingleAttackers + numberInsectShips);
}

	
   



void CreateWaveOfTieFighters (void)
{
	int idList[NumberTieFightersPerWave];
	int i;
	ObjectHandler* fighter; 
	int whichWay;		  // which way round do we go ??
	int initialAngle, angleStep;
	int startX, startY; 
	int waveType = rand() % NUMBER_WAVE_TYPES;

	whichWay = rand() % 2;
	initialAngle = rand() % ONE;
	angleStep = ONE / NumberTieFightersPerWave;
	startX = (rand() % 65) - 32;
	startY = (rand() % 18) - 9;

	switch(waveType)
		{
		case STATIC_HELIX:
		case SWITCHING_HELIX:
		case DARTING_STATIC_HELIX:
		case DARTING_SWITCHING_HELIX:
			{
			for (i = 0; i < NumberTieFightersPerWave; i++)
				{
				idList[i] = FindNextFreeTieFighterId();
				assert(idList[i] != -1);

				fighter = &TieFighters[idList[i]];

				ReviveObject(fighter);
				
				fighter->specialMovement = waveType;
				fighter->whichWay = whichWay;
				fighter->specialTimeLimit = SWITCH_PERIOD + (rand() % SWITCH_PERIOD); 

			   	if (i == 0)			// leader
					{ 
					fighter->coord.coord.t[2] = Z_DISTANCE;
					}
				else
					{
					fighter->coord.coord.t[2] 
						= TieFighters[idList[i-1]].coord.coord.t[2] + Z_SEPARATION;
					} 

				fighter->angle = initialAngle + (i * angleStep);
				fighter->angleIncrement = TieFighterAngularSpeed;
				fighter->radius = RADIUS;
					// vector specifying centre of circle
				setVECTOR( &fighter->firstVector, startX, startY, 0);

				fighter->coord.flg = 0;
				}
			}
			break;
		default:
			assert(FALSE);
		}
}



	


void CreateSingleAttacker (void)
{
	int id;
	ObjectHandler* attacker;
	int startX, startY;

	id = FindNextFreeSingleAttackerId();
	if (id == -1)
		return;

	attacker = &SingleAttackers[id];

	ReviveObject(attacker);
	attacker->subType = rand() % NUMBER_ATTACKER_SUBTYPES;
	
	attacker->lifeTime = 0;
	
	attacker->position.vx = 0;
	attacker->position.vy = 0;
	attacker->position.vz = 5 * SQUARE_SIZE;

		// now initialise by subtype
	switch(attacker->subType)
		{
		case RANDOM_JIGGLER:
			attacker->actionState = rand() % 4;
			attacker->specialTimeLimit = 10 + (rand() % 10);
			break;
		case X_Y_EVADER:
			break;
		case STATIC_CIRCLER:
			attacker->whichWay = rand() % 2;
			//attacker->radius = rand() % NumberPositions;
			attacker->radius = RADIUS;
			attacker->angleIncrement = SingleAttackerAngularSpeed;
			attacker->angle = rand() % ONE;

			startX = (rand() % 65) - 32;
			startY = (rand() % 18) - 9;
				// vector specifying centre of circle
			setVECTOR( &attacker->firstVector, startX, startY, 0);
			break;
		case SWITCHING_CIRCLER:
			attacker->whichWay = rand() % 2;
			//attacker->radius = rand() % NumberPositions;
			attacker->radius = RADIUS;
			attacker->specialTimeLimit = 30 + (rand() % 60);
			attacker->angleIncrement = SingleAttackerAngularSpeed;
			attacker->angle = rand() % ONE;

			startX = (rand() % 65) - 32;
			startY = (rand() % 18) - 9;
				// vector specifying centre of circle
			setVECTOR( &attacker->firstVector, startX, startY, 0);
			break;
		default:
			assert(FALSE);
		}
}

	
	



void CreateWaveOfInsectShips (void)
{
	int i;
	int startX, startY;

	int idList[NumberInsectShipsPerWave];
	ObjectHandler* insect; 

  	for (i = 0; i < NumberInsectShipsPerWave; i++)
		{
		idList[i] = FindNextFreeInsectShipId();
		assert(idList[i] != -1);

		insect = &InsectShips[idList[i]];

		ReviveObject(insect);

		startX = (rand() % 161) - 80;
		startY = (rand() % 99) - 49;

		insect->coord.coord.t[0] = startX; 
		insect->coord.coord.t[1] = startY; 
		insect->coord.coord.t[2] = INSECT_INITIAL_Z + (i * INSECT_Z_SEPARATION);
		insect->coord.flg = 0;
		}
}
	





void CreateRobot (void)
{
	int startX, startY;

	assert(TheRobot.alive == FALSE);

	//ReviveObject(&TheRobot);

	TheRobot.alive = TRUE;

		// centre of circle
	startX = (rand() % 17) - 8;
	startY = (rand() % 17) - 8;
	TheRobot.firstVector.vx = startX;
	TheRobot.firstVector.vy = startY;

	TheRobot.whichWay = rand() % 2;
	TheRobot.angle = rand() % ONE;
	TheRobot.angleIncrement = RobotCirclingSpeed;
	TheRobot.radius = RADIUS;
	TheRobot.movementSpeed = RobotMovementSpeed;
	TheRobot.rotationSpeed = RobotRotationSpeed;
	TheRobot.specialTimeLimit = RobotPeriod;
	TheRobot.secondPeriod = RobotZPeriod;
	TheRobot.thirdPeriod = RobotFiringPeriod;

	setVECTOR( &TheRobot.rotate, 0, 0, 0);
	setVECTOR( &TheRobot.position, 0, 0, 3 * SQUARE_SIZE/2);
}

	
	
	// NOTE: these functions below are code duplication
	// would be better style to avoid this

	// e.g. FindNextFreeObjectSubtypeID (ObjectHandler* array, int length)

int FindNextFreeTetrahedronId (void)
{
	int result = -1, i;

	for (i = 0; i < MAX_TETRAHEDRONS; i++)
		{
		if (Tetrahedrons[i].alive == FALSE)
			{
			result = i;
			break;
			}
		}

	return result;
}




int FindNextFreeTieFighterId (void)
{
	int result = -1, i;

	for (i = 0; i < MAX_TIE_FIGHTERS; i++)
		{
		if (TieFighters[i].alive == FALSE)
			{
			result = i;
			break;
			}
		}

	return result;
}


int FindNextFreeSingleAttackerId (void)
{
	int result = -1, i;

	for (i = 0; i < MAX_SINGLE_ATTACKERS; i++)
		{
		if (SingleAttackers[i].alive == FALSE)
			{
			result = i;
			break;
			}
		}

	return result;
}




int FindNextFreeInsectShipId (void)
{
	int result = -1, i;

	for (i = 0; i < MAX_INSECT_SHIPS; i++)
		{
		if (InsectShips[i].alive == FALSE)
			{
			result = i;
			break;
			}
		}

	return result;
}



int FindNextFreeSpecialObjectId (void)
{
	int result = -1, i;

	for (i = 0; i < MAX_SPECIAL_OBJECTS; i++)
		{
		if (SpecialObjects[i].alive == FALSE)
			{
			result = i;
			break;
			}
		}

	return result;
}






void HandleSpecialObjectClass (void)
{
	int count, id;
	ObjectHandler* object;
	int index;

	count = CountNumberOfLivingTypedObjects(SPECIAL_OBJECT);

	if (frameNumber > 0 && (count == 0) && ((rand() % SpecialObjectLikelihood) == 0) )
		{
		id = FindNextFreeSpecialObjectId();
		object = &SpecialObjects[id];

		object->alive = TRUE;
		object->actionState = -1;
		object->position.vx = (rand() % 120) - 60;
		object->position.vy = (rand() % 80) - 40;
		object->position.vz = 6 * SQUARE_SIZE;	

		object->sprite.rotate = (rand() % 360) * ONE;

		object->velocity.vx = 1 - (2 * rand() % 2);
		object->velocity.vy = 1 - (2 * rand() % 2);
		object->velocity.vz = -object->movementSpeed;
		object->movementMomentumFlag = TRUE;

		object->whichWay = rand() % 2;		// which way it spins
		
		//object->subType = rand() % NUMBER_SPECIAL_OBJECT_SUBTYPES;
		index = rand() % SIZE_OF_SPECIAL_OBJECT_TYPE_TABLE;
		object->subType = SpecialObjectTypeTable[index];	
		printf("special object type %d created\n", object->subType);
		}
	else			// at least one exists
		return;
}
	
	


#define SMALL_OFFSET 4

void PositionObjectOnInsideOfTunnel (ObjectHandler* object, int z)
{
	int whichEdge, offset;

	whichEdge = rand() % 4;

	switch(whichEdge)
		{
		case 0:			// ceiling
			offset = rand() % (TunnelWidth - (2 * SMALL_OFFSET));
			object->position.vx	= (-TunnelWidth / 2) + offset;
			object->position.vy = -TunnelHeight / 2 + SMALL_OFFSET; 
			object->rotate.vz = ONE/2;
			break;
		case 1:			// floor
			offset = rand() % (TunnelWidth - (2 * SMALL_OFFSET));
			object->position.vx	= (-TunnelWidth / 2) + offset;
			object->position.vy = TunnelHeight / 2 - SMALL_OFFSET;
			break;
		case 2:			// left wall
			offset = rand() % (TunnelHeight - (2 * SMALL_OFFSET));
			object->position.vx	= -TunnelWidth / 2 + SMALL_OFFSET;
			object->position.vy = (-TunnelHeight / 2) + offset;
			object->rotate.vz = ONE/4;
			break;
		case 3:			// right wall
			offset = rand() % (TunnelHeight - (2 * SMALL_OFFSET));
			object->position.vx	= TunnelWidth / 2 - SMALL_OFFSET;
			object->position.vy = (-TunnelHeight / 2) + offset;
			object->rotate.vz = 3 * ONE/4;
			break;
		}

	object->position.vz = z;

	object->coord.coord.t[0] = object->position.vx;
	object->coord.coord.t[1] = object->position.vy;
	object->coord.coord.t[2] = object->position.vz;
}




void HandleObjectContinuousFiring (ObjectHandler* object, VECTOR* direction)
{
	assert(object->canFire == TRUE);

	if (object->framesSinceLastFire > object->firingRate)
		{
		ObjectFiresAShot(object, direction);
		object->framesSinceLastFire = 0;
		}
}





void ObjectFiresAShot (ObjectHandler* object, VECTOR* direction)
{
	int bulletID = -1;
	int i;
	VECTOR normalisedDirection;
	int projectileID = -1;
	ObjectHandler* projectile = NULL;

	assert(object->canFire == TRUE);

	normalisedDirection.vx = direction->vx;
	normalisedDirection.vy = direction->vy;
	normalisedDirection.vz = direction->vz;
	NormaliseVector (&normalisedDirection);

	//assert(object->shotType == FIRE_SPRITE);		// OTHERS DISABLED FOR NOW

	switch(object->shotType)
		{
		case FIRE_SPRITE:
			for (i = 0; i < MAX_BULLETS; i++)
				{
				if (TheBullets[i].alive == FALSE)
					{
					projectileID = i;
					projectile = &TheBullets[projectileID];
					break;
					}
				}
			break;
		case TETRAHEDRON_MODEL:
			for (i = 0; i < MAX_TETRAHEDRONS; i++)
				{
				if (Tetrahedrons[i].alive == FALSE)
					{
					projectileID = i;
					projectile = &Tetrahedrons[projectileID];
					break;
					}
				}
			break;
		default:
			assert(FALSE);
		}

	if (projectileID == -1)
		return;					// no free projectiles
	else
		{
		assert(projectile != NULL);
		//printf("object type %d firing a shot type %d\n", object->type, object->shotType);

		projectile->alive = TRUE;
		projectile->lifeTime = 0;
		projectile->allegiance = object->allegiance;			
		
			// send in direction of ship at time of fire
		projectile->velocity.vx 
				= ((object->shotSpeed * normalisedDirection.vx) / ONE);
		projectile->velocity.vy 
				= ((object->shotSpeed * normalisedDirection.vy) / ONE);
		projectile->velocity.vz 
				= ((object->shotSpeed * normalisedDirection.vz) / ONE);
		   		
		projectile->movementMomentumFlag = TRUE; 
		
					// emitted from nose of ship
		projectile->position.vx = object->coord.coord.t[0];
		projectile->position.vy = object->coord.coord.t[1];
		projectile->position.vz = object->coord.coord.t[2];
		projectile->coord.coord.t[0] = projectile->position.vx;
		projectile->coord.coord.t[1] = projectile->position.vy;
		projectile->coord.coord.t[2] = projectile->position.vz;
		projectile->coord.flg = 0;

		TurnOnSound (1, 0, 60, 1, 10, 48, 48);
		}
}

	




void InitialiseView( void )
{
	ProjectionDistance = 192;			// used to be 312

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



   	



void InitialiseLighting(void)
{
	SortTheFlatLights();


		
		// general lighting
	GsSetAmbient(ONE/4, ONE/4, ONE/4);




	Fogging.dqa = -960;
	Fogging.dqb = 5120*5120;
	Fogging.rfc = 0; Fogging.gfc = 0; Fogging.bfc = 4;
	GsSetFogParam (&Fogging);


	lightmode = 1;			// allows fogging  
	GsSetLightMode(lightmode);
}




void SortTheFlatLights (void)
{
	#if 0		   // two blue lights, one up, one down
				// makes floor and ceiling watery, walls green/grey
	TheLights[0].vx = 0; TheLights[0].vy = ONE; TheLights[0].vz = 0;

	TheLights[0].r = 0; TheLights[0].g = 0; TheLights[0].b = 64;

	GsSetFlatLight(0, &TheLights[0]);


	TheLights[1].vx = 0; TheLights[1].vy = -ONE; TheLights[1].vz = 0;

	TheLights[1].r = 0; TheLights[1].g = 0; TheLights[1].b = 64;

	GsSetFlatLight(1, &TheLights[1]);
#endif


	TheLights[0].vx = 1; TheLights[0].vy = 1; TheLights[0].vz = 1;

	TheLights[0].r = 128; TheLights[0].g = 42; TheLights[0].b = 128;

	GsSetFlatLight(0, &TheLights[0]);


	TheLights[1].vx = -1; TheLights[1].vy = -1; TheLights[1].vz = 1;

	TheLights[1].r = 128; TheLights[1].g = 42; TheLights[1].b = 128;

	GsSetFlatLight(1, &TheLights[1]);
}





#define LIGHTING_CYCLE_TIME 120

void OscillateLighting (void)
{
	int cyclePoint = frameNumber % LIGHTING_CYCLE_TIME;	  
	int angle, i;

	TheLights[0].r = 0; TheLights[0].g = 0; TheLights[0].b = 64;
	TheLights[1].r = 0; TheLights[1].g = 0; TheLights[1].b = 64;
	angle = cyclePoint * (4096 / LIGHTING_CYCLE_TIME);
	TheLights[0].vx = rcos(angle); TheLights[0].vy = rsin(angle); TheLights[0].vz = 0;
	TheLights[1].vx = -TheLights[0].vx; TheLights[1].vy = -TheLights[0].vy; TheLights[1].vz = 0;

	for (i = 0; i < 2; i++)
		GsSetFlatLight(i, &TheLights[i]);
}





#define MINOR_OSCILLATION_PERIOD 60

void MinorOscillateLighting (void)
{
	int cyclePoint = frameNumber % MINOR_OSCILLATION_PERIOD;	  
	int angle;

	TheLights[2].r = 128; TheLights[2].g = 12; /* 42 */ TheLights[2].b = 128;
	angle = cyclePoint * (4096 / MINOR_OSCILLATION_PERIOD);
	TheLights[2].vx = rcos(angle); TheLights[2].vy = rsin(angle); TheLights[2].vz = 0;

	GsSetFlatLight(2, &TheLights[2]);	
}







void OscillateView (void)
{
	int x, y;



#if 0		 // no more switching within a given effect
		// switch once in a while
	if (frameNumber % ViewDistortPeriod == 0)
		{
		for (;;)
			{
			ViewDistortionMode = rand() % 4;
			if (ViewDistortionMode != OldViewDistortionMode)
				break;
			}
		ViewOscillationJustBegun = TRUE;
		}
#endif

	if (ViewOscillationJustBegun == TRUE)
		{
		switch(ViewDistortionMode)
			{
			case LEFT_TO_RIGHT:
				ViewHorizontalVelocity 
					= ViewBaseVelocity - (2 * ViewBaseVelocity * (rand() % 2));
				break;
			case UP_AND_DOWN:
				ViewVerticalVelocity 
					= ViewBaseVelocity - (2 * ViewBaseVelocity * (rand() % 2));
				break;
			case CIRCLING_CLOCKWISE:
			case CIRCLING_ANTICLOCKWISE:
				ViewAngle = rand() % ONE;
				break;
			default:
				assert(FALSE);
			}

		ViewOscillationJustBegun = FALSE;
		}

	x = view.vpx; y = view.vpy;
	switch(ViewDistortionMode)
		{
		case LEFT_TO_RIGHT:
			if (view.vpx + ViewHorizontalVelocity < PLAYER_MIN_X 
				|| view.vpx + ViewHorizontalVelocity > PLAYER_MAX_X)
					{
					ViewHorizontalVelocity = -ViewHorizontalVelocity;
					}
			view.vpx += ViewHorizontalVelocity;	
			break;
		case UP_AND_DOWN:
			if (view.vpy + ViewVerticalVelocity < PLAYER_MIN_Y 
				|| view.vpy + ViewVerticalVelocity > PLAYER_MAX_Y)
					{
					ViewVerticalVelocity = -ViewVerticalVelocity;
					}
			view.vpy += ViewVerticalVelocity;
			break;
		case CIRCLING_CLOCKWISE:
			ViewAngle += ViewAngularSpeed;
			view.vpx = (RADIUS * rcos(ViewAngle)) >> 12;
			view.vpy = (RADIUS * rsin(ViewAngle)) >> 12;
			break;
		case CIRCLING_ANTICLOCKWISE:
			ViewAngle -= ViewAngularSpeed;
			view.vpx = (RADIUS * rcos(ViewAngle)) >> 12;
			view.vpy = (RADIUS * rsin(ViewAngle)) >> 12;
			break;
		default:
			assert(FALSE);
		}
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

	sprite->u = xOffset;
	sprite->v = yOffset;
}
	
   
	
	  
	  
	  
	  
	// delay until game resets itself				
#define FRAME_DELAY 90			


	// losing a life
void HandlePlayersDeath (void)
{
	ExplodeObject(&PlayersShip);

	PlayersLives--;
	PlayerInteractive = FALSE;

		// set up delayed actions
	if (PlayersLives == 0)
		{
		RegisterDelayedAction (RESET_GAME, FRAME_DELAY);
		PlayersShip.currentHealth = 1;
		}
	else
		{
		RegisterDelayedAction (TURN_PLAYER_DISPLAY_OFF, 15);
		RegisterDelayedAction (TURN_PLAYER_DISPLAY_ON, FRAME_DELAY);
		RegisterDelayedAction (RESET_AFTER_LOSE_LIFE, FRAME_DELAY);
		PlayersShip.currentHealth = 1;
		}
}



	// assume enemies never have lives, just health
void HandleEnemysDeath (ObjectHandler* object)
{
	PlayersShip.meritRating 
		+= FindPlayersRewardForKillingObject(object);

	ExplodeObject(object);
		
	object->alive = FALSE;
}




void HandlePlayerEndOfGame (void)
{
	sprintf(BasicTextStrings[0], "You have died\n");
	sprintf(BasicTextStrings[1], "Your score: %d\n", PlayersShip.meritRating);
	sprintf(BasicTextStrings[2], "Press start\nto play again\n");

		// display messages until start indicates a new game
	DummyGuiLoopForTextPrinting(3, PADstart);

	ResetEntireProgram();
}




void ResetEntireProgram (void)
{
	RestartAfterPlayerLosesALife();

	FlyingSequence();

	PlayersLives = INITIAL_NUMBER_OF_LIVES;
	LevelNumber = 1;
	frameNumber = 1;
}




void RestartAfterPlayerLosesALife (void)
{
	int i;

	PlayersShip.currentHealth = PlayersShip.initialHealth;
	PlayerInteractive = TRUE;

	PlayersShip.position.vx = 0;
	PlayersShip.position.vx = 0;
	PlayersShip.position.vy = 0;
	PlayersShip.coord.coord.t[0] = 0;
	PlayersShip.coord.coord.t[1] = 0;
	PlayersShip.coord.coord.t[2] = 0;
	PlayersShip.coord.flg = 0;

	for (i = 0; i < MAX_BULLETS; i++)
		{
		TheBullets[i].alive = FALSE;		// start off dead
		TheBullets[i].currentHealth = TheBullets[i].initialHealth;
		}
	for (i = 0; i < MAX_CUBES; i++)
		{
		TheCubes[i].alive = FALSE;		// start off dead
		TheCubes[i].currentHealth = TheCubes[i].initialHealth;
		}
	for (i = 0; i < MAX_SQUARES; i++)
		{
		TheSquares[i].alive = FALSE;		// start off dead
		TheSquares[i].currentHealth = TheSquares[i].initialHealth;
		}
	for (i = 0; i < MAX_TETRAHEDRONS; i++)
		{
		Tetrahedrons[i].alive = FALSE;		// start off dead
		Tetrahedrons[i].currentHealth = Tetrahedrons[i].initialHealth;
		}
	for (i = 0; i < MAX_TIE_FIGHTERS; i++)
		{
		TieFighters[i].alive = FALSE;		// start off dead
		TieFighters[i].currentHealth = TieFighters[i].initialHealth;
		}
	for (i = 0; i < MAX_SINGLE_ATTACKERS; i++)
		{
		SingleAttackers[i].alive = FALSE;		// start off dead
		SingleAttackers[i].currentHealth = SingleAttackers[i].initialHealth;
		}
	for (i = 0; i < MAX_INSECT_SHIPS; i++)
		{
		InsectShips[i].alive = FALSE;		// start off dead
		InsectShips[i].currentHealth = InsectShips[i].initialHealth;
		}

	TheRobot.alive = FALSE;

	CreateTheTunnel();

	RegisterDelayedAction (MAKE_PLAYER_INTERACTIVE_AGAIN, FRAME_DELAY);
}






	// for better design: put initial values and increment values
	// into a table, test-and-tweak
		
	// at present, the interdependence between this function and
	// InitialiseEnemyMovementGlobals is left *implicit* (Bad practice)

void HandleLevelUpdating (void)
{
	int i;
	int levelAboveOne = LevelNumber-1;
	short tempo;


	printf("level updating: factor %d\n", levelAboveOne);



		// overall enemy wave appearance rate
	EnemyWaveFrequency = 100 - (levelAboveOne * 25);
	KeepWithinRange(EnemyWaveFrequency, 50, 100);
	RobotFrequency = 1000 - (levelAboveOne * 100);
	KeepWithinRange(RobotFrequency, 500, 1000);


		// update specific enemy-type parameters
	TieFighterMovementSpeed = 8 + (levelAboveOne * 3);
	KeepWithinRange(TieFighterMovementSpeed, 5, 30);
	TieFighterAngularSpeed = ANGULAR_SPEED + (levelAboveOne * 6);
	KeepWithinRange(TieFighterAngularSpeed, ANGULAR_SPEED, 5 * ANGULAR_SPEED);
	NumberTieFightersPerWave = 12 + (levelAboveOne * 3);
	KeepWithinRange(NumberTieFightersPerWave, 12, MAX_TIE_FIGHTERS);


	SingleAttackerMovementSpeed = 7 + (levelAboveOne * 3);
	KeepWithinRange(SingleAttackerMovementSpeed, 7, 25);
	SingleAttackerAngularSpeed = ANGULAR_SPEED + (levelAboveOne * 6);
	KeepWithinRange(SingleAttackerAngularSpeed, ANGULAR_SPEED, 5 * ANGULAR_SPEED);
	SingleAttackerFiringRate = 20 - (levelAboveOne * 2);
	KeepWithinRange(SingleAttackerFiringRate, 8, 20);
	SingleAttackerShotSpeed = 6 + (levelAboveOne * 2);
	KeepWithinRange(SingleAttackerShotSpeed, 6, 20);


	InsectShipMovementSpeed = 20 + (levelAboveOne * 3);
	KeepWithinRange(InsectShipMovementSpeed, 20, 36);
	NumberInsectShipsPerWave = 24 + (levelAboveOne * 4);
	KeepWithinRange(NumberInsectShipsPerWave, 24, MAX_INSECT_SHIPS);


		// make the tunnel move faster
	TunnelSpeed = 2 + (levelAboveOne * 2);


#if 0		// no longer like this factor
		// make player fire slower
	PlayersShip.firingRate = 12 + levelAboveOne;
	KeepWithinRange(PlayersShip.firingRate, 12, 18);
#endif




		// view distortion speeds
	ViewBaseVelocity = 2 + levelAboveOne;
	KeepWithinRange(ViewBaseVelocity, 2, 16);
	ViewAngularSpeed = (ONE/128) + (levelAboveOne * (ONE/512));
	KeepWithinRange(ViewAngularSpeed, ONE/128, ONE/8);



	SpecialObjectLikelihood = 250 - (levelAboveOne * 25);
	KeepWithinRange(SpecialObjectLikelihood, 100, 250);
}






	// collisions
	// one MAJOR way to speed up collision detection: it will often be the case
	// that only certain kinds of object are up for collision, especially
	// in a 'wave after wave' shoot-em-up: hence you can restrict the detection
	// to only two or three classes of objects at one time	
void CheckCollisions (void)
{
	ObjectHandler* object;
	int i, j;

	if (PlayerInteractive != TRUE)
		return;

	for (i = 0; i < MAX_BULLETS; i++)
		{
		if (TheBullets[i].alive == TRUE && TheBullets[i].allegiance != PLAYER)
			{
			if (ObjectsVeryClose(&TheBullets[i], &PlayersShip))
				{
				//printf("Player and bullet collision\n");
				PlayersShip.currentHealth--;
				TheBullets[i].alive = FALSE;
				TurnOnSound (4, 0, 60, 1, 20, 96, 96);
				}
			}
		} 

	for (i = 0; i < MAX_TIE_FIGHTERS; i++)
		{
		if (TieFighters[i].alive == TRUE)
			{
			if (ObjectsVeryClose(&TieFighters[i], &PlayersShip))
				{
				//printf("Player and fighter %d collision\n", i);
				PlayersShip.currentHealth--;
				TieFighters[i].currentHealth--;	
				}
			for (j = 0; j < MAX_BULLETS; j++)
				{
				if (TheBullets[j].alive == TRUE && TheBullets[j].allegiance == PLAYER)
					{
					if (ObjectsVeryClose(&TieFighters[i], &TheBullets[j]))
						{
						//printf("bullet and fighter %d collision\n", i);
						TieFighters[i].currentHealth--;
						TheBullets[j].alive = FALSE;
						}
					}
				}
			}
		}
		
	for (i = 0; i < MAX_SINGLE_ATTACKERS; i++)
		{
		if (SingleAttackers[i].alive == TRUE)
			{
			for (j = 0; j < MAX_BULLETS; j++)
				{
				if (TheBullets[j].alive == TRUE && TheBullets[j].allegiance == PLAYER)
					{
					if (ObjectsVeryClose(&SingleAttackers[i], &TheBullets[j]))
						{
						//printf("bullet and single attacker %d collision\n", i);
						SingleAttackers[i].currentHealth--;
						TheBullets[j].alive = FALSE;
						TurnOnSound (4, 0, 60, 1, 20, 96, 96);
						}
					}
				}
			}
		}
		
	for (i = 0; i < MAX_INSECT_SHIPS; i++)
		{
		if (InsectShips[i].alive == TRUE)
			{
			if (ObjectsVeryClose(&InsectShips[i], &PlayersShip))
				{
				//printf("Player and insect ship %d collision\n", i);
				PlayersShip.currentHealth--;
				InsectShips[i].currentHealth--;
				}
			for (j = 0; j < MAX_BULLETS; j++)
				{
				if (TheBullets[j].alive == TRUE && TheBullets[j].allegiance == PLAYER)
					{
					if (ObjectsVeryClose(&InsectShips[i], &TheBullets[j]))
						{
						//printf("bullet and insect %d collision\n", i);
						InsectShips[i].currentHealth--;
						TheBullets[j].alive = FALSE;
						}
					}
				}
			}
		} 

	for (i = 0; i < MAX_SPECIAL_OBJECTS; i++)
		{
		if (SpecialObjects[i].alive == TRUE && SpecialObjects[i].actionState != HOLD_STILL)
			{
			if (ObjectsVeryClose(&SpecialObjects[i], &PlayersShip))
				{
				//printf("Player and bonus type %d collision\n", SpecialObjects[i].subType);
				DealWithSpecialObjectAndPlayer( &SpecialObjects[i]);
				//SpecialObjects[i].alive = FALSE;

					// for the next second, we will display the sprite at top-left of screen
				SpecialObjects[i].actionState = HOLD_STILL;
				SpecialObjects[i].sprite.rotate = 0;
				SpecialObjects[i].scaleX = 2 * ONE;
				SpecialObjects[i].scaleY = 2 * ONE;
				setVECTOR (&SpecialObjects[i].position, 
					-SCREEN_WIDTH/4, -SCREEN_HEIGHT/4, 0);
				RegisterDelayedAction(KILL_ALL_SPECIAL_OBJECTS, 60);
				}
			}
		}

	if (TheRobot.alive == TRUE)
		{
		if (ObjectsVeryClose(&TheRobot, &PlayersShip))
			{
			//printf("Player and robot %d collision\n", i);
			PlayersShip.currentHealth = 0;
			TheRobot.currentHealth--;	
			}
		for (j = 0; j < MAX_BULLETS; j++)
			{
			if (TheBullets[j].alive == TRUE && TheBullets[j].allegiance == PLAYER)
				{
				if (ObjectsVeryClose(&TheRobot, &TheBullets[j]))
					{
					//printf("bullet and robot collision\n");
					TheRobot.currentHealth--;
					TheBullets[j].alive = FALSE;
					TurnOnSound (4, 0, 60, 1, 20, 96, 96);
					}
				}
			}
		}
}


	  



#define OBJECT_SIZE 32	    // very rough and ready
							// easiest way to improve this:
							// give the object class a vector expressing its
							// width, height and depth; hence the clipping cuboids
							// can be made specific to object type, and the overall
							// performance of this cuboid-collision-detection 
							// will be far better

int ObjectsVeryClose (ObjectHandler* first, ObjectHandler* second)
{
	if (abs(first->coord.coord.t[0] - second->coord.coord.t[0]) > OBJECT_SIZE)
		return FALSE;
	if (abs(first->coord.coord.t[1] - second->coord.coord.t[1]) > OBJECT_SIZE)
		return FALSE;
	if (abs(first->coord.coord.t[2] - second->coord.coord.t[2]) > OBJECT_SIZE)
		return FALSE;

	return TRUE; 
}




		 


void ExplodeObject (ObjectHandler* object)
{
	int cubeID;
	int whichAxis, sign;
	int i, j;
	int whichProgram, whichNote;

	whichProgram = 2 + (rand() % 2);
	whichNote = 60 + (rand() % 13);
	TurnOnSound (whichProgram, 0, whichNote, 1, 30, 64, 64);

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
				// cubes emitted, one per local object axis

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
			FntPrint(BasicTextStrings[i]);
			}
		
		FntFlush(-1);
		}	
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


   	


void NormaliseVector (VECTOR* vector)
{
	float magnitude;

	magnitude = (vector->vx * vector->vx)
				+ (vector->vy * vector->vy)
				+ (vector->vz * vector->vz);
	magnitude = pow(magnitude, 0.5);

	assert(magnitude != 0);

	vector->vx = vector->vx * ONE / magnitude;
	vector->vy = vector->vy * ONE / magnitude;
	vector->vz = vector->vz * ONE / magnitude;
		
#if 0
	if (frameNumber % 100 == 0)
		{
		magnitude = (vector->vx * vector->vx)
				+ (vector->vy * vector->vy)
				+ (vector->vz * vector->vz);
		magnitude = pow(magnitude, 0.5);
		//printf("new magnitude is %d\n", magnitude);
		dumpVECTOR(vector);
		}
#endif
}






void PrecompileCircularPositions (void)
{
	int i, angle;
	
	NumberPositions = ONE / ANGULAR_SPEED;
	
	for (i = 0; i < NumberPositions; i++)
		{
		angle = i * ANGULAR_SPEED;
		CircularXs[i] = (RADIUS * rcos(angle)) >> 12;
		CircularYs[i] = (RADIUS * rsin(angle)) >> 12; 
		} 
}
	   	






#define FOG_GENTLE 0
#define FOG_INCREMENT 1

void FlyingSequence (void)
{
	int	i;
	int	hsync = 0;
	GsDOBJ2 *op;
	int side;			  // buffer index
	MATRIX	tmpls, tmplw;
	ObjectHandler** pointer;
	ObjectHandler* object;
	int numberToDisplay;
	long pad;
	int localFrameCounter = 0, totalFrames = 180;
	int originalTunnelSpeed = TunnelSpeed;
	int time;
	u_short zValue;

	side = GsGetActiveBuff();

	lightmode = 0;		// turn fog off
	GsSetLightMode(lightmode);

	numberToDisplay = PerformBogusWorldClipping(ObjectArray, DisplayedObjects);

	while(1)
		{
		if (localFrameCounter == totalFrames)
			break;

		localFrameCounter++;

		pad = PadRead(0);

			// quit this stage
		if (pad & PADstart && pad & PADselect)
			break;		
		
		if (localFrameCounter < 60)
			{
			time = localFrameCounter;
			ProjectionDistance = 192 - (140 * time / 60);
			GsSetProjection(ProjectionDistance);
			TunnelSpeed = INITIAL_TUNNEL_SPEED + (time * 18 / 60);
			view.vpz = -220 + (time * 50 / 60);
			#if FOG_GENTLE
				Fogging.dqa = -1960 + (time * 1000 / 60);
				GsSetFogParam(&Fogging);
			#endif
			}
		else if (localFrameCounter == totalFrames - 60)
			{
			#if FOG_GENTLE
				lightmode = 1;
				GsSetLightMode(lightmode);
			#endif
			#if FOG_INCREMENT
				lightmode = 1;
				GsSetLightMode(lightmode);
				Fogging.dqa = -3960;  // -1960
				GsSetFogParam(&Fogging);
			#endif
			}
		else if (localFrameCounter > totalFrames - 60)
			{
			time = localFrameCounter - (totalFrames - 60);
			ProjectionDistance = 52 + (140 * time / 60);
			GsSetProjection(ProjectionDistance);
			TunnelSpeed = INITIAL_TUNNEL_SPEED + ( (60-time) * 18 / 60);
			view.vpz = -170 - (time * 50 / 60);
			#if FOG_GENTLE
				Fogging.dqa = -1960 + (time * 1000 / 60);
				GsSetFogParam(&Fogging);
			#endif
			#if FOG_INCREMENT
				Fogging.dqa = -3960 + (time * 3000 / 60);
				GsSetFogParam(&Fogging);
			#endif
			}	

		GsSetWorkBase((PACKET*)packetArea[side]);

		GsClearOt(0,0,&Wot[side]);
		
		UpdateTheTunnel();

		GsSetRefView2(&view);	 

		/***
		FntPrint("num to display: %d\n", numberToDisplay);
		FntPrint("vx %d, proj %d\n", view.vpz, ProjectionDistance);
		FntPrint("local frame: %d\n", localFrameCounter);
		***/


	
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

		hsync = VSync(0);		 

		ResetGraph(1);
		GsSwapDispBuff();
		GsSortClear(0,0,4,&Wot[side]);
		GsDrawOt(&Wot[side]);
		side ^= 1;
		FntFlush(-1);
		}	


		// restore fogging
	lightmode = 1;		// turn fog off
	GsSetLightMode(lightmode);

		// restore tunnel speed
	TunnelSpeed = originalTunnelSpeed;
}




	// should just directly use ObjectArray rather than calling this dummy clipper ...

int PerformBogusWorldClipping (ObjectHandler** firstArray, ObjectHandler** secondArray)
{
	int numberDisplayedObjects = 0;
	int i;
	ObjectHandler *object, **pointer;
	
	for (i = 0; i < MAX_OBJECTS; i++)
		{
		if (firstArray[i] != NULL)
			if (firstArray[i]->alive == TRUE)
				{
				secondArray[numberDisplayedObjects] = firstArray[i];
				numberDisplayedObjects++;
				}
		}
		
	return numberDisplayedObjects;	
}		





static int jasonTuneVoiceID;

void InitialiseSound (void)
{
	int i;

		// transfer VAB data
	vab0 = SsVabTransfer((u_char *)VH0_ADDRESS, (u_char *)VB0_ADDRESS, -1, 1);  
	if (vab0 < 0) 
		{
		printf("VAB open failed\n");
		exit(1);
		}

	SsSetMVol(127, 127);

		// background tune as VAB noise rather than using an SEQ:
		// just turn it on and never touch again
	jasonTuneVoiceID = SsUtKeyOn(vab0, 0, 0, 60, 0, 64, 64);



	for (i = 0; i < MAX_VOICES; i++)
		{
		TheVoices[i].value = 0;
		TheVoices[i].statusFlag = OFF;
		TheVoices[i].frameToStartOn = -1;
		TheVoices[i].duration = -1;
		TheVoices[i].program = -1;
		TheVoices[i].tone = -1;
		TheVoices[i].note = 60;		// default 60 (seems very common ... )
		TheVoices[i].leftVolume = 127;			  // default: max volume levels
		TheVoices[i].rightVolume = 127;
		}
}

	   


void HandleSound (void)
{
	int i;
	VoiceChannelHandler* data;

	for (i = 0; i < MAX_VOICES; i++)
		{
		data = &TheVoices[i];

		switch(data->statusFlag)
			{
			case OFF:
				break;
			case WAITING_TO_START:
				if (frameNumber == data->frameToStartOn)
					{
						// turn on
					data->value = SsUtKeyOn(vab0, data->program, data->tone, 
										data->note, 0, data->leftVolume, data->rightVolume);
					data->statusFlag = PLAYING;
					}	
				break;
			case PLAYING:
				if (frameNumber == data->frameToStartOn + data->duration)
					{
						// turn off
					SsUtKeyOff(data->value, vab0, data->program, data->tone, data->note);
					data->statusFlag = OFF;
					}
				break;
			default:
				assert(FALSE);
			}
		}
}

				 

void TurnOnSound (int whichProgram, int whichTone, int whichNote, int delayFromNow, int howLong,
									int leftVolume, int rightVolume)
{
#if SOUND_ON
	int soundIndex = -1;			// index into array TheVoices
	int i;
	VoiceChannelHandler* data;

	assert(whichProgram >= 0);
	assert(whichTone >= 0);

	for (i = 0; i < MAX_VOICES; i++)
		{
		if (TheVoices[i].statusFlag == OFF)	   // a free one
			{
			soundIndex = i;
			break;
			}
		}

	if (soundIndex == -1)
		{
		printf("no free sound voices\n");
		return;
		}

	data = &TheVoices[soundIndex];

	data->statusFlag = WAITING_TO_START;
	data->program = whichProgram;
	data->tone = whichTone;
	data->note = whichNote;
	data->frameToStartOn = frameNumber + delayFromNow;
	data->duration = howLong;
	data->leftVolume = leftVolume;
	data->rightVolume = rightVolume;
#endif
}


		


void CleanUpSound (void)
{	 
		// turn off background tune
	SsUtKeyOff(jasonTuneVoiceID, vab0, 0, 0, 60);

		// close entire VAB file
	SsVabClose(vab0);
}






int FindPlayersRewardForKillingObject (ObjectHandler* object)
{
	int reward;

	switch(object->type)
		{
		case TIE_FIGHTER:
			reward = 10;
			break;
		case SINGLE_ATTACKER:
			reward = 25;
			break;
		case INSECT_SHIP:
			reward = 5;
			break;
		case ROBOT:
			reward = 100;
			break;
		default:			// only these three types should ever get killed
			assert(FALSE);
		}

		// compensate for Level
	reward *= LevelNumber;

	return reward;
}






void InitialiseDelayedActionList (void)
{
	int i;

	for (i = 0; i < MAX_DELAYED_ACTIONS; i++)
		{
		ActionList[i].status = INERT;
		ActionList[i].frameToStart = -1;
		ActionList[i].typeFlag = -1;
		}
}




void HandleDelayedActionList (void)
{
	int i;

	for (i = 0; i < MAX_DELAYED_ACTIONS; i++)
		{
		if (ActionList[i].status == ACTIVELY_WAITING)
			{
			if (frameNumber == ActionList[i].frameToStart)
				{
				PerformDelayedAction( &ActionList[i]);
				ActionList[i].status = INERT;
				ActionList[i].frameToStart = -1;
				ActionList[i].typeFlag = -1;
				}
			}
		}
}




	// note: can avoid this switching by having the DelayedAction structure
	// store a pointer to a function and directly invoke it

void PerformDelayedAction (DelayedAction* action)
{
	switch(action->typeFlag)
		{
		case QUIT_PROGRAM:
				// program cleanup
			ResetGraph(3);

			#if SOUND_ON
				CleanUpSound();
			#endif
			exit(1);
			break;
		case RESET_GAME:
			HandlePlayerEndOfGame();
			break;
		case RESET_AFTER_LOSE_LIFE:
			RestartAfterPlayerLosesALife();
			break;
		case MAKE_PLAYER_INTERACTIVE_AGAIN:
			PlayerInteractive = TRUE;
			break;
		case TURN_PLAYER_DISPLAY_OFF:
			PlayersShip.handler.attribute |= GsDOFF;
			break;
		case TURN_PLAYER_DISPLAY_ON:
			TURN_NTH_BIT_OFF(PlayersShip.handler.attribute, 32, 31)
			break;	
		case REVERT_PLAYER_FIRING_RATE:
			RevertPlayerFiringRate();
			break;
		case REVERT_PLAYER_SHOT_SPEED:
			RevertPlayerShotSpeed();
			break;
		case REVERT_PLAYER_MOVEMENT_SPEED:
			RevertPlayerMovementSpeed();
			break;  
		case STOP_LIGHT_OSCILLATION:
			StopLightOscillation();
			break;
		case STOP_VIEW_OSCILLATION:
			StopViewOscillation();
			break;
		case KILL_ALL_SPECIAL_OBJECTS:
			KillAllSpecialObjects();
			break;
		default:
			assert(FALSE);
		}
}

  


void RegisterDelayedAction (int type, int delayFromNow)
{
	int actionID = -1;
	int i;

	for (i = 0; i < MAX_DELAYED_ACTIONS; i++)
		{
		if (ActionList[i].status == INERT)
			{
			actionID = i;
			break;
			}
		}

	assert(actionID != -1);

	ActionList[actionID].status = ACTIVELY_WAITING;
	ActionList[actionID].typeFlag = type;
	ActionList[actionID].frameToStart = frameNumber + delayFromNow;
}







	// NOTE: should play a brief sound, different sound per subType

void DealWithSpecialObjectAndPlayer (ObjectHandler* special)
{
	switch(special->subType)
		{
		case EXTRA_LIFE:
			PlayersLives++;
			break;
		case FIRE_QUICKER:
			PlayersShip.firingRate -= 3;
			RegisterDelayedAction(REVERT_PLAYER_FIRING_RATE, EFFECT_DURATION);
			break;
		case SHOTS_MOVE_FASTER:
			PlayersShip.shotSpeed += 40;
			RegisterDelayedAction(REVERT_PLAYER_SHOT_SPEED, EFFECT_DURATION);
			break;
		case SHIP_MOVES_FASTER:
			PlayersShip.movementSpeed += 3;
			RegisterDelayedAction(REVERT_PLAYER_MOVEMENT_SPEED, EFFECT_DURATION);
			break;
		case OSCILLATE_LIGHTS:
			LightOscillationFlag = TRUE;
			RegisterDelayedAction(STOP_LIGHT_OSCILLATION, EFFECT_DURATION);
			break;
		case OSCILLATE_VIEW:
			ViewOscillationFlag = TRUE;
			ViewOscillationJustBegun = TRUE;
			ViewDistortionMode = rand() % 4;
			RegisterDelayedAction(STOP_VIEW_OSCILLATION, EFFECT_DURATION);
			break;
		default:
			assert(FALSE);
		}
}

   


	// these three: revert player's ship attributes to normal/default 
void RevertPlayerFiringRate (void)
{
	PlayersShip.firingRate = 12;
}

void RevertPlayerShotSpeed (void)
{
	PlayersShip.shotSpeed = 75;
}

void RevertPlayerMovementSpeed (void)
{
	PlayersShip.movementSpeed = 5;
}




void StopLightOscillation (void)
{
	LightOscillationFlag = FALSE;

		// restore flat lights to previous states
	SortTheFlatLights();
}




void StopViewOscillation (void)
{
	ViewOscillationFlag = FALSE;
	ViewOscillationJustBegun = FALSE;
	OldViewDistortionMode = ViewDistortionMode;
	ViewDistortionMode = -1;		// 'none'

	view.vpx = ViewpointXDistance; 
	view.vpy = ViewpointYDistance; 
	view.vpz = ViewpointZDistance;
	
	view.rz = 0;
	view.super = WORLD;

	GsSetRefView2(&view);
}





void KillAllSpecialObjects (void)
{
	int i;

	for (i = 0; i < MAX_SPECIAL_OBJECTS; i++)
		{
		SpecialObjects[i].alive = FALSE;
		SpecialObjects[i].actionState = -1;
		SpecialObjects[i].scaleX = ONE;
		SpecialObjects[i].scaleY = ONE;
		}
}





void InitialiseTextStrings (void)
{
	int i;
	
	for (i = 0; i < 26; i++)
		{
		LinkSpriteToImageInfo( &LetterSprites[i], &AsciiTextureInfo);
		LetterSprites[i].w = LetterSprites[i].h = 8;
		if (i < 15)
			{
			LetterSprites[i].u += 8 + (i * 8);
			LetterSprites[i].v += 16;
			}
		else
			{
			LetterSprites[i].u += ((i-15) * 8);
			LetterSprites[i].v += 24;
			}
		}
	for (i = 0; i < 10; i++)
		{
		LinkSpriteToImageInfo( &NumberSprites[i], &AsciiTextureInfo);
		NumberSprites[i].w = NumberSprites[i].h = 8;
		NumberSprites[i].u += (i * 8);
		NumberSprites[i].v += 8;
		}

	for (i = 0; i < MAX_TEXT_STRINGS; i++)
		{
		AllStrings[i].x = 0;
		AllStrings[i].y = 0;
		AllStrings[i].length = 0;
		AllStrings[i].data[0] = '\n';
		}

	NumberOfStrings = 0;
}





void DisplayTextStrings (GsOT* orderingTable)
{
	int i, j;
	TextString* thisOne;
	GsSPRITE* sprite;
	int x, y;

	for (i = 0; i < MAX_TEXT_STRINGS; i++)
		{
		if (AllStrings[i].length != 0)
			{
			thisOne = &AllStrings[i];
			x = thisOne->x;
			y = thisOne->y;

			for (j = 0; j < thisOne->length; j++)
				{
				sprite = GetSpriteForCharacter(thisOne->data[j]);

				if (sprite != NULL)
					{
					sprite->x = x;
					sprite->y = y;

					GsSortSprite(sprite, orderingTable, 0); 
					}
				x += 8;
				}

				// mark string as EMPTY
			thisOne->length = 0;
			}
		}

	NumberOfStrings = 0;
}






GsSPRITE* GetSpriteForCharacter (char character)
{
	GsSPRITE* result;
	int index;

	if ( ((character >= 48 && character <= 57)
		|| (character >= 65 && character <= 90) 
		|| (character >= 97 && character <= 122)) == FALSE)
			return NULL;

		// make all one case
	if (character >= 65 && character <= 90)
		character += 32;

	if (character >= 97 && character <= 122)
		{
		index = ((int) character) - 97;
		result = &LetterSprites[index];
		}
	else
		{
		index = ((int) character) - 48;
		result = &NumberSprites[index];
		}

	return result;
}





void RegisterTextStringForDisplay (char* string, int x, int y)
{
	int i, id = -1;
	TextString* thisOne;

	for (i = 0; i < MAX_TEXT_STRINGS; i++)
		{
		if (AllStrings[i].length == 0)
			{
			id = i;
			break;
			}
		}

	assert(id != -1);
	thisOne = &AllStrings[id];

	thisOne->x = x;
	thisOne->y = y;
	thisOne->length = strlen(string);
	assert(thisOne->length > 0);
	assert(thisOne->length < MAX_STRING_LENGTH);

	strcpy( thisOne->data, string);

	NumberOfStrings++;
	assert(NumberOfStrings < MAX_TEXT_STRINGS);
}







	// this CD-file-access code taken straight from other samples
void DatafileSearch (void)
{
	int i, j;

	for (i = 0; i < NUMBER_FILES; i++) 
		{	   
		for (j = 0; j < 10; j++) 
			{	
			if (CdSearchFile(&(DataFiles[i].finfo), 
						DataFiles[i].fname) != 0)
				break;		
			else
				printf("%s not found.\n", DataFiles[i].fname);
			}
		}
}




   
void DatafileRead (void)
{
	int i, j;
	int cnt;

	for (i = 0; i < NUMBER_FILES; i++) 
		{		
		for (j = 0; j < 10; j++) 
			{	
			CdReadFile(DataFiles[i].fname, DataFiles[i].addr, 
									DataFiles[i].finfo.size);

			while ((cnt = CdReadSync(1, 0)) > 0 )
				VSync(0);	

			if (cnt == 0)
				break;	
			}
		}
}



	
		 