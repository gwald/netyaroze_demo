/************************************************************
 *															*
 *						main.c								*
 *					tmd file viewer							*
 *			   											    *															*
 *				LPGE     16/10/96							*		
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
#define MODEL_ADDRESS		(0x800c0000)



#define OT_LENGTH		(14)	  


	// Ordering Table handlers
GsOT		WorldOrderingTable[2];
	// actual Ordering Tables themselves							
GsOT_TAG	zSortTable[2][1<<OT_LENGTH];   
	// GPU packet work area
PACKET		GpuOutputPacket[2][PACKETMAX2];	 

	
	// NULL value (error detection)
#define NONE 1001

typedef struct
{
	int id;
	int alive;		 // boolean
	int type;		 // SUN, PLANET, MOON
	int which;		// index into circular list
	GsDOBJ2	handler;
	double scaleX, scaleY, scaleZ;			// scaling in 3D
	SVECTOR position;		// position
	SVECTOR velocity;		
	int movementMomentumFlag;
	SVECTOR rotate;			// angle
	SVECTOR twist;			// change of angle
	int rotationMomentumFlag;
	GsCOORDINATE2 coord;	 // coordinate system
} ObjectHandler;

#define MAX_OBJECTS		(64)	// 8 by 8 by 8 cube

int NumberOfModels;


ObjectHandler ObjectArray[MAX_OBJECTS];


	// object that player moves around in; view is local camera on cube
ObjectHandler PlayersCube;
int DisplayPlayer;		// boolean

	// player's movement and rotation speeds
int MovementSpeed;
int RotationSpeed;

	// initial values of player's movement and rotation speeds 
#define INITIAL_MOVEMENT_SPEED 150
#define INITIAL_ROTATION_SPEED 3



	// constants for user selection mode
#define MOVE_AROUND 0
#define CHANGE_LIGHTS 1
#define SET_AMBIENT_LIGHT 2
#define FOGGING 3
#define VIEW_DISTORTION 4

	// MUST update this iff add new selection modes
#define HIGHEST_SELECTION_MODE (VIEW_DISTORTION)

int SelectionMode;	   // current selection mode

u_long framesSinceLastSwitch;

char modeString[64];

int SelectedObject;			// id of selected object
u_long framesSinceLastSelection;
u_long framesSinceLastObjectMove;



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
	// set to -1 for WORLD, set to -2 for player's cube
int MainViewObjectID;
u_long numberFramesSinceViewpointChange;
		


   	// pad interface buffer	  
u_long PadStatus;	


	// simplest frame counter
u_long frameNumber = 0;	


	
		 



	// prototypes

int main(void);


	// text printing
void GetSelectionModeString (char *string);
void PrintModeInfo (void);
 

	// controller pad handling
int	DealWithController(void);

void HandlePlayerMoving (void);
void HandleLightControls (void);
void HandleAmbientControls (void);
void HandleFoggingControls (void);
void HandleViewControls (void);

	// object handlimh
int FindNextLowestObjectID (int objectID);
int FindNextHighestObjectID (int objectID);

void HandleAllObjects (void);



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


void UpdateCoordinateSystem (SVECTOR* rotationVector,
							SVECTOR* translationVector,
							GsCOORDINATE2* coordSystem);

void UpdateCoordinateSystem2 (SVECTOR* rotationVector, SVECTOR* twist,
							SVECTOR* position, SVECTOR* velocity,
							GsCOORDINATE2* coordSystem);

void ResetAll (void);

void ArrangeObjectsIntoLine (void);

void CreateSphere (int whichOne);

void SortObjectSize (ObjectHandler* object);


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
	
			// sort player's cube
		if (DisplayPlayer == TRUE)
			{
			GsGetLs( &PlayersCube.coord, &tempMatrix);
			GsSetLightMatrix(&tempMatrix);
			GsSetLsMatrix(&tempMatrix);
	    	GsSortObject4( &(PlayersCube.handler), 
					&WorldOrderingTable[outputBufferIndex], 
						14-OT_LENGTH, (u_long *)getScratchAddr(0));	  
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

			// main frame counter
		frameNumber++;

			// other frame counters
		framesSinceLastSwitch++;
		framesSinceLastSelection++;
		framesSinceLastObjectMove++;
		framesSinceLastLightChange++;
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
			FntPrint("Cube pos: %d %d %d\n",
				PlayersCube.coord.coord.t[0],
				PlayersCube.coord.coord.t[1],
				PlayersCube.coord.coord.t[2]);
			FntPrint("Cube ang: %d %d %d\n",
				PlayersCube.rotate.vx,
				PlayersCube.rotate.vy,
				PlayersCube.rotate.vz);
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
				FntPrint("View super: player's cube\n");
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
		default:
			PRINT_POSITION;		// should never get here
			exit(1);
		}
}


	 



   

	// select and start together to quit 
	// start alone to pause

int DealWithController (void)
{
		// how much the objects get moved by user in AFFECT_SPHERES mode
	static int objectMovement[3] = {10, 1, 1};
	
		// Most actions are dependent on selection mode
	switch(SelectionMode)
		{
		case MOVE_AROUND:		  // player moving
			HandlePlayerMoving();
			break;
#if 0			// disable
		case CHANGE_LIGHTS:
			HandleLightControls();			
			break;
#endif
		case SET_AMBIENT_LIGHT:
			HandleAmbientControls();
			break;
		case FOGGING:
			HandleFoggingControls();
			break;
		case VIEW_DISTORTION:
			HandleViewControls();
			break;	
		default:
			PRINT_POSITION;		// should never get here
			exit(1);
		}

	// The controls below always apply:
	// change selection mode, quit, reset demo

		// change selection mode
	if (PadStatus & PADL1 && PadStatus & PADR1)
		{
		if (framesSinceLastSwitch > 10)		// not too often
			{
			if (SelectionMode == HIGHEST_SELECTION_MODE)
				SelectionMode = 0;
			else
				SelectionMode++;	
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
	static int startPresses = 0;	 // number of frames start is held for

	if (PadStatus & PADselect)
		speedMultiplier = 10;		// fast movement
	else
		speedMultiplier = 1;		// slow movement
			
		// this chunk: left directional pad plus L1/L2: 
		// move player's cube
	if (PadStatus & PADLleft)
		{
		PlayersCube.velocity.vx -= MovementSpeed * speedMultiplier;
		ViewChanged = TRUE;
		}
	if (PadStatus & PADLright)
		{
		PlayersCube.velocity.vx += MovementSpeed * speedMultiplier;
		ViewChanged = TRUE;
		}
	if (PadStatus & PADLup)
		{
		PlayersCube.velocity.vy -= MovementSpeed * speedMultiplier;
		ViewChanged = TRUE;
		}
	if (PadStatus & PADLdown)
		{
		PlayersCube.velocity.vy += MovementSpeed * speedMultiplier;
		ViewChanged = TRUE;
		}
	if (PadStatus & PADL1)
		{
		PlayersCube.velocity.vz -= MovementSpeed * speedMultiplier;
		ViewChanged = TRUE;
		}
	if (PadStatus & PADL2)
		{
		PlayersCube.velocity.vz += MovementSpeed * speedMultiplier;
		ViewChanged = TRUE;
		}

		// this chunk: right directional pad plus R1/R2: 
		// rotate player's cube
	if (PadStatus & PADRleft)
		{
		PlayersCube.twist.vy -= RotationSpeed * speedMultiplier;
	    ViewChanged = TRUE;	 
		}
	if (PadStatus & PADRright)
		{
        PlayersCube.twist.vy += RotationSpeed * speedMultiplier;
        ViewChanged = TRUE;	  
		}
	if (PadStatus & PADRup)
		{
		PlayersCube.twist.vx -= RotationSpeed * speedMultiplier;
		ViewChanged = TRUE;	  
		}
	if (PadStatus & PADRdown)
		{
		PlayersCube.twist.vx += RotationSpeed * speedMultiplier;
		ViewChanged = TRUE;	  
		}
	if (PadStatus & PADR1)
		{
        PlayersCube.twist.vz -= RotationSpeed * speedMultiplier;
        ViewChanged = TRUE;	  
		}
	if (PadStatus & PADR2)
		{
        PlayersCube.twist.vz += RotationSpeed * speedMultiplier;
      	ViewChanged = TRUE;	   
		}

	if (PadStatus & PADstart)
		{
		startPresses++;
		if (startPresses > 20)
			{		 // toggle player display
			DisplayPlayer = 1-DisplayPlayer;
			startPresses = 0;
			}
		}
	else			 // reset counter
		startPresses = 0;
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
	if (PadStatus & PADstart)
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

		// Rleft and Rright: 
		// change what coordinate system we view from;
		// an object id; 
		// see below for setting to WORLD or player's cube
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

		// PADstart: toggle WORLD/player's cube/objects
		// circular list: (WORLD, player's cube, 
	if (PadStatus & PADstart)
		{
		if (numberFramesSinceViewpointChange > 20)
			{
			if (MainViewObjectID == -1)	  // WORLD
				{
				MainViewObjectID = -2;		// set to player's cube
				TheMainView.super = &PlayersCube.coord;
				}
			else if (MainViewObjectID == -2)	// player's cube
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
	ObjectHandler* object;

		// firstly deal with the spheres
	for (i = 0; i < MAX_OBJECTS; i++)
		{
		object = &ObjectArray[i];
		if (object->alive == TRUE)
			{
				// deal with movement and rotation
			UpdateCoordinateSystem (&object->rotate,
							&object->position,
							&object->coord);
			}
		}

		// now deal with player's cube

		// deal with movement and rotation
	UpdateCoordinateSystem2( &PlayersCube.rotate,
							&PlayersCube.twist,
							&PlayersCube.position,
							&PlayersCube.velocity,
							&PlayersCube.coord);


	 	// Position updating now done by UpdateCoordinateSystem2:
		// player's cube moves relative to itself, not to its super
		// ie left is relative to angle of player's cube
	SortObjectSize (&PlayersCube);
	 
		// update angle of object, relative to its super coord system
	PlayersCube.rotate.vx += PlayersCube.twist.vx;
	PlayersCube.rotate.vy += PlayersCube.twist.vy;
	PlayersCube.rotate.vz += PlayersCube.twist.vz;
		
		// momentum or not? 
	if (PlayersCube.movementMomentumFlag == FALSE)
		{
		PlayersCube.velocity.vx = 0;
		PlayersCube.velocity.vy = 0;
		PlayersCube.velocity.vz = 0;
		}
	if (PlayersCube.rotationMomentumFlag == FALSE)
		{
		PlayersCube.twist.vx = 0;
		PlayersCube.twist.vy = 0;
		PlayersCube.twist.vz = 0;
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
	InitialiseView();				  
	InitialiseLighting();	
  		
	ArrangeObjectsIntoLine();

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
	framesSinceLastObjectMove = 0;
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

		// now initialise the player's cube
	InitSingleObject( &PlayersCube);

	PlayersCube.alive = TRUE;
   
		// overall speed for moving player's cube
	MovementSpeed = INITIAL_MOVEMENT_SPEED;
	RotationSpeed = INITIAL_ROTATION_SPEED;

		// push player's cube away from the action
		// so player can see where things are
	PlayersCube.position.vz = -15000;
	PlayersCube.coord.flg = 0;
	DisplayPlayer = FALSE;
	UpdateCoordinateSystem2( &PlayersCube.rotate,
							&PlayersCube.twist,
							&PlayersCube.position,
							&PlayersCube.velocity,
							&PlayersCube.coord);
}

					  
					  


void InitSingleObject (ObjectHandler* object)
{
	object->id = NONE;
	object->alive = FALSE;
	object->type = 0;
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
	int whichSphere;
	
	//NumberOfModels = CountNumberOfObjectsInTmdFile(MODEL_ADDRESS);
	NumberOfModels = 1;



	for (i = 0; i < MAX_OBJECTS; i++)
		{
		if (i == NumberOfModels)
			break;

		LinkObjectHandlerToTmdObject( &(ObjectArray[i].handler), 
					i, MODEL_ADDRESS);
	
			// each object has its own independent coordinate system
		ObjectArray[i].handler.coord2 = &(ObjectArray[i].coord); 
		}

		
		// now sort the player's cube
	LinkObjectHandlerToTmdObject( &(PlayersCube.handler), 
					0, MODEL_ADDRESS);

		// player's cube has its own independent coordinate system
	PlayersCube.handler.coord2 = &(PlayersCube.coord);
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

		// viewpoint is behind of cube
	TheMainView.vpx = 0; 
	TheMainView.vpy = 0; 
	TheMainView.vpz = -2500;
	
		// reference point at centre of world
	TheMainView.vrx = 0; 
	TheMainView.vry = 0; 
	TheMainView.vrz = 0;
   
	TheMainView.rz = 0;

	MainViewObjectID = -2;		// player's cube
	numberFramesSinceViewpointChange = 0;
	
	TheMainView.super = &PlayersCube.coord;

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
	lightSourceArray[0].r = 0xd0;

	lightSourceArray[1].vy = 100;  
	lightSourceArray[1].g = 0xd0;

	lightSourceArray[2].vx = 100; 
	lightSourceArray[2].b = 0xd0;
	
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
	AmbientRed = ONE/4;
	AmbientGreen = ONE/4;
	AmbientBlue = ONE/4;
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

	


  

	/***
	 tell coordinate system about translation and rotation
	 this one sets coordinates absolutely, before rotation
	 i.e. is geared towards assigning position w.r.t. world,
	 and moving w.r.t. world rather than w.r.t. local coord. system
	 -- see below for different version
	***/								 
void UpdateCoordinateSystem (SVECTOR* rotationVector,
							SVECTOR* translationVector,
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



void UpdateCoordinateSystem2 (SVECTOR* rotation, SVECTOR* twist,
							SVECTOR* position, SVECTOR* velocity,
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
	ApplyMatrix(&tempMatrix, velocity, &realMovement);
	   
		// assign new matrix to coordinate system
	coordSystem->coord = tempMatrix;
	
		// tell GTE that coordinate system has been updated
	coordSystem->flg = 0;

		// update position by actual movement
	position->vx += realMovement.vx;
	position->vy += realMovement.vy;
	position->vz += realMovement.vz; 

		// set position absolutely	
	coordSystem->coord.t[0] = position->vx;
	coordSystem->coord.t[1] = position->vy;
	coordSystem->coord.t[2] = position->vz;
}







void ResetAll (void)
{
	InitialiseObjects();
	InitialiseView();
	InitialiseLighting();

	ArrangeObjectsIntoLine();

		// to start with, player moves around
	SelectionMode = MOVE_AROUND;	 
	framesSinceLastSwitch = 0;

	SelectedObject = 0;
	framesSinceLastSelection = 0;
	framesSinceLastObjectMove = 0;
}




void ArrangeObjectsIntoLine (void)
{	
	int i;
	
	for (i = 0; i < NumberOfModels; i++)
		{
		ObjectArray[i].alive = TRUE;
		ObjectArray[i].position.vx = i * 100;
		}  



#if 0	// OLD
	register int i, j, k;
	register int x, y, z;
	int currentObjectID = 0;
	ObjectHandler* object;
	int first, second, third, total;
	int plane1, rem;

	object = &ObjectArray[0];

	for (i = 0; i < 4; i++)
		{
		x = GridTopLeftFrontPoint.vx + (i * GridSpacing);
		for (j = 0; j < 4; j++)
			{
			y = GridTopLeftFrontPoint.vy + (j * GridSpacing);
			for (k = 0; k < 4; k++)
				{
				z = GridTopLeftFrontPoint.vz + (k * GridSpacing);

				object->alive = TRUE;
				object->position.vx = x;
				object->position.vy = y;
				object->position.vz = z;
				object++;
				}
			}
		}				 

	for (i = 0; i < 64; i++)
		{
		plane1 = i / 16;
		rem = i % 16;
		if (plane1 == 0 || plane1 == 3)
			{
			if (rem == 5 || rem == 6 
				|| rem == 9 || rem == 10)
				{
				ObjectArray[i].alive = FALSE;
				}
			if (rem == 0 || rem == 3 
				|| rem == 12 || rem == 15)
				{
				ObjectArray[i].type = CORNER;
				}
			else
				{
				ObjectArray[i].type = EDGE;
				}
			}
		else
			{
			if ( (rem == 0 || rem == 3 
				|| rem == 12 || rem == 15) == FALSE)
				{
				ObjectArray[i].alive = FALSE;
				}
			else
				{
				ObjectArray[i].type = EDGE;
				}
			}
			
			// tell system object has been moved
		ObjectArray[i].coord.flg = 0;
		}
#endif		// OLD
					
}
	 
	
	
	
void CreateSphere (int whichOne)
{
	int x, y, z;

#if 0	// OLD

	assert(whichOne >= 0);
	assert(whichOne < MAX_OBJECTS);	  

	x = GridTopLeftFrontPoint.vx + (GridSpacing * ((whichOne & 48) >> 4) );
	y = GridTopLeftFrontPoint.vy + (GridSpacing * ((whichOne & 12) >> 2) );
	z = GridTopLeftFrontPoint.vz + (GridSpacing * (whichOne & 3) );

	ObjectArray[whichOne].alive = TRUE;
	ObjectArray[whichOne].position.vx = x;
	ObjectArray[whichOne].position.vy = y;
	ObjectArray[whichOne].position.vz = z;
	ObjectArray[whichOne].coord.flg = 0;
#endif	// OLD

}  	




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





