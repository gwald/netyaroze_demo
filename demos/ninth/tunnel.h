#include <libps.h>

#include <stdio.h>

#include "asssert.h"

#include "general.h"

#include "dump.h"

#include "trig.h"

#include "tmd.h"

#include "object.h"

#include "vector.h"

#include "matrix.h"

#include "coord.h"

#include "tuto1.h"







	// main tunnel data
	  
#define MAX_TUNNEL_SECTIONS 64

#define NUMBER_SHAPES_PER_SECTION 12

#define NUMBER_SECTIONS_ALIVE 8



	// when updating ships
#define SECTION_LOOKAHEAD 3

	// this one MUST be at least (2 * SECTION_LOOKAHEAD + 1)
#define MAX_SECTIONS_CONSIDERED 7

#define MAX_TO_CHANGE_LIVING_STATUS 8

#define SUBDIVISION_LOOKAHEAD 2

#define SUBDIVISION_EXTENT 3







typedef struct
{
	int id;

	int radius, length;

	VECTOR splinePoint;		  // in world coords: the spline point
	VECTOR splineVector;
	VECTOR sectionCentre;		  // in world coords
	
	int thetaX, thetaY;	 	  // rotation angles from previous section
								// NOTE: rotation order FIXED at (y then x)

	GsCOORDINATE2 coordinateData;	   // own coords

	GsCOORDINATE2 circleCoordinates;	  // virtual circle: vertices for tunnel polygons

		// vertices on circumference of circle: vertices in world coords
	VECTOR pointsOnCircle[NUMBER_SHAPES_PER_SECTION];
	VECTOR polygonCentres[NUMBER_SHAPES_PER_SECTION];
	VECTOR normalsToPolygons[NUMBER_SHAPES_PER_SECTION];
	int polygonIDs[NUMBER_SHAPES_PER_SECTION];
} TunnelSection;

extern TunnelSection TheTunnel[MAX_TUNNEL_SECTIONS];


extern int NumberTunnelSections;

extern int TunnelSectionShapeAngle;

extern int TunnelSectionLength;
extern int TunnelSectionRadius;






	// ways to describe the tunnel
#define SPLINE_POINTS 0
#define SPLINE_VECTORS 1
#define SECTION_ROTATIONS 2


	// error limits: when finding rotations from spline points
#define Y_ROTATION_TOLERANCE_ERROR 25
#define X_ROTATION_TOLERANCE_ERROR 25

#define ANGLE_CHANGE 1


	// checking a given spline
#define SPLINE_POINT_DISTANCE_TOLERANCE 35


#define MAXIMUM_SECTION_ROTATION (ONE/8)

#define MAXIMUM_PURE_X_ROTATION (ONE/8)
#define MAXIMUM_PURE_Y_ROTATION (ONE/8)
#define MAXIMUM_MIXED_X_ROTATION (ONE/16)
#define MAXIMUM_MIXED_Y_ROTATION (ONE/16)



	// ways to move along a tunnel
#define FORWARDS 0
#define BACKWARDS 1



	// polygon management

#define CREATED_TMD_ADDRESS 0x80095000
extern u_long* CreatedTMDPointer;


#define START_OF_CREATED_TMDS_STACK	0x80096000
#define END_OF_CREATED_TMDS_STACK 0x8013f000
extern int NumberOfCreatedTMDs;
extern u_long CurrentTMDStackAddress;
#define SIZEOF_SINGLE_POLYGON_TMD 0x00000100






		// prototypes


void InitialiseTheTunnel (void);



void InitTunnelToVoid (void);

void SetBasicTunnelParameters (void);





void CalculateCirclePointsPolygonSidesCentresAndNormals (void);




void PropagateRotationsAlongTunnel (void);		  			

void FindCompoundRotationAngles (int dx, int dy, int dz, int* thetaX, int* thetaY);




void GenerateSplineFromListOfRotations (int numberOfSections, 
		SVECTOR* rotationList, VECTOR* outputSpline, 
		int sectionDistance, VECTOR* startingPosition,
			MATRIX* startingMatrix);


void CreateFirstSpline (void);





void FindSplineCentralPoints (void);





void CreateTheTunnel (void);





void UpdateTheTunnel (void);






int GetNextFreePolygonID (void);

void CountNumberLivingPolys	(int* nonDivided, int* divided);



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


void ProperCreateFirstTunnel (void);



void CalculateSplineVectorsFromSplinePoints (void);

void CalculateSplinePointsFromSplineVectors (void);



void SortTunnelMainDescriptionsFromSplinePoints (void);

void SortTunnelMainDescriptionsFromRotationsList (void);





void VerifySplinePointDescription (void);
void VerifySplineVectorDescription (void);
void VerifySectionRotationDescription (void);

int TestIfTunnelIsCircular (void);


void PrintTunnelSplinePoints (void);
void PrintTunnelSplineVectors (void);
void PrintTunnelSectionRotations (void);



void FleshOutTunnelFromItsDescriptions (void);




void PositionObjectInTunnelSection (ObjectHandler* object, 
							int whichSection, VECTOR* offset);
void BringAliveLocalTunnelAroundShip (ObjectHandler* object);




void HandleShipsMovementInTunnel (ObjectHandler* object);

void UpdateTunnelPolygonLivingStatus (int firstID, 
									int nextID, int direction);

void UpdateTunnelPolygonLivingStatus2 (int viewSectionBefore, int viewSectionAfter, 
			int directionBefore, int directionAfter);

void UpdateTunnelSubdivision (int startViewSection, 
		int nextViewSection, int direction);

void UpdateTunnelSubdivision2 (int viewSectionBefore, 
									int viewSectionAfter);

void DealWithShipRotation (VECTOR* twist,
							GsCOORDINATE2* coordSystem, MATRIX* matrix);

int FindWhichWayShipMovesInSection (ObjectHandler* object, int section);



TunnelSection* FindClosestSectionToPoint (VECTOR* position, 
								int originalSection);

int FindSectionPointIsIn (VECTOR* point, int mainGuess);

int FindViewPointSection (int guess);


int FindWhichWayShipPointsInSection (ObjectHandler* object, 
							TunnelSection* section);

void FindPositionAndDirectionOfView (VECTOR* position, 
								VECTOR* direction);

void FindVisibleSectionEnds (int section, int* end1, int* end2);

int FindWhichWayViewLooksInSection (int section);


int GetRelativeSection (int first, int numberOn, int direction);
int GetDisplacedSection (int current, int N);
int ConvertIdToRealID (int id);

int DistanceBetweenSections (int first, 
						int second, int direction);


	

int DetectCollisionWithTunnel (ObjectHandler* object, 
			int nextSection, VECTOR* newPos);		
			
			
void HandleCollisionWithTunnel (ObjectHandler* object);
