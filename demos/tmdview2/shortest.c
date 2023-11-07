/************************************************************
 *															*
 *						simplest 3D object viewer			*
 *			   											    *															*
 *				L Evans     27/01/97						*		
 *															*
 *	Copyright (C) 1996 Sony Computer Entertainment Inc.		*
 *	All Rights Reserved										*
 *															*
 ***********************************************************/

	 
//#include <stdio.h>
//#include <rand.h>
#include <libps.h>
#include "tmd.h"

#define SCREEN_WIDTH 320			 
#define	SCREEN_HEIGHT 240
		
#define PACKETMAX2 (2048*24)			  /* Max GPU packets */
static PACKET packetArea[2][PACKETMAX2]; /* GPU PACKETS AREA */

#define SINGLE_MODEL_ADDRESS 0x800c0000

#define OT_LENGTH	9		

static GsOT Wot[2];			/* Handler of OT */
static GsOT_TAG wtags[2][1<<OT_LENGTH]; 

static GsRVIEW2 view;
int ProjectionDistance;	

GsF_LIGHT TheLights[3];	
int frameNumber = 0;

typedef struct
{			
	GsDOBJ2	handler;
	u_long modelAddress;
	int whichModel;

	VECTOR position;		// position

	SVECTOR rotate;			// angle

	int rotationSpeed;

	GsCOORDINATE2 coord;	 // coordinate system	
} ObjectHandler;

ObjectHandler TheSingleObject;

	
volatile u_char *bb0, *bb1;	   // low-level pad buffers: never need to touch
  	 
	 // constants for interface	to controller pad
#define PADLup     (1<<12)
#define PADLdown   (1<<14)
#define PADLleft   (1<<15)
#define PADLright  (1<<13)
#define PADRup     (1<< 4)
#define PADRdown   (1<< 6)
#define PADRleft   (1<< 7)
#define PADRright  (1<< 5)
#define PADi       (1<< 9)
#define PADj       (1<<10)
#define PADk       (1<< 8)
#define PADl       (1<< 3)
#define PADm       (1<< 1)
#define PADn       (1<< 2)
#define PADo       (1<< 0)
#define PADh       (1<<11)
#define PADL1      PADn
#define PADL2      PADo
#define PADR1      PADl
#define PADR2      PADm
#define PADstart   PADh
#define PADselect  PADk

/****************************************************************************
					prototypes
****************************************************************************/


void main (void);	   
int DealWithControllerPad(void);
void InitialiseAll(void);
void SetUpTheObject (void);
void UpdateObjectCoordinates (SVECTOR* rotationVector,
							GsCOORDINATE2* coordSystem);
void PadInit (void);
u_long PadRead(void);


		
		  	  

/****************************************************************************
					 functions
****************************************************************************/




GsDOBJ2	handler;
u_long modelAddress;
int whichModel;
VECTOR position;		// position
SVECTOR rotate;			// angle
GsCOORDINATE2 coord;	 // coordinate system




void main (void)
{
	int	i;
	int side;			  // buffer index
	MATRIX	tmpls, tmplw;
	int hsync;

	InitialiseAll();
	
	side = GsGetActiveBuff();
	
	for(;;)
		{
		FntPrint("frame: %d\n", frameNumber);
		FntPrint("hsync: %d\n", hsync);

		frameNumber++;

		if (DealWithControllerPad() == 0)	   // quitting
			break;
					
		GsSetWorkBase((PACKET*)packetArea[side]);

		GsClearOt(0,0,&Wot[side]);

		UpdateObjectCoordinates(&TheSingleObject.rotate, &TheSingleObject.coord);
		
		GsSetRefView2(&view); 

		GsGetLs(&(TheSingleObject.coord), &tmpls);
		GsSetLightMatrix(&tmpls);		
		GsSetLsMatrix(&tmpls);		
		GsSortObject4( &(TheSingleObject.handler), &Wot[side], 
					3, getScratchAddr(0));	
					
		hsync = VSync(0);	 // leave this line out for super fast frame rate

		ResetGraph(1);
		GsSwapDispBuff();
		GsSortClear(0,0,4,&Wot[side]);
		GsDrawOt(&Wot[side]);
		side ^= 1;
		FntFlush(-1);
		}

		// program cleanup
	ResetGraph(3);
}


  
   	


int DealWithControllerPad (void)
{
	long pad = PadRead();

		// quit program
	if (pad & PADstart && pad & PADselect)
		return 0;		
						
		// world-relative rotations
	if (pad & PADL1)			TheSingleObject.rotate.vz -= ONE/64;
	if (pad & PADL2)			TheSingleObject.rotate.vz += ONE/64;
	if (pad & PADLleft)			TheSingleObject.rotate.vy -= ONE/64;
	if (pad & PADLright)		TheSingleObject.rotate.vy += ONE/64;
	if (pad & PADLup)			TheSingleObject.rotate.vx -= ONE/64;
	if (pad & PADLdown)			TheSingleObject.rotate.vx += ONE/64;	   

		// move object closer to/further from screen
	if (pad & PADR1)
		view.vpz += 50;
	if (pad & PADR2)
		view.vpz -= 50;

	return 1;
}

 



void InitialiseAll (void)
{	
	int i;

		// init controller pad
	PadInit();

		// main graphical initialisations
	GsInitGraph(SCREEN_WIDTH,SCREEN_HEIGHT,GsINTER|GsOFSGPU,1,0);
	GsDefDispBuff(0,0,0,SCREEN_HEIGHT);
	GsInit3D();		   

	Wot[0].length=OT_LENGTH;	
	Wot[0].org=wtags[0];	   
	Wot[1].length=OT_LENGTH;
	Wot[1].org=wtags[1];

	GsClearOt(0,0,&Wot[0]);
	GsClearOt(0,0,&Wot[1]);	

		// screen to viewpoint distance
	ProjectionDistance = 192;			
	GsSetProjection(ProjectionDistance);  

		// viewpoint
	view.vrx = 0; 
	view.vry = 0; 
	view.vrz = 0; 
	view.vpx = 0; 
	view.vpy = 0; 
	view.vpz = -2000;
	view.rz = 0;
	view.super = WORLD;
	GsSetRefView2(&view);
		
		// three flat lights
	TheLights[0].vx = ONE; TheLights[0].vy = 0; TheLights[0].vz = 0;
	TheLights[0].r = 128; TheLights[0].g = 0; TheLights[0].b = 0;
	GsSetFlatLight(0, &TheLights[0]);
	TheLights[1].vx = 0; TheLights[1].vy = ONE; TheLights[1].vz = 0;
	TheLights[1].r = 0; TheLights[1].g = 128; TheLights[1].b = 0;
	GsSetFlatLight(1, &TheLights[1]);
	TheLights[2].vx = 0; TheLights[2].vy = 0; TheLights[2].vz = ONE;
	TheLights[2].r = 0; TheLights[2].g = 0; TheLights[2].b = 128;
	GsSetFlatLight(2, &TheLights[2]);

		// ambient lighting
	GsSetAmbient(ONE/4, ONE/4, ONE/4);	   

	SetUpTheObject();
		 
		// set up the basic screen-printing font
	FntLoad( 960, 256);
	FntOpen( 44, 96, 256, 200, 0, 512);		  
}



	     



void SetUpTheObject (void)
{
	u_long *pointer;

	TheSingleObject.modelAddress = 0;
	TheSingleObject.whichModel = -1;

	TheSingleObject.position.vx = 0;
	TheSingleObject.position.vy = 0;
	TheSingleObject.position.vz = 0;

	TheSingleObject.rotate.vx = 0;
	TheSingleObject.rotate.vy = 0;
	TheSingleObject.rotate.vz = 0;

	GsInitCoordinate2(WORLD, &TheSingleObject.coord);

	TheSingleObject.handler.coord2 = &(TheSingleObject.coord);

		// set pointer to top of TMD in main memory
	pointer = (u_long *) SINGLE_MODEL_ADDRESS;
	pointer++;			// skip TMD header	 
	GsMapModelingData(pointer);			 // Map TMD offset addresses to real addresses
	pointer += 2;	  // skip some more
		// link TMD data to object handler	
	GsLinkObject4( (u_long)pointer, &TheSingleObject.handler, 0);
}

	
		 
		 		  


	// this function performs world-relative movement and rotation
void UpdateObjectCoordinates (SVECTOR* rotationVector,
							GsCOORDINATE2* coordSystem)
{
	MATRIX tempMatrix;

	tempMatrix.t[0] = coordSystem->coord.t[0];
	tempMatrix.t[1] = coordSystem->coord.t[1];
	tempMatrix.t[2] = coordSystem->coord.t[2];
	
		// get rotation matrix from rotation vector
	RotMatrix(rotationVector, &tempMatrix);

		// assign new matrix to coordinate system
	coordSystem->coord = tempMatrix;

	   	// tell GTE that coordinate system has been updated
	coordSystem->flg = 0;
}

void PadInit (void)
{
	GetPadBuf(&bb0, &bb1);
}

u_long PadRead(void)
{
	return(~(*(bb0+3) | *(bb0+2) << 8 | *(bb1+3) << 16 | *(bb1+2) << 24));
}


		   
	
	   	 


		 


   	


		



