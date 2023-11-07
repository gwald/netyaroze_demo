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


			 
#include <stdio.h>
#include <rand.h>
#include <libps.h>
#include "pad.h"
#include "tmd.h"




#define SCREEN_WIDTH 320			 
#define	SCREEN_HEIGHT 240




#define PACKETMAX 2048		/* Max GPU packets */
#define PACKETMAX2 (PACKETMAX*24)

static PACKET packetArea[2][PACKETMAX2]; /* GPU PACKETS AREA */



	// the lone TMD
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
  	 


/****************************************************************************
					prototypes
****************************************************************************/


void main (void);
		   
int DealWithControllerPad(void);

void InitialiseAll(void);

void SetUpTheObject (void);


void UpdateObjectCoordinates (SVECTOR* rotationVector,
							VECTOR* translationVector,
							GsCOORDINATE2* coordSystem);

	
void InitialiseView(void);

void InitialiseLighting(void);			

long* InitialiseModel(long* adrs);
		  	  

/****************************************************************************
					 functions
****************************************************************************/





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

		UpdateObjectCoordinates(&TheSingleObject.rotate, &TheSingleObject.position, 
							&TheSingleObject.coord);
		
		GsSetRefView2(&view); 
	
		GsGetLs(&(TheSingleObject.coord), &tmpls);
				   
		GsSetLightMatrix(&tmpls);
					
		GsSetLsMatrix(&tmpls);
					
		GsSortObject4( &(TheSingleObject.handler), 
				&Wot[side], 
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
	long pad;
	int speedMultiplier;
  

	pad = PadRead();


		// quit program
	if (pad & PADstart && pad & PADselect)
		return 0;

		
		// how fast do we turn
	if (pad & PADselect)
		speedMultiplier = 10;
	else
		speedMultiplier = 1;
		
						
		// world-relative rotations
	if (pad & PADL1)
		TheSingleObject.rotate.vz -= TheSingleObject.rotationSpeed * speedMultiplier;
	if (pad & PADL2)
		TheSingleObject.rotate.vz += TheSingleObject.rotationSpeed * speedMultiplier;
	if (pad & PADLleft)
		TheSingleObject.rotate.vy -= TheSingleObject.rotationSpeed * speedMultiplier;
	if (pad & PADLright)
		TheSingleObject.rotate.vy += TheSingleObject.rotationSpeed * speedMultiplier;
	if (pad & PADLup)
		TheSingleObject.rotate.vx -= TheSingleObject.rotationSpeed * speedMultiplier;
	if (pad & PADLdown)
		TheSingleObject.rotate.vx += TheSingleObject.rotationSpeed * speedMultiplier;
			   

		// move object closer to/further from screen
	if (pad & PADR1)
		view.vpz += 50 * speedMultiplier;
	if (pad & PADR2)
		view.vpz -= 50 * speedMultiplier;


	return 1;
}

 



void InitialiseAll (void)
{	
	int i;

		// init controller pad
	PadInit();

		// main graphical initialisations
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

	SetUpTheObject();
		 
		// set up the basic screen-printing font
	FntLoad( 960, 256);
	FntOpen( 44, 96, 256, 200, 0, 512);		  
}



	     



void SetUpTheObject (void)
{
	TheSingleObject.modelAddress = 0;
	TheSingleObject.whichModel = -1;

	TheSingleObject.position.vx = 0;
	TheSingleObject.position.vy = 0;
	TheSingleObject.position.vz = 0;

	TheSingleObject.rotate.vx = 0;
	TheSingleObject.rotate.vy = 0;
	TheSingleObject.rotate.vz = 0;

	TheSingleObject.rotationSpeed = 0;

	GsInitCoordinate2(WORLD, &TheSingleObject.coord);

	TheSingleObject.handler.coord2 = &(TheSingleObject.coord);

	TheSingleObject.modelAddress = SINGLE_MODEL_ADDRESS;
	TheSingleObject.whichModel = 0;

	TheSingleObject.rotationSpeed = ONE/128;

	LinkObjectHandlerToTmdObject( &TheSingleObject.handler, 
						TheSingleObject.whichModel, TheSingleObject.modelAddress);
}

	
		 
		 		  


	// this function performs world-relative movement and rotation
void UpdateObjectCoordinates (SVECTOR* rotationVector,
							VECTOR* translationVector,
							GsCOORDINATE2* coordSystem)
{
	MATRIX tempMatrix;
	
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

		   
	
	   	 



void InitialiseView (void)
{
	ProjectionDistance = 192;			
	GsSetProjection(ProjectionDistance);  

	view.vrx = 0; 
	view.vry = 0; 
	view.vrz = 0; 
	view.vpx = 0; 
	view.vpy = 0; 
	view.vpz = -2000;
	view.rz = 0;
	view.super = WORLD;

	GsSetRefView2(&view);
}



   	



void InitialiseLighting (void)
{
		// two flat lights
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
}


		





long* InitialiseModel (long* adrs)
{
	long *dop;

	dop = adrs;
	dop++;
	GsMapModelingData(dop);
	dop++;
	dop++;
	return (dop);
}

