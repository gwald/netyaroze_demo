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

GsIMAGE WaveTextureInfo;
GsSPRITE WaveSprite;

GsIMAGE GreenCircleTextureInfo;
GsSPRITE GreenCircleSprite;

GsIMAGE StarsTextureInfo;
GsSPRITE StarsSprite;

GsIMAGE PurpleNoiseTextureInfo;
GsSPRITE PurpleNoiseSprite;

GsIMAGE OrangeTileTextureInfo;
GsSPRITE OrangeTileSprite;

GsIMAGE MultipleTextureInfo;
GsSPRITE MultipleSprite;





int ScreenWidth = 320;		 
int ScreenHeight = 240;



#define PACKETMAX	2048		/* Max GPU packets */
#define PACKETMAX2	(PACKETMAX*24)

static PACKET packetArea[2][PACKETMAX2]; /* GPU PACKETS AREA */
  

#define OT_LENGTH	9		

static GsOT Wot[2];			/* Handler of OT */
static GsOT_TAG wtags[2][1<<OT_LENGTH];



long ProjectionDistance;

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




	// dynamic TMD management
#define SIZEOF_SINGLE_POLYGON_TMD 0x80

#define START_OF_CREATED_TMDS_STACK 0x800f0000
#define END_OF_CREATED_TMDS_STACK 0x80110000

int NumberOfCreatedTMDs;

u_long CurrentTMDStackAddress;



/****************************************************************************
					prototypes
****************************************************************************/


void main (void);

void InitialiseAll (void);

void InitialiseLighting (void);
void InitialiseView (void);

void InitialiseObjects (void);

void CreateMoreLittleCubesAroundLargerOne (void);

void InitialiseCubes (void);

void CleanupAndExit (void);

void DealWithControllerPad (void);


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
					int polysPerSide, int type, RECT *rectList);


void SortCubeTextureAreas (Cube *cube, int type, RECT *rectList);


void CreateTheCubesPolygons (Cube *cube);



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
	int bufferIndex;			  // buffer index
	u_short zValue;
	MATRIX tmpls;
	ObjectHandler *object;
	VECTOR rvp, vp, rvr, vr;
	int i;
	
	InitialiseAll();

	bufferIndex = GsGetActiveBuff();

	while (QuitFlag == FALSE)
		{		
		FntPrint("~cf00frame %d\n", frameNumber);
		FntPrint("~cf00hsync %d\n", hsync);
		FntPrint("~cf00projection %d\n", ProjectionDistance);
		FntPrint("~cf00vp %d %d %d\n", TheView.vpx, TheView.vpy, TheView.vpz);
		FntPrint("~cf00ViewMoveTime %d\n", ViewMoveTime);
		FntPrint("~cf00ViewDistanceFromOrigin %d\n", ViewDistanceFromOrigin);

			// find and print vp and vr in world coordinate space
		setVECTOR( &rvp, TheView.vpx, TheView.vpy, TheView.vpz);
		//dumpVECTOR( &rvp);
		ExpressSuperPointInSub( &rvp, TheView.super, &vp);
		FntPrint("~cf00viewpoint in world %d %d %d\n",
			vp.vx, vp.vy, vp.vz);

		setVECTOR( &rvr, TheView.vrx, TheView.vry, TheView.vrz);
		//dumpVECTOR( &rvr);
		ExpressSuperPointInSub( &rvr, TheView.super, &vr);
		FntPrint("~cf00view reference in world %d %d %d\n",
			vr.vx, vr.vy, vr.vz);
		//printf("\n\n\n");




		/******
		RegisterTextStringForDisplay("right pad to move cube", -150, -100);
		RegisterTextStringForDisplay("right pad plus START to rotate cube", -150, -90);
		RegisterTextStringForDisplay("right pad plus Lleft to scale cube", -150, -80);
		RegisterTextStringForDisplay("right pad plus L1 changes projection", -150, -70);
		RegisterTextStringForDisplay("right pad plus L2 to move viewpoint", -150, -60);
		RegisterTextStringForDisplay("select makes controls act quicker", -150, -50);
		RegisterTextStringForDisplay("start and select to quit", -150, -40);			
		******/

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
		GsSortClear(0,0,4,&Wot[bufferIndex]);
		GsDrawOt(&Wot[bufferIndex]);
		bufferIndex = GsGetActiveBuff();
		FntFlush(-1);
		}

	CleanupAndExit();
}






void InitialiseAll (void)
{
	PadInit();

	InitialiseRandomNumbers();

	InitialiseTextStrings();

	InitialiseSound();	   

	GsInitGraph(ScreenWidth, ScreenHeight, GsINTER|GsOFSGPU, 1, 0);
	GsDefDispBuff(0,0,0,ScreenHeight);

	GsInit3D();		   

		// sort our only ordering table
	Wot[0].length=OT_LENGTH;	
	Wot[0].org=wtags[0];	   
	Wot[1].length=OT_LENGTH;
	Wot[1].org=wtags[1];

	GsClearOt(0,0,&Wot[0]);
	GsClearOt(0,0,&Wot[1]);

	InitialiseLighting();

	InitialiseView();

	ProperInitialiseTexture(FIRE_TEXTURE_ADDRESS, &FireTextureInfo);
	LinkSpriteToImageInfo(&FireSprite, &FireTextureInfo);

	ProperInitialiseTexture(MASCOTS_TEXTURE_ADDRESS, &MascotsTextureInfo);
	LinkSpriteToImageInfo(&MascotsSprite, &MascotsTextureInfo);

	ProperInitialiseTexture(WAVE_TEXTURE_ADDRESS, &WaveTextureInfo);
	LinkSpriteToImageInfo(&WaveSprite, &WaveTextureInfo);

	ProperInitialiseTexture(GREEN_CIRCLE_TEXTURE_ADDRESS, &GreenCircleTextureInfo);
	LinkSpriteToImageInfo(&GreenCircleSprite, &GreenCircleTextureInfo);

	ProperInitialiseTexture(STARS_TEXTURE_ADDRESS, &StarsTextureInfo);
	LinkSpriteToImageInfo(&StarsSprite, &StarsTextureInfo);

	ProperInitialiseTexture(PURPLE_NOISE_TEXTURE_ADDRESS, &PurpleNoiseTextureInfo);
	LinkSpriteToImageInfo(&PurpleNoiseSprite, &PurpleNoiseTextureInfo);

	ProperInitialiseTexture(ORANGE_TILE_TEXTURE_ADDRESS, &OrangeTileTextureInfo);
	LinkSpriteToImageInfo(&OrangeTileSprite, &OrangeTileTextureInfo);

	ProperInitialiseTexture(MULTIPLE_TEXTURE_ADDRESS, &MultipleTextureInfo);
	LinkSpriteToImageInfo(&MultipleSprite, &MultipleTextureInfo);

		 		
		// sort basic text printing
	FntLoad( 960, 256);
	FntOpen( -120, 30, 256, 200, 0, 512);

	InitialiseObjects();

	ClearTheCreatedTmds();

	InitialiseCubes();
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
	ViewDistanceFromOrigin = 1000;  

	TheView.vrx = 0; 
	TheView.vry = 0; 
	TheView.vrz = 0; 

	TheView.vpx = 0; 
	TheView.vpy = 0; 
	TheView.vpz = -ViewDistanceFromOrigin; //-220;
	
	TheView.rz = 0;

	GsInitCoordinate2( WORLD, &ViewCoords);
	CopyCoordinateSystem( &ViewCoords, &PreviousViewCoords);

	TheView.super = &ViewCoords;

	GsSetRefView2(&TheView);
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
	RECT insideTextureAreasList[6];
	RECT singleInsideCubeTexture;
	//RECT outsideTextureAreasList[6];
	//RECT singleOutsideCubeTexture;
	int i;

	printf("Start of InitialiseCubes\n");

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
		1, ONE_TEXTURE_PER_CORNER, insideTextureAreasList);
	

   




#if 0		// outside cube not ready
	setRECT( &singleOutsideCubeTexture,
			WaveTextureInfo.px,
			WaveTextureInfo.py,
			WaveTextureInfo.pw,
			WaveTextureInfo.ph);
	CreateCube ( &OutsideCube, OUTSIDE_CUBE, 25000, VISIBLE_FROM_INSIDE, 1,
			SINGLE_CUBE_TEXTURE, &singleOutsideCubeTexture);
#endif

	printf("End of InitialiseCubes\n");

	printf("INSIDE cube\n");
	PrintCubeInfo (&InsideCube);
	//printf("OUTSIDE cube\n");
	//PrintCubeInfo (&OutsideCube);
}








void CleanupAndExit (void)
{
	//StoreScreen2 ( (u_long*)SCREEN_SAVE_ADDRESS, 0, 0, ScreenWidth, ScreenHeight);
	StoreScreen();

	ResetGraph(3);

	CleanUpSound();

		// if this program part of a multiple module,
		// can printf to siocons to tell user to
		// invoke a new batch file, etc
}





void DealWithControllerPad (void)
{
	long pad;
	int controlSpeed;

	pad = PadRead();

		// <start> and <select> to quit
	if (pad & PADselect && pad & PADstart)
		{
		QuitFlag = TRUE;
		return;
		}

		// sort how fast controls act
	if (pad & PADselect)
		controlSpeed = 10;
	else
		controlSpeed = 1;


		// L1 modifier: use Rpad to change projection distance
	if (pad & PADL1)
		{
		if (pad & PADRup)
			{
			ProjectionDistance += controlSpeed;
			if (ProjectionDistance > 5000)
				ProjectionDistance = 5000;
			GsSetProjection(ProjectionDistance);
			}
		if (pad & PADRdown)
			{
			ProjectionDistance -= controlSpeed;
			if (ProjectionDistance < 5)
				ProjectionDistance = 5;
			GsSetProjection(ProjectionDistance);
			}
		
		return;
		}


		// L2 modifier: move viewing point in GsRVIEW2
	if (pad & PADL2)
		{
		if (pad & PADRleft)
			{
			TheView.vpx -= controlSpeed;
			}
		if (pad & PADRright)
			{
			TheView.vpx += controlSpeed;
			}
		if (pad & PADRup)
			{
			TheView.vpy -= controlSpeed;
			}
		if (pad & PADRdown)
			{
			TheView.vpy += controlSpeed;
			}
		if (pad & PADR1)
			{
			TheView.vpz -= controlSpeed;
			}
		if (pad & PADR2)
			{
			TheView.vpz += controlSpeed;
			}
		return;
		}

        // Lleft modifier: use Rpad to change ViewMoveTime
	if (pad & PADLleft)
		{
		if (pad & PADRup)
			{
			ViewMoveTime += controlSpeed;
			if (ViewMoveTime > 120)
				ViewMoveTime = 120;
			}
		if (pad & PADRdown)
			{
			ViewMoveTime -= controlSpeed;
			if (ViewMoveTime < 1)
				ViewMoveTime = 1;
			}
		
		return;
		}

        // Lright modifier: use Rpad to change ViewDistanceFromOrigin
	if (pad & PADLright)
		{
		if (pad & PADRup)
			{
			ViewDistanceFromOrigin += 10 * controlSpeed;
			if (ViewDistanceFromOrigin > 25000)
				ViewDistanceFromOrigin = 25000;
			TheView.vpz = -ViewDistanceFromOrigin;
			}
		if (pad & PADRdown)
			{
			ViewDistanceFromOrigin -= 10 * controlSpeed;
			if (ViewDistanceFromOrigin < 200)
				ViewDistanceFromOrigin = 200;
			TheView.vpz = -ViewDistanceFromOrigin;
			}
		
		return;
		}




		// unmodified Rpad: the view controls
	if (ViewMoveFlag == FALSE)
		{
		if (pad & PADRleft)
			{
			StartViewMove(VIEW_TWIST_LEFT);
			}
		if (pad & PADRright)
			{
			StartViewMove(VIEW_TWIST_RIGHT);
			}
		if (pad & PADRup)
			{
			StartViewMove(VIEW_TWIST_UP);
			}
		if (pad & PADRdown)
			{
			StartViewMove(VIEW_TWIST_DOWN);
			}
		if (pad & PADR1)
			{
			StartViewMove(VIEW_TWIST_CLOCKWISE);
			}
		if (pad & PADR2)
			{
			StartViewMove(VIEW_TWIST_ANTICLOCKWISE);
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
	int i;

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

	printf("Normals :-\n\n");
	for (i = 0; i < 6; i++)
		{
		dumpVECTOR( &(cube->normals[i]) );
		}

	CreateTheCubesPolygons(cube);
} 






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
	RECT *rect;
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

				printf("\n\ni %d j %d k %d\n\n", i, j, k);

				//rect = &cube->textureAreas[whichPoly];
				switch(cube->textureType)
					{
					case SINGLE_CUBE_TEXTURE:
						rect = &cube->textureAreas[0];
						break;
					case ONE_TEXTURE_PER_FACE:
						rect = &cube->textureAreas[i];
						break;
					case ONE_TEXTURE_PER_POLYGON:
						rect = &cube->textureAreas[whichPoly];
						break;
					case ONE_TEXTURE_PER_MAIN_AXIS:
						whichOne = i % 3;
						rect = &cube->textureAreas[whichOne];
						break;
					case ONE_TEXTURE_PER_CORNER:	
						if ((i % 2) == 0)
							rect = &cube->textureAreas[0];
						else
							rect = &cube->textureAreas[1];
						break;
					default:
						assert(FALSE);
					}

				printf("\n\nTexture for poly: ");dumpRECT(rect); printf("\n\n");

				GetTexturePageOfRectangle (rect, &tPageID, &baseX, &baseY);

				u0 = baseX;				v0 = baseY;
				u1 = baseX + rect->w-1;	v1 = baseY;
				u2 = baseX;				v2 = baseY + rect->h-1;
				u3 = baseX + rect->w-1;	v3 = baseY + rect->h-1;

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

				cube->polygons[i] = polygon;

				whichPoly++;
				}
			}
		}
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

	printf("gridX %d gridY %d\n\n\n", gridX, gridY);

	switch(whichFace)
		{
		case PLUS_X_Y_PLANE:
			setVECTOR( &centreToPlane, 0, 0, distance/2);
			setVECTOR( &xOnPlane, -distance, 0, 0);
			setVECTOR( &yOnPlane, 0, -distance, 0);
			break;
		case MINUS_X_Y_PLANE:
			setVECTOR( &centreToPlane, 0, 0, -distance/2);
			setVECTOR( &xOnPlane, distance, 0, 0);
			setVECTOR( &yOnPlane, 0, -distance, 0);
			break;
		case PLUS_X_Z_PLANE:
			setVECTOR( &centreToPlane, 0, distance/2, 0);
			setVECTOR( &xOnPlane, distance, 0, 0);
			setVECTOR( &yOnPlane, 0, 0, -distance);
			break;
		case MINUS_X_Z_PLANE:
			setVECTOR( &centreToPlane, 0, -distance/2, 0);
			setVECTOR( &xOnPlane, distance, 0, 0);
			setVECTOR( &yOnPlane, 0, 0, distance);
			break;
		case PLUS_Y_Z_PLANE:
			setVECTOR( &centreToPlane, distance/2, 0, 0);
			setVECTOR( &xOnPlane, 0, 0, distance);
			setVECTOR( &yOnPlane, 0, -distance, 0);
			break;
		case MINUS_Y_Z_PLANE:
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
	dumpVECTOR(&topLeft);
	printf("\n\n");

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

	dumpSVECTOR(v1);
	dumpSVECTOR(v2);
	dumpSVECTOR(v3);
	dumpSVECTOR(v4);
	printf("\n\n");
}







void DrawCube (Cube *cube, GsOT *ot)
{
	int i;
	MATRIX tmpls;
	int numberPolygons;
	MiniObject *polygon;

	if (cube->alive != TRUE)
		return;

#if 0
	if (frameNumber % 15 == 0)
		{
		int count = 0;
		for (i = 0; i < MAX_POLYGONS; i++)
			{
			if (ThePolygons[i].alive == TRUE)
				count++;
			}
		printf("count %d\n", count);
		}
#endif

	numberPolygons = 6 * cube->polysPerSide * cube->polysPerSide;

	for (i = 0; i < numberPolygons; i++)
		{
		polygon = cube->polygons[i];

		if (polygon->alive == TRUE)
			{
			/****
			if (frameNumber % 15 == 0)
				{
				printf("drawing polynow\n");
				}
			****/

			GsGetLs(&(polygon->coord), &tmpls);	   
									   
			GsSetLightMatrix(&tmpls);
										
			GsSetLsMatrix(&tmpls);
										
			GsSortObject4( &(polygon->handler), 
					ot, 3, getScratchAddr(0));
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
		GsLinkObject4( (u_long)pointer, &ThePolygons[i].handler, 0);
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
}
