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


#include "pad.h"

#include "2d1.h"

#include "rand.h"

#include "text_str.h"

#include "address.h"

#include "sound.h"

#include "tmd.h"

#include "object.h"

#include "geom.h"

#include "sincos.h"

#include "breakout.h"








/****************************************************************************
					structures, constants, globals
****************************************************************************/



GsIMAGE FireTextureInfo;
GsSPRITE FireSprite;

GsIMAGE MascotsTextureInfo;
GsSPRITE MascotsSprite;

GsIMAGE WaveTextureInfo;
GsSPRITE WaveSprite;

GsIMAGE MassivePictureTextureInfo;
GsSPRITE MassivePictureSprite;

GsIMAGE MassivePicture2TextureInfo;
GsSPRITE MassivePicture2Sprite;




	// not really variable for this program
#define ScreenWidth 320		 
#define ScreenHeight 240



#define PACKETMAX	2048		/* Max GPU packets */
#define PACKETMAX2	(PACKETMAX*24)

PACKET packetArea[2][PACKETMAX2]; /* GPU PACKETS AREA */

PACKET packetArea2[2][PACKETMAX*24];		// another packet area
PACKET packetArea3[2][PACKETMAX];
PACKET packetArea4[2][PACKETMAX];
PACKET packetArea5[2][PACKETMAX];
PACKET packetArea6[2][PACKETMAX];
PACKET packetArea7[2][PACKETMAX];

  

#define FIRST_OT_LENGTH	9		

GsOT Wot[2];			/* Handler of OT */
GsOT_TAG wtags[2][1<<FIRST_OT_LENGTH];


#define SECOND_OT_LENGTH 4

GsOT Wot2[2];			/* Handler of secondOT */
GsOT_TAG wtags2[2][1<<SECOND_OT_LENGTH];

GsOT Wot3[2];			
GsOT_TAG wtags3[2][1<<SECOND_OT_LENGTH];

GsOT Wot4[2];			
GsOT_TAG wtags4[2][1<<SECOND_OT_LENGTH];

GsOT Wot5[2];			
GsOT_TAG wtags5[2][1<<SECOND_OT_LENGTH];

GsOT Wot6[2];			
GsOT_TAG wtags6[2][1<<SECOND_OT_LENGTH];

GsOT Wot7[2];			
GsOT_TAG wtags7[2][1<<SECOND_OT_LENGTH];


 



long ProjectionDistance;

	// view moves
#define VIEW_TWIST_LEFT 1
#define VIEW_TWIST_RIGHT 2
#define VIEW_TWIST_UP 3
#define VIEW_TWIST_DOWN 4
#define VIEW_TWIST_CLOCKWISE 5
#define VIEW_TWIST_ANTICLOCKWISE 6

#define VIEW_X_MOVE 7
#define VIEW_Y_MOVE 8
#define VIEW_Z_MOVE 9

#define TOGGLE_CIRCLING_DIRECTION 10
#define CHANGE_CIRCLING_PLANE_UP 11
#define CHANGE_CIRCLING_PLANE_DOWN 12


GsRVIEW2 TheView;
GsCOORDINATE2 ViewCoords;
GsCOORDINATE2 PreviousViewCoords;
int ViewMoveFlag;
int ViewMoveTime;		// in frames
int ViewMoveStartFrame;
int ViewDistanceFromOrigin;
int ViewCorner;		// only for FOURTH_VIEW_MODE
int ViewCirclePlane, ViewCircleDirection, ViewCircleAngle;		// for FIFTH_VIEW_MODE

#define FIRST_VIEW_MODE 12
#define SECOND_VIEW_MODE 13
#define THIRD_VIEW_MODE 14
#define FOURTH_VIEW_MODE 15
#define FIFTH_VIEW_MODE 16



int BasicViewMode;


#define AUTO_CAMERA 0
#define CONTROLLER_PAD_CAMERA 1
int CameraControlFlag;


int MutaterState;


#define FIXED_IN_FIRST 24
#define FIXED_IN_SECOND 25
#define FIXED_IN_FOURTH 26
#define FIXED_IN_FIFTH 27
#define MUTATE_ON_FOUR 28
#define MUTATE_ON_THREE 29

int AutoCameraMode;



#define MOVE_IN 67
#define MOVE_OUT 68

int DistanceOscillationFrequency, DistanceOscillationFlag;
int DistanceOscillationStartFrame, DistanceOscillationDuration;
int DistanceMovementSpeed;






int AutoCameraMoveFrequency;








GsF_LIGHT TheLights[3];

GsFOGPARAM TheFogging;

int OverallLightMode;






int QuitFlag = FALSE;

int frameNumber = 0;




	  







	// object types
#define CUBE 0
#define POLYGON 1




#define MAX_CUBES 12
ObjectHandler TheCubes[MAX_CUBES];

#define MAX_POLYGONS 1024
MiniObject ThePolygons[MAX_POLYGONS];




   



	// textured inside or out
#define VISIBLE_FROM_INSIDE 1
#define VISIBLE_FROM_OUTSIDE	2


	// ways to texture the polygons
#define SINGLE_CUBE_TEXTURE 1			   // 1
#define ONE_TEXTURE_PER_FACE 2			   // 6
#define ONE_TEXTURE_PER_POLYGON 3		   // n.polys
#define ONE_TEXTURE_PER_MAIN_AXIS 4		   // 3
#define ONE_TEXTURE_PER_CORNER 5		   // 2





#define MAX_POLYS_PER_SIDE 6
#define MAX_POLYGONS_PER_CUBE (MAX_POLYS_PER_SIDE * MAX_POLYS_PER_SIDE * 6)

typedef struct
{
	int id;
	int alive;
	int sideLength;
	int sideFlag;
	int polysPerSide;
	int textureType;
	int textureChopFlag;

	RECT textureAreas[MAX_POLYGONS_PER_CUBE];		// 16 bit textures
	MiniObject *polygons[MAX_POLYGONS_PER_CUBE];
	VECTOR normals[6];

	GsCOORDINATE2 coord;
} Cube;




#define INSIDE_CUBE 1
#define OUTSIDE_CUBE 2

Cube InsideCube;
Cube OutsideCube;


	// all cube textures are 16 bit so that they
	// can be drawn onto
#define CUBE_TEXTURES_PIXEL_MODE 16






	// face geometry

	// which can be got to from which others
int FaceAccessTable[6][4];

VECTOR FaceInsideNormals[6];
VECTOR FaceOutsideNormals[6];

	// x and y vector for each plane, to treat it as standard Cartesian 2d space
	// note that Y goes up and X goes right
VECTOR FaceXVectors[6];
VECTOR FaceYVectors[6];


VECTOR CubeCorners[8];




	// dynamic TMD management
#define SIZEOF_SINGLE_POLYGON_TMD 0x00000100


	// INTERACTION WITH ADDRESS.H AND BATCH FILE
#define START_OF_CREATED_TMDS_STACK 0x80110000
#define END_OF_CREATED_TMDS_STACK 0x80130000

int NumberOfCreatedTMDs;

u_long CurrentTMDStackAddress;




#define MAX_DRAW_PROCESSES 12

typedef struct
{
	int id;
	int alive;

	RECT clipArea;
	u_short offsets[2];

	int otLabel; 
	int packetAreaLabel;
} DrawProcess;

DrawProcess *AllDrawProcesses[MAX_DRAW_PROCESSES];

	// draw processes ids
#define FIRST 1
#define SECOND 2
#define THIRD 3
#define FOURTH 4
#define FIFTH 5
#define SIXTH 6

DrawProcess FirstDrawProcess;
DrawProcess SecondDrawProcess;
DrawProcess ThirdDrawProcess;
DrawProcess FourthDrawProcess;
DrawProcess FifthDrawProcess;
DrawProcess SixthDrawProcess;




int ProgramQuitFlag = FALSE;

#define MODE_MENU_SCREEN 1
#define MODE_MAIN_GAME 2

int TopLevelGameState = MODE_MENU_SCREEN;


int FramesInNewState = 0;

#define CHOOSE_GRAPHICS_FLAG 0
#define CHOOSE_NUMBER_PLAYERS 1
#define CHOOSE_GAME_DIFFICULTY 2
#define CHOOSE_GAME_DEMO 3
#define CHOOSE_START_GAME 4


#define TORTOISE 0
#define WILDERBEAST 1
#define GREYHOUND 2
#define CHEETAH 3

#define PLAY_ON_INNER_SIMPLE_OUTER_TEXTURE 1
#define PLAY_ON_INNER_FLASH_OUTER_TEXTURE 2
#define PLAY_ON_OUTER_ONLY 3
#define PLAY_ON_BOTH_SURFACES 4

int BackgroundGraphicsFlag = PLAY_ON_INNER_SIMPLE_OUTER_TEXTURE;
int OverallCameraType = FIXED_IN_FIRST;
int OverallCameraSpeedFlag = TORTOISE;
int MenuScreenOption = CHOOSE_GRAPHICS_FLAG;



#define CHICKEN 0
#define WALLABY 1
#define COBRA 2
#define DRAGON 3

int OverallGameDifficulty = CHICKEN;




int	hsync = 0;
int maxHsync = 0;
int average = 0;
int hsyncRecord[60];
int gpuLoad = 0, cpuLoad = 0;
int bufferIndex;

int FrameWhenDemoStarts;

	// duration when uninterrupted
#define ROLLING_DEMO_DURATION (60 * 24)


	// pause before can act in new mode
#define NEW_MODE_PAUSE 45




#define INITIAL_GRAPHICS_FLAG 1

#define OPENING_SEQUENCE_FLAG 1



int GameHasFinishedAndResultsNeedDisplayingFlag = FALSE; 



/****************************************************************************
					prototypes
****************************************************************************/


void main (void);


void HandleGameState (void);
void ChangeGameState (int newState);

void SetMenuScreenDefaultSettings (void);

void HandleSimpleMenuScreen (void);

void DealWithMenuControls (void);

void HandleMainGameLoop (void);



void InitialiseAll (void);

void SortVideoMode (void);

void InitialiseLighting (void);
void InitialiseView (void);

void InitialiseObjects (void);

void InitialiseFaceGeometry (void);

void CreateMoreLittleCubesAroundLargerOne (void);

void InitialiseCubes (void);
void CreateTheInnerCube	(void);
void CreateTheOuterCube (void);



void CleanupProgram (void);



void CycleLightsAroundAxes (void);




void DealWithControllerPad (void);

void DealWithViewControls (long pad, int controlSpeed);




void HandleAllObjects (void);

void HandleACube (ObjectHandler* object);


void UpdateObjectCoordinates (VECTOR* twist,
							VECTOR* position, VECTOR* velocity,
							GsCOORDINATE2* coordSystem, MATRIX* matrix);

void UpdateObjectCoordinates2 (SVECTOR* rotationVector,
							VECTOR* translationVector,
							GsCOORDINATE2* coordSystem);


void RotateCoordinateSystem (VECTOR* twist,
							GsCOORDINATE2* coordSystem);


void CopyCoordinateSystem (GsCOORDINATE2 *from, GsCOORDINATE2 *to);

void CopyMatrix (MATRIX *from, MATRIX *to);

void ExpressSubPointInSuper (VECTOR* superPoint, 
			GsCOORDINATE2* subSystem, VECTOR* subPointOutput);

void ExpressSuperPointInSub (VECTOR* subPoint, 
			GsCOORDINATE2* subSystem, VECTOR* superPointOutput);

u_short SortSpriteObjectPosition (ObjectHandler* object);




void StoreScreen (void);



void InitCube (Cube *cube);

void CreateCube (Cube *cube, int id, int sideLength, int sideFlag, 
					int polysPerSide, int type, int chopFlag, RECT *rectList);

void SortCubeTextureAreas (Cube *cube, int type, RECT *rectList);

void CreateTheCubesPolygons (Cube *cube);

void SortRectGivenPolygonsPerSide (RECT *rect, int numPolysPerSide, int x, int y);

void SortVertices (Cube *cube, int whichFace, int gridX, int gridY,
		TMD_VERT *v1, TMD_VERT *v2, TMD_VERT *v3, TMD_VERT *v4);

void DrawCube (Cube *cube, GsOT *ot);





MiniObject *GetNextFreePolygon (void);

u_long CreateAnotherPolygonTMD (int clutX, int clutY, 
					int tPageID, int pixelMode, 
					u_char u0, u_char v0,
					u_char u1, u_char v1, 
					u_char u2, u_char v2, 
					u_char u3, u_char v3,
					TMD_NORM* norm0, 
					TMD_VERT* vert0, 
					TMD_VERT* vert1, 
					TMD_VERT* vert2, 
					TMD_VERT* vert3);

void ClearTheCreatedTmds (void);

void PrintCubeInfo (Cube *cube);



void StartFirstModeViewMove (int viewMove);
void StartSecondModeViewMove (int viewMove);
void StartFourthModeViewMove (int viewMove);
void MakeFifthModeViewMove (int modeMove);

void HandleTheView (void);

void HandleTheFirstViewMode (void);
void HandleTheSecondViewMode (void);
void HandleTheThirdViewMode (void);
void HandleTheFourthViewMode (void);

void FindPossibleDiagonalMove (int cornerID, VECTOR *moves);
void FindDiagonalCorner (int *corner);

void HandleTheFifthViewMode (void);



void VerifyAndFixProperRotation (void);







void HandleAutoCamera (void);

void HandleDistanceOscillation (void);

void SetUpAutoCamera (void);

void SetAutoCameraTimers (void);


void PrintDataTypeSizes (void);






void InitialiseDrawingProcesses (void);

void SetUpDrawingProcesses (void);

void CreateDrawProcess(DrawProcess *process, int id,
		RECT *area, u_short *offsets, int otLabel, int workAreaLabel);

void RegisterDrawProcess (DrawProcess *process);

void RemoveDrawProcess (DrawProcess *process);

void HandleOffScreenDrawing (int bufferIndex);

void ExecuteSingleDrawProcess (DrawProcess *process, int bufferIndex);

void GetOtOfProcess (DrawProcess *process, int bufferIndex, GsOT **ot);

void GetWorkAreaOfProcess (DrawProcess *process, 
							int bufferIndex, PACKET **workArea);

void DoWorkOfDrawProcess (int processID, GsOT *ot);





void InitNewBasicViewMode (void);

void InitTheFirstViewMode (void);
void InitTheSecondViewMode (void);
void InitTheThirdViewMode (void);
void InitTheFourthViewMode (void);
void InitTheFifthViewMode (void);





void OpeningSequence (void);

void OscillateSpriteRGB (GsSPRITE *sprite, int frame, int cycleLength);
void SortFireBackgroundTextureRGB (GsSPRITE *sprite, int frame);

void SortInitialGraphics (void);


void BreakoutFontPrinting (void);


void PrintViewInfo (void);


void GuiLoopSingleFntPrint (char *string);




void HandleEndOfGame (void);
  
void GuiLoopForSeveralFntPrints (void);



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


#define setNORMAL(normal, x, y, z)				\
				(normal)->nx = (x), (normal)->ny = (y), (normal)->nz = (z)
				
#define setVERTEX(vertex, x, y, z)				\
				(vertex)->vx = (x), (vertex)->vy = (y), (vertex)->vz = (z)
				
		

#define copyRECT(from, to)		  	\
	(to)->x = (from)->x, (to)->y = (from)->y, (to)->w = (from)->w, (to)->h = (from)->h	  


	
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



	// nice and simple
void main (void)
{
	InitialiseAll();

	//PrintDataTypeSizes();

	bufferIndex = GsGetActiveBuff();

	OpeningSequence();
						
	HandleGameState();

	CleanupProgram();
}





void HandleGameState (void)
{
	while (ProgramQuitFlag != TRUE)
		{
		switch(TopLevelGameState)
			{
			case MODE_MENU_SCREEN:
				HandleSimpleMenuScreen();
				break;
			case MODE_MAIN_GAME:
				HandleMainGameLoop();
				break;
			default:
				assert(FALSE);
			}
		}
}





void ChangeGameState (int newState)
{
	switch(newState)
		{
		case MODE_MENU_SCREEN:
			assert(TopLevelGameState == MODE_MAIN_GAME);
			TopLevelGameState = MODE_MENU_SCREEN;
			SetMenuScreenDefaultSettings();
			break;
		case MODE_MAIN_GAME:
			assert(TopLevelGameState == MODE_MENU_SCREEN);
			TopLevelGameState = MODE_MAIN_GAME;
			ClearTheCreatedTmds();
			//CreateTheInnerCube();
			//CreateTheOuterCube();
			InitialiseBreakout2();
			ViewDistanceFromOrigin = 2000;
			switch(MenuScreenOption)
				{
				case CHOOSE_GAME_DEMO:		   // make random choices
					CameraControlFlag = AUTO_CAMERA;
					AutoCameraMode = MUTATE_ON_FOUR + (rand() % 2);
					BackgroundGraphicsFlag = PLAY_ON_INNER_SIMPLE_OUTER_TEXTURE + (rand() % 4);
					OverallCameraSpeedFlag = GREYHOUND + (rand() % 2);
					FrameWhenDemoStarts = frameNumber;
					BreakoutNumberOfPlayers = 1;
					break;
				case CHOOSE_START_GAME:		  // menu choices apply
					//AutoCameraMode = OverallCameraType;
					switch(OverallGameDifficulty)
						{
						case CHICKEN:
							AutoCameraMode = FIXED_IN_FIRST;
							OverallCameraSpeedFlag = TORTOISE;
							break;
						case WALLABY:
							AutoCameraMode = FIXED_IN_FOURTH;
							OverallCameraSpeedFlag = WILDERBEAST;
							break;
						case COBRA:
							AutoCameraMode = MUTATE_ON_THREE;
							OverallCameraSpeedFlag = GREYHOUND;
							break;
						case DRAGON:
							AutoCameraMode = MUTATE_ON_FOUR;
							OverallCameraSpeedFlag = CHEETAH;
							break;
						default:
							assert(FALSE);
						}
					break;
				default:
					assert(FALSE);
				}

			CreateTheInnerCube();
			CreateTheOuterCube();
				
			if (CameraControlFlag == AUTO_CAMERA)
				{
				SetUpAutoCamera();
				}
			else
				{
				InitNewBasicViewMode();
				}
			break;
		default:
			assert(FALSE);
		}

	FramesInNewState = 0;
}





void SetMenuScreenDefaultSettings (void)
{
	BackgroundGraphicsFlag = PLAY_ON_INNER_SIMPLE_OUTER_TEXTURE;
	CameraControlFlag = AUTO_CAMERA;
	OverallCameraType = FIXED_IN_FIRST;
	OverallCameraSpeedFlag = TORTOISE;
	MenuScreenOption = CHOOSE_GRAPHICS_FLAG;
	BreakoutNumberOfPlayers = 1;
}







char string1[64], string2[64], string3[64], string4[64];

void HandleSimpleMenuScreen (void)
{
	int string2offset, string3offset, string4offset;

	frameNumber++;
	FramesInNewState++;

	DealWithMenuControls();

	GsSetRefView2(&TheView);

	HandleSound();

	switch(BackgroundGraphicsFlag)
		{
		case PLAY_ON_INNER_SIMPLE_OUTER_TEXTURE:
			sprintf(string1, "Play on Inner surface");
			sprintf(string2, "background graphics simple and fast");
			string2offset = 0;
			break;
		case PLAY_ON_INNER_FLASH_OUTER_TEXTURE:
			sprintf(string1, "Play on Inner surface");
			sprintf(string2, "background graphics flashy but slow");
			string2offset = 0;
			break;
		case PLAY_ON_OUTER_ONLY:
			sprintf(string1, "Play on Outer surface");
			sprintf(string2, "no background graphics");
			string2offset = 50;
			break;
		case PLAY_ON_BOTH_SURFACES:
			sprintf(string1, "Play on both surfaces");
			sprintf(string2, "no background graphics");
			string2offset = 50;
			break;
		default:
			assert(FALSE);
		}

	switch(BreakoutNumberOfPlayers)
		{
		case 1:
			sprintf(string3, "Single player");
			string3offset = 0;
			break;
		case 2:
			sprintf(string3, "Two player");
			string3offset = 15;
			break;
		default:
			assert(FALSE);
		}

	switch(OverallGameDifficulty)
		{
		case CHICKEN:
			sprintf(string4, "Game difficulty chicken");
			string4offset = 0;
			break;
		case WALLABY:
			sprintf(string4, "Game difficulty wallabe");
			string4offset = 0;
			break;
		case COBRA:
			sprintf(string4, "Game difficulty cobra");
			string4offset = 10;
			break;
		case DRAGON:
			sprintf(string4, "Game difficulty dragon");
			string4offset = 0;
			break;
		default:
			assert(FALSE);
		}

	switch(MenuScreenOption)
		{
		case CHOOSE_GRAPHICS_FLAG:
			RegisterTextStringForDisplay(string1, -72, -90, RED_COLOUR);
			RegisterTextStringForDisplay(string2, -128 + string2offset, -75, RED_COLOUR);
			RegisterTextStringForDisplay(string3, -55 + string3offset, -35, NORMAL_COLOUR);
			RegisterTextStringForDisplay(string4, -88 + string4offset, 5, NORMAL_COLOUR);
			RegisterTextStringForDisplay("Rolling demo", -46, 45, NORMAL_COLOUR);
			RegisterTextStringForDisplay("Start Game", -38, 75, NORMAL_COLOUR);
			break;
		case CHOOSE_NUMBER_PLAYERS:
			RegisterTextStringForDisplay(string1, -72, -90, NORMAL_COLOUR);
			RegisterTextStringForDisplay(string2, -128 + string2offset, -75, NORMAL_COLOUR);
			RegisterTextStringForDisplay(string3, -55 + string3offset, -35, RED_COLOUR);
			RegisterTextStringForDisplay(string4, -88 + string4offset, 5, NORMAL_COLOUR);
			RegisterTextStringForDisplay("Rolling demo", -46, 45, NORMAL_COLOUR);
			RegisterTextStringForDisplay("Start Game", -38, 75, NORMAL_COLOUR);
			break;
		case CHOOSE_GAME_DIFFICULTY:
			RegisterTextStringForDisplay(string1, -72, -90, NORMAL_COLOUR);
			RegisterTextStringForDisplay(string2, -128 + string2offset, -75, NORMAL_COLOUR);
			RegisterTextStringForDisplay(string3, -55 + string3offset, -35, NORMAL_COLOUR);
			RegisterTextStringForDisplay(string4, -88 + string4offset, 5, RED_COLOUR);
			RegisterTextStringForDisplay("Rolling demo", -46, 45, NORMAL_COLOUR);
			RegisterTextStringForDisplay("Start Game", -38, 75, NORMAL_COLOUR);
			break;
		case CHOOSE_GAME_DEMO:
			RegisterTextStringForDisplay(string1, -72, -90, NORMAL_COLOUR);
			RegisterTextStringForDisplay(string2, -128 + string2offset, -75, NORMAL_COLOUR);
			RegisterTextStringForDisplay(string3, -55 + string3offset, -35, NORMAL_COLOUR);
			RegisterTextStringForDisplay(string4, -88 + string4offset, 5, NORMAL_COLOUR);
			RegisterTextStringForDisplay("Rolling demo", -46, 45, RED_COLOUR);
			RegisterTextStringForDisplay("Start Game", -38, 75, NORMAL_COLOUR);	
			break;
		case CHOOSE_START_GAME:
			RegisterTextStringForDisplay(string1, -72, -90, NORMAL_COLOUR);
			RegisterTextStringForDisplay(string2, -128 + string2offset, -75, NORMAL_COLOUR);
			RegisterTextStringForDisplay(string3, -55 + string3offset, -35, NORMAL_COLOUR);
			RegisterTextStringForDisplay(string4, -88 + string4offset, 5, NORMAL_COLOUR);
			RegisterTextStringForDisplay("Rolling demo", -46, 45, NORMAL_COLOUR);
			RegisterTextStringForDisplay("Start Game", -38, 75, RED_COLOUR);
			break;
		default:
			assert(FALSE);			
		}

	GsSetWorkBase( (PACKET*)packetArea[bufferIndex]);

	GsClearOt(0, 0, &Wot[bufferIndex]);

	DisplayTextStrings (&Wot[bufferIndex]);

	cpuLoad = VSync(1);
	DrawSync(0);
	gpuLoad = VSync(1);
	hsync = VSync(0);	
			 
	ResetGraph(1);

	GsSwapDispBuff();

	GsSortClear(0, 0, 0, &Wot[bufferIndex]);

	GsDrawOt(&Wot[bufferIndex]);

	bufferIndex = GsGetActiveBuff();

	FntFlush(-1);
}






void DealWithMenuControls (void)
{
	long pad;
	int pause = 10;
	static int framesSinceLastChoice = 0;

	pad = PadRead();

	framesSinceLastChoice++;

	if (FramesInNewState < NEW_MODE_PAUSE)
		return;

	if (framesSinceLastChoice >= pause)
		{
		if (pad & PADstart && pad & PADselect)
			{
			ProgramQuitFlag = TRUE;
			return;
			}
	
		switch(MenuScreenOption)
			{
			case CHOOSE_GRAPHICS_FLAG:
				if (pad & PADRright)
					{
					if (BackgroundGraphicsFlag == PLAY_ON_BOTH_SURFACES)
						return;
					else
						{
						BackgroundGraphicsFlag++;
						}
					framesSinceLastChoice = 0;
					return;
					}
				else if (pad & PADRleft)
					{
					if (BackgroundGraphicsFlag == PLAY_ON_INNER_SIMPLE_OUTER_TEXTURE)
						return;
					else
						{
						BackgroundGraphicsFlag--;
						}
					framesSinceLastChoice = 0;
					return;
					}
				break;
			case CHOOSE_NUMBER_PLAYERS:
				if ((pad & PADRright) || (pad & PADRleft))
					{
					if (BreakoutNumberOfPlayers == 1)
						BreakoutNumberOfPlayers = 2;
					else
						{
						assert(BreakoutNumberOfPlayers == 2);
						BreakoutNumberOfPlayers = 1;
						}	
					framesSinceLastChoice = 0;
					return;
					}
				break;
			case CHOOSE_GAME_DIFFICULTY:
				if (pad & PADRright)
					{
					if (OverallGameDifficulty == DRAGON)
						return;
					else
						{
						OverallGameDifficulty++;
						}
					framesSinceLastChoice = 0;
					return;
					}
				else if (pad & PADRleft)
					{
					if (OverallGameDifficulty == CHICKEN)
						return;
					else
						{
						OverallGameDifficulty--;
						}
					framesSinceLastChoice = 0;
					return;
					}
				break;
			case CHOOSE_GAME_DEMO:
				if ((pad & PADRright) || (pad & PADRleft) || (pad & PADRdown))
					{
					ChangeGameState(MODE_MAIN_GAME);
					BreakoutAutoPilotFlag = TRUE;
					//printf("going to main game from menu screen\n");
					framesSinceLastChoice = 0;
					return;
					}
				break;
			case CHOOSE_START_GAME:
				if ((pad & PADRright) || (pad & PADRleft) || (pad & PADRdown))
					{
					ChangeGameState(MODE_MAIN_GAME);
					BreakoutAutoPilotFlag = FALSE;
					//printf("going to main game from menu screen\n");
					framesSinceLastChoice = 0;
					return;
					}
				break;
			default:
				assert(FALSE);
			}

		if (pad & PADLup)
			{
			if (MenuScreenOption == CHOOSE_GRAPHICS_FLAG)
				MenuScreenOption = CHOOSE_START_GAME;
			else
				MenuScreenOption--;
			framesSinceLastChoice = 0;
			return;
			}
		if (pad & PADLdown)
			{
			if (MenuScreenOption == CHOOSE_START_GAME)
				MenuScreenOption = CHOOSE_GRAPHICS_FLAG;
			else
				MenuScreenOption++;
			framesSinceLastChoice = 0;
			return;
			}
		}
}

	





int numBricks;

void HandleMainGameLoop (void)
{
	u_short zValue;
	MATRIX tmpls;
	ObjectHandler *object;
	int phase;		 
	int i;
	
		
#if 0
	FntPrint("~cf00frame %d\n", frameNumber);
	FntPrint("~cf00hsync %d\n", hsync);		  
	FntPrint("~cf00maxHsync %d\n", maxHsync);
	FntPrint("~cf00Average %d\n", average);
	FntPrint("~cf00cpu %d gpu %d\n", cpuLoad, gpuLoad);
	FntPrint("~cf00Many keys modify Rpad actions\n");	 
	FntPrint("~cf00L right mod: view move time\n");
	FntPrint("~cf00L down mod: auto camera mode\n");
	FntPrint("~cf00Select mod: basic view mode\n");
	FntPrint("~cf00L up mod: distance to origin\n");
	FntPrint("~cf00L left toggles camera control\n");
	switch(BasicViewMode)
		{
		case FIRST_VIEW_MODE:
			FntPrint("~cf00FIRS\n");
			break;
		case SECOND_VIEW_MODE:
			FntPrint("~cf00SECOND\n");
			break;
		case THIRD_VIEW_MODE:
			FntPrint("~cf00THIRD\n");
			break;
		case FOURTH_VIEW_MODE:
			FntPrint("~cf00FOURTH\n");
			break;
		case FIFTH_VIEW_MODE:
			FntPrint("~cf00FIFTH\n");
			break;
		default:
			assert(FALSE);
		}
	switch(CameraControlFlag)
		{
		case CONTROLLER_PAD_CAMERA:
			FntPrint("control pad camera\n");
			break;
		case AUTO_CAMERA:
			FntPrint("Proper Auto camera\n");
			break;
		default:
			assert(FALSE);
		}
	FntPrint("~cf00auto camera mode %d\n", AutoCameraMode);

	if (frameNumber % 60 == 0)
		{
		numBricks = CountNumberOfBricks();
		}
	FntPrint("~cf00Number Bricks %d\n", numBricks);

#endif

	if (frameNumber % 10 == 0)
		{
		numBricks = CountNumberOfBricks();
		//printf("numBricks %d\n", numBricks);
		}
	//FntPrint("~cf00Number Bricks %d\n", numBricks);

	if (GameHasFinishedAndResultsNeedDisplayingFlag == TRUE)
		HandleEndOfGame();

		// sort breakout
	BreakoutFontPrinting();

	BreakoutMainLoopGameLogic();

	frameNumber++;
	FramesInNewState++;

	DealWithControllerPad();

	//GsSetRefView2(&TheView);
	HandleTheView();

	HandleSound();

	HandleAllObjects();

	GsSetWorkBase( (PACKET*)packetArea[bufferIndex]);

	GsClearOt(0, 0, &Wot[bufferIndex]);

	DisplayTextStrings (&Wot[bufferIndex]);

	CycleLightsAroundAxes();
			
	for (i = 0; i < MAX_OBJECTS; i++)
		{
		if (ObjectArray[i] != NULL)
			{
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
		}

	DrawCube( &InsideCube, &Wot[bufferIndex]);
	DrawCube( &OutsideCube, &Wot[bufferIndex]);

	cpuLoad = VSync(1);
	DrawSync(0);
	gpuLoad = VSync(1);
	hsync = VSync(0);
	
	if (hsync > maxHsync)
		maxHsync = hsync;
		
	phase = frameNumber % 60;
	if (phase == 0)
		{
		average = 0;
		for (i = 0; i < 60; i++)
			{
			average += hsyncRecord[i];
			}
		average /= 60;
		} 
	else
		{
		hsyncRecord[phase] = hsync;
		}	
			 
	ResetGraph(1);

	GsSwapDispBuff();

	GsSortClear(0, 0, 0, &Wot[bufferIndex]);

	GsDrawOt(&Wot[bufferIndex]);

	bufferIndex = GsGetActiveBuff();

	FntFlush(-1);

		// handle list of tasks, each for drawing into off-screen area of VRAM
	HandleOffScreenDrawing(bufferIndex);
}




void InitialiseAll (void)
{
	PadInit();

	InitialiseRandomNumbers();

	InitialiseTextStrings();

	InitialiseSound();	   

	SortVideoMode();

	GsInitGraph(ScreenWidth, ScreenHeight, GsINTER|GsOFSGPU, 1, 0);
	GsDefDispBuff(0,0,0,ScreenHeight);

	GsInit3D();		   

		// sort our first ordering table
	Wot[0].length = FIRST_OT_LENGTH;	
	Wot[0].org = wtags[0];	   
	Wot[1].length = FIRST_OT_LENGTH;
	Wot[1].org = wtags[1];

	GsClearOt(0,0,&Wot[0]);
	GsClearOt(0,0,&Wot[1]);

	SortInitialGraphics();

		// sort our second ordering table
	Wot2[0].length = SECOND_OT_LENGTH;	
	Wot2[0].org = wtags2[0];	   
	Wot2[1].length = SECOND_OT_LENGTH;
	Wot2[1].org = wtags2[1];

	GsClearOt(0,0,&Wot2[0]);
	GsClearOt(0,0,&Wot2[1]);

		// sort our third ordering table
	Wot3[0].length = SECOND_OT_LENGTH;	
	Wot3[0].org = wtags3[0];	   
	Wot3[1].length = SECOND_OT_LENGTH;
	Wot3[1].org = wtags3[1];

	GsClearOt(0,0,&Wot3[0]);
	GsClearOt(0,0,&Wot3[1]);

		// sort our fourth ordering table
	Wot4[0].length = SECOND_OT_LENGTH;	
	Wot4[0].org = wtags4[0];	   
	Wot4[1].length = SECOND_OT_LENGTH;
	Wot4[1].org = wtags4[1];

	GsClearOt(0,0,&Wot4[0]);
	GsClearOt(0,0,&Wot4[1]);

		// sort our fifth ordering table
	Wot5[0].length = SECOND_OT_LENGTH;	
	Wot5[0].org = wtags5[0];	   
	Wot5[1].length = SECOND_OT_LENGTH;
	Wot5[1].org = wtags5[1];

	GsClearOt(0,0,&Wot5[0]);
	GsClearOt(0,0,&Wot5[1]);

		// sort our sixth ordering table
	Wot6[0].length = SECOND_OT_LENGTH;	
	Wot6[0].org = wtags6[0];	   
	Wot6[1].length = SECOND_OT_LENGTH;
	Wot6[1].org = wtags6[1];

	GsClearOt(0,0,&Wot6[0]);
	GsClearOt(0,0,&Wot6[1]);

		// sort our seventh ordering table
	Wot7[0].length = SECOND_OT_LENGTH;	
	Wot7[0].org = wtags7[0];	   
	Wot7[1].length = SECOND_OT_LENGTH;
	Wot7[1].org = wtags7[1];

	GsClearOt(0,0,&Wot7[0]);
	GsClearOt(0,0,&Wot7[1]);
		
	InitialiseFaceGeometry();

	InitialiseLighting();

	InitialiseView();

	ProperInitialiseTexture(FIRE_TEXTURE_ADDRESS, &FireTextureInfo);
	LinkSpriteToImageInfo(&FireSprite, &FireTextureInfo);

	ProperInitialiseTexture(MASCOTS_TEXTURE_ADDRESS, &MascotsTextureInfo);
	LinkSpriteToImageInfo(&MascotsSprite, &MascotsTextureInfo);

	ProperInitialiseTexture(WAVE_TEXTURE_ADDRESS, &WaveTextureInfo);
	LinkSpriteToImageInfo(&WaveSprite, &WaveTextureInfo);


	ProperInitialiseTexture(MASSIVE_PICTURE_TEXTURE_ADDRESS, &MassivePictureTextureInfo);
	LinkSpriteToImageInfo(&MassivePictureSprite, &MassivePictureTextureInfo);

	//ProperInitialiseTexture(MASSIVE_PICTURE_2_TEXTURE_ADDRESS, &MassivePicture2TextureInfo);

	ForceTextureIntoPosition(MASSIVE_PICTURE_TEXTURE_ADDRESS, 
				&MassivePicture2TextureInfo, 320, 256, -1, -1);

	LinkSpriteToImageInfo(&MassivePicture2Sprite, &MassivePicture2TextureInfo);


		 		
		// sort basic text printing
	FntLoad( 960, 256);
	FntOpen( -120, -50, 256, 200, 0, 512);

	InitialiseObjects();

	ClearTheCreatedTmds();

	InitialiseDrawingProcesses();

	//printf("before init breakout\n");
	InitialiseBreakout2();
	//printf("after init breakout\n");
	HighScore.points = 200;			  // once only init
	//strcpy(HighScore.name, "Whizzkid");

	SetUpDrawingProcesses();

	SetMenuScreenDefaultSettings();
}





	// simplest overall video-mode initialiser

void SortVideoMode (void)
{
	int currentMode;
	
	currentMode = GetVideoMode();

	printf("\n");
	switch(currentMode)
		{
		case MODE_PAL:
			printf("currently in video mode PAL\n");
			break;
		case MODE_NTSC:
			printf("currently in video mode NTSC\n");
			break;
		default:
			assert(FALSE);
		}

	printf("If everything is black and white\n");
	printf("or there are lines at bottom or top of screen\n");
	printf("Then you need to change video mode\n\n");
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

  


		// overall lighting conditions
	OverallLightMode = 0;			// does not allow fogging  
	GsSetLightMode(OverallLightMode);
}






void InitialiseView (void)
{
	CameraControlFlag = AUTO_CAMERA;

	AutoCameraMode = MUTATE_ON_FOUR; 			// FIXED_IN_FIRST;
		
	SetUpAutoCamera();
}








void InitialiseFaceGeometry (void)
{
	int i;

		// these go outwards from cube faces off into space
	setVECTOR( &FaceOutsideNormals[FRONT], 0, 0, -ONE);
	setVECTOR( &FaceOutsideNormals[RIGHT], ONE, 0, 0);
	setVECTOR( &FaceOutsideNormals[BACK], 0, 0, ONE);
	setVECTOR( &FaceOutsideNormals[LEFT], -ONE, 0, 0);
	setVECTOR( &FaceOutsideNormals[TOP], 0, -ONE, 0);
	setVECTOR( &FaceOutsideNormals[BOTTOM], 0, ONE, 0);

		// these go from face centres to centre of cube
	for (i = 0; i < 6; i++)
		{
		setVECTOR( &FaceInsideNormals[i],
			-FaceOutsideNormals[i].vx,
			-FaceOutsideNormals[i].vy,
			-FaceOutsideNormals[i].vz);
		}

		// corners of unit cube; also unit vectors
		// from any cube centre to its corners
	setVECTOR( &CubeCorners[FRONT_LEFT_TOP_CORNER], -ONE, -ONE, -ONE);
	setVECTOR( &CubeCorners[FRONT_LEFT_BOTTOM_CORNER], -ONE, ONE, -ONE);
	setVECTOR( &CubeCorners[FRONT_RIGHT_TOP_CORNER], ONE, -ONE, -ONE);
	setVECTOR( &CubeCorners[FRONT_RIGHT_BOTTOM_CORNER], ONE, ONE, -ONE);
	setVECTOR( &CubeCorners[BACK_LEFT_TOP_CORNER], -ONE, -ONE, ONE);
	setVECTOR( &CubeCorners[BACK_LEFT_BOTTOM_CORNER], -ONE, ONE, ONE);
	setVECTOR( &CubeCorners[BACK_RIGHT_TOP_CORNER], ONE, -ONE, ONE);
	setVECTOR( &CubeCorners[BACK_RIGHT_BOTTOM_CORNER], ONE, ONE, ONE);




	setVECTOR( &FaceXVectors[FRONT], ONE, 0, 0);
	setVECTOR( &FaceYVectors[FRONT], 0, -ONE, 0);

	setVECTOR( &FaceXVectors[RIGHT], 0, 0, ONE);
	setVECTOR( &FaceYVectors[RIGHT], 0, -ONE, 0);

	setVECTOR( &FaceXVectors[BACK], -ONE, 0, 0);
	setVECTOR( &FaceYVectors[BACK], 0, -ONE, 0);

	setVECTOR( &FaceXVectors[LEFT], 0, 0, -ONE);
	setVECTOR( &FaceYVectors[LEFT], 0, -ONE, 0);

	setVECTOR( &FaceXVectors[TOP], ONE, 0, 0);
	setVECTOR( &FaceYVectors[TOP], 0, 0, ONE);

	setVECTOR( &FaceXVectors[BOTTOM], ONE, 0, 0);
	setVECTOR( &FaceYVectors[BOTTOM], 0, 0, -ONE);
	


		// which can be got to from which others
	FaceAccessTable[FRONT][NORTH] = TOP;
	FaceAccessTable[FRONT][EAST] = RIGHT;
	FaceAccessTable[FRONT][SOUTH] = BOTTOM;
	FaceAccessTable[FRONT][WEST] = LEFT;

	FaceAccessTable[RIGHT][NORTH] = TOP;
	FaceAccessTable[RIGHT][EAST] = BACK;
	FaceAccessTable[RIGHT][SOUTH] = BOTTOM;
	FaceAccessTable[RIGHT][WEST] = FRONT;

	FaceAccessTable[BACK][NORTH] = TOP;
	FaceAccessTable[BACK][EAST] = LEFT;
	FaceAccessTable[BACK][SOUTH] = BOTTOM;
	FaceAccessTable[BACK][WEST] = RIGHT;

	FaceAccessTable[LEFT][NORTH] = TOP;
	FaceAccessTable[LEFT][EAST] = FRONT;
	FaceAccessTable[LEFT][SOUTH] = BOTTOM;
	FaceAccessTable[LEFT][WEST] = BACK;

	FaceAccessTable[TOP][NORTH] = BACK;
	FaceAccessTable[TOP][EAST] = RIGHT;
	FaceAccessTable[TOP][SOUTH] = FRONT;
	FaceAccessTable[TOP][WEST] = LEFT;

	FaceAccessTable[BOTTOM][NORTH] = FRONT;
	FaceAccessTable[BOTTOM][EAST] = RIGHT;
	FaceAccessTable[BOTTOM][SOUTH] = BACK;
	FaceAccessTable[BOTTOM][WEST] = LEFT;
}





void InitialiseObjects (void)
{
	int i;

	InitialiseObjectClass();
	InitialiseMiniObjectClass();

	for (i = 0; i < MAX_CUBES; i++)
		{
		InitSingleObject(&TheCubes[i]);

		BringObjectToLife(&TheCubes[i], CUBE, 
			CUBE_MODEL_ADDRESS, 0, NONE);

		RegisterObjectIntoObjectArray(&TheCubes[i]);
		}


	for (i = 0; i < MAX_POLYGONS; i++)
		{
		InitMiniObject( &ThePolygons[i]);

		RegisterMiniObjectIntoMiniObjectArray( &ThePolygons[i]);

		ThePolygons[i].alive = FALSE;
		}


	 
	LinkAllObjectsToModelsOrSprites();
	
	LinkAllObjectsToTheirCoordinateSystems();



	for (i = 0; i < MAX_CUBES; i++)
		{
		TheCubes[i].alive = FALSE;		// start off dead
		}

	//CreateMoreLittleCubesAroundLargerOne();
}





void CreateMoreLittleCubesAroundLargerOne (void)
{
	int i;

	for (i = 0; i < 8; i++)
		{
		TheCubes[i].alive = TRUE;
		}

	setVECTOR( &TheCubes[0].position, 400, 400, 400);
	setVECTOR( &TheCubes[1].position, 400, 400, -400);
	setVECTOR( &TheCubes[2].position, 400, -400, 400);
	setVECTOR( &TheCubes[3].position, -400, 400, 400);
	setVECTOR( &TheCubes[4].position, 400, -400, -400);
	setVECTOR( &TheCubes[5].position, -400, 400, -400);
	setVECTOR( &TheCubes[6].position, -400, -400, 400);
	setVECTOR( &TheCubes[7].position, -400, -400, -400);

	for (i = 0; i < 8; i++)
		{
		if (TheCubes[i].position.vx == 400)
			SetObjectScaling( &TheCubes[i], ONE/2, ONE/2, ONE/2);
		if (TheCubes[i].position.vy == 400)
			{
			TheCubes[i].rotationMomentumFlag = TRUE;
			setVECTOR( &TheCubes[i].twist, 0, 0, (rand() % 30) );
			}
		}
}





void InitialiseCubes (void)
{
	//RECT insideTextureAreasList[6];
	RECT singleInsideCubeTexture;
	//RECT outsideTextureAreasList[6];
	RECT singleOutsideCubeTexture;
	//int i;
	GsIMAGE *usersChoice;

	assert(FALSE);				// out of date

	//printf("Start of InitialiseCubes\n");

		// init to void and dead
	InitCube(&InsideCube);
	InitCube(&OutsideCube);

		// inside cube: each surface gets same large 16bit texture			 
	setRECT( &singleInsideCubeTexture,
			MassivePictureTextureInfo.px,
			MassivePictureTextureInfo.py,
			MassivePictureTextureInfo.pw,
			MassivePictureTextureInfo.ph);
	
	CreateCube ( &InsideCube, INSIDE_CUBE, 800, VISIBLE_FROM_OUTSIDE,
		2, SINGLE_CUBE_TEXTURE, TRUE, &singleInsideCubeTexture);




	//printf("BackgroundGraphicsFlag: %d\n", BackgroundGraphicsFlag);
	switch(BackgroundGraphicsFlag)
		{
		case TRUE:
			usersChoice = &MassivePicture2TextureInfo;
			break;
		case FALSE:
			usersChoice = &WaveTextureInfo;
			break;
		default:
			assert(FALSE);
		}
	setRECT( &singleOutsideCubeTexture,
			usersChoice->px,
			usersChoice->py,
			usersChoice->pw,
			usersChoice->ph);




		// outside cube: each surface gets same texture
		// which depends on user

#if 0		// old: forces simple texture
	setRECT( &singleOutsideCubeTexture,
			WaveTextureInfo.px,
			WaveTextureInfo.py,
			WaveTextureInfo.pw,
			WaveTextureInfo.ph);
#endif

	

	CreateCube ( &OutsideCube, OUTSIDE_CUBE, 5000, VISIBLE_FROM_INSIDE, 
		2, SINGLE_CUBE_TEXTURE, TRUE, &singleOutsideCubeTexture);

	//printf("End of InitialiseCubes\n");

	//printf("INSIDE cube\n");
	//PrintCubeInfo (&InsideCube);
	//printf("OUTSIDE cube\n");
	//PrintCubeInfo (&OutsideCube);

}





void CreateTheInnerCube	(void)
{
	RECT singleInsideCubeTexture;

	InitCube(&InsideCube);

	if (BackgroundGraphicsFlag == PLAY_ON_OUTER_ONLY)
		return;

		// inside cube: each surface gets same large 16bit texture			 
	setRECT( &singleInsideCubeTexture,
			MassivePictureTextureInfo.px,
			MassivePictureTextureInfo.py,
			MassivePictureTextureInfo.pw,
			MassivePictureTextureInfo.ph);
	
	CreateCube ( &InsideCube, INSIDE_CUBE, 800, VISIBLE_FROM_OUTSIDE,
		2, SINGLE_CUBE_TEXTURE, TRUE, &singleInsideCubeTexture);
}






void CreateTheOuterCube (void)
{
	RECT outsideTextureAreasList[6];
	RECT singleOutsideCubeTexture;

	InitCube(&OutsideCube);

		// outside cube: each surface gets same texture
		// which depends on user
	//printf("BackgroundGraphicsFlag: %d\n", BackgroundGraphicsFlag);
	switch(BackgroundGraphicsFlag)
		{
		case PLAY_ON_INNER_SIMPLE_OUTER_TEXTURE:
				// six textures, one for each face; each one a portion of mascots2.tim
			setRECT( &outsideTextureAreasList[0],
				MascotsTextureInfo.px + 36,
				MascotsTextureInfo.py + 59,
				32, 32);
			setRECT( &outsideTextureAreasList[1],
				MascotsTextureInfo.px + 35,
				MascotsTextureInfo.py + 3,
				32, 32);
			setRECT( &outsideTextureAreasList[2],
				MascotsTextureInfo.px + 103,
				MascotsTextureInfo.py + 2,
				32, 32);
			setRECT( &outsideTextureAreasList[3],
				MascotsTextureInfo.px + 171,
				MascotsTextureInfo.py + 63,
				32, 32);
			setRECT( &outsideTextureAreasList[4],
				MascotsTextureInfo.px + 172,
				MascotsTextureInfo.py + 2,
				32, 32);
			setRECT( &outsideTextureAreasList[5],
				MascotsTextureInfo.px + 70,
				MascotsTextureInfo.py + 63,
				32, 32);
			CreateCube ( &OutsideCube, OUTSIDE_CUBE, 5000, VISIBLE_FROM_INSIDE, 
				4, ONE_TEXTURE_PER_FACE, FALSE, outsideTextureAreasList);
			break;
		case PLAY_ON_INNER_FLASH_OUTER_TEXTURE:
			setRECT( &singleOutsideCubeTexture,
					MassivePicture2TextureInfo.px,
					MassivePicture2TextureInfo.py,
					MassivePicture2TextureInfo.pw,
					MassivePicture2TextureInfo.ph);
			CreateCube ( &OutsideCube, OUTSIDE_CUBE, 5000, VISIBLE_FROM_INSIDE, 
				4, SINGLE_CUBE_TEXTURE, TRUE, &singleOutsideCubeTexture);
			break;
		case PLAY_ON_OUTER_ONLY:
		case PLAY_ON_BOTH_SURFACES:
			setRECT( &singleOutsideCubeTexture,
					MassivePictureTextureInfo.px,
					MassivePictureTextureInfo.py,
					MassivePictureTextureInfo.pw,
					MassivePictureTextureInfo.ph);
			CreateCube ( &OutsideCube, OUTSIDE_CUBE, 5000, VISIBLE_FROM_INSIDE, 
				4, SINGLE_CUBE_TEXTURE, TRUE, &singleOutsideCubeTexture);
			break;


#if 0
		case TRUE:			  // single massive picture for outside cube surfaces
			setRECT( &singleOutsideCubeTexture,
					MassivePicture2TextureInfo.px,
					MassivePicture2TextureInfo.py,
					MassivePicture2TextureInfo.pw,
					MassivePicture2TextureInfo.ph);
			CreateCube ( &OutsideCube, OUTSIDE_CUBE, 5000, VISIBLE_FROM_INSIDE, 
				4, SINGLE_CUBE_TEXTURE, TRUE, &singleOutsideCubeTexture);
			break;
		case FALSE:	
				// six textures, one for each face; each one a portion of mascots2.tim
			setRECT( &outsideTextureAreasList[0],
				MascotsTextureInfo.px + 36,
				MascotsTextureInfo.py + 59,
				32, 32);
			setRECT( &outsideTextureAreasList[1],
				MascotsTextureInfo.px + 35,
				MascotsTextureInfo.py + 3,
				32, 32);
			setRECT( &outsideTextureAreasList[2],
				MascotsTextureInfo.px + 103,
				MascotsTextureInfo.py + 2,
				32, 32);
			setRECT( &outsideTextureAreasList[3],
				MascotsTextureInfo.px + 171,
				MascotsTextureInfo.py + 63,
				32, 32);
			setRECT( &outsideTextureAreasList[4],
				MascotsTextureInfo.px + 172,
				MascotsTextureInfo.py + 2,
				32, 32);
			setRECT( &outsideTextureAreasList[5],
				MascotsTextureInfo.px + 70,
				MascotsTextureInfo.py + 63,
				32, 32);
			CreateCube ( &OutsideCube, OUTSIDE_CUBE, 5000, VISIBLE_FROM_INSIDE, 
				4, ONE_TEXTURE_PER_FACE, FALSE, outsideTextureAreasList);		
			break;
#endif
		default:
			assert(FALSE);
		}	
}




void CleanupProgram (void)
{
	// This screen-storing done in-game when cheats are on
	//StoreScreen2 ( (u_long*)SCREEN_SAVE_ADDRESS, 0, 0, ScreenWidth, ScreenHeight);
	//StoreScreen();

	ResetGraph(3);

	CleanUpSound();

		// if this program part of a multiple module,
		// can printf to siocons to tell user to
		// invoke a new batch file, etc
}




void CycleLightsAroundAxes (void)
{
	int cyclePoint, theta;
	int lightEffectPeriod = 90;
	int smallerPeriod;
	int overallPhase;
	int localCyclePoint;

	cyclePoint = (frameNumber % lightEffectPeriod);

	theta = ONE * cyclePoint / lightEffectPeriod;

		// not very sorted, but will do
	TheLights[0].vx	= rcos(theta);
	TheLights[0].vy	= rsin(theta);
	//GsSetFlatLight(0, &TheLights[0]);

	TheLights[1].vz	= rcos(theta);
	TheLights[1].vx	= rsin(theta);
	//GsSetFlatLight(1, &TheLights[1]);

	TheLights[2].vy	= rcos(theta);
	TheLights[2].vz	= rsin(theta);
	//GsSetFlatLight(2, &TheLights[2]);

	overallPhase = (cyclePoint * 2) / lightEffectPeriod;
	smallerPeriod = lightEffectPeriod/2;
	switch(overallPhase)
		{
		case 0:
			localCyclePoint	= cyclePoint;
			TheLights[0].r = 64 + ((localCyclePoint * 128) / smallerPeriod);
			TheLights[1].g = 64 + ((localCyclePoint * 128) / smallerPeriod);
			TheLights[2].b = 64 + ((localCyclePoint * 128) / smallerPeriod);
			break;
		case 1:
			localCyclePoint	= cyclePoint - smallerPeriod;
			TheLights[0].r = 192 - ((localCyclePoint * 128) / smallerPeriod);
			TheLights[1].g = 192 - ((localCyclePoint * 128) / smallerPeriod);
			TheLights[2].b = 192 - ((localCyclePoint * 128) / smallerPeriod);
			break;
		default:
			assert(FALSE);
		}

		// register in system
	GsSetFlatLight(0, &TheLights[0]);
	GsSetFlatLight(1, &TheLights[1]);
	GsSetFlatLight(2, &TheLights[2]);

	//dumpLIGHT( &TheLights[0]);
	//dumpLIGHT( &TheLights[1]);
	//dumpLIGHT( &TheLights[2]);
}



   





void DealWithControllerPad (void)
{
	long pad;
	int controlSpeed;
	int pause = 12;
	static int framesSinceLastChoice = 0;
	int longPauseFlag = FALSE;

	pad = PadRead();

	framesSinceLastChoice++;

	if (FramesInNewState < NEW_MODE_PAUSE)
		return;

	if (MenuScreenOption == CHOOSE_GAME_DEMO)
		{
		if ( (frameNumber - FrameWhenDemoStarts) > ROLLING_DEMO_DURATION)
			{
			ChangeGameState(MODE_MENU_SCREEN);
			}
		else if (pad != 0)
			{
			ChangeGameState(MODE_MENU_SCREEN);
			}
		return;		// no controller actions in demo mode; just return to menu
		}
				

		// <start> and <select> to quit
	if (pad & PADselect && pad & PADstart)
		{
		ChangeGameState(MODE_MENU_SCREEN);
		return;
		}


#if 0		// old and dodgy pause
		// pause while start held down
	if (pad & PADstart)
		{
		while (pad & PADstart)
			{
			pad = PadRead();
			VSync(0);
			}
		}
#endif

	if (pad & PADstart)		// permanent pause, toggle by <start>
		{
		if (framesSinceLastChoice > pause)
			{
			assert(longPauseFlag == FALSE);
			longPauseFlag = TRUE;
			framesSinceLastChoice = 0;
			}
		if (longPauseFlag == TRUE)
			{
			for (;;)
				{
				if (framesSinceLastChoice < pause)
					{
					GuiLoopSingleFntPrint("\t\t~cf00Game Paused\n\n\n~cf00Press start to resume\n");
					}
				pad = PadRead();
				framesSinceLastChoice++;
				VSync(0);
				if ((pad & PADstart) && (framesSinceLastChoice > pause))
					{
					longPauseFlag = FALSE;
					break;
					}
				}
			framesSinceLastChoice = 0;
			}
		}



/******		// OLD
		// sort how fast controls act
	if (pad & PADselect)
		controlSpeed = 10;
	else
		controlSpeed = 1;
******/
	controlSpeed = 1;




		// select and R1 and R2 together: capture the screen as large 16-bit TIM
	if ((pad & PADselect) && (pad & PADR1) && (pad & PADR2) && (framesSinceLastChoice >= pause))
		{
		printf("\n\n\nAbout to capture screen\n\n\n\n");
		StoreScreen();
		CleanupProgram();
		exit(1);
		}





#if 0		// OFF
	if (pad & PADLdown)		  // Ldown: change AutoCameraMode 
		{
		if (framesSinceLastChoice >= pause)
			{
			if (pad & PADRup)
				{
				if (AutoCameraMode == FIXED_IN_FIRST)
					AutoCameraMode = MUTATE_ON_THREE;
				else
					AutoCameraMode--;
				framesSinceLastChoice = 0;
				SetUpAutoCamera();
				return;
				}
			if (pad & PADRdown)
				{
				if (AutoCameraMode == MUTATE_ON_THREE)
					AutoCameraMode = FIXED_IN_FIRST;
				else
					AutoCameraMode++;
				framesSinceLastChoice = 0;
				SetUpAutoCamera();
				return;				
				}
			}
		}
#endif





#if 0		 // OFF
	if (pad & PADLup)		  // Lup to change ViewDistanceFromOrigin
		{
		if (pad & PADRup)
			{
			ViewDistanceFromOrigin += 20 * controlSpeed;
			if (ViewDistanceFromOrigin > 4000)
				ViewDistanceFromOrigin = 4000;
			TheView.vpz = -ViewDistanceFromOrigin; 
			return;
			}
		if (pad & PADRdown)
			{
			ViewDistanceFromOrigin -= 20 * controlSpeed;
			if (ViewDistanceFromOrigin < 100)
				ViewDistanceFromOrigin = 100;
			TheView.vpz = -ViewDistanceFromOrigin;
			return;
			}
		}
#endif





	 
	  			
		

#if 0		   // OFF
		// Lright: change ViewMoveTime
	if (pad & PADLright)
		{
		if (pad & PADRup)
			{
			ViewMoveTime++;
			}
		if (pad & PADRdown)
			{
			ViewMoveTime--;
			if (ViewMoveTime < 1)
				ViewMoveTime = 1;
			}
		}
#endif




		

	if (pad & PADR2)	 // choose next BasicViewMode
		{
		if (framesSinceLastChoice >= pause && CameraControlFlag == CONTROLLER_PAD_CAMERA)
			{					  
			if (BasicViewMode == FIFTH_VIEW_MODE)	 
				{
				BasicViewMode = FIRST_VIEW_MODE;
				}
			else
				BasicViewMode++;
			InitNewBasicViewMode();
			framesSinceLastChoice = 0;
			return;
			}
		}
	if (pad & PADR1)		 // choose previous BasicViewMode
		{
		if (framesSinceLastChoice >= pause && CameraControlFlag == CONTROLLER_PAD_CAMERA)
			{					  
			if (BasicViewMode == FIRST_VIEW_MODE)	 
				{
				BasicViewMode = FIFTH_VIEW_MODE;
				}
			else
				BasicViewMode--;
			InitNewBasicViewMode();
			framesSinceLastChoice = 0;
			return;
			}
		}




	if (CameraControlFlag == CONTROLLER_PAD_CAMERA)
		{
		DealWithViewControls(pad, controlSpeed);
		}


#if 0		// no more
	   // Lleft modifier: flip from player-control-camera to auto-camera
	if (pad & PADLleft)
		{
		if (framesSinceLastChoice >= pause)
			{					  
			if (CameraControlFlag == CONTROLLER_PAD_CAMERA)
				{
				CameraControlFlag = AUTO_CAMERA;
				SetUpAutoCamera();
				}
			else
				{
				assert(CameraControlFlag == AUTO_CAMERA);
				CameraControlFlag = CONTROLLER_PAD_CAMERA;
				InitNewBasicViewMode();	
				}
			framesSinceLastChoice = 0;
			return;
			}
		}
#endif
}





void DealWithViewControls (long pad, int controlSpeed)
{
	VECTOR twist;
	VECTOR relativeMove, worldMove;
	int pause = 20;
	static int framesSinceLastViewControlMove = 0;

	assert(CameraControlFlag == CONTROLLER_PAD_CAMERA);

	framesSinceLastViewControlMove++;

	switch(BasicViewMode)
		{
		case FIRST_VIEW_MODE:
			if (ViewMoveFlag == FALSE)
				{
				if (pad & PADLleft)			// four discrete moves, two discrete twists
					{
					StartFirstModeViewMove(VIEW_TWIST_LEFT);
					}
				if (pad & PADLright)
					{
					StartFirstModeViewMove(VIEW_TWIST_RIGHT);
					}
				if (pad & PADLup)
					{
					StartFirstModeViewMove(VIEW_TWIST_UP);
					}
				if (pad & PADLdown)
					{
					StartFirstModeViewMove(VIEW_TWIST_DOWN);
					}
				if (pad & PADL1)
					{
					StartFirstModeViewMove(VIEW_TWIST_CLOCKWISE);
					}
				if (pad & PADL2)
					{
					StartFirstModeViewMove(VIEW_TWIST_ANTICLOCKWISE);
					}
				}
			break;
		case SECOND_VIEW_MODE:
			setVECTOR( &twist, 0, 0, 0);

			if (pad & PADLleft)			 // six rotations
				{
				twist.vy += 25 * controlSpeed;
				}
			if (pad & PADLright)
				{
				twist.vy -= 25 * controlSpeed;
				}
			if (pad & PADLup)
				{
				twist.vx -= 25 * controlSpeed;
				}
			if (pad & PADLdown)
				{
				twist.vx += 25 * controlSpeed;
				}
			if (pad & PADL1)
				{
				twist.vz -= 25 * controlSpeed;
				}
			if (pad & PADL2)
				{
				twist.vz += 25 * controlSpeed;
				}

			RotateCoordinateSystem (&twist, &ViewCoords);

			GsSetRefView2(&TheView);
			break;
		case THIRD_VIEW_MODE:
			setVECTOR( &twist, 0, 0, 0);
			setVECTOR( &relativeMove, 0, 0, 0);

			if (pad & PADLleft)			  // six rotations
				{
				twist.vy -= 25 * controlSpeed;
				}
			if (pad & PADLright)
				{
				twist.vy += 25 * controlSpeed;
				}
			if (pad & PADLup)
				{
				twist.vx -= 25 * controlSpeed;
				}
			if (pad & PADLdown)
				{
				twist.vx += 25 * controlSpeed;
				}
			if (pad & PADL1)
				{
				twist.vz -= 25 * controlSpeed;
				}
			if (pad & PADL2)
				{
				twist.vz += 25 * controlSpeed;
				}

			if (pad & PADRup)	   // move forwards and backwards
				{
				setVECTOR( &relativeMove, 0, 0, 20 * controlSpeed);
				}
			else if (pad & PADRdown)
				{
				setVECTOR( &relativeMove, 0, 0, 20 * -controlSpeed);
				} 

			RotateCoordinateSystem (&twist, &ViewCoords);

				// find relative movement in world terms
			ApplyMatrixLV( &ViewCoords.coord, &relativeMove, &worldMove);

			ViewCoords.coord.t[0] += worldMove.vx;
			ViewCoords.coord.t[1] += worldMove.vy;
			ViewCoords.coord.t[2] += worldMove.vz;

			ViewCoords.flg = 0;

			GsSetRefView2(&TheView);
			break;
		case FOURTH_VIEW_MODE:
			if (ViewMoveFlag == FALSE)	   // three moves, two twists
				{
				if (pad & PADLleft)
					{
					StartFourthModeViewMove(VIEW_X_MOVE);
					}
				if (pad & PADLright)
					{
					StartFourthModeViewMove(VIEW_Y_MOVE);
					}
				if (pad & PADLdown)
					{
					StartFourthModeViewMove(VIEW_Z_MOVE);
					}
				if (pad & PADL1)
					{
					StartFourthModeViewMove(VIEW_TWIST_CLOCKWISE);
					}
				if (pad & PADL2)
					{
					StartFourthModeViewMove(VIEW_TWIST_ANTICLOCKWISE);
					}
				}
			break;
		case FIFTH_VIEW_MODE:		   // circling
			if (framesSinceLastViewControlMove > pause)
				{		 	
				if ((pad & PADLleft) || (pad & PADLright))
					{
					MakeFifthModeViewMove(TOGGLE_CIRCLING_DIRECTION);
					framesSinceLastViewControlMove = 0; 
					}
				if (pad & PADLup)
					{
					MakeFifthModeViewMove(CHANGE_CIRCLING_PLANE_UP);
					framesSinceLastViewControlMove = 0;
					}
				if (pad & PADLdown)
					{
					MakeFifthModeViewMove(CHANGE_CIRCLING_PLANE_DOWN);
					framesSinceLastViewControlMove = 0;
					}
				}
			break;
		default:
			assert(FALSE);
		}
}






void HandleAllObjects (void)
{
	ObjectHandler* object;
	int i;

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
					case POLYGON:
						assert(FALSE);		// don't need handling
						break;
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



		

  	 


	// this function does object-relative movement and rotation
	// only allows one angle rotation per frame
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
		// order will by zyx
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



	// this function does self-relative rotation
void RotateCoordinateSystem (VECTOR* twist,
							GsCOORDINATE2* coordSystem)
{	
	MATRIX xMatrix, yMatrix, zMatrix;
	SVECTOR xVector, yVector, zVector;
	
	if (twist->vz != 0)
		{
		zVector.vx = 0;
		zVector.vy = 0;
		zVector.vz = twist->vz;
			
		RotMatrix(&zVector, &zMatrix);

		MulMatrix0(&coordSystem->coord, &zMatrix, &coordSystem->coord);
		}
	else if (twist->vy != 0)
		{
		yVector.vx = 0;
		yVector.vy = twist->vy;
		yVector.vz = 0;
			
		RotMatrix(&yVector, &yMatrix);

		MulMatrix0(&coordSystem->coord, &yMatrix, &coordSystem->coord);
		}
	else if (twist->vx != 0)
		{
		xVector.vx = twist->vx;
		xVector.vy = 0;
		xVector.vz = 0;
			
		RotMatrix(&xVector, &xMatrix);

		MulMatrix0(&coordSystem->coord, &xMatrix, &coordSystem->coord);
		}

		// tell GTE that coordinate system has been updated
	coordSystem->flg = 0;
}





void CopyCoordinateSystem (GsCOORDINATE2 *from, GsCOORDINATE2 *to)
{
	assert(from != to);

	to->super = from->super;

	CopyMatrix( &from->coord, &to->coord);

	to->flg = 0;
}






void CopyMatrix (MATRIX *from, MATRIX *to)
{
	assert(from != to);

	to->m[0][0] = from->m[0][0];
	to->m[0][1] = from->m[0][1];
	to->m[0][2] = from->m[0][2];
	to->m[1][0] = from->m[1][0];
	to->m[1][1] = from->m[1][1];
	to->m[1][2] = from->m[1][2];
	to->m[2][0] = from->m[2][0];
	to->m[2][1] = from->m[2][1];
	to->m[2][2] = from->m[2][2];

	to->t[0] = from->t[0];
	to->t[1] = from->t[1];
	to->t[2] = from->t[2];
}



void ExpressSubPointInSuper (VECTOR* superPoint, 
			GsCOORDINATE2* subSystem, VECTOR* subPointOutput)
{
	MATRIX* matrix;

	matrix = &subSystem->coord;

	setVECTOR(subPointOutput, 
				((superPoint->vx * matrix->m[0][0]
				+ superPoint->vy * matrix->m[1][0]
				+ superPoint->vz * matrix->m[2][0]) >> 12),

				((superPoint->vx * matrix->m[0][1]
				+ superPoint->vy * matrix->m[1][1]
				+ superPoint->vz * matrix->m[2][1]) >> 12),

				((superPoint->vx * matrix->m[0][2]
				+ superPoint->vy * matrix->m[1][2]
				+ superPoint->vz * matrix->m[2][2]) >> 12) );
}



void ExpressSuperPointInSub (VECTOR* subPoint, 
			GsCOORDINATE2* subSystem, VECTOR* superPointOutput)
{
	MATRIX* matrix;

	matrix = &subSystem->coord;

	setVECTOR(superPointOutput, 
				((subPoint->vx * matrix->m[0][0]
				+ subPoint->vy * matrix->m[0][1]
				+ subPoint->vz * matrix->m[0][2]) >> 12),

				((subPoint->vx * matrix->m[1][0]
				+ subPoint->vy * matrix->m[1][1]
				+ subPoint->vz * matrix->m[1][2]) >> 12),

				((subPoint->vx * matrix->m[2][0]
				+ subPoint->vy * matrix->m[2][1]
				+ subPoint->vz * matrix->m[2][2]) >> 12) );
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






#define STORING_SCREEN 1

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
	printf("Dsave[0]: filename %08x %x\n\n\n", (u_int)destination, (w*h/2+5)*4);

	DrawSync(0);
	VSync(0);
#endif
}
		  










void InitCube (Cube *cube)
{
	int i;

	cube->id = -1;
	cube->alive = FALSE;
	cube->sideLength = -1;
	cube->sideFlag = -1;
	cube->polysPerSide = -1;
	cube->textureType = -1;
	cube->textureChopFlag = -1;

	for (i = 0; i < MAX_POLYGONS_PER_CUBE; i++)
		{
		setRECT( &(cube->textureAreas[i]), 0, 0, 0, 0);
		cube->polygons[i] = NULL;
		}
	for (i = 0; i < 6; i++)
		{
		setVECTOR( &(cube->normals[i]), 0, 0, 0);
		}

	GsInitCoordinate2( WORLD, &cube->coord);
}







void CreateCube (Cube *cube, int id, int sideLength, int sideFlag, 
				   int polysPerSide, int type, int chopFlag, RECT *rectList)
{
	assert(id >= 0);
	assert(sideLength > 0);
	assert(polysPerSide >= 1);		
	assert(polysPerSide <= MAX_POLYS_PER_SIDE);

	InitCube(cube);
	cube->alive = TRUE;

	cube->id = id;
	cube->sideLength = sideLength;
	cube->sideFlag = sideFlag;
	cube->polysPerSide = polysPerSide;

	SortCubeTextureAreas(cube, type, rectList);

	cube->textureChopFlag = chopFlag;			

		// we know the normals: 
		// cube constructed at world origin with world's orientation
	switch(sideFlag)
		{
		case VISIBLE_FROM_INSIDE:
			setVECTOR( &(cube->normals[PLUS_X_Y_PLANE]), 0, 0, -ONE);
			setVECTOR( &(cube->normals[MINUS_X_Y_PLANE]), 0, 0, ONE);
			setVECTOR( &(cube->normals[PLUS_X_Z_PLANE]), 0, -ONE, 0);
			setVECTOR( &(cube->normals[MINUS_X_Z_PLANE]), 0, ONE, 0);
			setVECTOR( &(cube->normals[PLUS_Y_Z_PLANE]), -ONE, 0, 0);
			setVECTOR( &(cube->normals[MINUS_Y_Z_PLANE]), ONE, 0, 0);
			break;
		case VISIBLE_FROM_OUTSIDE:
			setVECTOR( &(cube->normals[PLUS_X_Y_PLANE]), 0, 0, ONE);
			setVECTOR( &(cube->normals[MINUS_X_Y_PLANE]), 0, 0, -ONE);
			setVECTOR( &(cube->normals[PLUS_X_Z_PLANE]), 0, ONE, 0);
			setVECTOR( &(cube->normals[MINUS_X_Z_PLANE]), 0, -ONE, 0);
			setVECTOR( &(cube->normals[PLUS_Y_Z_PLANE]), ONE, 0, 0);
			setVECTOR( &(cube->normals[MINUS_Y_Z_PLANE]), -ONE, 0, 0);
			break;
		default:	
			assert(FALSE);
		}

	CreateTheCubesPolygons(cube);
} 





	// just copying rectangle data into cube's memory space

void SortCubeTextureAreas (Cube *cube, int type, RECT *rectList)
{
	int numberPolygons;
	int i;

	assert(cube->alive == TRUE);

	switch(type)
		{
		case SINGLE_CUBE_TEXTURE:
			setRECT( &cube->textureAreas[0], 
				rectList[0].x, rectList[0].y,
				rectList[0].w, rectList[0].h);
			break;
		case ONE_TEXTURE_PER_FACE:
			for (i = 0; i < 6; i++)
				{
				setRECT( &cube->textureAreas[i],
					rectList[i].x, rectList[i].y,
					rectList[i].w, rectList[i].h);	
				}
			break;
		case ONE_TEXTURE_PER_POLYGON:
			numberPolygons = 6 * cube->polysPerSide * cube->polysPerSide;
			for (i = 0; i < numberPolygons; i++)
				{
				setRECT( &cube->textureAreas[i],
					rectList[i].x, rectList[i].y,
					rectList[i].w, rectList[i].h);	
				}
			break;
		case ONE_TEXTURE_PER_MAIN_AXIS:
			for (i = 0; i < 3; i++)
				{
				setRECT( &cube->textureAreas[i],
					rectList[i].x, rectList[i].y,
					rectList[i].w, rectList[i].h);	
				}
			break;
		case ONE_TEXTURE_PER_CORNER:	
			for (i = 0; i < 2; i++)
				{
				setRECT( &cube->textureAreas[i],
					rectList[i].x, rectList[i].y,
					rectList[i].w, rectList[i].h);	
				}
			break;
		default:
			assert(FALSE);
		}

	cube->textureType = type;
}




void CreateTheCubesPolygons (Cube *cube)
{
	MiniObject *polygon;
	u_long address;
	TMD_NORM normal;
	TMD_VERT vert1, vert2, vert3, vert4;
	int tPageID;
	RECT rect;
	int baseX, baseY;
	u_char u0, v0, u1, v1, u2, v2, u3, v3;
	int i, j, k;
	int number;
	int whichOne;
	int whichPoly;

	assert(cube->alive == TRUE);

	number = cube->polysPerSide;
	assert(number >= 1);
	assert(number <= MAX_POLYS_PER_SIDE);

	//printf("START of CreateTheCubesPolygons\n");
	//printf("NumberOfCreatedTMDs %d\n", NumberOfCreatedTMDs);
	//printf("CurrentTMDStackAddress %08x\n", (int) CurrentTMDStackAddress);

	whichPoly = 0;
	for (i = 0; i < 6; i++)		  // plane
		{
		for (j = 0; j < number; j++)	// grid X on plane
			{
			for (k = 0; k < number; k++)	// grid Y on plane
				{
				polygon = GetNextFreePolygon();
				assert(polygon != NULL);

				assert(whichPoly < (6 * number * number) );

				//printf("\n\ni %d j %d k %d\n\n", i, j, k);

				switch(cube->textureType)
					{
					case SINGLE_CUBE_TEXTURE:
						copyRECT(&cube->textureAreas[0],&rect);
						break;
					case ONE_TEXTURE_PER_FACE:
						copyRECT(&cube->textureAreas[i],&rect);
						break;
					case ONE_TEXTURE_PER_POLYGON:
						copyRECT(&cube->textureAreas[whichPoly],&rect);
						break;
					case ONE_TEXTURE_PER_MAIN_AXIS:
						whichOne = i % 3;
						copyRECT(&cube->textureAreas[whichOne],&rect);
						break;
					case ONE_TEXTURE_PER_CORNER:	
						if ((i % 2) == 0)
							copyRECT(&cube->textureAreas[0],&rect);
						else
							copyRECT(&cube->textureAreas[1],&rect);
						break;
					default:
						assert(FALSE);
					}

				//printf("RECT before "); dumpRECT(&rect);

				//SortRectGivenPolygonsPerSide( &rect, number, j, k);
				if (cube->textureChopFlag == TRUE)
					{
				   	switch (cube->sideFlag)
						{
						case VISIBLE_FROM_OUTSIDE:
							SortRectGivenPolygonsPerSide( &rect, number, j, k);
							break;
						case VISIBLE_FROM_INSIDE:
							SortRectGivenPolygonsPerSide( &rect, number, k, j);
							break;
						default:	
							assert(FALSE);
						}
					}

				//printf("RECT after "); dumpRECT(&rect);

				//printf("\n\nTexture for poly: ");dumpRECT(&rect); printf("\n\n");

				GetTexturePageOfRectangle (&rect, &tPageID, &baseX, &baseY);

				u0 = baseX;				v0 = baseY;
				u1 = baseX + rect.w-1;	v1 = baseY;
				u2 = baseX;				v2 = baseY + rect.h-1;
				u3 = baseX + rect.w-1;	v3 = baseY + rect.h-1;

				setNORMAL( &normal, cube->normals[i].vx,
					cube->normals[i].vy, cube->normals[i].vz);

				SortVertices(cube, i, j, k, &vert1, &vert2, &vert3, &vert4);

				address = CreateAnotherPolygonTMD (0, 0, 
							tPageID, CUBE_TEXTURES_PIXEL_MODE, 
						 u0, v0,
						 u1, v1, 
						 u2, v2, 
						 u3, v3,
						 &normal, 
						 &vert1, &vert2, &vert3, &vert4);
							
				LinkObjectHandlerToSingleTMD (&polygon->handler, 
							address);

				polygon->alive = TRUE;

				cube->polygons[whichPoly] = polygon;

				whichPoly++;
				}
			}
		}

	//printf("END of CreateTheCubesPolygons\n");
	//printf("NumberOfCreatedTMDs %d\n", NumberOfCreatedTMDs);
	//printf("CurrentTMDStackAddress %08x\n", (int) CurrentTMDStackAddress);
}






void SortRectGivenPolygonsPerSide (RECT *rect, int numPolysPerSide, int x, int y)
{
	int width, height;

	assert(numPolysPerSide >= 1);
	assert(numPolysPerSide <= MAX_POLYS_PER_SIDE);

	assert(x >= 0);
	assert(x < numPolysPerSide);
	assert(y >= 0);
	assert(y < numPolysPerSide);

	width = rect->w / numPolysPerSide;
	height = rect->h / numPolysPerSide;

	rect->x += width * x;
	rect->y += height * y;

	rect->w = width;
	rect->h = height;
}






void SortVertices (Cube *cube, int whichFace, int gridX, int gridY,
		TMD_VERT *v1, TMD_VERT *v2, TMD_VERT *v3, TMD_VERT *v4)
{
	VECTOR centreToPlane;
	VECTOR xOnPlane, yOnPlane;
	VECTOR topLeft;
	int distance, number;
	VECTOR temp;

	assert(cube->alive == TRUE);

	distance = cube->sideLength; 
	number = cube->polysPerSide;
	assert(number > 0);

	assert(gridX >= 0);
	assert(gridX < number);
	assert(gridY >= 0);
	assert(gridY < number);

	//printf("gridX %d gridY %d\n\n\n", gridX, gridY);

	switch(whichFace)
		{
		case PLUS_X_Y_PLANE:	  // back
			setVECTOR( &centreToPlane, 0, 0, distance/2);
			setVECTOR( &xOnPlane, -distance, 0, 0);
			setVECTOR( &yOnPlane, 0, -distance, 0);
			break;
		case MINUS_X_Y_PLANE:	// front
			setVECTOR( &centreToPlane, 0, 0, -distance/2);
			setVECTOR( &xOnPlane, distance, 0, 0);
			setVECTOR( &yOnPlane, 0, -distance, 0);
			break;
		case PLUS_X_Z_PLANE:	  // bottom
			setVECTOR( &centreToPlane, 0, distance/2, 0);
			setVECTOR( &xOnPlane, distance, 0, 0);
			setVECTOR( &yOnPlane, 0, 0, -distance);
			break;
		case MINUS_X_Z_PLANE:		 // top
			setVECTOR( &centreToPlane, 0, -distance/2, 0);
			setVECTOR( &xOnPlane, distance, 0, 0);
			setVECTOR( &yOnPlane, 0, 0, distance);
			break;
		case PLUS_Y_Z_PLANE:		 // right
			setVECTOR( &centreToPlane, distance/2, 0, 0);
			setVECTOR( &xOnPlane, 0, 0, distance);
			setVECTOR( &yOnPlane, 0, -distance, 0);
			break;
		case MINUS_Y_Z_PLANE:		 // left
			setVECTOR( &centreToPlane, -distance/2, 0, 0);
			setVECTOR( &xOnPlane, 0, 0, -distance);
			setVECTOR( &yOnPlane, 0, -distance, 0);
			break;
		default:
			assert(FALSE);
		}

	setVECTOR( &topLeft, 
		centreToPlane.vx - (xOnPlane.vx/2) + (yOnPlane.vx/2),
		centreToPlane.vy - (xOnPlane.vy/2) + (yOnPlane.vy/2),
		centreToPlane.vz - (xOnPlane.vz/2) + (yOnPlane.vz/2) );
	//dumpVECTOR(&topLeft);
	//printf("\n\n");

	setVERTEX(v1, 
		topLeft.vx + (xOnPlane.vx * gridX / number) - (yOnPlane.vx * gridY / number),
		topLeft.vy + (xOnPlane.vy * gridX / number) - (yOnPlane.vy * gridY / number),
		topLeft.vz + (xOnPlane.vz * gridX / number) - (yOnPlane.vz * gridY / number) );

	setVERTEX(v2, 
		v1->vx + (xOnPlane.vx / number),
		v1->vy + (xOnPlane.vy / number),
		v1->vz + (xOnPlane.vz / number) );

	setVERTEX(v3, 
		v1->vx - (yOnPlane.vx / number),
		v1->vy - (yOnPlane.vy / number),
		v1->vz - (yOnPlane.vz / number) );

	setVERTEX(v4, 
		v1->vx + (xOnPlane.vx / number) - (yOnPlane.vx / number),
		v1->vy + (xOnPlane.vy / number) - (yOnPlane.vy / number),
		v1->vz + (xOnPlane.vz / number) - (yOnPlane.vz / number) );
	
		// the order of v1 to v4 above is fixed for VISIBLE_FROM_OUTSIDE
	switch(cube->sideFlag)
		{
		case VISIBLE_FROM_INSIDE:	   // reverse order: swap v2 and v3
			setVECTOR( &temp, v2->vx, v2->vy, v2->vz);
			setVECTOR(v2, v3->vx, v3->vy, v3->vz);
			setVECTOR(v3, temp.vx, temp.vy, temp.vz);
			break;
		case VISIBLE_FROM_OUTSIDE:	 // all is well
			break;
		default:	
			assert(FALSE);
		}

	//dumpSVECTOR(v1);
	//dumpSVECTOR(v2);
	//dumpSVECTOR(v3);
	//dumpSVECTOR(v4);
	//printf("\n\n");
}







void DrawCube (Cube *cube, GsOT *ot)
{
	int i;
	MATRIX tmpls;
	int numberPolygons;
	MiniObject *polygon;
	GsCOORDINATE2 world;

	GsInitCoordinate2( WORLD, &world);

	if (cube->alive != TRUE)
		return;

	numberPolygons = 6 * cube->polysPerSide * cube->polysPerSide;

	for (i = 0; i < numberPolygons; i++)
		{
		polygon = cube->polygons[i];

		if (polygon != NULL)
			{
			if (polygon->alive == TRUE)
				{
				GsGetLs( &world, &tmpls);	   
										   
				GsSetLightMatrix(&tmpls);
											
				GsSetLsMatrix(&tmpls);
											
				GsSortObject4( &(polygon->handler), 
						ot, 3, getScratchAddr(0));
				}
			}
		}
}




MiniObject *GetNextFreePolygon (void)
{
	MiniObject *polygon;
	int i;

	polygon = NULL;
	for (i = 0; i < MAX_POLYGONS; i++)
		{
		if (ThePolygons[i].alive == FALSE)
			{
			polygon = &ThePolygons[i];
			break;
			}
		}

	return polygon;
}






u_long CreateAnotherPolygonTMD (int clutX, int clutY, 
					int tPageID, int pixelMode, 
					u_char u0, u_char v0,
					u_char u1, u_char v1, 
					u_char u2, u_char v2, 
					u_char u3, u_char v3,
					TMD_NORM* norm0, 
					TMD_VERT* vert0, 
					TMD_VERT* vert1, 
					TMD_VERT* vert2, 
					TMD_VERT* vert3)
{
	u_long address;

	address = CurrentTMDStackAddress;

	assert(address < END_OF_CREATED_TMDS_STACK);		

	CreateSimpleTMD ( (u_long*)address, clutX, clutY, tPageID,
				 pixelMode, 
				 u0, v0,
				 u1, v1, 
				 u2, v2, 
				 u3, v3,
				 norm0, 
				 vert0, 
				 vert1, 
				 vert2, 
				 vert3);

	NumberOfCreatedTMDs++;
	CurrentTMDStackAddress += SIZEOF_SINGLE_POLYGON_TMD;

	assert(address < END_OF_CREATED_TMDS_STACK);

	return address;
}





void ClearTheCreatedTmds (void)
{
	int i;
	u_long *pointer;

	NumberOfCreatedTMDs = 0;
	CurrentTMDStackAddress = START_OF_CREATED_TMDS_STACK;

		// link them all to a safe and simple TMD
	pointer = (u_long*) CUBE_MODEL_ADDRESS;
	pointer++;
	GsMapModelingData(pointer);
	pointer += 2;

	for (i = 0; i < MAX_POLYGONS; i++)
		{
		ThePolygons[i].alive = FALSE;
		//GsLinkObject4( (u_long)pointer, &ThePolygons[i].handler, 0); // NO NEED
		}
}	  



void PrintCubeInfo (Cube *cube)
{
	int i;

	printf("id %d\n", cube->id);
	printf("sideLength %d\n", cube->sideLength);
	printf("sideFlag %d\n", cube->sideFlag);
	printf("textureType %d\n", cube->textureType);
	printf("polysPerSide %d\n", cube->polysPerSide);

	for (i = 0; i < 6 * cube->polysPerSide * cube->polysPerSide; i++)
		{
		dumpRECT( &cube->textureAreas[i]);
		printf("poly pointer %ld\n", (long)cube->polygons[i]);
		}
	for (i = 0; i < 6; i++)
		{
		dumpVECTOR( &cube->normals[i]);
		}

	dumpCOORD2( &cube->coord);
}





void StartFirstModeViewMove (int viewMove)
{
	assert(viewMove >= VIEW_TWIST_LEFT);
	assert(viewMove <= VIEW_TWIST_ANTICLOCKWISE);

	ViewMoveFlag = viewMove;
	ViewMoveStartFrame = frameNumber;

		// work out useful axes on the circle that the view is rotating on
	switch(ViewMoveFlag)
		{
		case FALSE:
			assert(FALSE);
			break;
		case VIEW_TWIST_LEFT:		   // store the previous one for proper checking
		case VIEW_TWIST_RIGHT:		
		case VIEW_TWIST_UP:	
		case VIEW_TWIST_DOWN:
		case VIEW_TWIST_CLOCKWISE:	   
		case VIEW_TWIST_ANTICLOCKWISE:
			CopyCoordinateSystem( &ViewCoords, &PreviousViewCoords);	   
			break;
		default:
			assert(FALSE);
		}
}


  

void StartSecondModeViewMove (int viewMove)
{
	assert(viewMove >= VIEW_TWIST_LEFT);
	assert(viewMove <= VIEW_TWIST_ANTICLOCKWISE);

	ViewMoveFlag = viewMove;
	ViewMoveStartFrame = frameNumber;
}






void StartFourthModeViewMove (int viewMove)
{
	switch(viewMove)
		{
		case VIEW_X_MOVE:
		case VIEW_Y_MOVE:
		case VIEW_Z_MOVE:
		case VIEW_TWIST_CLOCKWISE:
		case VIEW_TWIST_ANTICLOCKWISE:
			ViewMoveFlag = viewMove;
			ViewMoveStartFrame = frameNumber;
			break;
		default:
			printf("BAD value %d\n", viewMove); 
			assert(FALSE);
		}
}




void MakeFifthModeViewMove (int viewMove)
{
	switch(viewMove)
		{
		case TOGGLE_CIRCLING_DIRECTION:			
			if (ViewCircleDirection == ANTICLOCKWISE)
				{
				ViewCircleDirection = CLOCKWISE;
				}
			else
				{
				assert(ViewCircleDirection == CLOCKWISE);
				ViewCircleDirection = ANTICLOCKWISE;
				}
			break;
		case CHANGE_CIRCLING_PLANE_UP:
			if (ViewCirclePlane == X_Y_PLANE)
				{
				ViewCirclePlane = Y_EQUALS_MINUS_Z_PLANE;
				}
			else
				ViewCirclePlane--;

			break;
		case CHANGE_CIRCLING_PLANE_DOWN:
			if (ViewCirclePlane == Y_EQUALS_MINUS_Z_PLANE)
				{
				ViewCirclePlane = X_Y_PLANE;
				}
			else
				ViewCirclePlane++;

			break;
		default:
			assert(FALSE);
		}
}

			 


void HandleTheView (void)
{
	if (CameraControlFlag == AUTO_CAMERA)
		HandleAutoCamera();

	switch(BasicViewMode)
		{
		case FIRST_VIEW_MODE:
			HandleTheFirstViewMode();
			break;
		case SECOND_VIEW_MODE:
			HandleTheSecondViewMode();
			break;
		case THIRD_VIEW_MODE:
			HandleTheThirdViewMode();
			break;
		case FOURTH_VIEW_MODE:
			HandleTheFourthViewMode();
			break;
		case FIFTH_VIEW_MODE:
			HandleTheFifthViewMode();
			break;
		default:
			assert(FALSE);
		}	

	//if (frameNumber % 30 == 0)
	//	{
	//	PrintViewInfo();
	//	}
}






void HandleTheFirstViewMode (void)
{	
	VECTOR twist;

	setVECTOR( &twist, 0, 0, 0);

	if (ViewMoveFlag != FALSE
			&& frameNumber == ViewMoveStartFrame + ViewMoveTime)
		{
		VerifyAndFixProperRotation();
		ViewMoveFlag = FALSE;
		ViewMoveStartFrame = -1;
		GsSetRefView2(&TheView);
		return;
		}

		// all view movements are done by rotating the view coordinate system
		// by 90 degrees over ViewMoveTime frames
	switch (ViewMoveFlag)
		{
		case FALSE:
			// nowt to do, leave the switch
			break;
		case VIEW_TWIST_LEFT:
			setVECTOR( &twist, 0, (ONE / (4 * ViewMoveTime)), 0);
			break;
		case VIEW_TWIST_RIGHT:
			setVECTOR( &twist, 0, -(ONE / (4 * ViewMoveTime)), 0);
			break;
		case VIEW_TWIST_UP:
			setVECTOR( &twist, -(ONE / (4 * ViewMoveTime)), 0, 0);
			break;
		case VIEW_TWIST_DOWN:
			setVECTOR( &twist, (ONE / (4 * ViewMoveTime)), 0, 0);
			break;
		case VIEW_TWIST_CLOCKWISE:		  
			setVECTOR( &twist, 0, 0, -(ONE / (4 * ViewMoveTime)) );
			break;
		case VIEW_TWIST_ANTICLOCKWISE:	   
			setVECTOR( &twist, 0, 0, (ONE / (4 * ViewMoveTime)) );
			break;
		default:
			printf("bad value %d\n", ViewMoveFlag);
			assert(FALSE);
		}

	RotateCoordinateSystem (&twist,
							&ViewCoords);

	GsSetRefView2(&TheView);
}




	// lots of small, incremental rotations lead to the accumalation
	// of errors in a matrix; single large rotations are better

void VerifyAndFixProperRotation (void)
{
	VECTOR totalTwist;
	GsCOORDINATE2 properCoordResult;

	setVECTOR( &totalTwist, 0, 0, 0);

	switch (ViewMoveFlag)
		{
		case FALSE:
			assert(FALSE);
			break;
		case VIEW_TWIST_LEFT:
			setVECTOR( &totalTwist, 0, 1024, 0);
			break;
		case VIEW_TWIST_RIGHT:
			setVECTOR( &totalTwist, 0, -1024, 0);
			break;
		case VIEW_TWIST_UP:
			setVECTOR( &totalTwist, -1024, 0, 0);
			break;
		case VIEW_TWIST_DOWN:
			setVECTOR( &totalTwist, 1024, 0, 0);
			break;
		case VIEW_TWIST_CLOCKWISE:		  
			setVECTOR( &totalTwist, 0, 0, -1024);
			break;
		case VIEW_TWIST_ANTICLOCKWISE:	   
			setVECTOR( &totalTwist, 0, 0, 1024);
			break;
		default:
			printf("bad value %d\n", ViewMoveFlag);
			assert(FALSE);
		}

	CopyCoordinateSystem( &PreviousViewCoords, &properCoordResult);
	RotateCoordinateSystem (&totalTwist,
							&properCoordResult);
							
	CopyCoordinateSystem( &properCoordResult, &ViewCoords);	
}

	  

	


void HandleTheSecondViewMode (void)
{
	VECTOR twist;

	assert(BasicViewMode == SECOND_VIEW_MODE);

	if (ViewMoveFlag != FALSE
			&& frameNumber == ViewMoveStartFrame + ViewMoveTime)
		{
		ViewMoveFlag = FALSE;
		ViewMoveStartFrame = -1;
		GsSetRefView2(&TheView);
		return;
		}

	setVECTOR( &twist, 0, 0, 0);

	switch(ViewMoveFlag)
		{
		case FALSE:
			return;
			break;
		case VIEW_TWIST_LEFT:
			twist.vy += (750 / ViewMoveTime);
			break;
		case VIEW_TWIST_RIGHT:
			twist.vy -= (750 / ViewMoveTime);
			break;
		case VIEW_TWIST_UP:
			twist.vx -= (750 / ViewMoveTime);
			break;
		case VIEW_TWIST_DOWN:
			twist.vx += (750 / ViewMoveTime);
			break;
		case VIEW_TWIST_CLOCKWISE:
			twist.vz -= (750 / ViewMoveTime);
			break;
		case VIEW_TWIST_ANTICLOCKWISE:
			twist.vz += (750 / ViewMoveTime);
			break;
		default:
			assert(FALSE);
		}

	RotateCoordinateSystem (&twist, &ViewCoords);

	GsSetRefView2(&TheView);
}




void HandleTheThirdViewMode (void)
{
	assert(BasicViewMode == THIRD_VIEW_MODE);

	// nowt to do
	GsSetRefView2(&TheView);
}

			 



	// diagonal viewer: know ViewCorner (current/recent past)
	// hence know what the x,y,z moves mean from that corner

void HandleTheFourthViewMode (void)
{
	VECTOR possibleMove;
	int angleTwist;

	if (ViewMoveFlag != FALSE
			&& frameNumber == ViewMoveStartFrame + ViewMoveTime)
		{
		ViewMoveFlag = FALSE;
		ViewMoveStartFrame = -1;
		FindDiagonalCorner( &ViewCorner);
		GsSetRefView2(&TheView);
		return;
		}

		// all view movements are done by rotating the view coordinate system
		// by 90 degrees over ViewMoveTime frames
	switch (ViewMoveFlag)
		{
		case FALSE:
			// nowt to do, leave the switch
			break;
		case VIEW_X_MOVE:
			FindPossibleDiagonalMove(ViewCorner, &possibleMove);
			TheView.vpx += possibleMove.vx / ViewMoveTime;
			break;
		case VIEW_Y_MOVE:
			FindPossibleDiagonalMove(ViewCorner, &possibleMove);
			TheView.vpy += possibleMove.vy / ViewMoveTime;
			break;
		case VIEW_Z_MOVE:
			FindPossibleDiagonalMove(ViewCorner, &possibleMove);
			TheView.vpz += possibleMove.vz / ViewMoveTime;
			break;
		case VIEW_TWIST_CLOCKWISE:		  
			angleTwist = (360/3) / ViewMoveTime;		// in degrees
			TheView.rz += (4096 * angleTwist);
			break;
		case VIEW_TWIST_ANTICLOCKWISE:	   
			angleTwist = (360/3) / ViewMoveTime;		// in degrees
			TheView.rz -= (4096 * angleTwist);
			break;
		default:
			assert(FALSE);
		}

	GsSetRefView2(&TheView);
}




		 


void FindPossibleDiagonalMove (int cornerID, VECTOR *moves)
{
	int distancePerDimension, sideLength;

	distancePerDimension 
		= (ViewDistanceFromOrigin * 1000) / SQUARE_ROOT_OF_THREE_TIMES_A_THOUSAND;
	sideLength = 2 * distancePerDimension;
	//printf("distancePerDimension: %d\n", distancePerDimension);
	//printf("sideLength: %d\n", sideLength);

	switch(cornerID)
		{
		case FRONT_LEFT_TOP_CORNER:
			setVECTOR(moves, sideLength, sideLength, sideLength);
			break;
		case FRONT_LEFT_BOTTOM_CORNER:
			setVECTOR(moves, sideLength, -sideLength, sideLength);
			break;
		case FRONT_RIGHT_TOP_CORNER:
			setVECTOR(moves, -sideLength, sideLength, sideLength);
			break;
		case FRONT_RIGHT_BOTTOM_CORNER:
			setVECTOR(moves, -sideLength, -sideLength, sideLength);
			break;
		case BACK_LEFT_TOP_CORNER:
			setVECTOR(moves, sideLength, sideLength, -sideLength);
			break;
		case BACK_LEFT_BOTTOM_CORNER:
			setVECTOR(moves, sideLength, -sideLength, -sideLength);
			break;
		case BACK_RIGHT_TOP_CORNER:
			setVECTOR(moves, -sideLength, sideLength, -sideLength);
			break;
		case BACK_RIGHT_BOTTOM_CORNER:
			setVECTOR(moves, -sideLength, -sideLength, -sideLength);
			break;
		default:
			assert(FALSE);
		}
}





void FindDiagonalCorner (int *corner)
{
	if (TheView.vpx > 0)
		{
		if (TheView.vpy > 0)
			{
			if (TheView.vpz > 0)
				{
				*corner = BACK_RIGHT_BOTTOM_CORNER;
				}
			else
				{
				assert(TheView.vpz < 0);
				*corner = FRONT_RIGHT_BOTTOM_CORNER;
				}
			}
		else
			{
			assert(TheView.vpy < 0);
			if (TheView.vpz > 0)
				{
				*corner = BACK_RIGHT_TOP_CORNER;
				}
			else
				{
				assert(TheView.vpz < 0);
				*corner = FRONT_RIGHT_TOP_CORNER;
				}
			}
		}
	else
		{
		assert(TheView.vpx < 0);
		if (TheView.vpy > 0)
			{
			if (TheView.vpz > 0)
				{
				*corner = BACK_LEFT_BOTTOM_CORNER;
				}
			else
				{
				assert(TheView.vpz < 0);
				*corner = FRONT_LEFT_BOTTOM_CORNER;
				}
			}
		else
			{
			assert(TheView.vpy < 0);
			if (TheView.vpz > 0)
				{
				*corner = BACK_LEFT_TOP_CORNER;
				}
			else
				{
				assert(TheView.vpz < 0);
				*corner = FRONT_LEFT_TOP_CORNER;
				}
			}
		}
}







	 // how much of circle is traversed per move
#define VIEW_CIRCLING_ARC_ANGLE	(ONE/4)

void HandleTheFifthViewMode (void)
{
	VECTOR firstTwist, secondTwist, extraTwist;

	assert(ViewCircleAngle >= 0 && ViewCircleAngle < 4096);

	switch(ViewCircleDirection)
		{
		case CLOCKWISE:
			ViewCircleAngle	-= VIEW_CIRCLING_ARC_ANGLE / ViewMoveTime;
			break;
		case ANTICLOCKWISE:
			ViewCircleAngle	+= VIEW_CIRCLING_ARC_ANGLE / ViewMoveTime;
			break;
		default:
			assert(FALSE);
		}

	if (ViewCircleAngle < 0)
		ViewCircleAngle += ONE;
	else if (ViewCircleAngle >= ONE)
		ViewCircleAngle -= ONE;

	switch(ViewCirclePlane)
		{
		case X_Y_PLANE:
			setVECTOR( &firstTwist, ONE/4, 0, 0);
			setVECTOR( &secondTwist, 0, 0, 0);
			setVECTOR( &extraTwist, 0, ViewCircleAngle, 0);
			break;
		case X_Z_PLANE:
			setVECTOR( &firstTwist, 0, 0, 0);
			setVECTOR( &secondTwist, 0, 0, 0);
			setVECTOR( &extraTwist, 0, ViewCircleAngle, 0);
			break;
		case Y_Z_PLANE:
			setVECTOR( &firstTwist, 0, 0, 0);
			setVECTOR( &secondTwist, 0, 0, 0);
			setVECTOR( &extraTwist, ViewCircleAngle, 0, 0);
			break;
		case X_EQUALS_Y_PLANE:
			setVECTOR( &firstTwist, 0, 0, -ONE/8);
			setVECTOR( &secondTwist, 0, 0, 0);
			setVECTOR( &extraTwist, 0, ViewCircleAngle, 0);
			break;
		case X_EQUALS_MINUS_Y_PLANE:
			setVECTOR( &firstTwist, 0, 0, ONE/8);
			setVECTOR( &secondTwist, 0, 0, 0);
			setVECTOR( &extraTwist, 0, ViewCircleAngle, 0);
			break;
		case X_EQUALS_Z_PLANE:
			setVECTOR( &firstTwist, ONE/4, 0, 0);
			setVECTOR( &secondTwist, 0, 0, -ONE/8);
			setVECTOR( &extraTwist, 0, ViewCircleAngle, 0);
			break;
		case X_EQUALS_MINUS_Z_PLANE:
			setVECTOR( &firstTwist, ONE/4, 0, 0);
			setVECTOR( &secondTwist, 0, 0, ONE/8);
			setVECTOR( &extraTwist, 0, ViewCircleAngle, 0);
			break;
		case Y_EQUALS_Z_PLANE:
			setVECTOR( &firstTwist, 0, ONE/4, 0);
			setVECTOR( &secondTwist, 0, 0, -ONE/8);
			setVECTOR( &extraTwist, 0, ViewCircleAngle, 0);
			break;
		case Y_EQUALS_MINUS_Z_PLANE:
			setVECTOR( &firstTwist, 0, ONE/4, 0);
			setVECTOR( &secondTwist, 0, 0, ONE/8);
			setVECTOR( &extraTwist, 0, ViewCircleAngle, 0);
			break;
		default:
			assert(FALSE);
		}

	GsInitCoordinate2( WORLD, &ViewCoords);
	CopyCoordinateSystem( &ViewCoords, &PreviousViewCoords);

	TheView.vpx = 0; 
	TheView.vpy = 0; 
	TheView.vpz = -ViewDistanceFromOrigin;

	if (firstTwist.vx != 0 || firstTwist.vy != 0 || firstTwist.vz != 0)
		{
		RotateCoordinateSystem( &firstTwist, &ViewCoords);
		}
	if (secondTwist.vx != 0 || secondTwist.vy != 0 || secondTwist.vz != 0)
		{
		RotateCoordinateSystem( &secondTwist, &ViewCoords);
		}

	RotateCoordinateSystem( &extraTwist, &ViewCoords);

	GsSetRefView2(&TheView);
}



   






void HandleAutoCamera (void)
{
	int modeMove;
	int newState;
	int randomNumber;

	//printf("AutoCameraMode %d\n", AutoCameraMode);
	//printf("ViewMoveFlag %d\n", ViewMoveFlag);
	//printf("ViewMoveStartFrame %d\n", ViewMoveStartFrame);
	//printf("MutaterState %d\n", MutaterState);	  

	switch(AutoCameraMode)
		{
		case FIXED_IN_FIRST:		 // make random move every N frames
			DistanceOscillationFlag = FALSE;
			if (frameNumber % AutoCameraMoveFrequency == 0)
				{
				modeMove = VIEW_TWIST_LEFT + (rand() % 6);		   // four moves, two twists
				//printf("AUTO_CAMERA: StartFirstModeViewMove\n\n\n");
				StartFirstModeViewMove(modeMove);
				}
			break;
		case FIXED_IN_SECOND:
			DistanceOscillationFlag = FALSE;
			if (frameNumber % AutoCameraMoveFrequency == 0)		  // six rotations
				{
				modeMove = VIEW_TWIST_LEFT + (rand() % 6);
				//printf("AUTO_CAMERA: StartSecondModeViewMove\n\n\n");
				StartSecondModeViewMove(modeMove);
				}
			break;
		case FIXED_IN_FOURTH:			
			DistanceOscillationFlag = FALSE;					   // three moves, two twists
			if (frameNumber % AutoCameraMoveFrequency == 0)
				{
				modeMove = VIEW_X_MOVE + (rand() % 5);
				if (modeMove > VIEW_Z_MOVE)
					modeMove -= 5;		// blaggery with #define values
				//printf("AUTO_CAMERA: StartFourthModeViewMove\n\n\n"); 
				StartFourthModeViewMove(modeMove);
				}
			break;
		case FIXED_IN_FIFTH:
			DistanceOscillationFlag = FALSE;
			if (frameNumber % AutoCameraMoveFrequency == 0)
				{
				modeMove = TOGGLE_CIRCLING_DIRECTION + (rand() % 3);  // one toggle, plane +/-
				//printf("AUTO_CAMERA: MakeFifthModeViewMove\n\n\n");
				MakeFifthModeViewMove(modeMove);
				}
			break;
		case MUTATE_ON_FOUR:
			assert((ViewMoveTime+2) == AutoCameraMoveFrequency);

				// once in a while: change basic mode and start new move
			if (frameNumber % AutoCameraMoveFrequency == 0)
				{
				for (;;)
					{
					randomNumber = rand() % 4;
					switch(randomNumber)
						{
						case 0: 	newState = FIRST_VIEW_MODE; 		break;
						case 1: 	newState = SECOND_VIEW_MODE; 		break;
						case 2: 	newState = FOURTH_VIEW_MODE; 		break;
						case 3: 	newState = FIFTH_VIEW_MODE; 		break;
						default:	assert(FALSE);
						}
					if (newState != MutaterState)
						break;
					}
				MutaterState = newState;
				BasicViewMode = MutaterState;
				//printf("AUTO_CAMERA: set new basic mode %d\n\n\n", BasicViewMode);
				InitNewBasicViewMode();
				//printf("Mutate on four move\n\n");
				switch(MutaterState)
					{
					case FIRST_VIEW_MODE:		 // make random move every N frames
						modeMove = VIEW_TWIST_LEFT + (rand() % 6);		   // four moves, two twists
						//printf("AUTO_CAMERA: StartFirstModeViewMove\n\n\n");
						StartFirstModeViewMove(modeMove);
						break;
					case SECOND_VIEW_MODE:
						modeMove = VIEW_TWIST_LEFT + (rand() % 6);
						//printf("AUTO_CAMERA: StartSecondModeViewMove\n\n\n");
						StartSecondModeViewMove(modeMove);
						break;
					case FOURTH_VIEW_MODE:									   // three moves, two twists
						modeMove = VIEW_X_MOVE + (rand() % 5);
						if (modeMove > VIEW_Z_MOVE)
							modeMove -= 5;		// blaggery with #define values
						//printf("AUTO_CAMERA: StartFourthModeViewMove\n\n\n");
						StartFourthModeViewMove(modeMove);
						break;
					case FIFTH_VIEW_MODE:
						modeMove = TOGGLE_CIRCLING_DIRECTION + (rand() % 3);  // one toggle, plane +/-
						//printf("AUTO_CAMERA: MakeFifthModeViewMove\n\n\n");
						MakeFifthModeViewMove(modeMove);
						break;
					default:
						assert(FALSE);
					}

					// see if start distance oscillation also
				if ( (rand() % DistanceOscillationFrequency) == 0)
					{
					DistanceOscillationFlag = MOVE_IN + (rand() % 2);
					DistanceOscillationStartFrame = frameNumber; 
					}	
				}
			else
				{
				if (DistanceOscillationFlag != FALSE)
					{
					HandleDistanceOscillation();
					}
				}
			break;
		case MUTATE_ON_THREE:
			assert((ViewMoveTime+2) == AutoCameraMoveFrequency);

				// once in a while: change basic mode and start new move
			if (frameNumber % AutoCameraMoveFrequency == 0)
				{
				for (;;)
					{
					randomNumber = rand() % 3;
					switch(randomNumber)
						{
						case 0: 	newState = SECOND_VIEW_MODE; 		break;
						case 1: 	newState = FOURTH_VIEW_MODE; 		break;
						case 2: 	newState = FIFTH_VIEW_MODE; 		break;
						default:	assert(FALSE);
						}
					if (newState != MutaterState)
						break;
					}
				MutaterState = newState;
				BasicViewMode = MutaterState;
				//printf("AUTO_CAMERA: set new basic mode %d\n\n\n", BasicViewMode);
				InitNewBasicViewMode();
				//printf("Mutate on three move\n\n");
				switch(MutaterState)
					{
					case SECOND_VIEW_MODE:
						modeMove = VIEW_TWIST_LEFT + (rand() % 6);
						//printf("AUTO_CAMERA: StartSecondModeViewMove\n\n\n");
						StartSecondModeViewMove(modeMove);
						break;
					case FOURTH_VIEW_MODE:									   // three moves, two twists
						modeMove = VIEW_X_MOVE + (rand() % 5);
						if (modeMove > VIEW_Z_MOVE)
							modeMove -= 5;		// blaggery with #define values
						//printf("AUTO_CAMERA: StartFourthModeViewMove\n\n\n");
						StartFourthModeViewMove(modeMove);
						break;
					case FIFTH_VIEW_MODE:
						modeMove = TOGGLE_CIRCLING_DIRECTION + (rand() % 3);  // one toggle, plane +/-
						//printf("AUTO_CAMERA: MakeFifthModeViewMove\n\n\n");
						MakeFifthModeViewMove(modeMove);
						break;
					default:
						assert(FALSE);
					}

					// see if start distance oscillation also
				if ( (rand() % DistanceOscillationFrequency) == 0)
					{
					DistanceOscillationFlag = MOVE_IN + (rand() % 2); 
					DistanceOscillationStartFrame = frameNumber;
					}
				}
			else
				{
				if (DistanceOscillationFlag != FALSE)
					{
					HandleDistanceOscillation();
					}
				}
			break;
		default:
			assert(FALSE);		// no other top level modes handled
		}
}






#define FURTHEST_OUT 3000
#define FURTHEST_IN 1000
#define TOTAL_MOVE 1000

void HandleDistanceOscillation (void)
{
	int frameInCycle;

	assert(DistanceOscillationFlag != FALSE);
	
	frameInCycle = frameNumber - DistanceOscillationStartFrame;

	if (frameNumber - DistanceOscillationStartFrame >= DistanceOscillationDuration)
		{
		DistanceOscillationFlag = FALSE; 
		DistanceOscillationStartFrame = -1;
		}
	else
		{
		if (frameInCycle < (DistanceOscillationDuration/2))
			{
			switch(DistanceOscillationFlag)
				{
				case MOVE_IN:
					ViewDistanceFromOrigin -= ((2 * TOTAL_MOVE) / DistanceOscillationDuration);
					if (ViewDistanceFromOrigin < FURTHEST_IN)
						ViewDistanceFromOrigin = FURTHEST_IN;
					break;
				case MOVE_OUT:
					ViewDistanceFromOrigin += ((2 * TOTAL_MOVE) / DistanceOscillationDuration);
					if (ViewDistanceFromOrigin > FURTHEST_OUT)
						ViewDistanceFromOrigin = FURTHEST_OUT;
					break;
				case FALSE:
					assert(FALSE);		// shouldn't get here
					break;
				default:
					assert(FALSE);
				}
			}
		else if (frameInCycle == (DistanceOscillationDuration/2))
			{
			if (DistanceOscillationFlag == MOVE_IN)
				DistanceOscillationFlag = MOVE_OUT;
			else 
				DistanceOscillationFlag = MOVE_IN;
			}
		else
			{
			switch(DistanceOscillationFlag)
				{
				case MOVE_IN:
					ViewDistanceFromOrigin -= ((2 * TOTAL_MOVE) / DistanceOscillationDuration);
					if (ViewDistanceFromOrigin < FURTHEST_IN)
						ViewDistanceFromOrigin = FURTHEST_IN;
					break;
				case MOVE_OUT:
					ViewDistanceFromOrigin += ((2 * TOTAL_MOVE) / DistanceOscillationDuration);
					if (ViewDistanceFromOrigin > FURTHEST_OUT)
						ViewDistanceFromOrigin = FURTHEST_OUT;
					break;
				case FALSE:
					assert(FALSE);		// shouldn't get here
					break;
				default:
					assert(FALSE);
				}
			}
		}

#if 0
	switch(DistanceOscillationFlag)
		{
		case MOVE_IN:
			ViewDistanceFromOrigin -= DistanceMovementSpeed;
			if (ViewDistanceFromOrigin < FURTHEST_IN)
				ViewDistanceFromOrigin = FURTHEST_IN;
			break;
		case MOVE_OUT:
			ViewDistanceFromOrigin += DistanceMovementSpeed;
			if (ViewDistanceFromOrigin > FURTHEST_OUT)
				ViewDistanceFromOrigin = FURTHEST_OUT;
			break;
		case FALSE:
			assert(FALSE);		// shouldn't get here
			break;
		default:
			assert(FALSE);
		}
#endif
}






void SetUpAutoCamera (void)
{
	int baseState;
	
	assert(CameraControlFlag == AUTO_CAMERA);

	switch(AutoCameraMode)
		{
		case FIXED_IN_FIRST:		 
			BasicViewMode = FIRST_VIEW_MODE;
			MutaterState = -1;
			break;
		case FIXED_IN_SECOND:
			BasicViewMode = SECOND_VIEW_MODE;
			MutaterState = -1;
			break;
		case FIXED_IN_FOURTH:
			BasicViewMode = FOURTH_VIEW_MODE;
			MutaterState = -1;
			break;
		case FIXED_IN_FIFTH:
			BasicViewMode = FIFTH_VIEW_MODE;
			MutaterState = -1;
			break;
		case MUTATE_ON_FOUR:
			baseState = rand() % 4;
			switch(baseState)
				{
				case 0:		MutaterState = FIRST_VIEW_MODE;			break;
				case 1:		MutaterState = SECOND_VIEW_MODE;		break;
				case 2:		MutaterState = FOURTH_VIEW_MODE;		break;
				case 3:		MutaterState = FIFTH_VIEW_MODE;			break;
				default:	assert(FALSE);
				}
			BasicViewMode = MutaterState;
			break;
		case MUTATE_ON_THREE:
			baseState = rand() % 3;
			switch(baseState)
				{
				case 0:		MutaterState = SECOND_VIEW_MODE;		break;
				case 1:		MutaterState = FOURTH_VIEW_MODE;		break;
				case 2:		MutaterState = FIFTH_VIEW_MODE;			break;
				default:	assert(FALSE);
				}
			BasicViewMode = MutaterState;
			break;
		default:
			assert(FALSE);		// no other top level modes handled
		}

	DistanceOscillationFlag = FALSE;


#if 0
	switch(OverallCameraSpeedFlag)
		{
		case TORTOISE:
			ViewMoveTime = 30;		
			AutoCameraMoveFrequency = 180;
			break;
		case WILDERBEAST:
			ViewMoveTime = 30;		
			AutoCameraMoveFrequency = 90;
			break;
		case GREYHOUND:
			ViewMoveTime = 25;		
			AutoCameraMoveFrequency = 45;
			break;
		case CHEETAH:
			ViewMoveTime = 12;		
			AutoCameraMoveFrequency = 30;
			break;
		default:
			assert(FALSE);
		}
#endif

	SetAutoCameraTimers();

	InitNewBasicViewMode();
}






	// switch on AutoCameraMode	and OverallCameraSpeedFlag

void SetAutoCameraTimers (void)
{
	switch(AutoCameraMode)
		{
		case FIXED_IN_FIRST:
		case FIXED_IN_SECOND:
		case FIXED_IN_FOURTH:
		case FIXED_IN_FIFTH:
			switch(OverallCameraSpeedFlag)
				{
				case TORTOISE:
					ViewMoveTime = 30;		
					AutoCameraMoveFrequency = 180;
					break;
				case WILDERBEAST:
					ViewMoveTime = 30;		
					AutoCameraMoveFrequency = 90;
					break;
				case GREYHOUND:
					ViewMoveTime = 25;		
					AutoCameraMoveFrequency = 45;
					break;
				case CHEETAH:
					ViewMoveTime = 12;		
					AutoCameraMoveFrequency = 30;
					break;
				default:
					assert(FALSE);
				}
			DistanceOscillationFrequency = -1;
			DistanceMovementSpeed = 0;
			DistanceOscillationDuration = 1;
			break;
		case MUTATE_ON_FOUR:
		case MUTATE_ON_THREE:
			switch(OverallCameraSpeedFlag)
				{
				case TORTOISE:		
					AutoCameraMoveFrequency = 90;
					DistanceOscillationFrequency = 16;
					DistanceOscillationDuration = 64;
					DistanceMovementSpeed = 2;
					break;
				case WILDERBEAST:
					AutoCameraMoveFrequency = 60;
					DistanceOscillationFrequency = 8;
					DistanceOscillationDuration = 64;
					DistanceMovementSpeed = 5;
					break;
				case GREYHOUND:		
					AutoCameraMoveFrequency = 40;
					DistanceOscillationFrequency = 4;
					DistanceOscillationDuration = 60;
					DistanceMovementSpeed = 15;
					break;
				case CHEETAH:		
					AutoCameraMoveFrequency = 25;
					DistanceOscillationFrequency = 2; 
					DistanceOscillationDuration = 30;
					DistanceMovementSpeed = 45;
					break;
				default:
					assert(FALSE);
				}
			ViewMoveTime = AutoCameraMoveFrequency-2;
			break;
		default:
			assert(FALSE);
		}
}





void PrintDataTypeSizes (void)
{
	ObjectHandler object;
	Cube cube;
	MiniObject miniObject;
	DrawProcess process;

	printf("\n\nsizes of data types in bytes\n\n");

	printf("ObjectHandler: %d\n", sizeof(object));
	printf("Cube: %d\n", sizeof(cube));
	printf("MiniObject: %d\n", sizeof(miniObject));
	printf("DrawProcess: %d\n", sizeof(process));

	printf("packetArea: %d\n", sizeof(packetArea));
	printf("packetArea2: %d\n", sizeof(packetArea2));
	printf("packetArea3: %d\n", sizeof(packetArea3));
	printf("packetArea4: %d\n", sizeof(packetArea4));
	printf("packetArea5: %d\n", sizeof(packetArea5));
	printf("packetArea6: %d\n", sizeof(packetArea6));
	printf("packetArea7: %d\n", sizeof(packetArea7));
	printf("Wot: %d\n", sizeof(Wot));
	printf("wtags: %d\n", sizeof(wtags));
	printf("wtags2: %d\n", sizeof(wtags2));
	printf("wtags3: %d\n", sizeof(wtags3));
	printf("wtags4: %d\n", sizeof(wtags4));
	printf("wtags5: %d\n", sizeof(wtags5));
	printf("wtags6: %d\n", sizeof(wtags6));
	printf("wtags7: %d\n", sizeof(wtags7));

	printf("ObjectArray: %d\n", sizeof(ObjectArray));
	printf("MiniObjectArray: %d\n", sizeof(MiniObjectArray));
	printf("TheCubes: %d\n", sizeof(TheCubes));
	printf("ThePolygons: %d\n", sizeof(ThePolygons));
	printf("AllDrawProcesses: %d\n", sizeof(AllDrawProcesses));
}





void InitialiseDrawingProcesses (void)
{
	int i;

	for (i = 0; i < MAX_DRAW_PROCESSES; i++)
		{
		AllDrawProcesses[i] = NULL;
		}
}





void SetUpDrawingProcesses (void)
{
	RECT clipArea;
	u_short offsets[2];

		// for breakout, only need one draw process
	setRECT( &clipArea, 
		MassivePictureTextureInfo.px, MassivePictureTextureInfo.py, 
		MassivePictureTextureInfo.pw, MassivePictureTextureInfo.ph);
	offsets[0] = MassivePictureTextureInfo.px;
	offsets[1] = MassivePictureTextureInfo.py;
	//dumpRECT( &clipArea);
	//printf("offsets %d and %d\n\n", offsets[0], offsets[1]);
	CreateDrawProcess( &FirstDrawProcess, FIRST,
		&clipArea, offsets, FIRST, FIRST);
	RegisterDrawProcess( &FirstDrawProcess);
}






void CreateDrawProcess(DrawProcess *process, int id,
		RECT *area, u_short *offsets, int otLabel, int packetAreaLabel)
{	
	assert(process != NULL);

	assert(area->w >= 1);
	assert(area->h >= 1);
	
	process->id = id;
	process->alive = TRUE;
	
	setRECT( &process->clipArea, area->x, area->y, area->w, area->h);
	process->offsets[0] = offsets[0];
	process->offsets[1] = offsets[1];

	process->otLabel = otLabel;
	process->packetAreaLabel = packetAreaLabel;
}




void RegisterDrawProcess (DrawProcess *process)
{
	int freeSlotID;
	int i;

		// firstly look for free slot in array
	freeSlotID = -1;
	for (i = 0; i < MAX_DRAW_PROCESSES; i++)
		{
		if (AllDrawProcesses[i] == NULL)
			{
			freeSlotID = i;
			break;
			}
		}

	if (freeSlotID == -1)
		{
		printf("\n\n No free slots to register draw process\n\n\n");
		return;
		}
	else
		{
		AllDrawProcesses[freeSlotID] = process;
		}
}





void RemoveDrawProcess (DrawProcess *process)
{
	int slotID;
	int i;

		// firstly look for the slot in array
	slotID = -1;
	for (i = 0; i < MAX_DRAW_PROCESSES; i++)
		{
		if (AllDrawProcesses[i] == process)
			{
			slotID = i;
			break;
			}
		}

	if (slotID == -1)
		{
		printf("\n\n process was not found in array, could not remove\n\n\n");
		return;
		}
	else
		{
		AllDrawProcesses[slotID] = NULL;
		}
}





void HandleOffScreenDrawing (int bufferIndex)
{
	DrawProcess *drawProcess;
	int i;

	for (i = 0; i < MAX_DRAW_PROCESSES; i++)
		{
		drawProcess = AllDrawProcesses[i];

		if (drawProcess != NULL)
			{
			if (drawProcess->alive == TRUE)
				{
				ExecuteSingleDrawProcess (drawProcess, bufferIndex);
				}
			}
		}
}



void ExecuteSingleDrawProcess (DrawProcess *process, int bufferIndex)
{
	RECT previousClip;
	u_short previousOffsets[2];
	GsOT *ot;
	PACKET *packetArea;

	assert(process->alive == TRUE);

		// store previous
	setRECT( &previousClip, GsDRAWENV.clip.x, GsDRAWENV.clip.y,
		GsDRAWENV.clip.w, GsDRAWENV.clip.h);
	previousOffsets[0] = GsDRAWENV.ofs[0];
	previousOffsets[1] = GsDRAWENV.ofs[1];

/******
	if (frameNumber % 15 == 0)
		{
		printf("previous business :-\n");
		dumpRECT( &previousClip);
		printf("previous offsets %d, %d\n\n\n", 
			previousOffsets[0], previousOffsets[1]);
		}
******/

		// set new drawing environment
	setRECT( &GsDRAWENV.clip, 
			process->clipArea.x,
			process->clipArea.y,
			process->clipArea.w,
			process->clipArea.h);
	GsDRAWENV.ofs[0] = process->offsets[0];
	GsDRAWENV.ofs[1] = process->offsets[1];
	PutDrawEnv( &GsDRAWENV);

/*******
	if (frameNumber % 15 == 0)
		{
		printf("newly set business :-\n");
		dumpRECT( &GsDRAWENV.clip);
		printf("new offsets %d, %d\n\n\n", 
			GsDRAWENV.ofs[0], GsDRAWENV.ofs[1]);
		}
*******/

		// get the right OT
	GetOtOfProcess(process, bufferIndex, &ot);
	GsClearOt(0, 0, ot);

		// get the right packet area, use as work base for GPU
	GetWorkAreaOfProcess(process, bufferIndex, &packetArea);
	GsSetWorkBase(packetArea);

		// actually register draw requests into OT
	DoWorkOfDrawProcess( process->id, ot);

	GsDrawOt(ot);

		// restore previous settings
	setRECT( &GsDRAWENV.clip, previousClip.x, previousClip.y,
			previousClip.w, previousClip.h);	
	GsDRAWENV.ofs[0] = previousOffsets[0];	
	GsDRAWENV.ofs[1] = previousOffsets[1];
	PutDrawEnv(&GsDRAWENV);

/******
	if (frameNumber % 15 == 0)
		{
		printf("newly restored business :-\n");
		dumpRECT( &GsDRAWENV.clip);
		printf("newly restored offsets %d, %d\n\n\n", 
			GsDRAWENV.ofs[0], GsDRAWENV.ofs[1]);
		}
******/
}





void GetOtOfProcess (DrawProcess *process, int bufferIndex, GsOT **ot)
{
	switch(process->otLabel)
		{
		case FIRST:
			*ot = &Wot2[bufferIndex];			
			break;
		case SECOND:
			*ot = &Wot3[bufferIndex];			
			break;	
		case THIRD:
			*ot = &Wot4[bufferIndex];			
			break;
		case FOURTH:
			*ot = &Wot5[bufferIndex];			
			break;
		case FIFTH:
			*ot = &Wot6[bufferIndex];			
			break;
		case SIXTH:
			*ot = &Wot7[bufferIndex];			
			break;
		default:	
			assert(FALSE);
		}
}




void GetWorkAreaOfProcess (DrawProcess *process, 
							int bufferIndex, PACKET **workArea)
{
	switch(process->packetAreaLabel)
		{
		case FIRST:
			*workArea = packetArea2[bufferIndex];
			break;
		case SECOND:
			*workArea = packetArea3[bufferIndex];
			break;
		case THIRD:
			*workArea = packetArea4[bufferIndex];
			break;
		case FOURTH:
			*workArea = packetArea5[bufferIndex];
			break;
		case FIFTH:
			*workArea = packetArea6[bufferIndex];
			break;
		case SIXTH:						  
			*workArea = packetArea7[bufferIndex];
			break;
		default:	
			assert(FALSE);
		}
}





void DoWorkOfDrawProcess (int processID, GsOT *ot)
{
	switch(processID)
		{
		case FIRST:
			BreakoutDrawing (ot);
			break;
	    case SECOND:
			assert(FALSE);		// not used here
			break;
		case THIRD:
			assert(FALSE);		// not used here
			break;
		case FOURTH:
			assert(FALSE);		// not used here
			break;
		case FIFTH:
			assert(FALSE);		// not used here
			break;
		case SIXTH:
			assert(FALSE);		// not used here
			break;
		default:	
			assert(FALSE);
		}
}




void InitNewBasicViewMode (void)
{
	ViewCorner = -1;

	switch(BasicViewMode)
		{
		case FIRST_VIEW_MODE:
			InitTheFirstViewMode();
			break;
		case SECOND_VIEW_MODE:
			InitTheSecondViewMode();
			break;
		case THIRD_VIEW_MODE:
			InitTheThirdViewMode();
			break;
		case FOURTH_VIEW_MODE:
			InitTheFourthViewMode();
			break;
		case FIFTH_VIEW_MODE:
			InitTheFifthViewMode();
			break;
		default:
			printf("bad value %d\n", BasicViewMode);
			assert(FALSE);
		}

	ViewMoveFlag = FALSE;
	ViewMoveStartFrame = -1;

		// screen-to-viewpoint distance
	ProjectionDistance = 192;			
	GsSetProjection(ProjectionDistance);

	TheView.super = &ViewCoords;

	GsSetRefView2(&TheView);
}







void InitTheFirstViewMode (void)
{
	assert(BasicViewMode == FIRST_VIEW_MODE);

	//ViewDistanceFromOrigin = 2000;  

	TheView.vrx = 0; 
	TheView.vry = 0; 
	TheView.vrz = 0; 

	TheView.vpx = 0; 
	TheView.vpy = 0; 
	TheView.vpz = -ViewDistanceFromOrigin; 
	
	TheView.rz = 0;

	GsInitCoordinate2( WORLD, &ViewCoords);
	CopyCoordinateSystem( &ViewCoords, &PreviousViewCoords);
}






void InitTheSecondViewMode (void)
{
	assert(BasicViewMode == SECOND_VIEW_MODE);

	//ViewDistanceFromOrigin = 2000;  

	TheView.vrx = 0; 
	TheView.vry = 0; 
	TheView.vrz = 0; 

	TheView.vpx = 0; 
	TheView.vpy = 0; 
	TheView.vpz = -ViewDistanceFromOrigin; 
	
	TheView.rz = 0;

	GsInitCoordinate2( WORLD, &ViewCoords);
	CopyCoordinateSystem( &ViewCoords, &PreviousViewCoords);
}





void InitTheThirdViewMode (void)
{
	assert(BasicViewMode == THIRD_VIEW_MODE);

	//ViewDistanceFromOrigin = 500;  

	TheView.vrx = 0; 
	TheView.vry = 0; 
	TheView.vrz = 0; 

	TheView.vpx = 0; 
	TheView.vpy = 0; 
	TheView.vpz = -ViewDistanceFromOrigin; 
	
	TheView.rz = 0;

	GsInitCoordinate2( WORLD, &ViewCoords);
	CopyCoordinateSystem( &ViewCoords, &PreviousViewCoords);
}

	  





void InitTheFourthViewMode (void)
{
	int corner;
	int distancePerDimension; 	// ViewDistanceFromOrigin is root-3 times bigger than this

	//ViewDistanceFromOrigin = 2000;

	corner = rand() % 8;

	TheView.vrx = 0; 
	TheView.vry = 0; 
	TheView.vrz = 0; 

	distancePerDimension 
		= (ViewDistanceFromOrigin * 1000) / SQUARE_ROOT_OF_THREE_TIMES_A_THOUSAND;

	TheView.vpx = (CubeCorners[corner].vx * distancePerDimension) >> 12; 
	TheView.vpy = (CubeCorners[corner].vy * distancePerDimension) >> 12; 
	TheView.vpz = (CubeCorners[corner].vz * distancePerDimension) >> 12; 
	
	TheView.rz = 0;

	ViewCorner = corner;

	//dumpRVIEW2( &TheView);

	GsInitCoordinate2( WORLD, &ViewCoords);
	CopyCoordinateSystem( &ViewCoords, &PreviousViewCoords);
}
   		






void InitTheFifthViewMode (void)
{
	int startAngle;
	VECTOR firstTwist, secondTwist, extraTwist;

	startAngle = rand() % 4096;

	ViewCirclePlane = X_Y_PLANE + (rand() % 9);
	ViewCircleDirection = CLOCKWISE + (rand() % 2);
	ViewCircleAngle = startAngle;

	//ViewDistanceFromOrigin = 2000;

	TheView.vrx = 0; 
	TheView.vry = 0; 
	TheView.vrz = 0; 

	switch(ViewCirclePlane)
		{
		case X_Y_PLANE:
			setVECTOR( &firstTwist, ONE/4, 0, 0);
			setVECTOR( &secondTwist, 0, 0, 0);
			setVECTOR( &extraTwist, 0, startAngle, 0);
			break;
		case X_Z_PLANE:
			setVECTOR( &firstTwist, 0, 0, 0);
			setVECTOR( &secondTwist, 0, 0, 0);
			setVECTOR( &extraTwist, 0, startAngle, 0);
			break;
		case Y_Z_PLANE:
			setVECTOR( &firstTwist, 0, 0, 0);
			setVECTOR( &secondTwist, 0, 0, 0);
			setVECTOR( &extraTwist, startAngle, 0, 0);
			break;
		case X_EQUALS_Y_PLANE:
			setVECTOR( &firstTwist, 0, 0, -ONE/8);
			setVECTOR( &secondTwist, 0, 0, 0);
			setVECTOR( &extraTwist, 0, startAngle, 0);
			break;
		case X_EQUALS_MINUS_Y_PLANE:
			setVECTOR( &firstTwist, 0, 0, ONE/8);
			setVECTOR( &secondTwist, 0, 0, 0);
			setVECTOR( &extraTwist, 0, startAngle, 0);
			break;
		case X_EQUALS_Z_PLANE:
			setVECTOR( &firstTwist, ONE/4, 0, 0);
			setVECTOR( &secondTwist, 0, 0, -ONE/8);
			setVECTOR( &extraTwist, 0, startAngle, 0);
			break;
		case X_EQUALS_MINUS_Z_PLANE:
			setVECTOR( &firstTwist, ONE/4, 0, 0);
			setVECTOR( &secondTwist, 0, 0, ONE/8);
			setVECTOR( &extraTwist, 0, startAngle, 0);
			break;
		case Y_EQUALS_Z_PLANE:
			setVECTOR( &firstTwist, 0, ONE/4, 0);
			setVECTOR( &secondTwist, 0, 0, -ONE/8);
			setVECTOR( &extraTwist, 0, startAngle, 0);
			break;
		case Y_EQUALS_MINUS_Z_PLANE:
			setVECTOR( &firstTwist, 0, ONE/4, 0);
			setVECTOR( &secondTwist, 0, 0, ONE/8);
			setVECTOR( &extraTwist, 0, startAngle, 0);
			break;
		default:
			assert(FALSE);
		}

	GsInitCoordinate2( WORLD, &ViewCoords);
	CopyCoordinateSystem( &ViewCoords, &PreviousViewCoords);

	TheView.vpx = 0; 
	TheView.vpy = 0; 
	TheView.vpz = -ViewDistanceFromOrigin;

	TheView.rz = 0;

	if (firstTwist.vx != 0 || firstTwist.vy != 0 || firstTwist.vz != 0)
		{
		RotateCoordinateSystem( &firstTwist, &ViewCoords);
		}
	if (secondTwist.vx != 0 || secondTwist.vy != 0 || secondTwist.vz != 0)
		{
		RotateCoordinateSystem( &secondTwist, &ViewCoords);
		}

	RotateCoordinateSystem( &extraTwist, &ViewCoords);

	GsSetRefView2( &TheView);
}








void OpeningSequence (void)
{
#if OPENING_SEQUENCE_FLAG
	int duration = 160;
	int frames = 0;
	int pad;

	assert(duration >= 30);

	bufferIndex = GsGetActiveBuff();

		// just display one large sprite
	MassivePictureSprite.scalex = (ONE * ScreenWidth) / MassivePictureSprite.w;
	MassivePictureSprite.x = -(ScreenWidth/2);
	MassivePictureSprite.y = -(ScreenHeight/2);

	while (frames <= duration)
		{
		frames++;

		pad = PadRead();
		if (pad & PADstart && pad & PADselect)
			{
			CleanupProgram();
			exit(1);
			}

		GsSetRefView2(&TheView);

		HandleSound();

		GsSetWorkBase( (PACKET*)packetArea[bufferIndex]);

		GsClearOt(0, 0, &Wot[bufferIndex]);

		SortFireBackgroundTextureRGB( &MassivePictureSprite, frames);
			
		GsSortSprite( &MassivePictureSprite, &Wot[bufferIndex], 0);

		cpuLoad = VSync(1);
		DrawSync(0);
		gpuLoad = VSync(1);
		hsync = VSync(0);	
				 
		ResetGraph(1);

		GsSwapDispBuff();

		GsSortClear(0, 0, 0, &Wot[bufferIndex]);

		GsDrawOt(&Wot[bufferIndex]);

		bufferIndex = GsGetActiveBuff();
		}

	MassivePictureSprite.scalex = ONE;
	MassivePictureSprite.x = 0;
	MassivePictureSprite.y = 0;
#endif
}







	// specific to 160-frame opening sequence

void SortFireBackgroundTextureRGB (GsSPRITE *sprite, int frame)
{
	if (frame >= 0 && frame < 64)
		{
		sprite->r = (frame * 4) / 3;
		sprite->g = (frame * 4) / 3;
		sprite->b = (frame * 4) / 3; 
		}
	else if (frame >= 64 && frame < 96)
		{
		sprite->g = 4 * (96 - frame);
		}
	else if (frame >= 96 && frame < 128)
		{
		sprite->b = 4 * (128 - frame);
		}
	else if (frame >= 128 && frame < 160)
		{
		sprite->r = 4 * (160 - frame);
		}	
}






GsIMAGE YarozePictureImageInfo;

void SortInitialGraphics (void)
{
#if INITIAL_GRAPHICS_FLAG
	int duration = 60;
	int frames = 0;

	bufferIndex = GsGetActiveBuff();	 

	while (frames <= duration)
		{
		frames++;

		//GsSetRefView2(&TheView);

		HandleSound();

		GsSetWorkBase( (PACKET*)packetArea[bufferIndex]);

		//GsClearOt(0, 0, &Wot[bufferIndex]);
			
		cpuLoad = VSync(1);
		DrawSync(0);
		gpuLoad = VSync(1);
		hsync = VSync(0);	
				 
		//ResetGraph(1);

		GsSwapDispBuff();

		if (frames <= 4)		  // firstly clear to black
			{
			GsClearOt(0, 0, &Wot[bufferIndex]);
			GsSortClear(0, 0, 0, &Wot[bufferIndex]);
			GsDrawOt(&Wot[bufferIndex]);
			}
		else if (frames % 4 == 0)
			{
				// load the picture directly into double-buffer space
			ForceTextureIntoPosition(YAROZE_PICTURE_TEXTURE_ADDRESS, 
				&YarozePictureImageInfo, 32, 0, -1, -1);
			ForceTextureIntoPosition(YAROZE_PICTURE_TEXTURE_ADDRESS, 
				&YarozePictureImageInfo, 32, 240, -1, -1);
			}

		//GsSortClear(0, 0, 0, &Wot[bufferIndex]);

		//GsDrawOt(&Wot[bufferIndex]);

		bufferIndex = GsGetActiveBuff();
		}
#endif
}






char string66[64];
char string77[64];
char string88[64];
 
void BreakoutFontPrinting (void)		  // overriding the one in breakout.c
{
	switch(BreakoutNumberOfPlayers)
		{
		case 1:
			sprintf(string66, "Score %d", CurrentScore.points);
			RegisterTextStringForDisplay(string66, -145, -100, RED_COLOUR);

			sprintf(string77, "Lives %d", PlayerOneNumberOfLives);
			RegisterTextStringForDisplay(string77, 60, -100, RED_COLOUR);
			

			if (BreakoutAutoPilotFlag == TRUE)
				{
				sprintf(string88, "High Score %d", HighScore.points);
				RegisterTextStringForDisplay(string88, 40, 100, RED_COLOUR);

				RegisterTextStringForDisplay("Rolling demo", -145, 100, RED_COLOUR);
				}
			else
				{
				assert(BreakoutAutoPilotFlag == FALSE);

				sprintf(string88, "High Score %d", HighScore.points);
				RegisterTextStringForDisplay(string88, -45, 100, RED_COLOUR);

				//RegisterTextStringForDisplay("Play game", -145, 100, RED_COLOUR);
				}
			break;
		case 2:
			sprintf(string66, "Player One %d Lives", PlayerOneNumberOfLives);
			RegisterTextStringForDisplay(string66, -60, -100, RED_COLOUR);

			sprintf(string77, "Player Two %d Lives", PlayerTwoNumberOfLives);
			RegisterTextStringForDisplay(string77, -60, 100, RED_COLOUR);
			break;
		default:
			assert(FALSE);
		}
}





void PrintViewInfo (void)
{
	printf("\n\nPrintViewInfo\n\n");

	dumpRVIEW2( &TheView);

	dumpCOORD2( &ViewCoords);
	dumpCOORD2( &PreviousViewCoords);

	printf("ViewMoveFlag is %d\n", ViewMoveFlag);
	printf("ViewMoveTime is %d\n", ViewMoveTime);
	printf("ViewMoveStartFrame is %d\n", ViewMoveStartFrame);
	printf("ViewDistanceFromOrigin is %d\n", ViewDistanceFromOrigin);
	printf("ViewCorner is %d\n", ViewCorner);

	printf("BasicViewMode is %d\n", BasicViewMode);

	printf("CameraControlFlag is %d\n", CameraControlFlag);

	printf("AutoCameraMode is %d\n", AutoCameraMode);

	printf("AutoCameraMoveFrequency is %d\n", AutoCameraMoveFrequency);

	printf("ViewCirclePlane %d\n", ViewCirclePlane);
	printf("ViewCircleDirection %d\n", ViewCircleDirection);
	printf("ViewCircleAngle %d\n", ViewCircleAngle);

	printf("\n\n\n\n");
}






void GuiLoopSingleFntPrint (char *string)
{
	GsSetRefView2(&TheView);

	HandleSound();

	FntPrint(string);

	GsSetWorkBase( (PACKET*)packetArea[bufferIndex]);

	GsClearOt(0, 0, &Wot[bufferIndex]);

	cpuLoad = VSync(1);
	DrawSync(0);
	gpuLoad = VSync(1);
	hsync = VSync(0);	
			 
	ResetGraph(1);

	GsSwapDispBuff();

	GsSortClear(0, 0, 0, &Wot[bufferIndex]);

	GsDrawOt(&Wot[bufferIndex]);

	bufferIndex = GsGetActiveBuff();

	FntFlush(-1);	
}





char string90[64], string91[64], string92[64];

	// NOTE: this function has responsibilities of doing what 
	// used to be done in breakout\BreakoutMainLoopGameLogic
	// is resetting objects and level, etc etc

void HandleEndOfGame (void)
{
	int minimumDuration = 60;
	int i;
	long pad = 0;

	assert(GameHasFinishedAndResultsNeedDisplayingFlag == TRUE);

	switch(BreakoutNumberOfPlayers)
		{
		case 1:
			sprintf(string90, "~c00fThe game is OVER player one\n\n\n");
			sprintf(string91, "~cf00The ball has broken out!!\n\n\n");

			if (CurrentScore.points > HighScore.points)
				{
				HighScore.points = CurrentScore.points;
				sprintf(string92, 
					"~c0f0New Hi Score of %d !!!\n", HighScore.points);
				}
			else
				sprintf(string92, "\n");
			break;
		case 2:
			sprintf(string90, "~c0f0The ball has broken out!!\n\n\n\n\n");
			if (PlayerOneNumberOfLives > 0)
				{
				assert(PlayerTwoNumberOfLives <= 0);
				sprintf(string91, "~cf00Player One wins by %d lives\n",
					PlayerOneNumberOfLives);
				}
			else
				{
				assert(PlayerTwoNumberOfLives > 0);
				sprintf(string91, "~cf00Player Two wins by %d lives\n",
					PlayerTwoNumberOfLives);
				}
			sprintf(string92, "\n");
			break; 
		default:
			assert(FALSE);
		}

	for (i = 0; i < minimumDuration; i++)
		{
		GuiLoopForSeveralFntPrints();
		}
	while (pad == 0)
		{
		GuiLoopForSeveralFntPrints();
		pad = PadRead();
		}

	GameHasFinishedAndResultsNeedDisplayingFlag = FALSE;

	Level = 1;
	PlayerOneNumberOfLives = 5;
	PlayerTwoNumberOfLives = 5;
	DestroyAllBricks();
	CurrentScore.points = 0;
	SetUpLevel1();

	ChangeGameState(MODE_MENU_SCREEN);
}




	// uses strings 90, 91, 92

void GuiLoopForSeveralFntPrints (void)
{
	GsSetRefView2(&TheView);

	HandleSound();

	FntPrint(string90);
	FntPrint(string91);
	FntPrint(string92);

	GsSetWorkBase( (PACKET*)packetArea[bufferIndex]);

	GsClearOt(0, 0, &Wot[bufferIndex]);

	cpuLoad = VSync(1);
	DrawSync(0);
	gpuLoad = VSync(1);
	hsync = VSync(0);	
			 
	ResetGraph(1);

	GsSwapDispBuff();

	GsSortClear(0, 0, 0, &Wot[bufferIndex]);

	GsDrawOt(&Wot[bufferIndex]);

	bufferIndex = GsGetActiveBuff();

	FntFlush(-1);	
}
