#include "tunnel.h"





int NumberOfCreatedTMDs = 0;
u_long CurrentTMDStackAddress = START_OF_CREATED_TMDS_STACK;

u_long* CreatedTMDPointer = (u_long*) CREATED_TMD_ADDRESS;



int NumberTunnelSections;

int TunnelSectionShapeAngle;

int TunnelSectionLength;

int TunnelOuterRadius;
int TunnelMiddleRadius;
int TunnelInnerRadius;



	// three ways of describing the tunnel

int TunnelBaseDescription;

VECTOR TunnelSplinePoints[MAX_TUNNEL_SECTIONS];
VECTOR TunnelSplineVectors[MAX_TUNNEL_SECTIONS];
SVECTOR TunnelSectionRotations[MAX_TUNNEL_SECTIONS];


	// starting conditions
VECTOR TunnelStartPoint;
MATRIX TunnelInitialOrientation;



TunnelSection TheTunnel[MAX_TUNNEL_SECTIONS];



#define setVECTOR(vector, x, y, z)				\
				(vector)->vx = (x), (vector)->vy = (y), (vector)->vz = (z)	

#define setNORMAL(normal, x, y, z)				\
				(normal)->nx = (x), (normal)->ny = (y), (normal)->nz = (z)
				
#define setVERTEX(vertex, x, y, z)				\
				(vertex)->vx = (x), (vertex)->vy = (y), (vertex)->vz = (z)	









   
void InitialiseTheTunnel (void)
{
	InitTunnelToVoid();

	SetBasicTunnelParameters();

	ProperCreateFirstTunnel();




#if 0				// OLD AND DODGY
		// main tunnel data initialisations
	TunnelSectionShapeAngle = ONE / NUMBER_SHAPES_PER_SECTION;
	assert(TunnelSectionShapeAngle < ONE/4);		// must be less than PI/2

	TunnelSectionLength = SQUARE_SIZE * 16;
	TunnelOuterRadius = SQUARE_SIZE * 8;



  
		// HERE: create tunnel data itself

		// generate spline from list of rotation vectors
	CreateFirstSpline();

	FindSplineCentralPoints();
	
		// sort out rotations and coordinate transforms along tunnel length
	PropagateRotationsAlongTunnel(); 
	 
		// sort out where vertices of polygons should go
	CalculateCirclePoints();
#endif
}






	// clear all tunnel data
void InitTunnelToVoid (void)
{
	int i, j;
	TunnelSection* thisSection;

	TunnelBaseDescription = -1;

	NumberTunnelSections = -1;
	TunnelSectionShapeAngle = -1;
	TunnelSectionLength = -1;
	TunnelOuterRadius = -1;

	setVECTOR( &TunnelStartPoint, 0, 0, 0);
	InitMatrix( &TunnelInitialOrientation);

	for (i = 0; i < MAX_TUNNEL_SECTIONS; i++)
		{
		thisSection = &TheTunnel[i];

		thisSection->id = i;

		thisSection->radius = -1;
		thisSection->length = -1;

		setVECTOR( &thisSection->splinePoint, 0, 0, 0);
		setVECTOR( &thisSection->splineVector, 0, 0, 0);
		setVECTOR( &thisSection->sectionCentre, 0, 0, 0);

		thisSection->thetaX = 0;
		thisSection->thetaY = 0;

		GsInitCoordinate2( WORLD, &thisSection->coordinateData);
		GsInitCoordinate2( WORLD, &thisSection->circleCoordinates);

		for (j = 0; j < NUMBER_SHAPES_PER_SECTION; j++)
			{
			setVECTOR( &thisSection->pointsOnCircle[j], 0, 0, 0);
			setVECTOR( &thisSection->polygonSidesOnCircle[j], 0, 0, 0);
			setVECTOR( &thisSection->polygonCentres[j], 0, 0, 0);
			setVECTOR( &thisSection->normalsToPolygons[j], 0, 0, 0);
			thisSection->polygonIDs[j] = -1;
			}

		setVECTOR( &TunnelSplinePoints[i], 0, 0, 0);
		setVECTOR( &TunnelSplineVectors[i], 0, 0, 0);
		setVECTOR( &TunnelSectionRotations[i], 0, 0, 0);
		}
}





void SetBasicTunnelParameters (void)
{
	int i;
	
		// number polygons per section determines angular smoothness
	TunnelSectionShapeAngle = ONE / NUMBER_SHAPES_PER_SECTION;
	assert(TunnelSectionShapeAngle < ONE/4);		// must be less than PI/2

		// two main dimensions
	TunnelSectionLength = SQUARE_SIZE * 16;
	TunnelOuterRadius = SQUARE_SIZE * 8;

	TunnelInnerRadius 
		= (TunnelOuterRadius * rcos(TunnelSectionShapeAngle/2) ) / ONE;
	TunnelMiddleRadius = (TunnelOuterRadius + TunnelInnerRadius) / 2;

	printf("Main tunnel dimensions :-\n");
	printf("section shape angle %d\n", TunnelSectionShapeAngle);
	printf("section length %d\n", TunnelSectionLength);
	printf("outer radius: %d, inner radius: %d, middle radius: %d\n", 
		TunnelOuterRadius, TunnelInnerRadius, TunnelMiddleRadius);

	for (i = 0; i < MAX_TUNNEL_SECTIONS; i++)
		{
		TheTunnel[i].radius = TunnelOuterRadius;
		TheTunnel[i].length = TunnelSectionLength;
		}
}






	// working out where the points on the circle are
	// these will be the vertices of the polygons that 
	// make up the tunnel walls

void CalculateCirclePointsPolygonSidesCentresAndNormals (void)
{
	int i, j;
	VECTOR xAxis, yAxis;
	int angle;
	int circleX, circleY;
	VECTOR worldPosition;
	GsCOORDINATE2* coord;
	VECTOR *point, *thisPoint, *nextPoint;
	VECTOR *polygonSide, *polygonCentre, *polygonNormal, *sectionCentre;
	TunnelSection *thisSection, *nextSection;
	VECTOR *p1, *p2, *p3, *p4;

	for (i = 0; i < NumberTunnelSections-1; i++)
		{
		thisSection = &TheTunnel[i];
		coord = &thisSection->coordinateData;

			// get the x and y axes of this tunnel section, in world coord terms
		setVECTOR( &xAxis, coord->coord.m[0][0], coord->coord.m[0][1], coord->coord.m[0][2]);
		setVECTOR( &yAxis, coord->coord.m[1][0], coord->coord.m[1][1], coord->coord.m[1][2]);

			// find x and y on circle, know x and y axes, hence get circle points in world
		for (j = 0; j < NUMBER_SHAPES_PER_SECTION; j++)
			{
			angle = j * TunnelSectionShapeAngle; 
			point = &thisSection->pointsOnCircle[j];

			circleX = (TunnelOuterRadius * rcos(angle)) >> 12;	  // x = r.cos(theta)
			circleY = (TunnelOuterRadius * rsin(angle)) >> 12;	  // y = r.sin(theta)

			worldPosition.vx = thisSection->splinePoint.vx
								+ ( ((circleX * xAxis.vx) + (circleY * yAxis.vx)) >> 12);
			worldPosition.vy = thisSection->splinePoint.vy
								+ ( ((circleX * xAxis.vy) + (circleY * yAxis.vy)) >> 12);
			worldPosition.vz = thisSection->splinePoint.vz
								+ ( ((circleX * xAxis.vz) + (circleY * yAxis.vz)) >> 12);

			setVECTOR(point, worldPosition.vx,	
				worldPosition.vy, worldPosition.vz);
			}
		}


		// now that points on circle are known, find polygon sides, centres,
		// and normals
	for (i = 0; i < NumberTunnelSections; i++)
		{
		if (i == NumberTunnelSections-1)
			{
			thisSection = &TheTunnel[i];
			nextSection = &TheTunnel[0];
			}
		else
			{
			thisSection = &TheTunnel[i];
			nextSection = &TheTunnel[i+1];
			}

		for (j = 0; j < NUMBER_SHAPES_PER_SECTION; j++)
			{
			point = &thisSection->pointsOnCircle[j];
			if (j == NUMBER_SHAPES_PER_SECTION-1)
				nextPoint = &thisSection->pointsOnCircle[0];
			else
				nextPoint = &thisSection->pointsOnCircle[j+1];

			polygonSide = &thisSection->polygonSidesOnCircle[j];
			polygonCentre = &thisSection->polygonCentres[j];
			polygonNormal = &thisSection->normalsToPolygons[j];
			sectionCentre = &thisSection->sectionCentre;

				// find polygon sides on circle: this point to next point
			setVECTOR(polygonSide, nextPoint->vx - thisPoint->vx,
						nextPoint->vy - thisPoint->vy,
						nextPoint->vz - thisPoint->vz);

			if (j == NUMBER_SHAPES_PER_SECTION-1)
				{
				p1 = &thisSection->pointsOnCircle[j];
				p2 = &thisSection->pointsOnCircle[0];
				p3 = &nextSection->pointsOnCircle[j];
				p4 = &nextSection->pointsOnCircle[0];
				}
			else
				{
				p1 = &thisSection->pointsOnCircle[j];
				p2 = &thisSection->pointsOnCircle[j+1];
				p3 = &nextSection->pointsOnCircle[j];
				p4 = &nextSection->pointsOnCircle[j+1];
				}

				// find centre of polygon
			setVECTOR(polygonCentre, ((p1->vx + p2->vx + p3->vx + p4->vx) / 4),
									((p1->vy + p2->vy + p3->vy + p4->vy) / 4),
									((p1->vz + p2->vz + p3->vz + p4->vz) / 4) );

				// calculate normals: from centre of poly to centre of section
				// ==> poly always visible from inside section
			setVECTOR(polygonNormal, sectionCentre->vx - polygonCentre->vx,
							sectionCentre->vy - polygonCentre->vy,
							sectionCentre->vz - polygonCentre->vz);
			ScaleVectorToUnit (polygonNormal);
			//printf("normal :-\n");
			//dumpVECTOR(&polygonNormal);
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
	GsCOORDINATE2 temp;
	SVECTOR rotationNextFromThis, rotationPreviousFromThis;


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

			thisSection->coordinateData.coord.t[0] = thisSection->splinePoint.vx;
			thisSection->coordinateData.coord.t[1] = thisSection->splinePoint.vy;
			thisSection->coordinateData.coord.t[2] = thisSection->splinePoint.vz;
			thisSection->coordinateData.flg = 0;
			continue;
			}

			// last section of tunnel: same coord system as one before
			// (no further spline point to curve towards)
			// i.e. effectively always a straight
		else if (i == NumberTunnelSections-1)
			{
			thisSection->thetaX = 0;
			thisSection->thetaY = 0;

				// no rotation, i.e. same coord system as previous section
			CopyCoordinateSystem( &TheTunnel[i-1].coordinateData, 
						&thisSection->coordinateData);
			CopyCoordinateSystem( &thisSection->coordinateData, 
								&thisSection->circleCoordinates);
									
			thisSection->coordinateData.coord.t[0] = thisSection->splinePoint.vx;
			thisSection->coordinateData.coord.t[1] = thisSection->splinePoint.vy;
			thisSection->coordinateData.coord.t[2] = thisSection->splinePoint.vz;
			thisSection->coordinateData.flg = 0;
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
		//printf("previous section coord:-\n");
		//dumpCOORD2(previousCoordSystem);

			// working out relative coords of spline point for this section:
			// still looking with same view as last section
		ConvertPointWorldToObject (&worldVector, 
						previousCoordSystem, &relativeVector);
	
		//printf("relative spline vector :-\n");
		//dumpVECTOR(&relativeVector);

		dx = relativeVector.vx;
		dy = relativeVector.vy;
		dz = relativeVector.vz;
		//printf("dx %d dy %d dz %d\n", dx, dy, dz);

			// here: find thetaX and thetaY
		if (abs(dx) < X_ROTATION_TOLERANCE_ERROR)	// no theta-y rotation
			{
			thisSection->thetaY = 0;
			if (abs(dy) < Y_ROTATION_TOLERANCE_ERROR)		// no theta-x rotation: this section same coord as one before
				{
				//printf("\nno rotation at all\n\n");
				thisSection->thetaX = 0;
				thisSection->thetaY = 0;
				}
			else		 // just a theta-x rotation (up/down)
				{
				assert(dz > 0);		// angle < PI/2

					// minus sign: way that thetaX lies on yz plane
				thisSection->thetaX = -rasin3(dy, TunnelSectionLength);
				//printf("just thetaX: %d\n", thisSection->thetaX);
				}
			}
		else
			{
			if (abs(dy) < Y_ROTATION_TOLERANCE_ERROR)		   // only theta-y rotation (left/right)
				{
				thisSection->thetaX = 0;

				assert(dz > 0);		// angle < PI/2

					// minus sign: way that thetaY lies on xz plane
					// i.e. turn to the left is +ve thetaY rotation
				thisSection->thetaY = -rasin3(dx, TunnelSectionLength);
				//printf("just thetaY: %d\n", thisSection->thetaY);
				}
			else		// need to rotate by both
				{
				FindCompoundRotationAngles(dx, dy, dz,
						&thisSection->thetaX, &thisSection->thetaY);
				#if 0
				printf("BOTH: thetaX %d, thetaY %d\n", 
								thisSection->thetaX, thisSection->thetaY);
				#endif
				}
			}

			// here: change the coordinate system for the section
		assert(abs(thisSection->thetaX) < ONE/4);		 // angle < PI/2
		assert(abs(thisSection->thetaY) < ONE/4);		 // angle < PI/2

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

		thisCoordSystem->coord.t[0] = thisSplinePoint->vx;
		thisCoordSystem->coord.t[1] = thisSplinePoint->vy;
		thisCoordSystem->coord.t[2] = thisSplinePoint->vz;
		thisCoordSystem->flg = 0; 

		// at this point: should check that next spline point
		// is now virtually dead ahead

			// in all cases, make the virtual circle sit at the start
			// of the tunnel section
			// (CHANGE this later for smoother curvature)
		CopyCoordinateSystem(thisCoordSystem, &thisSection->circleCoordinates);



		printf("section coord:-\n");
		dumpCOORD2(thisCoordSystem);
		//printf("circle coord:-\n");
		//dumpCOORD2(&thisSection->circleCoordinates);
		}
}






	// from our viewpoint, next spline point on neither x or y axes,
	// find thetaY and thetaX such that those rotations (order: y then x)
	// will take us to that point
	// Note: only approximate

void FindCompoundRotationAngles (int dx, int dy, int dz, int* thetaX, int* thetaY)
{
	int angleX = 0, angleY = 0;
	int newDX, newDY;
	GsCOORDINATE2 coord, temp, semiFixed;
	SVECTOR rotationVector;
	VECTOR originalPosition, oldPosition, newPosition;

		// rotate firstly y, then x; i.e. we firstly rotate left/right until point
		// is roughly in our y axis, then rotate up/down until it is nearly dead ahead
	assert(dx != 0 && dy != 0);

	GsInitCoordinate2(WORLD, &coord);
	GsInitCoordinate2(WORLD, &temp);
	GsInitCoordinate2(WORLD, &semiFixed);

	setVECTOR( &originalPosition, dx, dy, dz);

	//printf("original position:\n");
	//dumpVECTOR(&originalPosition);

		// firstly: rotate left/right until point is close to / nearly on our y axis
	if (dx > 0)			 // tilt to the right, i.e. thetaY is -ve
		{
		newDX = originalPosition.vx;
		for (;;)
			{
			if (abs(newDX) < X_ROTATION_TOLERANCE_ERROR)	 // close enough
				break;
			angleY -= ANGLE_CHANGE;		   // very slow to change by 1	
			setVECTOR( &rotationVector, 0, angleY, 0);
			GsInitCoordinate2(WORLD, &coord);		 // start centred
			DeriveNewCoordSystemFromRotation( &coord, &rotationVector, &temp);
			CopyCoordinateSystem( &temp, &coord);
				// find new dx under our tentative rotation
			ConvertPointWorldToObject (&originalPosition, 
							&coord, &newPosition);
			newDX = newPosition.vx;
			//printf("newDX: %d, old DX: %d\n", newDX, originalPosition.vx);
			assert(newDX <= originalPosition.vx);		// must be getting closer  
			}
		}
	else					   // dx < 0, ==> tilt to the left, i.e. thetaY is +ve
		{
		newDX = originalPosition.vx;
		for (;;)
			{
			if (abs(newDX) < X_ROTATION_TOLERANCE_ERROR)	 // close enough
				break;
			angleY += ANGLE_CHANGE;				 
			setVECTOR( &rotationVector, 0, angleY, 0);
			GsInitCoordinate2(WORLD, &coord);		// start centred
			DeriveNewCoordSystemFromRotation( &coord, &rotationVector, &temp);
			CopyCoordinateSystem( &temp, &coord);
				// find new dx under our tentative rotation
			ConvertPointWorldToObject (&originalPosition, 
							&coord, &newPosition);
			newDX = newPosition.vx;
			//printf("newDX: %d, old DX: %d\n", newDX, originalPosition.vx);
			assert(newDX >= originalPosition.vx);		// must be getting closer
			}
		}

		// now our first rotation is fixed;
		// semiFixed holds our
	*thetaY = angleY;
	//printf("angleY: %d\n", angleY);
	setVECTOR( &rotationVector, 0, *thetaY, 0);
	GsInitCoordinate2(WORLD, &coord);		// start centred
	DeriveNewCoordSystemFromRotation( &coord, &rotationVector, &semiFixed);
	//printf("semiFixed coord:\n");
	//dumpCOORD2(&semiFixed);
		// 'old'position: where point appears to be from our interim state semiFixed,
		// before we try thetaX rotations
	ConvertPointWorldToObject (&originalPosition, 
							&semiFixed, &oldPosition);
	//printf("original position:\n");
	//dumpVECTOR(&originalPosition);
	//printf("oldPosition:\n");
	//dumpVECTOR(&oldPosition);


		// now, after thetaY rotation, point is close to / nearly on our (new) y axis
		// i.e. rotate up/down
	if (dy > 0)			  
		{
		newDY = oldPosition.vy;
		for (;;)
			{
			if (abs(newDY) < Y_ROTATION_TOLERANCE_ERROR)	 // close enough
				break;
			angleX += ANGLE_CHANGE;				 
			setVECTOR( &rotationVector, angleX, 0, 0);
			CopyCoordinateSystem( &semiFixed, &coord);	  // start from just thetaY rotation
			DeriveNewCoordSystemFromRotation( &coord, &rotationVector, &temp);
			CopyCoordinateSystem( &temp, &coord);
				// find new dx under our tentative rotation
			ConvertPointWorldToObject (&oldPosition, 
							&coord, &newPosition);
			newDY = newPosition.vy;
			//printf("newDY: %d, old DY: %d\n", newDY, oldPosition.vy);
			assert(newDY <= oldPosition.vy);		// must be getting closer
			}
		}
	else					
		{
		newDY = oldPosition.vy;
		for (;;)
			{
			if (abs(newDY) < Y_ROTATION_TOLERANCE_ERROR)	 // close enough
				break;
			angleX -= ANGLE_CHANGE;
			setVECTOR( &rotationVector, angleX, 0, 0);
			CopyCoordinateSystem( &semiFixed, &coord);	  // start from just thetaY rotation
			DeriveNewCoordSystemFromRotation( &coord, &rotationVector, &temp);
			CopyCoordinateSystem( &temp, &coord);
				// find new dx under our tentative rotation
			ConvertPointWorldToObject (&oldPosition, 
							&coord, &newPosition);
			newDY = newPosition.vy;
			//printf("newDY: %d, old DY: %d\n", newDY, oldPosition.vy);
			assert(newDY >= oldPosition.vy);		// must be getting closer
			}
		}

	*thetaX = angleX;
	//printf("angleX: %d\n", angleX);
}


  




	// Note: last rotation in rotationList is left unused

void GenerateSplineFromListOfRotations (int numberOfSections, 
			SVECTOR* rotationList, VECTOR* outputSpline, 
			int sectionDistance, VECTOR* startingPosition,
						MATRIX* startingMatrix)
{
	int i;
	GsCOORDINATE2 coord, temp;		 // tunnel coord system: moving ship
	SVECTOR* thisRotation;		   // one section to the next
	VECTOR *thisSplinePoint, *nextSplinePoint;		// world coords
	VECTOR worldVector;		// from above-former to above-latter, world coords

	assert(FALSE);			// NO LONGER CURRENT

	assert(numberOfSections > 0);
	assert(numberOfSections <= MAX_TUNNEL_SECTIONS);

	assert(sectionDistance > 0);

	GsInitCoordinate2(WORLD, &coord);
	GsInitCoordinate2(WORLD, &temp);

		// set the starting point for the entire tunnel
	setVECTOR(&outputSpline[0], startingPosition->vx,
					startingPosition->vy, startingPosition->vz);

		// copy the starting matrix: the orientation at the start
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
	SVECTOR rotationList[MAX_TUNNEL_SECTIONS];
	VECTOR splineCreated[MAX_TUNNEL_SECTIONS];
	int i;
	VECTOR start;

		// init data

	assert(FALSE);				// NO LONGER CURRENT

		// starting point
	setVECTOR( &start, 0, 0, 0);
	InitMatrix(&matrix);
	for (i = 0; i < MAX_TUNNEL_SECTIONS; i++)
		{
		setVECTOR( &rotationList[i], 0, 0, 0);
		setVECTOR( &splineCreated[i], 0, 0, 0);
		}

#if 1
		// the rotations: three xy kinks
	setVECTOR( &rotationList[0], 0, 0, 0);
	for (i = 4; i < MAX_TUNNEL_SECTIONS; i++)
		{
		//setVECTOR( &rotationList[i], ONE/16, ONE/16, 0);
		setVECTOR( &rotationList[i], 0, ONE/16, 0);
		}
	//setVECTOR( &rotationList[3], ONE/16, ONE/16, 0);
	//setVECTOR( &rotationList[4], ONE/16, ONE/16, 0);
	//setVECTOR( &rotationList[5], ONE/16, ONE/16, 0);
#endif



#if 0		// steady curve to the left
	for (i = 1; i < MAX_TUNNEL_SECTIONS; i++)
		{
		setVECTOR( &rotationList[i], 0, ONE/32, 0);
		}
#endif

	

/***/		// print rotations
	printf("the rotation vectors:-\n");
	for (i = 0; i < MAX_TUNNEL_SECTIONS; i++)
		{
		dumpVECTOR( &rotationList[i]);
		}
	printf("\n\n\n\n");
/***/

	GenerateSplineFromListOfRotations (MAX_TUNNEL_SECTIONS, rotationList,
											splineCreated, TunnelSectionLength,
												&start, &matrix);

/***/		// print spline
	printf("the spline points:-\n");
	for (i = 0; i < MAX_TUNNEL_SECTIONS; i++)
		{
		dumpVECTOR( &splineCreated[i]);
		}
	printf("\n\n\n\n");
/***/

		// assign to the tunnel
	for (i = 0; i < MAX_TUNNEL_SECTIONS; i++)
		{
		setVECTOR( &TheTunnel[i].splinePoint, splineCreated[i].vx,
				splineCreated[i].vy, splineCreated[i].vz);
		}
}






void FindSplineCentralPoints (void)
{
	int i;
	VECTOR *currentPoint, *nextPoint;
	VECTOR previousStartToCentre;

	for (i = 0; i < NumberTunnelSections; i++)
		{
		currentPoint = &TheTunnel[i].splinePoint;

		if (i == NumberTunnelSections-1)
			{
			setVECTOR( &previousStartToCentre, 
				&TheTunnel[i-1].sectionCentre.vx - &TheTunnel[i-1].splinePoint.vx,
				&TheTunnel[i-1].sectionCentre.vy - &TheTunnel[i-1].splinePoint.vy,
				&TheTunnel[i-1].sectionCentre.vz - &TheTunnel[i-1].splinePoint.vz);

			setVECTOR( &TheTunnel[i].sectionCentre,
				currentPoint->vx + previousStartToCentre.vx,
				currentPoint->vy + previousStartToCentre.vy,
				currentPoint->vz + previousStartToCentre.vz);
				
			}
		else
			{
			nextPoint = &TheTunnel[i+1].splinePoint;

			setVECTOR( &TheTunnel[i].sectionCentre, 
				currentPoint->vx + ((nextPoint->vx - currentPoint->vx)/2),
				currentPoint->vy + ((nextPoint->vy - currentPoint->vy)/2), 
				currentPoint->vz + ((nextPoint->vz - currentPoint->vz)/2) );
			}
		}
}








	// actually create the TMDs dynamically: 
	// each one a single textured polygon

void CreateTheTunnel (void)
{
	int i;
	int index;
	TunnelSection *thisSection, *nextSection;
	VECTOR *p1, *p2, *p3, *p4;
	TMD_VERT v1, v2, v3, v4;
	ObjectHandler* polygon;
	int polygonID;
	u_long address;
	TMD_NORM normal;
	int clutX, clutY;
	VECTOR* polygonNormal;

		// i limits: don't link first and last section together
	for (i = 0; i < NumberTunnelSections-1; i++)
		{
		thisSection = &TheTunnel[i];
		nextSection = &TheTunnel[i+1];

		for (index = 0; index < NUMBER_SHAPES_PER_SECTION; index++)
			{
				//	order of vertices
				//	   0 +-----+ 1
				// 		 |     |
				//     2 +-----+ 3
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

			polygonNormal = &thisSection->normalsToPolygons[index];
			setNORMAL(&normal, polygonNormal->vx, polygonNormal->vy, polygonNormal->vz);

			//polygon = GetNextFreePolygon();
			//assert(polygon != NULL); 
			// NEED TO USE IDs 

			polygonID = GetNextFreePolygonID();
			assert(polygonID >= 0);
			assert(polygonID < MAX_POLYGONS);

			thisSection->polygonIDs[index] = polygonID;

			polygon = &ThePolygons[polygonID];
		
			clutX = 0; clutY = 480;
			
				// at present: using 4-bit wave texture
				// make 16-bit for dynamic texturing by off-screen drawing
			address = CreateAnotherPolygonTMD (clutX, clutY, 10, 4, 
					0, 0,
					63, 0, 
					0, 63, 
					63, 63,
					&normal, 
					&v1, &v2, &v3, &v4);

			LinkObjectHandlerToSingleTMD (&polygon->handler, 
					address);

				// so that the polygon is reserved, ie never overwritten,
				// but lies dormant until activation
			polygon->alive = UNDEAD;
			}
		}	
}






	// here: could set subdivision for the polygons closest to the viewpoint
	// at present, most/all of the updating will be on a ship-level basis
void UpdateTheTunnel (void)
{
#if 0
	int i;

	for (i = 0; i < NumberTunnelSections; i++)
		{
		}
#endif
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





int GetNextFreePolygonID (void)
{
	int id = -1;
	int i;

	for (i = 0; i < MAX_POLYGONS; i++)
		{
		if (ThePolygons[i].alive == FALSE)
			{
			id = i;
			break;
			}
		}

	return id;
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

	assert(address < END_OF_CREATED_TMDS_STACK);		

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







void ProperCreateFirstTunnel (void)
{
	int i;


	NumberTunnelSections = 12;



	assert(NumberTunnelSections >= 0);
	assert(NumberTunnelSections < MAX_TUNNEL_SECTIONS);


	
		// firstly: set TunnelBaseDescription
		// and assign to one of the three global arrays
		// use simple preprocessor switches 
		// to choose how to initialise tunnel

#if 0			   // points; tunnel goes dead ahead  
	TunnelBaseDescription = SPLINE_POINTS;

	for (i = 0; i < NumberTunnelSections; i++)
		{
		setVECTOR( &TunnelSplinePoints[i], 0, 0, i * TunnelSectionLength);
		}
#endif





#if 0			// vectors; tunnel goes dead ahead		 
				// can set TunnelStartPoint here
	TunnelBaseDescription = SPLINE_VECTORS;	 
	
	for (i = 0; i < NumberTunnelSections; i++)
		{
		setVECTOR( &TunnelSplineVectors[i], 0, 0, TunnelSectionLength);
		}
#endif




#if 1			   // rotation angles; tunnel curves to the left
					// can set TunnelStartPoint and
					// TunnelInitialOrientation here
	TunnelBaseDescription = SECTION_ROTATIONS;

		// always start with a straight
	setVECTOR( &TunnelSectionRotations[0], 0, 0, 0);
	for (i = 1; i < NumberTunnelSections; i++)
		{
		setVECTOR( &TunnelSectionRotations[i], 0, ONE/64, 0);
		}
#endif



	assert(NumberTunnelSections <= MAX_TUNNEL_SECTIONS);
	

	// secondly: sort out the tunnel sections' local coordinate systems

	switch (TunnelBaseDescription)
		{
		case SPLINE_POINTS:
			VerifySplinePointDescription();
			CalculateSplineVectorsFromSplinePoints();
			SortTunnelMainDescriptionsFromSplinePoints();
			break;
		case SPLINE_VECTORS:
			VerifySplineVectorDescription();
			CalculateSplinePointsFromSplineVectors();
			SortTunnelMainDescriptionsFromSplinePoints();
			break;
		case SECTION_ROTATIONS:
			VerifySectionRotationDescription();
			SortTunnelMainDescriptionsFromRotationsList();
			break;
		default:	
			assert(FALSE);
		}

	PrintTunnelSplinePoints();
	PrintTunnelSplineVectors();
	PrintTunnelSectionRotations();

		// thirdly: calculate all other section data
	FleshOutTunnelFromItsDescriptions();
}





void CalculateSplineVectorsFromSplinePoints (void)
{
	int i;
	VECTOR *thisPoint, *nextPoint;

	for (i = 0; i < NumberTunnelSections; i++)
		{
		if (i == NumberTunnelSections-1)	   // same as previous one
			{
			setVECTOR( &TunnelSplineVectors[i], 
					TunnelSplineVectors[i-1].vx,
					TunnelSplineVectors[i-1].vy, 
					TunnelSplineVectors[i-1].vz);
			}
		else
			{
			thisPoint = &TunnelSplinePoints[i];
			nextPoint = &TunnelSplinePoints[i+1];

			setVECTOR( &TunnelSplineVectors[i], 
					nextPoint->vx - thisPoint->vx,
					nextPoint->vy - thisPoint->vy, 
					nextPoint->vz - thisPoint->vz);
			}
		}

		// now copy into tunnel itself
	for (i = 0; i < NumberTunnelSections-1; i++)
		{
		setVECTOR( &TheTunnel[i].splinePoint, TunnelSplinePoints[i].vx,
				TunnelSplinePoints[i].vy, TunnelSplinePoints[i].vz);
		setVECTOR( &TheTunnel[i].splineVector, TunnelSplineVectors[i].vx,
				TunnelSplineVectors[i].vy, TunnelSplineVectors[i].vz);
		}
}




void CalculateSplinePointsFromSplineVectors (void)
{
	int i;
	VECTOR *thisPoint, *nextPoint, *thisVector;

		// begin with specified start point
	setVECTOR( &TunnelSplinePoints[0], TunnelStartPoint.vx, 
					TunnelStartPoint.vy, TunnelStartPoint.vz);

	for (i = 0; i < NumberTunnelSections-1; i++)
		{
		thisVector = &TunnelSplineVectors[i];

		thisPoint = &TunnelSplinePoints[i];
		nextPoint = &TunnelSplinePoints[i+1];

		setVECTOR( nextPoint, thisPoint->vx + thisVector->vx,
			   thisPoint->vy + thisVector->vy,
			   thisPoint->vz + thisVector->vz);
		}

		// now copy into tunnel itself
	for (i = 0; i < NumberTunnelSections-1; i++)
		{
		setVECTOR( &TheTunnel[i].splinePoint, TunnelSplinePoints[i].vx,
				TunnelSplinePoints[i].vy, TunnelSplinePoints[i].vz);
		setVECTOR( &TheTunnel[i].splineVector, TunnelSplineVectors[i].vx,
				TunnelSplineVectors[i].vy, TunnelSplineVectors[i].vz);
		}
}




void SortTunnelMainDescriptionsFromSplinePoints (void)
{
		// call this poorly-named function
	PropagateRotationsAlongTunnel();
}







	// we have only a tunnel description in terms of rotations;
	// find the spline vectors, spline points and coordinate systems
void SortTunnelMainDescriptionsFromRotationsList (void)
{
	int i;
	GsCOORDINATE2 coord, temp;		 // tunnel coord system: moving ship
	SVECTOR* thisRotation;		   // one section to the next
	VECTOR *thisSplinePoint, *nextSplinePoint;		// world coords
	VECTOR thisSplineVector;		// from above-former to above-latter, world coords
	TunnelSection *thisSection, *previousSection;
	SVECTOR rotationNextFromThis, rotationPreviousFromThis;

	GsInitCoordinate2(WORLD, &coord);
	GsInitCoordinate2(WORLD, &temp);

		// set the starting point for the entire tunnel
	setVECTOR( &TheTunnel[0].splinePoint, TunnelStartPoint.vx,
					TunnelStartPoint.vy, TunnelStartPoint.vz);

		// sort out the starting matrix: starting orientation
	CopyMatrix( &TunnelInitialOrientation, &coord.coord);


		// go through from section to section, 
		// find coord system of next section,
	for (i = 0; i < NumberTunnelSections; i++)
		{
		thisSection = &TheTunnel[i];

		thisRotation = &TunnelSectionRotations[i];
		thisSection->thetaX = thisRotation->vx;
		thisSection->thetaY = thisRotation->vy;

		if (i == NumberTunnelSections-1)
			{
			previousSection = &TheTunnel[i-1];

				// inherits same coord system as previous section
			CopyCoordinateSystem( &previousSection->coordinateData, 
									&thisSection->coordinateData);
				// circle inherits directly from section (CHANGE LATER)
			CopyCoordinateSystem( &thisSection->coordinateData, 
								&thisSection->circleCoordinates);

				// set same spline vector as previous section
			setVECTOR( &thisSection->splineVector, 
					previousSection->splineVector.vx,
					previousSection->splineVector.vy,
					previousSection->splineVector.vz);

				// hence set spline point
			setVECTOR( &thisSection->splinePoint, 
					previousSection->splinePoint.vx + thisSection->splinePoint.vx,
					previousSection->splinePoint.vy + thisSection->splinePoint.vy,
					previousSection->splinePoint.vz + thisSection->splinePoint.vz);

			thisSection->coordinateData.coord.t[0] = thisSection->splinePoint.vx;
			thisSection->coordinateData.coord.t[1] = thisSection->splinePoint.vy;
			thisSection->coordinateData.coord.t[2] = thisSection->splinePoint.vz;
			thisSection->coordinateData.flg = 0;

			//printf("section coord %d AFTER all updating:-\n", i);
			//dumpCOORD2(&thisSection->coordinateData);

			continue;
			}

		thisSplinePoint = &thisSection->splinePoint;
		nextSplinePoint = &TheTunnel[i+1].splinePoint;

		//printf("known spline point:-\n");
		//dumpVECTOR(thisSplinePoint);

		//printf("section coord BEFORE rotate:-\n");
		//dumpCOORD2(&coord);
		
			// find next section's coordinate system from rotations
			// order of rotations is Y then X
		DeriveNewCoordSystemFromRotation (&coord,
				thisRotation, &temp);
		CopyCoordinateSystem( &temp, &coord);

			// copy into Tunnel itself
		CopyCoordinateSystem( &coord, &thisSection->coordinateData);
			// circle inherits directly from section (CHANGE LATER)
		CopyCoordinateSystem( &thisSection->coordinateData, 
							&thisSection->circleCoordinates);	

		//printf("rotation angle:-\n");
		//dumpVECTOR(thisRotation);
		//printf("section coord AFTER rotate:-\n");
		//dumpCOORD2(&coord);

			// the next spline point is TunnelSectionLength away, 
			// directly ahead 
			// i.e. relative vector is always (0, 0, TunnelSectionLength);
			// here we find this vector in world coord system
		setVECTOR( &thisSplineVector, 
				((TunnelSectionLength * coord.coord.m[2][0]) >> 12),
				((TunnelSectionLength * coord.coord.m[2][1]) >> 12),
				((TunnelSectionLength * coord.coord.m[2][2]) >> 12));

		//printf("world vector to next spline point:-\n");
		//dumpVECTOR(&worldVector);

			// now we know current point plus vector to next point:
			// hence find next point itself
		setVECTOR(nextSplinePoint, thisSplinePoint->vx + thisSplineVector.vx,
						thisSplinePoint->vy + thisSplineVector.vy,
						thisSplinePoint->vz + thisSplineVector.vz);	

			// copy spline vector into tunnel data
		setVECTOR( &thisSection->splineVector, thisSplineVector.vx,
			thisSplineVector.vy, thisSplineVector.vz);

		thisSection->coordinateData.coord.t[0] = thisSplinePoint->vx;
		thisSection->coordinateData.coord.t[1] = thisSplinePoint->vy;
		thisSection->coordinateData.coord.t[2] = thisSplinePoint->vz;
		thisSection->coordinateData.flg = 0;

		//printf("section coord %d AFTER all updating:-\n", i);
		//dumpCOORD2(&thisSection->coordinateData);
		}
}





void VerifySplinePointDescription (void)
{
	int i;
	VECTOR *thisPoint, *nextPoint;
	VECTOR toNext;

	for (i = 0; i < NumberTunnelSections-1; i++)
		{
		thisPoint = &TunnelSplinePoints[i];
		nextPoint = &TunnelSplinePoints[i+1];

		setVECTOR( &toNext, nextPoint->vx - thisPoint->vx,
							nextPoint->vy - thisPoint->vy,
							nextPoint->vz - thisPoint->vz);

		assert(abs(SizeOfVector(&toNext) - TunnelSectionLength) 
							<= SPLINE_POINT_DISTANCE_TOLERANCE);
		}
}





void VerifySplineVectorDescription (void)
{
	int i;
	VECTOR *thisVector;

	for (i = 0; i < NumberTunnelSections; i++)
		{
		thisVector = &TunnelSplineVectors[i];
	 
		assert(abs(SizeOfVector(thisVector) - TunnelSectionLength) 
						<= SPLINE_POINT_DISTANCE_TOLERANCE);
		}
}




void VerifySectionRotationDescription (void)
{
	int i;
	SVECTOR *thisRotationVector;

	for (i = 0; i < NumberTunnelSections; i++)
		{
		thisRotationVector = &TunnelSectionRotations[i];

			// z rotation isn't meaningful in quasi-cylindrical tunnel
		assert(thisRotationVector->vz == 0);		 

		if (i == 0)		   // first section must be a straight
			{
			assert(thisRotationVector->vx == 0);
			assert(thisRotationVector->vy == 0);
			}
	 	else
	 		{					  
			assert(abs(thisRotationVector->vx) < MAXIMUM_SECTION_ROTATION);
			assert(abs(thisRotationVector->vy) < MAXIMUM_SECTION_ROTATION);
			}
		}
}



void PrintTunnelSplinePoints (void)
{
	int i;
	VECTOR *thisSplinePoint;

	printf("\n\nHere are the tunnel spline points\n");

	for (i = 0; i < NumberTunnelSections; i++)
		{
		thisSplinePoint = &TheTunnel[i].splinePoint;

		dumpVECTOR(thisSplinePoint);
		}

	printf("\n\n\n");
}


void PrintTunnelSplineVectors (void)
{
	int i;
	VECTOR *thisSplineVector;

	printf("\n\nHere are the tunnel spline vectors\n");

	for (i = 0; i < NumberTunnelSections; i++)
		{
		thisSplineVector = &TheTunnel[i].splineVector;

		dumpVECTOR(thisSplineVector);
		}
	
	printf("\n\n\n");
}


void PrintTunnelSectionRotations (void)		
{
	int i;
	int thetaX, thetaY;

	printf("\n\nHere are the tunnel rotations\n");

	for (i = 0; i < NumberTunnelSections; i++)
		{
		thetaX = TheTunnel[i].thetaX;
		thetaY = TheTunnel[i].thetaY;

		printf("thetaX %d, thetaY %d\n", thetaX, thetaY);
		}

	printf("\n\n\n");
}





	// here, we know spline points and vectors,
	// rotation angles and coordinate systems, for all sections;
	// hence we can precalculate all other useful data
void FleshOutTunnelFromItsDescriptions (void)
{
	FindSplineCentralPoints();

	CalculateCirclePointsPolygonSidesCentresAndNormals();
}
	





	// offset is the offset from the spline point
void PositionObjectInTunnelSection (ObjectHandler* object, 
										int whichSection, VECTOR* offset)
{
	TunnelSection* section;
	VECTOR offsetInWorld;

	assert(whichSection >= 0);
	assert(whichSection < NumberTunnelSections);
	assert(offset->vz >= 0);
	assert(offset->vz < TunnelSectionLength);

	section = &TheTunnel[whichSection];

	ExpressSubPointInSuper(offset, &section->coordinateData, &offsetInWorld);

	setVECTOR( &object->position, 
			section->splinePoint.vx + offsetInWorld.vx,
			section->splinePoint.vy + offsetInWorld.vy,
			section->splinePoint.vz + offsetInWorld.vz);
			
	object->coord.flg = 0;

	object->tunnelSection = whichSection;

	BringAliveLocalTunnelAroundShip(object);
}
			   






	
	// Note: this function makes polygons alive only ahead of the ship

void BringAliveLocalTunnelAroundShip (ObjectHandler* object)
{
	TunnelSection* section;
	int id;
	int polygonID;
	int i, j;

	id = object->tunnelSection;

	for (i = 0; i < NUMBER_SECTIONS_ALIVE; i++)
		{
		if (id+i >= NumberTunnelSections)
			break;

		section = &TheTunnel[id+i];

		for (j = 0; j < NUMBER_SHAPES_PER_SECTION; j++)
			{
			polygonID = section->polygonIDs[j];

			assert(polygonID >= 0);
			assert(polygonID < MAX_POLYGONS);

			ThePolygons[polygonID].alive = TRUE;
			}
		}
}

	
	
	


			  


void HandleShipsMovementInTunnel (ObjectHandler* object)
{
	TunnelSection *nextSection, *thisSection;
	VECTOR worldMovement, newPosition;
	VECTOR *splinePoint;
	VECTOR splinePointToShip, temp, relativePosition;
	int legalDistanceFromSectionCentralLine;
	int x, y;
	
	thisSection = &TheTunnel[object->tunnelSection];
	splinePoint = &thisSection->splinePoint;

		// find new position after move
	ApplyMatrixLV( &object->coord.coord, &object->velocity, &worldMovement);
	setVECTOR( &newPosition, 
			object->position.vx + worldMovement.vx,
			object->position.vy + worldMovement.vy,
			object->position.vz + worldMovement.vz);

		// find which section ship will be in after movement
	nextSection = FindClosestSectionToPoint( &newPosition, object->tunnelSection);
	
		// collision detection done as circular tunnel:
		// firstly express ship new position in section coordinates
		// by first translating then rotating 
	setVECTOR( &temp, 
				newPosition.vx - splinePoint->vx,
				newPosition.vy - splinePoint->vy,
				newPosition.vz - splinePoint->vz);
	ExpressSuperPointInSub ( &temp, 
				&thisSection->coordinateData, &relativePosition);

	if (frameNumber % 30 == 0)
		{
		printf("newPos world, then relative to section %d\n", object->tunnelSection);
		dumpVECTOR( &newPosition);
		dumpVECTOR( &relativePosition);
		}

		// now that we know ship's position in tunnel section's coordinates,
		// collision detection is simply: has position exceeded given radius
	legalDistanceFromSectionCentralLine = TunnelMiddleRadius - object->collisionRadius;
	x = relativePosition.vx;
	y = relativePosition.vy;
	if ( ((x * x) + (y * y)) 
			>= 
			(legalDistanceFromSectionCentralLine * legalDistanceFromSectionCentralLine) )
		{
		printf("COLLISION WITH POLY\n");
		printf("legalDistanceFromSectionCentralLine: %d\n",
			legalDistanceFromSectionCentralLine);
		}
	

#if 1			// NOT YET
	if (nextSection->id != object->tunnelSection)
		{
			// update the ship
		object->tunnelSection = nextSection->id;
		}
#endif

	UpdateObjectCoordinates(&object->twist, 
			&object->position, &object->velocity, 
			&object->coord, &object->matrix);

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


		



		
	


#if 0
		// update obj coord
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
#endif


}
	   






TunnelSection* FindClosestSectionToPoint (VECTOR* position, int originalSection)
{
	int currentSectionID;
	TunnelSection* consideredSections[MAX_SECTIONS_CONSIDERED];
	TunnelSection* thisSection;
	int x, y, z;
	int distances[MAX_SECTIONS_CONSIDERED];
	int minDistance = -1, whichOne = -1;
	int numberConsidered = 0;
	VECTOR *sectionCentre;
	int i;

	assert(originalSection >= 0);
	assert(originalSection < NumberTunnelSections);

	for (i = originalSection; i >= originalSection - SECTION_LOOKAHEAD; i--)
		{
		if (i < 0)
			break;
		consideredSections[numberConsidered] = &TheTunnel[i];
		numberConsidered++;
		}
	for (i = originalSection+1; i <= originalSection + SECTION_LOOKAHEAD; i++)
		{
		if (i > NumberTunnelSections-1)
			break;
		consideredSections[numberConsidered] = &TheTunnel[i];
		numberConsidered++;
		}

	for (i = 0; i < numberConsidered; i++)
		{
		sectionCentre = &consideredSections[i]->sectionCentre;

		x = sectionCentre->vx - position->vx;
		y = sectionCentre->vy - position->vy;
		z = sectionCentre->vz - position->vz;

			// only need to find which closer -> don't need actual distances
			// hence don't need to take square root
		distances[i] = (x * x) + (y * y) + (z * z); 

		if (i == 0)
			{
			minDistance = distances[i];
			whichOne = 0;
			}
		else
			{
			if (distances[i] < minDistance)
				{
				minDistance = distances[i];
				whichOne = i;
				}
			}
		}

	return consideredSections[whichOne];
}