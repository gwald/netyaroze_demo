/************************************************************
 *															*
 *						main.c								*
 *						3-D object manipulation				*
 *			   											    *															*
 *				LPGE     05/11/96							*		
 *															*
 *	Copyright (C) 1996 Sony Computer Entertainment Inc.		*
 *	All Rights Reserved										*
 *															*
 ***********************************************************/



	  




/************************************************************************
*																		*
*			libraries	  												*
*																		*
************************************************************************/
#include <libps.h>

#include "asssert.h"
#include "pad.h"
#include "tim.h"
#include "tmd.h"
#include "sincos.h"

		


	
	

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
#define MODEL_ADDRESS	(0x800c0000)



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

			
	// the axes' ends: used by the external views
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




	// arbitrary flags for movement patterns
#define FIRST_SIMPLE_PATH 1



typedef struct
{
	int id;
	int alive;		 // boolean
	int type;		 // SUN, PLANET, MOON
	int which;		// index into circular list
	GsDOBJ2	handler;
	double scaleX, scaleY, scaleZ;			// scaling in 3D: volume control
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
	int specialMovement;		// flag to indicate arbitrary movement patterns		
} ObjectHandler;

#define MAX_OBJECTS		64

#define MAX_PLANETS 3
#define MAX_MOONS 3

	// the spheres: sun, moon, planets
ObjectHandler ObjectArray[MAX_OBJECTS];

int NumberOfObjects;


	// ship that player moves around in; view is local camera on ship
ObjectHandler PlayersShuttle;



int ShipID;






	

	// player's movement and rotation speeds
int MovementSpeed;
int RotationSpeed;

	// initial values of player's movement and rotation speeds 
#define INITIAL_MOVEMENT_SPEED 150
#define INITIAL_ROTATION_SPEED 6



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

	// director's cut
int WhichDirectorSequence;	// 0 or 1: index into array DirectorSequences 
int SequencePosition;		// index into sub-array: 0 to 2
int SingleViewCycleTime;		// in frames
int DirectorFrameCounter;

	// local view
VECTOR LocalAngle;
int ViewingPointDistance;
int ReferencePointDistance;

int DirectorLocalView;
int DirectorLocalViewDistance;
int DirectorWhichCircularView;
int DirectorWhichWayRoundCirclingView;	  
int DirectorCircleViewRadius;	
int DirectorCircleViewAngleIncrement;
int DirectorPathDistance;
int DirectorPathAngle;

int DirectorSequences[2][3] = { {LOCAL_VIEW, CIRCLING_VIEW, FLYBY_VIEW},
								{LOCAL_VIEW, FLYBY_VIEW, CIRCLING_VIEW} };


									 


   	// pad interface buffer	  
u_long PadStatus;	



	// simplest frame counter
u_long frameNumber = 0;	


#define NORMAL_TIME_DELAY 7			// in frames
#define LONG_TIME_DELAY 15




	







/************************************************************************
*																		*
*			prototypes	  												*
*																		*
************************************************************************/

int main(void);


	// text printing
void GetSelectionModeString (char *string);
void PrintModeInfo (void);
void WriteObjectSelectionModeInfoIntoString (char* string);
void WriteObjectSelectionInfo (void);

void WriteObjectMoveInfo (void);
void WriteObjectSpinInfo (void);
void WriteObjectVolumeInfo (void);
void WriteChangeDisplayInfo (void);
 

	// controller pad handling
int	DealWithController(void);

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


	// object handling
int FindNextLowestObjectID (int objectID);
int FindNextHighestObjectID (int objectID);
int FindNextUnusedObjectID (void);

void HandleAllObjects (void);
void DealWithSpatialMovement (void);
void HandleSpecialMovement (ObjectHandler* object);


void InitialiseAll(void);


void InitialiseView(void);
void SetNormalView (void);
void SetUpSpecialView (void);
void SetUpExternalView (int whichExternalView, int externalViewDistance);
void SetUpCirclingView (int whichCircularView, 
					int whichWayRoundCirclingView, int circleViewRadius,
					int circleViewAngle, int circleViewAngleIncrement);
void SetUpFlybyView (int pathDistance, int pathAngle, 
						int flybyCycleTime);
void SetUpLocalView (VECTOR *localAngle, int viewingPointDistance,
								int referencePointDistance);
void SetUpDirectorsView (void);



void DealWithSpecialView (void);

void DealWithStandardView (void);
void DealWithCirclingView (void);
void DealWithFlybyView (void);
void DealWithDirectorsView (void);
void DealWithLocalView (void);





void InitialiseLighting(void);
void InitialiseObjects(void);
void InitSingleObject (ObjectHandler* object);

	// basic visual highlighting
void HighlightObject (ObjectHandler* object);
void UnHighlightObject (ObjectHandler* object);



void InitialiseModels(void);

void SetUpSolarSystem (void);

void ArrangeObjectsIntoLine (void);

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
int FindObjectSpeed (ObjectHandler* object);
int FindObjectMotionType (ObjectHandler* object);


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





/************************************************************************
*																		*
*			macros		  												*
*																		*
************************************************************************/



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







/************************************************************************
*																		*
*			functions	  												*
*																		*
************************************************************************/



int main (void)
{
	int	outputBufferIndex;
	MATRIX tempMatrix;
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
		FntPrint("# of models in TMD file: %d\n", NumberOfObjects);
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
			sprintf(string, "%s\n", "Moving objects");
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





	// print text onto the screen;
	// what we print depends on user's selection mode
void PrintModeInfo (void)
{
	switch(SelectionMode)
		{
		case MOVE_SHIP:
			if (StringCounter == SelectedOption)
				FntPrint(SelectionString);
			FntPrint("first obj pos: %d %d %d\n",
				PlayersShuttle.coord.coord.t[0],
				PlayersShuttle.coord.coord.t[1],
				PlayersShuttle.coord.coord.t[2]);
			FntPrint("first obj ang: %d %d %d\n",
				PlayersShuttle.rotate.vx,
				PlayersShuttle.rotate.vy,
				PlayersShuttle.rotate.vz);
			if (StringCounter == SelectedOption)
				FntPrint(DeSelectionString);
			StringCounter++;
			FntPrint("use start to use colour menu\n");
			break;
		case CHANGE_LIGHTS:
			if (StringCounter == SelectedOption)
				FntPrint(SelectionString);
			FntPrint("Selected light: %d\n", SelectedLight);
			if (StringCounter == SelectedOption)
				FntPrint(DeSelectionString);
			StringCounter++;
			assert(SelectedLight >= 0 && SelectedLight <= 2);
			if (StringCounter == SelectedOption)
				FntPrint(SelectionString);
			FntPrint("Direction: %d %d %d\n",
					lightSourceArray[SelectedLight].vx,
					lightSourceArray[SelectedLight].vy,
					lightSourceArray[SelectedLight].vz);
			if (StringCounter == SelectedOption)
				FntPrint(DeSelectionString);
			StringCounter++;


			if (StringCounter == SelectedOption)
				FntPrint(SelectionString);
			FntPrint("Colour: r %d, g %d, b %d\n",
					lightSourceArray[SelectedLight].r,
					lightSourceArray[SelectedLight].g,
					lightSourceArray[SelectedLight].b);
			if (StringCounter == SelectedOption)
				FntPrint(DeSelectionString);
			StringCounter++;

			break;
		case SET_AMBIENT_LIGHT:
			FntPrint("Ambient light colours:-\n");		// never highlight: not a variable

			if (StringCounter == SelectedOption)
				FntPrint(SelectionString);
			FntPrint("Red: %d\n", AmbientRed);
			if (StringCounter == SelectedOption)
				FntPrint(DeSelectionString);
			StringCounter++;


			if (StringCounter == SelectedOption)
				FntPrint(SelectionString);
			FntPrint("Green: %d\n", AmbientGreen);
			if (StringCounter == SelectedOption)
				FntPrint(DeSelectionString);
			StringCounter++;

			if (StringCounter == SelectedOption)
				FntPrint(SelectionString);
			FntPrint("Blue: %d\n", AmbientBlue);
			if (StringCounter == SelectedOption)
				FntPrint(DeSelectionString);
			StringCounter++;

			break;
		case FOGGING:
			if (StringCounter == SelectedOption)
				FntPrint(SelectionString);
			FntPrint("dqa %d\n", FoggingData.dqa);
			if (StringCounter == SelectedOption)
				FntPrint(DeSelectionString);
			StringCounter++;

			if (StringCounter == SelectedOption)
				FntPrint(SelectionString);
			FntPrint("dqb %d\n", FoggingData.dqb);
			if (StringCounter == SelectedOption)
				FntPrint(DeSelectionString);
			StringCounter++;

			if (StringCounter == SelectedOption)
				FntPrint(SelectionString);
			FntPrint("Fog colours: \nr %d g %d b %d\n",
				FoggingData.rfc, FoggingData.gfc, FoggingData.bfc);
			if (StringCounter == SelectedOption)
				FntPrint(DeSelectionString);
			StringCounter++;

			if (StringCounter == SelectedOption)
				FntPrint(SelectionString);
			FntPrint("Lighting mode: %d\n",
					OverallLightingMode);
			if (OverallLightingMode == 0)
				FntPrint("I.e. fogging is OFF\n");
			else
				{
				assert(OverallLightingMode == 1);
				FntPrint("I.e. fogging is ON\n");
				}
			if (StringCounter == SelectedOption)
				FntPrint(DeSelectionString);
			StringCounter++;
			break;
		case VIEW_DISTORTION:
			switch(ViewMode)
				{
				case GENERAL_DISTORTION:
					if (StringCounter == SelectedOption)
						FntPrint(SelectionString);
					FntPrint("view mode :-\nGeneral\n");
					if (StringCounter == SelectedOption)
						FntPrint(DeSelectionString);
					StringCounter++;

					if (StringCounter == SelectedOption)
						FntPrint(SelectionString);
					FntPrint("rz: %d\n", TheMainView.rz);
					if (StringCounter == SelectedOption)
						FntPrint(DeSelectionString);
					StringCounter++;
					
					if (StringCounter == SelectedOption)
						FntPrint(SelectionString);
					FntPrint("Projection: %d\n", ProjectionDistance);
					if (StringCounter == SelectedOption)
						FntPrint(DeSelectionString);
					StringCounter++;
					break;
				case MOVE_BOTH_ABSOLUTELY:
					if (StringCounter == SelectedOption)
						FntPrint(SelectionString);
					FntPrint("view mode :-\nMove both absolutely\n");
					if (StringCounter == SelectedOption)
						FntPrint(DeSelectionString);
					StringCounter++;
					
					if (StringCounter == SelectedOption)
						FntPrint(SelectionString);
					FntPrint("Reference point :-\n%d %d %d\n",
						TheMainView.vrx, TheMainView.vry, TheMainView.vrz);
					if (StringCounter == SelectedOption)
						FntPrint(DeSelectionString);
					StringCounter++;
					
					if (StringCounter == SelectedOption)
						FntPrint(SelectionString);
					FntPrint("Viewing point :-\n %d %d %d\n",
						TheMainView.vpx, TheMainView.vpy, TheMainView.vpz);
					if (StringCounter == SelectedOption)
						FntPrint(DeSelectionString);
					StringCounter++;
					break;
				case MOVE_REFERENCE:
					if (StringCounter == SelectedOption)
						FntPrint(SelectionString);
					FntPrint("view mode :-\nMove reference\n");
					if (StringCounter == SelectedOption)
						FntPrint(DeSelectionString);
					StringCounter++;
					
					if (StringCounter == SelectedOption)
						FntPrint(SelectionString);
					FntPrint("Reference point :-\n%d %d %d\n",
						TheMainView.vrx, TheMainView.vry, TheMainView.vrz);
					if (StringCounter == SelectedOption)
						FntPrint(DeSelectionString);
					StringCounter++;
				 
					break;
				case MOVE_VIEWING_POINT:
					if (StringCounter == SelectedOption)
						FntPrint(SelectionString);
					FntPrint("view mode :-\nMove viewing point\n");
					if (StringCounter == SelectedOption)
						FntPrint(DeSelectionString);
					StringCounter++;

					if (StringCounter == SelectedOption)
						FntPrint(SelectionString);
					FntPrint("Viewing point :-\n %d %d %d\n",
						TheMainView.vpx, TheMainView.vpy, TheMainView.vpz);
					if (StringCounter == SelectedOption)
						FntPrint(DeSelectionString);
					StringCounter++;
					
					break;
				case CHANGE_SUPER:
				   	if (StringCounter == SelectedOption)
						FntPrint(SelectionString);
					FntPrint("view mode :-\nChange super\n");
					if (StringCounter == SelectedOption)
						FntPrint(DeSelectionString);
					StringCounter++;				
					
					if (StringCounter == SelectedOption)
						FntPrint(SelectionString);
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
					if (StringCounter == SelectedOption)
						FntPrint(DeSelectionString);
					StringCounter++;

					break;
				default:				// should never get here
					assert(FALSE);
				}
			break;	
		case SELECT_OBJECT:					
				// sort printing of sub-mode
			WriteObjectSelectionModeInfoIntoString(objectModeString);
			if (StringCounter == SelectedOption)
				FntPrint(SelectionString);
			FntPrint("submode: %s", objectModeString);
			if (StringCounter == SelectedOption)
				FntPrint(DeSelectionString);
			StringCounter++;

				// say which object is selected
			if (StringCounter == SelectedOption)
				FntPrint(SelectionString);
			FntPrint("Selected object id: %d\n", SelectedObject);
			if (StringCounter == SelectedOption)
				FntPrint(DeSelectionString);
			StringCounter++;

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
					if (StringCounter == SelectedOption)
						FntPrint(SelectionString);
					FntPrint("no special view\n");
					if (StringCounter == SelectedOption)
						FntPrint(DeSelectionString);
					StringCounter++;
					break;
				case EXTERNAL_VIEW:
					if (StringCounter == SelectedOption)
						FntPrint(SelectionString);
					FntPrint("external view\n");
					if (StringCounter == SelectedOption)
						FntPrint(DeSelectionString);
					StringCounter++;

					if (StringCounter == SelectedOption)
						FntPrint(SelectionString);
					switch(WhichExternalView)
						{
						case PLUS_X:
							FntPrint("From right\n");
			   				break;
						case MINUS_X:
							FntPrint("From left\n");
							break;
						case PLUS_Y:
							FntPrint("From below\n");
							break;
						case MINUS_Y:
							FntPrint("From above\n");
							break;
						case PLUS_Z:
				 			FntPrint("From front\n");
							break;
						case MINUS_Z:
							FntPrint("From back\n");
							break;
						default:				   // should never get here
							assert(FALSE);
						}
					if (StringCounter == SelectedOption)
						FntPrint(DeSelectionString);
					StringCounter++;

					if (StringCounter == SelectedOption)
						FntPrint(SelectionString);
					FntPrint("external distance: %d\n", ExternalViewDistance);
					if (StringCounter == SelectedOption)
						FntPrint(DeSelectionString);
					StringCounter++;
					break;
				case CIRCLING_VIEW:	
					if (StringCounter == SelectedOption)
						FntPrint(SelectionString);
					FntPrint("Circling view\n");
					if (StringCounter == SelectedOption)
						FntPrint(DeSelectionString);
					StringCounter++;

					if (StringCounter == SelectedOption)
						FntPrint(SelectionString);
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
					if (StringCounter == SelectedOption)
						FntPrint(DeSelectionString);
					StringCounter++;


					if (StringCounter == SelectedOption)
						FntPrint(SelectionString);
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
					if (StringCounter == SelectedOption)
						FntPrint(DeSelectionString);
					StringCounter++;


					if (StringCounter == SelectedOption)
						FntPrint(SelectionString);
					FntPrint("orbit radius: %d\n", CircleViewRadius);
					if (StringCounter == SelectedOption)
						FntPrint(DeSelectionString);
					StringCounter++;


					FntPrint("orbit angle: %d\n", CircleViewAngle);


					if (StringCounter == SelectedOption)
						FntPrint(SelectionString);
					FntPrint("angular speed: %d\n", CircleViewAngleIncrement);
					if (StringCounter == SelectedOption)
						FntPrint(DeSelectionString);
					StringCounter++;
					break;
				case FLYBY_VIEW:	
					if (StringCounter == SelectedOption)
						FntPrint(SelectionString);
					FntPrint("Fly-by view\n");
					if (StringCounter == SelectedOption)
						FntPrint(DeSelectionString);
					StringCounter++;

					switch(TypeOfFlybyView)
						{
						case CIRCULAR:
							FntPrint("object is in orbit\n");
							break;
						case LINEAR:
							FntPrint("object is not in orbit\n");
							break;
						default:
							assert(FALSE);
						}
					
					if (StringCounter == SelectedOption)
						FntPrint(SelectionString);
					FntPrint("path distance: %d\n", PathDistance);
					if (StringCounter == SelectedOption)
						FntPrint(DeSelectionString);
					StringCounter++;


					if (StringCounter == SelectedOption)
						FntPrint(SelectionString);
					FntPrint("path angle: %d\n", PathAngle);
					if (StringCounter == SelectedOption)
						FntPrint(DeSelectionString);
					StringCounter++;


					if (StringCounter == SelectedOption)
						FntPrint(SelectionString);
					FntPrint("cycle time: %d\n", FlybyCycleTime);
					if (StringCounter == SelectedOption)
						FntPrint(DeSelectionString);
					StringCounter++;
					break;
				case DIRECTORS_VIEW:	
					if (StringCounter == SelectedOption)
						FntPrint(SelectionString);
					FntPrint("Director's cut\n");
					if (StringCounter == SelectedOption)
						FntPrint(DeSelectionString);
					StringCounter++;
					break;
				case LOCAL_VIEW:
					if (StringCounter == SelectedOption)
						FntPrint(SelectionString);
					FntPrint("local view\n");
					if (StringCounter == SelectedOption)
						FntPrint(DeSelectionString);
					StringCounter++;


					if (StringCounter == SelectedOption)
						FntPrint(SelectionString);
					FntPrint("Local angle :-\n%d %d %d\n", 
						LocalAngle.vx, LocalAngle.vy, LocalAngle.vz);
					if (StringCounter == SelectedOption)
						FntPrint(DeSelectionString);
					StringCounter++;


					FntPrint("Distances from object to\n");


					if (StringCounter == SelectedOption)
						FntPrint(SelectionString);
					FntPrint("viewing point: %d\n", ViewingPointDistance);
					if (StringCounter == SelectedOption)
						FntPrint(DeSelectionString);
					StringCounter++;


					if (StringCounter == SelectedOption)
						FntPrint(SelectionString);
					FntPrint("reference point: %d\n", ReferencePointDistance);		
					if (StringCounter == SelectedOption)
						FntPrint(DeSelectionString);
					StringCounter++;			
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
	switch(ObjectArray[SelectedObject].inOrbit)
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

	if (ObjectArray[SelectedObject].inOrbit == TRUE)
		{
		if (StringCounter == SelectedOption)
			FntPrint(SelectionString);
		FntPrint("orbital radius: %d\n",
			ObjectArray[SelectedObject].radius);
		if (StringCounter == SelectedOption)
			FntPrint(DeSelectionString);
		StringCounter++;
			   

		if (StringCounter == SelectedOption)
			FntPrint(SelectionString);
		switch(ObjectArray[SelectedObject].whichWay)
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
		if (StringCounter == SelectedOption)
			FntPrint(DeSelectionString);
		StringCounter++;
		}	
	else
		{
		if (StringCounter == SelectedOption)
			FntPrint(SelectionString);
		FntPrint("position: %d %d %d\n",
			ObjectArray[SelectedObject].position.vx,
			ObjectArray[SelectedObject].position.vy,
			ObjectArray[SelectedObject].position.vz);
		if (StringCounter == SelectedOption)
			FntPrint(DeSelectionString);
		StringCounter++;


		if (StringCounter == SelectedOption)
			FntPrint(SelectionString);
		FntPrint("momentum: %d\n",
			ObjectArray[SelectedObject].movementMomentumFlag);
		if (StringCounter == SelectedOption)
			FntPrint(DeSelectionString);
		StringCounter++;
		}
}




void WriteObjectSpinInfo (void)
{
	if (StringCounter == SelectedOption)
		FntPrint(SelectionString);
	FntPrint("Angle: %d %d %d\n",
		ObjectArray[SelectedObject].rotate.vx,
		ObjectArray[SelectedObject].rotate.vy,
		ObjectArray[SelectedObject].rotate.vz);
	if (StringCounter == SelectedOption)
		FntPrint(DeSelectionString);
	StringCounter++;
			
	if (StringCounter == SelectedOption)
		FntPrint(SelectionString);
	FntPrint("Rotational momentum: %d\n",
		ObjectArray[SelectedObject].rotationMomentumFlag);
	if (StringCounter == SelectedOption)
		FntPrint(DeSelectionString);
	StringCounter++;
}





char tempString[52];

void WriteObjectVolumeInfo (void)
{
	sprintf2(tempString, "scale X: %f\n", 
		ObjectArray[SelectedObject].scaleX);
	if (StringCounter == SelectedOption)
		FntPrint(SelectionString);
	FntPrint(tempString);
	if (StringCounter == SelectedOption)
		FntPrint(DeSelectionString);
	StringCounter++;


	sprintf2(tempString, "scale Y: %f\n", 
		ObjectArray[SelectedObject].scaleY);
	if (StringCounter == SelectedOption)
		FntPrint(SelectionString);
	FntPrint(tempString);
	if (StringCounter == SelectedOption)
		FntPrint(DeSelectionString);
	StringCounter++;


	sprintf2(tempString, "scale Z: %f\n", 
		ObjectArray[SelectedObject].scaleZ);
	if (StringCounter == SelectedOption)
		FntPrint(SelectionString);
	FntPrint(tempString);
	if (StringCounter == SelectedOption)
		FntPrint(DeSelectionString);
	StringCounter++;
}




void WriteChangeDisplayInfo (void)
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
	if (StringCounter == SelectedOption)
		FntPrint(SelectionString);
	if (boolean)
		FntPrint("fog only\n");
	else
		FntPrint("normal lighting\n");
	if (StringCounter == SelectedOption)
		FntPrint(DeSelectionString);
	StringCounter++;
			

	boolean = 
		ObjectArray[SelectedObject].handler.attribute & GsLOFF;
	if (StringCounter == SelectedOption)
		FntPrint(SelectionString);
	if (boolean)
		FntPrint("light calculations OFF\n");
	else
		FntPrint("light calculations ON\n"); 
	if (StringCounter == SelectedOption)
		FntPrint(DeSelectionString);
	StringCounter++;


		// calculate current semi-transparency setting
	if (ObjectArray[SelectedObject].handler.attribute & 1<<28)
		semiTransparencySetting++;
	if (ObjectArray[SelectedObject].handler.attribute & 1<<29)
		semiTransparencySetting += 2;

	if (StringCounter == SelectedOption)
		FntPrint(SelectionString);
	FntPrint("semi-trans type: %d\n", 
		semiTransparencySetting);
	if (StringCounter == SelectedOption)
		FntPrint(DeSelectionString);
	StringCounter++;


	boolean = 
		ObjectArray[SelectedObject].handler.attribute & GsALON;
	if (StringCounter == SelectedOption)
		FntPrint(SelectionString);
	if (boolean)
		FntPrint("semi-transparency ON\n");
	else
		FntPrint("semi-transparency OFF\n");
	if (StringCounter == SelectedOption)
		FntPrint(DeSelectionString);
	StringCounter++;


	boolean = 
		ObjectArray[SelectedObject].handler.attribute & GsDOFF;	
	if (StringCounter == SelectedOption)
		FntPrint(SelectionString);
	if (boolean)
		FntPrint("display OFF\n");
	else
		FntPrint("display ON\n"); 
	if (StringCounter == SelectedOption)
		FntPrint(DeSelectionString);
	StringCounter++;
}



			


   

	// select and start together to quit 
	// start alone to pause

int DealWithController (void)
{
	static int resetCounter = 0;
	int currentFrame;

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

		// Rup: select previous option      
	if (PadStatus & PADRup)
		{
		if (framesSinceLastSwitch > NORMAL_TIME_DELAY)
			{
			if (SelectionMode != MOVE_SHIP
				|| PadStatus & PADstart)
				{		
				if (SelectedOption == 0)
					SelectedOption = NumberOfOptions-1;
				else
					SelectedOption--;
				framesSinceLastSwitch = 0;
				}
			}
		}

		// Rdown: select next option
	if (PadStatus & PADRdown)
		{
		if (framesSinceLastSwitch > NORMAL_TIME_DELAY)
			{
			if (SelectionMode != MOVE_SHIP
				|| PadStatus & PADstart)
				{
				if (SelectedOption == NumberOfOptions-1)
					SelectedOption = 0;
				else
					SelectedOption++;
				framesSinceLastSwitch = 0;
				}
			}
		}

	if (SelectedOption == 0)			// i.e. SelectionMode highlighted
		{
			// Rright: get next selection mode
		if (PadStatus & PADRright)
			{
			if (framesSinceLastSwitch > NORMAL_TIME_DELAY)
				{
				if (SelectionMode != MOVE_SHIP
					|| PadStatus & PADstart)
					{
						// unhighlight object if move out of SELECT_OBJECT mode
					if (SelectionMode == SELECT_OBJECT)
						{
						assert(SelectedObject >= 0 && SelectedObject < MAX_OBJECTS);
						UnHighlightObject( &ObjectArray[SelectedObject]);
						}
					if (SelectionMode == HIGHEST_SELECTION_MODE)
						SelectionMode = 0;
					else
						SelectionMode++;
						// highlight object if move into of SELECT_OBJECT mode
					if (SelectionMode == SELECT_OBJECT)
						{
						assert(SelectedObject >= 0 && SelectedObject < MAX_OBJECTS);
						HighlightObject( &ObjectArray[SelectedObject]);
						}
					ResetSelectionData();
					framesSinceLastSwitch = 0;
					}
				}
			}

			// Rleft: get next selection mode
		if (PadStatus & PADRleft)
			{
			if (framesSinceLastSwitch > NORMAL_TIME_DELAY)
				{
				if (SelectionMode != MOVE_SHIP
					|| PadStatus & PADstart)
					{
						// unhighlight object if move out of SELECT_OBJECT mode
					if (SelectionMode == SELECT_OBJECT)
						{
						assert(SelectedObject >= 0 && SelectedObject < MAX_OBJECTS);
						UnHighlightObject( &ObjectArray[SelectedObject]);
						}
					if (SelectionMode == 0)
						SelectionMode = HIGHEST_SELECTION_MODE;
					else
						SelectionMode--;
						// highlight object if move into of SELECT_OBJECT mode
					if (SelectionMode == SELECT_OBJECT)
						{
						assert(SelectedObject >= 0 && SelectedObject < MAX_OBJECTS);
						HighlightObject( &ObjectArray[SelectedObject]);
						}
					ResetSelectionData();
					framesSinceLastSwitch = 0;
					}
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

		// L1 & R1: step frame by frame
	if (PadStatus & PADL1 && PadStatus & PADR1)
		{
		currentFrame = VSync(-1);		// number of V_BLANKS since program began
		while (VSync(-1) < currentFrame + 25)
			;
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
								if (ObjectArray[SelectedObject].inOrbit == TRUE)
									{
									numberOfOptions = 8;
									}
								else
									{
									;			 // do nowt
										// NOT HANDLED YET
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
	int i;

	if (PadStatus & PADselect)
		speedMultiplier = 10;		// fast movement
	else
		speedMultiplier = 1;		// slow movement
			
	if (PadStatus & PADLleft)
		{
		for (i = 0; i < NumberOfObjects; i++)
			ObjectArray[i].velocity.vx -= MovementSpeed * speedMultiplier;
		ViewChanged = TRUE;
		}
	if (PadStatus & PADLright)
		{
		for (i = 0; i < NumberOfObjects; i++)
			ObjectArray[i].velocity.vx += MovementSpeed * speedMultiplier;
		ViewChanged = TRUE;
		}
	if (PadStatus & PADLup)
		{
		for (i = 0; i < NumberOfObjects; i++)
			ObjectArray[i].velocity.vy -= MovementSpeed * speedMultiplier;
		ViewChanged = TRUE;
		}
	if (PadStatus & PADLdown)
		{
		for (i = 0; i < NumberOfObjects; i++)
			ObjectArray[i].velocity.vy += MovementSpeed * speedMultiplier;
		ViewChanged = TRUE;
		}
	if (PadStatus & PADL1)
		{
		for (i = 0; i < NumberOfObjects; i++)
			ObjectArray[i].velocity.vz -= MovementSpeed * speedMultiplier;
		ViewChanged = TRUE;
		}
	if (PadStatus & PADL2)
		{
		for (i = 0; i < NumberOfObjects; i++)
			ObjectArray[i].velocity.vz += MovementSpeed * speedMultiplier;
		ViewChanged = TRUE;
		}

		// this chunk: right directional pad plus R1/R2: 
		// rotate player's cube
	if (PadStatus & PADRleft && ((PadStatus & PADstart) == 0))
		{
		for (i = 0; i < NumberOfObjects; i++)
			ObjectArray[i].twist.vy -= RotationSpeed * speedMultiplier;
	    ViewChanged = TRUE;	 
		}
	if (PadStatus & PADRright && ((PadStatus & PADstart) == 0))
		{
        for (i = 0; i < NumberOfObjects; i++)
			ObjectArray[i].twist.vy += RotationSpeed * speedMultiplier;
        ViewChanged = TRUE;	  
		}
	if (PadStatus & PADRup && ((PadStatus & PADstart) == 0))
		{
		for (i = 0; i < NumberOfObjects; i++)
			ObjectArray[i].twist.vx -= RotationSpeed * speedMultiplier;
		ViewChanged = TRUE;	  
		}
	if (PadStatus & PADRdown && ((PadStatus & PADstart) == 0))
		{
		for (i = 0; i < NumberOfObjects; i++)
			ObjectArray[i].twist.vx += RotationSpeed * speedMultiplier;
		ViewChanged = TRUE;	  
		}
	if (PadStatus & PADR1)
		{
        for (i = 0; i < NumberOfObjects; i++)
			ObjectArray[i].twist.vz -= RotationSpeed * speedMultiplier;
        ViewChanged = TRUE;	  
		}
	if (PadStatus & PADR2)
		{
        for (i = 0; i < NumberOfObjects; i++)
			ObjectArray[i].twist.vz += RotationSpeed * speedMultiplier;
      	ViewChanged = TRUE;	   
		}



#if 0		// OLD
	if (SelectedOption == 1)		// movement of ship
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

		
	if (SelectedOption == 2)		// rotation of ship
		{
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
			FoggingData.dqa -= fogChange;
			FoggingChanged = TRUE;
			}
		if (PadStatus & PADLdown)
			{
			FoggingData.dqa += fogChange;
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

					// only two valid values
				assert(OverallLightingMode == 0
					|| OverallLightingMode == 1);

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
			#if 0			// UNFINISHED
			{
			VECTOR direction;			// of viewing point from reference point

				// four left-pad directional controls:
				// rotate reference point
			if (PadStatus & PADLleft)
				{
				ViewChanged = TRUE;
				}
			if (PadStatus & PADLright)
				{
				ViewChanged = TRUE;
				}
			if (PadStatus & PADLup)
				{
			   	ViewChanged = TRUE;		
				}
			if (PadStatus & PADLdown)
				{
				ViewChanged = TRUE;
				}

				// Rup/Rdown: move forward/back relative to current position
				// i.e. move towards the viewing point
			if (PadStatus & PADRup)
				{
				direction.vx = TheMainView.vpx - TheMainView.vrx;
				direction.vy = TheMainView.vpy - TheMainView.vry;
				direction.vz = TheMainView.vpz - TheMainView.vrz;

				TheMainView.vrx += (viewChange * direction.vx) >> 4;
				TheMainView.vry += (viewChange * direction.vy) >> 4;
				TheMainView.vrz += (viewChange * direction.vz) >> 4;

				ViewChanged = TRUE;
				}
			if (PadStatus & PADRdown)	   // move away
				{
				direction.vx = TheMainView.vpx - TheMainView.vrx;
				direction.vy = TheMainView.vpy - TheMainView.vry;
				direction.vz = TheMainView.vpz - TheMainView.vrz;

				TheMainView.vrx -= (viewChange * direction.vx) >> 4;
				TheMainView.vry -= (viewChange * direction.vy) >> 4;
				TheMainView.vrz -= (viewChange * direction.vz) >> 4;

				ViewChanged = TRUE;
				}
			}
			#endif		// UNFINISHED
			break;
		case MOVE_VIEWING_POINT:
			#if 0		// UNFINISHED
			{
			VECTOR direction;			// of reference point from viewing point

				// four left-pad directional controls:
				// rotate reference point
			if (PadStatus & PADLleft)
				{
				ViewChanged = TRUE;
				}
			if (PadStatus & PADLright)
				{
				ViewChanged = TRUE;
				}
			if (PadStatus & PADLup)
				{
			   	ViewChanged = TRUE;		
				}
			if (PadStatus & PADLdown)
				{
				ViewChanged = TRUE;
				}

				// Rup/Rdown: move forward/back relative to current position
				// i.e. move twoards/away from reference point
			if (PadStatus & PADRup)
				{
				direction.vx = TheMainView.vrx - TheMainView.vpx;
				direction.vy = TheMainView.vry - TheMainView.vpy;
				direction.vz = TheMainView.vrz - TheMainView.vpz;

				TheMainView.vpx += (viewChange * direction.vx) >> 4;
				TheMainView.vpy += (viewChange * direction.vy) >> 4;
				TheMainView.vpz += (viewChange * direction.vz) >> 4;

				ViewChanged = TRUE;
				}
			if (PadStatus & PADRdown)
				{
				direction.vx = TheMainView.vrx - TheMainView.vpx;
				direction.vy = TheMainView.vry - TheMainView.vpy;
				direction.vz = TheMainView.vrz - TheMainView.vpz;

				TheMainView.vpx -= (viewChange * direction.vx) >> 4;
				TheMainView.vpy -= (viewChange * direction.vy) >> 4;
				TheMainView.vpz -= (viewChange * direction.vz) >> 4;

				ViewChanged = TRUE;
				}
			}
			#endif		// UNFINISHED
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

						TheMainView.super 
							= &ObjectArray[MainViewObjectID].coord;		
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

					   	TheMainView.super 
							= &ObjectArray[MainViewObjectID].coord;
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
				UnHighlightObject( &ObjectArray[SelectedObject]);
				SelectedObject = FindNextHighestObjectID (SelectedObject);
				HighlightObject( &ObjectArray[SelectedObject]);
				framesSinceLastSelection = 0;
				return;
				}
			}
			// Ldown: select previous object
		if (PadStatus & PADLdown)
			{
			if (framesSinceLastSelection > NORMAL_TIME_DELAY)
				{
			   	UnHighlightObject( &ObjectArray[SelectedObject]);
				SelectedObject = FindNextLowestObjectID (SelectedObject);
				HighlightObject( &ObjectArray[SelectedObject]);
				framesSinceLastSelection = 0;
				return;
				}
			}
		}

		// check that selected ID is valid before indexing array
	assert(SelectedObject >= 0);
	assert(SelectedObject < MAX_OBJECTS);

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
				// flyby; director's cut

		
				// if view super not world, set to world
			if (TheMainView.super != WORLD)
				{
				TheMainView.super = WORLD;
				ViewChanged = TRUE;
				return;
				}

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

	if (PadStatus & PADselect)
		speedMultiplier = 10;		// fast movement
	else
		speedMultiplier = 1;		// slow movement

	assert(SelectedObject >= 0);
	assert(SelectedObject < MAX_OBJECTS);

	if (ObjectArray[SelectedObject].inOrbit == TRUE)
		{
		if (SelectedOption == 3)		  // change object radius
			{
			if (PadStatus & PADLup)
				{
				ObjectArray[SelectedObject].radius += speedMultiplier;
				}
			if (PadStatus & PADLdown)
				{
				ObjectArray[SelectedObject].radius -= speedMultiplier;
				}
			}

		if (SelectedOption == 4)		  // toggle which way round object follows orbit
			{
			if (PadStatus & PADLup)
				{
				if (framesSinceLastSelection > NORMAL_TIME_DELAY)
					{
					if (ObjectArray[SelectedObject].whichWay == CLOCKWISE)
						ObjectArray[SelectedObject].whichWay = ANTICLOCKWISE;
					else
						ObjectArray[SelectedObject].whichWay = CLOCKWISE;
					}
				framesSinceLastSelection = 0;
				}
			}
		}
	else
		{
		if (SelectedOption == 3)		  // change object velocity
			{
			if (ObjectArray[SelectedObject].movementMomentumFlag == TRUE)
				movementModifier = 4;
			else
				movementModifier = 0;
			if (PadStatus & PADLleft)
				{
				ObjectArray[SelectedObject].velocity.vx 
						-= MovementSpeed * speedMultiplier >> movementModifier;
				}
			if (PadStatus & PADLright)
				{
				ObjectArray[SelectedObject].velocity.vx 
						 += MovementSpeed * speedMultiplier >> movementModifier;
				}
			if (PadStatus & PADLup)
				{
				ObjectArray[SelectedObject].velocity.vy 
						 -= MovementSpeed * speedMultiplier >> movementModifier;
				}
			if (PadStatus & PADLdown)
				{
				ObjectArray[SelectedObject].velocity.vy 
						 += MovementSpeed * speedMultiplier >> movementModifier;
				}
			if (PadStatus & PADL1)
				{
				ObjectArray[SelectedObject].velocity.vz 
						 -= MovementSpeed * speedMultiplier >> movementModifier;
				}
			if (PadStatus & PADL2)
				{
				ObjectArray[SelectedObject].velocity.vz 
						 += MovementSpeed * speedMultiplier >> movementModifier;
				}
			}

		if (SelectedOption == 4)		  // turn object momentum on and off
			{
			if (PadStatus & PADLleft)
				{
				if (framesSinceLastSelection > NORMAL_TIME_DELAY)
					{
					ObjectArray[SelectedObject].movementMomentumFlag = FALSE; 
					framesSinceLastSelection = 0;
					}
				}
			if (PadStatus & PADLright)
				{
				if (framesSinceLastSelection > NORMAL_TIME_DELAY)
					{
					ObjectArray[SelectedObject].movementMomentumFlag = TRUE;
					framesSinceLastSelection = 0;
					}
				}
			}
		}
}





void HandleChangingAnObjectsSpin (void)
{
	int speedMultiplier; 

	if (PadStatus & PADselect)
		speedMultiplier = 10;		// fast twisting
	else
		speedMultiplier = 1;		// slow twisting

	assert(SelectedObject >= 0);
	assert(SelectedObject < MAX_OBJECTS);

	if (SelectedOption == 3)		// spin: twist vector
		{
			// this chunk: left directional pad plus L1/L2: 
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
		}

	if (SelectedOption == 4)			// angular momentum
		{
			// Lleft and Lright: set object spin momentum on or off
			// when off, this submode adjusts angle of object
		if (PadStatus & PADLleft)
			{
			if (framesSinceLastSelection > NORMAL_TIME_DELAY)
				{
				ObjectArray[SelectedObject].rotationMomentumFlag = FALSE;
				framesSinceLastSelection = 0;
				}
			}
		if (PadStatus & PADLright)
			{
			if (framesSinceLastSelection > NORMAL_TIME_DELAY)
				{
				ObjectArray[SelectedObject].rotationMomentumFlag = TRUE;
				framesSinceLastSelection = 0;
				}
			}
		}
}




void HandleChangingAnObjectsVolume (void)
{
	double volumeMultiplier; 

	if (PadStatus & PADselect)
		volumeMultiplier = 0.1;		// fast twisting
	else
		volumeMultiplier = 0.01;		// slow twisting

	assert(SelectedObject >= 0);
	assert(SelectedObject < MAX_OBJECTS);

	if (SelectedOption == 3)			// change scaleX
		{
		if (PadStatus & PADLup)
			{
			ObjectArray[SelectedObject].scaleX += volumeMultiplier; 	  
			}
		if (PadStatus & PADLdown)
			{	
			ObjectArray[SelectedObject].scaleX -= volumeMultiplier;  
			}
		}

	if (SelectedOption == 4)			// change scaleY
		{
		if (PadStatus & PADLup)
			{
			ObjectArray[SelectedObject].scaleY += volumeMultiplier; 	  
			}
		if (PadStatus & PADLdown)
			{	
			ObjectArray[SelectedObject].scaleY -= volumeMultiplier;  
			}
		}

	if (SelectedOption == 5)			// change scaleZ
		{
		if (PadStatus & PADLup)
			{
			ObjectArray[SelectedObject].scaleZ += volumeMultiplier; 	  
			}
		if (PadStatus & PADLdown)
			{	
			ObjectArray[SelectedObject].scaleZ -= volumeMultiplier;  
			}
		}
}





void HandleChangingAnObjectsDisplay (void)
{
	int semiTransparency = 0;

	assert(SelectedObject >= 0);
	assert(SelectedObject < MAX_OBJECTS);

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
		}

	if (SelectedOption == 4)		// Lup: toggle light calculations
		{ 
		if (PadStatus & PADLup)
			{
			if (framesSinceLastSelection > NORMAL_TIME_DELAY)
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
		}

	if (SelectedOption == 5)		   // Lup: increase semi-transparency setting
		{
			// find current semi-transparency setting
		if (ObjectArray[SelectedObject].handler.attribute & 1<<28)
			semiTransparency++;
		if (ObjectArray[SelectedObject].handler.attribute & 1<<29)
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
		}

	if (SelectedOption == 6)		// Lup: toggle semi-transparency on/off
		{
		if (PadStatus & PADLup)
			{
			if (framesSinceLastSelection > NORMAL_TIME_DELAY)
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
		}

	if (SelectedOption == 7)			  // Lup: toggle object display
		{
		if (PadStatus & PADLup)
			{
			if (framesSinceLastSelection > NORMAL_TIME_DELAY)
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
	int speedMultiplier;			// rate of change of fly-by view attributes

	if (PadStatus & PADselect)	   
		speedMultiplier = 10;
	else
		speedMultiplier = 1;

	if (SelectedOption == 4) 		// left directional controls: alter local angle
  		{
	  	if (PadStatus & PADLleft)
			{
			LocalAngle.vx += 10 * speedMultiplier;
			ViewChanged = TRUE;
			}
		if (PadStatus & PADLright)
			{
			LocalAngle.vx -= 10 * speedMultiplier;
			ViewChanged = TRUE;
			}
		if (PadStatus & PADLup)
			{
			LocalAngle.vy += 10 * speedMultiplier;
			ViewChanged = TRUE;
			}
		if (PadStatus & PADLdown)
			{
			LocalAngle.vy -= 10 * speedMultiplier;
			ViewChanged = TRUE;
			}
		if (PadStatus & PADL1)
			{
			LocalAngle.vz += 10 * speedMultiplier;
			ViewChanged = TRUE;
			}
		if (PadStatus & PADL2)
			{
			LocalAngle.vz -= 10 * speedMultiplier;
			ViewChanged = TRUE;
			}
		}

	if (SelectedOption == 5)		// Lup/Ldown: change viewing point distance
		{
		if (PadStatus & PADLup)
			{
			ReferencePointDistance += 10 * speedMultiplier;
			ViewChanged = TRUE;
			}
		if (PadStatus & PADLdown)
			{
			ReferencePointDistance -= 10 * speedMultiplier;
			ViewChanged = TRUE;
			}
		}

	if (SelectedOption == 6)			// Lup/Ldown: change the reference point distance
		{
		if (PadStatus & PADLup)
			{
			ReferencePointDistance += 10 * speedMultiplier;
			ViewChanged = TRUE;
			}
		if (PadStatus & PADLdown)
			{
			ReferencePointDistance -= 10 * speedMultiplier;
			ViewChanged = TRUE;
			}
		}
}

		
	
		
	


	// find id of first existing object with lower id 
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


	// find id of first existing object with higher id
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




int FindNextUnusedObjectID (void)
{
	int id;
	int found;
	int i;

	found = -1;
	for (i = 0; i < MAX_OBJECTS; i++)
		{
		if (ObjectArray[i].alive == FALSE)
			{
			found = 1;
			id = i;
			break;
			}
		}

	if (found == -1)
		return -1;
	else
		return id;
}







	// deal with movement and rotation, etc
void HandleAllObjects (void)
{
	int i;

		// firstly deal with the spheres
	for (i = 0; i < MAX_OBJECTS; i++)
		{
		if (ObjectArray[i].alive == TRUE)
			{
				// deal with movement and rotation
			UpdateCoordinateSystem2 (&ObjectArray[i].rotate,
							&ObjectArray[i].twist,
							&ObjectArray[i].position,
							&ObjectArray[i].velocity,
							&ObjectArray[i].coord);

			SortObjectSize (&ObjectArray[i]);

				// update angle
			ObjectArray[i].rotate.vx += ObjectArray[i].twist.vx;
			ObjectArray[i].rotate.vy += ObjectArray[i].twist.vy;
			ObjectArray[i].rotate.vz += ObjectArray[i].twist.vz;

#if 0
				// update position
			ObjectArray[i].position.vx += ObjectArray[i].velocity.vx;
			ObjectArray[i].position.vy += ObjectArray[i].velocity.vy;
			ObjectArray[i].position.vz += ObjectArray[i].velocity.vz;
#endif

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
	UpdateCoordinateSystem3( &PlayersShuttle.rotate,
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




	// Sort out all spatial movement

	// This version: Not using hierarchical coordinate system because
	// we want all the bodies to be able to spin without 
	// afffecting others:
	// hence calculate position as offset from position of
	// body which object orbits around
void DealWithSpatialMovement (void)
{
	ObjectHandler* object;
	ObjectHandler* orbitCentreObject;
	int planeX, planeY;
	int newX, newY, newZ;
	int i;

	for (i = 0; i < MAX_OBJECTS; i++)
		{
		if (ObjectArray[i].alive == TRUE)
			{
			if (ObjectArray[i].inOrbit == TRUE)
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
			else
				{
				if (ObjectArray[i].specialMovement != NONE)
					{
					HandleSpecialMovement( &ObjectArray[i]);
					}
				}
			}
		}
}




#define HALF_RECTANGLE 1000
#define SHIP_SPEED 20

void HandleSpecialMovement (ObjectHandler* object)
{
	if (object->specialMovement == FIRST_SIMPLE_PATH)
		{
		if (object->position.vy == HALF_RECTANGLE)
			{
			if (object->position.vz == HALF_RECTANGLE)
				{
				// rotate here
				object->rotate.vx += ONE/4;
				object->velocity.vx = 0;
				object->velocity.vy = 0;
				object->velocity.vz = -SHIP_SPEED; 
				}
			else if (object->position.vz == -HALF_RECTANGLE)
				{
				// rotate here
				object->rotate.vx += ONE/4;
				object->velocity.vx = 0;
				object->velocity.vy = -SHIP_SPEED;
				object->velocity.vz = 0;
				}
			}
		else if (object->position.vy == -HALF_RECTANGLE)
			{
			if (object->position.vz == HALF_RECTANGLE)
				{
				// rotate here
				object->rotate.vx += ONE/4;
				object->velocity.vx = 0;
				object->velocity.vy = SHIP_SPEED;
				object->velocity.vz = 0; 
				}
			else if (object->position.vz == -HALF_RECTANGLE)
				{
				// rotate here
				object->rotate.vx += ONE/4;
				object->velocity.vx = 0;
				object->velocity.vy = 0;
				object->velocity.vz = SHIP_SPEED;
				}
			}
		}






#if 0
	if (object->specialMovement == FIRST_SIMPLE_PATH)
		{
		switch (object->position.vy)
			{
			case HALF_RECTANGLE:
				switch(object->position.vz)
					{
					case HALF_RECTANGLE:
						// rotate here
						object->rotate.vx += ONE/4;
						object->velocity.vx = 0;
						object->velocity.vy = 0;
						object->velocity.vz = -SHIP_SPEED; 
						break;
					case -HALF_RECTANGLE:
						// rotate here
						object->rotate.vx += ONE/4;
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
						object->rotate.vx += ONE/4;
						object->velocity.vx = 0;
						object->velocity.vy = SHIP_SPEED;
						object->velocity.vz = 0; 
						break;
					case -HALF_RECTANGLE:
						// rotate here
						object->rotate.vx += ONE/4;
						object->velocity.vx = 0;
						object->velocity.vy = 0;
						object->velocity.vz = SHIP_SPEED;
						break;
					}
				break;
			}
		}
#endif
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

	InitialiseModels();

	SetUpSolarSystem();
	
		// sort out print-to-screen font
	FntLoad(960, 256);	
	FntOpen(32, 32, 256, 200, 0, 512); 	

		// set up controller pad buffers
	PadInit(); 	 		

		// to start with, player moves around
	SelectionMode = MOVE_SHIP;	 
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
	InitSingleObject( &PlayersShuttle);

	PlayersShuttle.alive = TRUE;
   
		// overall speed for moving player's ship 
		MovementSpeed = INITIAL_MOVEMENT_SPEED; 
		RotationSpeed = INITIAL_ROTATION_SPEED;

		// push player's ship away from solar system
		// so player can see where things are
    PlayersShuttle.position.vz = -2000;
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

	object->specialMovement = NONE;
}


   


	// here, we highlight the selected object by turning off the
	// light calculations
void HighlightObject (ObjectHandler* object)
{
	// DO NOTHING object->handler.attribute |= GsLOFF;
}



	// restore light calculations
void UnHighlightObject (ObjectHandler* object)
{
	// DO NOTHING TURN_NTH_BIT_OFF(object->handler.attribute, 32, 6)
}





	// link TMD 3D object models to object handlers
void InitialiseModels (void)
{
	int i;

	NumberOfObjects = CountNumberOfObjectsInTmdFile(MODEL_ADDRESS);
	printf("NumberOfObjects: %d\n", NumberOfObjects);
	
		// each object the same: green sphere
	for (i = 0; i < MAX_OBJECTS; i++)
		{
		if (i == NumberOfObjects)
			break;

		printf("linking to object %d\n", i);
		LinkObjectHandlerToTmdObject( &(ObjectArray[i].handler), 
					i, MODEL_ADDRESS);
	
			// each object has its own independent coordinate system
		ObjectArray[i].handler.coord2 = &(ObjectArray[i].coord);
		}
		
		// now sort the player's ship
	LinkObjectHandlerToTmdObject( &(PlayersShuttle.handler), 
					0, MODEL_ADDRESS);

		// player's ship has its own independent coordinate system
	PlayersShuttle.handler.coord2 = &(PlayersShuttle.coord);
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

	MainViewObjectID = -2;		// the shuttle
	// NOT YET MainViewObjectID = -1;
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
	LocalAngle.vx = 0;	  
	LocalAngle.vy = 0;
	LocalAngle.vz = ONE;
	ViewingPointDistance = 500;
	ReferencePointDistance = 1000;

		// default values for director view
	WhichDirectorSequence = NONE;	
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
			SetUpLocalView (&LocalAngle, ViewingPointDistance,
								ReferencePointDistance);
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




void SetUpLocalView (VECTOR *localAngle, int viewingPointDistance,
								int referencePointDistance)
{
	LocalAngle.vx = localAngle->vx;
	LocalAngle.vy = localAngle->vy;
	LocalAngle.vz = localAngle->vz;

	ViewingPointDistance = viewingPointDistance;
	ReferencePointDistance = referencePointDistance;
}



	
	// no settings here, but easy to make director view editable
	// function below sets up the view with its standard preferences
void SetUpDirectorsView (void)
{
	SpecialViewFlag = TRUE;
	SpecialViewType = DIRECTORS_VIEW;

		// set the parameters that do not change across
		// a director view session

		// choose which sequence randomly
	if (rand() % 2 == 0)
		WhichDirectorSequence = 0;
	else
		WhichDirectorSequence = 1;

		// choose start point on sequence randomly
	SequencePosition = rand() % 3;
 
	SingleViewCycleTime = 30;
	DirectorFrameCounter = 0;

	DirectorLocalViewDistance = 2000;
	DirectorCircleViewRadius = 2000;	
	DirectorPathDistance = 2000;
}







void DealWithSpecialView (void)
{
	assert(SpecialViewFlag == TRUE);

	switch (SpecialViewType)
		{
		case EXTERNAL_VIEW:
			DealWithStandardView();
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




void DealWithStandardView (void)
{
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
	baseObject = &ObjectArray[SelectedObject];

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
}






void DealWithCirclingView (void)
{
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
	baseObject = &ObjectArray[SelectedObject];

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
}





void DealWithFlybyView (void)
{
	MATRIX tempMatrix;
	VECTOR displacement;		
	VECTOR realDisplacement;	
	ObjectHandler* baseObject;

	assert(TheMainView.super == WORLD);		// free-roaming view
	assert(FlybyCycleTime > 0);

		// displacement relative to viewed object
	displacement.vx = 0;
	displacement.vy = 0;
	displacement.vz = 0;

		// find which object we are viewing
	assert(SelectedObject >= 0);
	assert(SelectedObject < MAX_OBJECTS);
	baseObject = &ObjectArray[SelectedObject];

		// what kind of motion does object exhibit?
	TypeOfFlybyView = FindObjectMotionType(baseObject);

	switch(TypeOfFlybyView)
		{
		case LINEAR:
			{
			int objectSpeed;

			objectSpeed = FindObjectSpeed(baseObject);

				// set view reference point to centre of object
				// so that we keep looking toward the object
			TheMainView.vrx = baseObject->coord.coord.t[0];
			TheMainView.vry = baseObject->coord.coord.t[1];
			TheMainView.vrz = baseObject->coord.coord.t[2];

				// if just started a flyby cycle, position viewing point
				// on parallel path	to object, upwind of object
			if (NumberFlybyFramesSoFar == 0)		
				{
				displacement.vx = (PathDistance * rcos(PathAngle)) >> 12;
				displacement.vy = (PathDistance * rsin(PathAngle)) >> 12;
				displacement.vz = (objectSpeed / FlybyCycleTime) / 2;

					// find rotation matrix from rotation angle of object
				   	// i.e. matrix expressing object angle w.r.t. super
				RotMatrix(&baseObject->rotate, &tempMatrix);

					// find position from object in terms of super coords
				ApplyMatrixLV(&tempMatrix, &displacement, &realDisplacement);

					// hence position viewpoint 
				TheMainView.vpx = TheMainView.vrx + realDisplacement.vx;
				TheMainView.vpy = TheMainView.vry + realDisplacement.vy;
				TheMainView.vpz = TheMainView.vrz + realDisplacement.vz;
				}  
			}
			break;
		case CIRCULAR:
			{
			ObjectHandler* centralObject;		// object that baseObject orbits
			int viewDistanceFromCentralBody;
			int angleStart, angleMiddle;
			int planeX, planeY;

				// set view reference point to centre of object
				// so that we keep looking toward the object
			TheMainView.vrx = baseObject->coord.coord.t[0];
			TheMainView.vry = baseObject->coord.coord.t[1];
			TheMainView.vrz = baseObject->coord.coord.t[2];

				// if just started a flyby cycle, position viewing point
				// on tangent removed from orbit of object
				// ie a tangent to orbital circle that has been moved 
				// in the direction of circle radius
			if (NumberFlybyFramesSoFar == 0)
				{
				assert(baseObject->centralBodyID >= 0);
				assert(baseObject->centralBodyID < MAX_OBJECTS);

				centralObject = &ObjectArray[baseObject->centralBodyID];

				angleStart = baseObject->angle;
				angleMiddle = angleStart 
						+ ((baseObject->angleIncrement * FlybyCycleTime)/2);
				viewDistanceFromCentralBody = baseObject->radius + PathDistance;

				planeX = (viewDistanceFromCentralBody * rcos(angleMiddle)) >> 12;
				planeY = (viewDistanceFromCentralBody * rsin(angleMiddle)) >> 12; 


					// this is offset position from orbiting body
				displacement.vx = (planeX * baseObject->firstVector.vx)
					+ (planeY * baseObject->secondVector.vx);
				displacement.vy = (planeX * baseObject->firstVector.vy)
					+ (planeY * baseObject->secondVector.vy);
				displacement.vz = (planeX * baseObject->firstVector.vz)
					+ (planeY * baseObject->secondVector.vz);
			
					// the vectors are relative to size ONE
					// hence shift-right 12 to make them unit vectors
				TheMainView.vpx 
					= centralObject->position.vx + (displacement.vx >> 12);
				TheMainView.vpy 	
					= centralObject->position.vy + (displacement.vy >> 12);
				TheMainView.vpz 
					= centralObject->position.vz + (displacement.vz >> 12);
				}
			}
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
	int currentSubMode;

	assert(WhichDirectorSequence >= 0 && WhichDirectorSequence <= 1);
	assert(SequencePosition >= 0 && SequencePosition <= 2);

	currentSubMode = DirectorSequences[WhichDirectorSequence][SequencePosition];

	switch(currentSubMode)
		{
		case LOCAL_VIEW:				
			// STRONG PREFERENCE for forward ie PLUS_Z view
			break;
		case CIRCLING_VIEW:
			// STRONG PREFERENCE for x_z_plane
			break;
		case FLYBY_VIEW:
			// STRONG PREFERENCE for looking from underneath, i.e. angle is 270 degrees
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
			if (WhichDirectorSequence == 1)		// go back to other sequence
				{
				WhichDirectorSequence = 0;
				DirectorFrameCounter = 0;
				}
			else
				WhichDirectorSequence++;		// advance to next sequence
			}
		else
			SequencePosition++;	  // next view in this sequence
		}	
} 




void DealWithLocalView (void)
{
	MATRIX tempMatrix;
	VECTOR displacement1, displacement2;		// from object, relative to object
	VECTOR realDisplacement1, realDisplacement2;	// from object, relative to world
	ObjectHandler* baseObject;

	assert(TheMainView.super == WORLD);		// free-roaming view

		// displacement relative to viewed object
	displacement1.vx = 0;
	displacement1.vy = 0;
	displacement1.vz = 0;
	displacement2.vx = 0;
	displacement2.vy = 0;
	displacement2.vz = 0;

		// find which object we are viewing
	assert(SelectedObject >= 0);
	assert(SelectedObject < MAX_OBJECTS);
	baseObject = &ObjectArray[SelectedObject];

		// viewing point: between centre and surface of object
	displacement1.vx = ViewingPointDistance * LocalAngle.vx;
	displacement1.vy = ViewingPointDistance * LocalAngle.vy;
	displacement1.vz = ViewingPointDistance * LocalAngle.vz;

		// reference point: beyond surface of object
	displacement2.vx = ReferencePointDistance * LocalAngle.vx;
	displacement2.vy = ReferencePointDistance * LocalAngle.vy;
	displacement2.vz = ReferencePointDistance * LocalAngle.vz;			

		// find rotation matrix from rotation angle of object
		// i.e. matrix expressing object angle w.r.t. super
	RotMatrix(&baseObject->rotate, &tempMatrix);

		// find position from object in terms of super coords
	ApplyMatrixLV(&tempMatrix, &displacement1, &realDisplacement1);
	ApplyMatrixLV(&tempMatrix, &displacement2, &realDisplacement2);

		// hence position viewpoint 
	TheMainView.vpx = baseObject->coord.coord.t[0] + realDisplacement1.vx;
	TheMainView.vpy = baseObject->coord.coord.t[1] + realDisplacement1.vy;
	TheMainView.vpz = baseObject->coord.coord.t[2] + realDisplacement1.vz;

		// and reference point 
	TheMainView.vrx = baseObject->coord.coord.t[0] + realDisplacement2.vx;
	TheMainView.vry = baseObject->coord.coord.t[1] + realDisplacement2.vy;
	TheMainView.vrz = baseObject->coord.coord.t[2] + realDisplacement2.vz;

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

	




void SetUpSolarSystem (void)
{
	ArrangeObjectsIntoLine();




#if 0			// OLD OLD OLD
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

#if 1
	for (i = 0; i < 4; i++)
		{
		ShipID = FindNextUnusedObjectID();
		ObjectArray[ShipID].alive = TRUE;
		ObjectArray[ShipID].type = SHIP;
		ObjectArray[ShipID].specialMovement = FIRST_SIMPLE_PATH;
		ObjectArray[ShipID].movementMomentumFlag = TRUE;

			// make it larger to be visible
		ObjectArray[ShipID].scaleX = 4;
		ObjectArray[ShipID].scaleY = 4;
		ObjectArray[ShipID].scaleZ = 4;

		switch(i)
			{
			case 0:
				ObjectArray[ShipID].position.vy = -HALF_RECTANGLE;
				ObjectArray[ShipID].velocity.vz = SHIP_SPEED;
				break;
			case 1:
				ObjectArray[ShipID].position.vz = HALF_RECTANGLE;
				ObjectArray[ShipID].velocity.vy = SHIP_SPEED;
				ObjectArray[ShipID].rotate.vx = ONE/4;
				break;
			case 2:
				ObjectArray[ShipID].position.vy = HALF_RECTANGLE;
				ObjectArray[ShipID].velocity.vz = -SHIP_SPEED;
				ObjectArray[ShipID].rotate.vx = ONE/2;
				break;
			case 3:
				ObjectArray[ShipID].position.vz = -HALF_RECTANGLE;
				ObjectArray[ShipID].velocity.vy = -SHIP_SPEED;
				ObjectArray[ShipID].rotate.vx = 3 * ONE/4;
				break;
			default:
				assert(FALSE);
			}
		}
#endif



#endif		// OLD OLD OLD
}




void ArrangeObjectsIntoLine (void)
{
	int i;
   
	for (i = 0; i < NumberOfObjects; i++)
		{
		ObjectArray[i].alive = TRUE;
		ObjectArray[i].position.vx = i * 250;
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







void ResetAll (void)
{
	InitialiseObjects();
	InitialiseLighting();
	SetUpSolarSystem();

		// to start with, player moves around
	SelectionMode = MOVE_SHIP;	 
	framesSinceLastSwitch = 0;
	ResetSelectionData();

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





