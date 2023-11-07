/************************************************************
 *															*
 *						Tuto6.c								*
 *						3-D object manipulation				*
 *			   											    *															*
 *				LPGE     07/11/96							*		
 *															*
 *	Copyright (C) 1996 Sony Computer Entertainment Inc.		*
 *	All Rights Reserved										*
 *															*
 ***********************************************************/


         	// FINALLY sorted out proper object movement
         	// TMDs now easy to move and rotate
         	// either world-relative or object-relative  




/************************************************************************
*																		*
*			libraries	  												*
*																		*
************************************************************************/
#include <libps.h>

#include "asssert.h"
#include "dump.h"
#include "pad.h"
#include "tim.h"
#include "tmd.h"
#include "sincos.h"
#include "tangent.h"
#include "vector.h"
#include "object.h"
		


	
	

/************************************************************************
*																		*
*			constants, globals, structures	  							*
*																		*
************************************************************************/


	// standard booleans
#define FALSE 0
#define TRUE 1


	// GPU packet space
#define PACKETMAX		(10000)
#define PACKETMAX2		(PACKETMAX*24)



	// TMD model of sphere: holds 4 spheres, identical except for colour
#define SPHERE_MODEL_ADDRESS (0x800c0000)
#define SHUTTLE_MODEL_ADDRESS (0x800e0000)
#define SHIP_MODEL_ADDRESS (0x800f0000)


#define OT_LENGTH		(14)   


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


	// types of sphere objects
#define SUN 0
#define PLANET 1
#define MOON 2
#define SHIP 3

	// NULL value for integers (error detection)
#define NONE 1001

	// ways of going round a circle
#define CLOCKWISE 1002
#define ANTICLOCKWISE 1003

   

	// types of special view
#define EXTERNAL_VIEW 0
#define CIRCLING_VIEW 1
#define FLYBY_VIEW 2
#define DIRECTORS_VIEW 3
#define LOCAL_VIEW 4

#define MAX_SPECIAL_VIEW (LOCAL_VIEW)

			
	// the axes' ends: used by the external and local views
#define PLUS_X 0
#define MINUS_X 1
#define PLUS_Y 2
#define MINUS_Y 3
#define PLUS_Z 4
#define MINUS_Z 5


	// the 3 simplest planes: used by the circling view
#define X_Y_PLANE 0
#define X_Z_PLANE 1
#define Y_Z_PLANE 2


	// types of object motion: used by fly-by view
#define CIRCULAR 0
#define LINEAR 1


	// types of 'special movement' 
	// i.e. labels for simple maneouvres
#define FIRST_SIMPLE_PATH 0





	// ship that player moves around in; view is local camera on ship
ObjectHandler PlayersShuttle;



#define MAX_PLANETS 3
#define MAX_MOONS 9
#define MAX_MOONS_PER_PLANET 3

#define MAX_SHIPS 24


ObjectHandler TheSun;
ObjectHandler ThePlanets[MAX_PLANETS];
ObjectHandler TheMoons[MAX_MOONS];
ObjectHandler TheShips[MAX_SHIPS];





	// player's movement and rotation speeds
int MovementSpeed;
int RotationSpeed;
int FiringRate;

	// initial values of player's movement and rotation speeds 
#define INITIAL_MOVEMENT_SPEED 150
#define INITIAL_ROTATION_SPEED 6





#define MOVE 0
#define MENUS 1
int SuperMode;

	// constants for user selection mode
#define MOVE_SHIP 0
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


char SelectionString[12] = "~cf0f";
char DeSelectionString[12] = "~cfff";

int StringCounter;			// how many strings on this screen so far?
							// used to highlight the right string


int NumberOfOptions;		// on current selection screen
int SelectedOption;			// the highlighted one








#define MOVE_OBJECT 0
#define CHANGE_SPIN 1
#define CHANGE_VOLUME 2
#define CHANGE_DISPLAY 3
#define OBJECT_VIEWS 4

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


#define GENERAL_DISTORTION 0
#define MOVE_BOTH_ABSOLUTELY 1
#define MOVE_REFERENCE 2
#define MOVE_VIEWING_POINT 3
#define CHANGE_SUPER 4

#define MAX_VIEW_MODE (CHANGE_SUPER)


int ViewMode;
u_long numberFramesSinceViewModeChange;



	// special views: external, circling, fly-by, director's cut
int SpecialViewFlag;
int SpecialViewType;	

	// external: six views	
int WhichExternalView;
int ExternalViewDistance;
	
	// circling
int WhichCircularView;				// which plane: X_Y_PLANE, etc (hash-defines above)
int WhichWayRoundCirclingView;		// CLOCKWISE or ANTICLOCKWISE
int CircleViewRadius;	
int CircleViewAngle;
int CircleViewAngleIncrement;

	// fly-by
int TypeOfFlybyView;	// LINEAR or CIRCULAR, depending on object
int PathDistance;		// distance of parallel path from path of object
int PathAngle;			// xy-plane angle theta of parallel path from object path
int FlybyCycleTime;		// in frames
int NumberFlybyFramesSoFar;	

	// local view
int WhichLocalView;
int LocalViewDistance;

	// director's cut 
int SequencePosition;		// index into array: 0 to 2
int SingleViewCycleTime;		// in frames
int DirectorFrameCounter;
   
int DirectorLocalView;
int DirectorLocalViewDistance;

int DirectorWhichCircularView;
int DirectorWhichWayRoundCirclingView;	  
int DirectorCircleViewRadius;	
int DirectorCircleViewAngleIncrement;

int DirectorPathDistance;
int DirectorPathAngle;

int DirectorSequence[3] = {LOCAL_VIEW, CIRCLING_VIEW, FLYBY_VIEW};


	// scratchpad globals to save settings overridden by director
int OldWhichLocalView;
int OldLocalViewDistance;

int OldWhichCircularView;
int OldWhichWayRoundCirclingView;
int OldCircleViewRadius;
int OldCircleViewAngleIncrement;
		
int OldPathDistance;
int OldPathAngle;
int OldFlybyCycleTime;



									 


   	// pad interface buffer	  
u_long PadStatus;	



	// simplest frame counter
u_long frameNumber = 0;	


#define NORMAL_TIME_DELAY 7			// in frames
#define LONG_TIME_DELAY 15




MATRIX GlobalMatrix;

/************************************************************************
*																		*
*			prototypes		  											*
*																		*
************************************************************************/



int main (void);
	  
void GetSelectionModeString (char *string);
void PrintModeInfo (void);
void WriteObjectSelectionModeInfoIntoString (char* string);
void WriteObjectSelectionInfo (void);
						 				 
				   
void WriteObjectMoveInfo (void);
void WriteObjectSpinInfo (void);
void WriteObjectVolumeInfo (void);
void WriteChangeDisplayInfo (void);



int DealWithController (void);



void ResetSelectionData (void);
int FindNumberOfOptions (void);



void HandleShipMoving (void);
void HandleLightControls (void);
void HandleAmbientControls (void);
void HandleFoggingControls (void);
void HandleViewControls (void);



void HandleObjectSelection (void);

void HandleMovingAnObject (void);
void HandleChangingAnObjectsSpin (void);
void HandleChangingAnObjectsVolume (void);
void HandleChangingAnObjectsDisplay (void);

void HandleExternalView (void);
void HandleCirclingView (void);
void HandleFlybyView (void);
void HandleLocalView (void);



void HandleAllObjects (void);


void DealWithSpatialMovement (void);
void HandleOrbitalMovement (ObjectHandler* object);
void HandleSpecialMovement (ObjectHandler* object);




void InitialiseAll(void);	   

   
void InitialiseObjects (void);


void SetUpSolarSystem (void);


void PutObjectIntoOrbit (ObjectHandler* object, int centralBodyID,
		int initialAngle, int angleIncrement, int radius, int whichWay,
			VECTOR* firstAngle, VECTOR* secondAngle);



int RandomOrientation (void);
int RandomAngle (void);
int RandomInteger (int maximum);	 	  	  
	   


void SetObjectScaling (ObjectHandler* object, int scaleX,
						int scaleY, int scaleZ);						 
void SortObjectSize (ObjectHandler* object);
void FindObjectDimensions (ObjectHandler* object, VECTOR* dimensions);



int FindObjectSpeed (ObjectHandler* object);
int FindObjectMotionType (ObjectHandler* object);


								 
void UpdateCoordinateSystem (SVECTOR* rotationVector,
							VECTOR* translationVector,
							GsCOORDINATE2* coordSystem);
void UpdateCoordinateSystem2 (SVECTOR* rotation, VECTOR* twist,
							VECTOR* position, VECTOR* velocity,
							GsCOORDINATE2* coordSystem);
void UpdateCoordinateSystem3 (SVECTOR* rotation, VECTOR* twist,
							VECTOR* position, VECTOR* velocity,
							GsCOORDINATE2* coordSystem);
void UpdateCoordinateSystem4 (SVECTOR* rotation, VECTOR* twist,
							VECTOR* position, VECTOR* velocity,
							GsCOORDINATE2* coordSystem);	
	
	
void InitialiseView (void);
void SetNormalView (void);
void SetUpSpecialView (void);

void SetUpExternalView (int whichExternalView, int externalViewDistance);


void SetUpCirclingView (int whichCircularView, 
					int whichWayRoundCirclingView, int circleViewRadius,
					int circleViewAngle, int circleViewAngleIncrement);
void SetUpFlybyView (int pathDistance, int pathAngle, 
						int flybyCycleTime);
void SetUpLocalView (int whichLocalView, int localViewDistance);
void SetUpDirectorsView (void);






void DealWithSpecialView (void);
void DealWithExternalView (void);
void DealWithCirclingView (void);
void DealWithFlybyView (void);
void DealWithDirectorsView (void);
void DealWithLocalView (void);

void InitialiseLighting (void);	

void TurnAllLightCalculationsOff (void);

void ResetAll (void);


/************************************************************************
*																		*
*			macros		  												*
*																		*
************************************************************************/

		 


#define mod(a) ((a) > 0 ? (a) : (-(a)))


	// this would be unreadable if done within functions
	// NOTE: very slow, but very easy to optimise
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


#define BEFORE_STRING						   		\
			if (StringCounter == SelectedOption)	\
				FntPrint(SelectionString);
			

#define	AFTER_STRING								\
			if (StringCounter == SelectedOption)    \
				FntPrint(DeSelectionString);		\
			StringCounter++;







/************************************************************************
*																		*
*			functions	  												*
*																		*
************************************************************************/



int main (void)
{
	int	outputBufferIndex;
	MATRIX tempMatrix;
	ObjectHandler* object;
	ObjectHandler** pointer;
	int	i;
	



	InitialiseAll();



	while (1) 				 // main loop
		{
		DealWithSpatialMovement();

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

			// update view, if changed; special views require 
			// continuous view updating
		if (ViewChanged || SpecialViewFlag)
			{
			if (SpecialViewFlag == TRUE)
				DealWithSpecialView();
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
		pointer = &(ObjectArray[0]);
		for (i = 0; i < MAX_OBJECTS; i++) 
			{
			if (*pointer != NULL)
				{
				if ( (*pointer)->alive == TRUE)
					{
					object = *pointer;

					GsGetLs(&(object->coord), &tempMatrix);
			   
					GsSetLightMatrix(&tempMatrix);
				
					GsSetLsMatrix(&tempMatrix);
				
					GsSortObject4( &(object->handler), 
						&WorldOrderingTable[outputBufferIndex], 
							14-OT_LENGTH, (u_long *)getScratchAddr(0));
					}	 
				}
			pointer++;
			}

			// add player's ship into ordering table
		GsGetLs(&PlayersShuttle.coord, &tempMatrix);
		   
		GsSetLightMatrix(&tempMatrix);
			
		GsSetLsMatrix(&tempMatrix);
			
		GsSortObject4( &PlayersShuttle.handler, 
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
		framesSinceLastLightChange++;
		framesSinceLastLightModeChanged++;
		numberFramesSinceViewpointChange++;	
		numberFramesSinceViewModeChange++;

			// deal with text: highlight relevant line of text 
			// by printing in another colour using '~cXYZ' specification
			// where X,Y and Z are all 0-f hexadecimal r,g,b values
		StringCounter = 0;

		FntPrint("~c0ffFrame %d\n~cfff", frameNumber);
		FntPrint("super mode: ");
		switch(SuperMode)
			{
			case MOVE:
				FntPrint("moving\n");
				break;
			case MENUS:
				FntPrint("menus\n");
				break;
			default:
				assert(FALSE);
			}
			// print selection mode
		GetSelectionModeString(modeString);
		if (StringCounter == SelectedOption)
			FntPrint("%sMain Mode :-\n%s%s", SelectionString, modeString, DeSelectionString);
		else
			FntPrint("Main Mode :-\n%s", modeString);
		StringCounter++;

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
		case MOVE_SHIP:
			sprintf(string, "%s\n", "Moving around");
			break;
		case CHANGE_LIGHTS:
			sprintf(string, "%s\n", "Change lights");
			break;
		case SET_AMBIENT_LIGHT:
			sprintf(string, "%s\n", "Ambient light");
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
			assert(FALSE);		// should never get here
		}
}








#if 0
char str1[80];
char str2[80];
#endif


	// print text onto the screen;
	// what we print depends on user's selection mode
void PrintModeInfo (void)
{
#if 0
	double x, y, z;
	double virtualThetaX, virtualThetaY, virtualThetaZ;
#endif

	switch(SelectionMode)
		{
		case MOVE_SHIP:
			BEFORE_STRING
		
			FntPrint("Ship pos: %d %d %d\n",
				PlayersShuttle.coord.coord.t[0],
				PlayersShuttle.coord.coord.t[1],
				PlayersShuttle.coord.coord.t[2]);
			FntPrint("Ship ang: %d %d %d\n",
				PlayersShuttle.rotate.vx,
				PlayersShuttle.rotate.vy,
				PlayersShuttle.rotate.vz);

#if 0
			virtualThetaX 
				= ((double)PlayersShuttle.rotate.vx) / ((double)4096) * ((double)2) * 3.14159;
			virtualThetaY 
				= ((double)PlayersShuttle.rotate.vy) / ((double)4096) * ((double)2) * 3.14159;
			virtualThetaZ 
				= ((double)PlayersShuttle.rotate.vz) / ((double)4096) * ((double)2) * 3.14159;

			x = ONE / (1 + pow( tan(virtualThetaY), 2) + pow(virtualThetaZ, 2) );
			y = ONE / (1 + pow( tan(virtualThetaX), 2) + pow(virtualThetaZ, 2) );
			z = ONE / (1 + pow( tan(virtualThetaY), 2) + pow(virtualThetaX, 2) );

			sprintf2(str1, "direction V: %f %f %f\n", x, y, z);
			sprintf2(str2, "virtuals: %f %f %f\n", virtualThetaX, virtualThetaY, virtualThetaZ);
			FntPrint(str2);
			FntPrint(str1);	 
#endif

			AFTER_STRING

			break;
		case CHANGE_LIGHTS:
			BEFORE_STRING
			FntPrint("Selected light: %d\n", SelectedLight);
			AFTER_STRING

			assert(SelectedLight >= 0 && SelectedLight <= 2);
			BEFORE_STRING
			FntPrint("Direction: %d %d %d\n",
					lightSourceArray[SelectedLight].vx,
					lightSourceArray[SelectedLight].vy,
					lightSourceArray[SelectedLight].vz);
			AFTER_STRING

			BEFORE_STRING
			FntPrint("Colour: r %d, g %d, b %d\n",
					lightSourceArray[SelectedLight].r,
					lightSourceArray[SelectedLight].g,
					lightSourceArray[SelectedLight].b);
			AFTER_STRING

			break;
		case SET_AMBIENT_LIGHT:
			FntPrint("Ambient light colours:-\n");		// never highlight: not a variable

			BEFORE_STRING
			FntPrint("Red: %d\n", AmbientRed);
			AFTER_STRING


			BEFORE_STRING
			FntPrint("Green: %d\n", AmbientGreen);
			AFTER_STRING

			BEFORE_STRING
			FntPrint("Blue: %d\n", AmbientBlue);
			AFTER_STRING

			break;
		case FOGGING:
			BEFORE_STRING
			FntPrint("dqa %d\n", FoggingData.dqa);
			AFTER_STRING

			BEFORE_STRING
			FntPrint("dqb %d\n", FoggingData.dqb);
			AFTER_STRING

			BEFORE_STRING
			FntPrint("Fog colours: \nr %d g %d b %d\n",
				FoggingData.rfc, FoggingData.gfc, FoggingData.bfc);
			AFTER_STRING

			BEFORE_STRING
			FntPrint("Lighting mode: %d\n",
					OverallLightingMode);
			if (OverallLightingMode == 0)
				FntPrint("I.e. fogging is OFF\n");
			else
				{
				assert(OverallLightingMode == 1);
				FntPrint("I.e. fogging is ON\n");
				}
			AFTER_STRING
			break;
		case VIEW_DISTORTION:
			switch(ViewMode)
				{
				case GENERAL_DISTORTION:
					BEFORE_STRING
					FntPrint("view mode :-\nGeneral\n");
					AFTER_STRING

					BEFORE_STRING
					FntPrint("rz: %d\n", TheMainView.rz);
					AFTER_STRING
					
					BEFORE_STRING
					FntPrint("Projection: %d\n", ProjectionDistance);
					AFTER_STRING
					break;
				case MOVE_BOTH_ABSOLUTELY:
					BEFORE_STRING
					FntPrint("view mode :-\nMove both absolutely\n");
					AFTER_STRING
					
					BEFORE_STRING
					FntPrint("Reference point :-\n%d %d %d\n",
						TheMainView.vrx, TheMainView.vry, TheMainView.vrz);
					AFTER_STRING
					
					BEFORE_STRING
					FntPrint("Viewing point :-\n %d %d %d\n",
						TheMainView.vpx, TheMainView.vpy, TheMainView.vpz);
					AFTER_STRING
					break;
				case MOVE_REFERENCE:
					BEFORE_STRING
					FntPrint("view mode :-\nMove reference\n");
					AFTER_STRING
					
					BEFORE_STRING
					FntPrint("Reference point :-\n%d %d %d\n",
						TheMainView.vrx, TheMainView.vry, TheMainView.vrz);
					AFTER_STRING
				 
					break;
				case MOVE_VIEWING_POINT:
					BEFORE_STRING
					FntPrint("view mode :-\nMove viewing point\n");
					AFTER_STRING

					BEFORE_STRING
					FntPrint("Viewing point :-\n %d %d %d\n",
						TheMainView.vpx, TheMainView.vpy, TheMainView.vpz);
					AFTER_STRING
					
					break;
				case CHANGE_SUPER:
				   	BEFORE_STRING
					FntPrint("view mode :-\nChange super\n");
					AFTER_STRING				
					
					BEFORE_STRING
					switch(MainViewObjectID)
						{
						case -2:			// player's ship
							FntPrint("View super: player's ship\n");
							break;
						case -1:
							FntPrint("View super: WORLD\n");
							break;
						default:
							FntPrint("View super: object id %d\n", 
							MainViewObjectID);
			   			}
					AFTER_STRING

					break;
				default:				// should never get here
					assert(FALSE);
				}
			break;	
		case SELECT_OBJECT:					
				// sort printing of sub-mode
			WriteObjectSelectionModeInfoIntoString(objectModeString);
			BEFORE_STRING
			FntPrint("submode: %s", objectModeString);
			AFTER_STRING

				// say which object is selected
			BEFORE_STRING
			FntPrint("Selected object id: %d\n", SelectedObject);
			AFTER_STRING

				// print data relevant to submode
			WriteObjectSelectionInfo();
			break;
		default:
			assert(FALSE);		// should never get here
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
		case CHANGE_VOLUME:
			sprintf(string, "%s\n", "Change volume");
			break;
		case CHANGE_DISPLAY:
			sprintf(string, "%s\n", "Change display");
			break;
		case OBJECT_VIEWS:
			sprintf(string, "%s\n", "Change views");
			break;
		default:
			assert(FALSE);		// should never get here
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
			WriteObjectMoveInfo();
			break;
		case CHANGE_SPIN:
			WriteObjectSpinInfo();
			break;
		case CHANGE_VOLUME:
			WriteObjectVolumeInfo();
			break;
		case CHANGE_DISPLAY:
			WriteChangeDisplayInfo();
			break;
		case OBJECT_VIEWS:		
			switch(SpecialViewType)
				{
				case NONE:
					BEFORE_STRING
					FntPrint("no special view\n");
					AFTER_STRING
					break;
				case EXTERNAL_VIEW:
					BEFORE_STRING
					FntPrint("external view\n");
					AFTER_STRING

					BEFORE_STRING
					switch(WhichExternalView)
						{
						case PLUS_X:
							FntPrint("From left\n");
			   				break;
						case MINUS_X:
							FntPrint("From right\n");
							break;
						case PLUS_Y:
							FntPrint("From below\n");
							break;
						case MINUS_Y:
							FntPrint("From above\n");
							break;
						case PLUS_Z:
				 			FntPrint("From back\n");
							break;
						case MINUS_Z:
							FntPrint("From front\n");
							break;
						default:				   // should never get here
							assert(FALSE);
						}
					AFTER_STRING

					BEFORE_STRING
					FntPrint("external distance: %d\n", ExternalViewDistance);
					AFTER_STRING
					break;
				case CIRCLING_VIEW:	
					BEFORE_STRING
					FntPrint("Circling view\n");
					AFTER_STRING

					BEFORE_STRING
					switch(WhichCircularView)
						{
						case X_Y_PLANE:
							FntPrint("circle in x-y plane\n");
							break;
						case X_Z_PLANE:
							FntPrint("circle in x-z plane\n");
							break;
						case Y_Z_PLANE:
							FntPrint("circle in y-z plane\n");
							break;
						default:
							assert(FALSE);		   // should never get here
						}
					AFTER_STRING


					BEFORE_STRING
					switch(WhichWayRoundCirclingView)
						{
						case CLOCKWISE:
							FntPrint("orbiting clockwise\n");
							break;
						case ANTICLOCKWISE:
							FntPrint("orbiting anticlockwise\n");
							break;
						default:
							assert(FALSE);		 // should never get here
						}
					AFTER_STRING


					BEFORE_STRING
					FntPrint("orbit radius: %d\n", CircleViewRadius);
					AFTER_STRING

					FntPrint("orbit angle: %d\n", CircleViewAngle);

					BEFORE_STRING
					FntPrint("angular speed: %d\n", CircleViewAngleIncrement);
					AFTER_STRING
					break;
				case FLYBY_VIEW:	

					BEFORE_STRING
					FntPrint("Fly-by view\n");
					AFTER_STRING

					switch(TypeOfFlybyView)
						{
						case CIRCULAR:
							FntPrint("object is in orbit\n");
							return;
						case LINEAR:
							FntPrint("object is not in orbit\n");
							break;
						default:
							assert(FALSE);
						}
					
					BEFORE_STRING
					FntPrint("path distance: %d\n", PathDistance);
					AFTER_STRING

					if (TypeOfFlybyView == LINEAR)
						{
						BEFORE_STRING
						FntPrint("path angle: %d\n", PathAngle);
						AFTER_STRING
						}

					BEFORE_STRING
					FntPrint("cycle time: %d\n", FlybyCycleTime);
					AFTER_STRING
					break;
				case DIRECTORS_VIEW:	
					BEFORE_STRING
					FntPrint("Director's cut\n");
					AFTER_STRING
					break;
				case LOCAL_VIEW:
					BEFORE_STRING
					FntPrint("local view\n");
					AFTER_STRING

					BEFORE_STRING
					switch(WhichLocalView)
						{
						case PLUS_X:
							FntPrint("Looking right\n");
			   				break;
						case MINUS_X:
							FntPrint("Looking left\n");
							break;
						case PLUS_Y:
							FntPrint("Looking down\n");
							break;
						case MINUS_Y:
							FntPrint("Looking up\n");
							break;
						case PLUS_Z:
				 			FntPrint("Looking backward\n");
							break;
						case MINUS_Z:
							FntPrint("Looking forwards\n");
							break;
						default:				   // should never get here
							assert(FALSE);
						}
					AFTER_STRING

					BEFORE_STRING
					FntPrint("local view distance: %d\n", LocalViewDistance);
					AFTER_STRING

					break;
				default:
					assert(FALSE);
				}
			break;
		default:
			assert(FALSE);		// should never get here
		}
}




void WriteObjectMoveInfo (void)
{
	ObjectHandler* object = ObjectArray[SelectedObject];
	assert(object != NULL);

	switch(object->inOrbit)
		{
		case TRUE:
			FntPrint("object is in orbit\n");
			break;
		case FALSE:
			FntPrint("object is not in orbit\n");
			break;
		default:
			assert(FALSE);				// should never get here
		}

	if (object->inOrbit == TRUE)
		{
		BEFORE_STRING
		FntPrint("orbital radius: %d\n",
			object->radius);
		AFTER_STRING
			   

		BEFORE_STRING
		switch(object->whichWay)
			{
			case CLOCKWISE:
				FntPrint("orbit clockwise\n");
				break;
			case ANTICLOCKWISE:
				FntPrint("orbit anticlockwise\n");
				break;
			default:	
				assert(FALSE);		// should never get here
			}
		AFTER_STRING
		}	
	else
		{
		BEFORE_STRING
		FntPrint("position: %d %d %d\n",
			object->position.vx,
			object->position.vy,
			object->position.vz);
		AFTER_STRING


		BEFORE_STRING
		FntPrint("momentum: %d\n",
			object->movementMomentumFlag);
		AFTER_STRING
		}
}




void WriteObjectSpinInfo (void)
{
	ObjectHandler* object = ObjectArray[SelectedObject];
	assert(object != NULL);

	BEFORE_STRING
	FntPrint("Angle: %d %d %d\n",
		object->rotate.vx,
		object->rotate.vy,
		object->rotate.vz);
	AFTER_STRING
			
	BEFORE_STRING
	FntPrint("Rotational momentum: %d\n",
		object->rotationMomentumFlag);
	AFTER_STRING
}






void WriteObjectVolumeInfo (void)
{
	VECTOR dimensions;
	ObjectHandler* object = ObjectArray[SelectedObject];
	assert(object != NULL);

#if 0		// DOES NOT find absolute volume:
			// only finds RELATIVE vol to 'normal size for this TMD'
	FindObjectDimensions (object, &dimensions);
	FntPrint("dimensions: %d %d %d\n", 
		dimensions.vx, dimensions.vy, dimensions.vz);
#endif
	
	BEFORE_STRING
	FntPrint("scale X: %d\n", object->scaleX);
	AFTER_STRING


	BEFORE_STRING
	FntPrint("scale Y: %d\n", object->scaleY);
	AFTER_STRING


	BEFORE_STRING
	FntPrint("scale Z: %d\n", object->scaleZ);
	AFTER_STRING
}




void WriteChangeDisplayInfo (void)
{
	int boolean, semiTransparencySetting = 0;
	ObjectHandler* object = ObjectArray[SelectedObject];
	assert(object != NULL);

	// bit 3: toggle normal lighting / fog only mode; GsFOG
	// bit 6: toggle light calculations; GsLOFF
	// 28 and 29: semi-transparency flags; 
	// use as 1 four-value int: GsAZERO <-> GsATHREE
	// bit 30: semi-transparency on/off: GsALON
	// bit 31: display on/off: GsDOFF

	boolean = 
		object->handler.attribute & GsFOG;
	BEFORE_STRING
	if (boolean)
		FntPrint("fog only\n");
	else
		FntPrint("normal lighting\n");
	AFTER_STRING
			

	boolean = 
		object->handler.attribute & GsLOFF;
	BEFORE_STRING
	if (boolean)
		FntPrint("light calculations OFF\n");
	else
		FntPrint("light calculations ON\n"); 
	AFTER_STRING

		// calculate current semi-transparency setting
	if (object->handler.attribute & 1<<28)
		semiTransparencySetting++;
	if (object->handler.attribute & 1<<29)
		semiTransparencySetting += 2;

	BEFORE_STRING
	FntPrint("semi-trans type: %d\n", 
		semiTransparencySetting);
	AFTER_STRING


	boolean = 
		object->handler.attribute & GsALON;
	BEFORE_STRING
	if (boolean)
		FntPrint("semi-transparency ON\n");
	else
		FntPrint("semi-transparency OFF\n");
	AFTER_STRING


	boolean = 
		object->handler.attribute & GsDOFF;	
	BEFORE_STRING
	if (boolean)
		FntPrint("display OFF\n");
	else
		FntPrint("display ON\n"); 
	AFTER_STRING
}




	// select and start together to quit 
	// start alone to pause

int DealWithController (void)
{
	static int resetCounter = 0;
	int currentFrame;			// for stepping

	if (SuperMode == MENUS)
		{
	   		// Most actions are dependent on selection mode
		switch(SelectionMode)
			{
			case MOVE_SHIP:		  // player moving
				HandleShipMoving();
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
				assert(FALSE);		// should never get here
			}
		}
	else
		HandleShipMoving();

		// Rup: select previous option      
	if (PadStatus & PADRup && SuperMode == MENUS)
		{
		if (framesSinceLastSwitch > NORMAL_TIME_DELAY)
			{
			if (SelectedOption == 0)
				SelectedOption = NumberOfOptions-1;
			else
				SelectedOption--;
			framesSinceLastSwitch = 0;
			}
		}

		// Rdown: select next option
	if (PadStatus & PADRdown && SuperMode == MENUS)
		{
		if (framesSinceLastSwitch > NORMAL_TIME_DELAY)
			{
			if (SelectedOption == NumberOfOptions-1)
				SelectedOption = 0;
			else
				SelectedOption++;
			framesSinceLastSwitch = 0;
			}
		}

	if (SelectedOption == 0)			// i.e. SelectionMode highlighted
		{
			// Rright: get next selection mode
		if (PadStatus & PADRright && SuperMode == MENUS)
			{
			if (framesSinceLastSwitch > NORMAL_TIME_DELAY)
				{
					// unhighlight object if move out of SELECT_OBJECT mode
				if (SelectionMode == SELECT_OBJECT)
					{
					assert(SelectedObject >= 0 && SelectedObject < MAX_OBJECTS);
					assert( ObjectArray[SelectedObject] != NULL);
					UnHighlightObject( ObjectArray[SelectedObject]);
					}
				if (SelectionMode == HIGHEST_SELECTION_MODE)
					SelectionMode = 0;
				else
					SelectionMode++;
					// highlight object if move into of SELECT_OBJECT mode
				if (SelectionMode == SELECT_OBJECT)
					{
					assert(SelectedObject >= 0 && SelectedObject < MAX_OBJECTS);
					assert( ObjectArray[SelectedObject] != NULL);
					HighlightObject( ObjectArray[SelectedObject]);
					}
				ResetSelectionData();
				framesSinceLastSwitch = 0;
				}
			}

			// Rleft: get next selection mode
		if (PadStatus & PADRleft && SuperMode == MENUS)
			{
			if (framesSinceLastSwitch > NORMAL_TIME_DELAY)
				{
					// unhighlight object if move out of SELECT_OBJECT mode
				if (SelectionMode == SELECT_OBJECT)
					{
					assert(SelectedObject >= 0 && SelectedObject < MAX_OBJECTS);
					assert( ObjectArray[SelectedObject] != NULL);
					UnHighlightObject( ObjectArray[SelectedObject]);
					}
				if (SelectionMode == 0)
					SelectionMode = HIGHEST_SELECTION_MODE;
				else
					SelectionMode--;
					// highlight object if move into of SELECT_OBJECT mode
				if (SelectionMode == SELECT_OBJECT)
					{
					assert(SelectedObject >= 0 && SelectedObject < MAX_OBJECTS);
					assert( ObjectArray[SelectedObject] != NULL);
					HighlightObject( ObjectArray[SelectedObject]);
					}
				ResetSelectionData();
				framesSinceLastSwitch = 0;
				}
			}
		}

   		// L2 and R2 held down together for a while: reset all
	if (PadStatus & PADL2 && PadStatus & PADR2) 
		{
		resetCounter++;
		if (resetCounter > LONG_TIME_DELAY)			   
			{
			ResetAll();
			resetCounter = 0;
			}
		}
	else	
		resetCounter = 0;

		// start & select: quit
	if (PadStatus & PADselect && PadStatus & PADstart) 
		{
		return 0;
		}

		// L2 & R2: step frame by frame
	if (PadStatus & PADL2 && PadStatus & PADR2)
		{
		currentFrame = VSync(-1);		// number of V_BLANKS since program began
		while (VSync(-1) < currentFrame + 25)
			;
		}

#if 0		// DISABLED to allow toggling of super mode: MOVE/MENUS
		// start: pause
	if (PadStatus & PADstart)
		{
		while (PadStatus & PADstart)
			{
			PadStatus = PadRead();
			}
		}
#endif

	if (PadStatus & PADstart)
		{
		if (framesSinceLastSwitch > NORMAL_TIME_DELAY)
			{
			SuperMode = 1 - SuperMode;
			framesSinceLastSwitch = 0;
			}
		}
		
		// return value: program does not quit yet		  	   
	return 1;
}




void ResetSelectionData (void)
{
	NumberOfOptions = FindNumberOfOptions();
	SelectedOption = 0;
}



	// how many interactive options are there on the screen at this time
	// ie number of interactive text menu items
int FindNumberOfOptions (void)
{
	int numberOfOptions;

	switch(SelectionMode)
		{
		case MOVE_SHIP:
			numberOfOptions = 2;
			break;
		case CHANGE_LIGHTS:
			numberOfOptions = 4;
			break;
		case SET_AMBIENT_LIGHT:		
			numberOfOptions = 4;
			break;
		case FOGGING:
			numberOfOptions = 5;
			break;
		case VIEW_DISTORTION:
			switch(ViewMode)
				{
				case GENERAL_DISTORTION:
				case MOVE_BOTH_ABSOLUTELY:
					numberOfOptions = 4;
					break;
				case MOVE_REFERENCE:
				case MOVE_VIEWING_POINT:
				case CHANGE_SUPER:
					numberOfOptions = 3;
					break;
				default:				// should never get here
					assert(FALSE);
				}
			break;
		case SELECT_OBJECT:
			switch(ObjectSelectionMode)
				{
				case MOVE_OBJECT:
					numberOfOptions = 5;
					break;
				case CHANGE_SPIN:
					numberOfOptions = 5;
					break;
				case CHANGE_VOLUME:
					numberOfOptions = 6;
					break;
				case CHANGE_DISPLAY:
					numberOfOptions = 8;
					break;
				case OBJECT_VIEWS:
					if (SpecialViewFlag == FALSE)
						numberOfOptions = 4;
					else 
						{
						assert(SpecialViewFlag == TRUE);	
						switch(SpecialViewType)
							{
							case EXTERNAL_VIEW:
								numberOfOptions = 6;
								break;			 
							case CIRCLING_VIEW:
								numberOfOptions = 8;
								break;
							case FLYBY_VIEW:		// depends on object: orbiting or flying ?
								assert(SelectedObject >= 0);
								assert(SelectedObject < MAX_OBJECTS);
								assert( ObjectArray[SelectedObject] != NULL);
								if (ObjectArray[SelectedObject]->inOrbit == TRUE)
									{
									numberOfOptions = 4;
									}
								else
									{
									numberOfOptions = 7;
									}
								break;
							case DIRECTORS_VIEW:
								numberOfOptions = 4;
								break;
							case LOCAL_VIEW:
								numberOfOptions = 7;
								break;
							default:				// should never get here
								assert(FALSE);
							}
						}					
					break;
				default:				// should never get here
					assert(FALSE);
				}
			break;
		default:				// should never get here
			assert(FALSE);
		}

	return numberOfOptions;
}










void HandleShipMoving (void)
{
	int speedMultiplier;			// movement rate
	static int framesSinceLastFire = 0;

	if (PadStatus & PADselect)
		speedMultiplier = 10;		// fast movement
	else
		speedMultiplier = 1;		// slow movement


	if (SuperMode == MENUS)
		return;

	if (PadStatus & PADLleft)
		{
		PlayersShuttle.twist.vz -= RotationSpeed * speedMultiplier;;
		if (TheMainView.super == &PlayersShuttle.coord)
			ViewChanged = TRUE;
		}
	if (PadStatus & PADLright)
		{
		PlayersShuttle.twist.vz += RotationSpeed * speedMultiplier;;
		if (TheMainView.super == &PlayersShuttle.coord)
			ViewChanged = TRUE;
		}
	if (PadStatus & PADLup)
		{
		PlayersShuttle.twist.vx -= RotationSpeed * speedMultiplier;;
		if (TheMainView.super == &PlayersShuttle.coord)
			ViewChanged = TRUE;
		}
	if (PadStatus & PADLdown)
		{
		PlayersShuttle.twist.vx += RotationSpeed * speedMultiplier;;
		if (TheMainView.super == &PlayersShuttle.coord)
			ViewChanged = TRUE;
		}
	if (PadStatus & PADL1)
		{
		PlayersShuttle.velocity.vz -= MovementSpeed * speedMultiplier;
		if (TheMainView.super == &PlayersShuttle.coord)
			ViewChanged = TRUE;
		}
	if (PadStatus & PADL2)
		{
		PlayersShuttle.velocity.vz += MovementSpeed * speedMultiplier;
		if (TheMainView.super == &PlayersShuttle.coord)
			ViewChanged = TRUE;
		}


	if (PadStatus & PADR2)
		{
		if (framesSinceLastFire > FiringRate)
			{
			// FireAShot();		// ashot move in direction of ship ie m[2]
			framesSinceLastFire = 0;
			}
		}

	framesSinceLastFire = 0;
			




#if 0	// OLD OLD OLD
	if (SelectedOption == 1)		// movement and rotation of ship
		{
		if ( (PadStatus & PADR1) == 0)		 // R1 not down: movement
			{
				// this chunk: left directional pad plus L1/L2: 
				// move player's ship
			if (PadStatus & PADLleft)
				{
				PlayersShuttle.velocity.vx -= MovementSpeed * speedMultiplier;
				if (TheMainView.super == &PlayersShuttle.coord)
					ViewChanged = TRUE;
				}
			if (PadStatus & PADLright)
				{
				PlayersShuttle.velocity.vx += MovementSpeed * speedMultiplier;
				if (TheMainView.super == &PlayersShuttle.coord)
					ViewChanged = TRUE;
				}
			if (PadStatus & PADLup)
				{
				PlayersShuttle.velocity.vy -= MovementSpeed * speedMultiplier;
				if (TheMainView.super == &PlayersShuttle.coord)
					ViewChanged = TRUE;
				}
			if (PadStatus & PADLdown)
				{
				PlayersShuttle.velocity.vy += MovementSpeed * speedMultiplier;
				if (TheMainView.super == &PlayersShuttle.coord)
					ViewChanged = TRUE;
				}
			if (PadStatus & PADL1)
				{
				PlayersShuttle.velocity.vz -= MovementSpeed * speedMultiplier;
				if (TheMainView.super == &PlayersShuttle.coord)
					ViewChanged = TRUE;
				}
			if (PadStatus & PADL2)
				{
				PlayersShuttle.velocity.vz += MovementSpeed * speedMultiplier;
				if (TheMainView.super == &PlayersShuttle.coord)
					ViewChanged = TRUE;
				}
			}
		else
			{			   			// rotation of ship
				// this chunk: left directional pad plus L1/L2: 
				// move player's ship
			if (PadStatus & PADLleft)
				{
				PlayersShuttle.twist.vy -= RotationSpeed * speedMultiplier;
				if (TheMainView.super == &PlayersShuttle.coord)
					ViewChanged = TRUE;
				}
			if (PadStatus & PADLright)
				{
				PlayersShuttle.twist.vy += RotationSpeed * speedMultiplier;
				if (TheMainView.super == &PlayersShuttle.coord)
					ViewChanged = TRUE;
				}
			if (PadStatus & PADLup)
				{
				PlayersShuttle.twist.vx -= RotationSpeed * speedMultiplier;
				if (TheMainView.super == &PlayersShuttle.coord)
					ViewChanged = TRUE;
				}
			if (PadStatus & PADLdown)
				{
				PlayersShuttle.twist.vx += RotationSpeed * speedMultiplier;
				if (TheMainView.super == &PlayersShuttle.coord)
					ViewChanged = TRUE;
				}
			if (PadStatus & PADL1)
				{
				PlayersShuttle.twist.vz -= RotationSpeed * speedMultiplier;
				if (TheMainView.super == &PlayersShuttle.coord)
					ViewChanged = TRUE;
				}
			if (PadStatus & PADL2)
				{
				PlayersShuttle.twist.vz += RotationSpeed * speedMultiplier;
				if (TheMainView.super == &PlayersShuttle.coord)
					ViewChanged = TRUE;
				}
			}
		}
#endif
}






void HandleLightControls (void)
{
	int lightChange;			  // how fast light changes

		// only three lights
	assert(SelectedLight >= 0 && SelectedLight <= 2);

	if (PadStatus & PADselect)	   
		lightChange = 10;
	else
		lightChange = 1;
					 

	if (SelectedOption == 1)			// select next light
		{
			// Lleft: get previous light
		if (PadStatus & PADLleft)
			{
			if (framesSinceLastLightChange > NORMAL_TIME_DELAY)	
				{
				if (SelectedLight == 0)
					SelectedLight = 2;
				else
					SelectedLight--;
				framesSinceLastLightChange = 0;
				}
			}
			// Lright: get next light
		if (PadStatus & PADLright)
			{
			if (framesSinceLastLightChange > NORMAL_TIME_DELAY)	
				{
				if (SelectedLight == 2)
					SelectedLight = 0;
				else
					SelectedLight++;
				framesSinceLastLightChange = 0;
				}
			}
		}


	if (SelectedOption == 2)	 // alter the direction of the selected light
		{				   
		if (PadStatus & PADLleft)
			{
			lightSourceArray[SelectedLight].vx -= 10 * lightChange;
			LightsChanged = TRUE;
			}
		if (PadStatus & PADLright)
			{
			lightSourceArray[SelectedLight].vx += 10 * lightChange;
			LightsChanged = TRUE;
			}
		if (PadStatus & PADLup)
			{
			lightSourceArray[SelectedLight].vy += 10 * lightChange;
			LightsChanged = TRUE;
			}
		if (PadStatus & PADLdown)
			{
			lightSourceArray[SelectedLight].vy -= 10 * lightChange;
			LightsChanged = TRUE;
			}
		if (PadStatus & PADL1)
			{
			lightSourceArray[SelectedLight].vz += 10 * lightChange;
			LightsChanged = TRUE;
			}
 		if (PadStatus & PADL2)
			{
			lightSourceArray[SelectedLight].vz -= 10 * lightChange;
			LightsChanged = TRUE;
			}
		}
 
	if (SelectedOption == 3)		// change colour of selected light
		{
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

	
	if (SelectedOption == 1)
		{
			// alter the ambient red colour
		if (PadStatus & PADLup)
			{
			AmbientRed += colourChange;
			AmbientLightChanged = TRUE;
			}
		if (PadStatus & PADLdown)
			{
			AmbientRed -= colourChange;
			AmbientLightChanged = TRUE;
			}
		}

	if (SelectedOption == 2)
		{
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
		}

	if (SelectedOption == 3)
		{
   		if (PadStatus & PADLup)
			{
			AmbientBlue += colourChange;
			AmbientLightChanged = TRUE;
			}
		if (PadStatus & PADLdown)
			{
			AmbientBlue -= colourChange;
			AmbientLightChanged = TRUE;
			}
		}	
}


void HandleFoggingControls (void)
{
	int fogChange;			 // rate of change of fog parameters
	
	if (PadStatus & PADselect)	// how fast fog paramters change
		fogChange = 20;
	else
		fogChange = 2;

	if (SelectedOption == 1)				// change dqa
		{
   		if (PadStatus & PADLup)
			{
			FoggingData.dqa += fogChange;
			FoggingChanged = TRUE;
			}
		if (PadStatus & PADLdown)
			{
			FoggingData.dqa -= fogChange;
			FoggingChanged = TRUE;
			}
		}

	if (SelectedOption == 2)			   // change dqb
		{
		if (PadStatus & PADLup)
			{
			FoggingData.dqb += fogChange;
			FoggingChanged = TRUE;
			}
		if (PadStatus & PADLdown)
			{
			FoggingData.dqb -= fogChange;
			FoggingChanged = TRUE;
			}
		}


	if (SelectedOption == 3)		  	// alter the fog-in-distance colour
		{
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

			// R1: make colour black
		if (PadStatus & PADR1)
			{
			FoggingData.rfc = 0;		
			FoggingData.gfc = 0;
			FoggingData.bfc = 0;
			FoggingChanged = TRUE;
			}
			// R2: make colour white
		if (PadStatus & PADR2)
			{
			FoggingData.rfc = 255;		
			FoggingData.gfc = 255;
			FoggingData.bfc = 255;
			FoggingChanged = TRUE;
			}
		}
		
	if (SelectedOption == 4)			// toggle overall lighting mode
		{
		if (PadStatus & PADLup)
			{
			if (framesSinceLastLightModeChanged > NORMAL_TIME_DELAY)
				{
					// only two valid values
				assert(OverallLightingMode == 0
					|| OverallLightingMode == 1);

					// flip: either zero or one
				OverallLightingMode = 1 - OverallLightingMode;

				framesSinceLastLightModeChanged = 0;
				LightingModeChanged = TRUE;
		   		}
			}
		}
}





void HandleViewControls (void)
{
	int viewChange;			// rate of change of viewpoint

	if (PadStatus & PADselect)	   // how fast viewpoint changes
		viewChange = 10;
	else
		viewChange = 1;


	if (SelectedOption == 1)		// view submode
		{
			// Lright: get next view mode
		if (PadStatus & PADLright)
			{
			if (numberFramesSinceViewModeChange > NORMAL_TIME_DELAY)
				{
				if (ViewMode == MAX_VIEW_MODE)
					ViewMode = 0;
				else
					ViewMode++;
				numberFramesSinceViewModeChange = 0;
				ResetSelectionData();
				SelectedOption = 1;
				return;
				}
			}

			// Lleft: get previous view mode
		if (PadStatus & PADLleft)
			{
			if (numberFramesSinceViewModeChange > NORMAL_TIME_DELAY)
				{
				if (ViewMode == 0)
					ViewMode = MAX_VIEW_MODE;
				else
					ViewMode--;
				numberFramesSinceViewModeChange = 0;
				ResetSelectionData();
				SelectedOption = 1;
				return;
				}
			}
		}

	switch(ViewMode)
		{
		case GENERAL_DISTORTION:
			if (SelectedOption == 2)		// change the 'rz' distortion
				{
				if (PadStatus & PADLup)
					{
					TheMainView.rz += 10 * viewChange;
					ViewChanged = TRUE;
					}
				if (PadStatus & PADLdown)
					{
					TheMainView.rz -= 10 * viewChange;
					ViewChanged = TRUE;
					}
				}

			if (SelectedOption == 3)			// change projection distance
				{
				if (PadStatus & PADLup)
					{
					ProjectionDistance += 10 * viewChange;
					ProjectionChanged = TRUE;
					}
				if (PadStatus & PADLdown)
					{
					ProjectionDistance -= 10 * viewChange;
					ProjectionChanged = TRUE;
					}
				}
			break;
		case MOVE_BOTH_ABSOLUTELY:
			if (SelectedOption == 2)		// move the reference point absolutely
				{
				if (PadStatus & PADLleft)
					{
					TheMainView.vrx += 20 * viewChange;
					ViewChanged = TRUE;
					}
				if (PadStatus & PADLright)
					{
					TheMainView.vrx -= 20 * viewChange;
					ViewChanged = TRUE;
					}
				if (PadStatus & PADLup)
					{
					TheMainView.vry += 20 * viewChange;
					ViewChanged = TRUE;
					}
				if (PadStatus & PADLdown)
					{
					TheMainView.vry -= 20 * viewChange;
					ViewChanged = TRUE;
					}
				if (PadStatus & PADL1)
					{
					TheMainView.vrz += 20 * viewChange;
					ViewChanged = TRUE;
					}
				if (PadStatus & PADL2)
					{
					TheMainView.vrz -= 20 * viewChange;
					ViewChanged = TRUE;
					}
				}

			if (SelectedOption == 3)		// move the viewing point absolutely
				{
				if (PadStatus & PADLleft)
					{
					TheMainView.vpx += 20 * viewChange;
					ViewChanged = TRUE;
					}
				if (PadStatus & PADLright)
					{
					TheMainView.vpx -= 20 * viewChange;
					ViewChanged = TRUE;
					}
				if (PadStatus & PADLup)
					{
					TheMainView.vpy += 20 * viewChange;
					ViewChanged = TRUE;
					}
				if (PadStatus & PADLdown)
					{
					TheMainView.vpy -= 20 * viewChange;
					ViewChanged = TRUE;
					}
				if (PadStatus & PADL1)
					{
					TheMainView.vpz += 20 * viewChange;
					ViewChanged = TRUE;
					}
				if (PadStatus & PADL2)
					{
					TheMainView.vpz -= 20 * viewChange;
					ViewChanged = TRUE;
					}
				}
			break;
		case MOVE_REFERENCE:
				// NOT HANDLED YET
			break;
		case MOVE_VIEWING_POINT:
				// NOT HANDLED YET
			break;
		case CHANGE_SUPER:
			if (SelectedOption == 2)			// change SUPER coord of view
				{	
					// Lup: view from next object up
				if (PadStatus & PADLup)
					{
					if (numberFramesSinceViewpointChange > NORMAL_TIME_DELAY)
						{
						if (MainViewObjectID >= 0)		// Not world or ship
							{
							MainViewObjectID 
								= FindNextHighestObjectID(MainViewObjectID);
							}
						else			// get first object
							{
							MainViewObjectID 
								= FindNextHighestObjectID(-1);
							}
						assert(MainViewObjectID >= 0
							&& MainViewObjectID < MAX_OBJECTS);
						assert(ObjectArray[MainViewObjectID] != NULL);

						TheMainView.super 
							= &ObjectArray[MainViewObjectID]->coord;		
						ViewChanged = TRUE;
						numberFramesSinceViewpointChange = 0;
						}
					}

					// Ldown: view from next object down
				if (PadStatus & PADLdown)
					{
					if (numberFramesSinceViewpointChange > NORMAL_TIME_DELAY)
						{
						if (MainViewObjectID >= 0)		// Not world or ship
							{
							MainViewObjectID 
								= FindNextLowestObjectID(MainViewObjectID);
							}
						else
							{
							MainViewObjectID 
								= FindNextLowestObjectID(MAX_OBJECTS);
							}
							
						assert(MainViewObjectID >= 0
							&& MainViewObjectID < MAX_OBJECTS);
						assert(ObjectArray[MainViewObjectID] != NULL);

					   	TheMainView.super 
							= &ObjectArray[MainViewObjectID]->coord;
						ViewChanged = TRUE;
						numberFramesSinceViewpointChange = 0;
						}
					}
				
					// Lleft: force return to WORLD view
				if (PadStatus & PADLleft)
					{
					if (numberFramesSinceViewpointChange > NORMAL_TIME_DELAY)
						{
						MainViewObjectID = -1;		  // set to world
						TheMainView.super = WORLD;
									
						ViewChanged = TRUE;
						numberFramesSinceViewpointChange = 0;
						}
					}

					// Lright: force return to PlayersShuttle view
				if (PadStatus & PADLright)
					{
					if (numberFramesSinceViewpointChange > NORMAL_TIME_DELAY)
						{
						MainViewObjectID = -2;		  // set to player's ship
						TheMainView.super = &(PlayersShuttle.coord);
									
						ViewChanged = TRUE;
						numberFramesSinceViewpointChange = 0;
						}
					}
				}
			break;
		default:			   // should never get here
			assert(FALSE);
		} 
}






void HandleObjectSelection (void)
{
	if (SelectedOption == 1)			// object submode
		{
			// Lright: select next submode
		if (PadStatus & PADLright)
			{
			if (framesSinceLastSelection > NORMAL_TIME_DELAY)
				{
				if (ObjectSelectionMode == MAX_OBJECT_SELECTION_MODE)
					ObjectSelectionMode = 0;
				else
					ObjectSelectionMode++;
				framesSinceLastSelection = 0;
				ResetSelectionData();
				SelectedOption = 1;
				return;
				}
			}
			// Lleft: select previous submode
		if (PadStatus & PADLleft)
			{
			if (framesSinceLastSelection > NORMAL_TIME_DELAY)
				{
				if (ObjectSelectionMode == 0)
					ObjectSelectionMode = MAX_OBJECT_SELECTION_MODE;
				else
					ObjectSelectionMode--;
				framesSinceLastSelection = 0;
				ResetSelectionData();
				SelectedOption = 1;
				return;
				}
			}
		}

	if (SelectedOption == 2)
		{
			// Lup: select next object
		if (PadStatus & PADLup)
			{
			if (framesSinceLastSelection > NORMAL_TIME_DELAY)
				{
				assert(ObjectArray[SelectedObject] != NULL);
				UnHighlightObject( ObjectArray[SelectedObject]);
				SelectedObject = FindNextHighestObjectID (SelectedObject);
				assert(ObjectArray[SelectedObject] != NULL);
				HighlightObject( ObjectArray[SelectedObject]);
				framesSinceLastSelection = 0;
				return;
				}
			}
			// Ldown: select previous object
		if (PadStatus & PADLdown)
			{
			if (framesSinceLastSelection > NORMAL_TIME_DELAY)
				{
				assert(ObjectArray[SelectedObject] != NULL);
			   	UnHighlightObject( ObjectArray[SelectedObject]);
				SelectedObject = FindNextLowestObjectID (SelectedObject);
				assert(ObjectArray[SelectedObject] != NULL);
				HighlightObject( ObjectArray[SelectedObject]);
				framesSinceLastSelection = 0;
				return;
				}
			}
		}

		// check that selected ID is valid before indexing array
	assert(SelectedObject >= 0);
	assert(SelectedObject < MAX_OBJECTS);
	assert(ObjectArray[SelectedObject] != NULL);

	switch(ObjectSelectionMode)
		{
		case MOVE_OBJECT:
			HandleMovingAnObject();
			break;
		case CHANGE_SPIN:
			HandleChangingAnObjectsSpin();
			break;
		case CHANGE_VOLUME:
			HandleChangingAnObjectsVolume();
			break;
		case CHANGE_DISPLAY:
			HandleChangingAnObjectsDisplay();
			break;
		case OBJECT_VIEWS:
				// options: external view (above, left, front, etc)
				// circling: which plane, radius, speed, etc
				// flyby; local view; director's cut

			if (SelectedOption == 3)		// which type of special view
				{					
					// Lleft: get next special view
				if (PadStatus & PADLleft)
					{
					if (framesSinceLastSelection > NORMAL_TIME_DELAY)
						{
						if (SpecialViewFlag == FALSE)
							{
							SpecialViewFlag = TRUE;
							SpecialViewType = EXTERNAL_VIEW;
							}
						else
							{
							if (SpecialViewType == MAX_SPECIAL_VIEW)
								SpecialViewType = 0;
							else
								SpecialViewType++;
							}
						framesSinceLastSelection = 0;
						ResetSelectionData();
						SelectedOption = 3;
						SetUpSpecialView();
						return;
						}
					}

					// Lright : get previous special view
				if (PadStatus & PADLright)
					{
					if (framesSinceLastSelection > NORMAL_TIME_DELAY)
						{
						if (SpecialViewFlag == FALSE)
							{
							SpecialViewFlag = TRUE;
							SpecialViewType = EXTERNAL_VIEW;
							}
						else
							{
							if (SpecialViewType == 0)
								SpecialViewType = MAX_SPECIAL_VIEW;
							else
								SpecialViewType--;
							}
						framesSinceLastSelection = 0;
						ResetSelectionData();
						SelectedOption = 3;
						SetUpSpecialView();
						return;
						}
					}

					// Ldown: force return to default i.e. WORLD view
				if (PadStatus & PADLdown)
					{
					if (framesSinceLastSelection > NORMAL_TIME_DELAY)
						{
						SpecialViewFlag = FALSE;
						SpecialViewType = NONE;
						TheMainView.super = WORLD;
						ViewChanged = TRUE;
						framesSinceLastSelection = 0;
						ResetSelectionData();
						SelectedOption = 3;
						return;
						}
					}
				}

				// user interaction depends on which special view
			if (SpecialViewFlag)
				{
				switch(SpecialViewType)
					{
					case EXTERNAL_VIEW:
						HandleExternalView();
						break;
					case CIRCLING_VIEW:
						HandleCirclingView();
						break;
					case FLYBY_VIEW:
						HandleFlybyView();
						break;
					case DIRECTORS_VIEW:		// no specific user interaction here
						break;
					case LOCAL_VIEW:
						HandleLocalView();
						break;
					default:
						assert(FALSE);			 // should never get here
					}
				}	 
			break;
		default:
			assert(FALSE);			// should never get here
		}
}




   


void HandleMovingAnObject (void)
{
	int speedMultiplier, movementModifier;
	ObjectHandler* object; 

	if (PadStatus & PADselect)
		speedMultiplier = 10;		// fast movement
	else
		speedMultiplier = 1;		// slow movement

	assert(SelectedObject >= 0);
	assert(SelectedObject < MAX_OBJECTS);
	object = ObjectArray[SelectedObject];
	assert(object != NULL);

	if (object->inOrbit == TRUE)
		{
		if (SelectedOption == 3)		  // change object radius
			{
			if (PadStatus & PADLup)
				{
				object->radius += speedMultiplier;
				}
			if (PadStatus & PADLdown)
				{
				object->radius -= speedMultiplier;
				}
			}

		if (SelectedOption == 4)		  // toggle which way round object follows orbit
			{
			if (PadStatus & PADLup)
				{
				if (framesSinceLastSelection > NORMAL_TIME_DELAY)
					{
					if (object->whichWay == CLOCKWISE)
						object->whichWay = ANTICLOCKWISE;
					else
						object->whichWay = CLOCKWISE;
					}
				framesSinceLastSelection = 0;
				}
			}
		}
	else
		{
		if (SelectedOption == 3)		  // change object velocity
			{
			if (object->movementMomentumFlag == TRUE)
				movementModifier = 4;
			else
				movementModifier = 0;


			if (PadStatus & PADLleft)
				{
				object->velocity.vx 
						-= MovementSpeed * speedMultiplier >> movementModifier;
				}
			if (PadStatus & PADLright)
				{
				object->velocity.vx 
						 += MovementSpeed * speedMultiplier >> movementModifier;
				}
			if (PadStatus & PADLup)
				{
				object->velocity.vy 
						 -= MovementSpeed * speedMultiplier >> movementModifier;
				}
			if (PadStatus & PADLdown)
				{
				object->velocity.vy 
						 += MovementSpeed * speedMultiplier >> movementModifier;
				}
			if (PadStatus & PADL1)
				{
				object->velocity.vz 
						 -= MovementSpeed * speedMultiplier >> movementModifier;
				}
			if (PadStatus & PADL2)
				{
				object->velocity.vz 
						 += MovementSpeed * speedMultiplier >> movementModifier;
				}
			}

		if (SelectedOption == 4)		  // turn object momentum on and off
			{
			if (PadStatus & PADLleft)
				{
				if (framesSinceLastSelection > NORMAL_TIME_DELAY)
					{
					object->movementMomentumFlag = FALSE; 
					framesSinceLastSelection = 0;
					}
				}
			if (PadStatus & PADLright)
				{
				if (framesSinceLastSelection > NORMAL_TIME_DELAY)
					{
					object->movementMomentumFlag = TRUE;
					framesSinceLastSelection = 0;
					}
				}
			}
		}
}





void HandleChangingAnObjectsSpin (void)
{
	int speedMultiplier; 
	ObjectHandler* object;

	if (PadStatus & PADselect)
		speedMultiplier = 10;		// fast twisting
	else
		speedMultiplier = 1;		// slow twisting

	assert(SelectedObject >= 0);
	assert(SelectedObject < MAX_OBJECTS);
	object = ObjectArray[SelectedObject];
	assert(object != NULL);

	if (SelectedOption == 3)		// spin: twist vector
		{
			// this chunk: left directional pad plus L1/L2: 
			// set object's spin
		if (PadStatus & PADLleft)
			{
			object->twist.vy
						-= RotationSpeed * speedMultiplier;	 
			}
		if (PadStatus & PADLright)
			{
	        object->twist.vy 
	        			+= RotationSpeed * speedMultiplier; 
			}
		if (PadStatus & PADLup)
			{
			object->twist.vx 
						-= RotationSpeed * speedMultiplier;	  
			}
		if (PadStatus & PADLdown)
			{	
			object->twist.vx 
						+= RotationSpeed * speedMultiplier;  
			}
		if (PadStatus & PADL1)
			{
	      	object->twist.vz 
						-= RotationSpeed * speedMultiplier;	  
			}
		if (PadStatus & PADL2)
			{
			object->twist.vz 
					+= RotationSpeed * speedMultiplier;	   
			}
		}

	if (SelectedOption == 4)			// angular momentum
		{
			// Lleft and Lright: set object spin momentum on or off
			// when off, this submode adjusts angle of object
		if (PadStatus & PADLleft)
			{
			if (framesSinceLastSelection > NORMAL_TIME_DELAY)
				{
				object->rotationMomentumFlag = FALSE;
				framesSinceLastSelection = 0;
				}
			}
		if (PadStatus & PADLright)
			{
			if (framesSinceLastSelection > NORMAL_TIME_DELAY)
				{
				object->rotationMomentumFlag = TRUE;
				framesSinceLastSelection = 0;
				}
			}
		}
}




void HandleChangingAnObjectsVolume (void)
{
	int volumeMultiplier; 
	ObjectHandler* object;

	if (PadStatus & PADselect)
		volumeMultiplier = 100;		// fast twisting
	else
		volumeMultiplier = 10;		// slow twisting

	assert(SelectedObject >= 0);
	assert(SelectedObject < MAX_OBJECTS);
	object = ObjectArray[SelectedObject];
	assert(object != NULL);

	if (SelectedOption == 3)			// change scaleX
		{
		if (PadStatus & PADLup)
			{
			object->scaleX += volumeMultiplier; 	  
			}
		if (PadStatus & PADLdown)
			{	
			object->scaleX -= volumeMultiplier;  
			}
		}

	if (SelectedOption == 4)			// change scaleY
		{
		if (PadStatus & PADLup)
			{
			object->scaleY += volumeMultiplier; 	  
			}
		if (PadStatus & PADLdown)
			{	
			object->scaleY -= volumeMultiplier;  
			}
		}

	if (SelectedOption == 5)			// change scaleZ
		{
		if (PadStatus & PADLup)
			{
			object->scaleZ += volumeMultiplier; 	  
			}
		if (PadStatus & PADLdown)
			{	
			object->scaleZ -= volumeMultiplier;  
			}
		}
}





void HandleChangingAnObjectsDisplay (void)
{
	int semiTransparency = 0;
	ObjectHandler* object;

	assert(SelectedObject >= 0);
	assert(SelectedObject < MAX_OBJECTS);
	object = ObjectArray[SelectedObject];
	assert(object != NULL);

	// NOTE: toggling bit 6 (light calculations flag)
	// doesn't work because that bit is used to show
	// which object is currently selected ....

	// bit 3: toggle normal lighting / fog only mode; GsFOG
	// bit 6: toggle light calculations; GsLOFF
	// 28 and 29: semi-transparency flags; 
	// use as 1 four-value int: GsAZERO <-> GsATHREE
	// bit 30: semi-transparency on/off: GsALON
	// bit 31: display on/off: GsDOFF

	if (SelectedOption == 3)		 // Lup: toggle normal lighting / fog only
		{	
		if (PadStatus & PADLup)
			{
			if (framesSinceLastSelection > NORMAL_TIME_DELAY)
				{
				if (object->handler.attribute & GsFOG)
					{
					TURN_NTH_BIT_OFF(object->handler.attribute, 32, 3)
					}  
				else
					{
					object->handler.attribute |= GsFOG;
					}
				framesSinceLastSelection = 0;
				}
			}	
		}

	if (SelectedOption == 4)		// Lup: toggle light calculations
		{ 
		if (PadStatus & PADLup)
			{
			if (framesSinceLastSelection > NORMAL_TIME_DELAY)
				{
				if (object->handler.attribute & GsLOFF)
					{
					TURN_NTH_BIT_OFF(object->handler.attribute, 32, 6)
					}  
				else
					{
					object->handler.attribute |= GsLOFF;
					}
				framesSinceLastSelection = 0;
				}
			}
		}

	if (SelectedOption == 5)		   // Lup: increase semi-transparency setting
		{
			// find current semi-transparency setting
		if (object->handler.attribute & 1<<28)
			semiTransparency++;
		if (object->handler.attribute & 1<<29)
			semiTransparency += 2;

		if (PadStatus & PADLup)
			{
			if (framesSinceLastSelection > NORMAL_TIME_DELAY)
				{
				if (semiTransparency == 3)
					semiTransparency = 0;
				else	
					semiTransparency++;

					// now set in attribute of GsDOBJ2 structure
				if (semiTransparency == 1 || semiTransparency == 3)
					object->handler.attribute |= 1<<28;
				else
					TURN_NTH_BIT_OFF(object->handler.attribute, 32, 28)
							
				if (semiTransparency == 2 || semiTransparency == 3)
					object->handler.attribute |= 1<<29;	 
				else
					TURN_NTH_BIT_OFF(object->handler.attribute, 32, 29)
				framesSinceLastSelection = 0;
				}
			}
		}

	if (SelectedOption == 6)		// Lup: toggle semi-transparency on/off
		{
		if (PadStatus & PADLup)
			{
			if (framesSinceLastSelection > NORMAL_TIME_DELAY)
				{
				if (object->handler.attribute & GsALON)
					{
					TURN_NTH_BIT_OFF(object->handler.attribute, 32, 30)
					}  
				else
					{
					object->handler.attribute |= GsALON;
					}
				framesSinceLastSelection = 0;
				}
			}
		}

	if (SelectedOption == 7)			  // Lup: toggle object display
		{
		if (PadStatus & PADLup)
			{
			if (framesSinceLastSelection > NORMAL_TIME_DELAY)
				{
				if (object->handler.attribute & GsDOFF)
					{
					TURN_NTH_BIT_OFF(object->handler.attribute, 32, 31);
					}  
				else
					{
					object->handler.attribute |= GsDOFF;
					}
				framesSinceLastSelection = 0;
				}
			}
		}
}





void HandleExternalView (void)
{
	int speedMultiplier; 

	if (PadStatus & PADselect)
		speedMultiplier = 10;		// fast twisting
	else
		speedMultiplier = 1;		// slow twisting

	if (SelectedOption == 4)		   // Lup/Ldown: change which external view	  
		{
		if (PadStatus & PADLup)
			{
			if (framesSinceLastSelection > NORMAL_TIME_DELAY)
				{
				if (WhichExternalView == PLUS_X)
					WhichExternalView = MINUS_Z;
				else
					WhichExternalView--;
				framesSinceLastSelection = 0;
				}
			}
		if (PadStatus & PADLdown)
			{
			if (framesSinceLastSelection > NORMAL_TIME_DELAY)
				{
				if (WhichExternalView == MINUS_Z)
					WhichExternalView = PLUS_X;
				else
					WhichExternalView++;
				framesSinceLastSelection = 0;
				}
			}
		}

	if (SelectedOption == 5)		   // Lup/Ldown: change view distance
		{
		if (PadStatus & PADLup)
			{
			ExternalViewDistance += 50 * speedMultiplier;
			}
		if (PadStatus & PADLdown)
		   	{
			ExternalViewDistance -= 50 * speedMultiplier;
			if (ExternalViewDistance < 100)
			ExternalViewDistance = 100;
			}
		}
}




void HandleCirclingView (void)
{
	int speedMultiplier;			// rate of change of circling view attributes

	if (PadStatus & PADselect)	   
		speedMultiplier = 10;
	else
		speedMultiplier = 1;

	if (SelectedOption == 4)		   // Lup: which plane of viewing orbit
		{
		if (PadStatus & PADLup)
			{
			if (framesSinceLastSelection > NORMAL_TIME_DELAY)
				{
				if (WhichCircularView == Y_Z_PLANE)
					WhichCircularView = X_Y_PLANE;
				else
					WhichCircularView++;
				framesSinceLastSelection = 0;
				}
			}
		}

	if (SelectedOption == 5)		   // Lup: toggle which way round the view orbits
		{
		if (PadStatus & PADLup)
			{
			if (framesSinceLastSelection > NORMAL_TIME_DELAY)
				{
				if (WhichWayRoundCirclingView == CLOCKWISE)
					WhichWayRoundCirclingView = ANTICLOCKWISE;
				else
					WhichWayRoundCirclingView = CLOCKWISE;
				framesSinceLastSelection = 0;
				}
			}
		}

	if (SelectedOption == 6)	// Lup/Ldown: change circling view radius
		{
		if (PadStatus & PADLup)
			{
			CircleViewRadius += 20 * speedMultiplier;
			}
		if (PadStatus & PADLdown)
			{
			CircleViewRadius -= 20 * speedMultiplier;
			if (CircleViewRadius < 200)
				CircleViewRadius = 200;
			}
		}
						
	if (SelectedOption == 7)	// Lup/Ldown: change circling orbital speed		
		{
		if (PadStatus & PADLup)
			{
			CircleViewAngleIncrement += speedMultiplier;
			}
		if (PadStatus & PADLdown)
			{
			CircleViewAngleIncrement -= speedMultiplier;
			if (CircleViewAngleIncrement < 1)
				CircleViewAngleIncrement = 1;
			}
		} 
}






void HandleFlybyView (void)
{
	int speedMultiplier;			// rate of change of fly-by view attributes

	if (PadStatus & PADselect)	   
		speedMultiplier = 10;
	else
		speedMultiplier = 1;

	if (TypeOfFlybyView == CIRCULAR)
		return;			// no longer supported

	if (SelectedOption == 4)	// Lup/Ldown: path distance
		{
		if (PadStatus & PADLup)
			{
			PathDistance += 20 * speedMultiplier;
			}
		if (PadStatus & PADLdown)
			{
			PathDistance -= 20 * speedMultiplier;
			if (PathDistance < 10)
				PathDistance = 10;
			}
		}

	if (SelectedOption == 5)			// Lup/Ldown: change path angle
		{
		if (PadStatus & PADLup)
			{
			PathAngle += 4 * speedMultiplier;
			}
		if (PadStatus & PADLdown)
			{
			PathAngle -= 4 * speedMultiplier;
			}
		}
		
	if (SelectedOption == 6)			// Lup/Ldown: change flyby cycle time
		{
		if (PadStatus & PADLup)
			{
			FlybyCycleTime++;
			}
		if (PadStatus & PADLdown)
			{
			FlybyCycleTime--;
			}
		}
 }			
 
 
 	   
 	   
 	    
 
 void HandleLocalView (void)
 {
	int speedMultiplier;			// rate of change of local view attributes

	if (PadStatus & PADselect)	   
		speedMultiplier = 10;
	else
		speedMultiplier = 1;

	if (SelectedOption == 4)		   // Lup/Ldown: change which local view	  
		{
		if (PadStatus & PADLup)
			{
			if (framesSinceLastSelection > NORMAL_TIME_DELAY)
				{
				if (WhichLocalView == PLUS_X)
					WhichLocalView = MINUS_Z;
				else
					WhichLocalView--;
				framesSinceLastSelection = 0;
				}
			}
		if (PadStatus & PADLdown)
			{
			if (framesSinceLastSelection > NORMAL_TIME_DELAY)
				{
				if (WhichLocalView == MINUS_Z)
					WhichLocalView = PLUS_X;
				else
					WhichLocalView++;
				framesSinceLastSelection = 0;
				}
			}
		}

	if (SelectedOption == 5)		   // Lup/Ldown: change view distance
		{
		if (PadStatus & PADLup)
			{
			LocalViewDistance += 50 * speedMultiplier;
			}
		if (PadStatus & PADLdown)
		   	{
			LocalViewDistance -= 50 * speedMultiplier;
			if (LocalViewDistance < 100)
				LocalViewDistance = 100;
			}
		}
}






	// deal with movement and rotation, etc
void HandleAllObjects (void)
{
	ObjectHandler* object;
	ObjectHandler** pointer;
	int i;
	 
	pointer = &(ObjectArray[0]);  
	for (i = 0; i < MAX_OBJECTS; i++)
		{
		if (*pointer != NULL)
			{
			if ( (*pointer)->alive == TRUE)
				{
				object = *pointer;

					// deal with movement and rotation
				UpdateCoordinateSystem (&object->rotate,
								&object->position,
								&object->coord);

				SortObjectSize (object);

					// update angle
				object->rotate.vx += object->twist.vx;
				object->rotate.vy += object->twist.vy;
				object->rotate.vz += object->twist.vz;

					// update position
				object->position.vx += object->velocity.vx;
				object->position.vy += object->velocity.vy;
				object->position.vz += object->velocity.vz;

					// momentum or not? 
				if (object->movementMomentumFlag == FALSE)
					{
					object->velocity.vx = 0;
					object->velocity.vy = 0;
					object->velocity.vz = 0;
					} 
				if (object->rotationMomentumFlag == FALSE)
					{
					object->twist.vx = 0;
					object->twist.vy = 0;
					object->twist.vz = 0;
					}
				}
			}
		pointer++;
		}

		// now deal with player's ship
	   
		// deal with movement and rotation
	UpdateCoordinateSystem4( &PlayersShuttle.rotate,
							&PlayersShuttle.twist,
							&PlayersShuttle.position,
							&PlayersShuttle.velocity,
							&PlayersShuttle.coord);


	 	// Position updating now done by UpdateCoordinateSystem2:
		// player's ship moves relative to itself, not to its super
		// ie left is relative to angle of player's ship
	 
		// momentum or not? 
	if (PlayersShuttle.movementMomentumFlag == FALSE)
		{
		PlayersShuttle.velocity.vx = 0;
		PlayersShuttle.velocity.vy = 0;
		PlayersShuttle.velocity.vz = 0;
		}
	if (PlayersShuttle.rotationMomentumFlag == FALSE)
		{
		PlayersShuttle.twist.vx = 0;
		PlayersShuttle.twist.vy = 0;
		PlayersShuttle.twist.vz = 0;
		}
}




	// Sort out all orbital movement

	// This version: Not using hierarchical coordinate system because
	// we want all the bodies to be able to spin without 
	// afffecting others:
	// hence calculate position as offset from position of
	// body which object orbits around
void DealWithSpatialMovement (void)
{
	ObjectHandler* object;
	ObjectHandler** pointer;
	int i;

	pointer = &(ObjectArray[0]);
	for (i = 0; i < MAX_OBJECTS; i++)
		{
		if (*pointer != NULL)
			{
			if ( (*pointer)->alive == TRUE)
				{		
				object = *pointer;
					  
				if (object->inOrbit == TRUE)
					{
					HandleOrbitalMovement(object);
					}
				else if (object->specialMovement != NONE)
					{
					HandleSpecialMovement(object);
					}
				}
			}
		pointer++;
		}
}





void HandleOrbitalMovement (ObjectHandler* object)
{
	ObjectHandler* orbitCentreObject;
	int centralBodyID;
	int planeX, planeY;
	int newX, newY, newZ;
	
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
			assert(FALSE);		// should never get here
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
	centralBodyID = object->centralBodyID;
	assert(centralBodyID >= 0);
	assert(centralBodyID < MAX_OBJECTS);
	assert(ObjectArray[centralBodyID] != NULL);

	orbitCentreObject = ObjectArray[centralBodyID];
					
		// the vectors are relative to size ONE
		// hence shift-right 12 to make them unit vectors
	object->position.vx 
		= orbitCentreObject->position.vx + (newX >> 12);
	object->position.vy 	
		= orbitCentreObject->position.vy + (newY >> 12);
	object->position.vz 
		= orbitCentreObject->position.vz + (newZ >> 12);

}





#define HALF_RECTANGLE 1000
#define SHIP_SPEED 10
#define SQUARE_ROUTE_OF_THREE 1.732
#define CORNER_DISTANCE (HALF_RECTANGLE * SQUARE_ROUTE_OF_THREE)

void HandleSpecialMovement (ObjectHandler* object)
{ 										
	switch (object->specialMovement)	
		{
		case FIRST_SIMPLE_PATH:
			switch(object->position.vy)
				{
				case HALF_RECTANGLE:
					switch(object->position.vz)	
						{
						case HALF_RECTANGLE:
							// rotate here
							object->rotate.vx -= ONE/4;
							object->velocity.vx = 0;
							object->velocity.vy = 0;
							object->velocity.vz = -SHIP_SPEED; 
							break;
					 	case -HALF_RECTANGLE:
							// rotate here
							object->rotate.vx -= ONE/4;
							object->velocity.vx = 0;
							object->velocity.vy = -SHIP_SPEED;
							object->velocity.vz = 0;
							break;
						}
					break;
				case -HALF_RECTANGLE:
					switch(object->position.vz)
						{
						case HALF_RECTANGLE:
							// rotate here
							object->rotate.vx -= ONE/4;
							object->velocity.vx = 0;
							object->velocity.vy = SHIP_SPEED;
							object->velocity.vz = 0; 
							break;
					 	case -HALF_RECTANGLE:
							// rotate here
							object->rotate.vx -= ONE/4;
							object->velocity.vx = 0;
							object->velocity.vy = 0;
							object->velocity.vz = SHIP_SPEED;
							break;
						}
					break;
				}
			break;
		default:				// should never get here
			assert(FALSE);
		}
}





  



void InitialiseAll(void)
{
	ResetGraph(0);					
	PadStatus = 0;		
	
	GlobalMatrix.m[0][0] = ONE;
	GlobalMatrix.m[0][1] = 0;
	GlobalMatrix.m[0][2] = 0;
	GlobalMatrix.m[1][0] = 0;
	GlobalMatrix.m[1][1] = ONE;
	GlobalMatrix.m[1][2] = 0;
	GlobalMatrix.m[2][0] = 0;
	GlobalMatrix.m[2][1] = 0;
	GlobalMatrix.m[2][2] = ONE;				   

		// graphical initialisation:
		// screen resolution 512 by 240, non-interlace, use GsGPU offset
	GsInitGraph(512 ,240, GsOFSGPU|GsNONINTER, 1, 0);	
									
	GsDefDispBuff(0, 0, 0, 256);   
	GsInit3D();						

	WorldOrderingTable[0].length = OT_LENGTH;	  
	WorldOrderingTable[1].length = OT_LENGTH;		
	WorldOrderingTable[0].org = zSortTable[0];	 
	WorldOrderingTable[1].org = zSortTable[1];	   
	
	InitialiseLighting();

	InitialiseObjects();
	
		// sort out print-to-screen font
	FntLoad(960, 256);	
	FntOpen(32, 32, 256, 200, 0, 512); 	

		// set up controller pad buffers
	PadInit(); 	 		

		// to start with, player moves around
	SuperMode = MOVE;
	SelectionMode = MOVE_SHIP;	 
	framesSinceLastSwitch = 0;

	SelectedObject = 0;
	framesSinceLastSelection = 0;
	ResetSelectionData();

	InitialiseView();
	ViewChanged = TRUE;  
}







void InitialiseObjects (void)
{
	int i;

	InitialiseObjectClass();

		// firstly: zero-clearing
	InitSingleObject( &PlayersShuttle);
	InitSingleObject( &TheSun);
	for (i = 0; i < MAX_PLANETS; i++)
		{
		InitSingleObject( &ThePlanets[i]);
		}
	for (i = 0; i < MAX_MOONS; i++)
		{
		InitSingleObject( &TheMoons[i]);
		}
	for (i = 0; i < MAX_SHIPS; i++)
		{
		InitSingleObject( &TheShips[i]);
		}

		// secondly: set up the solar system
	SetUpSolarSystem();

		// thirdly: link with models
	LinkAllObjectsToModels();

		// fourthly: link coordinates properly
	LinkAllObjectsToTheirCoordinateSystems();

		// sort player's ship: special case
	LinkObjectHandlerToTmdObject( &(PlayersShuttle.handler), 
					0, SHIP_MODEL_ADDRESS);		  // SHUTTLE_MODEL_ADDRESS
		// player's ship has its own independent coordinate system
	PlayersShuttle.handler.coord2 = &(PlayersShuttle.coord);
}








void SetUpSolarSystem (void)
{
	int i, j;
	int moonID;
	VECTOR angle1, angle2;

		// sort up specifics for player's ship

		// overall speed for moving player's ship 
	MovementSpeed = INITIAL_MOVEMENT_SPEED; 
	RotationSpeed = INITIAL_ROTATION_SPEED;
	FiringRate = 5;				// frame frequency

		// push player's ship away from solar system
		// so player can see where things are
	PlayersShuttle.position.vz = -12000;

	


		// only one Sun
	BringObjectToLife ( &TheSun, SUN, SPHERE_MODEL_ADDRESS, 3, NONE);	
		// make the sun massive
	SetObjectScaling( &TheSun, 2 * ONE, 2 * ONE, 2 * ONE); 
	RegisterObjectIntoObjectArray( &TheSun);
	

		// the planets
	for (i = 0; i < MAX_PLANETS; i++)
		{
		BringObjectToLife ( &ThePlanets[i], PLANET, SPHERE_MODEL_ADDRESS, 5, NONE);	
 
			// now make the planets orbit the sun
		RandomVector( &angle1);
		AnyUnitPerpendicularVector( &angle1, &angle2);

		PutObjectIntoOrbit ( &ThePlanets[i], TheSun.id,
			((ONE / MAX_PLANETS) * i), (ONE/720), 
				(((i+1) * PLANET_ORBIT_SIZE) + 300), RandomOrientation(),
					&angle1, &angle2);
			
			// move planets away from sun	
		ThePlanets[i].position.vx += (i+1) * PLANET_ORBIT_SIZE;

			// make the planets large
		SetObjectScaling( &ThePlanets[i], 1.4 * ONE, 1.4 * ONE, 1.4 * ONE);

		RegisterObjectIntoObjectArray( &ThePlanets[i]);

			
			// the moons
		for (j = 0; j < MAX_MOONS_PER_PLANET; j++)
			{
			moonID = i * (MAX_PLANETS) + j;

			BringObjectToLife ( &TheMoons[moonID], MOON, SPHERE_MODEL_ADDRESS, 7 + (j%3), NONE);	
		 
				// now make the moons orbit the planet
			RandomVector (&angle1);
			AnyUnitPerpendicularVector (&angle1, &angle2);
			
			PutObjectIntoOrbit ( &TheMoons[moonID], ThePlanets[i].id,
				((ONE / MAX_MOONS) * j), (ONE/180), 
					(((j+1) * MOON_ORBIT_SIZE)+100), RandomOrientation(),
						&angle1, &angle2);
	
				// move moons away from planet	
			TheMoons[moonID].position.vx += (j+1) * MOON_ORBIT_SIZE;

			RegisterObjectIntoObjectArray( &TheMoons[moonID]);
			}
		}

		// now deal with first four ships
	for (i = 0; i < 4; i++)
		{
		BringObjectToLife ( &TheShips[i], SHIP, SHIP_MODEL_ADDRESS, 0, NONE);
		TheShips[i].specialMovement = FIRST_SIMPLE_PATH;
		TheShips[i].movementMomentumFlag = TRUE;

			// make it larger to be visible
		SetObjectScaling( &TheShips[i], 4 * ONE, 4 * ONE, 4 * ONE);

			// set them up with different places and speeds
		switch(i)
			{
		   case 0: 
				TheShips[i].position.vy = HALF_RECTANGLE;
				TheShips[i].velocity.vz = -SHIP_SPEED;
				break;
			case 1: 
				TheShips[i].position.vz = -HALF_RECTANGLE;
				TheShips[i].velocity.vy = -SHIP_SPEED;
				TheShips[i].rotate.vx = 3 * ONE/4;
				break;
			case 2: 
				TheShips[i].position.vy = -HALF_RECTANGLE;
				TheShips[i].velocity.vz = SHIP_SPEED;
				TheShips[i].rotate.vx = ONE/2;
				break;
			case 3: 
				TheShips[i].position.vz = HALF_RECTANGLE;
				TheShips[i].velocity.vy = SHIP_SPEED;
				TheShips[i].rotate.vx = ONE/4;
				break;
			default:
				assert(FALSE);
			}
				
		RegisterObjectIntoObjectArray( &TheShips[i]);
		}
}





void PutObjectIntoOrbit (ObjectHandler* object, int centralBodyID,
		int initialAngle, int angleIncrement, int radius, int whichWay,
			VECTOR* firstAngle, VECTOR* secondAngle)
{
	assert(object != NULL);

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
	// ONE is normal (no-scale) size
void SetObjectScaling (ObjectHandler* object, int scaleX,
						int scaleY, int scaleZ)
{
	object->scalingFlag = TRUE;

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

	if (object->scalingFlag == FALSE)
		return;

	scalingVector.vx = object->scaleX;
	scalingVector.vy = object->scaleY;
	scalingVector.vz = object->scaleZ;

	ScaleMatrix(&object->coord.coord, &scalingVector);

		// tell graphics system that coordinate system is changed
	object->coord.flg = 0;
}





void FindObjectDimensions (ObjectHandler* object, VECTOR* dimensions)
{
	int i, j;

	dimensions->vx = 0; 
	dimensions->vy = 0; 
	dimensions->vz = 0;

	for (i = 0; i < 3; i++)
		{
		dimensions->vx += mod(object->coord.coord.m[i][0]);
		dimensions->vy += mod(object->coord.coord.m[i][1]);
		dimensions->vz += mod(object->coord.coord.m[i][2]);
		}
}

	





int FindObjectSpeed (ObjectHandler* object)
{
	int speed, squareOfSpeed;

	squareOfSpeed = pow(object->position.vx, 2) 
				+ pow(object->position.vy, 2)
				+ pow(object->position.vz, 2);

	speed = pow(squareOfSpeed, 0.5);

	return speed;
}





int FindObjectMotionType (ObjectHandler* object)
{
	if (object->inOrbit == FALSE)
		return LINEAR;
	else
	  	return CIRCULAR;
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

#if 0
		// copy translation part of position matrix
	tempMatrix.t[0] = coordSystem->coord.t[0];
	tempMatrix.t[1] = coordSystem->coord.t[1];
	tempMatrix.t[2] = coordSystem->coord.t[2];
#endif

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

#if 0		// OLD DEBUG
		// DEBUG
	if (frameNumber % 10 == 0)	
		{
		printf("twist: %d %d %d\n", twist->vx, twist->vy, twist->vz);
		printf("realTwist: %d %d %d\n", 
			realTwist.vx, realTwist.vy, realTwist.vz); 
		}
#endif
}




	// proper object-relative movement and rotation
void UpdateCoordinateSystem4 (SVECTOR* rotation, VECTOR* twist,
							VECTOR* position, VECTOR* velocity,
							GsCOORDINATE2* coordSystem)
{	
	VECTOR realMovement;
	MATRIX xMatrix, yMatrix, zMatrix;
	SVECTOR xVector, yVector, zVector;
	MATRIX copy;

	// velocity is object-relative, is firstly converted to 
	// world-relative terms
	// eg
	// find the object-local velocity in super coordinate terms
	ApplyMatrixLV(&GlobalMatrix, velocity, &realMovement);
	
		// update position by super-relative movement
	position->vx += realMovement.vx;
	position->vy += realMovement.vy;
	position->vz += realMovement.vz;


	xVector.vx = twist->vx;
	xVector.vy = 0;
	xVector.vz = 0;
	yVector.vx = 0;
	yVector.vy = twist->vy;
	yVector.vz = 0;
	zVector.vx = 0;
	zVector.vy = 0;
	zVector.vz = twist->vz;

	RotMatrix(&xVector, &xMatrix);
	RotMatrix(&yVector, &yMatrix);
	RotMatrix(&zVector, &zMatrix);

		// to get world-relative rotations instead:
		// use 

	MulMatrix0(&GlobalMatrix, &xMatrix, &copy);
	MulMatrix0(&copy, &yMatrix, &GlobalMatrix);
	MulMatrix0(&GlobalMatrix, &zMatrix, &copy);

	GlobalMatrix = copy;

		// copy GlobalMatrix
	coordSystem->coord = GlobalMatrix;
	
		// set position absolutely	
	coordSystem->coord.t[0] = position->vx;
	coordSystem->coord.t[1] = position->vy;
	coordSystem->coord.t[2] = position->vz;

		// tell GTE that coordinate system has been updated
	coordSystem->flg = 0;
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

		// viewpoint is behind ship
	TheMainView.vpx = 0; 
	TheMainView.vpy = 0; 
	TheMainView.vpz = -200;
	
		// reference point at centre of ship
	TheMainView.vrx = 0; 
	TheMainView.vry = 0; 
	TheMainView.vrz = 0;
   
	TheMainView.rz = 0;

	// OLD	MainViewObjectID = -1;		// player's ship
	MainViewObjectID = -1;
	numberFramesSinceViewpointChange = 0;
	
	TheMainView.super = &PlayersShuttle.coord;
	// NOT YET TheMainView.super = WORLD;

	ViewMode = GENERAL_DISTORTION;
	numberFramesSinceViewModeChange = 0;
	
	ViewChanged = TRUE;

	SpecialViewFlag = FALSE;
	SpecialViewType = NONE;
	
		// default values for external view: look from above
	WhichExternalView = MINUS_Y;
		// long way away to see surrounding bodies
	ExternalViewDistance = 12000;

		// reasonable default values for circling
	WhichCircularView = X_Y_PLANE;			  
	WhichWayRoundCirclingView = CLOCKWISE;		
	CircleViewRadius = 5000;	
	CircleViewAngle = 0;
	CircleViewAngleIncrement = 24;

		// default values for fly-by
	TypeOfFlybyView = NONE;
	PathDistance = 2000;	  
	PathAngle = 0;				   
	FlybyCycleTime = 30;
	NumberFlybyFramesSoFar = 0;
		

		// local view
	WhichLocalView = PLUS_Z;
	LocalViewDistance = 5000;

		// default values for director view	
	SequencePosition = 0;
	SingleViewCycleTime = 30;	  
	DirectorFrameCounter = 0; 

	DirectorLocalView = NONE;
	DirectorLocalViewDistance = 2000;
	DirectorWhichCircularView = NONE;
	DirectorWhichWayRoundCirclingView = NONE;	  
	DirectorCircleViewRadius = 2000;	
	DirectorCircleViewAngleIncrement = 24;
	DirectorPathDistance = 2000;
	DirectorPathAngle = 3072;		// 270 degrees, i.e. underneath object
}





void SetNormalView (void)
{
	SpecialViewFlag = FALSE;
	SpecialViewType = NONE;
}




void SetUpSpecialView (void)
{					  
	assert(SpecialViewFlag == TRUE);

	switch(SpecialViewType)
		{
		case EXTERNAL_VIEW:
			SetUpExternalView (WhichExternalView, ExternalViewDistance);
			break;
		case CIRCLING_VIEW:
			SetUpCirclingView (WhichCircularView, 
					WhichWayRoundCirclingView, CircleViewRadius,
					CircleViewAngle, CircleViewAngleIncrement);
			break;
		case FLYBY_VIEW:
			SetUpFlybyView (PathDistance, PathAngle, 
						FlybyCycleTime);
			break;
		case DIRECTORS_VIEW:		
			SetUpDirectorsView();
			break;
		case LOCAL_VIEW:
			SetUpLocalView (WhichLocalView, LocalViewDistance);
			break;
		default:
			assert(FALSE);			 // should never get here
		}
}



void SetUpExternalView (int whichExternalView, int externalViewDistance)
{
	SpecialViewFlag = TRUE;
	SpecialViewType = EXTERNAL_VIEW;

	assert(whichExternalView >= PLUS_X
			&& whichExternalView <= MINUS_Z);
	
	WhichExternalView = whichExternalView;
	ExternalViewDistance = externalViewDistance;
}	





void SetUpCirclingView (int whichCircularView, 
					int whichWayRoundCirclingView, int circleViewRadius,
					int circleViewAngle, int circleViewAngleIncrement)
{
		// typical use of assert: argument verification
		// i.e. not trusting application-caller functions
	assert(whichCircularView == X_Y_PLANE
			|| whichCircularView == X_Z_PLANE
			|| whichCircularView == Y_Z_PLANE);
			
	assert(whichWayRoundCirclingView == CLOCKWISE
			|| whichWayRoundCirclingView == ANTICLOCKWISE); 
			
	SpecialViewFlag = TRUE;
	SpecialViewType = CIRCLING_VIEW;	

	WhichCircularView = whichCircularView;				
	WhichWayRoundCirclingView = whichWayRoundCirclingView;	   
	CircleViewRadius = circleViewRadius;
	CircleViewAngle = circleViewAngle;
	CircleViewAngleIncrement = circleViewAngleIncrement;
}	


	
	
void SetUpFlybyView (int pathDistance, int pathAngle, 
						int flybyCycleTime)
{
   	SpecialViewFlag = TRUE;
	SpecialViewType = FLYBY_VIEW;	

	PathDistance = pathDistance;				
	PathAngle = pathAngle;	   
	FlybyCycleTime = flybyCycleTime;
	NumberFlybyFramesSoFar = 0;
}	




void SetUpLocalView (int whichLocalView, int localViewDistance)
{
	SpecialViewFlag = TRUE;
	SpecialViewType = LOCAL_VIEW;

	assert(whichLocalView >= PLUS_X
			&& whichLocalView <= MINUS_Z);

	WhichLocalView = whichLocalView;
	LocalViewDistance = localViewDistance;
}



	
	// no settings here, but easy to make director view editable
	// function below sets up the view with its standard preferences
void SetUpDirectorsView (void)
{
	SpecialViewFlag = TRUE;
	SpecialViewType = DIRECTORS_VIEW;

		// set the parameters that do not change across
		// a director view session

		// choose start point on sequence randomly
	SequencePosition = rand() % 3;
 
	SingleViewCycleTime = 30;
	DirectorFrameCounter = 0;
   
		// HERE: set the directors preferences
	DirectorLocalView = MINUS_Z;		// looking forward
	DirectorLocalViewDistance;

	DirectorWhichCircularView = X_Z_PLANE;
	DirectorWhichWayRoundCirclingView = ANTICLOCKWISE;	  
	DirectorCircleViewRadius = 5000;	
	DirectorCircleViewAngleIncrement = 24;

	DirectorPathDistance = 2000;
	DirectorPathAngle = 0;		// from the side
}







void DealWithSpecialView (void)
{
	assert(SpecialViewFlag == TRUE);

	switch (SpecialViewType)
		{
		case EXTERNAL_VIEW:
			DealWithExternalView();
			break;
		case CIRCLING_VIEW:	
			DealWithCirclingView();
			break;
		case FLYBY_VIEW:		 	
		   	DealWithFlybyView();
		   	break;
		case DIRECTORS_VIEW:	
			DealWithDirectorsView();
			break;
		case LOCAL_VIEW:
			DealWithLocalView();
			break;
		default:
			assert(FALSE);			// should never get here
		}
}




void DealWithExternalView (void)
{
	ObjectHandler* baseObject;
	VECTOR displacement;

	displacement.vx = 0;
	displacement.vy = 0;
	displacement.vz = 0;

		// find where we are relative to viewed object
	switch(WhichExternalView)
		{
		case PLUS_X:
			displacement.vx = ExternalViewDistance;
			break;
		case MINUS_X:
			displacement.vx = -ExternalViewDistance;
			break;
		case PLUS_Y:
			displacement.vy = ExternalViewDistance;
			break;
		case MINUS_Y:
			displacement.vy = -ExternalViewDistance;
			break;
		case PLUS_Z:
			displacement.vz = ExternalViewDistance;
			break;
		case MINUS_Z:
			displacement.vz = -ExternalViewDistance;
			break;
		default:
			assert(FALSE);
		}

		// find which object we are viewing
	assert(SelectedObject >= 0);
	assert(SelectedObject < MAX_OBJECTS);
	baseObject = ObjectArray[SelectedObject];
	TheMainView.super = &(baseObject->coord);

		// set view reference point to centre of object
	TheMainView.vrx = 0;
	TheMainView.vry = 0;
	TheMainView.vrz = 0;	

	TheMainView.vpx = displacement.vx;
	TheMainView.vpy = displacement.vy;
	TheMainView.vpz = displacement.vz;
	   
		// register change in view
	ViewChanged = TRUE;





#if 0 		// OLD : uses WORLD as view super
			// NOTE: it DOES work, but two disadvantages:
			// (i) cannot fix ANGLE of external view onto object
			// ie when looking from PLUS_X, always looking from same direction,
			// but object may by at any xy angle
			// ALSO: must use RotMatrix to find realDisplacement given
			// object's rotation angle
			// IF object was in hierarchical coordinate system, this would have to be
			// iterative ==> BETTER TO SET view.super to OBJECT.coord

	MATRIX tempMatrix;
	VECTOR displacement;		// from object, relative to object
	VECTOR realDisplacement;	// from object, relative to world
	ObjectHandler* baseObject;

	assert(TheMainView.super == WORLD);		// free-roaming view

		// displacement relative to viewed object
	displacement.vx = 0;
	displacement.vy = 0;
	displacement.vz = 0;

		// find where we are relative to viewed object
	switch(WhichExternalView)
		{
		case PLUS_X:
			displacement.vx = ExternalViewDistance;
			break;
		case MINUS_X:
			displacement.vx = -ExternalViewDistance;
			break;
		case PLUS_Y:
			displacement.vy = ExternalViewDistance;
			break;
		case MINUS_Y:
			displacement.vy = -ExternalViewDistance;
			break;
		case PLUS_Z:
			displacement.vz = ExternalViewDistance;
			break;
		case MINUS_Z:
			displacement.vz = -ExternalViewDistance;
			break;
		default:
			assert(FALSE);
		}

		// find which object we are viewing
	assert(SelectedObject >= 0);
	assert(SelectedObject < MAX_OBJECTS);
	baseObject = ObjectArray[SelectedObject];
	///////TheMainView.super = &(baseObject.coord);

		// set view reference point to centre of object
	TheMainView.vrx = baseObject->coord.coord.t[0];
	TheMainView.vry = baseObject->coord.coord.t[1];
	TheMainView.vrz = baseObject->coord.coord.t[2];
			
		// find rotation matrix from rotation angle of object
		// i.e. matrix expressing object angle w.r.t. super
	RotMatrix(&baseObject->rotate, &tempMatrix);

		// find position from object in terms of super coords
	ApplyMatrixLV(&tempMatrix, &displacement, &realDisplacement);

		// hence position viewpoint 
	TheMainView.vpx = TheMainView.vrx + realDisplacement.vx;
	TheMainView.vpy = TheMainView.vry + realDisplacement.vy;
	TheMainView.vpz = TheMainView.vrz + realDisplacement.vz;

		// register change in view
	ViewChanged = TRUE;
#endif 		// OLD
}






void DealWithCirclingView (void)
{
	VECTOR displacement;		// from object, relative to object
	ObjectHandler* baseObject;

		// displacement relative to viewed object
	displacement.vx = 0;
	displacement.vy = 0;
	displacement.vz = 0;

		// find where we are relative to viewed object
		// start on semi-arbitrary point on circle
	switch(WhichCircularView)
		{
		case X_Y_PLANE:		// use standard Cartesian theta
			displacement.vx = (CircleViewRadius * rcos(CircleViewAngle)) >> 12;
			displacement.vy = (CircleViewRadius * rsin(CircleViewAngle)) >> 12;
			break;
		case X_Z_PLANE:		// use theta again: pretend z axis is y axis
			displacement.vx = (CircleViewRadius * rcos(CircleViewAngle)) >> 12;
			displacement.vz = (CircleViewRadius * rsin(CircleViewAngle)) >> 12;
			break;
		case Y_Z_PLANE:		// use theta again: pretend z axis is x axis
			displacement.vz = (CircleViewRadius * rcos(CircleViewAngle)) >> 12;
			displacement.vy = (CircleViewRadius * rsin(CircleViewAngle)) >> 12;				
			break;
		default:
			assert(FALSE);			// should never get here
		}

		// circling: update angle
	switch(WhichWayRoundCirclingView)
		{
		case CLOCKWISE:
			CircleViewAngle -= CircleViewAngleIncrement;
			break;
		case ANTICLOCKWISE:
			CircleViewAngle += CircleViewAngleIncrement;
			break;
		default:
			assert(FALSE);			// should never get here
		}

		// find which object we are viewing
	assert(SelectedObject >= 0);
	assert(SelectedObject < MAX_OBJECTS);
	baseObject = ObjectArray[SelectedObject];
	TheMainView.super = &(baseObject->coord);

		// set view reference point to centre of object
	TheMainView.vrx = 0;
	TheMainView.vry = 0;
	TheMainView.vrz = 0;

		// hence position viewpoint 
	TheMainView.vpx = displacement.vx;
	TheMainView.vpy = displacement.vy;
	TheMainView.vpz = displacement.vz;

#if 1
	// DEBUG
	if (frameNumber % 5 == 0)
		{
		dumpVECTOR(&displacement);
		}
#endif

		// register change in view
	ViewChanged = TRUE;
}





void DealWithFlybyView (void)
{
	MATRIX tempMatrix;
	VECTOR displacement;			
	ObjectHandler* baseObject;

	assert(FlybyCycleTime > 0);

		// displacement relative to viewed object
	displacement.vx = 0;
	displacement.vy = 0;
	displacement.vz = 0;

		// find which object we are viewing
	assert(SelectedObject >= 0);
	assert(SelectedObject < MAX_OBJECTS);
	baseObject = ObjectArray[SelectedObject];
	TheMainView.super = &(baseObject->coord);

		// what kind of motion does object move in;	linear or circular for now
	TypeOfFlybyView = FindObjectMotionType(baseObject);

	switch(TypeOfFlybyView)
		{
		case LINEAR:
			{
			int objectSpeed;

			objectSpeed = FindObjectSpeed(baseObject);

				// set view reference point to centre of object
				// so that we keep looking toward the object
			TheMainView.vrx = 0;
			TheMainView.vry = 0;
			TheMainView.vrz = 0;

				// position viewing point
				// on parallel path	to object, upwind of object
			displacement.vx = (PathDistance * rcos(PathAngle)) >> 12;
			displacement.vy = (PathDistance * rsin(PathAngle)) >> 12;
			displacement.vz = -((objectSpeed * FlybyCycleTime) / 2)
							+ (objectSpeed * NumberFlybyFramesSoFar);

				// hence position viewpoint 
			TheMainView.vpx = displacement.vx;
			TheMainView.vpy = displacement.vy;
			TheMainView.vpz = displacement.vz;  
			}
			break;
		case CIRCULAR:
			#if 0			// no point to circular flyby view
			{
			ObjectHandler* centralObject;		// object that baseObject orbits
			int viewDistanceFromCentralBody;
			int oldPlaneX, oldPlaneY;
			int newPlaneX, newPlaneY;
			int displacementX, displacementY;

				// set view reference point to centre of object
				// so that we keep looking toward the object
			TheMainView.vrx = 0;
			TheMainView.vry = 0;
			TheMainView.vrz = 0;

			assert(baseObject->centralBodyID >= 0);
			assert(baseObject->centralBodyID < MAX_OBJECTS);

				// if just started a flyby cycle, position viewing point
				// on tangent removed from orbit of object
				// ie a tangent to orbital circle that has been moved 
				// in the direction of circle radius
			if (NumberFlybyFramesSoFar == 0)
				{
				FlybyAngleMiddle = baseObject->angle 
					+ ((baseObject->angleIncrement * FlybyCycleTime)/2);
				}
	
			centralObject = ObjectArray[baseObject->centralBodyID];
			assert(centralObject != NULL);

			oldPlaneX = (baseObject->radius * rcos(baseObject->angle)) >> 12;
			oldPlaneY = (baseObject->radius * rsin(baseObject->angle)) >> 12;

			viewDistanceFromCentralBody = baseObject->radius + PathDistance;

			newPlaneX = (viewDistanceFromCentralBody * rcos(FlybyAngleMiddle)) >> 12;
			newPlaneY = (viewDistanceFromCentralBody * rsin(FlybyAngleMiddle)) >> 12; 

			displacementX = newPlaneX - oldPlaneX;
			displacementY = newPlaneY - oldPlaneY;

			printf("disp X and Y: %d, %d\n", displacementX, displacementY);

			displacement.vx = (displacementX * baseObject->firstVector.vx)
					+ (displacementY * baseObject->secondVector.vx);
			displacement.vy = (displacementX * baseObject->firstVector.vy)
					+ (displacementY * baseObject->secondVector.vy);
			displacement.vz = (displacementX * baseObject->firstVector.vz)
					+ (displacementY * baseObject->secondVector.vz);
							
				// the vectors are relative to size ONE
				// hence shift-right 12 to make them unit vectors
			TheMainView.vpx = displacement.vx >> 12;
			TheMainView.vpy = displacement.vy >> 12;
			TheMainView.vpz = displacement.vz >> 12;
			}
			#endif 			// no point to circular flyby view
			break;
		default:
			assert(FALSE);
		}
						
	NumberFlybyFramesSoFar++;

		// have we reached end of one cycle?
	if (NumberFlybyFramesSoFar >= FlybyCycleTime)
		NumberFlybyFramesSoFar = 0;

		// register change in view
	ViewChanged = TRUE;
}




	

void DealWithDirectorsView (void)
{
	int currentSubMode;		// local, circling or flyby

	assert(SequencePosition >= 0 && SequencePosition <= 2);

	currentSubMode = DirectorSequence[SequencePosition];

	switch(currentSubMode)
		{
		case LOCAL_VIEW:
				// save old values
			OldWhichLocalView = WhichLocalView;
			OldLocalViewDistance = LocalViewDistance;
				// impose director's values
			WhichLocalView = DirectorLocalView;
			LocalViewDistance = DirectorLocalViewDistance;	
				// sort local view			
			DealWithLocalView();
				// restore old values
			WhichLocalView = OldWhichLocalView;
			LocalViewDistance = OldLocalViewDistance;
			break;
		case CIRCLING_VIEW:
				// save old values
			OldWhichCircularView = WhichCircularView;
			OldWhichWayRoundCirclingView = WhichWayRoundCirclingView;
			OldCircleViewRadius = CircleViewRadius;
			OldCircleViewAngleIncrement = CircleViewAngleIncrement;
				// impose director's values
			WhichCircularView = DirectorWhichCircularView;
			WhichWayRoundCirclingView = DirectorWhichWayRoundCirclingView;
			CircleViewRadius = DirectorCircleViewRadius;
			CircleViewAngleIncrement = DirectorCircleViewAngleIncrement;	
				// sort local view			
			DealWithCirclingView();
				// restore old values
			WhichCircularView = OldWhichCircularView;
			WhichWayRoundCirclingView = OldWhichWayRoundCirclingView;
			CircleViewRadius = OldCircleViewRadius;
			CircleViewAngleIncrement = OldCircleViewAngleIncrement;
			break;
		case FLYBY_VIEW:
				// save old values
			OldPathDistance = PathDistance;
			OldPathAngle = PathAngle;
			OldFlybyCycleTime = FlybyCycleTime;		
				// impose director's values
			PathDistance = DirectorPathDistance;
			PathAngle = DirectorPathAngle;
			FlybyCycleTime = SingleViewCycleTime;			
				// sort local view			
			DealWithFlybyView();			   
				// restore old values
			PathDistance = OldPathDistance;
			PathAngle = OldPathAngle;
			FlybyCycleTime = OldFlybyCycleTime;		
			break;
		default:					// should never get here
			assert(FALSE);
		}

	DirectorFrameCounter++;
	
		// update status variables of director view:
		// which view are we about to see next
	if (DirectorFrameCounter % SingleViewCycleTime == 0)		// time for next
		{
		if (SequencePosition == 2)		// end of this sequence
			{
			SequencePosition = 0;
			}
		else
			SequencePosition++;	  // next view in this sequence

		NumberFlybyFramesSoFar = 0;

			// toggles: which way to circle, flyby from left or right
		if (DirectorWhichWayRoundCirclingView == CLOCKWISE)
			DirectorWhichWayRoundCirclingView = ANTICLOCKWISE;
		else
			DirectorWhichWayRoundCirclingView = CLOCKWISE;
		if (DirectorPathAngle == 0)
			DirectorPathAngle = 2048;
		else
			DirectorPathAngle = 0;
		DirectorFrameCounter = 0;
		}	
} 




void DealWithLocalView (void)
{
	ObjectHandler* baseObject;
	VECTOR displacement;

	displacement.vx = 0;
	displacement.vy = 0;
	displacement.vz = 0;

		// find where we are relative to object
	switch(WhichLocalView)
		{
		case PLUS_X:
			displacement.vx = LocalViewDistance;
			break;
		case MINUS_X:
			displacement.vx = -LocalViewDistance;
			break;
		case PLUS_Y:
			displacement.vy = LocalViewDistance;
			break;
		case MINUS_Y:
			displacement.vy = -LocalViewDistance;
			break;
		case PLUS_Z:
			displacement.vz = LocalViewDistance;
			break;
		case MINUS_Z:
			displacement.vz = -LocalViewDistance;
			break;
		default:
			assert(FALSE);
		}

		// find which object we are viewing	from
	assert(SelectedObject >= 0);
	assert(SelectedObject < MAX_OBJECTS);
	baseObject = ObjectArray[SelectedObject];
	TheMainView.super = &(baseObject->coord);

		// set view reference point to centre of object
	TheMainView.vpx = displacement.vx/20;
	TheMainView.vpy = displacement.vy/20;
	TheMainView.vpz = displacement.vz/20;	

	TheMainView.vrx = displacement.vx;
	TheMainView.vry = displacement.vy;
	TheMainView.vrz = displacement.vz;
	   
		// register change in view
	ViewChanged = TRUE;
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






void TurnAllLightCalculationsOff (void)
{
	int i;

	for (i = 0; i < MAX_OBJECTS; i++)	
		{
		if (ObjectArray[i] != NULL)
			{
			if (ObjectArray[i]->alive == TRUE)
				{
				TURN_NTH_BIT_OFF( ObjectArray[i]->handler.attribute, 32, 6);
				}
			}
		}
}











void ResetAll (void)
{
	InitialiseObjects();
	InitialiseLighting();
 
		// to start with, player moves around
	SelectionMode = MOVE_SHIP;	 
	framesSinceLastSwitch = 0;
	ResetSelectionData();

	SelectedObject = 0;
	framesSinceLastSelection = 0;

	InitialiseView();
	ViewChanged = TRUE;
}











