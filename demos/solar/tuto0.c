/************************************************************
 *															*
 *						Tuto0.c								*
 *					basic 3-D object manipulation			*
 *			   											    *															*
 *				LPGE     11/10/96							*		
 *															*
 *	Copyright (C) 1996 Sony Computer Entertainment Inc.		*
 *	All Rights Reserved										*
 *															*
 ***********************************************************/


		


#include <libps.h>

#include "asssert.h"
#include "pad.h"
#include "tim.h"
#include "tmd.h"
#include "sincos.h"


	

	// standard booleans
#define FALSE 0
#define TRUE 1

	// GPU packet space
#define PACKETMAX		(10000)
#define PACKETMAX2		(PACKETMAX*24)



	// TMD model of sphere: holds 4 spheres, identical except for colour
#define SPHERE_MODEL_ADDRESS		(0x800c0000)
#define CUBE_MODEL_ADDRESS 			(0x800d0000)


#define OT_LENGTH		(14)	  // 16384 levels of z resolution


	// Ordering Table handlers
GsOT		WorldOrderingTable[2];
	// actual Ordering Tables themselves							
GsOT_TAG	zSortTable[2][1<<OT_LENGTH];   
	// GPU packet work area
PACKET		GpuOutputPacket[2][PACKETMAX2];	 


	// distance of orbiting body from central body
	// or, distance between adjacent orbits
#define ORBIT_SIZE 500


	// constants for type of object
#define SUN 0
#define PLANET 1
#define MOON 2

	// NULL value (error detection)
#define NONE 1001

typedef struct
{
	int id;
	int alive;		 // boolean
	int type;		 // SUN, PLANET, MOON
	int which;		// index into circular list
	GsDOBJ2	handler;
	SVECTOR translate;		// movement
	int movementMomentumFlag;
	SVECTOR rotate;			// angle
	SVECTOR twist;			// change of angle
	int rotationMomentumFlag;
	GsCOORDINATE2 coord;	 // coordinate system
	int inOrbit;		// boolean
	int angle;		   // within circle
	int angleIncrement;	  // speed of orbit: amount of increment per frame
	int radius;
} ObjectHandler;

#define MAX_OBJECTS		(64)

	// the spheres: sun, moon, planets
ObjectHandler ObjectArray[MAX_OBJECTS];


	// cube that player moves around in; view is local camera on cube
ObjectHandler PlayersCube;

int MovementSpeed;
int RotationSpeed;

#define INITIAL_MOVEMENT_SPEED 150
#define INITIAL_ROTATION_SPEED 0.4

 
   
  
	// screen-to-viewpoint distance  
u_long ProjectionDistance;

	// viewpoint
GsRVIEW2 TheMainView;

	// parallel light sources						  
GsF_LIGHT lightSourceArray[3];				


   	// pad interface buffer	  
u_long PadStatus;	



u_long frameNumber = 0;					





	// prototypes

int main(void);
 

int	DealWithController(void);

void HandleAllObjects (void);
void DealWithOrbitalMovement (ObjectHandler* object);

void InitialiseAll(void);
void InitialiseView(void);
void InitialiseLighting(void);
void InitialiseObjects(void);
void HighlightObject (ObjectHandler* object);
void UnHighlightObject (ObjectHandler* object);
void InitialiseModels(void);

void SetUpSolarSystem (void);

void UpdateCoordinateSystem (SVECTOR* rotationVector,
							SVECTOR* translationVector,
							GsCOORDINATE2* coordSystem);


int rsin_sub(int angle);
int rsin (int angle);
int rcos (int angle);
double quickSin (int angle);
double quickCos (int angle);





int main (void)
{
	int	outputBufferIndex;
	MATRIX tempMatrix;
	int	i;



	InitialiseAll();



	while (1) 				 // main loop
		{
		if (DealWithController() == 0) 
			break;

		HandleAllObjects();

			// update viewpoint
		GsSetRefView2(&TheMainView);
		
			// update projection distance
		GsSetProjection(ProjectionDistance);	

			// find which buffer is active
		outputBufferIndex = GsGetActiveBuff();	

			// set address for GPU scratchpad area
		GsSetWorkBase( (PACKET*)GpuOutputPacket[outputBufferIndex]);

		GsClearOt(0, 0, &WorldOrderingTable[outputBufferIndex]);	
 
			// deal with cubes in the cubic grid :-
			// for each object:
			// get local <-> screen matrix, set light matrix,
			// set local <-> screen matrix, 
			// add object into ordering table
		for (i = 0; i < MAX_OBJECTS; i++) 
			{
			if (ObjectArray[i].alive == TRUE)
				{
				GsGetLs(&ObjectArray[i].coord, &tempMatrix);
		   
				GsSetLightMatrix(&tempMatrix);
			
				GsSetLsMatrix(&tempMatrix);
			
				GsSortObject4( &(ObjectArray[i].handler), 
					&WorldOrderingTable[outputBufferIndex], 
						14-OT_LENGTH, (u_long *)getScratchAddr(0));
				}
			}

			// find status of the controller pad
		PadStatus = PadRead();	

			// wait for end of drawing
		DrawSync(0);		
			   
			// wait for V-BLANK
		VSync(0);  
		
			// swap drawing and display buffers		
		GsSwapDispBuff();		
		
			// register clear-command: clear to colour black
		GsSortClear(0x0, 0x0, 0x0, &WorldOrderingTable[outputBufferIndex]);
		
			// register request to draw ordering table
		GsDrawOt(&WorldOrderingTable[outputBufferIndex]);

			// simple tracking of frame rate
		frameNumber++;
		FntPrint("Frame %d\n", frameNumber);
		FntFlush(-1);
    	}

		// cleanup
	ResetGraph(3);
	return 0;
}
	 



   

	// select and start together to quit 
	// start alone to pause

int DealWithController (void)
{
	if (PadStatus & PADLleft)
		{
		ObjectArray[0].translate.vx -= 50;
		}
	if (PadStatus & PADLright)
		{
		ObjectArray[0].translate.vx += 50;
		}
	if (PadStatus & PADLup)
		{
		ObjectArray[1].translate.vx -= 50;
		}
	if (PadStatus & PADLdown)
		{
		ObjectArray[1].translate.vx += 50;
		}
	if (PadStatus & PADL1)
		{
		ObjectArray[2].translate.vx -= 50;
		}
	if (PadStatus & PADL2)
		{
		ObjectArray[2].translate.vx += 50;
		}

		// this chunk: Right directional pad: move player's cube
	if (PadStatus & PADRleft)
		{
		PlayersCube.translate.vx -= MovementSpeed;
		}
	if (PadStatus & PADRright)
		{
		PlayersCube.translate.vx += MovementSpeed;
		}
	if (PadStatus & PADRup)
		{
		PlayersCube.translate.vy -= MovementSpeed;
		}
	if (PadStatus & PADRdown)
		{
		PlayersCube.translate.vy += MovementSpeed;
		}
	if (PadStatus & PADR1)
		{
		PlayersCube.translate.vz -= MovementSpeed;
		}
	if (PadStatus & PADR2)
		{
		PlayersCube.translate.vz += MovementSpeed;
		}
	
		// quit
	if (PadStatus & PADselect && PadStatus & PADstart) 
		{
		return 0;
		}

		// pause
	if (PadStatus & PADstart)
		{
		while (PadStatus & PADstart)
			{
			PadStatus = PadRead();
			}
		}

	return 1;
}




void HandleAllObjects (void)
{
	int i;

		// firstly deal with the cubic array of cubes
	for (i = 0; i < MAX_OBJECTS; i++)
		{
		if (ObjectArray[i].alive == TRUE)
			{
			if (ObjectArray[i].inOrbit == TRUE)
				{
				DealWithOrbitalMovement(&ObjectArray[i]);
				}

				// deal with movement and rotation
			UpdateCoordinateSystem (&ObjectArray[i].rotate,
							&ObjectArray[i].translate,
							&ObjectArray[i].coord);

				// update angle
			ObjectArray[i].rotate.vx += ObjectArray[i].twist.vx;
			ObjectArray[i].rotate.vy += ObjectArray[i].twist.vy;
			ObjectArray[i].rotate.vz += ObjectArray[i].twist.vz;

				// momentum or not? 
			if (ObjectArray[i].movementMomentumFlag == FALSE)
				{
				ObjectArray[i].translate.vx = 0;
				ObjectArray[i].translate.vy = 0;
				ObjectArray[i].translate.vz = 0;
				} 
			if (ObjectArray[i].rotationMomentumFlag == FALSE)
				{
				ObjectArray[i].twist.vx = 0;
				ObjectArray[i].twist.vy = 0;
				ObjectArray[i].twist.vz = 0;
				}
			}
		}

		// now deal with player's cube
	UpdateCoordinateSystem( &PlayersCube.rotate,
							&PlayersCube.translate,
							&PlayersCube.coord);

		// update angle vector
	PlayersCube.rotate.vx += PlayersCube.twist.vx;
	PlayersCube.rotate.vy += PlayersCube.twist.vy;
	PlayersCube.rotate.vz += PlayersCube.twist.vz;

		// momentum or not? 
	if (PlayersCube.movementMomentumFlag == FALSE)
		{
		PlayersCube.translate.vx = 0;
		PlayersCube.translate.vy = 0;
		PlayersCube.translate.vz = 0;
		}
	if (PlayersCube.movementMomentumFlag == FALSE)
		{
		PlayersCube.twist.vx = 0;
		PlayersCube.twist.vy = 0;
		PlayersCube.twist.vz = 0;
		}
}


	// blagg: just rotate it around
void DealWithOrbitalMovement (ObjectHandler* object)
{
	object->twist.vz += object->angleIncrement;
}








void InitialiseAll(void)
{
	ResetGraph(0);					
	PadStatus = 0;						   

		// graphical initialisation:
		// screen resolution 512 by 240, non-interlace, use GsGPU offset
	GsInitGraph(512 ,240, GsOFSGPU|GsNONINTER, 1, 0);	
									
	GsDefDispBuff(0, 0, 0, 256);   
	GsInit3D();						

	WorldOrderingTable[0].length = OT_LENGTH;	  
	WorldOrderingTable[1].length = OT_LENGTH;		
	WorldOrderingTable[0].org = zSortTable[0];	 
	WorldOrderingTable[1].org = zSortTable[1];	   
			  
	InitialiseObjects();
	InitialiseModels();			
	InitialiseView();				  
	InitialiseLighting();	

	SetUpSolarSystem();
	
		// sort out print-to-screen font
	FntLoad(960, 256);	
	FntOpen(32, 32, 256, 200, 0, 512); 

		// set up controller pad buffers
	PadInit(); 	   			 
}







void InitialiseObjects (void)
{
	int i;

		// firstly initialise the cubes in the grid
	for (i = 0; i < MAX_OBJECTS; i++)
		{
		ObjectArray[i].id = i;
		ObjectArray[i].alive = FALSE;
		ObjectArray[i].type = NONE;
		ObjectArray[i].which = NONE;

			// start off stationary with no momentum
		ObjectArray[i].translate.vx = 0;
		ObjectArray[i].translate.vy = 0;
		ObjectArray[i].translate.vz = 0;
		ObjectArray[i].movementMomentumFlag = FALSE;

		ObjectArray[i].rotate.vx = 0;
		ObjectArray[i].rotate.vy = 0;
		ObjectArray[i].rotate.vz = 0;
		ObjectArray[i].twist.vx = 0;
		ObjectArray[i].twist.vy = 0;
		ObjectArray[i].twist.vz = 0;
		ObjectArray[i].rotationMomentumFlag = FALSE;

			// this for initialisation, rather than link to WORLD coords
		GsInitCoordinate2(WORLD, &(ObjectArray[i].coord) );

			// initialise orbital movement variables
		ObjectArray[i].inOrbit = FALSE;
		ObjectArray[i].angle = 0;
		ObjectArray[i].angleIncrement = 0;
		ObjectArray[i].radius = 0;
		}

		// now initialise the player's cube
	PlayersCube.id = NONE;
	PlayersCube.alive = TRUE;
	PlayersCube.type = NONE;
	PlayersCube.which = NONE;

		// start off stationary with no momentum
	PlayersCube.translate.vx = 0;
	PlayersCube.translate.vy = 0;
	PlayersCube.translate.vz = 0;
	PlayersCube.movementMomentumFlag = FALSE;

	PlayersCube.rotate.vx = 0;
	PlayersCube.rotate.vy = 0;
	PlayersCube.rotate.vz = 0;
	PlayersCube.twist.vx = 0;
	PlayersCube.twist.vy = 0;
	PlayersCube.twist.vz = 0;
	PlayersCube.rotationMomentumFlag = FALSE;

		// this for initialisation, rather than link to WORLD coords
	GsInitCoordinate2(WORLD, &(PlayersCube.coord) );

		// initialise orbital movement variables
	PlayersCube.inOrbit = FALSE;
	PlayersCube.angle = 0;
	PlayersCube.angleIncrement = 0;
	PlayersCube.radius = 0;

		// overall speed for moving player's cube
	MovementSpeed = INITIAL_MOVEMENT_SPEED;
	RotationSpeed = INITIAL_ROTATION_SPEED;

		// push player's cube away from solar system
		// so player can see where things are
	PlayersCube.translate.vz -= 2500;
}

   


	// here, we highlight the selected object by making it
	// semi-transparent
void HighlightObject (ObjectHandler* object)
{
	object->handler.attribute |= 1<<30;
}



	// standard way of handling 3D object: set GsDOBJ2 attribute to zero
void UnHighlightObject (ObjectHandler* object)
{
	object->handler.attribute = 0;
}





	// link TMD 3D object models to object handlers
void InitialiseModels (void)
{
	int i;
	
	for (i = 0; i < MAX_OBJECTS; i++)
		{
			// CHANGE THIS CALL BELOW: (i%3) MOST DUBIOUS
		LinkObjectHandlerToTmdObject( &(ObjectArray[i].handler), 
					i % 3, SPHERE_MODEL_ADDRESS);

			// each object has its own independent coordinate system
		ObjectArray[i].handler.coord2 = &(ObjectArray[i].coord);
		}   

		
		// now sort the player's cube
	LinkObjectHandlerToTmdObject( &(PlayersCube.handler), 
					0, CUBE_MODEL_ADDRESS);

		// player's cube has its own independent coordinate system
	PlayersCube.handler.coord2 = &(PlayersCube.coord);
}
	




	// set up the viewpoint
void InitialiseView (void)
{
		// screen-to-viewpoint distance
	ProjectionDistance = 2000;

		// register screen-to-viewpoint distance in graphics system
	GsSetProjection(ProjectionDistance);		

		// viewpoint is z-wise back from reference point
		// ie z increases going further into screen, standard view	

		// viewpoint is at centre of cube ??????
	TheMainView.vpx = 0; 
	TheMainView.vpy = 0; 
	TheMainView.vpz = -12000;
	
		// reference point at centre of world
	TheMainView.vrx = 0; 
	TheMainView.vry = 0; 
	TheMainView.vrz = 0;
   
	TheMainView.rz = 0;
	
	TheMainView.super = &PlayersCube.coord;

   	GsSetRefView2(&TheMainView);
}



	// set up lighting: three parallel light sources
void InitialiseLighting (void)
{
	lightSourceArray[0].vx = 0; 
	lightSourceArray[0].vy = 0; 
	lightSourceArray[0].vz = 100;
	
	lightSourceArray[0].r=0xd0; 
	lightSourceArray[0].g=0; 
	lightSourceArray[0].b=0;
	
	GsSetFlatLight(0, &lightSourceArray[0]);

	lightSourceArray[1].vx = 0; 
	lightSourceArray[1].vy = 100; 
	lightSourceArray[1].vz = 0;

	lightSourceArray[1].r=0; 
	lightSourceArray[1].g=0xd0; 
	lightSourceArray[1].b=0;

	GsSetFlatLight(1, &lightSourceArray[1]);
  
	lightSourceArray[2].vx = 100; 
	lightSourceArray[2].vy = 0; 
	lightSourceArray[2].vz = 0;

	lightSourceArray[2].r = 0; 
	lightSourceArray[2].g = 0; 
	lightSourceArray[2].b = 0xd0;

	GsSetFlatLight(2, &lightSourceArray[2]);

		// set background lighting
	GsSetAmbient(ONE/4, ONE/4, ONE/4);

		// set lighting mode
	GsSetLightMode(0);
}





void SetUpSolarSystem (void)
{
		// only one Sun
	ObjectArray[0].alive = TRUE;
	ObjectArray[0].type = SUN;
	ObjectArray[0].which = 1;
		// sun's coordinate system is relative to WORLD
	ObjectArray[0].coord.super = WORLD; 

   		// now set up the spin that makes the planet orbit
	ObjectArray[0].inOrbit = TRUE;
	ObjectArray[0].angle = 0;
		// set speed of orbit
	ObjectArray[0].angleIncrement = ONE/1440;		// quarter degree

	ObjectArray[0].radius = ORBIT_SIZE;  

		// the single planet
	ObjectArray[1].alive = TRUE;
	ObjectArray[1].type = PLANET;
	ObjectArray[1].which = 1;
		// planet's coordinate system is relative to the sun
	ObjectArray[1].coord.super = &ObjectArray[0].coord;
 
		// now set up the spin that makes the moon orbit
	ObjectArray[1].inOrbit = TRUE;
	ObjectArray[1].angle = 0;
		// set speed of orbit
	ObjectArray[1].angleIncrement = ONE/180;	// two degrees
	ObjectArray[1].radius = 2 * ORBIT_SIZE;
	
		// move planet away from sun	
	ObjectArray[1].translate.vx += 2 * ORBIT_SIZE;

		// the single moon
	ObjectArray[2].alive = TRUE;
	ObjectArray[2].type = MOON;
	ObjectArray[2].which = 1; 
		// moon's coordinate system is relative to the planet
	ObjectArray[2].coord.super = &ObjectArray[1].coord;

		// move moon away from planet
	ObjectArray[2].translate.vx += ORBIT_SIZE;
}




	// tell coordinate system about translation and rotation								 
void UpdateCoordinateSystem (SVECTOR* rotationVector,
							SVECTOR* translationVector,
							GsCOORDINATE2* coordSystem)
{
   	MATRIX	tempMatrix;

		// copy translation part of position matrix
	tempMatrix.t[0] = coordSystem->coord.t[0];
	tempMatrix.t[1] = coordSystem->coord.t[1];
	tempMatrix.t[2] = coordSystem->coord.t[2];

		// get rotation matrix from rotation vector
	RotMatrix(rotationVector, &tempMatrix);

		// assign new matrix to coordinate system
	coordSystem->coord = tempMatrix;

		// set position by relative coordinates:
		// update from last time (momentum)
	coordSystem->coord.t[0] += translationVector->vx;
	coordSystem->coord.t[1] += translationVector->vy;
	coordSystem->coord.t[2] += translationVector->vz;

		// tell GTE that coordinate system has been updated
	coordSystem->flg = 0;
}


 






	// sin-cos lookup-table-access functions
	// see "sincos.h" for explanation
	// PUT THIS INTO sincos.c in useful\minimods

int rsin_sub(int angle)
{
	if (angle <= 2048)	
		{
		if (angle <= 1024) 
			{
			return (SinCosTable[angle]);
			}
		return (SinCosTable[2048-angle]);
		} 
	else 
		{
		if (angle <= 3072) 
			{
			return (-SinCosTable[angle-2048]);
			}
		return (-SinCosTable[4096-angle]);
		}
}



int rsin (int angle)
{
	if (angle < 0) 
		{
		return (-rsin_sub(-angle & 0x0fff));
		}
	else 
		{
		return (rsin_sub(angle & 0x0fff));
		}
}



int rcos (int angle)
{
    if (angle < 0) 
    	{
    	angle = -angle;
    	}

		// keep within range: & value is (ONE-1)
    angle &= 0x0fff;

	if (angle <= 2048)
		{
		if (angle <= 1024) 
			{
			return (SinCosTable[1024-angle]);
			}
		return (-SinCosTable[angle-1024]);
		} 
	else 
		{
		if (angle <= 3072)
			{
			return (-SinCosTable[3072-angle]);
			}
		return (SinCosTable[angle-3072]);
		}
}






	// this will be slower, but return in expected range (1 to -1)
double quickSin (int angle)
{
	double result;

	result = ((double) rsin(angle)) / 4096;	

	return result;
}


	// this will be slower, but return in expected range (1 to -1)
double quickCos (int angle)
{
	double result;

	result = ((double) rcos(angle)) / 4096;	

	return result;
}