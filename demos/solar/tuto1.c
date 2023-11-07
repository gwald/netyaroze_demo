/************************************************************
 *															*
 *						Tuto1.c								*
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
#define ORBIT_SIZE 250


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
	SVECTOR translate;		// position
	SVECTOR velocity;		
	int movementMomentumFlag;
	SVECTOR rotate;			// angle
	SVECTOR twist;			// change of angle
	int rotationMomentumFlag;
	GsCOORDINATE2 coord;	 // coordinate system
	int inOrbit;		// boolean
	int centralBodyID;		// id of body around which object orbits
	int angle;		   // within circle
	int angleIncrement;	  // speed of orbit: amount of increment per frame
	int radius;			  // distance from central body
} ObjectHandler;

#define MAX_OBJECTS		(3)

	// the spheres: sun, moon, planets
ObjectHandler ObjectArray[MAX_OBJECTS];


	// cube that player moves around in; view is local camera on cube
ObjectHandler PlayersCube;

int MovementSpeed;
int RotationSpeed;

#define INITIAL_MOVEMENT_SPEED 150
#define INITIAL_ROTATION_SPEED 3




#define MOVE_AROUND 0
#define AFFECT_SPHERES 1

int SelectionMode;

u_long framesSinceLastSwitch;

char modeString[64];

int SelectedObject;			// id of selected object
u_long framesSinceLastSelection;
u_long framesSinceLastObjectMove;

 
   
  
	// screen-to-viewpoint distance  
u_long ProjectionDistance;

	// viewpoint
GsRVIEW2 TheMainView;

	// parallel light sources						  
GsF_LIGHT lightSourceArray[3];				


   	// pad interface buffer	  
u_long PadStatus;	


	// simplest frame counter
u_long frameNumber = 0;					





	// prototypes

int main(void);
 

int	DealWithController(void);

void HandleAllObjects (void);
void DealWithOrbitalMovement (void);
void ReCalculateRadius (ObjectHandler* object);

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

void ResetAll (void);


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
		DealWithOrbitalMovement();

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
		framesSinceLastSwitch++;
		framesSinceLastSelection++;
		framesSinceLastObjectMove++;
		FntPrint("Frame %d\n", frameNumber);
			// print selection mode
		switch(SelectionMode)
			{
			case MOVE_AROUND:
				sprintf(modeString, "%s\n", "moving around");
				break;
			case AFFECT_SPHERES:
				sprintf(modeString, "%s\n", "affect solar system");
				break;
			default:
				assert(FALSE);		// never get here
			}
		FntPrint("Mode: %s", modeString);
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
		// how much the objects get moved by user
	static int objectMovement[3] = {50, 1, 1};
	
		// Most actions are dependent on selection mode
	switch(SelectionMode)
		{
		case MOVE_AROUND:
				// this chunk: left directional pad: move player's cube
			if (PadStatus & PADLleft)
				{
				PlayersCube.velocity.vx -= MovementSpeed;
				}
			if (PadStatus & PADLright)
				{
				PlayersCube.velocity.vx += MovementSpeed;
				}
			if (PadStatus & PADLup)
				{
				PlayersCube.velocity.vy -= MovementSpeed;
				}
			if (PadStatus & PADLdown)
				{
				PlayersCube.velocity.vy += MovementSpeed;
				}
			if (PadStatus & PADL1)
				{
				PlayersCube.velocity.vz -= MovementSpeed;
				}
			if (PadStatus & PADL2)
				{
				PlayersCube.velocity.vz += MovementSpeed;
				}

			// this chunk: right directional pad: rotate player's cube
			if (PadStatus & PADRleft)
				{
                PlayersCube.twist.vz -= RotationSpeed;
				}
			if (PadStatus & PADRright)
				{
                PlayersCube.twist.vz += RotationSpeed;
				}
			if (PadStatus & PADRup)
				{
				PlayersCube.twist.vy -= RotationSpeed;
				}
			if (PadStatus & PADRdown)
				{
				PlayersCube.twist.vy += RotationSpeed;
				}
			if (PadStatus & PADR1)
				{
                PlayersCube.twist.vx -= RotationSpeed;
				}
			if (PadStatus & PADR2)
				{
                PlayersCube.twist.vx += RotationSpeed;
				}

			break;
		case AFFECT_SPHERES:
			if (framesSinceLastObjectMove >= 3)	// dummy: used to be > 10
				{
				if (PadStatus & PADLleft)
					{
					ObjectArray[SelectedObject].translate.vx 
							-= objectMovement[SelectedObject];
					if (ObjectArray[SelectedObject].inOrbit == TRUE)
						{
						ReCalculateRadius(&ObjectArray[SelectedObject]);
						}
					}
				if (PadStatus & PADLright)
					{
					ObjectArray[SelectedObject].translate.vx 
						+= objectMovement[SelectedObject];
					if (ObjectArray[SelectedObject].inOrbit == TRUE)
						{
						ReCalculateRadius(&ObjectArray[SelectedObject]);
						}
					}
				if (PadStatus & PADLup)
					{
					ObjectArray[SelectedObject].translate.vy 
						-= objectMovement[SelectedObject];
					if (ObjectArray[SelectedObject].inOrbit == TRUE)
						{
						ReCalculateRadius(&ObjectArray[SelectedObject]);
						}
					}
				if (PadStatus & PADLdown)
					{
					ObjectArray[SelectedObject].translate.vy 
						+= objectMovement[SelectedObject];
					if (ObjectArray[SelectedObject].inOrbit == TRUE)
						{
						ReCalculateRadius(&ObjectArray[SelectedObject]);
						}
					}
				framesSinceLastObjectMove = 0;
				}
				
				// change which object is selected
			if (PadStatus & PADL1)
				{
				if (framesSinceLastSelection > 10)
					{
					UnHighlightObject(&ObjectArray[SelectedObject]);
					if (SelectedObject == MAX_OBJECTS-1)
						SelectedObject = 0;
					else
						SelectedObject++;
					HighlightObject(&ObjectArray[SelectedObject]);
					framesSinceLastSelection = 0;
					}
				}
			if (PadStatus & PADL2)
				{
				ObjectArray[2].velocity.vx += 50;
				}
			break;
		default:
			assert(FALSE);		// should never get here
		}

		// toggle selection mode
	if (PadStatus & PADL1 && PadStatus & PADR1)
		{
		if (framesSinceLastSwitch > 10)
			{
			switch(SelectionMode)
				{
				case MOVE_AROUND:
					SelectionMode = AFFECT_SPHERES;
					break;
				case AFFECT_SPHERES:
					SelectionMode = MOVE_AROUND;  
					break;
				default:	
					assert(FALSE);		// should never get here
				}
			framesSinceLastSwitch = 0;
			}
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

		// reset all
	if (PadStatus & PADL2 && PadStatus & PADR2) 
		{
		ResetAll();
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
				// deal with movement and rotation
			UpdateCoordinateSystem (&ObjectArray[i].rotate,
							&ObjectArray[i].translate,
							&ObjectArray[i].coord);

				// update angle
			ObjectArray[i].rotate.vx += ObjectArray[i].twist.vx;
			ObjectArray[i].rotate.vy += ObjectArray[i].twist.vy;
			ObjectArray[i].rotate.vz += ObjectArray[i].twist.vz;

				// update position
			ObjectArray[i].translate.vx += ObjectArray[i].velocity.vx;
			ObjectArray[i].translate.vy += ObjectArray[i].velocity.vy;
			ObjectArray[i].translate.vz += ObjectArray[i].velocity.vz;

				// momentum or not? 
			if (ObjectArray[i].movementMomentumFlag == FALSE)
				{
				ObjectArray[i].velocity.vx = 0;
				ObjectArray[i].velocity.vy = 0;
				ObjectArray[i].velocity.vz = 0;
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

		// update position
	PlayersCube.translate.vx += PlayersCube.velocity.vx;
	PlayersCube.translate.vy += PlayersCube.velocity.vy;
	PlayersCube.translate.vz += PlayersCube.velocity.vz;

		// momentum or not? 
	if (PlayersCube.movementMomentumFlag == FALSE)
		{
		PlayersCube.velocity.vx = 0;
		PlayersCube.velocity.vy = 0;
		PlayersCube.velocity.vz = 0;
		}
	if (PlayersCube.movementMomentumFlag == FALSE)
		{
		PlayersCube.twist.vx = 0;
		PlayersCube.twist.vy = 0;
		PlayersCube.twist.vz = 0;
		}
}




	// sort out all orbital movement
void DealWithOrbitalMovement (void)
{
	ObjectHandler* object;
	int oldX, oldY;
	int newX, newY;
	int moveX, moveY;
	int i;

	for (i = 0; i < MAX_OBJECTS; i++)
		{
		if (ObjectArray[i].alive == TRUE
				&& ObjectArray[i].inOrbit == TRUE)
			{
			object = &ObjectArray[i];

			oldX = object->translate.vx;
			oldY = object->translate.vy;

		  		// increment angle, keep within range
			object->angle += object->angleIncrement;
			if (object->angle > ONE)
				{
				object->angle -= ONE;
				assert(object->angle >= 0);
				assert(object->angle < ONE);
				}

				// find new position
				// shift-right 12 to scale rcos/rsin output to the right size
			newX = (object->radius * rcos(object->angle)) >> 12;
			newY = (object->radius * rsin(object->angle)) >> 12;

			moveX = newX - oldX;
			moveY = newY - oldY;

				// move object
			object->velocity.vx = moveX;
			object->velocity.vy = moveY;
			}
		}
}




	// object has been moved;
	// calculate the new radius
void ReCalculateRadius (ObjectHandler* object)
{
	ObjectHandler* centralBody;
	double newRadius;
		
	centralBody = &ObjectArray[object->centralBodyID];

	newRadius 
			= pow( (centralBody->translate.vx - object->translate.vx), 2)
			+ pow( (centralBody->translate.vy - object->translate.vy), 2);

	newRadius = pow(newRadius, 0.5);

	object->radius = (int) newRadius;
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

		// to start with, player moves around
	SelectionMode = MOVE_AROUND;	 
	framesSinceLastSwitch = 0;

	SelectedObject = 0;
	framesSinceLastSelection = 0;
	framesSinceLastObjectMove = 0;
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
		ObjectArray[i].velocity.vx = 0;
		ObjectArray[i].velocity.vy = 0;
		ObjectArray[i].velocity.vz = 0;
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
		ObjectArray[i].centralBodyID = NONE;
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
	PlayersCube.velocity.vx = 0;
	PlayersCube.velocity.vy = 0;
	PlayersCube.velocity.vz = 0;
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
	PlayersCube.centralBodyID = NONE;
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
			// second argument is just 'i'
			// this call will only work for this demo of 3 spheres
			// and one cube;
			// for proper solar system, set which sphere in TMD model
			// by object type (SUN, PLANET, MOON)
		LinkObjectHandlerToTmdObject( &(ObjectArray[i].handler), 
					i, SPHERE_MODEL_ADDRESS);

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
	   
		// the single planet
	ObjectArray[1].alive = TRUE;
	ObjectArray[1].type = PLANET;
	ObjectArray[1].which = 1;
		// planet's coordinate system is relative to the sun
	ObjectArray[1].coord.super = &ObjectArray[0].coord;
 
		// now make the planet orbit the sun
	ObjectArray[1].inOrbit = TRUE;
	ObjectArray[1].centralBodyID = 0;
	ObjectArray[1].angle = 0;
		// set speed of orbit
	ObjectArray[1].angleIncrement = ONE/720;	// half a degree
	ObjectArray[1].radius = 2 * ORBIT_SIZE;
	
		// move planet away from sun	
	ObjectArray[1].translate.vx += 2 * ORBIT_SIZE;

		// the single moon
	ObjectArray[2].alive = TRUE;
	ObjectArray[2].type = MOON;
	ObjectArray[2].which = 1; 
		// moon's coordinate system is relative to the planet
	ObjectArray[2].coord.super = &ObjectArray[1].coord;

	 	 // now make the moon orbit the planet
	ObjectArray[2].inOrbit = TRUE;
	ObjectArray[2].centralBodyID = 1;
	ObjectArray[2].angle = 0;
		// set speed of orbit
	ObjectArray[2].angleIncrement = ONE/180;	// two degrees
	ObjectArray[2].radius = ORBIT_SIZE;
	
		// move planet away from sun	
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
	
		// set position by absolute coordinates	
	coordSystem->coord.t[0] = translationVector->vx;
	coordSystem->coord.t[1] = translationVector->vy;
	coordSystem->coord.t[2] = translationVector->vz;

		// tell GTE that coordinate system has been updated
	coordSystem->flg = 0;
}






void ResetAll (void)
{
	InitialiseObjects();
	InitialiseView();
	SetUpSolarSystem();
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





