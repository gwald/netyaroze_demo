/************************************************************
 *															*
 *						Tuto7.c								*
 *					basic 3-D object manipulation			*
 *			   											    *															*
 *				LPGE     16/10/96							*		
 *															*
 *	Copyright (C) 1996 Sony Computer Entertainment Inc.		*
 *	All Rights Reserved										*
 *															*
 ***********************************************************/


   	

   
/**********************			libraries	  *****************************/
#include <libps.h>

#include "asssert.h"
#include "pad.h"
#include "tim.h"
#include "tmd.h"
#include "sincos.h"


	
	
/**********************			constants and globals	  *****************/


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
#define PLANET_ORBIT_SIZE 200
#define MOON_ORBIT_SIZE 150


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
	SVECTOR position;		// position
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
	int whichWay;		  // CLOCKWISE or ANTICLOCKWISE
	int radius;			  // distance from central body
	VECTOR firstVector;		  // these two describe the plane of orbit
	VECTOR secondVector;
} ObjectHandler;

#define MAX_OBJECTS		(64)

#define MAX_PLANETS 3
#define MAX_MOONS 3

	// the spheres: sun, moon, planets
ObjectHandler ObjectArray[MAX_OBJECTS];


	// cube that player moves around in; view is local camera on cube
ObjectHandler PlayersCube;


	// six cubes just to mark where axes are
ObjectHandler SixCubes[6];

	// player's movement and rotation speeds
int MovementSpeed;
int RotationSpeed;

	// initial values of player's movement and rotation speeds 
#define INITIAL_MOVEMENT_SPEED 150
#define INITIAL_ROTATION_SPEED 3



	// constants for user selection mode
#define MOVE_AROUND 0
#define PARADIGM_VIEW 1
#define CHANGE_LIGHTS 2
#define SET_AMBIENT_LIGHT 3
#define FOGGING 4
#define VIEW_DISTORTION 5


   
	// MUST update this iff add new selection modes
#define HIGHEST_SELECTION_MODE (VIEW_DISTORTION)


int ParadigmView;		  // at present: from each axes' end points
u_long framesSinceLastParadigmViewSwitch;
int ParadigmViewSorted;			// boolean flag
#define MAX_PARADIGM_VIEWS 10
char paradigmViewString[128];	  




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


			







/**********************			prototypes	  *****************************/

int main(void);


	// text printing
void GetSelectionModeString (char *string);
void PrintModeInfo (void);
void WriteParadigmViewIntoString (char* string);
 

	// controller pad handling
int	DealWithController(void);

void HandlePlayerMoving (void);
void HandleLightControls (void);
void HandleAmbientControls (void);
void HandleFoggingControls (void);
void HandleViewControls (void);
void HandleParadigmView (void);
void SetPlayersCubeToParadigmView (void);

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
							SVECTOR* translationVector,
							GsCOORDINATE2* coordSystem);

void UpdateCoordinateSystem2 (SVECTOR* rotationVector, SVECTOR* twist,
							SVECTOR* position, SVECTOR* velocity,
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

			// deal with the six cubes
		for (i = 0; i < 6; i++) 
			{
		  	GsGetLs(&SixCubes[i].coord, &tempMatrix);
		   
			GsSetLightMatrix(&tempMatrix);
			
			GsSetLsMatrix(&tempMatrix);
			
			GsSortObject4( &(SixCubes[i].handler), 
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
		framesSinceLastParadigmViewSwitch++;	

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
		case PARADIGM_VIEW:
			sprintf(string, "%s\n", "Paradigm view");
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
		case PARADIGM_VIEW:
			FntPrint("Paradigm view %d\n", ParadigmView);
			WriteParadigmViewIntoString(paradigmViewString);
			FntPrint("%s\n", paradigmViewString);
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


	 

	// write view info into string
void WriteParadigmViewIntoString (char* string)
{
	switch (ParadigmView)
		{
		case 0:
			sprintf(string, "%s\n", "View from -z to origin");
			break;
		case 1:
			sprintf(string, "%s\n", "View from +z to origin");
			break;
		case 2:
			sprintf(string, "%s\n", "View from -y to origin");
			break;
		case 3:
			sprintf(string, "%s\n", "View from +y to origin");
			break;
		case 4:
			sprintf(string, "%s\n", "View from -x to origin");
			break;
		case 5:
			sprintf(string, "%s\n", "View from +x to origin");
			break;
		case 6:
			sprintf(string, "%s\n", "View from x+,y+,z+\nto x-,y-,z-");
			break;
		case 7:
			sprintf(string, "%s\n", "View from x+,y+,z-\nto x-,y-,z+");
			break;
		case 8:
			sprintf(string, "%s\n", "View from x+,y-,z+\nto x-,y+,z-");
			break;
		case 9:
			sprintf(string, "%s\n", "View from x-,y+,z+\nto x+,y-,z-");
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
		case PARADIGM_VIEW:
			HandleParadigmView();
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
			framesSinceLastSwitch = 0;
			if (SelectionMode == PARADIGM_VIEW)
				ParadigmViewSorted = FALSE;
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

#if 0				// DISABLE PAUSE: conflicts with other controls
		// pause
	if (PadStatus & PADL1) 
		{
		pauseCounter++;
		if (pauseCounter > 15)
			{
			while (PadStatus & PADL1)
				{
				PadStatus = PadRead();
				}
			pauseCounter = 0;
			}
		}
	else
		pauseCounter = 0;
#endif
		  	   
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

		// L1 and R1: toggle WORLD/player's cube/objects
		// circular list: (WORLD, player's cube, 
	if (PadStatus & PADL1 && PadStatus & PADR1)
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



void HandleParadigmView (void)
{
		// R2: choose next view, R1: choose previous view
	if (PadStatus & PADR2)
		{
		if (framesSinceLastParadigmViewSwitch > 20)
			{
			if (ParadigmView == MAX_PARADIGM_VIEWS-1)
				ParadigmView = 0;
			else
				ParadigmView++;
			framesSinceLastParadigmViewSwitch = 0;
			ParadigmViewSorted = FALSE;
			}
		}
	if (PadStatus & PADR1)
		{
		if (framesSinceLastParadigmViewSwitch > 20)
			{
			if (ParadigmView == 0)
				ParadigmView = MAX_PARADIGM_VIEWS-1;
			else
				ParadigmView--;
			framesSinceLastParadigmViewSwitch = 0;
			ParadigmViewSorted = FALSE;
			}
		}

		// if player's cube not already there, sort it out
	if (ParadigmViewSorted == FALSE)
		{
		SetPlayersCubeToParadigmView();
		ParadigmViewSorted = TRUE;
		}
}





void SetPlayersCubeToParadigmView (void)
{
	SVECTOR position;
	SVECTOR angle;

		// initialise
	position.vx = position.vy = position.vz = 0;
	angle.vx = angle.vy = angle.vz = 0;

	switch (ParadigmView)
		{
		case 0:					// from -z to origin i.e. standard view
			position.vz = -6000;
			break;
		case 1:					// from +z to origin (inverse of 0)
			position.vz = 6000;
			angle.vy = ONE/2;
			break;
		case 2:					// from -y to origin
			position.vy = -6000;
			angle.vx = 3*ONE/4;
			break;
		case 3:					// from +y to origin (inverse of 2)
			position.vy = 6000;
			angle.vx = ONE/4;
			break;
		case 4:					// from -x to origin
			position.vx = -6000;
			angle.vy = ONE/4;
			break;
		case 5:					// from +x to origin (inverse of 4)
			position.vx = 6000;
			angle.vy = 3*ONE/4;
			break;
		case 6:				// from x+,y+,z+ to x-,y-,z-
			position.vx = 2500;
			position.vy = 2500;
			position.vz = 2500;
			angle.vx = -525;
			angle.vy = 405;
			break;
		case 7:				// from x+,y+,z- to x-,y-,z+
			position.vx = 2500;
			position.vy = 2500;
			position.vz = -2500;
			angle.vx = 528;
			angle.vy = -407;
			break;
		case 8:				// from x+,y-,z+ to x-,y+,z-
			position.vx = 2500;
			position.vy = -2500;
			position.vz = 2500;
			angle.vx = 2600;
			angle.vy = -407;
			break;
		case 9:				// from x-,y-,z- to x+,y+,z+
			position.vx = -2500;
			position.vy = 2500;
			position.vz = 2500;
			angle.vx = 3566;
			angle.vy = -400;
			break;
		default:
			PRINT_POSITION;			// should never get here
			exit(1);
		}

		// now put player's cube in the right place, looking the right way
	PlayersCube.position.vx = position.vx;
	PlayersCube.position.vy = position.vy;
	PlayersCube.position.vz = position.vz;

	PlayersCube.rotate.vx = angle.vx;
	PlayersCube.rotate.vy = angle.vy;
	PlayersCube.rotate.vz = angle.vz;

		// mark the change in viewpoint
	ViewChanged = TRUE;
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

		// deal with the six cubes
	for (i = 0; i < 6; i++)
		{
			// deal with movement and rotation
		UpdateCoordinateSystem (&SixCubes[i].rotate,
							&SixCubes[i].position,
							&SixCubes[i].coord);
		}
}




	// sort out all orbital movement
void DealWithOrbitalMovement (void)
{
	ObjectHandler* object;
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
				
			newX = (planeX * object->firstVector.vx)
					+ (planeY * object->secondVector.vx);
			newY = (planeX * object->firstVector.vy)
					+ (planeY * object->secondVector.vy);
			newZ = (planeX * object->firstVector.vz)
					+ (planeY * object->secondVector.vz);

				// the vectors are relative to size ONE
				// hence shift-right 12 to make them unit vectors
			object->position.vx = newX >> 12;
			object->position.vy = newY >> 12;
			object->position.vz = newZ >> 12;
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
	InitialiseView();				  
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

		// push player's cube away from solar system
		// so player can see where things are
	PlayersCube.position.vz = -2500;

		// now sort out the six cubes that mark the axes
	for (i = 0; i < 6; i++)
		{	
		InitSingleObject( &SixCubes[i]);
		}

		// positioning the six cubes
	SixCubes[0].position.vx = 20000;
	SixCubes[1].position.vx = -20000;
	SixCubes[2].position.vy = 20000;
	SixCubes[3].position.vy = -20000;
	SixCubes[4].position.vz = 20000;
	SixCubes[5].position.vz = -20000;
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
	object->handler.attribute = 0;
}





	// link TMD 3D object models to object handlers
void InitialiseModels (void)
{
	int i;
	
	for (i = 0; i < MAX_OBJECTS; i++)
		{
		int whichModel;	   // which sphere: red, blue, green

		if (ObjectArray[i].type != NONE)
			whichModel = ObjectArray[i].type;
		else
			whichModel = 0;	
			
		LinkObjectHandlerToTmdObject( &(ObjectArray[i].handler), 
					whichModel, SPHERE_MODEL_ADDRESS);
	
			// each object has its own independent coordinate system
		ObjectArray[i].handler.coord2 = &(ObjectArray[i].coord);
		}   

		
		// now sort the player's cube
	LinkObjectHandlerToTmdObject( &(PlayersCube.handler), 
					0, CUBE_MODEL_ADDRESS);

		// player's cube has its own independent coordinate system
	PlayersCube.handler.coord2 = &(PlayersCube.coord);

		// the six cubes
	for (i = 0; i < 6; i++)
		{	
		LinkObjectHandlerToTmdObject( &(SixCubes[i].handler), 
					0, CUBE_MODEL_ADDRESS);
	
			// each object has its own independent coordinate system
		SixCubes[i].handler.coord2 = &(SixCubes[i].coord);
		}
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

		// viewpoint is at centre of cube ??????
	TheMainView.vpx = 0; 
	TheMainView.vpy = 0; 
	TheMainView.vpz = -12000;
	
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
   	
   		// for pre-set views
   	ParadigmView = 0;	
	framesSinceLastParadigmViewSwitch = 0;
	ParadigmViewSorted = TRUE;		// start in different mode
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
		// sun's coordinate system is relative to WORLD
	ObjectArray[sunID].coord.super = WORLD;
	
		// make the sun large
	SetObjectScaling( &ObjectArray[sunID], 2, 2, 2); 
		   


		// the planets
	for (i = 0; i < MAX_PLANETS; i++)
		{
		planetID = i+1;

		ObjectArray[planetID].alive = TRUE;
		ObjectArray[planetID].type = PLANET;
		ObjectArray[planetID].which = i+1;
			// planets' coordinate system is relative to the sun
   		ObjectArray[planetID].coord.super = &ObjectArray[sunID].coord;
 
			// now make the planets orbit the sun
		RandomVector( &angle1);
		AnyUnitPerpendicularVector( &angle1, &angle2);
		
			// DEBUG
		
			{
			printf("random vector 1: %d %d %d\n", 
				angle1.vx, angle1.vy, angle1.vz);
			printf("random vector 2: %d %d %d\n", 
				angle2.vx, angle2.vy, angle2.vz);
			}


		PutObjectIntoOrbit ( &ObjectArray[planetID], sunID,
			((ONE / MAX_PLANETS) * i), (ONE/720), 
				((i+1) * PLANET_ORBIT_SIZE), RandomOrientation(),
					&angle1, &angle2 );
			
			// move planets away from sun	
		ObjectArray[planetID].position.vx += (i+1) * PLANET_ORBIT_SIZE;

			// make planets much smaller than sun
		SetObjectScaling( &ObjectArray[planetID], 0.5, 0.5, 0.5);

			
			// the moons
		for (j = 0; j < MAX_MOONS; j++)
			{
			moonID = MAX_PLANETS + (i*MAX_MOONS) + (j+1);
		
			ObjectArray[moonID].alive = TRUE;
			ObjectArray[moonID].type = MOON;
			ObjectArray[moonID].which = j+1;
				// moons' coordinate system is relative to the planet
   			ObjectArray[moonID].coord.super 
   							= &ObjectArray[planetID].coord;
 
				// now make the moons orbit the planet
			RandomVector (&angle1);
			AnyUnitPerpendicularVector (&angle1, &angle2);
	

				// DEBUG
		
			{
			printf("random vector 1: %d %d %d\n", 
				angle1.vx, angle1.vy, angle1.vz);
			printf("random vector 2: %d %d %d\n", 
				angle2.vx, angle2.vy, angle2.vz);
			}

			PutObjectIntoOrbit ( &ObjectArray[moonID], planetID,
				((ONE / MAX_MOONS) * j), (ONE/180), 
					((j+1) * MOON_ORBIT_SIZE), RandomOrientation(),
						&angle1, &angle2 );
	
				// move moons away from planet	
			ObjectArray[moonID].position.vx += (j+1) * MOON_ORBIT_SIZE;

				// make the moons much smaller than planets 
			SetObjectScaling( &ObjectArray[moonID], 0.5, 0.5, 0.5);
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




	// assigns random values to vector argument
	// such that it ends up of overall size ONE
void RandomVector (VECTOR* vector)
{
	int soFar, theRest;			  // magnitude so far, what is left
	
		// set randomly
	vector->vx = rand() % ONE;
	vector->vy = rand() % ONE;
	vector->vz = rand() % ONE;
	
	ScaleVectorToUnit(vector);
}




void ScaleVectorToUnit (VECTOR* vector)
{
	int length, square;

	square = pow(vector->vx, 2) + pow(vector->vy, 2) 
					+ pow(vector->vz, 2); 

	length = pow(square, 0.5);
	assert(length != 0);

	vector->vx *= ONE;
	vector->vy *= ONE;
	vector->vz *= ONE;

	vector->vx /= length;
	vector->vy /= length;
	vector->vz /= length;
}



	// assign to <output> any vector of size ONE
	// that is perpendicular to vector <input>
void AnyUnitPerpendicularVector (VECTOR* input, VECTOR* output)
{	 
	switch (rand() % 3)
		{
		case 0:
		if (input->vz != 0)
			{
		
				// set x and y randomly
			output->vx = rand() % ONE;
			output->vy = rand() % ONE;
	
			output->vz = ((-(input->vx) * output->vx) 
							+ (-(input->vy) * output->vy)) 
											/ input->vz;

			ScaleVectorToUnit(output);
			}
		else
			{
			output->vx = ONE;
			output->vy = 0;
			output->vz = 0;
			}
		break;
		case 1:
		if (input->vy != 0)
			{
		
				// set x and z randomly
			output->vx = rand() % ONE;
			output->vz = rand() % ONE;
	
			output->vy = ((-(input->vx) * output->vx) 
							+ (-(input->vz) * output->vz)) 
											/ input->vy;

			ScaleVectorToUnit(output);
			}
		else
			{
			output->vx = 0;
			output->vy = ONE;
			output->vz = 0;
			}
		break;
		case 2:
		if (input->vx != 0)
			{
		
				// set z and y randomly
			output->vz = rand() % ONE;
			output->vy = rand() % ONE;
	
			output->vx = ((-(input->vz) * output->vz) 
							+ (-(input->vy) * output->vy)) 
											/ input->vx;

			ScaleVectorToUnit(output);
			}
		else
			{
			output->vx = 0;
			output->vy = 0;
			output->vz = ONE;
			}
		break;
		default:
			assert(FALSE);
		}


	





#if 0		// OLD
	if (input->vz != 0)
		{
		
			// set x and y randomly
		output->vx = rand() % ONE;
		output->vy = rand() % ONE;
	
		output->vz = ((-(input->vx) * output->vx) 
						+ (-(input->vy) * output->vy)) 
											/ input->vz;

		ScaleVectorToUnit(output);
		}
	else
		{
		output->vx = ONE;
		output->vy = 0;
		output->vz = 0;
		}
#endif
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







void ResetAll (void)
{
	InitialiseObjects();
	InitialiseView();
	InitialiseLighting();
	SetUpSolarSystem();

		// to start with, player moves around
	SelectionMode = MOVE_AROUND;	 
	framesSinceLastSwitch = 0;

	SelectedObject = 0;
	framesSinceLastSelection = 0;
	framesSinceLastObjectMove = 0;
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





