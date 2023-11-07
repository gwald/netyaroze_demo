#include "tunnel.h"





int NumberOfCreatedTMDs = 0;
u_long CurrentTMDStackAddress = START_OF_CREATED_TMDS_STACK;

u_long* CreatedTMDPointer = (u_long*) CREATED_TMD_ADDRESS;



int NumberTunnelSections;

int TunnelSectionShapeAngle;

int TunnelSectionLength;
int TunnelSectionRadius;



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

		// main tunnel data initialisations
	NumberTunnelSections = 8;
	assert(NumberTunnelSections <= MAX_TUNNEL_SECTIONS);

	TunnelSectionShapeAngle = ONE / NUMBER_SHAPES_PER_SECTION;
	assert(TunnelSectionShapeAngle < ONE/4);		// must be less than PI/2

	TunnelSectionLength = SQUARE_SIZE * 16;
	TunnelSectionRadius = SQUARE_SIZE * 8;

		// HERE: create tunnel data itself

		// generate spline from list of rotation vectors
	CreateFirstSpline();

	FindSplineCentralPoints();
	
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
		setVECTOR( &thisSection->sectionCentre, 0, 0, 0);

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
	VECTOR* point;
	TunnelSection* thisSection;

	for (i = 0; i < NumberTunnelSections; i++)
		{
		thisSection = &TheTunnel[i];
		coord = &thisSection->coordinateData;

			// get the x and y axes of this tunnel section, in world coord terms
		setVECTOR( &xAxis, coord->coord.m[0][0], coord->coord.m[0][1], coord->coord.m[0][2]);
		setVECTOR( &yAxis, coord->coord.m[1][0], coord->coord.m[1][1], coord->coord.m[1][2]);
		
		#if 0
		printf("x axis then y axis\n");
		dumpVECTOR(&xAxis);
		dumpVECTOR(&yAxis);
		//printf("sizes: x %d y %d\n", SizeOfVector(&xAxis), SizeOfVector(&yAxis) );
		printf("section coord\n");
		dumpCOORD2(coord);
		#endif

			// find x and y on circle, know x and y axes, hence get xyz in world
		for (j = 0; j < NUMBER_SHAPES_PER_SECTION; j++)
			{
			angle = j * TunnelSectionShapeAngle; 
			point = &thisSection->pointsOnCircle[j];

			circleX = (TunnelSectionRadius * rcos(angle)) >> 12;	  // x = r.cos(theta)
			circleY = (TunnelSectionRadius * rsin(angle)) >> 12;	  // y = r.sin(theta)

			worldPosition.vx = thisSection->splinePoint.vx
								+ ( ((circleX * xAxis.vx) + (circleY * yAxis.vx)) >> 12);
			worldPosition.vy = thisSection->splinePoint.vy
								+ ( ((circleX * xAxis.vy) + (circleY * yAxis.vy)) >> 12);
			worldPosition.vz = thisSection->splinePoint.vz
								+ ( ((circleX * xAxis.vz) + (circleY * yAxis.vz)) >> 12);

			setVECTOR(point, worldPosition.vx,	
				worldPosition.vy, worldPosition.vz);

			//dumpVECTOR(point);
			}

#if 0
		for (j = 0; j < NUMBER_SHAPES_PER_SECTION/2; j++)
			{
			VECTOR *point1, *point2, point3;

			point1 = &thisSection->pointsOnCircle[j];
			point2 = &thisSection->pointsOnCircle[j+NUMBER_SHAPES_PER_SECTION/2];
			setVECTOR( &point3, point2->vx - point1->vx,
				point2->vy - point1->vy, point2->vz - point1->vz);
			dumpVECTOR(&point3);
			printf("size of dist across circle: %d\n", SizeOfVector(&point3) );
			}
#endif
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

				// no rotation, i.e. same coord system as previous section
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

void FindCompoundRotationAngles (int dx, int dy, int dz, int* thetaX, int* thetaY)
{
	int angleX = 0, angleY = 0;
	int newDX, newDY;
	GsCOORDINATE2 coord, temp, semiFixed;
	SVECTOR rotationVector;
	VECTOR originalPosition, oldPosition, newPosition;
	MATRIX tempMatrix;

		// rotate firstly y, then x; i.e. we firstly rotate left/right until point
		// is roughly in our y axis, then rotate up/down until it is nearly dead ahead
	assert(dx != 0 && dy != 0);

	GsInitCoordinate2(WORLD, &coord);
	GsInitCoordinate2(WORLD, &temp);
	GsInitCoordinate2(WORLD, &semiFixed);

	setVECTOR( &originalPosition, dx, dy, dz);

	printf("original position:\n");
	dumpVECTOR(&originalPosition);

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
			printf("newDX: %d, old DX: %d\n", newDX, originalPosition.vx);
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
			printf("newDX: %d, old DX: %d\n", newDX, originalPosition.vx);
			assert(newDX >= originalPosition.vx);		// must be getting closer
			}
		}

		// now our first rotation is fixed;
		// semiFixed holds our
	*thetaY = angleY;
	printf("angleY: %d\n", angleY);
	setVECTOR( &rotationVector, 0, *thetaY, 0);
	GsInitCoordinate2(WORLD, &coord);		// start centred
	DeriveNewCoordSystemFromRotation( &coord, &rotationVector, &semiFixed);
	printf("semiFixed coord:\n");
	dumpCOORD2(&semiFixed);
		// 'old'position: where point appears to be from our interim state semiFixed,
		// before we try thetaX rotations
	ConvertPointWorldToObject (&originalPosition, 
							&semiFixed, &oldPosition);
	printf("original position:\n");
	dumpVECTOR(&originalPosition);
	printf("oldPosition:\n");
	dumpVECTOR(&oldPosition);


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
			printf("newDY: %d, old DY: %d\n", newDY, oldPosition.vy);
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
			printf("newDY: %d, old DY: %d\n", newDY, oldPosition.vy);
			assert(newDY >= oldPosition.vy);		// must be getting closer
			}
		}

	*thetaX = angleX;
	printf("angleX: %d\n", angleX);
}


  


	// Note: last rotation in rotationList is left unused

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

	for (i = 0; i < NumberTunnelSections-1; i++)
		{
		currentPoint = &TheTunnel[i].splinePoint;
		nextPoint = &TheTunnel[i+1].splinePoint;

		setVECTOR( &TheTunnel[i].sectionCentre, 
			currentPoint->vx + ((nextPoint->vx - currentPoint->vx)/2),
			currentPoint->vy + ((nextPoint->vy - currentPoint->vy)/2), 
			currentPoint->vz + ((nextPoint->vz - currentPoint->vz)/2) );
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
	u_long address;
	TMD_NORM normal;
	VECTOR firstSide, secondSide, crossProduct;
	VECTOR polyCentre, normalVector;
	int clutX, clutY;
	VECTOR xAxis, yAxis;
	int angle, circleX, circleY;
	VECTOR inCoordPoint;
	GsCOORDINATE2* coord;


#if 0				// cubes at poly vertices to test tunnel shape
	for (i = 0; i < NumberTunnelSections; i++)
		{
		ObjectHandler* cube = NULL;
		thisSection = &TheTunnel[i];

		for (index = 0; index < NUMBER_SHAPES_PER_SECTION; index++)
			{
			p1 = &thisSection->pointsOnCircle[index];

			{
			int k;

			for (k = 0; k < MAX_CUBES; k++)
				{
				if (TheCubes[k].alive == FALSE)
					{
					cube = &TheCubes[k];
					break;
					}
				}
			assert(cube != NULL);
			}
				
			setVECTOR( &cube->position, p1->vx, p1->vy, p1->vz);
			UpdateObjectCoordinates2 ( &cube->rotate,
							&cube->position, &cube->coord);

			SetObjectScaling(cube, ONE/8, ONE/8, ONE/8);
			SortObjectSize(cube);
			cube->alive = TRUE;
			}
		}
#endif



		// i limits: don't link first and last section together
	for (i = 0; i < NumberTunnelSections-1; i++)
		{
		thisSection = &TheTunnel[i];
		nextSection = &TheTunnel[i+1];
		coord = &thisSection->coordinateData;

			// get the x and y axes of this tunnel section, in world coord terms
		setVECTOR( &xAxis, coord->coord.m[0][0], coord->coord.m[0][1], coord->coord.m[0][2]);
		setVECTOR( &yAxis, coord->coord.m[1][0], coord->coord.m[1][1], coord->coord.m[1][2]);

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

			//printf("poly vertices 1 to 4\n");
			//dumpVECTOR(p1); dumpVECTOR(p2); dumpVECTOR(p3); dumpVECTOR(p4);

			setVERTEX( &v1, p1->vx, p1->vy, p1->vz);
			setVERTEX( &v2, p2->vx, p2->vy, p2->vz);
			setVERTEX( &v3, p3->vx, p3->vy, p3->vz);
			setVERTEX( &v4, p4->vx, p4->vy, p4->vz);
 
			angle = index * TunnelSectionShapeAngle; 

			circleX = (TunnelSectionRadius * rcos(angle)) >> 12;	  // x = r.cos(theta)
			circleY = (TunnelSectionRadius * rsin(angle)) >> 12;	  // y = r.sin(theta)

			inCoordPoint.vx = thisSection->splinePoint.vx
								+ ( ((circleX * xAxis.vx) + (circleY * yAxis.vx)) >> 12);
			inCoordPoint.vy = thisSection->splinePoint.vy
								+ ( ((circleX * xAxis.vy) + (circleY * yAxis.vy)) >> 12);
			inCoordPoint.vz = thisSection->splinePoint.vz
								+ ( ((circleX * xAxis.vz) + (circleY * yAxis.vz)) >> 12);

				// normal vector: from centre of poly to centre of section
				// ==> poly always visible from inside section
			setVECTOR( &normalVector, (thisSection->splinePoint.vx - inCoordPoint.vx), 
							(thisSection->splinePoint.vy - inCoordPoint.vy),
							(thisSection->splinePoint.vz - inCoordPoint.vz) );
			ScaleVectorToUnit (&normalVector);
			//printf("normal :-\n");
			//dumpVECTOR(&normalVector);
			setNORMAL( &normal, normalVector.vx, 
				normalVector.vy, normalVector.vz);
			

#if 0
				// find centre of polygon
			setVECTOR( &polyCentre, ((p1->vx + p2->vx + p3->vx + p4->vx) / 4),
									((p1->vy + p2->vy + p3->vy + p4->vy) / 4),
									((p1->vz + p2->vz + p3->vz + p4->vz) / 4) );

			printf("polygon centre :-\n");
			dumpVECTOR(&polyCentre);
			printf("section centre :-\n");
			dumpVECTOR(&thisSection->sectionCentre); 

				// normal vector: from centre of poly to centre of section
				// ==> poly always visible from inside section
			setVECTOR( &normalVector, (thisSection->sectionCentre.vx - polyCentre.vx), 
							(thisSection->sectionCentre.vy - polyCentre.vy),
							(thisSection->sectionCentre.vz - polyCentre.vz) );
			ScaleVectorToUnit (&normalVector);
			printf("normal :-\n");
			dumpVECTOR(&normalVector);
			setNORMAL( &normal, normalVector.vx, 
				normalVector.vy, normalVector.vz);
#endif




#if 0	 // old way of calculating normal
				// side of poly from vertex 0 to vertex 1
			setVECTOR(&firstSide, p2->vx - p1->vx,
								p2->vy - p1->vy,
								p2->vz - p1->vz);
			ScaleVectorToUnit (&firstSide);

				// side of poly from vertex 0 to vertex 2
			setVECTOR(&secondSide, p3->vx - p2->vx,
								p3->vy - p2->vy,
								p3->vz - p2->vz);
			ScaleVectorToUnit (&secondSide);

			GetRightHandedNormal( &firstSide, &secondSide, &crossProduct);
			ScaleVectorToUnit (&crossProduct);

			printf("i: %d, index: %d\n", i, index);
			dumpVECTOR(&crossProduct);
				
			setNORMAL( &normal, crossProduct.vx, 
				crossProduct.vy, crossProduct.vz);
#endif

			polygon = GetNextFreePolygon();
			assert(polygon != NULL); 
		
			clutX = 0;
			if (index < NUMBER_SHAPES_PER_SECTION/2)
				clutY = 480;
			else
				clutY = 482;
			
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
			polygon->handler.coord2 = WORLD;
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

	assert(address < END_OF_CREATED_TMDS_STACK);		// start of program itself ...

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

