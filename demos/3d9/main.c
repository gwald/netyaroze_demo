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








/****************************************************************************
					structures and constants
****************************************************************************/



GsIMAGE FireTextureInfo;
GsSPRITE FireSprite;

GsIMAGE MascotsTextureInfo;
GsSPRITE MascotsSprite;

GsIMAGE Mascots2TextureInfo;
GsSPRITE Mascots2Sprite;

GsIMAGE Mascots3TextureInfo;
GsSPRITE Mascots3Sprite;

GsIMAGE WaveTextureInfo;
GsSPRITE WaveSprite;

GsIMAGE Wave2TextureInfo;
GsSPRITE Wave2Sprite;







int ScreenWidth;		 
int ScreenHeight;

#define LOW_RES 1
#define HI_RES 2

int ScreenResolution;



#define PACKETMAX	2048		/* Max GPU packets */
#define PACKETMAX2	(PACKETMAX*24)

PACKET packetArea[2][PACKETMAX2]; /* GPU PACKETS AREA */

PACKET packetArea2[2][PACKETMAX2];		// another packet area
PACKET packetArea3[2][PACKETMAX];
PACKET packetArea4[2][PACKETMAX];
PACKET packetArea5[2][PACKETMAX];
PACKET packetArea6[2][PACKETMAX];
PACKET packetArea7[2][PACKETMAX];

  

#define FIRST_OT_LENGTH	9		

GsOT Wot[2];			/* Handler of OT */
GsOT_TAG wtags[2][1<<FIRST_OT_LENGTH];


#define SECOND_OT_LENGTH 9

#define THIRD_OT_LENGTH 4

GsOT Wot2[2];			/* Handler of secondOT */
GsOT_TAG wtags2[2][1<<SECOND_OT_LENGTH];

GsOT Wot3[2];			
GsOT_TAG wtags3[2][1<<THIRD_OT_LENGTH];

GsOT Wot4[2];			
GsOT_TAG wtags4[2][1<<THIRD_OT_LENGTH];

GsOT Wot5[2];			
GsOT_TAG wtags5[2][1<<THIRD_OT_LENGTH];

GsOT Wot6[2];			
GsOT_TAG wtags6[2][1<<THIRD_OT_LENGTH];

GsOT Wot7[2];			
GsOT_TAG wtags7[2][1<<THIRD_OT_LENGTH];


 



long ProjectionDistance;

	// view moves
#define VIEW_TWIST_LEFT 1
#define VIEW_TWIST_RIGHT 2
#define VIEW_TWIST_UP 3
#define VIEW_TWIST_DOWN 4
#define VIEW_TWIST_CLOCKWISE 5
#define VIEW_TWIST_ANTICLOCKWISE 6

GsRVIEW2 TheView;
GsCOORDINATE2 ViewCoords;
GsCOORDINATE2 PreviousViewCoords;
int ViewMoveFlag;
int ViewMoveTime;		// in frames
int ViewMoveStartFrame;
int ViewDistanceFromOrigin;

int CubeSurfaceViewed;
int OrientationOntoSurface;








GsF_LIGHT TheLights[3];

GsFOGPARAM TheFogging;

int OverallLightMode;






int QuitFlag = FALSE;

int frameNumber = 0;




	  







	// object types
#define CUBE 0
#define POLYGON 1
#define MIRROR 2
#define SHIP 3




#define MAX_CUBES 12
ObjectHandler TheCubes[MAX_CUBES];

ObjectHandler TheMirror;

u_long MirrorPolygonAddress;
int MirrorBasePlane;
VECTOR MirrorBaseNormal;

ObjectHandler TheShip;




#define MAX_POLYGONS 1024
MiniObject ThePolygons[MAX_POLYGONS];




   



	// textured inside or out
#define VISIBLE_FROM_INSIDE 1
#define VISIBLE_FROM_OUTSIDE	2


	// ways to texture the polygons
#define SINGLE_CUBE_TEXTURE 1			   // 1
#define ONE_TEXTURE_PER_FACE 2			   // 6
#define ONE_TEXTURE_PER_POLYGON 3		   // n.polys
#define ONE_TEXTURE_PER_MAIN_AXIS 4		   // 2
#define ONE_TEXTURE_PER_CORNER 5		   // 2



#define MAX_POLYS_PER_SIDE 4
#define MAX_POLYGONS_PER_CUBE (MAX_POLYS_PER_SIDE * MAX_POLYS_PER_SIDE * 6)

typedef struct
{
	int id;
	int alive;
	int sideLength;
	int sideFlag;
	int polysPerSide;
	int textureType;

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





#define MOVE_VIEW 1
#define MOVE_SHIP 2

int MainMode = MOVE_VIEW;



/****************************************************************************
					prototypes
****************************************************************************/


void main (void);

void InitialiseAll (void);

void InitialiseLighting (void);
void InitialiseView (void);

void InitialiseObjects (void);

void InitialiseFaceGeometry (void);

void CreateMoreLittleCubesAroundLargerOne (void);

void InitialiseCubes (void);

void SetUpTheMirror (void);

void CleanupProgram (void);

void CycleLightsAroundAxes (void);



void DealWithControllerPad (void);


void HandleAllObjects (void);

void HandleACube (ObjectHandler* object);
void HandleAMirror (ObjectHandler* object);
void HandleAShip (ObjectHandler* object);





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
					int polysPerSide, int type, RECT *rectList);


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



void StartViewMove (int viewMove);

void HandleTheView (void);


void VerifyAndFixProperRotation (void);


void SortPlaneAndAngleViewed (void);


int SameVector (VECTOR *first, VECTOR *second);


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



void DrawMirrorViewOfWorld (GsOT *ot);

void GetPositionOfViewpoint (VECTOR *output);

void ResolveVector (VECTOR* original, VECTOR* resolver,
						VECTOR* parallelPart, VECTOR* orthogonalPart);


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


void main (void)
{
	int	hsync = 0;
	int bufferIndex;			  
	u_short zValue;
	MATRIX tmpls;
	ObjectHandler *object;
	int i;
		


	InitialiseAll();

	//PrintDataTypeSizes();

	bufferIndex = GsGetActiveBuff();



	while (QuitFlag == FALSE)
		{		
		FntPrint("~cf00frame %d\n", frameNumber);
		FntPrint("~cf00hsync %d\n", hsync);
		FntPrint("~cf00projection %d\n", ProjectionDistance);
		FntPrint("~cf00vp %d %d %d\n", TheView.vpx, TheView.vpy, TheView.vpz);
		FntPrint("~cf00ViewMoveTime %d\n", ViewMoveTime);
		FntPrint("~cf00ViewDistanceFromOrigin %d\n", ViewDistanceFromOrigin);
		switch(CubeSurfaceViewed)
			{
			case PLUS_X_Y_PLANE: FntPrint("~cf00back plane viewed\n"); break;
			case MINUS_X_Y_PLANE: FntPrint("~cf00front plane viewed\n"); break;
			case PLUS_X_Z_PLANE: FntPrint("~cf00bottom plane viewed\n"); break;
			case MINUS_X_Z_PLANE: FntPrint("~cf00top plane viewed\n"); break;
			case PLUS_Y_Z_PLANE: FntPrint("~cf00right plane viewed\n"); break;
			case MINUS_Y_Z_PLANE: FntPrint("~cf00left plane viewed\n"); break;
			case -1: FntPrint("~cf00 plane view interim\n"); break;
			default:
				assert(FALSE);
			}
		FntPrint("~cf00angle on plane %d\n", OrientationOntoSurface);
		switch(MainMode)
			{
			case MOVE_VIEW:
				FntPrint("Mode: move view\n");
				break;
			case MOVE_SHIP:
				FntPrint("Mode: move ship\n");
				break;
			default:
				assert(FALSE);
			}



#if 0
  		if (frameNumber % 15 == 0)
			{
			dumpMATRIX( &ViewCoords.coord);
			}
#endif

		//RegisterTextStringForDisplay("test 1234", 0, 0);

		frameNumber++;

		DealWithControllerPad();

		//GsSetRefView2(&TheView);
		HandleTheView();

		HandleSound();

		HandleAllObjects();

		GsSetWorkBase( (PACKET*)packetArea[bufferIndex]);

		GsClearOt(0, 0, &Wot[bufferIndex]);

		DisplayTextStrings (&Wot[bufferIndex]);

		//CycleLightsAroundAxes();
			
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

		DrawCube( &InsideCube, &Wot[bufferIndex]);
		DrawCube( &OutsideCube, &Wot[bufferIndex]);

		hsync = VSync(0);	
			 
		ResetGraph(1);

		GsSwapDispBuff();

		GsSortClear(0,0,0,&Wot[bufferIndex]);

		GsDrawOt(&Wot[bufferIndex]);

		bufferIndex = GsGetActiveBuff();

		FntFlush(-1);

			// handle list of tasks, each for drawing into off-screen area of VRAM
		HandleOffScreenDrawing(bufferIndex);

		//DrawSync(0);
	   	//VSync(0);
		}

	CleanupProgram();
}







void InitialiseAll (void)
{
	PadInit();

	InitialiseRandomNumbers();

	InitialiseTextStrings();

	InitialiseSound();	   

	ScreenResolution = LOW_RES;

		// note: should be responding to video mode here:
		// should detect using GetVideoMode,
		// hence set NTSC 240 and PAL 256 (LO_RES)
		// or NTSC 480 and PAL 512 (HI_RES)

	switch(ScreenResolution)
		{
		case LOW_RES:
			ScreenWidth = 320;
			ScreenHeight = 240;
			GsInitGraph(ScreenWidth, ScreenHeight, GsINTER|GsOFSGPU, 1, 0);
			GsDefDispBuff(0, 0, 0, ScreenHeight);
			break;
		case HI_RES:
			ScreenWidth = 640;
			ScreenHeight = 480;
			GsInitGraph(ScreenWidth, ScreenHeight, GsINTER|GsOFSGPU, 1, 0);
			GsDefDispBuff(0, 0, 0, 0);
			break;
		default:
			assert(FALSE);
		}

	GsInit3D();		   

		// sort our first ordering table
	Wot[0].length = FIRST_OT_LENGTH;	
	Wot[0].org = wtags[0];	   
	Wot[1].length = FIRST_OT_LENGTH;
	Wot[1].org = wtags[1];

	GsClearOt(0,0,&Wot[0]);
	GsClearOt(0,0,&Wot[1]);

		// sort our second ordering table
	Wot2[0].length = SECOND_OT_LENGTH;	
	Wot2[0].org = wtags2[0];	   
	Wot2[1].length = SECOND_OT_LENGTH;
	Wot2[1].org = wtags2[1];

	GsClearOt(0,0,&Wot2[0]);
	GsClearOt(0,0,&Wot2[1]);

		// sort our third ordering table
	Wot3[0].length = THIRD_OT_LENGTH;	
	Wot3[0].org = wtags3[0];	   
	Wot3[1].length = THIRD_OT_LENGTH;
	Wot3[1].org = wtags3[1];

	GsClearOt(0,0,&Wot3[0]);
	GsClearOt(0,0,&Wot3[1]);

		// sort our fourth ordering table
	Wot4[0].length = THIRD_OT_LENGTH;	
	Wot4[0].org = wtags4[0];	   
	Wot4[1].length = THIRD_OT_LENGTH;
	Wot4[1].org = wtags4[1];

	GsClearOt(0,0,&Wot4[0]);
	GsClearOt(0,0,&Wot4[1]);

		// sort our fifth ordering table
	Wot5[0].length = THIRD_OT_LENGTH;	
	Wot5[0].org = wtags5[0];	   
	Wot5[1].length = THIRD_OT_LENGTH;
	Wot5[1].org = wtags5[1];

	GsClearOt(0,0,&Wot5[0]);
	GsClearOt(0,0,&Wot5[1]);

		// sort our sixth ordering table
	Wot6[0].length = THIRD_OT_LENGTH;	
	Wot6[0].org = wtags6[0];	   
	Wot6[1].length = THIRD_OT_LENGTH;
	Wot6[1].org = wtags6[1];

	GsClearOt(0,0,&Wot6[0]);
	GsClearOt(0,0,&Wot6[1]);

		// sort our seventh ordering table
	Wot7[0].length = THIRD_OT_LENGTH;	
	Wot7[0].org = wtags7[0];	   
	Wot7[1].length = THIRD_OT_LENGTH;
	Wot7[1].org = wtags7[1];

	GsClearOt(0,0,&Wot7[0]);
	GsClearOt(0,0,&Wot7[1]);
		


	InitialiseLighting();

	InitialiseView();

	ProperInitialiseTexture(FIRE_TEXTURE_ADDRESS, &FireTextureInfo);
	LinkSpriteToImageInfo(&FireSprite, &FireTextureInfo);

	ProperInitialiseTexture(MASCOTS_TEXTURE_ADDRESS, &MascotsTextureInfo);
	LinkSpriteToImageInfo(&MascotsSprite, &MascotsTextureInfo);

	ForceTextureIntoPosition (MASCOTS_TEXTURE_ADDRESS, &Mascots2TextureInfo,
						640, 256, -1, -1);
	LinkSpriteToImageInfo(&Mascots2Sprite, &Mascots2TextureInfo);

	ForceTextureIntoPosition (MASCOTS_TEXTURE_ADDRESS, &Mascots3TextureInfo,
						691, 391, -1, -1);
	LinkSpriteToImageInfo(&Mascots3Sprite, &Mascots3TextureInfo);

	ProperInitialiseTexture(WAVE_TEXTURE_ADDRESS, &WaveTextureInfo);
	LinkSpriteToImageInfo(&WaveSprite, &WaveTextureInfo);

	ProperInitialiseTexture(WAVE_2_TEXTURE_ADDRESS, &Wave2TextureInfo);
	LinkSpriteToImageInfo(&Wave2Sprite, &Wave2TextureInfo);

		 		
		// sort basic text printing
	FntLoad( 960, 256);
	FntOpen( -120, 30, 256, 200, 0, 512);

	InitialiseFaceGeometry();

	InitialiseObjects();

	ClearTheCreatedTmds();

	InitialiseCubes();

	SetUpTheMirror();

	InitialiseDrawingProcesses();

	SetUpDrawingProcesses();
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
	OverallLightMode = 0;			// does not allow fogging  
	GsSetLightMode(OverallLightMode);
}






void InitialiseView (void)
{
		// screen-to-viewpoint distance
	ProjectionDistance = 192;			
	GsSetProjection(ProjectionDistance);


	ViewMoveFlag = FALSE;
	ViewMoveTime = 30;		// half a second
	ViewMoveStartFrame = -1;
	ViewDistanceFromOrigin = 2000;  

	//TheView.vrx = 0; 
	//TheView.vry = 0; 
	//TheView.vrz = 0; 
	
	TheView.vrx = 0; 		// looking at the centre of the mirror
	TheView.vry = 0; 
	TheView.vrz = 0;

	TheView.vpx = 0; 
	TheView.vpy = 0; 
	TheView.vpz = -ViewDistanceFromOrigin; //-220;
	
	TheView.rz = 0;

	GsInitCoordinate2( WORLD, &ViewCoords);
	CopyCoordinateSystem( &ViewCoords, &PreviousViewCoords);

	//TheView.super = &ViewCoords;
	TheView.super = WORLD;

	GsSetRefView2(&TheView);


	CubeSurfaceViewed = FRONT;
	OrientationOntoSurface = 0;
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


	InitSingleObject(&TheMirror);

	BringObjectToLife(&TheMirror, MIRROR, 
		CUBE_MODEL_ADDRESS, 0, NONE);

	RegisterObjectIntoObjectArray(&TheMirror);



	InitSingleObject(&TheShip);

	BringObjectToLife(&TheShip, SHIP, 
		SHIP_MODEL_ADDRESS, 0, NONE);

	RegisterObjectIntoObjectArray(&TheShip);
	setVECTOR( &TheShip.position, 0, -800, -200);
	SetObjectScaling( &TheShip, ONE*4, ONE*4, ONE*4);



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
	TheMirror.alive = FALSE;

#if 0
		// bring alive the first cube, position it ahead of camera
	TheCubes[0].alive = TRUE;
	TheCubes[0].position.vz = 500;
		// this for later variable scaling
	SetObjectScaling( &TheCubes[0], ONE, ONE, ONE);
#endif

	CreateMoreLittleCubesAroundLargerOne();
}





void CreateMoreLittleCubesAroundLargerOne (void)
{
	int i;

#if 0
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
#endif

	for (i = 0; i < 4; i++)
		{
		TheCubes[i].alive = TRUE;
		}

	setVECTOR( &TheCubes[0].position, 500, 500, -500);
	setVECTOR( &TheCubes[1].position, 500, -500, -500);
	setVECTOR( &TheCubes[2].position, -500, 500, -500);
	setVECTOR( &TheCubes[3].position, -500, -500, -500);

	for (i = 0; i < 4; i++)
		{
		if (TheCubes[i].position.vx == 500)
			{
			SetObjectScaling( &TheCubes[i], ONE/2, ONE/2, ONE/2);
			}
		if (TheCubes[i].position.vy == 500)
			{
			TheCubes[i].rotationMomentumFlag = TRUE;
			setVECTOR( &TheCubes[i].twist, 0, 0, 15);
			}
		}

#if 0
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
#endif
}






void InitialiseCubes (void)
{
	//RECT insideTextureAreasList[6];
	//RECT singleInsideCubeTexture;
	//RECT outsideTextureAreasList[6];
	RECT singleOutsideCubeTexture;
	//int i;

	//printf("Start of InitialiseCubes\n");

		// init to void and dead
	InitCube(&InsideCube);
	InitCube(&OutsideCube);

#if 0		// single texture
	setRECT( &singleInsideCubeTexture,
			WaveTextureInfo.px,
			WaveTextureInfo.py,
			WaveTextureInfo.pw,
			WaveTextureInfo.ph);
	
	CreateCube ( &InsideCube, INSIDE_CUBE, 400, VISIBLE_FROM_OUTSIDE,
		1, SINGLE_CUBE_TEXTURE, &singleInsideCubeTexture);
#endif


#if 0
	for (i = 0; i < 6; i++)
		{
		if ((i % 2) == 0)
			{
			setRECT( &insideTextureAreasList[i],
				WaveTextureInfo.px,
				WaveTextureInfo.py,
				WaveTextureInfo.pw,
				WaveTextureInfo.ph);
			}
		else
			{
			setRECT( &insideTextureAreasList[i],
				MascotsTextureInfo.px,
				MascotsTextureInfo.py,
				MascotsTextureInfo.pw,
				MascotsTextureInfo.ph);
			} 
		}
	CreateCube ( &InsideCube, INSIDE_CUBE, 400, VISIBLE_FROM_OUTSIDE,
		2, ONE_TEXTURE_PER_FACE, insideTextureAreasList);
#endif


#if 0
		// first cube: two textures
	setRECT( &insideTextureAreasList[0],
				WaveTextureInfo.px,
				WaveTextureInfo.py,
				WaveTextureInfo.pw,
				WaveTextureInfo.ph);
	setRECT( &insideTextureAreasList[1],
				MascotsTextureInfo.px,
				MascotsTextureInfo.py,
				MascotsTextureInfo.pw,
				MascotsTextureInfo.ph); 

	CreateCube ( &InsideCube, INSIDE_CUBE, 400, VISIBLE_FROM_OUTSIDE,
		2, ONE_TEXTURE_PER_CORNER, insideTextureAreasList);
#endif



	 
	

#if 0
		// second cube: two textures
	setRECT( &outsideTextureAreasList[0],
				WaveTextureInfo.px,
				WaveTextureInfo.py,
				WaveTextureInfo.pw,
				WaveTextureInfo.ph);
	setRECT( &outsideTextureAreasList[1],
				MascotsTextureInfo.px,
				MascotsTextureInfo.py,
				MascotsTextureInfo.pw,
				MascotsTextureInfo.ph); 


	CreateCube ( &OutsideCube, OUTSIDE_CUBE, 5000, VISIBLE_FROM_INSIDE,
		3, ONE_TEXTURE_PER_CORNER, outsideTextureAreasList);
#endif


#if 1		
	setRECT( &singleOutsideCubeTexture,
			Wave2TextureInfo.px,
			Wave2TextureInfo.py,
			Wave2TextureInfo.pw,
			Wave2TextureInfo.ph);

	CreateCube ( &OutsideCube, OUTSIDE_CUBE, 5000, VISIBLE_FROM_INSIDE, 
		3, SINGLE_CUBE_TEXTURE, &singleOutsideCubeTexture);
#endif

	//printf("End of InitialiseCubes\n");

	//printf("INSIDE cube\n");
	//PrintCubeInfo (&InsideCube);
	//printf("OUTSIDE cube\n");
	//PrintCubeInfo (&OutsideCube);
}





void SetUpTheMirror (void)
{
	ObjectHandler *object;
	RECT rect;
	int tPageID, baseX, baseY;
	TMD_VERT vert1, vert2, vert3, vert4;
	TMD_NORM normal1;
	u_char u0, v0, u1, v1, u2, v2, u3, v3;
	int sideLength = 2000;
	int halfSide = sideLength / 2;



		// create a mirror polygon whose texture is an off-screen
		// area used just for mirror drawing
	setRECT( &rect, 640, 256, 256, 256);

	GetTexturePageOfRectangle (&rect, &tPageID, &baseX, &baseY);

	/****
	u0 = baseX;				v0 = baseY;
	u1 = baseX + rect.w-1;	v1 = baseY;
	u2 = baseX;				v2 = baseY + rect.h-1;
	u3 = baseX + rect.w-1;	v3 = baseY + rect.h-1;
	****/
	#if 0		// without left-right flipping
	u0 = 0; v0 = 0;
	u1 = 255; v1 = 0;
	u2 = 0; v2 = 255;
	u3 = 255; v3 = 255;
	#endif
	u1 = 0; v1 = 0;
	u0 = 255; v0 = 0;
	u3 = 0; v3 = 255;
	u2 = 255; v2 = 255;

	setNORMAL( &normal1, 0, 0, -ONE);

	setVERTEX( &vert1, -halfSide, -halfSide, 0);
	setVERTEX( &vert2, halfSide, -halfSide, 0);
	setVERTEX( &vert3, -halfSide, halfSide, 0);
	setVERTEX( &vert4, halfSide, halfSide, 0);

	MirrorPolygonAddress = CreateAnotherPolygonTMD (0, 0, 
						tPageID, SIXTEEN_BIT_MODE, 
						 u0, v0,
						 u1, v1, 
						 u2, v2, 
						 u3, v3,
						 &normal1, 
						 &vert1, &vert2, &vert3, &vert4);



	 	// creating and positioning the mirror
	object = &TheMirror;
	object->alive = TRUE;
	//setVECTOR( &object->position, 0, 0, 800);
	UpdateObjectCoordinates (&object->twist,
							&object->position, &object->velocity,
							&object->coord, &object->matrix);
	LinkObjectHandlerToSingleTMD (&object->handler, 
							MirrorPolygonAddress);

	MirrorBasePlane = X_Y_PLANE;
	setVECTOR( &MirrorBaseNormal, 0, 0, -ONE);

}




void CleanupProgram (void)
{
	//StoreScreen2 ( (u_long*)SCREEN_SAVE_ADDRESS, 0, 0, ScreenWidth, ScreenHeight);
	StoreScreen();

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

	cyclePoint = (frameNumber % lightEffectPeriod);

	theta = ONE * cyclePoint / lightEffectPeriod;

		// not very sorted, but will do
	TheLights[0].vx	= rcos(theta);
	TheLights[0].vy	= rsin(theta);
	GsSetFlatLight(0, &TheLights[0]);

	TheLights[1].vz	= rcos(theta);
	TheLights[1].vx	= rsin(theta);
	GsSetFlatLight(1, &TheLights[1]);

	TheLights[2].vy	= rcos(theta);
	TheLights[2].vz	= rsin(theta);
	GsSetFlatLight(2, &TheLights[2]);
}



   



void DealWithControllerPad (void)
{
	long pad;
	int controlSpeed;
	int pause = 20;
	static int framesSinceLastModeToggle = 0;

	pad = PadRead();

		// <start> and <select> to quit
	if (pad & PADselect && pad & PADstart)
		{
		QuitFlag = TRUE;
		return;
		}

	framesSinceLastModeToggle++;


		// pause while start held down
	if (pad & PADstart)
		{
		while (pad & PADstart)
			{
			pad = PadRead();
			VSync(0);
			}
		}

		// sort how fast controls act
	if (pad & PADselect)
		controlSpeed = 10;
	else
		controlSpeed = 1;


		// Lup/Ldown: change main mode (move view or ship)
	if (framesSinceLastModeToggle > pause)
		{
		if ((pad & PADLup) || (pad & PADLdown))
			{
			if (MainMode == MOVE_VIEW)
				MainMode = MOVE_SHIP;
			else
				{
				assert(MainMode == MOVE_SHIP);
				MainMode = MOVE_VIEW;
				}
			framesSinceLastModeToggle = 0;
			}
		}




	if (MainMode == MOVE_VIEW)
		{
			// Rpad: move viewing point in GsRVIEW2
		if (pad & PADRleft)
			{
			TheView.vpx -= 20 * controlSpeed;
			}
		if (pad & PADRright)
			{
			TheView.vpx += 20 * controlSpeed;
			}
		if (pad & PADRup)
			{
			TheView.vpy -= 20 * controlSpeed;
			}
		if (pad & PADRdown)
			{
			TheView.vpy += 20 * controlSpeed;
			}
		if (pad & PADR1)
			{
			TheView.vpz -= 20 * controlSpeed;
			}
		if (pad & PADR2)
			{
			TheView.vpz += 20 * controlSpeed;
			}
		}
	else
		{
		assert(MainMode == MOVE_SHIP);

			// Rpad: the ship controls: two moves, six rotations
		if (pad & PADRleft)
			{
			setVECTOR( &TheShip.twist, 0, -12 * controlSpeed, 0);			
			}
		if (pad & PADRright)
			{
			setVECTOR( &TheShip.twist, 0, 12 * controlSpeed, 0);
			}
		if (pad & PADRup)
			{
			setVECTOR( &TheShip.twist, -12 * controlSpeed, 0, 0);
			}
		if (pad & PADRdown)
			{
			setVECTOR( &TheShip.twist, 12 * controlSpeed, 0, 0);
			}
		if (pad & PADR1)
			{
			setVECTOR( &TheShip.twist, 0, 0, -12 * controlSpeed);
			}
		if (pad & PADR2)
			{
			setVECTOR( &TheShip.twist, 0, 0, 12 * controlSpeed);
			}
		if (pad & PADL1)	   // forwards
			{
			setVECTOR( &TheShip.velocity, 0, 0, 15 * controlSpeed);
			}
		if (pad & PADL2)		 // backwards
			{
			setVECTOR( &TheShip.velocity, 0, 0, -15 * controlSpeed);
			}
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
					case MIRROR:
						HandleAMirror(object);
						break;
					case SHIP:
						HandleAShip(object);
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




void HandleAMirror (ObjectHandler* object)
{
	assert(object->type == MIRROR);

	UpdateObjectCoordinates (&object->twist,
							&object->position, &object->velocity,
							&object->coord, &object->matrix);

	if (object->movementMomentumFlag == FALSE)
		{
		setVECTOR( &object->velocity, 0, 0, 0);
		}
	if (object->rotationMomentumFlag == FALSE)
		{
		setVECTOR( &object->twist, 0, 0, 0);
		}
}



void HandleAShip (ObjectHandler* object)
{
	assert(object->type == SHIP);

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
		  










void InitCube (Cube *cube)
{
	int i;

	cube->id = -1;
	cube->alive = FALSE;
	cube->sideLength = -1;
	cube->sideFlag = -1;
	cube->polysPerSide = -1;
	cube->textureType = -1;

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
				   int polysPerSide, int type, RECT *rectList)
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

#if 0
	{
	int i;
	printf("Normals :-\n\n");
	for (i = 0; i < 6; i++)
		{
		dumpVECTOR( &(cube->normals[i]) );
		}
	}
#endif

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
				//GsGetLs(&(polygon->coord), &tmpls);
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





void StartViewMove (int viewMove)
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






void HandleTheView (void)
{	



#if 0			// OLD
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
			assert(FALSE);
		}

	RotateCoordinateSystem (&twist,
							&ViewCoords);

#endif
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
			assert(FALSE);
		}

	CopyCoordinateSystem( &PreviousViewCoords, &properCoordResult);
	RotateCoordinateSystem (&totalTwist,
							&properCoordResult);
							
	CopyCoordinateSystem( &properCoordResult, &ViewCoords);	

	SortPlaneAndAngleViewed();
}





void SortPlaneAndAngleViewed (void)
{
	int value;
	int i, j;
	VECTOR zAxis;
	VECTOR xAxis;
	VECTOR *planeXaxis, *planeYaxis, planeMinusXaxis, planeMinusYaxis;
	int plane;
	int angle;
	
	for (i = 0; i < 3; i++)
		{
		for (j = 0; j < 3; j++)
			{
			value = ViewCoords.coord.m[i][j];
			if (value != 0 && value != ONE && value != -ONE)
				{
				CubeSurfaceViewed = -1;
				OrientationOntoSurface = -1;
				return;
				}
			}
		}

	setVECTOR( &zAxis, 
			ViewCoords.coord.m[0][2],
			ViewCoords.coord.m[1][2],
			ViewCoords.coord.m[2][2]);

	plane = -1;
	for (i = 0; i < 6; i++)
		{
		if (SameVector( &zAxis, &FaceInsideNormals[i]) == TRUE)
			{
			plane = i;
			break;
			} 
		}

	if (plane == -1)
		{
		printf("PLANE IS -1\n");
		printf("zAxis: ");dumpVECTOR( &zAxis);
		assert(FALSE);
		}
	else
		{
		CubeSurfaceViewed = plane;

		setVECTOR( &xAxis, 
			ViewCoords.coord.m[0][0],
			ViewCoords.coord.m[1][0],
			ViewCoords.coord.m[2][0]);

		planeXaxis = &FaceXVectors[plane];
		planeYaxis = &FaceYVectors[plane];

		if (SameVector( &xAxis, planeXaxis) == TRUE)
			{
			angle = 0;
			}
		else if (SameVector( &xAxis, planeYaxis) == TRUE)
			{
			angle = 1024;
			} 
		else
			{
			setVECTOR( &planeMinusXaxis, -planeXaxis->vx,
				-planeXaxis->vy, -planeXaxis->vz);
			if (SameVector( &xAxis, &planeMinusXaxis) == TRUE)
				{
				angle = 2048;
				}
			else
				{
				setVECTOR( &planeMinusYaxis, -planeYaxis->vx,
				-planeYaxis->vy, -planeYaxis->vz);
				if (SameVector( &xAxis, &planeMinusYaxis) == TRUE)
					{
					angle = 3072;
					}
				else
					assert(FALSE);
				}
			}

		OrientationOntoSurface = angle;
		}
}






int SameVector (VECTOR *first, VECTOR *second)
{
	int boolean = FALSE;

	if ((first->vx == second->vx)
		&& (first->vy == second->vy)
		&& (first->vz == second->vz))
			{
			boolean = TRUE;
			}

	return boolean;
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

	setRECT( &clipArea, 640, 256, 
		256, 256);
	offsets[0] = 640 + 128;
	offsets[1] = 256 + 128;
	//dumpRECT( &clipArea);
	//printf("offsets %d and %d\n\n", offsets[0], offsets[1]);
	CreateDrawProcess( &FirstDrawProcess, FIRST,
		&clipArea, offsets, FIRST, FIRST);
	RegisterDrawProcess( &FirstDrawProcess);


#if 0				// OLD
	setRECT( &clipArea, MascotsTextureInfo.px, MascotsTextureInfo.py, 
		MascotsTextureInfo.pw, MascotsTextureInfo.ph);
	offsets[0] = MascotsTextureInfo.px;
	offsets[1] = MascotsTextureInfo.py;
	dumpRECT( &clipArea);
	printf("offsets %d and %d\n\n", offsets[0], offsets[1]);
	CreateDrawProcess( &SecondDrawProcess, SECOND,
		&clipArea, offsets, SECOND, SECOND);
	RegisterDrawProcess( &SecondDrawProcess);


   	setRECT( &clipArea, Wave2TextureInfo.px, Wave2TextureInfo.py, 
		Wave2TextureInfo.pw, Wave2TextureInfo.ph);
	offsets[0] = Wave2TextureInfo.px;
	offsets[1] = Wave2TextureInfo.py;
	dumpRECT( &clipArea);
	printf("offsets %d and %d\n\n", offsets[0], offsets[1]);
	CreateDrawProcess( &ThirdDrawProcess, THIRD,
		&clipArea, offsets, THIRD, THIRD);
	RegisterDrawProcess( &ThirdDrawProcess);


	setRECT( &clipArea, Wave3TextureInfo.px, Wave3TextureInfo.py, 
		Wave3TextureInfo.pw, Wave3TextureInfo.ph);
	offsets[0] = Wave3TextureInfo.px;
	offsets[1] = Wave3TextureInfo.py;
	dumpRECT( &clipArea);
	printf("offsets %d and %d\n\n", offsets[0], offsets[1]);
	CreateDrawProcess( &FourthDrawProcess, FOURTH,
		&clipArea, offsets, FOURTH, FOURTH);
	RegisterDrawProcess( &FourthDrawProcess);


	setRECT( &clipArea, Wave4TextureInfo.px, Wave4TextureInfo.py, 
		Wave4TextureInfo.pw, Wave4TextureInfo.ph);
	offsets[0] = Wave4TextureInfo.px;
	offsets[1] = Wave4TextureInfo.py;
	dumpRECT( &clipArea);
	printf("offsets %d and %d\n\n", offsets[0], offsets[1]);
	CreateDrawProcess( &FifthDrawProcess, FIFTH,
		&clipArea, offsets, FIFTH, FIFTH);
	RegisterDrawProcess( &FifthDrawProcess);




	setRECT( &clipArea, Wave5TextureInfo.px, Wave5TextureInfo.py, 
		Wave5TextureInfo.pw, Wave5TextureInfo.ph);
	offsets[0] = Wave5TextureInfo.px;
	offsets[1] = Wave5TextureInfo.py;	  
	dumpRECT( &clipArea);
	printf("offsets %d and %d\n\n", offsets[0], offsets[1]);
	CreateDrawProcess( &SixthDrawProcess, SIXTH,
		&clipArea, offsets, SIXTH, SIXTH);
	RegisterDrawProcess( &SixthDrawProcess);
#endif
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

#if 0
	if (frameNumber % 15 == 0)
		{
		printf("previous business :-\n");
		dumpRECT( &previousClip);
		printf("previous offsets %d, %d\n\n\n", 
			previousOffsets[0], previousOffsets[1]);
		}
#endif

		// set new drawing environment
	setRECT( &GsDRAWENV.clip, 
			process->clipArea.x,
			process->clipArea.y,
			process->clipArea.w,
			process->clipArea.h);
	GsDRAWENV.ofs[0] = process->offsets[0];
	GsDRAWENV.ofs[1] = process->offsets[1];
	PutDrawEnv( &GsDRAWENV);

#if 0
	if (frameNumber % 15 == 0)
		{
		printf("newly set business :-\n");
		dumpRECT( &GsDRAWENV.clip);
		printf("new offsets %d, %d\n\n\n", 
			GsDRAWENV.ofs[0], GsDRAWENV.ofs[1]);
		}
#endif

		// get the right OT
	GetOtOfProcess(process, bufferIndex, &ot);
	GsClearOt(0, 0, ot);

		// get the right packet area, use as work base for GPU
	GetWorkAreaOfProcess(process, bufferIndex, &packetArea);
	GsSetWorkBase(packetArea);

	//GsSortClear(0, 0, 0, ot);

		// actually register draw requests into OT
	DoWorkOfDrawProcess( process->id, ot);

	GsDrawOt(ot);

		// restore previous settings
	setRECT( &GsDRAWENV.clip, previousClip.x, previousClip.y,
			previousClip.w, previousClip.h);	
	GsDRAWENV.ofs[0] = previousOffsets[0];	
	GsDRAWENV.ofs[1] = previousOffsets[1];
	PutDrawEnv(&GsDRAWENV);

#if 0
	if (frameNumber % 15 == 0)
		{
		printf("newly restored business :-\n");
		dumpRECT( &GsDRAWENV.clip);
		printf("newly restored offsets %d, %d\n\n\n", 
			GsDRAWENV.ofs[0], GsDRAWENV.ofs[1]);
		}
#endif


#if 0
	if (frameNumber % 15 == 0)
		{
		printf("ot: %d\n", (int)ot);
		printf("wot[0] %d\n", (int) &Wot[0]);
		printf("wot[1] %d\n", (int) &Wot[1]);
		printf("wot2[0] %d\n", (int) &Wot2[0]);
		printf("wot2[1] %d\n", (int) &Wot2[1]);
		}
#endif
}





void GetOtOfProcess (DrawProcess *process, int bufferIndex, GsOT **ot)
{
	switch(process->otLabel)
		{
		case FIRST:
			*ot = &Wot2[bufferIndex];			
			break;
		case SECOND:
			assert(FALSE);		// not valid here			
			break;	
		case THIRD:
			assert(FALSE);		// not valid here			
			break;
		case FOURTH:
			assert(FALSE);		// not valid here			
			break;
		case FIFTH:
			assert(FALSE);		// not valid here			
			break;
		case SIXTH:
			assert(FALSE);		// not valid here			
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
			assert(FALSE);		// not valid here
			break;
		case THIRD:
			assert(FALSE);		// not valid here
			break;
		case FOURTH:
			assert(FALSE);		// not valid here
			break;
		case FIFTH:
			assert(FALSE);		// not valid here
			break;
		case SIXTH:						  
			assert(FALSE);		// not valid here
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
			//GsSortSprite( &FireSprite, ot, 0);
			DrawMirrorViewOfWorld(ot);
			break;
	    case SECOND:
			assert(FALSE);		// not valid here
			break;
		case THIRD:
			assert(FALSE);		// not valid here
			break;
		case FOURTH:
			assert(FALSE);		// not valid here
			break;
		case FIFTH:
			assert(FALSE);		// not valid here
			break;
		case SIXTH:
			assert(FALSE);		// not valid here
			break;
		default:	
			printf("bad value %d\n", processID);
			assert(FALSE);
		}
}





void DrawMirrorViewOfWorld (GsOT *ot)
{
	VECTOR mirrorCentre;
	VECTOR viewPoint;
	VECTOR viewPointToMirrorCentre;
	VECTOR mirrorNormal;
	int dotProduct;
	VECTOR parallel, perpendicular;
	VECTOR mirrorViewPoint;
	VECTOR previousViewPoint;
	MATRIX tmpls;
	ObjectHandler *object;
	int i;
	u_short zValue;

	setVECTOR( &mirrorCentre, 
			TheMirror.coord.coord.t[0],
			TheMirror.coord.coord.t[1],
			TheMirror.coord.coord.t[2]);

	GetPositionOfViewpoint( &viewPoint);

	setVECTOR( &viewPointToMirrorCentre,
			mirrorCentre.vx - viewPoint.vx,
			mirrorCentre.vy - viewPoint.vy,
			mirrorCentre.vz - viewPoint.vz);

	ExpressSubPointInSuper( &MirrorBaseNormal,
			&TheMirror.coord, &mirrorNormal);

	dotProduct = (mirrorNormal.vx * viewPointToMirrorCentre.vx)
				+ (mirrorNormal.vy * viewPointToMirrorCentre.vy)
				+ (mirrorNormal.vz * viewPointToMirrorCentre.vz);

#if 0
	if (frameNumber % 20 == 0)
		{
		printf("mirrorCentre: "); dumpVECTOR( &mirrorCentre);
		printf("viewPoint: "); dumpVECTOR( &viewPoint);
		printf("viewPointToMirrorCentre: "); dumpVECTOR( &viewPointToMirrorCentre);
		printf("mirrorNormal: "); dumpVECTOR( &mirrorNormal);
		printf("dot product %d\n", dotProduct);
		}
#endif

	if (dotProduct >= 0)
		{
		//if (frameNumber % 20 == 0)
		//	{
		//	printf("Not displaying mirror: back-facing\n");
		//	}
		return;
		}
	else		// find components of vector w.r.t mirror plane
		{
		ResolveVector( &viewPointToMirrorCentre, &mirrorNormal,
						&parallel, &perpendicular);

		setVECTOR( &mirrorViewPoint,
				viewPoint.vx + (2 * parallel.vx),			 // 2 * perp.
				viewPoint.vy + (2 * parallel.vy),
				viewPoint.vz + (2 * parallel.vz) );

		/***			 // alternative
		setVECTOR( &mirrorViewPoint,
			mirrorCentre.vx + parallel.vx - perpendicular.vx,
			mirrorCentre.vy + parallel.vy - perpendicular.vy,
			mirrorCentre.vz + parallel.vz - perpendicular.vz);
		***/

			// store previous view point
		setVECTOR( &previousViewPoint,
				TheView.vpx, TheView.vpy, TheView.vpz);

			// set the new view
		assert(TheView.super == WORLD);
		TheView.vpx = mirrorViewPoint.vx;
		TheView.vpy = mirrorViewPoint.vy;
		TheView.vpz = mirrorViewPoint.vz;
		GsSetRefView2( &TheView);

		#if 0
		if (frameNumber % 20 == 0)
			{
			printf("parallel: "); dumpVECTOR( &parallel);
			printf("perpendicular: "); dumpVECTOR( &perpendicular);
			printf("mirrorViewPoint: "); dumpVECTOR( &mirrorViewPoint);
			}
		#endif

			// draw all things into the OT: ie mirror's view-render on world	
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
								ot, 
									3, getScratchAddr(0));
						}
					else if (object->displayFlag == SPRITE)	  // speed-up here: use a fast flag
						{	  // some can use GsSortFastSprite
						zValue = SortSpriteObjectPosition(object);
						GsSortSprite( &object->sprite, ot, zValue);
						}
					else
						{
						assert(FALSE);
						}
					}
			}

		DrawCube( &InsideCube, ot);
		DrawCube( &OutsideCube, ot);

			// restore the previous view
		TheView.vpx = previousViewPoint.vx;
		TheView.vpy = previousViewPoint.vy;
		TheView.vpz = previousViewPoint.vz;
		GsSetRefView2( &TheView);
		}
}

	  



void GetPositionOfViewpoint (VECTOR *output)
{
	VECTOR relativePosition;

	if (TheView.super == WORLD)
		{
		setVECTOR( output, 
				TheView.vpx, TheView.vpy, TheView.vpz);		
		}
	else
		{
		setVECTOR( &relativePosition, 
				TheView.vpx, TheView.vpy, TheView.vpz);
		ExpressSubPointInSuper( &relativePosition, 
			TheView.super, output);
		}
}




	   

	// resolve a vector into two components, by a specified resolver; 
	// one component will be perpendicular to the resolver,
	// the other will be parallel
void ResolveVector (VECTOR* original, VECTOR* resolver,
						VECTOR* parallelPart, VECTOR* orthogonalPart)
{
	int sizeOfResolverSquared;
	int dotProduct;
	int smallest, scaleFactor;

	//printf("\n ResolveVector \n");
	//printf("original, resolver\n");
	//dumpVECTOR(original);
   	//dumpVECTOR(resolver);

	sizeOfResolverSquared = (resolver->vx * resolver->vx)
							+ (resolver->vy * resolver->vy)
							+ (resolver->vz * resolver->vz);
	assert(sizeOfResolverSquared > 0);
	//printf("sizeOfResolverSquared: %d\n", sizeOfResolverSquared); 
		
   	dotProduct = (original->vx * resolver->vx)
				+ (original->vy * resolver->vy)
				+ (original->vz * resolver->vz); 
	//printf("dotProduct: %d\n", dotProduct);

		// scaling to prevent integer overflow
	if (abs(dotProduct) > abs(sizeOfResolverSquared))
		smallest = abs(sizeOfResolverSquared);
	else
		smallest = abs(dotProduct);

	if (smallest > 1000000)
		scaleFactor = 200000;
	else if (smallest > 100000)
		scaleFactor = 20000;
	else if (smallest > 10000)
		scaleFactor = 2000;
	else if (smallest > 1000)
		scaleFactor = 200;
	else if (smallest > 100)
		scaleFactor = 20;
	else if (smallest > 10)
		scaleFactor = 2;
	else
		scaleFactor = 1;
		

	dotProduct /= scaleFactor;
	sizeOfResolverSquared /= scaleFactor;

	//printf("AFTER SCALE: sizeOfResolverSquared: %d\n", sizeOfResolverSquared);  
	//printf("dotProduct: %d\n", dotProduct);

	setVECTOR(parallelPart,
			((resolver->vx * dotProduct) / sizeOfResolverSquared),
			((resolver->vy * dotProduct) / sizeOfResolverSquared),
			((resolver->vz * dotProduct) / sizeOfResolverSquared) );

	setVECTOR(orthogonalPart, 
				original->vx - parallelPart->vx,
				original->vy - parallelPart->vy,
				original->vz - parallelPart->vz);
	//printf("parallelPart, orthogonalPart\n");
	//dumpVECTOR(parallelPart);
	//dumpVECTOR(orthogonalPart);
}