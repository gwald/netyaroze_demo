/*
 * Sample for Net Yaroze
 * This sample shows object movement and camera control in a 3D world.
 *
 * Copyright (C) 1993-1997 by Sony Computer Entertainment of America, Inc.
 * All rights Reserved
 * ----------------------------------------------------------------------------
 *
 * The overview of the code architecture is follows :-
 *
 * - Each header file contains its contents as part of an ifdef statement
 *   with a unique macro definition designed for that header file.  This
 *   is done to ensure a single inclusion of the contents even if the file
 *   is accidently included more than once in the same source code file.
 *
 *   For example, LibMain.h contains all the system header file includes and
 *   some generic definitions.  This file should be included in all source
 *   code and header files which make use of any of the system macro defs,
 *   data defs or function templates.  In case you include this file in more
 *   than one header files and they are included in a source code file, there
 *   will be multiple inclusion of the contents of LibMain.h in that source
 *   code file.  The unique ifdef inclusion of contents of each header file
 *   ensures that such a case will never occur.
 *
 *   This policy also ensures us an order free inclusion of various header
 *   files in any source code file or another header file.
 *
 * - The source code is divided into various parts - initialization, object,
 *   math, sound, controller and particle explosion libraries.  The source
 *   code and the data parts of each of these libraries are clearly defined
 *   in their source file and the corresponding header file.  The naming
 *   convension for these library files follows LibName.c and LibName.h.
 *
 * - The interface to these libraries is clearly defined. They have a init
 *   call to initialize them, one or more calls to acess the various
 *   functionalities and a final cleaning up function.  Care has been taken
 *   to keep the data section of each of these libraries independent from
 *   the rest of the source code.  Please look into each of these files to
 *   see the details about them.
 *
 * - Currently, main.c contains the source code for the game play in this
 *   sample.  This file contains function to initialize various objects
 *   in the world and how to handle them.
 *
 * - LibMain.h contains the includes for all system header files and some
 *   generic definitions.  LimMain.h should always be the first file included
 *   in each of the source code files.
 *
 * - LibData.h contains the macros for the memory adresses where various
 *   model, texture and music/sound effect data are loaded.
 *
 * - LibObjc.h contain various data structures and macro definitions which
 *   go with the data structures.  LibObjc.c contains generic functions
 *   which work on the object data structure.
 *
 *   A generic object handler data structure has been defined as part of
 *   LibObjc.h and all objects in the world are handled using this data
 *   structure.  Most of the routines contained in main.c and LibObjc.c
 *   work on this data structure.
 *
 * - This sample code demonstrates use of both GsRVIEW2 and GsVIEW2 way
 *   of handling the viewing point.  Please set the macro USE_GsVIEW2
 *   properly to select either of the viewing point code and data defs.
 *   A number of other global variables are defined here to play around
 *   with the viewing point position and it's inclination with respect
 *   to the object contained in it's super.  These variables can be
 *   easily manipulated using the second controller.  Please check out
 *   function InitView() and HandleView() to see how the view point is
 *   controlled.
 * ----------------------------------------------------------------------------
 */

#include "libmain.h"
#include "libdata.h"
#include "libpad.h"
#include "libmath.h"
#include "libsound.h"
#include "libxplod.h"
#include "libobjc.h"

/*
 * Macros used through out this program
 * ----------------------------------------------------------------------------
 */

/*
 * To use GsVIEW2 or GsRVIEW2
 * This example shows you the use of both GsVIEW2 and GsRVIEW2.
 * To use GsVIEW2 set the following macro to 1.
 */
#define	USE_GsVIEW2		1

/*
 * The following macro is used to conditionally include code to do the
 * reflection of the player object when it hits other objects in the world or
 * the wall.  This code controls the camera movement when the object changes
 * direction and speed after colliding with other objects or a wall in the
 * world.
 */
#define	REFLECT_OBJECT	0

/*
 * Various TMD/TIM data initialization
 * ---------------------------------------------------------------------------
 */

/* Various models and texture maps used throughout */
#define	MODEL_CUBE		0
#define	MODEL_SLAB		1
#define	MODEL_ENMY		2
#define MODEL_BALL		3
#define	MODEL_ROBO		4
#define MODEL_PROP		5
#define	MODEL_MAXNO		6

MODEL		Models[ MODEL_MAXNO ] = {
	{ADR_CUBE_MODEL, ADR_CUBE_TEXTURE , (2048   ),(2048),(2048   ),(4096   )},
	{ADR_SLAB_MODEL, ADR_SLAB_TEXTURE , ( 256   ),(  16),( 256   ),( 512   )},
	{ADR_SHIP_MODEL, ADR_SHIP_TEXTURE , ( 123+ 7),(  20),( 130   ),( 260   )},
	{ADR_BALL_MODEL, ADR_BALL_TEXTURE , (  16   ),(  16),(  16   ),(  32   )},
	{ADR_ROBO_MODEL, ADR_ROBO_TEXTURE1, (  40+40),(  85),(  40+40),(  80+80)},
	{ADR_PROP_MODEL, ADR_ROBO_TEXTURE2, (  80   ),(  85),(  80   ),( 160   )},
	};

/*
 * Ordering table and primitives related variables
 * ----------------------------------------------------------------------------
 */

/*
 * We use double buffereing mechanism to implement the ordering table
 * and the GPU work space.  Please look into LibObjc.h for the DB data
 * structure.  CurrDB provides current buffer pointer to be used for
 * drawing the next frame.
 */
DB			WorldDB[2];
DB			*CurrDB;

/*
 * Lighting mode and background color
 * ----------------------------------------------------------------------------
 */

/* PlayStation supports upto a maximum of three lighting points in a scene */
GsF_LIGHT	LightingPoints[3];

/* Fog parameters for implementing depth cuing */
GsFOGPARAM	FogParameters;

/* The ambient light parameters */
SVECTOR		Ambient;

/*
 * View point related definitions
 * ----------------------------------------------------------------------------
 */

/* View point position offset wrt the robot */
short		ViewPosOffsetX;
short		ViewPosOffsetY;
short		ViewPosOffsetZ;

/* View point reference position in the super coordinate system */
short		ViewRefOffsetX;
short		ViewRefOffsetY;
short		ViewRefOffsetZ;

/* View point angular inclination wrt x,y,z axes of super */
short		ViewAngleOffsetX;
short		ViewAngleOffsetY;
short		ViewAngleOffsetZ;

#if USE_GsVIEW2 >= 1
/* View point structure - a generic view point handler */
GsVIEW2		ViewPoint;

/* View point inclination wrt the x,y,z axes */
SVECTOR		ViewPointAngle;
#else
/* View point structure - a reference point type view point handler */
GsRVIEW2	ViewPoint;
#endif

/* Screen scaling factor wrt x,y,z axes */
SVECTOR		ScreenScaling;

/*
 * Macros and definitions related to various objects in the world
 * ---------------------------------------------------------------------------
 */

/* CUBE/SLAB/ENeMY/BALL/ROBOt/PROPeller object definitions */

#define	CUBE_NUM		6				// Number of faces in a cube
#define	SLAB_NUM		16				// Number of slabs in the world
#define	ENMY_NUM		8				// Number of enemies in the world
#define	BALL_NUM		32				// Number of balls in the world

OBJECT		Cube[ CUBE_NUM ];			// Cube subatance
OBJECT		Slab[ SLAB_NUM ];			// Wooden Slabs substance
OBJECT		Enmy[ ENMY_NUM ];			// Enemy substance
OBJECT		Ball[ BALL_NUM ];			// Number of balls in the world
OBJECT		Robo, Prop;					// Robot and it's propeller substance

/* Definitions for handling object's movement */

/* Macro used for PLAYER object movement */
#define	ROBO_ANGLE_YSTEP	(32)		// Y angle step for robot while truning
#define	ROBO_ANGLE_ZSTEP	(32)		// Z angle step for robot while turning
#define	ROBO_ANGLE_ZMIN		(-256)		// Minimum inclination to z while turning
#define	ROBO_ANGLE_ZMAX		(+256)		// Maximum inclination to z while turning

#define	ROBO_SPEED_YINIT	(48-(20/2))	// Y step when robot starts to fly from a stationary state
#define	ROBO_SPEED_YSTEP	(20)		// Y speed step for robot while flying
#define	ROBO_SPEED_GSTEP	(2)			// Gravity step in y direction for the robot
#define	ROBO_SPEED_YFACTOR	(2)			// Speed in y direction is actually half

#define	ROBO_SPEED_FSTEP	(3)			// Speed step for robot in forward direction
#define	ROBO_SPEED_FMIN		(-48)		// Minimum speed step for robot in forward direction
#define	ROBO_SPEED_FMAX		(+48)		// Maximum speed step for robot in forward direction

/* Macros used for rotating the propeller around it's y axis */
#define	PROP_SPEED_YMAX		(682)		// Maximum angle for object rotation
#define	PROP_SPEED_YSTEP	(512)		// Step angle for object rotation
#define	PROP_SPEED_GSTEP	(26)		// Step angle for object rotation

#define	PROP_PLAYSFX_MIN	(4)			// Minimum # frames after which to play sound effect
#define	PROP_PLAYSFX_MAX	(10)		// Maximum # frames after which to play sound effect

/* Macros used for handling the enemy remains */
#define	BALL_NOPER_ENMY		(4)			// Maximum # balls for each enemy remains

#define	BALL_SPEED_FMIN		(-8)		// Minimum speed in forward direction
#define	BALL_SPEED_FRAND	(17)		// Random factor to decide forward speed

#define	BALL_SPEED_VMIN		(-8)		// Minimum speed in vertical direction
#define	BALL_SPEED_VRAND	(17)		// Random factor to decide vertical speed
#define	BALL_SPEED_GSTEP	(1)			// Gravity on the enemy remains

#define	BALL_ROTATION_MIN	(-128)		// Minimum angular rotation around (x,y,z) axes
#define	BALL_ROTATION_RAND	(256)		// Random factor to decide maximum rotation around (x,y,z) axes

#define	BALL_LIFETIME_MIN	(900)		// Minimum life time of a ball
#define	BALL_LIFETIME_RAND	(901)		// Random factor to decide maximum life time of a ball

/*
 * Other global variables
 * ----------------------------------------------------------------------------
 */

/* Variables to change object's direction after reflection */
#if REFLECT_OBJECT >= 1
short		ObjcNewAngleY = 0;		   	// New direction for the object after reflection
short		ObjcNewAngleYStep = 0;		// Step to change the direction of the object
short		ObjcNewAngleYChange = 0;	// direction changes to new value if set to true
#endif

/* Game status */
short		GameStatus = GAME_PLAY;		// Game status for game play

/* Show debugging info when true - Show/Hide debuggin info from screen */
short		ShowDebug = FALSE;			// Used for debugging info on screen

/* Idle for this much time */
short		IdleTime = 0;				// Idle time for credit's screen

/* Maximum number of enemies per game */
#define	ROBO_MAXLIVES		8			// Robot has 4 lives
#define	ENMY_MAXLIVES		4			// Each enemy has 4 lives

short		EnmyKilled = 0;				// Number of enemies killed so far
   
/* Scoring done here */
#define	SCORE_KILLENEMY		10			// 10 points for killing an enemy
#define	SCORE_EATREMAINS	20			// 20 points for eating each enemy remains

long		CurrentScore = 0;			// Calculate score here

/*
 * Static function templates
 * ----------------------------------------------------------------------------
 */

static void  InitSystem(void);
static void  InitModels(void);
static void  InitLite(void);

static void  InitCube(void);
static void  InitSlab(void);
static void  InitEnmy(void);
static void  InitBall(void);
static void  InitRobo(void);

static void  InitView(void);

static short FindNewObjcPos(OBJECT *objc, short *x,short *y,short *z);

static void  HandleGameOver(void);

static void  HandleRobo(void);
static short HandleRoboCollision(short *dx,short *dy,short *dz);
static void  HandleKillRobo(void);

static void  HandleEnmy(void);
static void  HandleEnmyState(OBJECT *objc);
static short HandleEnmyCollision(OBJECT *objc, short *dx,short *dy,short *dz);
static void  HandleKillEnmy(OBJECT *objc);

static void  HandleBall(void);
static short HandleBallCollision(OBJECT *objc, short *dx,short *dy,short *dz);
static void  HandleKillBall(OBJECT *objc);

static void  HandleLite(void);
static void  HandleView(void);
static void  HandlePolyDivision(void);

static void  DrawScreen(void);
static void  DrawDebug(int vscnt);

/*
 * Initialize the display/controller/graphics/font sub-systems
 * ----------------------------------------------------------------------------
 */

/* Initialize the display/controller/graphics/font sub-systems */

static void InitSystem(void)
{
	short i;
	/* Initialize display mode to given mode - NTSC by default */
	SetVideoMode(MODE_NTSC);
	
	/* Initialize controller */
	PadInit();
	
	/* Initialize environment - drawing and display environment setting */
	GsInitGraph(DISP_BUFF_W, DISP_BUFF_H, (GsOFSGPU | GsNONINTER), 0, 0);
	
	/* Initialize draw/display environment structures */
	GsDefDispBuff(DISP_BUFF1_X, DISP_BUFF1_Y, DISP_BUFF2_X, DISP_BUFF2_Y);
	
	/* Initialize 3D graphics system within the library */
	GsInit3D();
	
	/* Initialize font system - load basic font pattern in frame buffer */
	FntLoad(FONT_BUFF_X, FONT_BUFF_Y);
	
	/* Set the font display window, background flag and character buffer */
	FntOpen(FONT_DISP_X, FONT_DISP_Y, FONT_DISP_W, FONT_DISP_H, FONT_DISP_ISBG, FONT_DISP_NCHARS);
	
	/* Initialize ordering table */
	for (i = 0; i < 2; i++) {
		WorldDB[i].OT.length = OT_LENGTH;
		WorldDB[i].OT.org    = WorldDB[i].OTTags;
		WorldDB[i].OT.offset = 0;
		WorldDB[i].OT.point  = 0;
		WorldDB[i].OT.tag    = NULL;
	}
	CurrDB = NULL;
	
	/* Set the random seed */
	srand(VSync(-1));
	return;
}

/* Initialize all models and load their texture maps into frame memory */

static void InitModels(void)
{
	short i;
	for(i = 0; i < MODEL_MAXNO; i++) {
		if (Models[i].ModelPtr != NULL) {
			/* Map TMD data structures to real addresses
			 * First three longs contain file id, flags and # of objects
			 */
			GsMapModelingData(Models[i].ModelPtr+1);
		}
		if (Models[i].TexturePtr != NULL) {
			unsigned long *tptr;
			GsIMAGE tim;
			RECT rect;
			
			/* Load texture map into memory */
			tptr = Models[i].TexturePtr;
			
			/* Skip id and initialize image structure */
			tptr ++;
			GsGetTimInfo(tptr, &tim);
			
			/* Transfer pixel data to VRAM */
			rect.x = tim.px;
			rect.y = tim.py;
			rect.w = tim.pw;
			rect.h = tim.ph;
			LoadImage(&rect, tim.pixel);
			
			/* If CLUT exists, transfer it to VRAM */
			if ((tim.pmode >> 3) & 0x01) {
				rect.x = tim.cx;
				rect.y = tim.cy;
				rect.w = tim.cw;
				rect.h = tim.ch;
				LoadImage(&rect, tim.clut);
			}
			printf("Texture rect = %d %d %d %d\n", tim.px, tim.py, tim.pw, tim.ph);
			printf("CLUT rect = %d %d %d %d\n", tim.cx, tim.cy, tim.cw, tim.ch);
		}
	}
	return;
}

/*
 * Initialize light sources (light source direction and color)
 * Also set the Fogging parameters and set the lighting mode
 */

static void InitLite(void)
{
	/* Light source #0 */
	LightingPoints[0].vx = 0;
	LightingPoints[0].vy = 0;
	LightingPoints[0].vz = 0;
	
	LightingPoints[0].r  = 0;
	LightingPoints[0].g  = 0;
	LightingPoints[0].b  = 0;
	GsSetFlatLight(0, &LightingPoints[0]);
	
	/* Lighting source #1 */
	LightingPoints[1].vx = 0;
	LightingPoints[1].vy = 0;
	LightingPoints[1].vz = 0;
	
	LightingPoints[1].r  = 0;
	LightingPoints[1].g  = 0;
	LightingPoints[1].b  = 0;
	GsSetFlatLight(1, &LightingPoints[1]);
	
	/* Lighting source #2 */
	LightingPoints[2].vx = 0;
	LightingPoints[2].vy = 0;
	LightingPoints[2].vz = 0;
	
	LightingPoints[2].r  = 0;
	LightingPoints[2].g  = 0;
	LightingPoints[2].b  = 0;
	GsSetFlatLight(2, &LightingPoints[2]);
	
	/* Set ambient (r, g, b - each can have a value between 0 and 4095) */
	Ambient.vx = ONE*3/4;
	Ambient.vy = ONE*3/4;
	Ambient.vz = ONE*3/4;
	GsSetAmbient(Ambient.vx, Ambient.vy, Ambient.vz);
	
	/* Set Fog parameters */
	FogParameters.dqa = -CLP_FAR * ONE / 64 / SCR_Z;
	FogParameters.dqb = (5 * ONE * ONE) / 4;
	FogParameters.rfc = BGCOL_R;
	FogParameters.gfc = BGCOL_G;
	FogParameters.bfc = BGCOL_B;
	GsSetFogParam(&FogParameters);
	
	/* Set light mode - FOG on */
	GsSetLightMode(GsLMODE_FOG);
	return;
}

/*
 * Initialize objects
 * ----------------------------------------------------------------------------
 */

/* Read TMD data from memory and initialize object */

static void InitCube(void)
{
	static short CubeAngle[][2] =
				{{0,0},{0,1024},{0,2048},{0,3072},{1024,0},{3072,0}};
	short i;
	for (i = 0; i < CUBE_NUM; i++) {
		/* Clean up Cube face object structure */
		memset((void *)(&Cube[i]), 0, sizeof(OBJECT));
		
		/* State of the object */
		Cube[i].State = (STATE_DIVISIBLE | STATE_ALIVE | STATE_STATIONARY);
		
		/* Set the type and model for this object */
		Cube[i].Type = MODEL_CUBE;
		Cube[i].Model = &Models[ Cube[i].Type ];		
		
		/* Initialize object inclination */
		Cube[i].Angle.vx = CubeAngle[i][0];
		Cube[i].Angle.vy = CubeAngle[i][1];
		Cube[i].Angle.vz = 0;
		
		/* Calculate the minimum/maximum x/y/z distances of each face
		 * of the binding box depending upon the inclination of the
		 * object wrt the z,y,x axes.
		 */
		Cube[i].DifX = Cube[i].Model->LenX;
		Cube[i].DifY = Cube[i].Model->LenY;
		Cube[i].DifZ = Cube[i].Model->LenZ;
		
		/* Initialize object handling structure.
		 * Only first cube face has super set to WORLD.
		 * All other cube faces have super set to the first
		 * face of the cube which is at index 0 in array
		 */
		InitObjc(&Cube[i], (i ? &Cube[0].Pos : WORLD), 0,0,0);
	}
	return;
}

/* Read TMD data from memory and initialize object */

static void InitSlab(void)
{
	short i,j;
	short x,y,z, temp;
	short cubeedge, slabedge;
	for (i = 0; i < SLAB_NUM; i++) {
		/* Clean up Slab object structure */
		memset((void *)(&Slab[i]), 0, sizeof(OBJECT));
		
		/* State of the object */
		Slab[i].State = (STATE_DIVISIBLE | STATE_ALIVE | STATE_STATIONARY);
		
		/* Set the type and model for this object */
		Slab[i].Type = MODEL_SLAB;
		Slab[i].Model = &Models[ Slab[i].Type ];
		
		/* Initialize object inclination */
		Slab[i].Angle.vx = (rand() % 2) * (ONE/4);
		Slab[i].Angle.vy = (rand() % 2) * (ONE/4);
		Slab[i].Angle.vz = (rand() % 2) * (ONE/4);
		
		/* Calculate the minimum/maximum x/y/z distances of each face
		 * of the binding box depending upon the inclination of the
		 * object wrt the z,y,x axes.
		 */
		x = Slab[i].Model->LenX;
		y = Slab[i].Model->LenY;
		z = Slab[i].Model->LenZ;
		if (Slab[i].Angle.vz == (ONE/4)) temp = x, x = y, y = temp;
		if (Slab[i].Angle.vy == (ONE/4)) temp = z, z = x, x = temp;
		if (Slab[i].Angle.vx == (ONE/4)) temp = y, y = z, z = temp;
		Slab[i].DifX = x;
		Slab[i].DifY = y;
		Slab[i].DifZ = z;
		
		/* Initialize object position */
		cubeedge = Cube[0].Model->Edge;
		slabedge = Slab[i].Model->Edge;
		temp = (cubeedge - slabedge)/2;
		do {
			x = (((rand() % cubeedge) / slabedge) * slabedge) - temp;
			y = (((rand() % cubeedge) / slabedge) * slabedge) - temp;
			z = (((rand() % cubeedge) / slabedge) * slabedge) - temp;
			for (j = 0; j < i; j++) {
				if (((x == 0) && (z == 0)) ||
					((x == Slab[j].Pos.coord.t[0]) &&
					 (y == Slab[j].Pos.coord.t[1]) &&
					 (z == Slab[j].Pos.coord.t[2])))
					break;
			}
		} while(j < i);
		
		/* Initialize object handling structure */
		InitObjc(&Slab[i], &Cube[0].Pos, x,y,z);
	}
	return;
}

/* Read TMD data from memory and initialize object */

static void InitEnmy()
{
	short i;
	for (i = 0; i < ENMY_NUM; i++) {
		/* Clean up enmy object structure */
		memset((void *)(&Enmy[i]), 0, sizeof(OBJECT));
		
		/* State of the object */
		Enmy[i].State = (STATE_ENMY_DEAD | STATE_STATIONARY);
		
		/* Set the type and model for this object */
		Enmy[i].Type = MODEL_ENMY;
		Enmy[i].Model = &Models[ Enmy[i].Type ];
		
		/* Initialize object inclination */
		Enmy[i].Angle.vx = 0;
		Enmy[i].Angle.vy = 0;
		Enmy[i].Angle.vz = 0;
		
		/* Calculate the minimum/maximum x/y/z distances of each face
		 * of the binding box depending upon the inclination of the
		 * object wrt the z,y,x axes.
		 */
		Enmy[i].DifX = Enmy[i].Model->LenX;
		Enmy[i].DifY = Enmy[i].Model->LenY;
		Enmy[i].DifZ = Enmy[i].Model->LenZ;
		
		/* Initialize object handling structure */
		InitObjc(&Enmy[i], &Cube[0].Pos, 0,0,0);
	
		/* Each enemy has 4 lives */
		Enmy[i].Lives = ENMY_MAXLIVES;
	}
	return;
}

/* Read TMD data from memory and initialize object */

static void InitBall()
{
	short i;
	for (i = 0; i < BALL_NUM; i++) {
		/* Clean up enmy object structure */
		memset((void *)(&Ball[i]), 0, sizeof(OBJECT));
		
		/* State of the object */
		Ball[i].State = (STATE_STATIONARY);
		
		/* Set the type and model for this object */
		Ball[i].Type = MODEL_BALL;
		Ball[i].Model = &Models[ Ball[i].Type ];
		
		/* Initialize object inclination */
		Ball[i].Angle.vx = 0;
		Ball[i].Angle.vy = 0;
		Ball[i].Angle.vz = 0;
		
		/* Calculate the minimum/maximum x/y/z distances of each face
		 * of the binding box depending upon the inclination of the
		 * object wrt the z,y,x axes.
		 */
		Ball[i].DifX = Ball[i].Model->LenX;
		Ball[i].DifY = Ball[i].Model->LenY;
		Ball[i].DifZ = Ball[i].Model->LenZ;
		
		/* Initialize object handling structure */
		InitObjc(&Ball[i], &Cube[0].Pos, 0,0,0);
	}
	return;
}

/* Read TMD data from memory and initialize object */

static void InitRobo(void)
{
	short x,y,z;
	/* Clean up robot/propeller object structure */
	memset((void *)(&Robo), 0, sizeof(OBJECT));
	memset((void *)(&Prop), 0, sizeof(OBJECT));
	
	/* State of the object */
	Robo.State = (STATE_ALIVE | STATE_STATIONARY);
	Prop.State = (STATE_ALIVE | STATE_STATIONARY);
	
	/* Set the type and model for this object */
	Robo.Type = MODEL_ROBO;
	Robo.Model = &Models[ Robo.Type ];
	Prop.Type = MODEL_PROP;
	Prop.Model = &Models[ Prop.Type ];
	
	/* Initialize object inclination */
	Robo.Angle.vx = 0;
	Robo.Angle.vy = 0;
	Robo.Angle.vz = 0;
	Prop.Angle.vx = 0;
	Prop.Angle.vy = 0;
	Prop.Angle.vz = 0;
	
	/* Calculate the minimum/maximum x/y/z distances of each face
	 * of the binding box depending upon the inclination of the
	 * object wrt the z,y,x axes.
	 */
	Robo.DifX = Robo.Model->LenX;
	Robo.DifY = Robo.Model->LenY;
	Robo.DifZ = Robo.Model->LenZ;
	Prop.DifX = Prop.Model->LenX;
	Prop.DifY = Prop.Model->LenY;
	Prop.DifZ = Prop.Model->LenZ;
		
	/* Robo's initial position */
	x = 0;
	y = Cube[0].MaxX - Robo.DifY;
	z = 0;

	/* Initialize object handling structure */
	InitObjc(&Robo, &Cube[0].Pos, x,y,z);
	InitObjc(&Prop, &Robo.Pos, 0,0,0);
	
	/* Initialize number of lives */
	Robo.Lives = ROBO_MAXLIVES;
	return;
}

/*
 * Initializes the view point position, angle and screen scaling
 * ----------------------------------------------------------------------------
 */

static void InitView(void)
{
	/* Set the projection screen position */
	GsSetProjection(SCR_Z);
	
	/* Set the screen scaling factor */
	ScreenScaling.vx = ONE/2;
	ScreenScaling.vy = ONE/2;
	ScreenScaling.vz = ONE;
	GsScaleScreen(&ScreenScaling);

#if USE_GsVIEW2 >= 1	
	ViewPosOffsetX = 0;
	ViewPosOffsetY = 128;
	ViewPosOffsetZ = 2048;

	ViewRefOffsetX = 0;
	ViewRefOffsetY = 0;
	ViewRefOffsetZ = 0;

	ViewAngleOffsetX = 128;
	ViewAngleOffsetY = 0;
	ViewAngleOffsetZ = 0;

	/* Set viewing point matrix */
	ViewPoint.view = GsIDMATRIX;
	
	/* Set the viewing point position */
	ViewPoint.view.t[0] = ViewPosOffsetX;
	ViewPoint.view.t[1] = ViewPosOffsetY;
	ViewPoint.view.t[2] = ViewPosOffsetZ;
	
	/* Set the viewing point super */
	ViewPoint.super = &Robo.Pos;
	
	/* Set the view point rotation angles */
	ViewPointAngle.vx = ViewAngleOffsetX;
	ViewPointAngle.vy = ViewAngleOffsetY;
	ViewPointAngle.vz = ViewAngleOffsetZ;
	
	/* Set the world/screen matrix */
	GsSetView2(&ViewPoint);
#else
	ViewPosOffsetX = 0;
	ViewPosOffsetY = 528;
	ViewPosOffsetZ = 2008;

	ViewRefOffsetX = 0;
	ViewRefOffsetY = 128;
	ViewRefOffsetZ = 0;

	ViewAngleOffsetX = 0;
	ViewAngleOffsetY = 0;
	ViewAngleOffsetZ = 0;

	/* Set the viewing point position */
	ViewPoint.vpx = -ViewPosOffsetX;
	ViewPoint.vpy = -ViewPosOffsetY;
	ViewPoint.vpz = -ViewPosOffsetZ;
		
	/* Set the viewing point reference */
	ViewPoint.vrx = -ViewRefOffsetX;
	ViewPoint.vry = -ViewRefOffsetY;
	ViewPoint.vrz = -ViewRefOffsetZ;
	
	/* Set the viewing point twist */
	ViewPoint.rz = ViewAngleOffsetZ * 360;
	
	/* Set the viewing point super */
	ViewPoint.super = &Robo.Pos;
	
	/* Set the world/screen matrix */
	GsSetRefView2(&ViewPoint);
#endif
	return;
}

/*
 * Finds the position for the given object on one of the slabs
 * Returns TRUE if found a new position, otherwise returns FALSE.
 * ----------------------------------------------------------------------------
 */

static short FindNewObjcPos(OBJECT *objc, short *x, short *y, short *z)
{
	/* Pick up one of the slabs randomly */
	short j,k = rand()%SLAB_NUM;
	
	/* Initialize object inclination */
	objc->Angle.vx = 0;
	objc->Angle.vy = 0;
	objc->Angle.vz = 0;
	
	/*
	 * Calculate the minimum/maximum x/y/z distances of each face
	 * of the binding box depending upon the inclination of the
	 * object wrt the z,y,x axes.
	 */
	objc->DifX = objc->Model->LenX;
	objc->DifY = objc->Model->LenY;
	objc->DifZ = objc->Model->LenZ;
	
	/* Set the objects starting position on one of the slabs */
	(*x) = Slab[k].Pos.coord.t[0];
	(*y) = Slab[k].Pos.coord.t[1] - (Slab[k].DifY + objc->DifY);
	(*z) = Slab[k].Pos.coord.t[2];
	
	/* Check if the enemy is beyond the cube's edge */
	if (((*x) <= (Cube[0].MinX + objc->DifX)) ||
		((*x) >= (Cube[0].MaxX - objc->DifX)) ||
		((*y) <= (Cube[0].MinY + objc->DifY)) ||
		((*y) >= (Cube[0].MaxY - objc->DifY)) ||
		((*z) <= (Cube[0].MinZ + objc->DifZ)) ||
		((*z) >= (Cube[0].MaxZ - objc->DifZ))) {
		/* We will try next time */
		return(FALSE);
	}
	
	/* Check if the object overlaps with any slab object */
	for (j = 0; j < SLAB_NUM; j++) {
		/* Check if object is overlapping with the slab */
		if ((j != k) &&
			(Slab[j].State & STATE_ALIVE) &&
			((*x) >= (Slab[j].MinX - objc->DifX)) &&
			((*x) <= (Slab[j].MaxX + objc->DifX)) &&
			((*y) >= (Slab[j].MinY - objc->DifY)) &&
			((*y) <= (Slab[j].MaxY + objc->DifY)) &&
			((*z) >= (Slab[j].MinZ - objc->DifZ)) &&
			((*z) <= (Slab[j].MaxZ + objc->DifZ))) {
			/* We will try next time */
			return(FALSE);
		}
	}
	
	/* Check if the object overlaps with any enemy object */
	for (j = 0; j < ENMY_NUM; j++) {
		/* Check if object is overlapping with the enemy */
		if ((objc != &Enmy[j]) &&
			(Enmy[j].State & STATE_ALIVE) &&
			((*x) >= (Enmy[j].MinX - objc->DifX)) &&
			((*x) <= (Enmy[j].MaxX + objc->DifX)) &&
			((*y) >= (Enmy[j].MinY - objc->DifY)) &&
			((*y) <= (Enmy[j].MaxY + objc->DifY)) &&
			((*z) >= (Enmy[j].MinZ - objc->DifZ)) &&
			((*z) <= (Enmy[j].MaxZ + objc->DifZ))) {
			/* We will try next time */
			return(FALSE);
		}
	}
	
	/* Check if the object overlaps with any enemy remains */
	for (j = 0; j < BALL_NUM; j++) {
		/* Check if object is overlapping with the enemy remains */
		if ((objc != &Ball[j]) &&
			(Ball[j].State & STATE_ALIVE) &&
			((*x) >= (Ball[j].MinX - objc->DifX)) &&
			((*x) <= (Ball[j].MaxX + objc->DifX)) &&
			((*y) >= (Ball[j].MinY - objc->DifY)) &&
			((*y) <= (Ball[j].MaxY + objc->DifY)) &&
			((*z) >= (Ball[j].MinZ - objc->DifZ)) &&
			((*z) <= (Ball[j].MaxZ + objc->DifZ))) {
			/* We will try next time */
			return(FALSE);
		}
	}
	
	/* Check if the object is overlapping with the robot */
	if ((objc != &Robo) &&
		(Robo.State & STATE_ALIVE) &&
		((*x) >= (Robo.MinX - objc->DifX)) &&
		((*x) <= (Robo.MaxX + objc->DifX)) &&
		((*y) >= (Robo.MinY - objc->DifY)) &&
		((*y) <= (Robo.MaxY + objc->DifY)) &&
		((*z) >= (Robo.MinZ - objc->DifZ)) &&
		((*z) <= (Robo.MaxZ + objc->DifZ))) {
		/* We will try next time */
		return(FALSE);
	}
	
	/* The object will not overlap with any other object */
	return(TRUE);
}

/*
 * Main program
 * ----------------------------------------------------------------------------
 */

main()
{
	/* Initialize various sub-systems */
	InitSystem();						// Initialize graphics system
	InitLite();							// Light matrix
	InitModels();						// Initialize all models used here
	
	/* Initialize variables for changing object's direction */
#if REFLECT_OBJECT >= 1
	ObjcNewAngleY = 0;					// New direction for the object after reflection
	ObjcNewAngleYStep = 0;				// Step to change the direction of the object
	ObjcNewAngleYChange = 0;			// direction changes to new value if set to true
#endif
	
	/* Initialize various globals */
	GameStatus = GAME_PLAY;				// Game is always in play mode
	ShowDebug = FALSE;					// Show debug info when true
	IdleTime = 0;						// Initialize the idle time
	
	/* Number of enemies killed so far */
	EnmyKilled = 0;						// Number of enemies killed
	CurrentScore = 0;					// Initialize score
	
	/* Initialize all objects */
	InitCube();							// Initialize cube face objects
	InitSlab();							// Initialize slab objects
	InitEnmy();							// Initialize enemy objects
	InitBall();							// Initialize enemy remains objects
	InitRobo();							// Initialize robot object
	
	/* Initialize View Point */
	InitView();							// Initialize View Point matrix
	
	/* Initialize particle explosion sub-system */
	ExplosionInitData(2048, 2048, 2048);// Initialize explosion data
	
	/* Initialize sound library */
	SoundInit();						// Sound initial setting
	
	/* Start to play game music */
	SoundPlayMusic();					// Sound playback start
	
	/* MAIN GAME LOOP - Quit if select button is pressed */
	while (GameStatus != GAME_OVER) {
		PadRead();						// Read the controller
		HandleGameOver();				// Check if game is over
		HandleLite();					// Handle lighting - light sources/ambient light
		HandleRobo();					// Handle the robot movement
		HandleEnmy();					// Handle enemy movement
		HandleBall();					// Handle enemy remains movement
		HandleView();					// Handle the view point
		HandlePolyDivision();			// Handle poly division of various objects
		DrawScreen();					// Handle screen display
	}
	
	/* Stop the sound play back and quit to Yaroze development console */
	SoundStop();						// Sound playback termination
	printf("Quitting from sample\n\n");	// Print a message before quitting
    return(0);
}

/*
 * Checks if game is over
 * ----------------------------------------------------------------------------
 */

static void HandleGameOver(void)
{
	if (PadData == (PADSelect | PADStart))
		GameStatus = GAME_OVER;
	return;
}

/*
 * Handle robot movement
 * ----------------------------------------------------------------------------
 */

/* Handle Robot - the player charecter */

static void HandleRobo()
{
	short angley, anglez;
	short speedf, speedy;
	short dx, dy, dz;
	short headx, headz;
	short where;
	
	/* Handle dead robot */
	if (!(Robo.State & STATE_ALIVE)) {
		/* Wait for all explosion particles to expire */
		if (ImplosionWaitData((!(old_PadData & PADStart) && (PadData & PADStart)) ? TRUE : FALSE))
			return;
		
		/* Check if all lives are over */	
		if (Robo.Lives <= 0) {
			/* No more lives left - the game is over */
			GameStatus = GAME_OVER;
			return;
		}
		
		/* Initialize the implosion data */
		if (ImplosionInitData()) {
			/*
			 * Try and turn on object at one of the slabs.
			 * Set the objects starting position at one of the slabs
			 */
			short x,y,z;
			if (!FindNewObjcPos(&Robo, &x, &y, &z))
				return;
			
			/* Initialize propeller inclination */
			Prop.Angle.vx = 0;
			Prop.Angle.vy = 0;
			Prop.Angle.vz = 0;
			
			/* Initialize object handling structure */
			InitObjc(&Robo, &Cube[0].Pos, x,y,z);
			InitObjc(&Prop, &Robo.Pos, 0,0,0);
			
			/* Prepare the implosion data */
			ImplosionMakeData(Robo.Model->ModelPtr, 0, &Cube[0].Pos, x,y,z);
			ImplosionMakeData(Prop.Model->ModelPtr, 0, &Cube[0].Pos, x,y,z);
		}
		else {
			/* Regenerate the Robot */
			Robo.State |= STATE_ALIVE;
			Prop.State |= STATE_ALIVE;
		}
		
		/* Finish the implosion data */
		ImplosionDoneData();
		return;
	}
	
	/*
	 * Handles reflection of the player object.  Whenever the player
	 * object reflects due to collision with another object or the
	 * walls of the cube, its direction changes.  Therefore the view
	 * will change suddenly because of this swift change in player
	 * object's direction.
	 *
	 * In order to avoid this swift change of scene, we will animate
	 * the change in direction in steps.  While this is being done,
	 * the actual game play will pause, i.e. all other objects in the
	 * world will remain where they are and the game play will pause
	 * until we have reached the desired anglular value around y axis.
	 */
#if REFLECT_OBJECT >= 1
	if (ObjcNewAngleYChange != 0)  {
		/* Calculate the change in object's inclination around y axix */
		if (abs(ObjcNewAngleYStep) > abs(ObjcNewAngleY)) {
			ObjcNewAngleYStep = ObjcNewAngleY;
		}
		
		/* Update the object's inclination around y axis */
		Robo.Angle.vy += ObjcNewAngleYStep;
		
		/* Round off the angle value */
		Robo.Angle.vy = (Robo.Angle.vy + ONE) % ONE;
		
		/* Re-calculate the object's matrix */
		HandleSetCoordinate(&Robo);
		
		/* Check if the object has been rotated to the desired angle around y axis */
		if ((ObjcNewAngleY -= ObjcNewAngleYStep) == 0) {
			ObjcNewAngleY = 0;
			ObjcNewAngleYStep = 0;
			ObjcNewAngleYChange = 0;
		}
		return;
	}
#endif
	
	/* Get the object's inclination around y,z axis */
	angley = Robo.Angle.vy;
	anglez = Robo.Angle.vz;
	
	/* Get the object's speed in forward and vertical directions */
	speedf = Robo.SpeedForward;
	speedy = Robo.SpeedY;
	
	/* Initialize the object's displacement */
	dx = 0, dy = 0, dz = 0;
	
	/*
	 * Change the angle of movement of the object wrt the y axis if
	 * left or right directional button on the controller is pressed.
	 * The object will appear to turn left or right.
	 *
	 * When left/right button is pressed the main player object will
	 * seem to tilt around the z axis.  This is achieved by changing
	 * the angle of the object wrt the z axis and simultaneously
	 * turning the camera in the opposite direction.  Once we let go
	 * of the buttons the object will slowly turn back to its normal
	 * position.
	 */
	if (PadData & PADLleft) {
		angley -= ROBO_ANGLE_YSTEP;
		if (angley < 0)		angley += ONE;
		
		anglez -= ROBO_ANGLE_ZSTEP;
		if (anglez < ROBO_ANGLE_ZMIN)	anglez = ROBO_ANGLE_ZMIN;
	}
	else
	if (PadData & PADLright) {
		angley += ROBO_ANGLE_YSTEP;
		if (angley >= ONE)	angley -= ONE;
		
		anglez += ROBO_ANGLE_ZSTEP;
		if (anglez > ROBO_ANGLE_ZMAX)	anglez = ROBO_ANGLE_ZMAX;
	}
	else {
		if (anglez < 0)	anglez += ROBO_ANGLE_ZSTEP;
		else
		if (anglez > 0)	anglez -= ROBO_ANGLE_ZSTEP;
	}
	
	/*
	 * Check the current state the robot is in, and make it fly
	 * vertically if the "X" button is tapped upon.
	 */
	if (!(old_PadData & PADRdown) && (PadData & PADRdown)) {
		if (!(Robo.State & STATE_Y_CHANGING))
			dy -= ROBO_SPEED_YINIT;
		speedy -= ROBO_SPEED_YSTEP;
	}
	else {
		speedy += ROBO_SPEED_GSTEP;
	}
	dy += (speedy / ROBO_SPEED_YFACTOR);
	
	/*
	 * Increase or decrease the speed of the object along the forward
	 * direction, when the up or down directional button on the controller
	 * are pressed. The object will seem to move forward or backward in
	 * current direction depending upon how long up or down directional
	 * button is pressed. The speed of the object in forward or backward
	 * direction will change to a predefined limit only.
	 */
	if (PadData & PADLup) {
		speedf += ROBO_SPEED_FSTEP;
		if (speedf > ROBO_SPEED_FMAX)	speedf = ROBO_SPEED_FMAX;
	}
	else
	if (PadData & PADLdown) {
		speedf -= ROBO_SPEED_FSTEP;
		if (speedf < ROBO_SPEED_FMIN)	speedf = ROBO_SPEED_FMIN;
	}
	
	/*
	 * Calculate sin and cos of the objects direction angle in the world.
	 * The Sin and Cos values are Fixed point values in 1.5.10 format. Then
	 * we calculate the change in object's position along x and z axes in
	 * the world.
	 */
	headx = MySin(angley);
	headz = MyCos(angley);
	dx += (((speedf * headx) + 0x200) >> 10);
	dz += (((speedf * headz) + 0x200) >> 10);
	
	/* Handle the collision of the object with other objects */
	where = HandleRoboCollision(&dx, &dy, &dz);
	
	/*
	 * The speed of the object will change in all directions depending
	 * upon the direction of the collision.
	 */
	if (where & COLLISION_VERT)	speedy = -(speedy * 2 / 3);
	if (where & COLLISION_HORZ)	speedf = -(speedf * 2 / 3);
	
	/*
	 * If the object gets pinned to the wall or another object,
	 * whenever the user applies forward/backward force and there
	 * is a wall or another object in the direction of the movement.
	 * The object will stop moving in vertical as well as horizontal
	 * direction.  The speed in horizontal direction is automatically
	 * controlled by the press of the up/down directional buttons
	 * while pinning the player object with another object.  We need
	 * to control the vertical speed of the object here when such a
	 * situation develops.
	 */
	if ((where & COLLISION_HORZ) && (dx == 0) && (dy == 0) && (dz == 0))
		speedy = 0;
	
	/*
	 * If there was a collision then the direction of the object changes.
	 * The new direction will depend upon the direction from which the
	 * object collided with another object and the objects inclination
	 * in the world.
	 *
	 * Set the new angle of the object and other parameters to animate this
	 * change in direction because of reflection from wall or other objects
	 */
#if REFLECT_OBJECT >= 1
	if (where & COLLISION_HORZ) {
		speedf = -speedf;
		if (where & COLLISION_X) {
			ObjcNewAngleY = ONE - angley;
			ObjcNewAngleYChange = +1;
		}
		if (where & COLLISION_Z) {
			ObjcNewAngleY = ONE - angley + ONE/2;
			ObjcNewAngleYChange = -1;
		}
		ObjcNewAngleY %= ONE;
		if ((headz * headx * ObjcNewAngleYChange) > 0)
			ObjcNewAngleY = -((angley - ObjcNewAngleY + ONE) % ONE);
		else
			ObjcNewAngleY =  ((ObjcNewAngleY - angley + ONE) % ONE);
		ObjcNewAngleYStep = (ObjcNewAngleY / 10);
	}
#endif
	
 	/* Update the object's new position and inclination. */
	Robo.Pos.coord.t[0] += dx;			// New position wrt x axis
	Robo.Pos.coord.t[1] += dy;			// New position wrt y axis
	Robo.Pos.coord.t[2] += dz;			// New position wrt z axis 
	Robo.Angle.vy = angley;				// Object's angle wrt y axis
	Robo.Angle.vz = anglez;				// Object's angle wrt z axis
	
 	/* Also re-calculate the matrix of the object */
	HandleSetCoordinate(&Robo);			// Re-calculate object's matrix
	
	/*
	 * Save robot position for the last given number of  frames -
	 * i.e. we use a FIFO of given number of  entries to save the
	 * position of the object in the world.  The following function
	 * calculates the object's state by examining the object's position
	 * in the last given number of frames.
	 */
	Robo.State = HandlePosFIFO(&Robo);	// Object's new state
	
	/* Update the objects extents in x,y,z directions */
	Robo.MinX += dx, Robo.MaxX += dx;	// Object's extents in x direction
	Robo.MinY += dy, Robo.MaxY += dy;	// Object's extents in y direction
	Robo.MinZ += dz, Robo.MaxZ += dz;	// Object's extents in z direction
	
	/* Update the objects vertical/forward speeds */
	Robo.SpeedY = speedy;				// Speed in vertical direction
	Robo.SpeedForward = speedf;			// Speed in forward direction

	/*
	 * Handle the propeller rotation here
	 */
	angley = Prop.SpeedAngleY;
	dy = Prop.SpeedAngleYPlaySfx;
	
	if (!(old_PadData & PADRdown) && (PadData & PADRdown)) {
		/* The object's rotation speed is going up */
		angley += PROP_SPEED_YSTEP;
		if (angley > PROP_SPEED_YMAX) angley = PROP_SPEED_YMAX;
	}
	else {
		/* The object's rotation speed is going down */
		angley -= PROP_SPEED_GSTEP;
		if (angley < 0) angley = 0;
	}
	
	/* Play the propeller sound until it's angular speed becomes zero */
	if (angley != 0) {
		if (dy == 0) {
			SoundVoiceQPlay(SOUND_VC_PROPELLER);
			dy = PROP_PLAYSFX_MAX -
					(angley * (PROP_PLAYSFX_MAX - PROP_PLAYSFX_MIN)) /
					PROP_SPEED_YMAX;
		}
		dy --;
	}
	else {
		dy = 0;
	}
	
	/* Change the angle of the object by 45 degrees */
	Prop.Angle.vy += angley;
	Prop.Angle.vy %= ONE;
	
	/* Updare the anglular inclination of propeller with y axis */
	Prop.SpeedAngleY = angley;
	Prop.SpeedAngleYPlaySfx = dy;
 	
	/* Re-calculate the matrix and coordinates of the object */
	HandleSetCoordinate(&Prop);
	return;
}

/*
 * Handle the collision of the Robot with other objects.
 *
 * This routines takes the robot's displacement vector
 * and finds out if it will collide with other objects
 * if this displacement is given to it.
 *
 * First of all we compute it's collision with all other
 * object's in the world.  The last collision detection
 * out of all these collision detection sequence is the
 * one which we should be worried about.
 *
 * There may be more than one objects lying in it's path.
 * And if the farther object comes first in our order of
 * collision detection computation, it will detect that as
 * the collision.  But later in the process it comes across
 * the second object and will compute that as the next
 * collision, and how rightly so.
 *
 * On the other hand, if the closer object in the direction
 * of the displacement vector comes first in the list then
 * it will detect that as the collision and reconfigure the
 * collision vector.  So the farther object will never be
 * detected as a collidinnng object.
 *
 * So the summary is that the last collision detected in
 * the collision detection process is the actual collision.
 */

static short HandleRoboCollision(short *dx, short *dy, short *dz)
{
	short i, slabid, ballid, enmyid, roboid;
	short where_onwall, where_onslab, where_onball, where_onenmy;
	short state = Robo.State;
	
	/* Check the collision of the object with enemies */
	where_onenmy = COLLISION_CLEAR;
	enmyid = roboid = -1;
	for (i = 0; i < ENMY_NUM; i++) {
		short where = HandleObjcCollision(&Robo, &Enmy[i], dx, dy, dz);
		if (where & COLLISION_XYZ) {
			if ((Robo.Pos.coord.t[1] - Robo.DifY + (*dy)) <=
				(Enmy[i].Pos.coord.t[1] - Enmy[i].DifY))
				enmyid = i;
			else
				roboid = 1;
			where_onenmy |= where;
		}
	}
	
	/* Check the collision of the object with enemy remains */
	where_onball = COLLISION_CLEAR;
	ballid = -1;
	for (i = 0; i < BALL_NUM; i++) {
		short where = HandleObjcCollision(&Robo, &Ball[i], dx, dy, dz);
		if (where & COLLISION_XYZ) {
			ballid = i;
			where_onball |= where;
		}
	}
	
	/* Check the collision of the object with slabs */
	where_onslab = COLLISION_CLEAR;
	slabid = -1;
	for (i = 0; i < SLAB_NUM; i++) {
		short where = HandleObjcCollision(&Robo, &Slab[i], dx, dy, dz);
		if (where & COLLISION_XYZ) {
			slabid = i;
			where_onslab |= where;
		}
	}
	
	/* Check the collision of the object with the walls of the cube */
	where_onwall = HandleWallCollision(&Robo, &Cube[0], dx, dy, dz);
	
	/*
	 * Calculate whether there was any collision and if so
	 * play the right sound effect.  The sound effect is played
	 * only when there is a collision with another object and
	 * the object is still changing it's position along any of
	 * the x,y,z axes.  The object's state is calculated by
	 * checking the object's position is given last number of
	 * frames.
	 *
	 * Please also make note of the fact that we do the following
	 * checks in exactly the reverse order of the collision computaion
	 * donw above.  This will make sure that we look at the last
	 * collision computation first.
	 */
	if (((where_onwall & COLLISION_X) && (state & STATE_X_CHANGING)) ||
		((where_onwall & COLLISION_Y) && (state & STATE_Y_CHANGING)) ||
		((where_onwall & COLLISION_Z) && (state & STATE_Z_CHANGING))) {
		SoundVoiceQPlay(SOUND_VC_COLLISION);
	}
	else
	if (((where_onslab & COLLISION_X) && (state & STATE_X_CHANGING)) ||
		((where_onslab & COLLISION_Y) && (state & STATE_Y_CHANGING)) ||
		((where_onslab & COLLISION_Z) && (state & STATE_Z_CHANGING))) {
		SoundVoiceQPlay(SOUND_VC_COLLISION);
	}
	else
	if (((where_onball & COLLISION_X) && (state & STATE_X_CHANGING)) ||
		((where_onball & COLLISION_Y) && (state & STATE_Y_CHANGING)) ||
		((where_onball & COLLISION_Z) && (state & STATE_Z_CHANGING))) {
		if (ballid != -1) {
			HandleKillBall(&Ball[ ballid ]);
			SoundVoiceQPlay(SOUND_VC_GRAB);
		}
	}
	else
	if (((where_onenmy & COLLISION_X) && (state & STATE_X_CHANGING)) ||
		((where_onenmy & COLLISION_Y) && (state & STATE_Y_CHANGING)) ||
		((where_onenmy & COLLISION_Z) && (state & STATE_Z_CHANGING))) {
		if (enmyid != -1) {
			HandleKillEnmy(&Enmy[ enmyid ]);
			SoundVoiceQPlay(SOUND_VC_CRASH);
		}
		else
		if (roboid == 1) {
			HandleKillRobo();
			SoundVoiceQPlay(SOUND_VC_CRASH);
		}
	}
	return(where_onwall | where_onslab);
}

/* Kill the Robot */

static void HandleKillRobo(void)
{
	/* Prepare the explosion data */
	short x = Robo.Pos.coord.t[0];
	short y = Robo.Pos.coord.t[1];
	short z = Robo.Pos.coord.t[2];
	ExplosionMakeData(Robo.Model->ModelPtr, 0, &Cube[0].Pos, x,y,z);
	ExplosionMakeData(Prop.Model->ModelPtr, 0, &Cube[0].Pos, x,y,z);
		
	/* Clear the alive attribute of the object */
	Robo.State &= ~STATE_ALIVE;
	Prop.State &= ~STATE_ALIVE;
		
	/* One less life for this enemy */
	Robo.Lives--;
	return;
}

/*
 * Handle enemy movement
 * ----------------------------------------------------------------------------
 */

/* Handle enemy */

static void HandleEnmy(void)
{
	short k;
	short angley;
	short speedy, speedf;
	short dx, dy, dz;
	short headx, headz;
	short where;
	
	/*
	 * Freeze enemies when there is an animation in progress
	 * due to reflection of robot from other objects or wall.
	 */
#if REFLECT_OBJECT >= 1
	if (ObjcNewAngleYChange != 0)
		return;
#endif
	
	/* Handle all enemies */
	for (k = 0; k < ENMY_NUM; k++) {
		/* Update the object's state - apply finite state machine on enmy object */
		HandleEnmyState(&Enmy[k]);
		
		/* Continue if the object is not alive - i.e. it does not exist */
		if (!(Enmy[k].State & STATE_ALIVE))
			continue;
			
		/* Get the object's inclination around y axis */
		angley = Enmy[k].Angle.vy;
		
		/* Get the object's speed in forward and vertical directions */
		speedy = Enmy[k].SpeedY;
		speedf = Enmy[k].SpeedForward;
		
		/* Initialize the object's displacement */
		dy = speedy;
		
		/*
		 * Calculate sin and cos of the objects direction angle in the world.
		 * The Sin and Cos values are Fixed point values in 1.5.10 format. Then
		 * we calculate the change in object's position along x and z axes in
		 * the world.
		 */
		headx = MySin(angley);
		headz = MyCos(angley);
		dx = (((speedf * headx) + 0x200) >> 10);
		dz = (((speedf * headz) + 0x200) >> 10);
		
		/* Handle the collision of the object with other objects */
		where = HandleEnmyCollision(&Enmy[k], &dx, &dy, &dz);
		
		/*
		 * The speed of the object will change in all directions depending
		 * upon the direction of the collision.
		 */
		if (where & COLLISION_VERT)	speedy = -(speedy * 2 / 3);
		if (where & COLLISION_HORZ)	speedf = -(speedf * 2 / 3);
		
		/*
		 * If the object gets pinned to the wall or another object,
		 * whenever the user applies forward/backward force and there
		 * is a wall or another object in the direction of the movement.
		 * The object will stop moving in vertical as well as horizontal
		 * direction.  The speed in horizontal direction is automatically
		 * controlled by the press of the up/down directional buttons
		 * while pinning the player object with another object.  We need
		 * to control the vertical speed of the object here when such a
		 * situation develops.
		 */
		if ((where & COLLISION_HORZ) && (dx == 0) && (dy == 0) && (dz == 0))
			speedy = 0;
		
		/*
		 * If there was a collision then the direction of the object changes.
		 * The new direction will depend upon the direction from which the
		 * object collided with another object and the objects inclination
		 * in the world.
		 */
		if (where & COLLISION_HORZ) {
			speedf = -speedf;
			if (where & COLLISION_X) angley = ONE - angley;
			if (where & COLLISION_Z) angley = ONE - angley + ONE/2;
			angley %= ONE;
		}
		
	 	/* Update the object's new position and inclination. */
		Enmy[k].Pos.coord.t[0] += dx;	// New position wrt x axis
		Enmy[k].Pos.coord.t[1] += dy;	// New position wrt y axis
		Enmy[k].Pos.coord.t[2] += dz;	// New position wrt z axis 
		Enmy[k].Angle.vy = angley;		// Object's angle wrt y axis
		
	 	/* Also re-calculate the matrix of the object */
		HandleSetCoordinate(&Enmy[k]);	// Re-calculate object's matrix
		
		/*
		 * Save robot position for the last given number of  frames -
		 * i.e. we use a FIFO of given number of  entries to save the
		 * position of the object in the world.  The following function
		 * calculates the object's state by examining the object's position
		 * in the last given number of frames.
		 */
		Enmy[k].State = HandlePosFIFO(&Enmy[k]);
		
		/* Update the objects extents in x,y,z directions */
		Enmy[k].MinX += dx, Enmy[k].MaxX += dx;
		Enmy[k].MinY += dy, Enmy[k].MaxY += dy;
		Enmy[k].MinZ += dz, Enmy[k].MaxZ += dz;
		
		/* Update the objects vertical/forward speeds */
		Enmy[k].SpeedY = speedy;		// Speed in vertical direction
		Enmy[k].SpeedForward = speedf;	// Speed in forward direction
	}
	return;
}

/* Performs AI for the enemies */

static void HandleEnmyState(OBJECT *objc)
{
	/* Get the current state the object is in */
	short state = (objc->State & STATE_ENMY_MASK);
	if (state == STATE_ENMY_DEAD) {
		short x,y,z;
		/* Is there any more regeneration possibility */
		if (objc->Lives <= 0)
			return;
		
		/* Is death state complete? */
		if (++objc->ChangeStateCounter < objc->ChangeStateThreshold)
			return;
		
		/* State of the object */
		objc->State = (STATE_ALIVE | STATE_ENMY_NEWSTATE | STATE_STATIONARY);
		
		/* Set the type and model for this object */
		objc->Type = MODEL_ENMY;
		objc->Model = &Models[ objc->Type ];
		
		/*
		 * Try and turn on object at one of the slabs.
		 * Set the objects starting position at one of the slabs
		 */
		if (!FindNewObjcPos(objc, &x, &y, &z))
			return;
		
		/* Initialize object handling structure */
		InitObjc(objc, &Cube[0].Pos, x,y,z);
		return;
	}
	
	if (state == STATE_ENMY_DYING) {
		/* Is death sequence complete? */
		if (++objc->ChangeStateCounter < objc->ChangeStateThreshold)
			return;
			
		/* Kill the enemy */
		HandleKillEnmy(objc);
		return;
	}
	
	if (state == STATE_ENMY_NEWSTATE) {
		/*
		 * The following holds the aliens personality probability table.
		 * here are the meanings of the values and the current distribution
		 *		0 attack - 30%
		 *		1 random - 20%
		 *		2 evade  - 10%
		 *		3 stop   - 10%
		 *		4 turn   - 20%
		 */
		static short Personality[10] = {0,1,4,0,1,4,3,0,2,3};
		
		/*
		 * The following provides tables used to set the random
		 * parameters for the given object.  These parameters
		 * provide for implementing a heuristic approach for
		 * the object's behaviour in the world.
		 */
		static AI EnmyAI[] = {
			{STATE_ENMY_ATTACK, 10, 26,  3, 16,   0,   1, 0, 1, 16, 17, 32, 33},
			{STATE_ENMY_RANDOM,  3,  8, 10, 51,   0,   1, 0, 1, 16, 17, 32, 33},
			{STATE_ENMY_EVADE , 10, 26,  3,  5,   0,   1, 0, 1, 16, 17, 32, 33},
			{STATE_ENMY_STOP  ,  3, 13,  6, 16,   0,   1, 0, 1,  0,  1,  0,  1},
			{STATE_ENMY_TURN  , 10, 61,  0,  4, -96, 193, 0, 1, 16, 17, 25, 26},
			};
		
		/*
		 * A new state has been requested, select a new
		 * state based on probability and personality
		 */
		AI *ai = &EnmyAI[ Personality[rand() % 10] ];
		objc->State                   &= ~STATE_ENMY_MASK;
		objc->State                   |=  ai->State;
		objc->ChangeStateCounter       =  0;
		objc->ChangeStateThreshold     =  ai->StateThresholdMin     + rand() % ai->StateThresholdRand;
		objc->ChangeParameterCounter   =  0;
		objc->ChangeParameterThreshold =  ai->ParameterThresholdMin + rand() % ai->ParameterThresholdRand;
		objc->AuxilliaryVarOne         =  ai->AuxilliaryVarOneMin   + rand() % ai->AuxilliaryVarOneRand;
		objc->AuxilliaryVarTwo         =  ai->AuxilliaryVarTwoMin   + rand() % ai->AuxilliaryVarTwoRand;
		objc->SpeedY                   = -ai->SpeedYMin             - rand() % ai->SpeedYRand;
		objc->SpeedForward             =  ai->SpeedForwardMin       + rand() % ai->SpeedForwardRand;
		return;
	}
	
	/* Test if it's time to change parameters */
	if (++objc->ChangeParameterCounter < objc->ChangeParameterThreshold)
		return;
			
	/* Reset parameter counter */
	objc->ChangeParameterCounter = 0;
	
	/* Check state of an object and take necessary action */
	if (state == STATE_ENMY_ATTACK) {
		/*
		 * Adjust heading toward player, use a heuristic approach
		 * that simply tries to keep turing the alien toward the
		 * player, later maybe we could add a bit of trajectory
		 * lookahead, so the alien could intercept the player???
		 *
		 * To determine which way the alien needs to turn we can use
		 * the following trick: based on the current trajectory of
		 * the alien and the vector from the alien to the player, we
		 * can compute a normal vector
		 */
		
		/* Compute heading vector (happens to be a unit vector) */
		int headx = MySin(objc->Angle.vy);
		int headz = MyCos(objc->Angle.vy);
		
		/*
		 * Compute target trajectory vector, players position
		 * minus aliens position
		 */
		int targetx = Robo.Pos.coord.t[0] - objc->Pos.coord.t[0];
		int targety = Robo.Pos.coord.t[1] - objc->Pos.coord.t[1];
		int targetz = Robo.Pos.coord.t[2] - objc->Pos.coord.t[2];
		
		/* Now compute y component of normal */
		int normaly = (headz * targetx - headx * targetz);
		
		/*
		 * Based on the sign of the result we can determine if
		 * we should turn the alien right or left.
		 */
		if (normaly >= 0)
			objc->Angle.vy += (96 + rand() % 96);
		else
			objc->Angle.vy -= (96 + rand() % 96);
		
		/*
		 * Based on the sign of the difference in y position
		 * of the object and player object - change the speed
		 * of the object in y direction
		 */
		if (targety != 0) {
			objc->SpeedY += (targety / MyAbs(targety));
		}
	}
	else	
	if (state == STATE_ENMY_RANDOM) {
		/* Select a new direction */
		objc->Angle.vy += (-320 + rand() % 641);
		
		/* Select a new vertical speed */
		objc->SpeedY += (-1 + rand() % 2);
		
		/* Select a new forward speed */
		objc->SpeedForward += (-2 + rand() % 4);
	}
	else	
	if (state == STATE_ENMY_EVADE) {
		/*
		 * Adjust heading away from player, use a heuristic approach
		 * that simply tries to keep turing the alien away from the
		 * player object.
		 *
		 * To determine which way the alien needs to turn we can use
		 * the following trick: based on the current trajectory of
		 * the alien and the vector from the alien to the player, we
		 * can compute a normal vector
		 */
		
		/* Compute heading vector (happens to be a unit vector) */
		int headx = MySin(objc->Angle.vy);
		int headz = MyCos(objc->Angle.vy);
		
		/*
		 * Compute target trajectory vector, players position
		 * minus aliens position
		 */
		int targetx = Robo.Pos.coord.t[0] - objc->Pos.coord.t[0];
		int targety = Robo.Pos.coord.t[1] - objc->Pos.coord.t[1];
		int targetz = Robo.Pos.coord.t[2] - objc->Pos.coord.t[2];
		
		/* Now compute y component of normal */
		int normaly = (headz * targetx - headx * targetz);
		
		/*
		 * Based on the sign of the result we can determine if
		 * we should turn the alien right or left.
		 */
		if (normaly >= 0)
			objc->Angle.vy -= (96 + rand() % 96);
		else
			objc->Angle.vy += (96 + rand() % 96);
		
		/*
		 * Based on the sign of the difference in y position
		 * of the object and player object - change the speed
		 * of the object in y direction
		 */	
		if (targety != 0) {
			objc->SpeedY -= (targety / MyAbs(targety));
		}
	}
	else	
	if (state == STATE_ENMY_STOP) {
		/* Stopping sequence - do nothing */
	}
	else	
	if (state == STATE_ENMY_TURN) {
		/* Continue turn and check angle for overflow/underflow */
		objc->Angle.vy += objc->AuxilliaryVarOne;
	}
	else {
		/* Unknown state - change the state to expecting new state */
		objc->ChangeStateCounter   = 0;
		objc->ChangeStateThreshold = 0;
	}
	
	/* Check angle for overflow/underflow */
	objc->Angle.vy = (objc->Angle.vy + ONE) % ONE;
	
	/* Test if current state sequence complete */
	if (++objc->ChangeStateCounter < objc->ChangeStateThreshold)
		return;
	
	/* Tell state machine that object is expecting a new state */
	objc->State &= ~STATE_ENMY_MASK;
	objc->State |=  STATE_ENMY_NEWSTATE;
	return;
}

/*
 * Handle the collision of the enemy with other objects.
 *
 * This routines takes the enemy's displacement vector
 * and finds out if it will collide with other objects
 * if this displacement is given to it.
 *
 * First of all we compute it's collision with all other
 * object's in the world.  The last collision detection
 * out of all these collision detection sequence is the
 * one which we should be worried about.
 *
 * There may be more than one objects lying in it's path.
 * And if the farther object comes first in our order of
 * collision detection computation, it will detect that as
 * the collision.  But later in the process it comes across
 * the second object and will compute that as the next
 * collision, and how rightly so.
 *
 * On the other hand, if the closer object in the direction
 * of the displacement vector comes first in the list then
 * it will detect that as the collision and reconfigure the
 * collision vector.  So the farther object will never be
 * detected as a collidinnng object.
 *
 * So the summary is that the last collision detected in
 * the collision detection process is the actual collision.
 */

static short HandleEnmyCollision(OBJECT *objc, short *dx, short *dy, short *dz)
{
	short i, slabid, ballid, enmyid, roboid;
	short where_onwall, where_onslab, where_onball, where_onenmy, where_onrobo;
	short state = objc->State;
		
	/* Check the collision of the object with robot */
	roboid = -1;
	where_onrobo = HandleObjcCollision(objc, &Robo, dx, dy, dz);
	if (where_onrobo & COLLISION_XYZ) {
		if ((Robo.Pos.coord.t[1] - Robo.DifY) <=
			(Enmy[i].Pos.coord.t[1] - Enmy[i].DifY + (*dy)))
			roboid = 0;
		else
			roboid = 1;
	}
	
	/* Check the collision of the object with other enemies */
	where_onenmy = COLLISION_CLEAR;
	enmyid = -1;
	for (i = 0; i < ENMY_NUM; i++) {
		short where = HandleObjcCollision(objc, &Enmy[i], dx, dy, dz);
		if (where & COLLISION_XYZ) {
			enmyid = i;
			where_onenmy |= where;
		}
	}
	
	/* Check the collision of the object with enemy remains */
	where_onball = COLLISION_CLEAR;
	ballid = -1;
	for (i = 0; i < BALL_NUM; i++) {
		short where = HandleObjcCollision(objc, &Ball[i], dx, dy, dz);
		if (where & COLLISION_XYZ) {
			ballid = i;
			where_onball |= where;
		}
	}
	
	/* Check the collision of the object with slabs */
	where_onslab = COLLISION_CLEAR;
	slabid = -1;
	for (i = 0; i < SLAB_NUM; i++) {
		short where = HandleObjcCollision(objc, &Slab[i], dx, dy, dz);
		if (where & COLLISION_XYZ) {
			slabid = i;
			where_onslab |= where;
		}
	}
	
	/* Check the collision of the object with the walls of the cube */
	where_onwall = HandleWallCollision(objc, &Cube[0], dx, dy, dz);

	/*
	 * Calculate whether there was any collision and if so
	 * play the right sound effect or whatever you want to do.
	 * The sound effect is played only when there is a collision
	 * with another object and the object is still changing it's
	 * position along any of the x,y,z axes.  The object's state
	 * is calculated by checking the object's position is given
	 * last number of frames.
	 *
	 * Please also make note of the fact that we do the following
	 * checks in exactly the reverse order of the collision computaion
	 * donw above.  This will make sure that we look at the last
	 * collision computation first.
	 */
	if (((where_onwall & COLLISION_X) && (state & STATE_X_CHANGING)) ||
		((where_onwall & COLLISION_Y) && (state & STATE_Y_CHANGING)) ||
		((where_onwall & COLLISION_Z) && (state & STATE_Z_CHANGING))) {
		/* Do whatever you want to do here */
	}
	else
	if (((where_onslab & COLLISION_X) && (state & STATE_X_CHANGING)) ||
		((where_onslab & COLLISION_Y) && (state & STATE_Y_CHANGING)) ||
		((where_onslab & COLLISION_Z) && (state & STATE_Z_CHANGING))) {
		/* Do whatever you want to do here */
	}
	else
	if (((where_onball & COLLISION_X) && (state & STATE_X_CHANGING)) ||
		((where_onball & COLLISION_Y) && (state & STATE_Y_CHANGING)) ||
		((where_onball & COLLISION_Z) && (state & STATE_Z_CHANGING))) {
		/* Do whatever you want to do here */
	}
	else
	if (((where_onenmy & COLLISION_X) && (state & STATE_X_CHANGING)) ||
		((where_onenmy & COLLISION_Y) && (state & STATE_Y_CHANGING)) ||
		((where_onenmy & COLLISION_Z) && (state & STATE_Z_CHANGING))) {
		/* Do whatever you want to do here */
	}
	else
	if (((where_onrobo & COLLISION_X) && (state & STATE_X_CHANGING)) ||
		((where_onrobo & COLLISION_Y) && (state & STATE_Y_CHANGING)) ||
		((where_onrobo & COLLISION_Z) && (state & STATE_Z_CHANGING))) {
		if (roboid == 0) {
			HandleKillEnmy(objc);
			SoundVoiceQPlay(SOUND_VC_CRASH);
		}
		else
		if (roboid == 1) {
			HandleKillRobo();
			SoundVoiceQPlay(SOUND_VC_CRASH);
		}
	}
	return(where_onwall | where_onslab | where_onball | where_onenmy);
}

/* This function performs AI for the aliens and transforms them */

static void HandleKillEnmy(OBJECT *objc)
{
	short i,k;
	short x,y,z;
	short angley;
	short speedf, speedy;
	
	/* Update the score */
	CurrentScore += SCORE_KILLENEMY;
	
	/* One more enemy killed */
	EnmyKilled ++;
	
	/* Find the initial position of object */
	x = objc->Pos.coord.t[0];
	y = objc->Pos.coord.t[1];
	z = objc->Pos.coord.t[2];
	
	/* Prepare the explosion data */
	ExplosionMakeData(objc->Model->ModelPtr, 0, &Cube[0].Pos, x,y,z);
		
	/* How long for total state */
	objc->ChangeStateCounter   = 0;
	objc->ChangeStateThreshold = 300 + rand() % 1501;

	/* Clear the alive attribute of the object */
	objc->State &= ~STATE_ALIVE;
	
	/* Change object's state */
	objc->State &= ~STATE_ENMY_MASK;
	objc->State |= STATE_ENMY_DEAD;
	
	/* One less life for this enemy */
	objc->Lives--;
	
	/* Create enemy remains */
	for (k = i = 0; (k < BALL_NOPER_ENMY) && (i < BALL_NUM); i++) {
		/* Find out the next ball object not in use */
		if (Ball[i].State & STATE_ALIVE)
			continue;
			
		/* State of the object */
		Ball[i].State = (STATE_ALIVE | STATE_STATIONARY);
		
		/* Set the type and model for this object */
		Ball[i].Type = MODEL_BALL;
		Ball[i].Model = &Models[ Ball[i].Type ];
		
		/* Initialize object inclination */
		Ball[i].Angle.vx = 0;
		Ball[i].Angle.vy = 0;
		Ball[i].Angle.vz = 0;
		
		/*
		 * Calculate the minimum/maximum x/y/z distances of each face
		 * of the binding box depending upon the inclination of the
		 * object wrt the z,y,x axes.
		 */
		Ball[i].DifX = Ball[i].Model->LenX;
		Ball[i].DifY = Ball[i].Model->LenY;
		Ball[i].DifZ = Ball[i].Model->LenZ;
		
		/* Initialize object handling structure */
		InitObjc(&Ball[i], &Cube[0].Pos, x, y, z);
		
		/*
		 * Set the initial speed of the object in vertical and
		 * forward directions as well as it's directional vector
		 */
		angley = rand() % ONE;
		speedf = BALL_SPEED_FMIN + rand() % BALL_SPEED_FRAND;
		speedy = BALL_SPEED_VMIN + rand() % BALL_SPEED_VRAND;
		
		/* Add the effect of the enemies as well as Robo's movement to this ball */
		Ball[i].SpeedX = ((speedf * MySin(angley)) +
					  		(objc->SpeedForward * MySin(objc->Angle.vy)) +
					  		(Robo.SpeedForward * MySin(Robo.Angle.vy))) >> 10;
		
		Ball[i].SpeedY = (speedy + objc->SpeedY + Robo.SpeedY);
		
		Ball[i].SpeedZ = ((speedf * MyCos(angley)) +
							(objc->SpeedForward * MyCos(objc->Angle.vy)) +
							(Robo.SpeedForward * MyCos(Robo.Angle.vy))) >> 10;
		
		/* Set the rotation vector of the object */
		Ball[i].AuxilliaryVarOne = BALL_ROTATION_MIN + rand() % BALL_ROTATION_RAND;
		
		/* Set the life of this remain of the enemy */
		Ball[i].ChangeStateCounter = 0;
		Ball[i].ChangeStateThreshold = BALL_LIFETIME_MIN + rand() % BALL_LIFETIME_RAND; 
		
		/* Create next enemy remain */
		k++;
	}
	return;
}	

/*
 * Handle ball movement (the enemy remains)
 * ----------------------------------------------------------------------------
 */

/* Handle ball */

static void HandleBall(void)
{
	short k;
	short speedx, speedy, speedz;
	short dx, dy, dz;
	short where;
	
	/*
	 * Freeze enemy remains when there is an animation in progress
	 * due to reflection of robot from other objects or wall.
	 */
#if REFLECT_OBJECT >= 1
	if (ObjcNewAngleYChange != 0)
		return;
#endif
	
	/* Handle all the enemy remains which are active */	
	for (k = 0; k < BALL_NUM; k++) {
		/* Check if the ball is alive */
		if (!(Ball[k].State & STATE_ALIVE))
			continue;
		
		/* Check if the balls life time if over */
		if (++Ball[k].ChangeStateCounter >= Ball[k].ChangeStateThreshold) {
			Ball[k].State &= ~STATE_ALIVE;
			continue;
		}
		
		/* Get the object's speed and displacements in x,y,z directions */
		dx = speedx = Ball[k].SpeedX;
		dy = speedy = Ball[k].SpeedY;
		dz = speedz = Ball[k].SpeedZ;
		
		/* Update speed in vertical direction due to gravity */
		speedy += BALL_SPEED_GSTEP;
		
		/* Handle the collision of the object with other objects */
		where = HandleBallCollision(&Ball[k], &dx, &dy, &dz);
		
		/*
		 * The speed of the object will change in all directions depending
		 * upon the direction of the collision.  The objects direction also
		 * changes along with it's speed after collision.
		 */
		if (where & COLLISION_X)	speedx = -(speedx * 2 / 3);
		if (where & COLLISION_Y)	speedy = -(speedy * 2 / 3);
		if (where & COLLISION_Z)	speedz = -(speedz * 2 / 3);
		
		/*
		 * If the object gets pinned to the wall or another object,
		 * whenever the user applies forward/backward force and there
		 * is a wall or another object in the direction of the movement.
		 * The object will stop moving in vertical as well as horizontal
		 * direction.  The speed in horizontal direction is automatically
		 * controlled by the press of the up/down directional buttons
		 * while pinning the player object with another object.  We need
		 * to control the vertical speed of the object here when such a
		 * situation develops.
		 */
		if ((where & COLLISION_HORZ) && (dx == 0) && (dy == 0) && (dz == 0))
			speedy = 0;
		
		/* Update the object's new position */
		Ball[k].Pos.coord.t[0] += dx;	// New position wrt x axis
		Ball[k].Pos.coord.t[1] += dy;	// New position wrt y axis
		Ball[k].Pos.coord.t[2] += dz;	// New position wrt z axis 
		
		/* Set the objects new inclination */
		if (Ball[k].State & STATE_XYZ_MASK) {
			/* Ball keeps rotating if it is moving */
			Ball[k].Angle.vx += Ball[k].AuxilliaryVarOne;
			Ball[k].Angle.vy += Ball[k].AuxilliaryVarOne;
			Ball[k].Angle.vz += Ball[k].AuxilliaryVarOne;
		}
		else {
			/* Ball lies flat with no rotation if it is stationary */
			Ball[k].Angle.vx = 0;
			Ball[k].Angle.vy = 0;
			Ball[k].Angle.vz = 0;
		}
		
	 	/* Also re-calculate the matrix and coordinates of the object */
		HandleSetCoordinate(&Ball[k]);	// Re-calculate object's matrix
		
		/*
		 * Save object's position for the last given number of  frames -
		 * i.e. we use a FIFO of given number of  entries to save the
		 * position of the object in the world.  The following function
		 * calculates the object's state by examining the object's position
		 * in the last given number of frames.
		 */
		Ball[k].State = HandlePosFIFO(&Ball[k]);
		
		/* Update the objects extents in x,y,z directions */
		Ball[k].MinX += dx, Ball[k].MaxX += dx;
		Ball[k].MinY += dy, Ball[k].MaxY += dy;
		Ball[k].MinZ += dz, Ball[k].MaxZ += dz;
		
	 	/* Update the balls speed along x,y,z axes */
		Ball[k].SpeedX = speedx;		// Speed along x axiz
		Ball[k].SpeedY = speedy;		// Speed along y axis
		Ball[k].SpeedZ = speedz;		// Speed along z axis
	}
	return;
}

/*
 * Handle the collision of the ball with other objects.
 *
 * This routines takes the balls's displacement vector
 * and finds out if it will collide with other objects
 * if this displacement is given to it.
 *
 * First of all we compute it's collision with all other
 * object's in the world.  The last collision detection
 * out of all these collision detection sequence is the
 * one which we should be worried about.
 *
 * There may be more than one objects lying in it's path.
 * And if the farther object comes first in our order of
 * collision detection computation, it will detect that as
 * the collision.  But later in the process it comes across
 * the second object and will compute that as the next
 * collision, and how rightly so.
 *
 * On the other hand, if the closer object in the direction
 * of the displacement vector comes first in the list then
 * it will detect that as the collision and reconfigure the
 * collision vector.  So the farther object will never be
 * detected as a collidinnng object.
 *
 * So the summary is that the last collision detected in
 * the collision detection process is the actual collision.
 */

static short HandleBallCollision(OBJECT *objc, short *dx, short *dy, short *dz)
{
	short i, slabid, ballid, enmyid, roboid;
	short where_onwall, where_onslab, where_onball, where_onenmy, where_onrobo;
	short state = objc->State;
	
	/* Check the collision of the object with robot */
	roboid = -1;
	where_onrobo = HandleObjcCollision(objc, &Robo, dx, dy, dz);
	if (where_onrobo & COLLISION_XYZ) {
		roboid = 0;
	}
	
	/* Check the collision of the object with enemies */
	where_onenmy = COLLISION_CLEAR;
	enmyid = -1;
	for (i = 0; i < ENMY_NUM; i++) {
		short where = HandleObjcCollision(objc, &Enmy[i], dx, dy, dz);
		if (where & COLLISION_XYZ) {
			enmyid = i;
			where_onenmy |= where;
		}
	}
	
	/* Check the collision of the object with enemy remains */
	where_onball = COLLISION_CLEAR;
	ballid = -1;
	for (i = 0; i < BALL_NUM; i++) {
		short where = HandleObjcCollision(objc, &Ball[i], dx, dy, dz);
		if (where & COLLISION_XYZ) {
			ballid = i;
			where_onball |= where;
		}
	}
	
	/* Check the collision of the object with slabs */
	where_onslab = COLLISION_CLEAR;
	slabid = -1;
	for (i = 0; i < SLAB_NUM; i++) {
		short where = HandleObjcCollision(objc, &Slab[i], dx, dy, dz);
		if (where & COLLISION_XYZ) {
			slabid = i;
			where_onslab |= where;
		}
	}
	
	/* Check the collision of the object with the walls of the cube */
	where_onwall = HandleWallCollision(objc, &Cube[0], dx, dy, dz);
	
	/*
	 * Calculate whether there was any collision and if so
	 * play the right sound effect or whatever you want to do.
	 * The sound effect is played only when there is a collision
	 * with another object and the object is still changing it's
	 * position along any of the x,y,z axes.  The object's state
	 * is calculated by checking the object's position is given
	 * last number of frames.
	 *
	 * Please also make note of the fact that we do the following
	 * checks in exactly the reverse order of the collision computaion
	 * donw above.  This will make sure that we look at the last
	 * collision computation first.
	 */
	if (((where_onwall & COLLISION_X) && (state & STATE_X_CHANGING)) ||
		((where_onwall & COLLISION_Y) && (state & STATE_Y_CHANGING)) ||
		((where_onwall & COLLISION_Z) && (state & STATE_Z_CHANGING))) {
		/* Do whatever you want to do here */
	}
	else
	if (((where_onslab & COLLISION_X) && (state & STATE_X_CHANGING)) ||
		((where_onslab & COLLISION_Y) && (state & STATE_Y_CHANGING)) ||
		((where_onslab & COLLISION_Z) && (state & STATE_Z_CHANGING))) {
		/* Do whatever you want to do here */
	}
	else
	if (((where_onball & COLLISION_X) && (state & STATE_X_CHANGING)) ||
		((where_onball & COLLISION_Y) && (state & STATE_Y_CHANGING)) ||
		((where_onball & COLLISION_Z) && (state & STATE_Z_CHANGING))) {
		/* Do whatever you want to do here */
	}
	else
	if (((where_onenmy & COLLISION_X) && (state & STATE_X_CHANGING)) ||
		((where_onenmy & COLLISION_Y) && (state & STATE_Y_CHANGING)) ||
		((where_onenmy & COLLISION_Z) && (state & STATE_Z_CHANGING))) {
		/* Do whatever you want to do here */
	}
	else
	if (((where_onrobo & COLLISION_X) && (state & STATE_X_CHANGING)) ||
		((where_onrobo & COLLISION_Y) && (state & STATE_Y_CHANGING)) ||
		((where_onrobo & COLLISION_Z) && (state & STATE_Z_CHANGING))) {
		if (roboid == 0) {
			HandleKillBall(objc);
			SoundVoiceQPlay(SOUND_VC_GRAB);
		}
	}
	return(where_onwall | where_onslab | where_onball | where_onenmy);
}

/* Kill the Ball */

static void HandleKillBall(OBJECT *objc)
{
	/* Update the score */
	CurrentScore += SCORE_EATREMAINS;
	
	/* Clear the alive attribute of the object */
	objc->State &= ~STATE_ALIVE;
	return;
}

/*
 * Change Lighting sources/Ambient light
 * ----------------------------------------------------------------------------
 */

static void HandleLite(void)
{
	if (PadData & PADRleft) {
		if (PadData & PADL2)	LightingPoints[0].r ++;
		if (PadData & PADR2)	LightingPoints[1].r ++;
		if (PadData & PADL1)	LightingPoints[2].r ++;
		if (PadData & PADR1)	Ambient.vx += 8, Ambient.vx %= ONE;
	}
	if (PadData & PADRup) {
		if (PadData & PADL2)	LightingPoints[0].g ++;
		if (PadData & PADR2)	LightingPoints[1].g ++;
		if (PadData & PADL1)	LightingPoints[2].g ++;
		if (PadData & PADR1)	Ambient.vy += 8, Ambient.vy %= ONE;
	}
	if (PadData & PADRright) {
		if (PadData & PADL2)	LightingPoints[0].b ++;
		if (PadData & PADR2)	LightingPoints[1].b ++;
		if (PadData & PADL1)	LightingPoints[2].b ++;
		if (PadData & PADR1)	Ambient.vz += 8, Ambient.vz %= ONE;
	}
	GsSetFlatLight(0, &LightingPoints[0]);
	GsSetFlatLight(1, &LightingPoints[1]);
	GsSetFlatLight(2, &LightingPoints[2]);
	GsSetAmbient(Ambient.vx, Ambient.vy, Ambient.vz);
	return;
}

/*
 * Handle the view point movement
 * ----------------------------------------------------------------------------
 */

static void HandleView(void)
{
	/* Calculate Matrix from Object Parameter and Set Coordinate */
	MATRIX tmp = GsIDMATRIX;
	
	/* Change the view point position and angular offsets */
	unsigned long TmpData = ((PadData >> 16) & 0xFFFF);
	if (TmpData & PADRleft) {
		if (TmpData & PADLright)	ViewPosOffsetX += 16;
		if (TmpData & PADLleft)		ViewPosOffsetX -= 16;
		if (TmpData & PADLup)		ViewPosOffsetY += 16;
		if (TmpData & PADLdown)		ViewPosOffsetY -= 16;
		if (TmpData & PADL1)		ViewPosOffsetZ += 16;
		if (TmpData & PADR1)		ViewPosOffsetZ -= 16;
	}
	if (TmpData & PADRup) {
		if (TmpData & PADLright)	ViewRefOffsetX += 16;
		if (TmpData & PADLleft)		ViewRefOffsetX -= 16;
		if (TmpData & PADLup)		ViewRefOffsetY += 16;
		if (TmpData & PADLdown)		ViewRefOffsetY -= 16;
		if (TmpData & PADL1)		ViewRefOffsetZ += 16;
		if (TmpData & PADR1)		ViewRefOffsetZ -= 16;
	}
	if (TmpData & PADRright) {
		if (TmpData & PADLup)		ViewAngleOffsetX += 16;
		if (TmpData & PADLdown)		ViewAngleOffsetX -= 16;
		if (TmpData & PADLright)	ViewAngleOffsetY += 16;
		if (TmpData & PADLleft)		ViewAngleOffsetY -= 16;
		if (TmpData & PADL1)		ViewAngleOffsetZ += 16;
		if (TmpData & PADR1)		ViewAngleOffsetZ -= 16;
		
		ViewAngleOffsetX = (ViewAngleOffsetX + ONE) % ONE;
		ViewAngleOffsetY = (ViewAngleOffsetY + ONE) % ONE;
		ViewAngleOffsetZ = (ViewAngleOffsetZ + ONE) % ONE;
	}
#if USE_GsVIEW2 >= 1	
	/* Set viewing point */
	ViewPoint.view.t[0] = ViewPosOffsetX;
	ViewPoint.view.t[1] = ViewPosOffsetY;
	ViewPoint.view.t[2] = ViewPosOffsetZ;
	
	/* set the view angle */
	ViewPointAngle.vx = ViewAngleOffsetX;
	ViewPointAngle.vy = ViewAngleOffsetY;
	ViewPointAngle.vz = ViewAngleOffsetZ + Robo.Angle.vz;
	
	/* Set translation */
	tmp.t[0] = ViewPoint.view.t[0];
	tmp.t[1] = ViewPoint.view.t[1];
	tmp.t[2] = ViewPoint.view.t[2];
	
	/* Rotate Matrix */
	RotMatrix(&ViewPointAngle, &tmp);
	
	/* Set Matrix to Coordinate */
	ViewPoint.view = tmp;
	
	/* Set the world/screen matrix */
	GsSetView2(&ViewPoint);
#else
	/* Set the viewing point position */
	ViewPoint.vpx = -ViewPosOffsetX;
	ViewPoint.vpy = -ViewPosOffsetY;
	ViewPoint.vpz = -ViewPosOffsetZ;
		
	/* Set the viewing point reference */
	ViewPoint.vrx = -ViewRefOffsetX;
	ViewPoint.vry = -ViewRefOffsetY;
	ViewPoint.vrz = -ViewRefOffsetZ;
	
	/* Set the viewing point twist */
	ViewPoint.rz = (ViewAngleOffsetZ - Robo.Angle.vz) * 360;
	
	/* Set the world/screen matrix */
	GsSetRefView2(&ViewPoint);
#endif
	return;
}

/*
 * Setting automatic division flags for all divisible objects
 * ----------------------------------------------------------------------------
 */
 
static void HandlePolyDivision(void)
{
	short i,j;
	
	/* Clear automatic polygon division flags of all objects
	 * in the world
	 */
	for (i = 0; i < CUBE_NUM; i++)	Cube[i].Object.attribute &= DIV_MASK_CLEAR;
	for (i = 0; i < SLAB_NUM; i++)	Slab[i].Object.attribute &= DIV_MASK_CLEAR;
	for (i = 0; i < ENMY_NUM; i++)	Enmy[i].Object.attribute &= DIV_MASK_CLEAR;
	for (i = 0; i < BALL_NUM; i++)	Ball[i].Object.attribute &= DIV_MASK_CLEAR;
	Robo.Object.attribute &= DIV_MASK_CLEAR;
	Prop.Object.attribute &= DIV_MASK_CLEAR;
	
	/* Check if the automatic polygon division for any of
	 * the walls or the slabs needs to set because of Robo
	 * being close to them.
	 */ 
	for (i = 0; i < SLAB_NUM; i++) {
		HandleObjcDivision(&Robo, &Slab[i]);
	}
	HandleWallDivision(&Robo, &Cube[0]);
	
	/* Check if the automatic polygon division for any of
	 * the walls or the slabs needs to set because of any
	 * of the enemies being close to them.
	 */ 
	for (j = 0; j < ENMY_NUM; j++) {
		for (i = 0; i < SLAB_NUM; i++) {
			HandleObjcDivision(&Enmy[j], &Slab[i]);
		}
		HandleWallDivision(&Enmy[j], &Cube[0]);
	}
	
	/* Check if the automatic polygon division for any of
	 * the walls or the slabs needs to set because of any
	 * of the enemie remains being close to them.
	 */ 
	for (j = 0; j < BALL_NUM; j++) {
		for (i = 0; i < SLAB_NUM; i++) {
			HandleObjcDivision(&Ball[j], &Slab[i]);
		}
		HandleWallDivision(&Ball[j], &Cube[0]);
	}
	return;
}

/*
 * Prepare the screen frame
 * ----------------------------------------------------------------------------
 */

static void DrawScreen(void)
{
	short i;
	int vsync_cnt;
	GsOT *ot;
	CurrDB = &(WorldDB[ GsGetActiveBuff() ]);				// Get active buffer ID
	ot = &CurrDB->OT;										// Get the current ordering table ptr
	GsSetWorkBase((PACKET *)CurrDB->GpuPacketArea);			// Set GPU packet generation area
	GsClearOt(0, 0, ot);									// Clear contents of OT
	for (i = 0; i < CUBE_NUM; i++) DrawObject(&Cube[i],ot);	// Add cube faces
	for (i = 0; i < SLAB_NUM; i++) DrawObject(&Slab[i],ot);	// Add slabs
	for (i = 0; i < ENMY_NUM; i++) DrawObject(&Enmy[i],ot);	// Add enmies
	for (i = 0; i < BALL_NUM; i++) DrawObject(&Ball[i],ot);	// Add enmies remains
	DrawObject(&Robo, ot);									// Add robot
	DrawObject(&Prop, ot);									// Add propeller
	ExplosionDrawData(ot, 14-OT_LENGTH);					// Draw explosion data
	DrawSync(0);											// Waiting for end of drawing
	vsync_cnt = VSync(2);									// Wait for V-BLNK
	GsSwapDispBuff();										// Replace double buffer
	GsSortClear(BGCOL_R, BGCOL_G, BGCOL_B, ot);				// Insert screen clear command at start of OT
	GsDrawOt(ot);											// Start drawing contents of OT as background
	DrawDebug(vsync_cnt);									// Print debug/help info on screen
	return;
}

/*
 * Displays debugging info on the screen
 * Also handles the idle sitiation - whenever the user does not
 * press any button for say for five minutes, starts showing the
 * credits screen
 */

static void DrawDebug(int vscnt)
{
	/*
	 * Handle the credit screen whenever the user
	 * is idle for 30 seconds
	 */
	IdleTime = (PadData != 0) ? 0 : (IdleTime+1);
	if (IdleTime >= (30*30*1)) {
		static char	*IdleScreen[] =  {
			"",
			"", "",
			"", "",
			"", "         A Net Yaroze Demo",
			"",
			"", "      Partial Copyright, 1997",
			"", " Sony Computer Entertainment, Inc.",
			"",
			"", "     Produced by  Bill Rehbock",
			"", "   Authored by Pradip Fatehpuria",
			"", "Sound Effects/Music by  Stan Weaver",
			};
		short i;
		for (i = 0; i < 19; i++) {
			FntPrint("%s\n", IdleScreen[i]);	
		}
		FntFlush(-1);
		return;
	}
	
	/* Show the
	 * Frame generation time,
	 * Current score,
	 * Player lives left,
	 * Enemies killed
	 */
	FntPrint("Time : %d\n", vscnt);
	FntPrint("Score : %d\n", CurrentScore);
	FntPrint("Lives left : %d\n", Robo.Lives);
	FntPrint("Enemies Killed : %d\n", EnmyKilled);
	FntPrint("\n");	
			
	/* Handle the Debugging information */
	if ((PadData >> 16) & PADStart) {
		ShowDebug = (ShowDebug ? FALSE : TRUE);
	}
	
	/* Quit if no debug info to be shown */
	if (ShowDebug == FALSE) {
		FntFlush(-1);
		return;
	}
	
//	ExplosionDebugData();
#if USE_GsVIEW2 >= 1
	FntPrint("Camera Position : %d %d %d\n", ViewPoint.view.t[0], ViewPoint.view.t[1], ViewPoint.view.t[2]);
	FntPrint("Camera Angle    : %d %d %d\n", ViewPointAngle.vx, ViewPointAngle.vy, ViewPointAngle.vz);
	FntPrint("\n");	
	FntPrint("View Position   : %d %d %d\n", ViewPosOffsetX, ViewPosOffsetY, ViewPosOffsetZ);
	FntPrint("View Angle      : %d %d %d\n", ViewAngleOffsetX, ViewAngleOffsetY, ViewAngleOffsetZ);
	FntPrint("\n");	
#else
	FntPrint("Camera Position : %d %d %d\n", ViewPoint.vpx, ViewPoint.vpy, ViewPoint.vpz);
	FntPrint("Camera Reference: %d %d %d\n", ViewPoint.vrx, ViewPoint.vry, ViewPoint.vrz);
	FntPrint("Camera Twist    : %d\n", ViewPoint.rz);
	FntPrint("\n");	
	FntPrint("View Position   : %d %d %d\n", ViewPosOffsetX, ViewPosOffsetY, ViewPosOffsetZ);
	FntPrint("View Reference  : %d %d %d\n", ViewRefOffsetX, ViewRefOffsetY, ViewRefOffsetZ);
	FntPrint("View Angle      : %d\n", ViewAngleOffsetZ);
	FntPrint("\n");	
#endif
//	FntPrint("Light Number 0  : %d %d %d\n", LightingPoints[0].r, LightingPoints[0].g, LightingPoints[0].b);
//	FntPrint("Light Number 1  : %d %d %d\n", LightingPoints[1].r, LightingPoints[1].g, LightingPoints[1].b);
//	FntPrint("Light Number 2  : %d %d %d\n", LightingPoints[2].r, LightingPoints[2].g, LightingPoints[2].b);
//	FntPrint("Ambient Light   : %d %d %d\n", Ambient.vx, Ambient.vy, Ambient.vz);
//	FntPrint("\n");	
//	FntPrint("Robot Matrix    : %d %d %d\n", Robo.Pos.coord.m[0][0], Robo.Pos.coord.m[0][1], Robo.Pos.coord.m[0][2]);
//	FntPrint("                  %d %d %d\n", Robo.Pos.coord.m[1][0], Robo.Pos.coord.m[1][1], Robo.Pos.coord.m[1][2]);
//	FntPrint("                  %d %d %d\n", Robo.Pos.coord.m[2][0], Robo.Pos.coord.m[2][1], Robo.Pos.coord.m[2][2]);
	FntPrint("Robot (x,y,z)   : %d %d %d\n", Robo.Pos.coord.t[0], Robo.Pos.coord.t[1], Robo.Pos.coord.t[2]);
	FntPrint("Robot Angle     : %d %d %d\n", Robo.Angle.vx, Robo.Angle.vy, Robo.Angle.vz);
	FntPrint("\n");	
	FntPrint("Robot State     : %d\n", Robo.State);
	FntPrint("Robot Y Speed   : %d\n", Robo.SpeedY);
	FntPrint("Robot F Speed   : %d\n", Robo.SpeedForward);
	FntPrint("\n");	
//	FntPrint("Propeller Speed : %d\n", Prop.SpeedAngleY);
//	FntPrint("Propeller SFX   : %d\n", Prop.SpeedAngleYPlaySfx);
//	FntPrint("\n");	
	FntPrint("Enemy States    : 0x%x, 0x%x\n", Enmy[0].State, Enmy[1].State);
	FntPrint("                  0x%x, 0x%x\n", Enmy[2].State, Enmy[3].State);
	FntPrint("                  0x%x, 0x%x\n", Enmy[4].State, Enmy[5].State);
	FntPrint("                  0x%x, 0x%x\n", Enmy[6].State, Enmy[7].State);
	FntFlush(-1);
	return;
}

/*
 * End of File
 * ----------------------------------------------------------------------------
 */
