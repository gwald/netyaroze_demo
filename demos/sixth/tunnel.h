#include <libps.h>

#include <stdio.h>

#include "asssert.h"

#include "general.h"

#include "dump.h"

#include "tmd.h"

#include "object.h"

#include "tuto1.h"







	// tunnel constants

#define Y_ROTATION_TOLERANCE_ERROR 25
#define X_ROTATION_TOLERANCE_ERROR 25

#define ANGLE_CHANGE 1




#define MAX_TUNNEL_SECTIONS 16

extern int NumberTunnelSections;

#define NUMBER_SHAPES_PER_SECTION 12
extern int TunnelSectionShapeAngle;

extern int TunnelSectionLength;
extern int TunnelSectionRadius;

typedef struct
{
	int id;

	VECTOR splinePoint;		  // in world coords: the spline point
	VECTOR sectionCentre;		  // in world coords
	
	int thetaX, thetaY;	 	  // rotation angles from previous section
								// NOTE: rotation order FIXED at (y then x)

	GsCOORDINATE2 coordinateData;	   // own coords

	GsCOORDINATE2 circleCoordinates;	  // virtual circle: vertices for tunnel polygons

		// vertices on circumference of circle: vertices in world coords
	VECTOR pointsOnCircle[NUMBER_SHAPES_PER_SECTION];
} TunnelSection;

extern TunnelSection TheTunnel[MAX_TUNNEL_SECTIONS];







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

void CalculateCirclePoints (void);

void PropagateRotationsAlongTunnel (void);		  			

void FindCompoundRotationAngles (int dx, int dy, int dz, int* thetaX, int* thetaY);

void GenerateSplineFromListOfRotations (int numberOfSections, SVECTOR* rotationList,
											VECTOR* outputSpline, int sectionDistance,
												VECTOR* startingPosition,
													MATRIX* startingMatrix);


void CreateFirstSpline (void);

void FindSplineCentralPoints (void);

void CreateTheTunnel (void);

void UpdateTheTunnel (void);

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
