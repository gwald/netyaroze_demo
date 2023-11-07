	// taken and modified from the shell program main.c in blackpsx\modules\third
	// using 'smaller' batch file



/****************************************************************************
					includes
****************************************************************************/


#include <stdio.h>

#include <libps.h>

#include "general.h"

#include "asssert.h"

#include "dump.h"

#include "pad.h"

#include "2d1.h"

#include "rand.h"

#include "text_str.h"

#include "address.h"

#include "sound.h"

#include "object.h"

#include "trig.h"

#include "vector.h"

#include "matrix.h"







/****************************************************************************
					structures and constants
****************************************************************************/



	// Here: declare a GsIMAGE for every texture to be used in program
GsIMAGE WaveTextureInfo;






int ScreenWidth, ScreenHeight;



#define PACKETMAX	2048		/* Max GPU packets */
#define PACKETMAX2	(PACKETMAX*24)

static PACKET packetArea[2][PACKETMAX2]; /* GPU PACKETS AREA */
  

#define OT_LENGTH	9		

static GsOT Wot[2];			/* Handler of OT */
static GsOT_TAG wtags[2][1<<OT_LENGTH];



long ProjectionDistance;
GsRVIEW2 TheView;


GsF_LIGHT TheLights[3];

GsFOGPARAM TheFogging;

int OverallLightMode;






int QuitFlag = FALSE;

int frameNumber = 0;














	// object types
#define SHIP 0
#define CUBE 1
#define SQUARE 2
#define POLYGON 3

ObjectHandler TheShip;

#define MAX_CUBES 12
ObjectHandler TheCubes[MAX_CUBES];

#define MAX_SQUARES 128
ObjectHandler TheSquares[MAX_SQUARES];

#define MAX_POLYGONS 128
ObjectHandler ThePolygons[MAX_POLYGONS];



	// graphical world clipping

ObjectHandler* DisplayedObjects[MAX_OBJECTS];



	// the walls of large cube
#define SQUARE_SIZE 128

#define FLOOR_X 4
#define FLOOR_Y 4
#define FLOOR_Z 4


#define WORLD_X (FLOOR_X * SQUARE_SIZE)
#define WORLD_Y (FLOOR_Y * SQUARE_SIZE)
#define WORLD_Z (FLOOR_Z * SQUARE_SIZE)





#define MAX_TUNNEL_SECTIONS 64

int NumberTunnelSections;

#define NUMBER_SHAPES_PER_SECTION 12
int TunnelSectionShapeAngle;

int TunnelSectionLength;
int TunnelSectionRadius;

typedef struct
{
	int id;

	VECTOR splinePoint;		  // in world coords: the spline point
	
	int thetaX, thetaY;	 	  // rotation angles from previous section
								// NOTE: rotation order FIXED at (y then x)

	GsCOORDINATE2 coordinateData;	   // own coords

	GsCOORDINATE2 circleCoordinates;	  // virtual circle: vertices for tunnel polygons

		// vertices on circumference of circle: vertices in world coords
	VECTOR pointsOnCircle[NUMBER_SHAPES_PER_SECTION];
} TunnelSection;

TunnelSection TheTunnel[MAX_TUNNEL_SECTIONS];







#define CREATED_TMD_ADDRESS 0x80095000
u_long* CreatedTMDPointer = (u_long*) CREATED_TMD_ADDRESS;


#define START_OF_CREATED_TMDS_STACK	0x80096000
int NumberOfCreatedTMDs = 0;
u_long CurrentTMDStackAddress = START_OF_CREATED_TMDS_STACK;
#define SIZEOF_SINGLE_POLYGON_TMD 0x00000100

/****************************************************************************
					prototypes
****************************************************************************/


void main (void);


int PerformWorldClipping (ObjectHandler** firstArray, ObjectHandler** secondArray);
void FindViewPoint (VECTOR* output);




void InitialiseAll (void);



void SortVideoMode (void);

void InitialiseLighting (void);
void InitialiseView (void);





void InitialiseObjects (void);



void InitialiseTheTunnel (void);
void InitTunnelToVoid (void);
void CalculateCirclePoints (void);
void PropagateRotationsAlongTunnel (void);
void GenerateSplineFromListOfRotations (int numberOfSections, SVECTOR* rotationList,
											VECTOR* outputSpline, int sectionDistance,
												VECTOR* startingPosition,
													MATRIX* startingMatrix);


void CreateFirstSpline (void);

void CreateTheTunnel (void);

void UpdateTheTunnel (void);



void CleanupAndExit (void);



void DealWithControllerPad (void);




void HandleAllObjects (void);

void HandleTheShip (ObjectHandler* object);
void HandleACube (ObjectHandler* object);
void HandleASquare (ObjectHandler* object);
void HandleAPolygon (ObjectHandler* object);



void CheckCollisions (void);
int ObjectsVeryClose (ObjectHandler* first, ObjectHandler* second);




void UpdateObjectCoordinates (VECTOR* twist,
							VECTOR* position, VECTOR* velocity,
							GsCOORDINATE2* coordSystem, MATRIX* matrix);

void UpdateObjectCoordinates2 (SVECTOR* rotationVector,
							VECTOR* translationVector,
							GsCOORDINATE2* coordSystem);

void DeriveNewCoordSystemFromRotation (GsCOORDINATE2* original,
			SVECTOR* rotation, GsCOORDINATE2* output);

u_short SortSpriteObjectPosition (ObjectHandler* object);




void StoreScreen (void);




int CheckCoordinateSystemIsBase (GsCOORDINATE2* coord);

void CopyCoordinateSystem (GsCOORDINATE2* from, GsCOORDINATE2* to);





void CreateExtraTMDs (void);




ObjectHandler* GetNextFreePolygon (void);

u_long CreateAnotherPolygonTMD (int clutX, int clutY, int tPageID, int pixelMode, 
					u_char u0, u_char v0,
					u_char u1, u_char v1, 
					u_char u2, u_char v2, 
					u_char u3, u_char v3,
					TMD_NORM* norm0, 
					TMD_VERT* vert0, 
					TMD_VERT* vert1, 
					TMD_VERT* vert2, 
					TMD_VERT* vert3);

void GetCrossProduct (VECTOR* first, VECTOR* second, VECTOR* output);





/****************************************************************************
					 macros
****************************************************************************/


#define min(a,b) ( ((a) > (b)) ? (b) : (a))

#define max(a,b) ( ((a) > (b)) ? (a) : (b))

#define KeepWithinRange(quantity, min, max)					\
{															\
	if ((quantity) < (min))									\
		(quantity) = (min);									\
	else if ((quantity) > (max))							\
		(quantity) = (max);									\
}



#define setVECTOR(vector, x, y, z)				\
				(vector)->vx = (x), (vector)->vy = (y), (vector)->vz = (z)	

#define setNORMAL(normal, x, y, z)				\
				(normal)->nx = (x), (normal)->ny = (y), (normal)->nz = (z)
				
#define setVERTEX(vertex, x, y, z)				\
				(vertex)->vx = (x), (vertex)->vy = (y), (vertex)->vz = (z)	


#define TURN_NTH_BIT_OFF(argument, sizeInBits, N)			\
	{														\
	u_long onlyNthBitOn;									\
															\
	onlyNthBitOn = 1 << (N);								\
															\
	argument ^= onlyNthBitOn;								\
	}


		
  
/****************************************************************************
					functions
****************************************************************************/


void main (void)
{
	int	hsync = 0;
	int bufferIndex;			  // buffer index
	int i;
	u_short zValue;
	ObjectHandler* object;
	MATRIX tmpls;
	int numberToDisplay;
	
	InitialiseAll();

	bufferIndex = GsGetActiveBuff();

	while (QuitFlag == FALSE)
		{		
		FntPrint("frame: %d\n", frameNumber);

		FntPrint("VP: %d %d %d\n", TheView.vpx, TheView.vpy, TheView.vpz);

		FntPrint("proj: %d\n", ProjectionDistance);
	
		//RegisterTextStringForDisplay ("display test", -50, 80);
		
		FntPrint("ship %d %d %d\n", TheShip.position.vx,
			TheShip.position.vy, TheShip.position.vz);				

		frameNumber++;

		DealWithControllerPad();

		HandleSound();

		HandleAllObjects();

		GsSetRefView2( &TheView);

		GsSetWorkBase( (PACKET*)packetArea[bufferIndex]);

		GsClearOt(0, 0, &Wot[bufferIndex]);

		numberToDisplay 
					= PerformWorldClipping(ObjectArray, DisplayedObjects);
		FntPrint("num to display: %d\n", numberToDisplay);

		DisplayTextStrings (&Wot[bufferIndex]);

		for (i = 0; i < numberToDisplay; i++)
			{
			object = DisplayedObjects[i];

			if (object->displayFlag == TMD)
				{
				GsGetLs(&(object->coord), &tmpls);
						   
				GsSetLightMatrix(&tmpls);
							
				GsSetLsMatrix(&tmpls);
							
				GsSortObject4( &(object->handler), 
						&Wot[bufferIndex], 
							3, getScratchAddr(0));
				}
			else
				{
				zValue = SortSpriteObjectPosition(object);
				GsSortSprite( &object->sprite, &Wot[bufferIndex], zValue);
				}
			}

		hsync = VSync(0);
				 
		ResetGraph(1);

		GsSwapDispBuff();

		GsSortClear(0, 0, 32, &Wot[bufferIndex]);

		GsDrawOt(&Wot[bufferIndex]);

		bufferIndex = GsGetActiveBuff();

		FntPrint("hsync: %d\n", hsync);
		FntFlush(-1);
		}

	CleanupAndExit();
}






	// look at objects in first array,
	// if they're close to view point,
	// put them into second array

	// CURRENTLY DISABLED (clipping with no criterion -> all get included)

int PerformWorldClipping (ObjectHandler** firstArray, ObjectHandler** secondArray)
{
	int numberDisplayedObjects = 0;
	int i;
	VECTOR viewPoint;
	int SimpleClipDistance;

	FindViewPoint( &viewPoint);

	SimpleClipDistance = SQUARE_SIZE * 8;
	
	for (i = 0; i < MAX_OBJECTS; i++)
		{
		if (firstArray[i] != NULL)
			if (firstArray[i]->alive == TRUE)
				{
				#if 0
				if (abs(firstArray[i]->coord.coord.t[0] - viewPoint.vx) < SimpleClipDistance
					&& abs(firstArray[i]->coord.coord.t[1] - viewPoint.vy) < SimpleClipDistance
					&& abs(firstArray[i]->coord.coord.t[2] - viewPoint.vz) < SimpleClipDistance)
					{
					secondArray[numberDisplayedObjects] = firstArray[i];
					numberDisplayedObjects++;
					}
				#endif
					// no clipping: direct copy
				secondArray[numberDisplayedObjects] = firstArray[i];
				numberDisplayedObjects++;
				}
		}
		
	return numberDisplayedObjects;	
}		






void FindViewPoint (VECTOR* output)
{
	setVECTOR( output, TheView.vpx, TheView.vpy, TheView.vpz);
}





void InitialiseAll (void)
{
	PadInit();

	InitialiseRandomNumbers();

	InitialiseTextStrings();

	InitialiseSound();	   

	ScreenWidth = 320;
	ScreenHeight = 240;

	GsInitGraph(ScreenWidth, ScreenHeight, GsINTER|GsOFSGPU, 1, 0);
	GsDefDispBuff(0,0,0,ScreenHeight);

	GsInit3D();		  
	
	SortVideoMode(); 

		// sort our only ordering table
	Wot[0].length = OT_LENGTH;	
	Wot[0].org = wtags[0];	   
	Wot[1].length = OT_LENGTH;
	Wot[1].org = wtags[1];

	GsClearOt(0, 0, &Wot[0]);
	GsClearOt(0, 0, &Wot[1]);

	InitialiseLighting();

	InitialiseView();

		/// texture for square model
	ProperInitialiseTexture(WAVE_TEXTURE_ADDRESS, &WaveTextureInfo);
		 
		// sort basic text printing
	FntLoad(960, 256);
	FntOpen(0, 0, 256, 200, 0, 512);

	CreateExtraTMDs();

	InitialiseObjects();

	InitialiseTheTunnel();

	CreateTheTunnel();		
}





void SortVideoMode (void)
{
	int currentMode;
	
	currentMode = GetVideoMode();

	printf("\n");
	switch(currentMode)
		{
		case MODE_PAL:
			printf("currently in video mode PAL\n");
			break;
		case MODE_NTSC:
			printf("currently in video mode NTSC\n");
			break;
		default:
			assert(FALSE);
		}

	printf("If everything is black and white\n");
	printf("or there are lines at bottom or top of screen\n");
	printf("Then you need to change video mode\n\n");
}





void InitialiseLighting (void)
{
		// three flat light sources
	TheLights[0].vx = ONE; TheLights[0].vy = 0; TheLights[0].vz = 0;
	TheLights[0].r = 128; TheLights[0].g = 0; TheLights[0].b = 0;
	GsSetFlatLight(0, &TheLights[0]);

	TheLights[1].vx = 0; TheLights[1].vy = ONE; TheLights[1].vz = 0;
	TheLights[1].r = 0; TheLights[1].g = 128; TheLights[1].b = 0;
	GsSetFlatLight(1, &TheLights[1]);

	TheLights[2].vx = 0; TheLights[2].vy = 0; TheLights[2].vz = ONE;
	TheLights[2].r = 0; TheLights[2].g = 0; TheLights[2].b = 128;
	GsSetFlatLight(2, &TheLights[2]);

		
		// background lighting
	GsSetAmbient(ONE/4, ONE/4, ONE/4);

  
		// distance colour blending
  	TheFogging.dqa = -960;
	TheFogging.dqb = 5120*5120;
	TheFogging.rfc = 0; 
	TheFogging.gfc = 0; 
	TheFogging.bfc = 4;
	GsSetFogParam( &TheFogging);


		// overall lighting conditions
	OverallLightMode = 0;			// NO fogging yet 
	GsSetLightMode(OverallLightMode);
}






void InitialiseView (void)
{
	ProjectionDistance = 192;			

	GsSetProjection(ProjectionDistance);  

	TheView.vrx = 0; 
	TheView.vry = 0; 
	TheView.vrz = 0; 

	TheView.vpx = 0; 
	TheView.vpy = 0; 
	TheView.vpz = 220;
	
	TheView.rz = 0;

	TheView.super = &TheShip.coord;

	GsSetRefView2(&TheView);
}







void InitialiseObjects (void)
{
	int i;

	InitialiseObjectClass();



	InitSingleObject(&TheShip);

	BringObjectToLife (&TheShip, SHIP, 
					SHIP_MODEL_ADDRESS, 0, NONE);

	setVECTOR( &TheShip.position, 0, 0, SQUARE_SIZE*6);
	setVECTOR( &TheShip.rotate, 0, ONE/2, 0);
	UpdateObjectCoordinates2 ( &TheShip.rotate,
							&TheShip.position, &TheShip.coord);
	setVECTOR( &TheShip.rotate, 0, 0, 0); 

	TheShip.movementSpeed = 20;
	TheShip.rotationSpeed = 25;

	RegisterObjectIntoObjectArray(&TheShip);



	for (i = 0; i < MAX_CUBES; i++)
		{
		InitSingleObject(&TheCubes[i]);

		BringObjectToLife(&TheCubes[i], CUBE, 
			CUBE_MODEL_ADDRESS, 0, NONE);

		RegisterObjectIntoObjectArray(&TheCubes[i]);
		}

	for (i = 0; i < MAX_SQUARES; i++)
		{
		InitSingleObject(&TheSquares[i]);

		BringObjectToLife(&TheSquares[i], SQUARE, 
			SQUARE_MODEL_ADDRESS, 0, NONE);

		RegisterObjectIntoObjectArray(&TheSquares[i]);
		}

	for (i = 0; i < MAX_POLYGONS; i++)
		{
		InitSingleObject(&ThePolygons[i]);

		BringObjectToLife(&ThePolygons[i], POLYGON, 
			SQUARE_MODEL_ADDRESS, 0, NONE);

		RegisterObjectIntoObjectArray(&ThePolygons[i]);

		ThePolygons[i].alive = FALSE;
		}



	 
	LinkAllObjectsToModelsOrSprites();
	
	LinkAllObjectsToTheirCoordinateSystems();



	for (i = 0; i < MAX_CUBES; i++)
		{
		TheCubes[i].alive = FALSE;		// start off dead
		}
	for (i = 0; i < MAX_SQUARES; i++)
		{
		TheSquares[i].alive = FALSE;		// start off dead
		}
}







void InitialiseTheTunnel (void)
{
	InitTunnelToVoid();

		// main tunnel data initialisations
	NumberTunnelSections = 8;
	assert(NumberTunnelSections <= MAX_TUNNEL_SECTIONS);

	TunnelSectionShapeAngle = ONE / NUMBER_SHAPES_PER_SECTION;
	assert(TunnelSectionShapeAngle < ONE/4);		// must be less than PI/2

	TunnelSectionLength = SQUARE_SIZE * 8;
	TunnelSectionRadius = SQUARE_SIZE * 4;

		// HERE: create tunnel data itself

		// generate spline from list of rotation vectors
	CreateFirstSpline();
	
		// sort out rotations and coordinate transforms along tunnel length
	PropagateRotationsAlongTunnel(); 
	 
		// sort out where vertices of polygons should go
	CalculateCirclePoints();
}







void InitTunnelToVoid (void)
{
	int i, j;
	TunnelSection* thisSection;

	for (i = 0; i < MAX_TUNNEL_SECTIONS; i++)
		{
		thisSection = &TheTunnel[i];

		thisSection->id = i;

		setVECTOR( &thisSection->splinePoint, 0, 0, 0);

		thisSection->thetaX = 0;
		thisSection->thetaY = 0;

		GsInitCoordinate2( WORLD, &thisSection->coordinateData);
		GsInitCoordinate2( WORLD, &thisSection->circleCoordinates);

		for (j = 0; j < NUMBER_SHAPES_PER_SECTION; j++)
			{
			setVECTOR( &thisSection->pointsOnCircle[j], 0, 0, 0);
			}
		}
}



	// working out where the points on the circle are
	// these will be the vertices of the polygons that 
	// make up the tunnel walls

void CalculateCirclePoints (void)
{
	int i, j;
	VECTOR xAxis, yAxis;
	int angle;
	int circleX, circleY;
	VECTOR worldPosition;
	GsCOORDINATE2* coord;

	for (i = 0; i < NumberTunnelSections; i++)
		{
		coord = &TheTunnel[i].coordinateData;

			// get the x and y axes of this tunnel section
		setVECTOR( &xAxis, coord->coord.m[0][0], coord->coord.m[0][1], coord->coord.m[0][2]);
		setVECTOR( &yAxis, coord->coord.m[1][0], coord->coord.m[1][1], coord->coord.m[1][2]);

			// find x and y on circle, know x and y axes, hence get xyz in world
		for (j = 0; j < NUMBER_SHAPES_PER_SECTION; j++)
			{
			angle = j * TunnelSectionShapeAngle; 

			circleX = (TunnelSectionRadius * rcos(angle)) >> 12;
			circleY = (TunnelSectionRadius * rsin(angle)) >> 12;

			worldPosition.vx = TheTunnel[i].splinePoint.vx
								+ ( ((circleX * xAxis.vx) + (circleY * yAxis.vx)) >> 12);
			worldPosition.vy = TheTunnel[i].splinePoint.vy
								+ ( ((circleX * xAxis.vy) + (circleY * yAxis.vy)) >> 12);
			worldPosition.vz = TheTunnel[i].splinePoint.vz
								+ ( ((circleX * xAxis.vz) + (circleY * yAxis.vz)) >> 12);

			setVECTOR( &TheTunnel[i].pointsOnCircle[j], worldPosition.vx,	
				worldPosition.vy, worldPosition.vz);
			}
		}
}





	// knowing the spline points, find out all the rotations from one
	// section unto the next (as if hypothetical object flew along tunnel)

void PropagateRotationsAlongTunnel (void)
{
	int i;
	VECTOR *thisSplinePoint, *nextSplinePoint;		// spline points: world coords
	VECTOR worldVector;		// from this section spline point to the next, world coords
	VECTOR relativeVector;	  // from this section spline point to the next, section coords
	GsCOORDINATE2 *thisCoordSystem, *previousCoordSystem;
	TunnelSection *previousSection, *thisSection, *nextSection;
	int dx, dy, dz;
	SVECTOR rotationVector;


		// firstly, calculate the rotations from one section to the next
		// from positions of spline points:
		// rotations are Relative to current (i.e. not-yet-rotated, 
		// inherited-from-previous-section) coordinate system
	for (i = 0; i < NumberTunnelSections; i++)
		{
		thisSection = &TheTunnel[i];

		if (i == 0)			   // first tunnel section Always a straight
			{
			thisSection->thetaX = 0;
			thisSection->thetaY = 0;

				// needs no changes to its coordinate system
			continue;
			}

			// last section of tunnel: same coord system as one before
			// (no further spline point to curve towards)
			// i.e. effectively always a straight
		else if (i == NumberTunnelSections-1)
			{
			thisSection->thetaX = 0;
			thisSection->thetaY = 0;

				// no rotation, i.e. straight inheritance from previous section
			CopyCoordinateSystem( &TheTunnel[i-1].coordinateData, 
						&thisSection->coordinateData);
			CopyCoordinateSystem( &thisSection->coordinateData, 
								&thisSection->circleCoordinates);
			continue;
			}

		assert( i > 0 && i < NumberTunnelSections-1);
		previousSection = &TheTunnel[i-1];
		nextSection = &TheTunnel[i+1];
		
		thisCoordSystem = &thisSection->coordinateData;
		previousCoordSystem = &previousSection->coordinateData;

		thisSplinePoint = &thisSection->splinePoint;
		nextSplinePoint = &nextSection->splinePoint;

			// find vector from this section spline point to next,
			// in world coordinate terms; this vector expresses where the
			// next section start point is
		setVECTOR( &worldVector, nextSplinePoint->vx - thisSplinePoint->vx,
								nextSplinePoint->vy - thisSplinePoint->vy,
								nextSplinePoint->vz - thisSplinePoint->vz);

		//printf("world spline vector :-\n");
		//dumpVECTOR(&worldVector);

			// working out relative coords of spline point for this section:
			// still looking with same view as last section
		relativeVector.vx = ((worldVector.vx * previousCoordSystem->coord.m[0][0])
						+ (worldVector.vy * previousCoordSystem->coord.m[1][0])
						+ (worldVector.vz * previousCoordSystem->coord.m[2][0])) >> 12;
		relativeVector.vy = ((worldVector.vx * previousCoordSystem->coord.m[0][1])
						+ (worldVector.vy * previousCoordSystem->coord.m[1][1])
						+ (worldVector.vz * previousCoordSystem->coord.m[2][1])) >> 12;
		relativeVector.vz = ((worldVector.vx * previousCoordSystem->coord.m[0][2])
						+ (worldVector.vy * previousCoordSystem->coord.m[1][2])
						+ (worldVector.vz * previousCoordSystem->coord.m[2][2])) >> 12;

		//printf("relative spline vector :-\n");
		//dumpVECTOR(&relativeVector);

		dx = relativeVector.vx;
		dy = relativeVector.vy;
		dz = relativeVector.vz;
		//printf("dx %d dy %d dz %d\n", dx, dy, dz);

			// here: find thetaX and thetaY
		if (dx == 0)	   // no theta-y rotation
			{
			thisSection->thetaY = 0;
			if (dy == 0)		// no theta-x rotation: this section same coord as one before
				{
				//printf("\nno rotation at all\n\n");
				thisSection->thetaX = 0;
				thisSection->thetaY = 0;
				}
			else		 // just a theta-x rotation: find how much
				{
				assert(dz > 0);		// angle < PI/2

				thisSection->thetaX = rasin3(dy, TunnelSectionLength);
				//printf("just thetaX: %d\n", thisSection->thetaX);
				assert(abs(thisSection->thetaX) < ONE/4);		 // angle < PI/2
				}
			}
		else
			{
			if (dy == 0)		   // only theta-y rotation
				{
				thisSection->thetaX = 0;

				assert(dz > 0);		// angle < PI/2

				thisSection->thetaY = rasin3(dx, TunnelSectionLength);
				//printf("just thetaY: %d\n", thisSection->thetaY);
				assert(abs(thisSection->thetaY) < ONE/4);		 // angle < PI/2
				}
			else
				{
				assert(FALSE);		// NO COMPOUND ROTATION HANDLED YET: next stage
				}
			}

			// here: change the coordinate system for the section
		assert(abs(thisSection->thetaX) < ONE/4);
		assert(abs(thisSection->thetaY) < ONE/4);

		if (thisSection->thetaX == 0
			&& thisSection->thetaY == 0)
				{
				//printf("no rotating: copy coord\n");
					// no rotation, i.e. straight inheritance from previous section
				CopyCoordinateSystem(previousCoordSystem, thisCoordSystem);
				}
		else
			{
			setVECTOR( &rotationVector, thisSection->thetaX, thisSection->thetaY, 0);
			//printf("rotating coord now, by:\n");
			//dumpVECTOR(&rotationVector);
			DeriveNewCoordSystemFromRotation (previousCoordSystem,
					&rotationVector, thisCoordSystem);
			}

			// in all cases, make the virtual circle sit at the start
			// of the tunnel section
			// (CHANGE this later for smoother curvature)
		CopyCoordinateSystem(thisCoordSystem, &thisSection->circleCoordinates);

		//printf("section coord:-\n");
		//dumpCOORD2(thisCoordSystem);
		//printf("circle coord:-\n");
		//dumpCOORD2(&thisSection->circleCoordinates);
		}
}


  


	// NOTE: last rotation in rotationList will NOT be used

void GenerateSplineFromListOfRotations (int numberOfSections, SVECTOR* rotationList,
											VECTOR* outputSpline, int sectionDistance,
												VECTOR* startingPosition,
													MATRIX* startingMatrix)
{
	int i;
	GsCOORDINATE2 coord, temp;		 // tunnel coord system: moving ship
	SVECTOR* thisRotation;		   // one section to the next
	VECTOR *thisSplinePoint, *nextSplinePoint;		// world coords
	VECTOR worldVector;		// from above-former to above-latter, world coords

	assert(numberOfSections > 0);
	assert(numberOfSections < MAX_TUNNEL_SECTIONS);

	assert(sectionDistance > 0);

	GsInitCoordinate2(WORLD, &coord);
	GsInitCoordinate2(WORLD, &temp);

		// set the starting point for the entire tunnel
	setVECTOR(&outputSpline[0], startingPosition->vx,
					startingPosition->vy, startingPosition->vz);

		// copy the starting matrix: any orientation at start
	CopyMatrix(startingMatrix, &coord.coord);

	for (i = 0; i < numberOfSections-1; i++)
		{
		thisRotation = &rotationList[i];
		assert(thisRotation->vz == 0);		// z rotation not used here

		if (i == 0)		   // first tunnel section Must be a straight
			{
			assert(thisRotation->vx == 0 && thisRotation->vy == 0);
			}

		thisSplinePoint = &outputSpline[i];

		//printf("known spline point:-\n");
		//dumpVECTOR(thisSplinePoint);

		nextSplinePoint = &outputSpline[i+1];

		//printf("section coord BEFORE rotate:-\n");
		//dumpCOORD2(&coord);
		
			// find next section's coordinate system from rotations
			// order of rotations is Y then X
		DeriveNewCoordSystemFromRotation (&coord,
				thisRotation, &temp);
		CopyCoordinateSystem( &temp, &coord);

		//printf("rotation angle:-\n");
		//dumpVECTOR(thisRotation);
		//printf("section coord AFTER rotate:-\n");
		//dumpCOORD2(&coord);

			// the next spline point is sectionDistance away, 
			// directly ahead 
			// i.e. relative vector is always (0, 0, sectionDistance);
			// here we find this vector in world coord system
		setVECTOR( &worldVector, ((sectionDistance * coord.coord.m[2][0]) >> 12),
							((sectionDistance * coord.coord.m[2][1]) >> 12),
							((sectionDistance * coord.coord.m[2][2]) >> 12));

		//printf("world vector to next spline point:-\n");
		//dumpVECTOR(&worldVector);

			// now we know current point plus vector to next point:
			// hence find next point itself
		setVECTOR(nextSplinePoint, thisSplinePoint->vx + worldVector.vx,
						thisSplinePoint->vy + worldVector.vy,
						thisSplinePoint->vz + worldVector.vz);	
		}
}




void CreateFirstSpline (void)
{
	MATRIX matrix;
	SVECTOR rotationList[8];
	VECTOR splineCreated[8];
	int i;
	VECTOR start;

		// the rotations: one straight, then curve to the left
	setVECTOR( &rotationList[0], 0, 0, 0);
	for (i = 1; i < 8; i++)
		{
		setVECTOR( &rotationList[i], 0, ONE/16, 0);
		}

		// starting point
	setVECTOR( &start, 0, 0, 0);

	InitMatrix(&matrix);
	assert(MatrixIsUnitMatrix(&matrix));

	GenerateSplineFromListOfRotations (8, rotationList,
											splineCreated, TunnelSectionLength,
												&start, &matrix);

/***		// print spline
	for (i = 0; i < 8; i++)
		{
		dumpVECTOR( &splineCreated[i]);
		}
***/

		// assign to the tunnel
	for (i = 0; i < 8; i++)
		{
		setVECTOR( &TheTunnel[i].splinePoint, splineCreated[i].vx,
				splineCreated[i].vy, splineCreated[i].vz);
		}
}





void CreateTheTunnel (void)
{
	int i;
	int index;
	TunnelSection *thisSection, *nextSection;
	VECTOR *p1, *p2, *p3, *p4;
	TMD_VERT v1, v2, v3, v4;
	ObjectHandler* polygon;
	u_long address;
	TMD_NORM normal;
	VECTOR firstSide, secondSide, crossProduct;

		// i limits: don't link first and last section together
	for (i = 0; i < NumberTunnelSections-1; i++)
		{
		thisSection = &TheTunnel[i];
		nextSection = &TheTunnel[i+1];

		for (index = 0; index < NUMBER_SHAPES_PER_SECTION; index++)
			{
			if (index == NUMBER_SHAPES_PER_SECTION-1)
				{
				p1 = &thisSection->pointsOnCircle[index];
				p2 = &thisSection->pointsOnCircle[0];
				p3 = &nextSection->pointsOnCircle[index];
				p4 = &nextSection->pointsOnCircle[0];
				}
			else
				{
				p1 = &thisSection->pointsOnCircle[index];
				p2 = &thisSection->pointsOnCircle[index+1];
				p3 = &nextSection->pointsOnCircle[index];
				p4 = &nextSection->pointsOnCircle[index+1];
				}

			setVERTEX( &v1, p1->vx, p1->vy, p1->vz);
			setVERTEX( &v2, p2->vx, p2->vy, p2->vz);
			setVERTEX( &v3, p3->vx, p3->vy, p3->vz);
			setVERTEX( &v4, p4->vx, p4->vy, p4->vz);

				// side of poly from vertex 1 to vertex 2
			setVECTOR(&firstSide, p2->vx - p1->vx,
								p2->vy - p1->vy,
								p2->vz - p1->vz);

				// side of poly from vertex 2 to vertex 3
			setVECTOR(&secondSide, p3->vx - p2->vx,
								p3->vy - p2->vy,
								p3->vz - p2->vz);

			GetRightHandedNormal( &firstSide, &secondSide, &crossProduct);

			printf("i: %d, index: %d\n", i, index);
			dumpVECTOR(&crossProduct);
				
			setNORMAL( &normal, crossProduct.vx, 
				crossProduct.vy, crossProduct.vz);

			polygon = GetNextFreePolygon();
			assert(polygon != NULL);
		
			address = CreateAnotherPolygonTMD (0, 480, 10, 4, 
					0, 0,
					63, 0, 
					0, 63, 
					63, 63,
					&normal, 
					&v1, &v2, &v3, &v4);

			LinkObjectHandlerToSingleTMD (&polygon->handler, 
					address);
			polygon->alive = TRUE;
			}
		}	
}




	// here: could set subdivision for the polygons closest to the viewpoint
void UpdateTheTunnel (void)
{
	int i;

	for (i = 0; i < NumberTunnelSections; i++)
		{
		}
}






void CleanupAndExit (void)
{
	StoreScreen();

	ResetGraph(3);

	CleanUpSound();

	exit(1);

		// if this program part of a multiple module,
		// can printf to siocons to tell user to
		// invoke a new batch file, etc
}





void DealWithControllerPad (void)
{
	long pad;
	int controlSpeed;

	pad = PadRead();

	if (pad & PADselect && pad & PADstart)
		QuitFlag = TRUE;

	if (pad & PADstart)
		{
		for (;;)
			{
			pad = PadRead();
			if ( (pad & PADstart) == 0)
				break;
			}
		}

		// L1 and L2: step slowly
	if (pad & PADL1 && pad & PADL2)
		{
		int frameCount;	 
		frameCount = VSync(-1);
		while (frameCount + 25 > VSync(-1))
			{
			;
			}
		}

		// how fast controls act
	if (pad & PADselect)
		controlSpeed = 5;
	else
		controlSpeed = 1;

	if (pad & PADL2)
		{
		if (pad & PADRup)
			{
			ProjectionDistance += 5 * controlSpeed;
			GsSetProjection(ProjectionDistance);
			return;
			}
		if (pad & PADRdown)
			{
			ProjectionDistance -= 5 * controlSpeed;
			GsSetProjection(ProjectionDistance);
			return;
			}
		}
	
	if (pad & PADL1)
		{
		if (pad & PADRup)
			{
			TheView.vpy += 10 * controlSpeed;
			return;
			}
		if (pad & PADRdown)
			{
			TheView.vpy -= 10 * controlSpeed;
			return;
			}
		if (pad & PADR2)
			{
			TheView.vpz -= 10 * controlSpeed;
			return;
			}
		if (pad & PADR1)
			{
			TheView.vpz += 10 * controlSpeed;
			return;
			}
		if (pad & PADRleft)
			{
			TheView.vpx -= 10 * controlSpeed;
			return;
			}
		if (pad & PADRright)
			{
			TheView.vpx += 10 * controlSpeed;
			return;
			}
		}

		// movement
	if (pad & PADLup)
		TheShip.velocity.vz -= TheShip.movementSpeed * controlSpeed;
	if (pad & PADLdown)
		TheShip.velocity.vz += TheShip.movementSpeed * controlSpeed;

		// rotation
	if (pad & PADRup)
		TheShip.twist.vx += TheShip.rotationSpeed * controlSpeed;
	if (pad & PADRdown)
		TheShip.twist.vx -= TheShip.rotationSpeed * controlSpeed;
	if (pad & PADR2)
		TheShip.twist.vz -= TheShip.rotationSpeed * controlSpeed;
	if (pad & PADR1)
		TheShip.twist.vz += TheShip.rotationSpeed * controlSpeed;
	if (pad & PADRleft)
		TheShip.twist.vy -= TheShip.rotationSpeed * controlSpeed;
	if (pad & PADRright)
		TheShip.twist.vy += TheShip.rotationSpeed * controlSpeed;
}





void HandleAllObjects (void)
{
	ObjectHandler* object;
	int i;

		// call these two before or after the main business?
	CheckCollisions();

	UpdateTheTunnel();

	for (i = 0; i < MAX_OBJECTS; i++)
		{
		if (ObjectArray[i] != NULL)
			{
			if (ObjectArray[i]->alive == TRUE)
				{
				object = ObjectArray[i];

					// updating common to all object types
				object->lifeTime++;
				if (object->canFire == TRUE)
					object->framesSinceLastFire++;
				// HandleCommonUpdating(object);

				switch(object->type)
					{
					case SHIP:
						HandleTheShip(object);
						break;
					case CUBE:
						HandleACube(object);
						break;
					case SQUARE:
						HandleASquare(object);
						break;
					case POLYGON:
						HandleAPolygon(object);
						break;
					default:
						assert(FALSE);
					}
				}
			}
		}
}




void HandleTheShip (ObjectHandler* object)
{
	assert(object->type == SHIP);

	UpdateObjectCoordinates(&object->twist, &object->position, 
								&object->velocity, &object->coord, &object->matrix);

	//KeepWithinRange(object->position.vx, -SQUARE_SIZE*8, SQUARE_SIZE*8);
	//KeepWithinRange(object->position.vy, -SQUARE_SIZE*8, SQUARE_SIZE*8);
	//KeepWithinRange(object->position.vz, -SQUARE_SIZE*8, SQUARE_SIZE*8);

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






void HandleACube (ObjectHandler* object)
{
	assert(object->type == CUBE);

	UpdateObjectCoordinates2 ( &object->rotate,
							&object->position, &object->coord);
}



	// NULL FUNCTION
void HandleASquare (ObjectHandler* object)
{
	assert(object->type == SQUARE);
}


	// NULL FUNCTION	
void HandleAPolygon (ObjectHandler* object)
{
	assert(object->type == POLYGON);
}





void CheckCollisions (void)
{
	int i, j;

	for (i = 0; i < MAX_CUBES; i++)
		{
		if (TheCubes[i].alive == TRUE)
			{
			for (j = 0; j < MAX_SQUARES; j++)
				{
				if (TheSquares[j].alive == TRUE)
					{
					if (ObjectsVeryClose(&TheCubes[i], &TheSquares[j]))
						{
						//printf("cube %d and square %d collision\n", i, j);
						TheCubes[i].currentHealth--;
						TheSquares[j].currentHealth--;
						}
					}
				}
			}
		}
}






	// extremely simple and crude collision detection

#define APPROXIMATE_RADIUS 32

int ObjectsVeryClose (ObjectHandler* first, ObjectHandler* second)
{
	if (abs(first->coord.coord.t[0] - second->coord.coord.t[0]) > APPROXIMATE_RADIUS)
		return FALSE;
	if (abs(first->coord.coord.t[1] - second->coord.coord.t[1]) > APPROXIMATE_RADIUS)
		return FALSE;
	if (abs(first->coord.coord.t[2] - second->coord.coord.t[2]) > APPROXIMATE_RADIUS)
		return FALSE;

	return TRUE; 
}





	// this function does object-relative movement and rotation
void UpdateObjectCoordinates (VECTOR* twist,
							VECTOR* position, VECTOR* velocity,
							GsCOORDINATE2* coordSystem, MATRIX* matrix)
{	
	VECTOR realMovement;
	MATRIX xMatrix, yMatrix, zMatrix;
	SVECTOR xVector, yVector, zVector;
	
		// find the object-local velocity in super coordinate terms
	ApplyMatrixLV(matrix, velocity, &realMovement);

		// update position by actual movement
	position->vx += realMovement.vx;
	position->vy += realMovement.vy;
	position->vz += realMovement.vz;

	if (twist->vx != 0)
		{
		xVector.vx = twist->vx;
		xVector.vy = 0;
		xVector.vz = 0;
			
		RotMatrix(&xVector, &xMatrix);

		MulMatrix0(matrix, &xMatrix, matrix);
		}
	else if (twist->vy != 0)
		{
		yVector.vx = 0;
		yVector.vy = twist->vy;
		yVector.vz = 0;
			
		RotMatrix(&yVector, &yMatrix);

		MulMatrix0(matrix, &yMatrix, matrix);
		}
	else if (twist->vz != 0)
		{
		zVector.vx = 0;
		zVector.vy = 0;
		zVector.vz = twist->vz;
			
		RotMatrix(&zVector, &zMatrix);

		MulMatrix0(matrix, &zMatrix, matrix);
		}
	 
	coordSystem->coord = *matrix;
	
		// set position absolutely	
	coordSystem->coord.t[0] = position->vx;
	coordSystem->coord.t[1] = position->vy;
	coordSystem->coord.t[2] = position->vz;

		// tell GTE that coordinate system has been updated
	coordSystem->flg = 0;
}








	// this does world-relative movement and rotation
void UpdateObjectCoordinates2 (SVECTOR* rotationVector,
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






void DeriveNewCoordSystemFromRotation (GsCOORDINATE2* original,
			SVECTOR* rotation, GsCOORDINATE2* output)
{
	SVECTOR realRotation;
	MATRIX rotationMatrix;
	SVECTOR xVector, yVector;

		// z component of rotation has no place in cylindrical tunnel
	setVECTOR( &realRotation, rotation->vx, rotation->vy, 0);

	if (realRotation.vx == 0)		 // just a theta-Y rotation
		{
		RotMatrix(&realRotation, &rotationMatrix);
		MulMatrix0(&original->coord, &rotationMatrix, &output->coord);
		}
	else
		{
		if (realRotation.vy == 0)		 // just a theta-X rotation
			{
			RotMatrix(&realRotation, &rotationMatrix);
			MulMatrix0(&original->coord, &rotationMatrix, &output->coord);
			}
		else	 // compound rotation: first by Y, then by X
			{
			setVECTOR( &yVector, 0, realRotation.vy, 0);	   // just y rotation
			RotMatrix(&yVector, &rotationMatrix);
			MulMatrix0(&original->coord, &rotationMatrix, &output->coord);

			setVECTOR( &xVector, realRotation.vx, 0, 0);	  // just x rotation
			RotMatrix(&xVector, &rotationMatrix);
			MulMatrix0(&output->coord, &rotationMatrix, &output->coord);
			}
		}

		// tell GTE that coordinate system has been updated
	output->flg = 0;
}





	// sort out the conversion of coordinates from local->world->screen			 
	// for objects that are displayed by GsSPRITE rather than GsDOBJ2
u_short SortSpriteObjectPosition (ObjectHandler* object)
{
	VECTOR screen;
	int visualX, visualY;
	u_short zValue;

	PushMatrix();

	ApplyMatrixLV( &GsWSMATRIX, &object->position, &screen);

	screen.vx += GsWSMATRIX.t[0];
	screen.vy += GsWSMATRIX.t[1];
	screen.vz += GsWSMATRIX.t[2];

	if (screen.vz == 0)			   // prevent division by zero
		return 0;	   // right at the front

	visualX = object->sprite.w * ProjectionDistance / screen.vz;	
	visualY = object->sprite.h * ProjectionDistance / screen.vz;	

	object->sprite.x = ((screen.vx * ProjectionDistance / screen.vz) - visualX/2);
	object->sprite.y = ((screen.vy * ProjectionDistance / screen.vz) - visualY/2);

	object->sprite.scalex = object->scaleX * ProjectionDistance / screen.vz; 
	object->sprite.scaley = object->scaleY * ProjectionDistance / screen.vz; 

	PopMatrix();

		// 3D clipping
	if (screen.vz < ProjectionDistance/2)
		object->sprite.attribute |= GsDOFF;			 // display off
	else
		TURN_NTH_BIT_OFF(object->sprite.attribute, 32, 31)		// ensure 31st bit is OFF

	zValue = (u_short) (screen.vz >> 6);	 // NOTE: need to adjust this
											// just a blagg to make it look right

	return zValue;			   // NOT YET CORRECT
}






#define STORING_SCREEN 0

void StoreScreen (void)
{
#if STORING_SCREEN
	u_long* destination;
	int x, y, w, h;
	RECT rect;

	destination = (u_long *) SCREEN_SAVE_ADDRESS;

	x = y = 0;		// top left of frame buffer

	w = ScreenWidth;
	h = ScreenHeight;

	*(destination+0) = 0x00000010;		/* ID */
	*(destination+1) = 0x00000002;		/* FLAG(15bit Direct,No Clut) */
	*(destination+2) = (w*h/2+3)*4;		/* pixel bnum */
	*(destination+3) = ((0 & 0xffff) << 16) | (640 & 0xffff);
						/* pixel DX,DY: at 640, 0 */
	*(destination+4) = ((h & 0xffff) << 16) | (w & 0xffff);
						/* pixel W,H */

	// NO CLUT since 16-bit mode used

	rect.x = x; 
	rect.y = y;
	rect.w = w; 
	rect.h = h;
	DrawSync(0);
	StoreImage(&rect, destination+5);		

	printf("\n\nPress [F10][F4] for dsave, to get screen picture\n"); 
	printf("Dsave[0]: filename %08x %x\n\n\n", destination, (w*h/2+5)*4);

	DrawSync(0);
	VSync(0);
#endif
}




	
int CheckCoordinateSystemIsBase (GsCOORDINATE2* coord)
{
	if (coord->super != WORLD)
		return FALSE;

	if (MatrixIsUnitMatrix( &coord->coord) == FALSE)
		return FALSE;

	return TRUE;
}
	






void CopyCoordinateSystem (GsCOORDINATE2* from, GsCOORDINATE2* to)
{
	to->super = from->super;

	CopyMatrix( &from->coord, &to->coord);

	to->flg = 0;
}
	







	// once the TMD has been created, can be used in the usual way
	// e.g. Linked using the object.c interface ....
void CreateExtraTMDs (void)
{
	TMD_NORM nm;
	TMD_VERT v0, v1, v2, v3;

		// make a single-polygon textured TMD
	setNORMAL(&nm, 0, ONE, 0);

	setVECTOR(&v0, 0,    0, 0); 		
	setVECTOR(&v1, 1024,    0, 1024); 
	setVECTOR(&v2, 1024, 0, 1024); 		
	setVECTOR(&v3, 1024, 0, 1024);

	makeTMD_F_4T(CreatedTMDPointer, 30720, 10, 
		0, 0, 
		0, 63, 
		63, 0, 
		63, 63, 
		&nm, &v0, &v1, &v2, &v3);
}






ObjectHandler* GetNextFreePolygon (void)
{
	ObjectHandler* polygon = NULL;
	int i;

	for (i = 0; i < MAX_POLYGONS; i++)
		{
		if (ThePolygons[i].alive == FALSE)
			{
			polygon = &ThePolygons[i];
			break;
			}
		}

	return polygon;
}





u_long CreateAnotherPolygonTMD (int clutX, int clutY, int tPageID, int pixelMode, 
					u_char u0, u_char v0,
					u_char u1, u_char v1, 
					u_char u2, u_char v2, 
					u_char u3, u_char v3,
					TMD_NORM* norm0, 
					TMD_VERT* vert0, 
					TMD_VERT* vert1, 
					TMD_VERT* vert2, 
					TMD_VERT* vert3)
{
	u_long address;

	address = CurrentTMDStackAddress;

	assert(address < 0x80140000);		// start of program itself ...

	CreateSimpleTMD ( (u_long*)address, clutX, clutY, tPageID,
				 pixelMode, 
				 u0, v0,
				 u1, v1, 
				 u2, v2, 
				 u3, v3,
				 norm0, 
				 vert0, 
				 vert1, 
				 vert2, 
				 vert3);

	NumberOfCreatedTMDs++;

	CurrentTMDStackAddress += SIZEOF_SINGLE_POLYGON_TMD;

	return address;
}




		 