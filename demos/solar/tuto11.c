/************************************************************
 *															*
 *						Tuto11.c							*
 *					basic 3-D object manipulation			*
 *			   											    *															*
 *				LPGE     21/10/96							*		
 *															*
 *	Copyright (C) 1996 Sony Computer Entertainment Inc.		*
 *	All Rights Reserved										*
 *															*
 ***********************************************************/




/*******
		Change from before:

		Spheres no longer in hierarchical coordinate system
		to enable them to spin independently.
		If they were still in the hierarchical coordinate system,
		spinning a planet would inevitably make the moons "orbit"
		by virtue of the fact that their world is spinning;
		this is no good, because they would all "orbit" in the same
		direction at the same angular speed.

		Here, the objects all have independent coordinate systems,
		orbital movement is still easy to calculate, and all the
		spheres spin autonomously
*******/



   
/**********************			libraries	  *****************************/
#include <libps.h>

#include "asssert.h"
#include "pad.h"
#include "tim.h"
#include "tmd.h"
#include "sincos.h"
#include "vector.h"


	
	
/**********************			constants and globals	  *****************/


	// standard booleans
#define FALSE 0
#define TRUE 1

	// GPU packet space
#define PACKETMAX		(10000)
#define PACKETMAX2		(PACKETMAX*24)



	// TMD model of sphere: holds 4 spheres, identical except for colour
#define SPHERE_MODEL_ADDRESS		(0x800c0000)
#define SHUTTLE_MODEL_ADDRESS 			(0x800e0000)


#define OT_LENGTH		(14)	  // 1024 levels of z resolution


	// Ordering Table handlers
GsOT		WorldOrderingTable[2];
	// actual Ordering Tables themselves							
GsOT_TAG	zSortTable[2][1<<OT_LENGTH];   
	// GPU packet work area
PACKET		GpuOutputPacket[2][PACKETMAX2];	 


	// distance of orbiting body from central body
	// or, distance between adjacent orbits
#define PLANET_ORBIT_SIZE 400
#define MOON_ORBIT_SIZE 250


	// constants for type of object
#define SUN 0
#define PLANET 1
#define MOON 2

	// NULL value (error detection)
#define NONE 1001

	// ways of going round circle
#define CLOCKWISE 1002
#define ANTICLOCKWISE 1003


typedef struct
{
	int id;
	int alive;		 // boolean
	int type;		 // SUN, PLANET, MOON
	int which;		// index into circular list
	GsDOBJ2	handler;
	double scaleX, scaleY, scaleZ;			// scaling in 3D
	VECTOR position;		// position
	VECTOR velocity;		
	int movementMomentumFlag;
	SVECTOR rotate;			// angle
	VECTOR twist;			// change of angle
	int rotationMomentumFlag;
	GsCOORDINATE2 coord;	 // coordinate system
	int inOrbit;		// boolean
	int centralBodyID;		// id of body around which object orbits
	int angle;		   // within circle
	int angleIncrement;	  // speed of orbit: amount of increment per frame
	int whichWay;		  // CLOCKWISE or ANTICLOCKWISE
	int radius;			  // distance from central body
	VECTOR firstVector;		  // these two lie on the plane of orbit
	VECTOR secondVector;	  // second is perpendicular to first
	VECTOR normalToOrbitalPlane;   // this vector defines the orbital plane
} ObjectHandler;

#define MAX_OBJECTS		(64)

#define MAX_PLANETS 3
#define MAX_MOONS 3

	// the spheres: sun, moon, planets
ObjectHandler ObjectArray[MAX_OBJECTS];


	// ship that player moves around in; view is local camera on ship
ObjectHandler PlayersShip;


	

	// player's movement and rotation speeds
int MovementSpeed;
int RotationSpeed;

	// initial values of player's movement and rotation speeds 
#define INITIAL_MOVEMENT_SPEED 150
#define INITIAL_ROTATION_SPEED 6



	// constants for user selection mode
#define MOVE_AROUND 0
#define CHANGE_LIGHTS 1
#define SET_AMBIENT_LIGHT 2
#define FOGGING 3
#define VIEW_DISTORTION 4
#define SELECT_OBJECT 5


   
	// MUST update this iff add new selection modes
#define HIGHEST_SELECTION_MODE (SELECT_OBJECT)



  




int SelectionMode;	   // current selection mode

u_long framesSinceLastSwitch;

char modeString[64];


#define MOVE_OBJECT 0
#define CHANGE_SPIN 1
#define CHANGE_DISPLAY 2
#define OBJECT_VIEWS 3

#define MAX_OBJECT_SELECTION_MODE (OBJECT_VIEWS)

int ObjectSelectionMode;
int SelectedObject;			// id of selected object
u_long framesSinceLastSelection;
char objectModeString[64];




	// three parallel light sources						  
GsF_LIGHT lightSourceArray[3];	
int SelectedLight;			// 0 to 2
int LightsChanged;			// boolean flag
u_long framesSinceLastLightChange;	 // frame counter

	// background lighting
int AmbientLightChanged;			// boolean flag
u_short AmbientRed, AmbientGreen, AmbientBlue;


	// fogging
GsFOGPARAM FoggingData;
int FoggingChanged;					// boolean flag

int OverallLightingMode;		// allows or bars fogging, etc 
int LightingModeChanged;	   	// boolean flag
u_long framesSinceLastLightModeChanged;		// frame counter
   
  
	// screen-to-viewpoint distance  
u_long ProjectionDistance;
int ProjectionChanged;


	// viewpoint
GsRVIEW2 TheMainView;
int ViewChanged;
	// object id: which object are we looking from (local camera)
	// set to -1 for WORLD, set to -2 for player's ship
int MainViewObjectID;
u_long numberFramesSinceViewpointChange;
		


   	// pad interface buffer	  
u_long PadStatus;	


	// simplest frame counter
u_long frameNumber = 0;	


			







/**********************			prototypes	  *****************************/

int main(void);


	// text printing
void GetSelectionModeString (char *string);
void PrintModeInfo (void);
void WriteObjectSelectionModeInfoIntoString (char* string);
void WriteObjectSelectionInfo (void);
 

	// controller pad handling
int	DealWithController(void);

void HandlePlayerMoving (void);
void HandleLightControls (void);
void HandleAmbientControls (void);
void HandleFoggingControls (void);
void HandleViewControls (void);
void HandleObjectSelection (void);

	// object handlimh
int FindNextLowestObjectID (int objectID);
int FindNextHighestObjectID (int objectID);

void HandleAllObjects (void);
void DealWithOrbitalMovement (void);


	// initialisers
void InitialiseAll(void);
void InitialiseView(void);
void InitialiseLighting(void);
void InitialiseObjects(void);
void InitSingleObject (ObjectHandler* object);

	// basic visual highlighting
void HighlightObject (ObjectHandler* object);
void UnHighlightObject (ObjectHandler* object);



void InitialiseModels(void);

void SetUpSolarSystem (void);

void PutObjectIntoOrbit (ObjectHandler* object, int centralBodyID,
		int initialAngle, int angleIncrement, int radius, int whichWay,
			VECTOR* firstAngle, VECTOR* secondAngle);


int RandomOrientation (void);
int RandomAngle (void);
int RandomInteger (int maximum);
void RandomVector (VECTOR* vector);
void ScaleVectorToUnit (VECTOR* vector);
void AnyUnitPerpendicularVector (VECTOR* input, VECTOR* output);


	// sort object volume
void SetObjectScaling (ObjectHandler* object, double scaleX,
						double scaleY, double scaleZ);
void SortObjectSize (ObjectHandler* object);


void UpdateCoordinateSystem (SVECTOR* rotationVector,
							VECTOR* translationVector,
							GsCOORDINATE2* coordSystem);

void UpdateCoordinateSystem2 (SVECTOR* rotationVector, VECTOR* twist,
							VECTOR* position, VECTOR* velocity,
							GsCOORDINATE2* coordSystem);

void UpdateCoordinateSystem3 (SVECTOR* rotation, VECTOR* twist,
							VECTOR* position, VECTOR* velocity,
							GsCOORDINATE2* coordSystem);

void ResetAll (void);


	// trigonometric functions (accessing lookup tables)
int rsin_sub(int angle);
int rsin (int angle);
int rcos (int angle);
double quickSin (int angle);
double quickCos (int angle);





	// debug macro: use before 'exit' when not using assertions
#define PRINT_POSITION 									\
	(printf("Arrival! at line %d of file %s.\n",		\
			__LINE__, __FILE__))


	// this would be unreadable if done within functions
	// NOTE: very slow, but easy to optimise
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



/**********************			functions	  *****************************/

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

			// update parallel lighting, if changed
		if (LightsChanged)
			{
			GsSetFlatLight(SelectedLight, &lightSourceArray[SelectedLight]);
			LightsChanged = FALSE;
			}

			// update ambient lighting, if changed	
		if (AmbientLightChanged)
			{
			GsSetAmbient(AmbientRed, AmbientGreen, AmbientBlue);
			AmbientLightChanged = FALSE;
			}

			// update fogging, if changed
		if (FoggingChanged)
			{
			GsSetFogParam(&FoggingData);
			FoggingChanged = FALSE;
			}

			// update projection distance, if changed
		if (ProjectionChanged)
			{
			GsSetProjection(ProjectionDistance);
			ProjectionChanged = FALSE;	
			}

			// update view, if changed
		if (ViewChanged)
			{
			GsSetRefView2(&TheMainView);
			ViewChanged = FALSE;
			}

			// update overall lighting mode, if changed
		if (LightingModeChanged)
			{		   // only two valid values
			assert(OverallLightingMode == 0
				|| OverallLightingMode == 1);					
			GsSetLightMode(OverallLightingMode);
			LightingModeChanged = FALSE;
			}
		  
			// find which buffer is active
		outputBufferIndex = GsGetActiveBuff();	

			// set address for GPU scratchpad area
		GsSetWorkBase( (PACKET*)GpuOutputPacket[outputBufferIndex]);

		GsClearOt(0, 0, &WorldOrderingTable[outputBufferIndex]);
		 
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

			// print player's ship
		GsGetLs(&PlayersShip.coord, &tempMatrix);
		   
		GsSetLightMatrix(&tempMatrix);
			
		GsSetLsMatrix(&tempMatrix);
			
		GsSortObject4( &PlayersShip.handler, 
					&WorldOrderingTable[outputBufferIndex], 
						14-OT_LENGTH, (u_long *)getScratchAddr(0));


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

			// main frame counter
		frameNumber++;

			// other frame counters
		framesSinceLastSwitch++;
		framesSinceLastSelection++;
		framesSinceLastLightModeChanged++;
		numberFramesSinceViewpointChange++;	

		FntPrint("Frame %d\n", frameNumber);
			// print selection mode
		GetSelectionModeString(modeString);
		FntPrint("Mode: %s", modeString);

			// Much of the text that get printed to the screen
			// is mode-dependent: hence call function that switches
			// on mode to print the appropriate business
		PrintModeInfo();

			// push text onto screen itself
		FntFlush(-1);
    	}

		// cleanup
	ResetGraph(3);
	return 0;
}




	// writes English description of selection mode into string argument
void GetSelectionModeString (char *string)
{
	switch(SelectionMode)
		{
		case MOVE_AROUND:
			sprintf(string, "%s\n", "Moving around");
			break;
		case CHANGE_LIGHTS:
			sprintf(string, "%s\n", "Change lights");
			break;
		case SET_AMBIENT_LIGHT:
			sprintf(string, "%s\n", "Change ambient lighting");
			break;
		case FOGGING:
			sprintf(string, "%s\n", "Change fogging");
			break;
		case VIEW_DISTORTION:
			sprintf(string, "%s\n", "Distort viewpoint");
			break;	
		case SELECT_OBJECT:
			sprintf(string, "%s\n", "Select object");
			break;
		default:
			PRINT_POSITION;		// should never get here
			exit(1);
		}
}





	// print text onto the screen;
	// what we print depends on user's selection mode
void PrintModeInfo (void)
{
	switch(SelectionMode)
		{
		case MOVE_AROUND:
			FntPrint("Ship pos: %d %d %d\n",
				PlayersShip.coord.coord.t[0],
				PlayersShip.coord.coord.t[1],
				PlayersShip.coord.coord.t[2]);
			FntPrint("Ship ang: %d %d %d\n",
				PlayersShip.rotate.vx,
				PlayersShip.rotate.vy,
				PlayersShip.rotate.vz);
			break;
		case CHANGE_LIGHTS:
			FntPrint("Selected light: %d\n", SelectedLight);
			assert(SelectedLight >= 0 && SelectedLight <= 2);
			FntPrint("Direction: %d %d %d\n",
					lightSourceArray[SelectedLight].vx,
					lightSourceArray[SelectedLight].vy,
					lightSourceArray[SelectedLight].vz);
			FntPrint("Colour: r %d, g %d, b %d\n",
					lightSourceArray[SelectedLight].r,
					lightSourceArray[SelectedLight].g,
					lightSourceArray[SelectedLight].b);
			break;
		case SET_AMBIENT_LIGHT:
			FntPrint("Ambient light colours:-\n");
			FntPrint("Red: %d\n", AmbientRed);
			FntPrint("Green: %d\n", AmbientGreen);
			FntPrint("Blue: %d\n", AmbientBlue);
			break;
		case FOGGING:
			FntPrint("Fog parameters:-\n");
			FntPrint("dqa %d, dqb %d\n", 
				FoggingData.dqa, FoggingData.dqb);
			FntPrint("Fog colours: r %d g %d b %d\n",
				FoggingData.rfc, FoggingData.gfc, FoggingData.bfc);
			FntPrint("Overall lighting mode: %d\n",
					OverallLightingMode);
			if (OverallLightingMode == 0)
				FntPrint("I.e. fogging is OFF\n");
			else
				{
				assert(OverallLightingMode == 1);
				FntPrint("I.e. fogging is ON\n");
				}
			break;
		case VIEW_DISTORTION:
			FntPrint("Reference: %d %d %d\n",
				TheMainView.vrx, TheMainView.vry, TheMainView.vrz);
			FntPrint("Viewpoint: %d %d %d\n",
				TheMainView.vpx, TheMainView.vpy, TheMainView.vpz);
			FntPrint("rz: %d\n", TheMainView.rz);
			FntPrint("Proj. dist: %d\n", ProjectionDistance);
			if (MainViewObjectID == -2)
				FntPrint("View super: player's ship\n");
			else if (MainViewObjectID == -1)
				FntPrint("View super: WORLD\n");
			else
				{
				assert(MainViewObjectID >= 0
					&& MainViewObjectID < MAX_OBJECTS);
				FntPrint("View super: object id %d\n", 
					MainViewObjectID);
				}
			break;	
		case SELECT_OBJECT:
			FntPrint("Selected object id: %d\n", SelectedObject);
					
				// sort printing of sub-mode
			WriteObjectSelectionModeInfoIntoString(objectModeString);
			FntPrint("submode: %s", objectModeString);

				// print data relevant to submode
			WriteObjectSelectionInfo();
			break;
		default:
			PRINT_POSITION;		// should never get here
			exit(1);
		}
}


	 



void WriteObjectSelectionModeInfoIntoString (char* string)
{
	switch(ObjectSelectionMode)
		{
		case MOVE_OBJECT:
			sprintf(string, "%s\n", "Move object");
			break;
		case CHANGE_SPIN:
			sprintf(string, "%s\n", "Change spin");
			break;
		case CHANGE_DISPLAY:
			sprintf(string, "%s\n", "Change display");
			break;
		case OBJECT_VIEWS:
			sprintf(string, "%s\n", "Change views");
			break;
		default:
			PRINT_POSITION;		// should never get here
			exit(1);
		}
}




	// print info relevant to sub-mode to screen
void WriteObjectSelectionInfo (void)
{
		// check that we have selected a valid sphere
	assert(SelectedObject >= 0);
	assert(SelectedObject < MAX_OBJECTS);
	
	switch(ObjectSelectionMode)
		{
		case MOVE_OBJECT:
			FntPrint("Position: %d %d %d\n",
				ObjectArray[SelectedObject].coord.coord.t[0],
				ObjectArray[SelectedObject].coord.coord.t[1],
				ObjectArray[SelectedObject].coord.coord.t[2]);
			if (ObjectArray[SelectedObject].inOrbit == TRUE)
				{
				FntPrint("radius of orbit: %d\n",
					ObjectArray[SelectedObject].radius);
				switch(ObjectArray[SelectedObject].whichWay)
					{
					case CLOCKWISE:
						FntPrint("orbit clockwise\n");
						break;
					case ANTICLOCKWISE:
						FntPrint("orbit anticlockwise\n");
						break;
					default:	
						PRINT_POSITION;		// should never get here
						exit(1);
					}
				}
			break;
		case CHANGE_SPIN:
			FntPrint("Angle: %d %d %d\n",
				ObjectArray[SelectedObject].rotate.vx,
				ObjectArray[SelectedObject].rotate.vy,
				ObjectArray[SelectedObject].rotate.vz);
			FntPrint("Rotational momentum: %d\n",
				ObjectArray[SelectedObject].rotationMomentumFlag);
			break;
		case CHANGE_DISPLAY:
			{
			int boolean, semiTransparencySetting = 0;

			// bit 3: toggle normal lighting / fog only mode; GsFOG
			// bit 6: toggle light calculations; GsLOFF
			// 28 and 29: semi-transparency flags; 
			// use as 1 four-value int: GsAZERO <-> GsATHREE
			// bit 30: semi-transparency on/off: GsALON
			// bit 31: display on/off: GsDOFF

			boolean = 
				ObjectArray[SelectedObject].handler.attribute & GsFOG;
			if (boolean)
				FntPrint("fog only\n");
			else
				FntPrint("normal lighting\n");
			
			boolean = 
				ObjectArray[SelectedObject].handler.attribute & GsLOFF;
			if (boolean)
				FntPrint("light calculations OFF\n");
			else
				FntPrint("light calculations ON\n");

			if (ObjectArray[SelectedObject].handler.attribute & 1<<28)
				semiTransparencySetting++;
			if (ObjectArray[SelectedObject].handler.attribute & 1<<29)
				semiTransparencySetting += 2;

			FntPrint("semi-trans type: %d\n", 
				semiTransparencySetting);

			boolean = 
				ObjectArray[SelectedObject].handler.attribute & GsALON;
			if (boolean)
				FntPrint("semi-transparency ON\n");
			else
				FntPrint("semi-transparency OFF\n");
				
			boolean = 
				ObjectArray[SelectedObject].handler.attribute & GsDOFF;
			if (boolean)
				FntPrint("display OFF\n");
			else
				FntPrint("display ON\n"); 

			}
			break;
		case OBJECT_VIEWS:
			break;
		default:
			PRINT_POSITION;		// should never get here
			exit(1);
		}
}


			


   

	// select and start together to quit 
	// start alone to pause

int DealWithController (void)
{
	static int pauseCounter = 0;

		// Most actions are dependent on selection mode
	switch(SelectionMode)
		{
		case MOVE_AROUND:		  // player moving
			HandlePlayerMoving();
			break;
		case CHANGE_LIGHTS:
			HandleLightControls();			
			break;
		case SET_AMBIENT_LIGHT:
			HandleAmbientControls();
			break;
		case FOGGING:
			HandleFoggingControls();
			break;
		case VIEW_DISTORTION:
			HandleViewControls();
			break;	
		case SELECT_OBJECT:
			HandleObjectSelection();
			break;
		default:
			PRINT_POSITION;		// should never get here
			exit(1);
		}

	// The controls below always apply:
	// change selection mode, quit, reset demo

		// change selection mode
	if (PadStatus & PADstart)
		{
		if (framesSinceLastSwitch > 10)		// not too often
			{
			if (SelectionMode == HIGHEST_SELECTION_MODE)
				SelectionMode = 0;
			else			  
				SelectionMode++;   
			if (SelectionMode == SELECT_OBJECT)
				{
				assert(SelectedObject >= 0);
				assert(SelectedObject < MAX_OBJECTS);
				HighlightObject( &ObjectArray[SelectedObject]);
				}	
			framesSinceLastSwitch = 0;
			}
		}

   		// quit
	if (PadStatus & PADselect && PadStatus & PADstart) 
		{
		return 0;
		}

		// reset all to initial state
	if (PadStatus & PADL2 && PadStatus & PADR2) 
		{
		ResetAll();
		}
		  	   
	return 1;
}






void HandlePlayerMoving (void)
{
	int speedMultiplier;			// movement rate

	if (PadStatus & PADselect)
		speedMultiplier = 10;		// fast movement
	else
		speedMultiplier = 1;		// slow movement
			
		// this chunk: left directional pad plus L1/L2: 
		// move player's ship
	if (PadStatus & PADLleft)
		{
		PlayersShip.velocity.vx -= MovementSpeed * speedMultiplier;
		ViewChanged = TRUE;
		}
	if (PadStatus & PADLright)
		{
		PlayersShip.velocity.vx += MovementSpeed * speedMultiplier;
		ViewChanged = TRUE;
		}
	if (PadStatus & PADLup)
		{
		PlayersShip.velocity.vy -= MovementSpeed * speedMultiplier;
		ViewChanged = TRUE;
		}
	if (PadStatus & PADLdown)
		{
		PlayersShip.velocity.vy += MovementSpeed * speedMultiplier;
		ViewChanged = TRUE;
		}
	if (PadStatus & PADL1)
		{
		PlayersShip.velocity.vz -= MovementSpeed * speedMultiplier;
		ViewChanged = TRUE;
		}
	if (PadStatus & PADL2)
		{
		PlayersShip.velocity.vz += MovementSpeed * speedMultiplier;
		ViewChanged = TRUE;
		}

		// this chunk: right directional pad plus R1/R2: 
		// rotate player's ship
	if (PadStatus & PADRleft)
		{
		PlayersShip.twist.vy -= RotationSpeed * speedMultiplier;
	    ViewChanged = TRUE;	 
		}
	if (PadStatus & PADRright)
		{
        PlayersShip.twist.vy += RotationSpeed * speedMultiplier;
        ViewChanged = TRUE;	  
		}
	if (PadStatus & PADRup)
		{
		PlayersShip.twist.vx -= RotationSpeed * speedMultiplier;
		ViewChanged = TRUE;	  
		}
	if (PadStatus & PADRdown)
		{
		PlayersShip.twist.vx += RotationSpeed * speedMultiplier;
		ViewChanged = TRUE;	  
		}
	if (PadStatus & PADR1)
		{
        PlayersShip.twist.vz -= RotationSpeed * speedMultiplier;
        ViewChanged = TRUE;	  
		}
	if (PadStatus & PADR2)
		{
        PlayersShip.twist.vz += RotationSpeed * speedMultiplier;
      	ViewChanged = TRUE;	   
		}
}






void HandleLightControls (void)
{
	int lightChange;			  // how fast light changes

		// only three valid values
	assert(SelectedLight >= 0 && SelectedLight <= 2);

	if (PadStatus & PADselect)	   
		lightChange = 20;
	else
		lightChange = 2;

		// these six: L-pad directional plus L1/L2:
		// alter the light colour
	if (PadStatus & PADLleft)
		{
		lightSourceArray[SelectedLight].r -= lightChange;
		LightsChanged = TRUE;
		}
	if (PadStatus & PADLright)
		{
		lightSourceArray[SelectedLight].r += lightChange;
		LightsChanged = TRUE;
		}
	if (PadStatus & PADLup)
		{
		lightSourceArray[SelectedLight].g += lightChange;
		LightsChanged = TRUE;
		}
	if (PadStatus & PADLdown)
		{
		lightSourceArray[SelectedLight].g -= lightChange;
		LightsChanged = TRUE;
		}
	if (PadStatus & PADL1)
		{
		lightSourceArray[SelectedLight].b += lightChange;
		LightsChanged = TRUE;
		}
	if (PadStatus & PADL2)
		{
		lightSourceArray[SelectedLight].b -= lightChange;
		LightsChanged = TRUE;
		}

		// these six: R-pad directional plus R1/R2:
		// alter the light direction
	if (PadStatus & PADRleft)
		{
		lightSourceArray[SelectedLight].vx -= 100 * lightChange;
		LightsChanged = TRUE;
		}
	if (PadStatus & PADRright)
		{
		lightSourceArray[SelectedLight].vx += 100 * lightChange;
		LightsChanged = TRUE;
		}
	if (PadStatus & PADRup)
		{
		lightSourceArray[SelectedLight].vy += 100 * lightChange;
		LightsChanged = TRUE;
		}
	if (PadStatus & PADRdown)
		{
		lightSourceArray[SelectedLight].vy -= 100 * lightChange;
		LightsChanged = TRUE;
		}
	if (PadStatus & PADR1)
		{
		lightSourceArray[SelectedLight].vz += 100 * lightChange;
		LightsChanged = TRUE;
		}
	if (PadStatus & PADR2)
		{
		lightSourceArray[SelectedLight].vz -= 100 * lightChange;
		LightsChanged = TRUE;
		}

		// PADstart: select next light
	if (PadStatus & PADL1 && PadStatus & PADR1)
		{
		if (framesSinceLastLightChange > 15)		// not too often
			{
			if (SelectedLight == 2)	
				SelectedLight = 0;
			else
				SelectedLight++;
			framesSinceLastLightChange = 0;
			}
		}
}
	




	// for ambient colour: can increase/decrease r, g and b
void HandleAmbientControls (void)
{
	int colourChange;			  // how fast colour changes

	if (PadStatus & PADselect)	   
		colourChange = 2000;
	else
		colourChange = 200; 

		// these six: L-pad directional plus Rup/Rdown:
		// alter the ambient colour
	if (PadStatus & PADLleft)
		{
		AmbientRed -= colourChange;
		AmbientLightChanged = TRUE;
		}
	if (PadStatus & PADLright)
		{
		AmbientRed += colourChange;
		AmbientLightChanged = TRUE;
		}
	if (PadStatus & PADLup)
		{
		AmbientGreen += colourChange;
		AmbientLightChanged = TRUE;
		}
	if (PadStatus & PADLdown)
		{
		AmbientGreen -= colourChange;
		AmbientLightChanged = TRUE;
		}
	if (PadStatus & PADRup)
		{
		AmbientBlue += colourChange;
		AmbientLightChanged = TRUE;
		}
	if (PadStatus & PADRdown)
		{
		AmbientBlue -= colourChange;
		AmbientLightChanged = TRUE;
		}	
}


void HandleFoggingControls (void)
{
	int fogChange;			 // rate of change of fog parameters
	
		// deal with overall lighting mode
	if (PadStatus & PADR2)
		{
		if (framesSinceLastLightModeChanged > 20)
			{
				// flip: either zero or one
			OverallLightingMode = 1 - OverallLightingMode;
				// only two valid values
			assert(OverallLightingMode == 0
				|| OverallLightingMode == 1);
			framesSinceLastLightModeChanged = 0;
			LightingModeChanged = TRUE;
			}
		}

	if (PadStatus & PADselect)	// how fast fog paramters change
		fogChange = 20;
	else
		fogChange = 2;

		// first six: L-pad directional plus L1/L2:
		// alter the fog-in-distance colour
	if (PadStatus & PADLleft)
		{
		FoggingData.rfc -= fogChange;
		FoggingChanged = TRUE;
		}
	if (PadStatus & PADLright)
		{
		FoggingData.rfc += fogChange;
		FoggingChanged = TRUE;
		}
	if (PadStatus & PADLup)
		{
		FoggingData.gfc += fogChange;
		FoggingChanged = TRUE;
		}
	if (PadStatus & PADLdown)
		{
		FoggingData.gfc -= fogChange;
		FoggingChanged = TRUE;
		}
	if (PadStatus & PADL1)
		{
		FoggingData.bfc += fogChange;
		FoggingChanged = TRUE;
		}
	if (PadStatus & PADL2)
		{
		FoggingData.bfc -= fogChange;
		FoggingChanged = TRUE;
		}	

		// four R-pad directional controls:
		// change values of 'dqa' and 'dqb' fog parameters
	if (PadStatus & PADRleft)
		{
		FoggingData.dqa -= fogChange;
		FoggingChanged = TRUE;
		}
	if (PadStatus & PADRright)
		{
		FoggingData.dqa += fogChange;
		FoggingChanged = TRUE;
		}
	if (PadStatus & PADRup)
		{
		FoggingData.dqb += fogChange;
		FoggingChanged = TRUE;
		}
	if (PadStatus & PADRdown)
		{
		FoggingData.dqb -= fogChange;
		FoggingChanged = TRUE;
		}

		// L1 and R1: make white or black
	if (PadStatus & PADL1 && PadStatus & PADR1)
		{
		if (FoggingData.rfc == 255 			  // if white
			&& FoggingData.gfc == 255
			&& FoggingData.bfc == 255) 
				{
				FoggingData.rfc = 0;		  // make black
				FoggingData.gfc = 0;
				FoggingData.bfc = 0;
				FoggingChanged = TRUE;
				}
		else				// else make white
			{
			FoggingData.rfc = 255;		  // make black
			FoggingData.gfc = 255;
			FoggingData.bfc = 255;
			FoggingChanged = TRUE;
			}
		}
}





void HandleViewControls (void)
{
	int viewChange;			// rate of change of viewpoint

	if (PadStatus & PADselect)	   // how fast viewpoint changes
		viewChange = 100;
	else
		viewChange = 10;

		// these six: L-pad directional plus L1/L2:
		// alter the 'vpz' viewpoint part of GsRVIEW2
		// SHOULD CHANGE THIS:
		// can set a direction and a magnitude
		// hence get vp point coord (scale unit direction vector)
	if (PadStatus & PADLleft)
		{
		TheMainView.vpx += viewChange;
		ViewChanged = TRUE;
		}
	if (PadStatus & PADLright)
		{
		TheMainView.vpx -= viewChange;
		ViewChanged = TRUE;
		}
	if (PadStatus & PADLup)
		{
		TheMainView.vpy += viewChange;
		ViewChanged = TRUE;
		}
	if (PadStatus & PADLdown)
		{
		TheMainView.vpy -= viewChange;
		ViewChanged = TRUE;
		}
	if (PadStatus & PADL1)
		{
		TheMainView.vpz += viewChange;
		ViewChanged = TRUE;
		}
	if (PadStatus & PADL2)
		{
		TheMainView.vpz -= viewChange;
		ViewChanged = TRUE;
		}

		// Rup and Rdown: change projection distance
	if (PadStatus & PADRup)
		{
		ProjectionDistance += 5 * viewChange;
		ProjectionChanged = TRUE;
		}
	if (PadStatus & PADRdown)
		{
		ProjectionDistance -= 5 * viewChange;
		ProjectionChanged = TRUE;
		}

		// Rleft: 
		// change what coordinate system we view from;
		// an object id; 
		// see below for setting to WORLD or player's ship
	if (PadStatus & PADRleft)
		{
		if (numberFramesSinceViewpointChange > 20)
			{
				// get next lowest id of an existing object
			MainViewObjectID 
				= FindNextLowestObjectID(MainViewObjectID);
				// actually set view 'super'
			TheMainView.super 
				= &ObjectArray[MainViewObjectID].coord;		
			ViewChanged = TRUE;
			numberFramesSinceViewpointChange = 0;
			}
		}
	if (PadStatus & PADRright)
		{
		if (numberFramesSinceViewpointChange > 20)
			{
				// get next highest id of an existing object
			MainViewObjectID 
				= FindNextHighestObjectID(MainViewObjectID);
				// actually set view 'super'
			TheMainView.super 
				= &ObjectArray[MainViewObjectID].coord;
			ViewChanged = TRUE;
			numberFramesSinceViewpointChange = 0;
			}
		}

		// L1 and R1: toggle WORLD/player's ship/objects
		// circular list: (WORLD, player's ship, 
	if (PadStatus & PADL1 && PadStatus & PADR1)
		{
		if (numberFramesSinceViewpointChange > 20)
			{
			if (MainViewObjectID == -1)	  // WORLD
				{
				MainViewObjectID = -2;		// set to player's ship
				TheMainView.super = &PlayersShip.coord;
				}
			else if (MainViewObjectID == -2)	// player's ship
				{
					// set to first existing object
				MainViewObjectID = FindNextHighestObjectID(-1);
				TheMainView.super 
					= &ObjectArray[MainViewObjectID].coord;
				}
			else
				{
				assert(MainViewObjectID >= 0
						&& MainViewObjectID < MAX_OBJECTS);
				MainViewObjectID = -1;		  // set to world
				TheMainView.super = WORLD;
				}

				// note that view had changed  
			ViewChanged = TRUE;
			numberFramesSinceViewpointChange = 0;
			}
		}

		// R1 and R2: change the 'rz' distortion
	if (PadStatus & PADR1)
		{
		TheMainView.rz += viewChange/2;
		ViewChanged = TRUE;
		}
	if (PadStatus & PADR2)
		{
		TheMainView.rz -= viewChange/2;
		ViewChanged = TRUE;
		}
}
		  





void HandleObjectSelection (void)
{
		// select and R1: select next submode
	if (PadStatus & PADR1 && PadStatus & PADselect)
		{
		if (framesSinceLastSelection > 12)
			{
			if (ObjectSelectionMode == MAX_OBJECT_SELECTION_MODE)
				ObjectSelectionMode = 0;
			else
				ObjectSelectionMode++;
			framesSinceLastSelection = 0;
			}
		}

		// select next object up
	if (PadStatus & PADR1)
		{
		if (framesSinceLastSelection > 5)
			{
			UnHighlightObject( &ObjectArray[SelectedObject]);
			SelectedObject = FindNextHighestObjectID (SelectedObject);
			HighlightObject( &ObjectArray[SelectedObject]);
			framesSinceLastSelection = 0;
			}
		}
		// select next object down
	if (PadStatus & PADR2)
		{
		if (framesSinceLastSelection > 5)
			{
			UnHighlightObject( &ObjectArray[SelectedObject]);
			SelectedObject = FindNextLowestObjectID (SelectedObject);
			HighlightObject( &ObjectArray[SelectedObject]);
			framesSinceLastSelection = 0;
			}
		}

		// check that selected ID is valid before indexing array
	assert(SelectedObject >= 0);
	assert(SelectedObject < MAX_OBJECTS);

	switch(ObjectSelectionMode)
		{
		case MOVE_OBJECT:
			{
			int speedMultiplier; 

			if (PadStatus & PADselect)
				speedMultiplier = 10;		// fast movement
			else
				speedMultiplier = 1;		// slow movement
	
				// this chunk: left directional pad plus L1/L2: 
				// move object around
			if (PadStatus & PADLleft)
				{
				ObjectArray[SelectedObject].velocity.vx 
						-= MovementSpeed * speedMultiplier;
				}
			if (PadStatus & PADLright)
				{
				ObjectArray[SelectedObject].velocity.vx 
						 += MovementSpeed * speedMultiplier;
				}
			if (PadStatus & PADLup)
				{
				ObjectArray[SelectedObject].velocity.vy 
						 -= MovementSpeed * speedMultiplier;
				}
			if (PadStatus & PADLdown)
				{
				ObjectArray[SelectedObject].velocity.vy 
						 += MovementSpeed * speedMultiplier;
				}
			if (PadStatus & PADL1)
				{
				ObjectArray[SelectedObject].velocity.vz 
						 -= MovementSpeed * speedMultiplier;
				}
			if (PadStatus & PADL2)
				{
				ObjectArray[SelectedObject].velocity.vz 
						 += MovementSpeed * speedMultiplier;
				}
				
				// Rup/Rdown: increase/decrease radius
			if (PadStatus & PADRup)
				{
				ObjectArray[SelectedObject].radius += speedMultiplier;
				}
			if (PadStatus & PADRdown)
				{
				ObjectArray[SelectedObject].radius -= speedMultiplier;
				}

				// Rleft: toggle which way object orbits in plane
			if (PadStatus & PADRleft) 
				{ 
				if (framesSinceLastSelection > 5)
					{
					if (ObjectArray[SelectedObject].whichWay == CLOCKWISE)
						ObjectArray[SelectedObject].whichWay = ANTICLOCKWISE;
					else
						ObjectArray[SelectedObject].whichWay = CLOCKWISE;
					framesSinceLastSelection = 0;
					}
				}
			}
			break;
		case CHANGE_SPIN:
			{
			int speedMultiplier; 

			if (PadStatus & PADselect)
				speedMultiplier = 10;		// fast twisting
			else
				speedMultiplier = 1;		// slow twisting

				// this chunk: left directional pad plus R1/R2: 
				// set object's spin
			if (PadStatus & PADLleft)
				{
				ObjectArray[SelectedObject].twist.vy
							-= RotationSpeed * speedMultiplier;	 
				}
			if (PadStatus & PADLright)
				{
        		ObjectArray[SelectedObject].twist.vy 
        					+= RotationSpeed * speedMultiplier; 
				}
			if (PadStatus & PADLup)
				{
				ObjectArray[SelectedObject].twist.vx 
							-= RotationSpeed * speedMultiplier;	  
				}
			if (PadStatus & PADLdown)
				{	
				ObjectArray[SelectedObject].twist.vx 
							+= RotationSpeed * speedMultiplier;  
				}
			if (PadStatus & PADL1)
				{
      		  	ObjectArray[SelectedObject].twist.vz 
      		  				-= RotationSpeed * speedMultiplier;	  
				}
			if (PadStatus & PADL2)
				{
      		  	ObjectArray[SelectedObject].twist.vz 
      		  				+= RotationSpeed * speedMultiplier;	   
				}

				// these two: set object spin momentum on or off
				// when off, this submode adjusts angle of object
			if (PadStatus & PADRleft)
				{
				ObjectArray[SelectedObject].rotationMomentumFlag = FALSE;
				}
			if (PadStatus & PADRright)
				{
				ObjectArray[SelectedObject].rotationMomentumFlag = TRUE;
				}
			}
			break;
		case CHANGE_DISPLAY:
			{
			int semiTransparency = 0;

			// bit 3: toggle normal lighting / fog only mode; GsFOG
			// bit 6: toggle light calculations; GsLOFF
			// 28 and 29: semi-transparency flags; 
			// use as 1 four-value int: GsAZERO <-> GsATHREE
			// bit 30: semi-transparency on/off: GsALON
			// bit 31: display on/off: GsDOFF

				// Lleft: toggle normal lighting / fog only
			if (PadStatus & PADLleft)
				{
				if (framesSinceLastSelection > 5)
					{
					if (ObjectArray[SelectedObject].handler.attribute & GsFOG)
						{
						TURN_NTH_BIT_OFF(ObjectArray[SelectedObject].handler.attribute, 32, 3)
						}  
					else
						{
						ObjectArray[SelectedObject].handler.attribute |= GsFOG;
						}
					framesSinceLastSelection = 0;
					}
				}	
				// Lright: toggle light calculations 
			if (PadStatus & PADLright)
				{
				if (framesSinceLastSelection > 5)
					{
					if (ObjectArray[SelectedObject].handler.attribute & GsLOFF)
						{
						TURN_NTH_BIT_OFF(ObjectArray[SelectedObject].handler.attribute, 32, 6)
						}  
					else
						{
						ObjectArray[SelectedObject].handler.attribute |= GsLOFF;
						}
					framesSinceLastSelection = 0;
					}
				}

				// Rdown: toggle semi-transparency on/off
			if (PadStatus & PADRdown)
				{
				if (framesSinceLastSelection > 5)
					{
					if (ObjectArray[SelectedObject].handler.attribute & GsALON)
						{
						TURN_NTH_BIT_OFF(ObjectArray[SelectedObject].handler.attribute, 32, 30)
						}  
					else
						{
						ObjectArray[SelectedObject].handler.attribute |= GsALON;
						}
					framesSinceLastSelection = 0;
					}
				}

				// find current semi-transparency setting
			if (ObjectArray[SelectedObject].handler.attribute & 1<<28)
				semiTransparency++;
			if (ObjectArray[SelectedObject].handler.attribute & 1<<29)
				semiTransparency += 2;

				// Rup: increase semi-transparency setting
			if (PadStatus & PADRup)
				{
				if (framesSinceLastSelection > 5)
					{
					if (semiTransparency == 3)
						semiTransparency = 0;
					else	
						semiTransparency++;

						// now set in attribute of GsDOBJ2 structure
					if (semiTransparency == 1 || semiTransparency == 3)
						ObjectArray[SelectedObject].handler.attribute |= 1<<28;
					else
						TURN_NTH_BIT_OFF(ObjectArray[SelectedObject].handler.attribute, 32, 28)
						
					if (semiTransparency == 2 || semiTransparency == 3)
						ObjectArray[SelectedObject].handler.attribute |= 1<<29;	 
					else
						TURN_NTH_BIT_OFF(ObjectArray[SelectedObject].handler.attribute, 32, 29)
					framesSinceLastSelection = 0;
					}
				}

				// L1: toggle object display
			if (PadStatus & PADL1)
				{
				if (framesSinceLastSelection > 5)
					{
					if (ObjectArray[SelectedObject].handler.attribute & GsDOFF)
						{
						TURN_NTH_BIT_OFF(ObjectArray[SelectedObject].handler.attribute, 32, 31);
						}  
					else
						{
						ObjectArray[SelectedObject].handler.attribute |= GsDOFF;
						}
					framesSinceLastSelection = 0;
					}
				}
			}	
			break;
		case OBJECT_VIEWS:
			break;
		default:
			PRINT_POSITION;			// should never get here
			exit(1);
		}
}
		
	



int FindNextLowestObjectID (int objectID)
{
	int nextLowestID;
	int i;

	nextLowestID = objectID-1;
	for (;;)
		{
		if (nextLowestID < 0)
			nextLowestID = MAX_OBJECTS-1;
		if (ObjectArray[nextLowestID].alive == TRUE)
			break;
		nextLowestID--;
		}

	return nextLowestID;
}


	
int FindNextHighestObjectID (int objectID)
{
	int nextHighestID;
	int i;

	nextHighestID = objectID+1;
	for (;;)
		{
		if (nextHighestID >= MAX_OBJECTS)
			nextHighestID = 0;
		if (ObjectArray[nextHighestID].alive == TRUE)
			break;
		nextHighestID++;
		}

	return nextHighestID;
}






void HandleAllObjects (void)
{
	int i;

		// firstly deal with the spheres
	for (i = 0; i < MAX_OBJECTS; i++)
		{
		if (ObjectArray[i].alive == TRUE)
			{
				// deal with movement and rotation
			UpdateCoordinateSystem (&ObjectArray[i].rotate,
							&ObjectArray[i].position,
							&ObjectArray[i].coord);

			SortObjectSize (&ObjectArray[i]);

				// update angle
			ObjectArray[i].rotate.vx += ObjectArray[i].twist.vx;
			ObjectArray[i].rotate.vy += ObjectArray[i].twist.vy;
			ObjectArray[i].rotate.vz += ObjectArray[i].twist.vz;

				// update position
			ObjectArray[i].position.vx += ObjectArray[i].velocity.vx;
			ObjectArray[i].position.vy += ObjectArray[i].velocity.vy;
			ObjectArray[i].position.vz += ObjectArray[i].velocity.vz;

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

		// now deal with player's ship
	   
		// deal with movement and rotation
	UpdateCoordinateSystem3( &PlayersShip.rotate,
							&PlayersShip.twist,
							&PlayersShip.position,
							&PlayersShip.velocity,
							&PlayersShip.coord);


	 	// Position updating now done by UpdateCoordinateSystem2:
		// player's ship moves relative to itself, not to its super
		// ie left is relative to angle of player's ship
	 
#if 0			// OLD
		// update angle of object, relative to its super coord system
	PlayersShip.rotate.vx += PlayersShip.twist.vx;
	PlayersShip.rotate.vy += PlayersShip.twist.vy;
	PlayersShip.rotate.vz += PlayersShip.twist.vz;
#endif   
		
		// momentum or not? 
	if (PlayersShip.movementMomentumFlag == FALSE)
		{
		PlayersShip.velocity.vx = 0;
		PlayersShip.velocity.vy = 0;
		PlayersShip.velocity.vz = 0;
		}
	if (PlayersShip.rotationMomentumFlag == FALSE)
		{
		PlayersShip.twist.vx = 0;
		PlayersShip.twist.vy = 0;
		PlayersShip.twist.vz = 0;
		}
}




	// sort out all orbital movement
	// this time: Not using hierarchical coordinate system because
	// we want all the bodies to be able to spin without 
	// afffecting others:
	// hence calculate position as offset from position of
	// body which object orbits around
void DealWithOrbitalMovement (void)
{
	ObjectHandler* object;
	ObjectHandler* orbitCentreObject;
	int planeX, planeY;
	int newX, newY, newZ;
	int i;

	for (i = 0; i < MAX_OBJECTS; i++)
		{
		if (ObjectArray[i].alive == TRUE
				&& ObjectArray[i].inOrbit == TRUE)
			{
			object = &ObjectArray[i];
				
				// update angle
			switch(object->whichWay)
				{
				case CLOCKWISE:
					object->angle += object->angleIncrement;
				   	break;
				case ANTICLOCKWISE:
					object->angle -= object->angleIncrement;
				   	break;
				default:
					PRINT_POSITION;		// should never get here
					exit(1);
				}

			planeX = (object->radius * rcos(object->angle)) >> 12;
			planeY = (object->radius * rsin(object->angle)) >> 12;
				
				// this is offset position from orbiting body
			newX = (planeX * object->firstVector.vx)
					+ (planeY * object->secondVector.vx);
			newY = (planeX * object->firstVector.vy)
					+ (planeY * object->secondVector.vy);
			newZ = (planeX * object->firstVector.vz)
					+ (planeY * object->secondVector.vz);

				// check that id is valid
			assert(object->centralBodyID >= 0);
			assert(object->centralBodyID < MAX_OBJECTS);

			orbitCentreObject = &ObjectArray[object->centralBodyID];
			
				// the vectors are relative to size ONE
				// hence shift-right 12 to make them unit vectors
			object->position.vx 
				= orbitCentreObject->position.vx + (newX >> 12);
			object->position.vy 	
				= orbitCentreObject->position.vy + (newY >> 12);
			object->position.vz 
				= orbitCentreObject->position.vz + (newZ >> 12);
			}
		}
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
	InitialiseLighting();	

	SetUpSolarSystem();

	InitialiseModels();
	
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
	InitialiseView();

	ResetAll();
}







void InitialiseObjects (void)
{
	int i;
	
		// initialise spheres
	for (i = 0; i < MAX_OBJECTS; i++)
		{
		InitSingleObject( &ObjectArray[i]);
		ObjectArray[i].id = i;
		}

		// now initialise the player's ship
	InitSingleObject( &PlayersShip);

	PlayersShip.alive = TRUE;
   
		// overall speed for moving player's ship 
		MovementSpeed = INITIAL_MOVEMENT_SPEED; 
		RotationSpeed = INITIAL_ROTATION_SPEED;

		// push player's ship away from solar system
		// so player can see where things are
	PlayersShip.position.vz = -12000;
}

					  
					  


void InitSingleObject (ObjectHandler* object)
{
	object->id = NONE;
	object->alive = FALSE;
	object->type = NONE;
	object->which = NONE;

	object->scaleX = 1;
	object->scaleY = 1;
	object->scaleZ = 1;
	   
			// start off stationary with no momentum
	object->position.vx = 0;
	object->position.vy = 0;
	object->position.vz = 0;
	object->velocity.vx = 0;
	object->velocity.vy = 0;
	object->velocity.vz = 0;
	object->movementMomentumFlag = FALSE;

	object->rotate.vx = 0;
	object->rotate.vy = 0;
	object->rotate.vz = 0;
	object->twist.vx = 0;
	object->twist.vy = 0;
	object->twist.vz = 0;
	object->rotationMomentumFlag = FALSE;

		// this for initialisation, rather than link to WORLD coords
	GsInitCoordinate2(WORLD, &(object->coord) );

	object->inOrbit = FALSE;
	object->centralBodyID = NONE;
	object->angle = 0;
	object->angleIncrement = 0;
	object->whichWay = NONE;
	object->radius = 0;
	object->firstVector.vx = 0;
	object->firstVector.vy = 0;
	object->firstVector.vz = 0;		
	object->secondVector.vx = 0;
	object->secondVector.vy = 0;
	object->secondVector.vz = 0;
	object->normalToOrbitalPlane.vx = 0;
	object->normalToOrbitalPlane.vy = 0;
	object->normalToOrbitalPlane.vz = 0;
}


   


	// here, we highlight the selected object by cancelling the
	// light calculations
void HighlightObject (ObjectHandler* object)
{
	object->handler.attribute |= 1<<6;
}



	// set standard way of handling 3D object: set GsDOBJ2 attribute to zero
void UnHighlightObject (ObjectHandler* object)
{
	TURN_NTH_BIT_OFF(object->handler.attribute, 32, 6)
}





	// link TMD 3D object models to object handlers
void InitialiseModels (void)
{
	int i;
	
	for (i = 0; i < MAX_OBJECTS; i++)
		{
		int whichModel;	   // which colour sphere

		if (ObjectArray[i].type != NONE)
			{
			switch(ObjectArray[i].type)
				{
				case SUN:
					whichModel = 3;
					break;
				case PLANET:
					whichModel = 5;
					break;
				case 2:
					whichModel = 7 + (i % 3);
					break;
				default:
					PRINT_POSITION;
					exit(1);
				}
			}
		else
			whichModel = 0;
			   			
		LinkObjectHandlerToTmdObject( &(ObjectArray[i].handler), 
					whichModel, SPHERE_MODEL_ADDRESS);
	
			// each object has its own independent coordinate system
		ObjectArray[i].handler.coord2 = &(ObjectArray[i].coord);
		}   

		
		// now sort the player's ship
	LinkObjectHandlerToTmdObject( &(PlayersShip.handler), 
					0, SHUTTLE_MODEL_ADDRESS);

		// player's ship has its own independent coordinate system
	PlayersShip.handler.coord2 = &(PlayersShip.coord);
}
	




	// set up the viewpoint
void InitialiseView (void)
{
		// screen-to-viewpoint distance
	ProjectionDistance = 2000;	
	ProjectionChanged = FALSE;

		// register screen-to-viewpoint distance in graphics system
	GsSetProjection(ProjectionDistance);		

		// viewpoint is z-wise back from reference point
		// ie z increases going further into screen, standard view	

		// viewpoint is at centre of ship
	TheMainView.vpx = 0; 
	TheMainView.vpy = 0; 
	TheMainView.vpz = -200;
	
		// reference point at centre of world
	TheMainView.vrx = 0; 
	TheMainView.vry = 0; 
	TheMainView.vrz = 0;
   
	TheMainView.rz = 0;

	MainViewObjectID = -2;		// player's ship
	numberFramesSinceViewpointChange = 0;
	
	TheMainView.super = &PlayersShip.coord;

	ViewChanged = FALSE;

   	GsSetRefView2(&TheMainView);
}



	// set up lighting: three parallel light sources
void InitialiseLighting (void)
{
	int i;

		// initialise the three parallel light sources
	for (i = 0; i < 2; i++)
		{
		lightSourceArray[i].vx = 0; 
		lightSourceArray[i].vy = 0; 
		lightSourceArray[i].vz = 0;
	
		lightSourceArray[i].r = 0; 
		lightSourceArray[i].g = 0; 
		lightSourceArray[i].b = 0;
		}

		// set the lights: directions and colours
		// to start with: each light goes along one of the three axes
		// (x,y,z) and each is one primary colour (r,g,b)
	lightSourceArray[0].vz = 100;
	lightSourceArray[0].r = 128;

	lightSourceArray[1].vy = 100;  
	lightSourceArray[1].g = 128;

	lightSourceArray[2].vx = 100; 
	lightSourceArray[2].b = 128;
	
		// now register lights with the system
	for (i = 0; i < 2; i++)
		{
		GsSetFlatLight(i, &lightSourceArray[i]);
		}

		// sort the management globals
	SelectedLight = 0;		  
	LightsChanged = FALSE;			
	framesSinceLastLightChange = 0;
  
		// sort ambient lighting: set globals, register them in system
		// initial values are arbitrary		
	AmbientRed = ONE/20;
	AmbientGreen = ONE/10;
	AmbientBlue = ONE/2;
	AmbientLightChanged = FALSE;
	
	GsSetAmbient(AmbientRed, AmbientGreen, AmbientBlue);
		 

		// now deal with fogging: objects disappearing into the distance
#if 0
	// THE first two fields: meaning UNKNOWN, documentation is poor
	// see GTE
#endif
	FoggingData.dqa = 0;
	FoggingData.dqb = 0;
		// set the fogging colour: black to start with
	FoggingData.rfc = 0;
	FoggingData.gfc = 0;
	FoggingData.bfc = 0;
	FoggingChanged = FALSE;			  

		// now deal with overall lighting mode
		// NOTE: fogging only effective in mode 1
		// ONLY VALID MODES ARE 0 and 1
	OverallLightingMode	= 0;

	GsSetLightMode(OverallLightingMode);
	LightingModeChanged = FALSE;
	framesSinceLastLightModeChanged = 0;
}

	




void SetUpSolarSystem (void)
{
	int i, j;
	int sunID, planetID, moonID;
	VECTOR angle1, angle2;


		// only one Sun: object id 0
	sunID = 0;
	ObjectArray[sunID].alive = TRUE;
	ObjectArray[sunID].type = SUN;
	ObjectArray[sunID].which = 1;
	
		// make the sun massive
	SetObjectScaling( &ObjectArray[sunID], 2, 2, 2); 

#if 0
		// make the sun spin randomly
	ObjectArray[sunID].twist.vx = rand() % 8;
	ObjectArray[sunID].twist.vy = rand() % 8;
	ObjectArray[sunID].twist.vz = rand() % 8; 
	ObjectArray[sunID].rotationMomentumFlag = TRUE;
#endif	   


		// the planets
	for (i = 0; i < MAX_PLANETS; i++)
		{
		planetID = i+1;

		ObjectArray[planetID].alive = TRUE;
		ObjectArray[planetID].type = PLANET;
		ObjectArray[planetID].which = i+1;
 
			// now make the planets orbit the sun
		RandomVector( &angle1);
		AnyUnitPerpendicularVector( &angle1, &angle2);

		PutObjectIntoOrbit ( &ObjectArray[planetID], sunID,
			((ONE / MAX_PLANETS) * i), (ONE/720), 
				(((i+1) * PLANET_ORBIT_SIZE) + 300), RandomOrientation(),
					&angle1, &angle2);
			
			// move planets away from sun	
		ObjectArray[planetID].position.vx += (i+1) * PLANET_ORBIT_SIZE;

			// make the planets large
		SetObjectScaling( &ObjectArray[planetID], 1.4, 1.4, 1.4);

#if 0
			// make planets spin randomly
		ObjectArray[planetID].twist.vx = rand() % 8;
		ObjectArray[planetID].twist.vy = rand() % 8;
		ObjectArray[planetID].twist.vz = rand() % 8; 
		ObjectArray[planetID].rotationMomentumFlag = TRUE;
#endif

			
			// the moons
		for (j = 0; j < MAX_MOONS; j++)
			{
			moonID = MAX_PLANETS + (i*MAX_MOONS) + (j+1);
		
			ObjectArray[moonID].alive = TRUE;
			ObjectArray[moonID].type = MOON;
			ObjectArray[moonID].which = j+1;
 
				// now make the moons orbit the planet
			RandomVector (&angle1);
			AnyUnitPerpendicularVector (&angle1, &angle2);
			
			PutObjectIntoOrbit ( &ObjectArray[moonID], planetID,
				((ONE / MAX_MOONS) * j), (ONE/180), 
					(((j+1) * MOON_ORBIT_SIZE)+100), RandomOrientation(),
						&angle1, &angle2);
	
				// move moons away from planet	
			ObjectArray[moonID].position.vx += (j+1) * MOON_ORBIT_SIZE;

#if 0
				// make moons spin randomly
			ObjectArray[moonID].twist.vx = rand() % 32;
			ObjectArray[moonID].twist.vy = rand() % 32;
			ObjectArray[moonID].twist.vz = rand() % 32; 
			ObjectArray[moonID].rotationMomentumFlag = TRUE;
#endif
			}
		}
}






void PutObjectIntoOrbit (ObjectHandler* object, int centralBodyID,
		int initialAngle, int angleIncrement, int radius, int whichWay,
			VECTOR* firstAngle, VECTOR* secondAngle)
{
	object->inOrbit = TRUE;
	object->centralBodyID = centralBodyID;
	object->angle = initialAngle;
	object->angleIncrement = angleIncrement;	
	object->radius = radius;
	object->whichWay = whichWay;
	object->firstVector.vx = firstAngle->vx;
	object->firstVector.vy = firstAngle->vy;
	object->firstVector.vz = firstAngle->vz;
	object->secondVector.vx = secondAngle->vx;
	object->secondVector.vy = secondAngle->vy;
	object->secondVector.vz = secondAngle->vz;
}
	   



int RandomOrientation (void)
{
	if (rand() % 2 == 0)
		return CLOCKWISE;
	else
		return ANTICLOCKWISE;
}



int RandomAngle (void)
{
	return ( rand() % ONE);
}


	// returns integer between 1 and argument
int RandomInteger (int maximum)
{
	return ( (rand() % maximum) + 1	);
}

	 	  	  
	   

	// set axis-wise scaling parameters
void SetObjectScaling (ObjectHandler* object, double scaleX,
						double scaleY, double scaleZ)
{
	object->scaleX = scaleX;
	object->scaleY = scaleY;
	object->scaleZ = scaleZ;
}
						 



	/****
	 scale object in 3 ways
	 NOTE: with a hierarchical coordinate system,
	 any scaling of object X will affect all objects whose coord.
	 systems are children of X's coordinate systems
	 i.e. this is NOT absolute volume change;
	 eg if ONLY scale the sun, ALL objects in the solar system
	 will be scaled by the same amount
	****/
void SortObjectSize (ObjectHandler* object)
{
	VECTOR scalingVector;

	scalingVector.vx = (int) (((double)ONE) * object->scaleX);
	scalingVector.vy = (int) (((double)ONE) * object->scaleY);
	scalingVector.vz = (int) (((double)ONE) * object->scaleZ);

	ScaleMatrix(&object->coord.coord, &scalingVector);

		// tell graphics system that coordinate system is changed
	object->coord.flg = 0;
}




	/***
	 tell coordinate system about translation and rotation
	 this one sets coordinates absolutely, before rotation
	 i.e. is geared towards assigning position w.r.t. world,
	 and moving w.r.t. world rather than w.r.t. local coord. system
	 -- see below for different version
	***/								 
void UpdateCoordinateSystem (SVECTOR* rotationVector,
							VECTOR* translationVector,
							GsCOORDINATE2* coordSystem)
{
   	MATRIX tempMatrix;

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



void UpdateCoordinateSystem2 (SVECTOR* rotation, VECTOR* twist,
							VECTOR* position, VECTOR* velocity,
							GsCOORDINATE2* coordSystem)
{
   	MATRIX tempMatrix; 
	VECTOR realMovement;

		// copy the translating vector (position)
	tempMatrix.t[0] = coordSystem->coord.t[0];
	tempMatrix.t[1] = coordSystem->coord.t[1];
	tempMatrix.t[2] = coordSystem->coord.t[2];

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




void UpdateCoordinateSystem3 (SVECTOR* rotation, VECTOR* twist,
							VECTOR* position, VECTOR* velocity,
							GsCOORDINATE2* coordSystem)
{
	MATRIX tempMatrix; 
	VECTOR realMovement;
	VECTOR realTwist;

#if 0
		// make twist much Larger: to reduce inaccuracies in matrix multiplication
	twist->vx = twist->vx << 12;
	twist->vy = twist->vy << 12;
	twist->vz = twist->vz << 12;
#endif
		
		// copy the translating vector (position)
	tempMatrix.t[0] = coordSystem->coord.t[0];
	tempMatrix.t[1] = coordSystem->coord.t[1];
	tempMatrix.t[2] = coordSystem->coord.t[2];

		// get rotation matrix from rotation vector
	RotMatrix(rotation, &tempMatrix);

		// find the object-local velocity in super coordinate terms
	ApplyMatrixLV(&tempMatrix, velocity, &realMovement);

		// get rotation matrix from rotation vector
	RotMatrix(rotation, &tempMatrix);

		// find the object-local twist in super coordinate terms
	ApplyMatrixLV(&tempMatrix, twist, &realTwist);
	   
		// assign new matrix to coordinate system
	coordSystem->coord = tempMatrix;
	
		// update position by super-relative movement
	position->vx += realMovement.vx;
	position->vy += realMovement.vy;
	position->vz += realMovement.vz; 

#if 0
		// update rotation by super-coord twist
	rotation->vx += twist->vx;
	rotation->vy += twist->vy;
	rotation->vz += twist->vz;
#endif

		// update rotation by super-coord twist
	rotation->vx += realTwist.vx;//  >> 12;
	rotation->vy += realTwist.vy;// >> 12;
	rotation->vz += realTwist.vz;// >> 12;

#if 0
		// reduce twist back to sensible size
	twist->vx = twist->vx >> 12;
	twist->vy = twist->vy >> 12;
	twist->vz = twist->vz >> 12;
#endif

		// set position absolutely	
	coordSystem->coord.t[0] = position->vx;
	coordSystem->coord.t[1] = position->vy;
	coordSystem->coord.t[2] = position->vz;

		// tell GTE that coordinate system has been updated
	coordSystem->flg = 0;

		// DEBUG
	if (frameNumber % 10 == 0)
		{
		printf("twist: %d %d %d\n", twist->vx, twist->vy, twist->vz);
		printf("realTwist: %d %d %d\n", 
			realTwist.vx, realTwist.vy, realTwist.vz); 
		}
}







void ResetAll (void)
{
	InitialiseObjects();
	InitialiseLighting();
	SetUpSolarSystem();

		// to start with, player moves around
	SelectionMode = MOVE_AROUND;	 
	framesSinceLastSwitch = 0;

	SelectedObject = 0;
	framesSinceLastSelection = 0;

	InitialiseView();
	ViewChanged = TRUE;
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





