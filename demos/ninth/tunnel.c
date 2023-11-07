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



#define ValidID(id)  ( (id >= 0) && (id < NumberTunnelSections) )

#define ValidPolygonID(id) ( (id >= 0) && (id < MAX_POLYGONS) )


#define PrintDirection(direction)											\
	switch(direction)														\
		{																	\
		case FORWARDS:	printf("FORWARDS\n"); 	break;						\
		case BACKWARDS:	printf("BACKWARDS\n"); 	break;						\
		default:		printf("BAD direction %d\n", direction); exit(1);	\
		}


#define ANTI_DIRECTION(direction)    (direction) == BACKWARDS ? FORWARDS : BACKWARDS


#define max(a,b)  (	((a) > (b)) ? (a) : (b)		)

#define min(a,b)  (	((a) < (b)) ? (a) : (b)		)




   
void InitialiseTheTunnel (void)
{
	InitTunnelToVoid();

	SetBasicTunnelParameters();

	ProperCreateFirstTunnel();
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

	printf("\n\nMain tunnel dimensions :-\n");
	printf("section shape angle %d\n", TunnelSectionShapeAngle);
	printf("section length %d\n", TunnelSectionLength);
	printf("outer radius: %d, inner radius: %d, middle radius: %d\n\n\n", 
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
	VECTOR *point;
	VECTOR *polygonCentre, *polygonNormal, *sectionCentre;
	TunnelSection *thisSection, *nextSection;
	VECTOR *p1, *p2, *p3, *p4;

	printf("\n\nCalculating circle points\n\n\n");

	for (i = 0; i < NumberTunnelSections; i++)
		{
		thisSection = &TheTunnel[i];
		coord = &thisSection->coordinateData;
			
		//printf("section %d\n", i);

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

			//printf("point %d on circle\n", j);
			//dumpVECTOR(point);
			}
		}

	printf("Calculating polygon centres and normals\n\n\n");

		// now that points on circle are known, find polygon sides, centres,
		// and normals
	for (i = 0; i < NumberTunnelSections; i++)
		{
		thisSection = &TheTunnel[i];
		if (i == NumberTunnelSections-1)
			{
			nextSection = &TheTunnel[0];
			}
		else
			{
			nextSection = &TheTunnel[i+1];
			}

		sectionCentre = &thisSection->sectionCentre;
		//printf("section %d: centre:\n", i);
		//dumpVECTOR(sectionCentre);

		for (j = 0; j < NUMBER_SHAPES_PER_SECTION; j++)
			{
			polygonCentre = &thisSection->polygonCentres[j];
			polygonNormal = &thisSection->normalsToPolygons[j];

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

			//printf("point %d on circle\n", j);
			//printf("polygon centre:\n");
			//dumpVECTOR(polygonCentre);

				// calculate normals: from centre of poly to centre of section
				// ==> poly always visible from inside section
			setVECTOR(polygonNormal, sectionCentre->vx - polygonCentre->vx,
							sectionCentre->vy - polygonCentre->vy,
							sectionCentre->vz - polygonCentre->vz);
			//printf("section is %d\n", i);
			//printf("finding normal: poly centre, normal\n");
			//dumpVECTOR(polygonCentre);
			//dumpVECTOR(polygonNormal);
			ScaleVectorToUnit (polygonNormal);
			//printf("polygon normal:\n");
			//dumpVECTOR(polygonNormal);
			}
		}
}






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

			thisSection->coordinateData.coord.t[0] = thisSection->splinePoint.vx;
			thisSection->coordinateData.coord.t[1] = thisSection->splinePoint.vy;
			thisSection->coordinateData.coord.t[2] = thisSection->splinePoint.vz;
			thisSection->coordinateData.flg = 0;
			continue;
			}
		else if (i == NumberTunnelSections-1)
			nextSection	= &TheTunnel[0];
		else
			nextSection = &TheTunnel[i+1];

		assert(i > 0);
		previousSection = &TheTunnel[i-1];
		
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
		ExpressSuperPointInSub (&worldVector, 
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
				//printf("BOTH: thetaX %d, thetaY %d\n", 
				//				thisSection->thetaX, thisSection->thetaY);
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



		//printf("section coord:-\n");
		//dumpCOORD2(thisCoordSystem);
		//printf("circle coord:-\n");
		//dumpCOORD2(&thisSection->circleCoordinates);
		}
}







	// from our viewpoint, next spline point on neither x or y axes,
	// find thetaY and thetaX such that those rotations (order: y then x)
	// will take us to that point
	// Note: only approximate

	// NOTE: rewrite this: can find the angles much more easily
	// using arcsin and arccos

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
			ExpressSuperPointInSub (&originalPosition, 
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
			ExpressSuperPointInSub (&originalPosition, 
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
	ExpressSuperPointInSub (&originalPosition, 
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
			ExpressSuperPointInSub (&oldPosition, 
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
			ExpressSuperPointInSub (&oldPosition, 
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
	printf("The rotation vectors:-\n");
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
	printf("The spline points:-\n");
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

	printf("\n\nCalculating spline central points\n\n\n");

	for (i = 0; i < NumberTunnelSections; i++)
		{
		currentPoint = &TheTunnel[i].splinePoint;
		if (i == NumberTunnelSections-1)
			{
			nextPoint = &TheTunnel[0].splinePoint;		  // wrap around
			}
		else
			{
			nextPoint = &TheTunnel[i+1].splinePoint;
			}

		setVECTOR( &TheTunnel[i].sectionCentre, 
				currentPoint->vx + ((nextPoint->vx - currentPoint->vx)/2),
				currentPoint->vy + ((nextPoint->vy - currentPoint->vy)/2), 
				currentPoint->vz + ((nextPoint->vz - currentPoint->vz)/2) );

		//printf("section %d centre:\n", i);
		//dumpVECTOR (&TheTunnel[i].sectionCentre);
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
	MiniObject* polygon;
	int polygonID;
	u_long address;
	TMD_NORM normal;
	int clutX, clutY;
	VECTOR* polygonNormal;

	printf("\n\nStart of CreateTheTunnel\nTmd stack addr 0x%X\n\n\n", 
						(u_long)CurrentTMDStackAddress);

		
	for (i = 0; i < NumberTunnelSections; i++)
		{
		thisSection = &TheTunnel[i];
		if (i == NumberTunnelSections - 1)
			nextSection = &TheTunnel[0];
		else
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
			assert(ValidPolygonID(polygonID));

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

	printf("\nEnd of CreateTheTunnel\nTmd stack addr 0x%X\n", 
						(u_long)CurrentTMDStackAddress);
	printf("So far %d TMDs created\n\n\n", NumberOfCreatedTMDs);
}





	// so far, all tunnel updating done by the ship handler
void UpdateTheTunnel (void)
{
	
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






void CountNumberLivingPolys	(int* nonDivided, int* divided)
{
	int i;

	*nonDivided = 0;
	*divided = 0;

	for (i = 0; i < MAX_POLYGONS; i++)
		{
		if (ThePolygons[i].alive == TRUE)
			{
			if (ThePolygons[i].handler.attribute == 0)
				(*nonDivided)++;
			else
				(*divided)++;
			}
		}
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


	NumberTunnelSections = 16;



	assert(NumberTunnelSections >= 0);
	assert(NumberTunnelSections < MAX_TUNNEL_SECTIONS);


	
		// firstly: set TunnelBaseDescription
		// and assign to one of the three global arrays
		// (TunnelSplinePoints, TunnelSplineVectors or TunnelSectionRotations) 
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

		// adjust start point to make track roughly circular around world origin
	setVECTOR( &TunnelStartPoint, 
		TunnelSectionLength * 5 / 2, 0, -TunnelSectionLength/2);

		// always start with a straight
	setVECTOR( &TunnelSectionRotations[0], 0, 0, 0);
	for (i = 1; i < NumberTunnelSections; i++)
		{
		setVECTOR( &TunnelSectionRotations[i], 0, ONE/16, 0);
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

	assert(TestIfTunnelIsCircular() == TRUE);

		// thirdly: calculate all other section data
	FleshOutTunnelFromItsDescriptions();
}





void CalculateSplineVectorsFromSplinePoints (void)
{
	int i;
	VECTOR *thisPoint, *nextPoint;

	printf("\n\nCalculating spline vectors from spline points\n\n\n");

	for (i = 0; i < NumberTunnelSections; i++)
		{
		thisPoint = &TunnelSplinePoints[i];
		if (i == NumberTunnelSections-1)	   // same as previous one
			{
			nextPoint = &TunnelSplinePoints[0]; 	// wrap around track
			}
		else
			{
			nextPoint = &TunnelSplinePoints[i+1];
			}

		setVECTOR( &TunnelSplineVectors[i], 
				nextPoint->vx - thisPoint->vx,
				nextPoint->vy - thisPoint->vy, 
				nextPoint->vz - thisPoint->vz);
		}

		// now copy into tunnel itself
	for (i = 0; i < NumberTunnelSections; i++)
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

	printf("\n\nCalculating spline points from spline vectors\n\n\n");

		// begin with specified start point
	setVECTOR( &TunnelSplinePoints[0], TunnelStartPoint.vx, 
					TunnelStartPoint.vy, TunnelStartPoint.vz);

		// only need to find N-1 points
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
	for (i = 0; i < NumberTunnelSections; i++)
		{
		setVECTOR( &TheTunnel[i].splinePoint, TunnelSplinePoints[i].vx,
				TunnelSplinePoints[i].vy, TunnelSplinePoints[i].vz);
		setVECTOR( &TheTunnel[i].splineVector, TunnelSplineVectors[i].vx,
				TunnelSplineVectors[i].vy, TunnelSplineVectors[i].vz);
		}
}




void SortTunnelMainDescriptionsFromSplinePoints (void)
{
	printf("\n\nCalculating tunnel from spline points\n\n\n");

	assert(FALSE);		// much needs redoing here

		// call this poorly-named function
	PropagateRotationsAlongTunnel();
}







	// we have only a tunnel description in terms of rotations;
	// find the spline vectors, spline points and coordinate systems
void SortTunnelMainDescriptionsFromRotationsList (void)
{
	int i;
	GsCOORDINATE2 coord, temp, circleCoord;			// tunnel coord system: moving ship
	SVECTOR* thisRotation;		   // one section to the next
	SVECTOR circleRotation;
	VECTOR *thisSplinePoint, *nextSplinePoint;		// world coords
	VECTOR thisSplineVector;		// from above-former to above-latter, world coords
	TunnelSection *thisSection;

	printf("\n\nCalculating tunnel from rotation vectors\n\n\n");

	GsInitCoordinate2(WORLD, &coord);
	GsInitCoordinate2(WORLD, &temp);
	GsInitCoordinate2(WORLD, &circleCoord);

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
		
		thisSplinePoint = &thisSection->splinePoint;

		//printf("known spline point:-\n");
		//dumpVECTOR(thisSplinePoint);

		//printf("section coord BEFORE rotate:-\n");
		//dumpCOORD2(&coord);

			// circle at half-way tilt
		setVECTOR( &circleRotation, 
				thisRotation->vx / 2,
				thisRotation->vy / 2,
				thisRotation->vz / 2);
		DeriveNewCoordSystemFromRotation (&coord,
				&circleRotation, &circleCoord);
		CopyCoordinateSystem( &circleCoord, 
							&thisSection->circleCoordinates);
		
			// find next section's coordinate system from rotations
			// order of rotations is Y then X
		DeriveNewCoordSystemFromRotation (&coord,
				thisRotation, &temp);
		CopyCoordinateSystem( &temp, &coord);

			// copy into Tunnel itself
		CopyCoordinateSystem( &coord, &thisSection->coordinateData);
		   
			

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
			// copy spline vector into tunnel data
		setVECTOR( &thisSection->splineVector, thisSplineVector.vx,
			thisSplineVector.vy, thisSplineVector.vz);

		if (i != NumberTunnelSections-1)
			{
			nextSplinePoint = &TheTunnel[i+1].splinePoint;
				// now we know current point plus vector to next point:
				// hence find next point itself
			setVECTOR(nextSplinePoint, thisSplinePoint->vx + thisSplineVector.vx,
							thisSplinePoint->vy + thisSplineVector.vy,
							thisSplinePoint->vz + thisSplineVector.vz);	
			}

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

	printf("\n\nVerifying spline point description\n");

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
	printf("OK\n\n\n");
}





void VerifySplineVectorDescription (void)
{
	int i;
	VECTOR *thisVector;

	printf("\n\nVerifying spline vector description\n");

	for (i = 0; i < NumberTunnelSections; i++)
		{
		thisVector = &TunnelSplineVectors[i];
	 
		assert(abs(SizeOfVector(thisVector) - TunnelSectionLength) 
						<= SPLINE_POINT_DISTANCE_TOLERANCE);
		}
	printf("OK\n\n\n");
}




void VerifySectionRotationDescription (void)
{
	int i;
	SVECTOR *thisRotationVector;

	printf("\n\nVerifying rotational description\n");

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
	 		if (abs(thisRotationVector->vx) < X_ROTATION_TOLERANCE_ERROR)
	 			{		  // no thetaX
				assert(abs(thisRotationVector->vy) <= MAXIMUM_PURE_Y_ROTATION);
	 			}
	 		else if (abs(thisRotationVector->vy) < Y_ROTATION_TOLERANCE_ERROR)
	 			{		  // no thetaY
				assert(abs(thisRotationVector->vx) <= MAXIMUM_PURE_X_ROTATION);
	 			} 	
	 		else
	 			{
				assert(abs(thisRotationVector->vx) <= MAXIMUM_MIXED_X_ROTATION);
				assert(abs(thisRotationVector->vy) <= MAXIMUM_MIXED_Y_ROTATION);
	 			}	
			}
		}
	printf("OK\n\n\n");
}






int TestIfTunnelIsCircular (void)
{
	TunnelSection* last;
	VECTOR tunnelStartPoint, relativePosition;

	last = &TheTunnel[NumberTunnelSections-1];

		// looking from last section, see where next point is
	setVECTOR( &tunnelStartPoint, 
			TheTunnel[0].splinePoint.vx,
			TheTunnel[0].splinePoint.vy,
			TheTunnel[0].splinePoint.vz);
	tunnelStartPoint.vx -= last->splinePoint.vx;
	tunnelStartPoint.vy -= last->splinePoint.vy;
	tunnelStartPoint.vz -= last->splinePoint.vz;

	ExpressSuperPointInSub( &tunnelStartPoint, 
					&last->coordinateData, &relativePosition);

#if 0
	printf("start point, last coord, relative start point\n");
	dumpVECTOR(&tunnelStartPoint);
	dumpCOORD2(&last->coordinateData);
	dumpVECTOR(&relativePosition);
#endif

		// it should be dead ahead
	if (abs(relativePosition.vx) > SPLINE_POINT_DISTANCE_TOLERANCE)
		return FALSE;
	if (abs(relativePosition.vy) > SPLINE_POINT_DISTANCE_TOLERANCE)
		return FALSE;		
	if (abs(relativePosition.vz - TunnelSectionLength) > SPLINE_POINT_DISTANCE_TOLERANCE)
		return FALSE;
	
	return TRUE;	
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

	assert(ValidID(whichSection));
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
}
			   






	
	// Note: we assume ship is looking forwards

void BringAliveLocalTunnelAroundShip (ObjectHandler* object)
{
	int sectionID;
	TunnelSection* section;
	int objectSectionID;
	int polygonID;
	int i, j;

	objectSectionID = object->tunnelSection;
	assert(ValidID(objectSectionID));


	for (i = -1; i < NUMBER_SECTIONS_ALIVE; i++)
		{
		switch(i)
			{
			case -1:	
				sectionID = GetRelativeSection (objectSectionID, -i, BACKWARDS);
				break;
			default:
				sectionID = GetRelativeSection (objectSectionID, i, FORWARDS);
			}
		
		section = &TheTunnel[sectionID];
				
		for (j = 0; j < NUMBER_SHAPES_PER_SECTION; j++)
			{
			polygonID = section->polygonIDs[j];
			assert(ValidPolygonID(polygonID));

			ThePolygons[polygonID].alive = TRUE;
			}		
		}


		// make these subdivided
	for (i = -1; i <= 1; i++)
		{
		switch(i)
			{
			case -1:	
				sectionID = GetRelativeSection (objectSectionID, -i, BACKWARDS);
				break;
			case 0: case 1:
				sectionID = GetRelativeSection (objectSectionID, i, FORWARDS);
				break;
			}

		printf("turning ON subdiv in section %d\n", sectionID);
		
		section = &TheTunnel[sectionID];
		
		for (j = 0; j < NUMBER_SHAPES_PER_SECTION; j++)
			{
			polygonID = section->polygonIDs[j];
			assert(ValidPolygonID(polygonID));

			SetMiniObjectSubdivision( &ThePolygons[polygonID], SUBDIVISION_EXTENT);
			}		
		}

#if 1
	{
	int divided, nonDivided;
	printf("\n\nAfter bringing tunnel around ship to life\n");
	CountNumberLivingPolys	(&nonDivided, &divided);
	printf("living: divided %d, nonDiv %d\n\n\n", divided, nonDivided);
	}
#endif
}

	
	


	
	// ensuring that the ship can only cross one section boundary per frame
	// makes many things much easier; speed can still be very high
#define SHIP_MAXIMUM_SPEED (TunnelSectionLength/2)

	// store initial states: view - section set
	// rotate ship
	// sort collision detection and handling
	// move ship
	// get new view - section set
	// update polygon's alive/undead and subdivision
	// update ship 'tunnelSection' member

void HandleShipsMovementInTunnel (ObjectHandler* object)
{
	VECTOR oldPos, newPos, worldMove;
	int startSection, nextSection;
	int shipDirectionBefore, shipDirectionAfter;
	int viewSectionBefore, viewSectionAfter;

		// store position before move
	setVECTOR (&oldPos, object->position.vx,
		object->position.vy, object->position.vz); 

		// store which section ship inside before move
	startSection = object->tunnelSection;
	//printf("ship starts in section %d\n", startSection);

		// store direction before move
	shipDirectionBefore = FindWhichWayViewLooksInSection(startSection);

		// find view section after move
	viewSectionBefore = FindViewPointSection(startSection);



		// firstly the ship rotates
	DealWithShipRotation(&object->twist,  
			&object->coord, &object->matrix);

		 
#if 0
	{
	int divided, nonDivided;
	CountNumberLivingPolys	(&nonDivided, &divided);
	printf("living: divided %d, nonDiv %d\n\n\n", divided, nonDivided);
	}
#endif


		// find the object velocity in world coordinate terms
	ApplyMatrixLV(&object->matrix, &object->velocity, &worldMove);

		// hence new position
	setVECTOR( &newPos, 
			oldPos.vx + worldMove.vx,
			oldPos.vy + worldMove.vy,
			oldPos.vz + worldMove.vz);

		
		// find which section ship will be in after movement
	nextSection = FindSectionPointIsIn ( &newPos, startSection);
		
		// detect collision and handle;
	if (DetectCollisionWithTunnel(object, nextSection, &newPos) == TRUE)
		{
			// reflect and dampen (maybe add noise)
		HandleCollisionWithTunnel(object);
		}

		// if none,	set new position
	setVECTOR (&object->position, newPos.vx,
			newPos.vy, newPos.vz);
	object->coord.coord.t[0] = object->position.vx;
	object->coord.coord.t[1] = object->position.vy;
	object->coord.coord.t[2] = object->position.vz;

		// tell GTE that coordinate system has been updated
	object->coord.flg = 0;

	   


		// find ship direction after move
	shipDirectionAfter = FindWhichWayViewLooksInSection(nextSection);

		// find view section after move
	viewSectionAfter = FindViewPointSection(nextSection);


		// if ship crosses from one section	into the next
	if (startSection != nextSection)
		{
			// update the ship
		object->tunnelSection = nextSection;
		}

		// if view has crossed sections, update the polygons' alive status and subdivision
	if (viewSectionBefore != viewSectionAfter)
		{
		UpdateTunnelPolygonLivingStatus2 (viewSectionBefore, viewSectionAfter, 
	 			shipDirectionBefore, shipDirectionAfter);

		UpdateTunnelSubdivision2 (viewSectionBefore, viewSectionAfter);
		}


		// momentum or not? 
	if (object->movementMomentumFlag == FALSE)
		{
		setVECTOR( &object->velocity, 0, 0, 0);
		}
	if (object->rotationMomentumFlag == FALSE)
		{
		setVECTOR( &object->twist, 0, 0, 0);
		}

		// bounds on velocity
	if (object->velocity.vz >= SHIP_MAXIMUM_SPEED)
		object->velocity.vz = SHIP_MAXIMUM_SPEED;
	else if (object->velocity.vz <= -SHIP_MAXIMUM_SPEED)
		object->velocity.vz = -SHIP_MAXIMUM_SPEED;
}







void UpdateTunnelPolygonLivingStatus (int firstID, int nextID, int direction)
{
	int sectionID;
	TunnelSection* section;
	int polygonID;
	int numberToChangeLiving;		  		
	int i, j;

	assert(ValidID(firstID));
	assert(ValidID(nextID));

		// how many members in the lists (both same length)
	numberToChangeLiving = DistanceBetweenSections (firstID, nextID, direction);
	assert(numberToChangeLiving < MAX_TO_CHANGE_LIVING_STATUS);

	//printf("first: %d, next: %d\n\
	//		direction: %d, numToChange: %d\n", 
	//		firstID, nextID, 
	//		direction, numberToChangeLiving);
	//PrintDirection(direction);

		// make some undead (since no longer visible)
	for (i = 0; i < numberToChangeLiving; i++)
		{
		sectionID = GetRelativeSection (firstID, i, direction);
		
		//printf("making section %d dead\n", sectionID);
		
		section = &TheTunnel[sectionID];
		
		for (j = 0; j < NUMBER_SHAPES_PER_SECTION; j++)
			{
			polygonID = section->polygonIDs[j];
			assert(ValidPolygonID(polygonID));

			ThePolygons[polygonID].alive = UNDEAD;
			}		
		}

		// make others alive (since they are now visible)
	for (i = 0; i < numberToChangeLiving; i++)
		{
		sectionID = GetRelativeSection (firstID, NUMBER_SECTIONS_ALIVE + i, direction);

		//printf("making section %d alive\n", sectionID);
		
		section = &TheTunnel[sectionID];
		
		for (j = 0; j < NUMBER_SHAPES_PER_SECTION; j++)
			{
			polygonID = section->polygonIDs[j];
			assert(ValidPolygonID(polygonID));

			ThePolygons[polygonID].alive = TRUE;
			}		
		}
}






	// clearly scope for optimisation here

void UpdateTunnelPolygonLivingStatus2 (int viewSectionBefore, int viewSectionAfter, 
			int directionBefore, int directionAfter)
{
	int sectionID;
	TunnelSection* section;
	int i, j;
	int polygonID;
	int otherWay;

	if ( (viewSectionBefore == viewSectionAfter)
			&& (directionBefore == directionAfter) )
				return;

		// turn off all those previously on
	for (i = -1; i < NUMBER_SECTIONS_ALIVE; i++)
		{
		switch(i)
			{
			case -1:	
				otherWay = ANTI_DIRECTION(directionBefore);
				sectionID = GetRelativeSection (viewSectionBefore, -i, otherWay);
				break;
			default:
				sectionID = GetRelativeSection (viewSectionBefore, i, directionBefore);
			}

		//printf("making section %d undead\n", sectionID);
		
		section = &TheTunnel[sectionID];
		
		for (j = 0; j < NUMBER_SHAPES_PER_SECTION; j++)
			{
			polygonID = section->polygonIDs[j];
			assert(ValidPolygonID(polygonID));

			ThePolygons[polygonID].alive = UNDEAD;
			}		
		}

		// turn on a new batch
	for (i = -1; i < NUMBER_SECTIONS_ALIVE; i++)
		{
		switch(i)
			{
			case -1:	
				otherWay = ANTI_DIRECTION(directionAfter);
				sectionID = GetRelativeSection (viewSectionBefore, -i, otherWay);
				break;
			default:
				sectionID = GetRelativeSection (viewSectionBefore, i, directionAfter);
			}

		//printf("making section %d alive\n", sectionID);
		
		section = &TheTunnel[sectionID];
		
		for (j = 0; j < NUMBER_SHAPES_PER_SECTION; j++)
			{
			polygonID = section->polygonIDs[j];
			assert(ValidPolygonID(polygonID));

			ThePolygons[polygonID].alive = TRUE;
			}		
		}
}





void UpdateTunnelSubdivision (int startViewSection, 
		int nextViewSection, int direction)
{
	int distance, numberToChange;
	int i, j;
	int sectionID;
	TunnelSection* section;
	int polygonID;

	assert(ValidID(startViewSection));
	assert(ValidID(nextViewSection));

	distance = DistanceBetweenSections(startViewSection,	
						nextViewSection, direction);

	numberToChange = min(distance, SUBDIVISION_LOOKAHEAD);
	//printf("num for subdiv: %d\n", numberToChange);

	//printf("first: %d, next: %d\n\
	//		direction: %d, numToChange: %d\n", 
	//		startViewSection, nextViewSection, 
	//		direction, numberToChange);
	//PrintDirection(direction);

		// make these no longer subdivided
	for (i = 0; i < numberToChange; i++)
		{
		sectionID = GetRelativeSection (startViewSection, i, direction);

		//printf("turn OFF subdiv in section %d\n", sectionID);
		
		section = &TheTunnel[sectionID];
		
		for (j = 0; j < NUMBER_SHAPES_PER_SECTION; j++)
			{
			polygonID = section->polygonIDs[j];
			assert(ValidPolygonID(polygonID));

			SetMiniObjectSubdivision( &ThePolygons[polygonID], 0);
			}		
		}

		// make these subdivided
	for (i = 0; i < numberToChange; i++)
		{
		sectionID = GetRelativeSection (nextViewSection, i, direction);

		//printf("turn ON subdiv in section %d\n", sectionID);
		
		section = &TheTunnel[sectionID];
		
		for (j = 0; j < NUMBER_SHAPES_PER_SECTION; j++)
			{
			polygonID = section->polygonIDs[j];
			assert(ValidPolygonID(polygonID));

			SetMiniObjectSubdivision( &ThePolygons[polygonID], SUBDIVISION_EXTENT);
			}		
		}
}






	// regardless of direction, make the view section, ones before and after subdivided
	// clearly scope for optimisation here

void UpdateTunnelSubdivision2 (int viewSectionBefore, int viewSectionAfter)
{
	TunnelSection* section;
	int sectionID;
	int polygonID;
	int i, j;

	if (viewSectionBefore == viewSectionAfter)
		return;

		// make these no longer subdivided
	for (i = -1; i <= 1; i++)
		{
		switch(i)
			{
			case -1:	
				sectionID = GetRelativeSection (viewSectionBefore, -i, BACKWARDS);
				break;
			case 0: case 1:
				sectionID = GetRelativeSection (viewSectionBefore, i, FORWARDS);
				break;
			}

		printf("turn OFF subdiv in section %d\n", sectionID);
		
		section = &TheTunnel[sectionID];
		
		for (j = 0; j < NUMBER_SHAPES_PER_SECTION; j++)
			{
			polygonID = section->polygonIDs[j];
			assert(ValidPolygonID(polygonID));

			SetMiniObjectSubdivision( &ThePolygons[polygonID], 0);
			}		
		}

		// make these subdivided
	for (i = -1; i <= 1; i++)
		{
		switch(i)
			{
			case -1:	
				sectionID = GetRelativeSection (viewSectionAfter, -i, BACKWARDS);
				break;
			case 0: case 1:
				sectionID = GetRelativeSection (viewSectionAfter, i, FORWARDS);
				break;
			}

		printf("turn ON subdiv in section %d\n", sectionID);
		
		section = &TheTunnel[sectionID];
		
		for (j = 0; j < NUMBER_SHAPES_PER_SECTION; j++)
			{
			polygonID = section->polygonIDs[j];
			assert(ValidPolygonID(polygonID));

			SetMiniObjectSubdivision( &ThePolygons[polygonID], SUBDIVISION_EXTENT);
			}		
		}


#if 0
	//printf("turn OFF subdiv in section %d\n", viewSectionBefore);
	section = &TheTunnel[viewSectionBefore];
	for (j = 0; j < NUMBER_SHAPES_PER_SECTION; j++)
		{
		polygonID = section->polygonIDs[j];
		assert(ValidPolygonID(polygonID));

		SetMiniObjectSubdivision( &ThePolygons[polygonID], 0);
		}

	//printf("turn ON subdiv in section %d\n", viewSectionAfter);
	section = &TheTunnel[viewSectionAfter];
	for (j = 0; j < NUMBER_SHAPES_PER_SECTION; j++)
		{
		polygonID = section->polygonIDs[j];
		assert(ValidPolygonID(polygonID));

		SetMiniObjectSubdivision( &ThePolygons[polygonID], SUBDIVISION_EXTENT);
		}
#endif
}





	// this function does object-relative rotation only
void DealWithShipRotation (VECTOR* twist,
							GsCOORDINATE2* coordSystem, MATRIX* matrix)
{	
	MATRIX xMatrix, yMatrix, zMatrix;
	SVECTOR xVector, yVector, zVector;

		// ONLY allow one axis rotation per frame
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

		// tell GTE that coordinate system has been updated
	coordSystem->flg = 0;
}
			  



	// express velocity in section coordinates, then just read off z component
int FindWhichWayShipMovesInSection (ObjectHandler* object, int section)
{
	int direction;
	VECTOR relativeVelocity, tunnelVelocity;

	assert(ValidID(section));

		// firstly express velocity in world coords
	ApplyMatrixLV( &object->coord.coord, 
				&object->velocity, &relativeVelocity);

		// now express it in local tunnel coords
	ExpressSuperPointInSub( &relativeVelocity,
		&TheTunnel[section].coordinateData, &tunnelVelocity);

#if 0
	if (frameNumber % 30 == 0)
		{
		printf("ship velocity: world then tunnel sec\n");
		dumpVECTOR(&object->velocity);
		dumpVECTOR(&relativeVelocity);
		printf("ship coords then tunnel coords\n");
		dumpCOORD2(&object->coord);
		dumpCOORD2(&TheTunnel[section].coordinateData);
		}
#endif

		// inverted signs since our TMD points backwards
	if (tunnelVelocity.vz < 0)
		direction = BACKWARDS;
	else
		direction = FORWARDS;

	return direction;
}

	 



	// finds closest section to point, given an idea:
	// searches for SECTION_LOOKAHEAD sections, behind and ahead of originalSection
	// along the circular list of tunnel sections

	// NOTE: often know that the thing can only have moved a small number of sections
	// in a single move, and which way it went -> much less searching;
	
	// also, to detect if object inside section:
	// if X has section S1 as closest section centre, and X is inside the
	// circular limits of S1, then X must be inside S1

TunnelSection* FindClosestSectionToPoint (VECTOR* position, 
											int originalSection)
{
	TunnelSection* consideredSections[MAX_SECTIONS_CONSIDERED];
	int sectionID;
	int x, y, z;
	int distances[MAX_SECTIONS_CONSIDERED];
	int minDistance = -1, whichOne = -1;
	int numberConsidered;
	int furthestBack, furthestForward;
	VECTOR *sectionCentre;
	int i;

	assert(ValidID(originalSection));

	furthestBack = GetDisplacedSection(originalSection, -SECTION_LOOKAHEAD);
	furthestForward = GetDisplacedSection(originalSection, SECTION_LOOKAHEAD);
	numberConsidered = 2 * SECTION_LOOKAHEAD + 1;

	// NOTE: easy rewrite to make function simpler
	// just one loop; sectionID = GetDisplacedSection(originalSection, i-SECTION_LOOKAHEAD);

	for (i = 0; i < numberConsidered; i++)
		{
		sectionID = furthestBack + i;
		sectionID = ConvertIdToRealID(sectionID);
		assert(ValidID(sectionID));
		
		consideredSections[i] = &TheTunnel[sectionID];
		}

		// for each member of list, find distance from the ship
	for (i = 0; i < numberConsidered; i++)
		{
		sectionCentre = &consideredSections[i]->sectionCentre;

		x = sectionCentre->vx - position->vx;
		y = sectionCentre->vy - position->vy;
		z = sectionCentre->vz - position->vz;	  
			// should check that each is smaller than MAX_VECTOR_ELEMENT_SIZE for no overflow

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






	// only tests 3 sections: main, one before, one ahead

int FindSectionPointIsIn (VECTOR* point, int mainGuess)
{
	int section, behindID, forwardID;
	int behindDistance, forwardDistance, distance;
	VECTOR* sectionCentre;
	int x, y, z;

	assert(ValidID(mainGuess));

	sectionCentre = &TheTunnel[mainGuess].sectionCentre;
	x = sectionCentre->vx - point->vx;
	y = sectionCentre->vy - point->vy;
	z = sectionCentre->vz - point->vz;
	distance = (x * x) + (y * y) + (z * z);
	section = mainGuess;
	
	behindID = GetRelativeSection(mainGuess, 1, BACKWARDS);
	sectionCentre = &TheTunnel[behindID].sectionCentre;
	x = sectionCentre->vx - point->vx;
	y = sectionCentre->vy - point->vy;
	z = sectionCentre->vz - point->vz;
	behindDistance = (x * x) + (y * y) + (z * z);

	forwardID = GetRelativeSection(mainGuess, 1, FORWARDS);
	sectionCentre = &TheTunnel[forwardID].sectionCentre;
	x = sectionCentre->vx - point->vx;
	y = sectionCentre->vy - point->vy;
	z = sectionCentre->vz - point->vz;
	forwardDistance = (x * x) + (y * y) + (z * z);

	if (distance < forwardDistance)	   // forward further away than current or behind
		{
		if (distance <= behindDistance)
			section = mainGuess;
		else
			section	= behindID;
		}
	else		// forward closer than current, must be closest
		{
		section	= forwardID;
		}

	return section;
}




		  
int FindViewPointSection (int guess)
{
	int viewSection;
	VECTOR viewPoint;
	VECTOR viewVector;		 // from reference point to view point

	assert(TheView.super == &TheShip.coord);
	assert(ValidID(guess));

		// view vector is TheView.vpz along ship's z axis
	setVECTOR( &viewVector,
			((TheShip.coord.coord.m[0][2] * TheView.vpz) >> 12),
			((TheShip.coord.coord.m[1][2] * TheView.vpz) >> 12),
			((TheShip.coord.coord.m[2][2] * TheView.vpz) >> 12) );

		// reference point at ship itself, viewpoint is viewVector away ...
	setVECTOR( &viewPoint, 
			TheShip.position.vx + viewVector.vx,
			TheShip.position.vy + viewVector.vy,
			TheShip.position.vz + viewVector.vz);

	viewSection = FindSectionPointIsIn( &viewPoint, guess);

#if 0
	{
	printf("guess: %d, viewSection: %d\n", guess, viewSection);
	printf("ref point then view point\n");
	dumpVECTOR(&TheShip.position);
	dumpVECTOR(&viewPoint);
	}
#endif

	return viewSection;
}





int FindSectionViewpointIsIn (VECTOR* point, int sectionID)
{
	TunnelSection* section;
	int id;
	VECTOR view;

	assert(ValidID(sectionID));

	setVECTOR( &view, point->vx, point->vy, point->vz);
	view.vx += (TheShip.coord.coord.m[0][2] * TheView.vpz) >> 12;
	view.vy += (TheShip.coord.coord.m[1][2] * TheView.vpz) >> 12;
	view.vz += (TheShip.coord.coord.m[2][2] * TheView.vpz) >> 12;

	section = FindClosestSectionToPoint ( &view, sectionID);
	id = section->id;

	return id;
}





	// strong assumptions about view setup
void FindPositionAndDirectionOfView (VECTOR* position, VECTOR* direction)
{
	VECTOR worldReferencePos, worldViewingVector;

	assert(TheView.super == &TheShip.coord);

	setVECTOR (&worldReferencePos, 
			TheShip.position.vx,
			TheShip.position.vy,
			TheShip.position.vz);

	setVECTOR (direction, 
			-TheShip.coord.coord.m[0][2],
			-TheShip.coord.coord.m[1][2],
			-TheShip.coord.coord.m[2][2]);

	setVECTOR( &worldViewingVector,
			((direction->vx * TheView.vpz) >> 12),
			((direction->vy * TheView.vpz) >> 12),
			((direction->vz * TheView.vpz) >> 12) );

	setVECTOR( position, 
			worldReferencePos.vx + worldViewingVector.vx,
			worldReferencePos.vy + worldViewingVector.vy,
			worldReferencePos.vz + worldViewingVector.vz);

#if 0
	if (frameNumber % 30 == 0)
		{
		printf("vectors: ship pos, view dir, view pos, vec R to P\n");
		dumpVECTOR( &TheShip.position);
		dumpVECTOR(direction);		 
		dumpVECTOR(position);
		dumpVECTOR(&worldViewingVector);
		}
#endif
}

	
	
	
	// find the two sections which mark reasonable ends of visible
void FindVisibleSectionEnds (int section, int* end1, int* end2)
{
	VECTOR viewDirection, viewPos, relativeDirection;
	int direction;
	int roughGuide;
	int roughLimit = 1000000;	// 1000 squared

	assert(ValidID(section));

	FindPositionAndDirectionOfView( &viewPos,
						&viewDirection);

	ExpressSuperPointInSub( &viewDirection, 
		&TheTunnel[section].coordinateData,
			&relativeDirection);

	if (relativeDirection.vz > 0)
		direction = FORWARDS;
	else
		direction = BACKWARDS;		

	roughGuide = (relativeDirection.vx * relativeDirection.vx)
				+ (relativeDirection.vy * relativeDirection.vy);

	if (roughGuide < roughLimit)
		{
		printf("nearly straight\n");
		*end1 =	section;
		*end2 = GetRelativeSection(section, 
						NUMBER_SECTIONS_ALIVE, direction);
		}
	else
		{
		printf("well side on\n");
		*end1 = GetRelativeSection(section, 
						NUMBER_SECTIONS_ALIVE/2, BACKWARDS);
		*end2 = GetRelativeSection(section, 
						NUMBER_SECTIONS_ALIVE/2, FORWARDS);
		}

	//printf("view direction wrt tunnel \n");
	//dumpVECTOR( &relativeDirection);
}





int FindWhichWayViewLooksInSection (int section)
{
	int direction;

	VECTOR viewDirection, viewPos, relativeDirection;

	assert(ValidID(section));

	FindPositionAndDirectionOfView( &viewPos,
						&viewDirection);

	ExpressSuperPointInSub( &viewDirection, 
		&TheTunnel[section].coordinateData,
			&relativeDirection);

	if (relativeDirection.vz > 0)
		direction = FORWARDS;
	else
		direction = BACKWARDS;

	return direction;
}	


  




int GetRelativeSection (int first, int numberOn, int direction)
{
	int final;

	assert(ValidID(first));
	assert(numberOn >= 0);

	switch (direction)
		{
		case FORWARDS:		
			final = first + numberOn;
			break;
		case BACKWARDS:		
			final = first - numberOn;
			break;
		default:	
			assert(FALSE);
		}

	final = ConvertIdToRealID(final);

	return final;
}
			




	// tunnel section IDs are a circular list
int GetDisplacedSection (int current, int N)
{
	int displaced;

	assert(ValidID(current));
	assert(abs(N) < NumberTunnelSections);
		
	displaced = current + N;
	displaced = ConvertIdToRealID(displaced);

	assert(ValidID(displaced));

	return displaced;
}



	// wrap into valid list index
int ConvertIdToRealID (int id)
{
	int realID = id;

	while (realID >= NumberTunnelSections)
		realID -= NumberTunnelSections;
	while (realID < 0)
		realID += NumberTunnelSections;

	assert(ValidID(realID));

	return realID;
}



int DistanceBetweenSections (int first, int second, int direction)
{
	int distance;

	assert(ValidID(first));
	assert(ValidID(second));
	
	switch (direction)
		{
		case FORWARDS:
			if (first <= second)
				{
				distance = second - first;
				}
			else
				{
				distance = second - first + NumberTunnelSections;
				}
			break;
		case BACKWARDS:
			if (first >= second)
				{
				distance = first - second;
				}
			else
				{
				distance = first - second + NumberTunnelSections;
				}
			break;
		default:	
			assert(FALSE);		  // invalid direction
		}

	return distance;
}
	 





void FindShortestDistance (int first, int second, 
			int* distance, int* direction)
{
	int simpleDist;
	int halfDistance = NumberTunnelSections/2;

	assert(ValidID(first));
	assert(ValidID(second));

	simpleDist = first - second;

	if (simpleDist == 0)			  // first larger
		{
		*distance = 0;
		*direction = -1;
		return;
		}
	else if (simpleDist > 0)			  // first larger
		{
		if (first - second >= halfDistance)
			{
			*distance = NumberTunnelSections - first + second;
			*direction = BACKWARDS;
			}
		else
			{
			*distance = first - second;
			*direction = FORWARDS;
			return;
			}
		}
	else
		{
		if (second - first >= halfDistance)
			{
			*distance = NumberTunnelSections - second + first;
			*direction = BACKWARDS;
			}
		else
			{
			*distance = second - first;
			*direction = FORWARDS;
			return;
			}

		}
}
		  




	// here: we only test new position, assume old one is OK, know that from the setting up
	// express position in 		   

int DetectCollisionWithTunnel (ObjectHandler* object, 
		int nextSection, VECTOR* newPos)
{
	int booleanFlag = FALSE;
	VECTOR worldPos, *splinePoint;
	VECTOR relativePos;
	int effectiveRadius;

	assert(ValidID(nextSection));

	splinePoint = &TheTunnel[nextSection].splinePoint;

	setVECTOR( &worldPos, 
		newPos->vx - splinePoint->vx,
		newPos->vy - splinePoint->vy,
		newPos->vz - splinePoint->vz);

		// express final ship position in local section coords
	ExpressSuperPointInSub( &worldPos, 
		&TheTunnel[nextSection].coordinateData, &relativePos);

	effectiveRadius = TunnelMiddleRadius - object->collisionRadius;

	assert(abs(relativePos.vx) < MAX_VECTOR_ELEMENT_SIZE);
	assert(abs(relativePos.vy) < MAX_VECTOR_ELEMENT_SIZE);
		
		// now: standard circle-centred-at-origin comparison
		// ie x**2 + y**2 >= r**2 ??
	if ( ((relativePos.vx * relativePos.vx) +	
			(relativePos.vy * relativePos.vy)) >= (effectiveRadius * effectiveRadius) )
		booleanFlag = TRUE; 

#if 0
	if (frameNumber % 30 == 0)
		{
		printf("eff. radius %d\n", effectiveRadius);
		printf("ship new pos: world then tunnel\n");
		dumpVECTOR(newPos);
		dumpVECTOR(&relativePos);
		printf("bool: %d\n", boolean);
		}
#endif
	
	return booleanFlag;
}
			
			
			




void HandleCollisionWithTunnel (ObjectHandler* object)
{
	printf("object id %d type %d in collision with tunnel\n",
		object->id, object->type);
}