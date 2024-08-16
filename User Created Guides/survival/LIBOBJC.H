/*
 * LIBOBJC.H
 *
 * Copyright (C) 1993-1997 by Sony Computer Entertainment of America, Inc.
 * All rights Reserved
 * ----------------------------------------------------------------------------
 */

#ifndef	__YAROZE_LIBOBJC_H
#define	__YAROZE_LIBOBJC_H

/*
 * This file contains data structure to handle various objects in the world.
 * ----------------------------------------------------------------------------
 */

#include "libmain.h"

/*
 * Macros for screen position and near/far clipping
 * ----------------------------------------------------------------------------
 */

/* Prjoection plane is at this distance in front of the camera */

#define SCR_Z				512

/* Near and Far clipping points on z axis */

#define	CLP_NEAR			128
#define	CLP_FAR				(2048*6)

/*
 * Flag for game control status
 * ----------------------------------------------------------------------------
 */

#define	GAME_OVER			0					// Game is over
#define	GAME_INTRO			1					// Game is in intro sequence
#define	GAME_DEMO			2					// Game is in demo mode
#define	GAME_PLAY			3					// Game is in play mode
#define	GAME_PAUSE			4					// Game is in pause mode
#define	GAME_SETTINGS		5					// Game is in user settings mode

/*
 * Display buffers/Font buffer related definitions
 * ----------------------------------------------------------------------------
 */

/*
 * Frame buffer (x,y,w,h) for the two buffers used to implement
 * the double buffering for flicker free frame generation.
 */

#define	DISP_BUFF1_X		0
#define	DISP_BUFF1_Y		0
#define	DISP_BUFF2_X		0
#define	DISP_BUFF2_Y		240
#define	DISP_BUFF_W			320
#define	DISP_BUFF_H			240

/* Frame buffer (x,y) to store the font data */

#define	FONT_BUFF_X			960
#define	FONT_BUFF_Y			256

/*
 * Macros used for the font sub-system - like the display window
 * (x,y,w,h) on screen, string buffer and background information.
 */

#define	FONT_DISP_X			(16-(DISP_BUFF_W/2))
#define	FONT_DISP_Y			(16-(DISP_BUFF_H/2))
#define	FONT_DISP_W			(DISP_BUFF_W-(16*2))
#define	FONT_DISP_H			(DISP_BUFF_H-(16*2))
#define	FONT_DISP_ISBG		0
#define	FONT_DISP_NCHARS	1024

/*
 * Ordering table and primitives related variables
 * ----------------------------------------------------------------------------
 */

#define OT_LENGTH			12					// Ordering table resolution (4096)
#define OT_SIZE				(1<<OT_LENGTH)		// Ordering table tag size (4096)

#define MAX_PACKET			0x4000				// Maximum number of packets per frame
#define MAX_PACKET_SIZE		(MAX_PACKET * 24)	// Average packet size is 24

typedef struct __db {
	GsOT		OT;								// Ordering table header
	GsOT_TAG	OTTags[OT_SIZE];				// Ordering table
	PACKET		GpuPacketArea[MAX_PACKET_SIZE];	// GPU packet area
} DB;

/*
 * Lighting mode and background color
 * ----------------------------------------------------------------------------
 */

#define	BGCOL_R				0x00			   	// Background color - r component
#define	BGCOL_G				0x00				// Background color - g component
#define	BGCOL_B				0x00				// Background color - b component

/*
 * The following structure handles all models/textures used in the
 * program.  It contains the model (TMD) data and the texture map
 * adresses in memory.  It also contains the maximum length of the
 * edge of the bounding box for the model along x,y,z axes.  The
 * various routines in this sample assume that the edges along x
 * amd z axes should be equal in length.
 * ----------------------------------------------------------------------------
 */

typedef struct __model {
	unsigned long	*ModelPtr;					// Ptr to TMD model data in memory
	unsigned long	*TexturePtr;				// Ptr to TIM texture data in memory
	short			LenX;						// Maximum length of the object along x axix / 2
	short			LenY;						// Maximum length of the object along y axix / 2
	short			LenZ;						// Maximum length of the object along z axix / 2
	short			Edge;						// Length of edge of the cube enclosing this object
} MODEL;

/*
 * A data structure to handle various objects in the world
 * ----------------------------------------------------------------------------
 */

/*
 * A position FIFO is used with each object.  This FIFO contains
 * the position of the object from last given number of frames.
 * This FIFO is used to calculate the current state of the object
 * depending upon its status in last given number of frames.
 */

#define	POSFIFO_LEN			4

/*
 * A data structure to handle various objects in the world
 */
	
typedef struct __object {
	/* Various status and attribute flags of an object */
	short			State;						// Current state of the object
	
	/* No of lives for this object - mainly used for Robo/Enemy */
	short			Lives;						// Number of lives for this guy
		
	/* Type of the object */
	short			Type;						// Type of object
	
	/* Position FIFO handling for this object */
	short			PosFIFOTop;					// Top of position FIFO
	SVECTOR			PosFIFO[ POSFIFO_LEN ];		// Position FIFO for the object
	
	/* TMD data model pointer in memory used for this object */
	MODEL			*Model;						// Model used for this object
	
	/* PlayStation library object handler for this object */ 
	GsDOBJ2			Object;						// Playstation object handler
	
	/* Inclination of the object in the world */
	SVECTOR			Angle;						// Object inclination in world
	
	/* PlayStation library object position handler for this object */
	GsCOORDINATE2	Pos;						// Object coordinate system
	
	/*
	 * Distance of edges of the bounding box for this object.
	 * The source code assumes that the centroid of the object
	 * is in the center of the object in (x,y,z) directions.
	 */
	short			DifX, DifY, DifZ;			// Distance of the boiunding box edge from the origin
	
	/*
	 * Minimum and maximum extents of the bounding box for
	 * this object in the world.
	 */
	short			MinX, MinY, MinZ;			// Minimum x,y,z extents of the object
	short			MaxX, MaxY, MaxZ; 			// Maximum x,y,z extents of the object
	
	/*
	 * Speed of the object in forward direction - ion x-z plane
	 * We also keep the object's speed in all three directions
	 * This will help us simplify the object's movement in the
	 * world.
	 *
	 * Currently the speed along x and z directions are being
	 * used only for the enemy remains.
	 */
	short			SpeedForward;				// Speed in forward direction
	short			SpeedX;						// Speed in x direction
	short			SpeedY;						// Speed in vertical direction
	short			SpeedZ;						// Speed in z direction
	
	/*
	 * The following are used to rotate an object around its
	 * y axis - specially used for the propeller.  The angular
	 * speed and the # frames after which to play the next
	 * sound effect are stored here.
	 */
	short			SpeedAngleY;				// Angular speed around y axes
	short			SpeedAngleYPlaySfx;			// Play sound effect after these # frames
	
	/*
	 * The following fields are used for computer controlled
	 * objects.  The following provide the random time intervals
	 * and parameters to change various attributes of the object
	 * involved.
	 */
	short			ChangeStateCounter;			// Counter to change object's state
	short			ChangeStateThreshold;		// Threshold to change object's state
	short			ChangeParameterCounter;		// Counter to change object's parameters
	short			ChangeParameterThreshold;	// Threshold to change object's parameters
	short			AuxilliaryVarOne;			// Auxialliary variable for whatever
	short			AuxilliaryVarTwo;			// Auxialliary variable for whatever
} OBJECT;

/*
 * Various states of an object in terms of changing position in (x,y,z)
 * direction.  These flags are used in conjunction with object movement
 * and it's status like whether it is stationary or moving in either
 * of x,y or z directions.  These flags are useful in deciding collision
 * detection and mechanics of objects when they collide with each other
 * or the boundary walls of the world.
 */

#define	STATE_XYZ_MASK		0x000F				// Objects (x,y,z) are changing
#define	STATE_STATIONARY	0x0000				// Object is in stationary state
#define	STATE_X_CHANGING	0x0001				// Object is changing position along x direction
#define	STATE_Y_CHANGING	0x0002				// Object is changing position along y direction
#define	STATE_Z_CHANGING	0x0004				// Object is changing position along z direction

/*
 * Various states an object can be in.  These are used for objects
 * controlled by the computer - namely enemy objects.  These macros
 * are used to implement the AI for these objects.
 */

#define	STATE_ENMY_MASK		0x00F0				// Mask for state of an object

#define STATE_ENMY_DEAD		0x0000				// Object is in dead
#define STATE_ENMY_DYING	0x0010				// Object is dying
#define STATE_ENMY_NEWSTATE	0x0020				// Object wishes a new state

#define STATE_ENMY_ATTACK	0x0030				// Object is attacking player
#define STATE_ENMY_RANDOM	0x0040				// Object is moving in random directions
#define STATE_ENMY_EVADE	0x0050				// Object is evading player
#define STATE_ENMY_STOP		0x0060				// Object is at a full stop
#define STATE_ENMY_TURN		0x0070				// Object is making a turn

/*
 * States whether the automatic division to be performed on
 * the polygons comprising this object.  Automatic division
 * flag of a GsDOBJ2 structure of such objects are set when
 * another object gets near them.  This is to avoid clipping
 * problems when smaller object's like the Robot or the enemy
 * get near a wall or a slab object and they seem to hide
 * behind the polygons comprising the Waall or the Slab.
 *
 * This flag is set for objects usually comprising of large
 * polygons.  The reason being the sorting of polygons in
 * such cases create a large error in z sorting.
 */

#define	STATE_DIVISIBLE		0x8000				// Object is divisible

/*
 * This tells us if the object is alive and to be considered
 * for various operations in the world.  All routines working
 * on the object structure check for this flag to decide whether
 * to perform the operation on it or not.
 */
#define	STATE_ALIVE			0x4000				// Object is alive

/*
 * Masks for collision routines
 * The collision routines return the flags which tell us
 * on which side of the bounding box the collision actually
 * occurred.
 */

#define		COLLISION_CLEAR	0x0000				// Clear the collision flags

#define		NEAR_MINX		0x0001				// Mask used for polygon division when near minimum x
#define		NEAR_MAXX		0x0002				// Mask used for polygon division when near maximum x
#define		NEAR_MINY		0x0004				// Mask used for polygon division when near maximum y
#define		NEAR_MAXY		0x0008				// Mask used for polygon division when near maximum y
#define		NEAR_MINZ		0x0010				// Mask used for polygon division when near maximum z
#define		NEAR_MAXZ		0x0020				// Mask used for polygon division when near maximum z

#define		COLLISION_MINX	0x0100				// Mask used by collision routines when on minimum x
#define		COLLISION_MAXX	0x0200				// Mask used by collision routines when on maximum x
#define		COLLISION_MINY	0x0400				// Mask used by collision routines when on minimum y
#define		COLLISION_MAXY	0x0800				// Mask used by collision routines when on maximum y
#define		COLLISION_MINZ	0x1000				// Mask used by collision routines when on minimum z
#define		COLLISION_MAXZ	0x2000				// Mask used by collision routines when on maximum z

#define		COLLISION_X		(COLLISION_MINX | COLLISION_MAXX)
#define		COLLISION_Y		(COLLISION_MINY | COLLISION_MAXY)
#define		COLLISION_Z		(COLLISION_MINZ | COLLISION_MAXZ)

#define		COLLISION_HORZ	(COLLISION_X | COLLISION_Z)
#define		COLLISION_VERT	(COLLISION_Y)

#define		COLLISION_XYZ	(COLLISION_X | COLLISION_Y | COLLISION_Z)

/*
 * Macros used for setting automatic division when a floating
 * object gets near another divisible object
 */

#define		DIV_THRESHOLD	16					// Proximity threshold for polygon division on objects
#define		DIV_MASK_CLEAR	0xFFFFF1FF			// Mask to clear off the division flags

/*
 * A data structure to handle various enemies AI
 * ----------------------------------------------------------------------------
 */

/*
 * The following provides various parameters needed for effecting the
 * various paraterems for a computer controlled object.   These are used
 * to calculate various object parameters on a randomly manner along with
 * some minimum values for each parameter.
 *
 * These parameters describe an object's behavious in the game.  A seperate
 * table can be constructed for each object with predefined values for
 * minimum and a maximum value possible.
 *
 * The table consists of a following entry per state the object can acquire.
 * Whereever an object is waiting to acquire a new state, the following
 * table is used along with a random distribution table for the new state.
 * The object then gets assigned the new state along with the following
 * paratemets and the object behaves accordingly until the assigned
 * state expires.
 *
 * Please look into HandleEnmyState(...) in main.c for detailed
 * implementation of an object's AI using the following table.
 */

typedef struct __ai {
	short State;								// The new state for an object
	
	short StateThresholdMin;					// Minimum threshold for maitaining a state
	short StateThresholdRand;					// Random part of threshold for maintaining a state
	
	short ParameterThresholdMin;				// Minimum threshold for parameter change
	short ParameterThresholdRand;				// Random part of threshold for parameter change
	
	short AuxilliaryVarOneMin;					// Minimum value for auxilliary variable 
	short AuxilliaryVarOneRand;					// Random part of value for auxilliary variable 
	
	short AuxilliaryVarTwoMin;					// Minimum value for auxilliary variable 
	short AuxilliaryVarTwoRand;					// Random part of value for auxilliary variable 
	
	short SpeedYMin;							// Minimum value for speed in vertical direction
	short SpeedYRand;							// Random part of value for speed in vertical direction
	
	short SpeedForwardMin;						// Minimum value for speed in forward direction
	short SpeedForwardRand;						// Random part of value for speed in forward direction
} AI;
		
/*
 * Function templates
 * ----------------------------------------------------------------------------
 */

extern void  InitObjc(OBJECT *objc, GsCOORDINATE2 *super, short x, short y, short z);

extern void  HandleSetCoordinate(OBJECT *objc);

extern void  DrawObject(OBJECT *objc, GsOT * ot);

extern short HandlePosFIFO(OBJECT *objc);

extern short HandleObjcCollision(OBJECT *objc, OBJECT *bloc, short *dx,short *dy,short *dz);
extern short HandleWallCollision(OBJECT *objc, OBJECT *wall, short *dx,short *dy,short *dz);

extern void  HandleObjcDivision(OBJECT *objc, OBJECT *bloc);
extern void  HandleWallDivision(OBJECT *objc, OBJECT *wall);

/*
 * End of File
 * ----------------------------------------------------------------------------
 */

#endif
