#ifndef GEOM_H_INCLUDED



#define CLOCKWISE 0
#define ANTICLOCKWISE 1

#define NORTH 0
#define EAST 1
#define SOUTH 2
#define WEST 3



#define X_Y_PLANE 65537
#define X_Z_PLANE 65538
#define Y_Z_PLANE 65539


#define X_EQUALS_Y_PLANE 65540
#define X_EQUALS_MINUS_Y_PLANE 65541
#define X_EQUALS_Z_PLANE 65542
#define X_EQUALS_MINUS_Z_PLANE 65543
#define Y_EQUALS_Z_PLANE 65544
#define Y_EQUALS_MINUS_Z_PLANE 65545


#define PLUS_X_Y_PLANE 0
#define MINUS_X_Y_PLANE 1
#define PLUS_X_Z_PLANE 2
#define MINUS_X_Z_PLANE 3
#define PLUS_Y_Z_PLANE 4
#define MINUS_Y_Z_PLANE 5


#define TOP (MINUS_X_Z_PLANE)
#define FRONT (MINUS_X_Y_PLANE)
#define RIGHT (PLUS_Y_Z_PLANE)
#define BACK (PLUS_X_Y_PLANE)
#define LEFT (MINUS_Y_Z_PLANE)
#define BOTTOM (PLUS_X_Z_PLANE)




#define X_AXIS 65540
#define Y_AXIS 65541
#define Z_AXIS 65542

#define PLUS_X_AXIS 4
#define MINUS_X_AXIS 5
#define PLUS_Y_AXIS 6
#define MINUS_Y_AXIS 7
#define PLUS_Z_AXIS 8
#define MINUS_Z_AXIS 9




#define FRONT_LEFT_TOP_CORNER 0
#define FRONT_LEFT_BOTTOM_CORNER 1
#define FRONT_RIGHT_TOP_CORNER 2
#define FRONT_RIGHT_BOTTOM_CORNER 3
#define BACK_LEFT_TOP_CORNER 4
#define BACK_LEFT_BOTTOM_CORNER 5
#define BACK_RIGHT_TOP_CORNER 6
#define BACK_RIGHT_BOTTOM_CORNER 7




#define SQUARE_ROOT_OF_THREE 1.732050807569
#define SQUARE_ROOT_OF_THREE_TIMES_A_THOUSAND 1732




#define GEOM_H_INCLUDED 1

#endif
