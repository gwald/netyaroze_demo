#ifndef GEOM_H_INCLUDED



#define NORTH 0
#define EAST 1
#define SOUTH 2
#define WEST 3



#define X_Y_PLANE 65537
#define X_Z_PLANE 65538
#define Y_Z_PLANE 65539


#define PLUS_X_Y_PLANE 0
#define MINUS_X_Y_PLANE 1
#define PLUS_X_Z_PLANE 2
#define MINUS_X_Z_PLANE 3
#define PLUS_Y_Z_PLANE 4
#define MINUS_Y_Z_PLANE 5


#define FRONT (MINUS_X_Y_PLANE)
#define BACK (PLUS_X_Y_PLANE)
#define TOP (MINUS_X_Z_PLANE)
#define BOTTOM (PLUS_X_Z_PLANE)
#define RIGHT (PLUS_Y_Z_PLANE)
#define LEFT (MINUS_Y_Z_PLANE)





#define X_AXIS 65540
#define Y_AXIS 65541
#define Z_AXIS 65542

#define PLUS_X_AXIS 4
#define MINUS_X_AXIS 5
#define PLUS_Y_AXIS 6
#define MINUS_Y_AXIS 7
#define PLUS_Z_AXIS 8
#define MINUS_Z_AXIS 9




#define GEOM_H_INCLUDED 1

#endif
