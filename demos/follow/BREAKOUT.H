#ifndef BREAKOUT_H_INCLUDED







#define SIXTEEN_BIT_MASK (1 << 25)
#define EIGHT_BIT_MASK (1 << 24)



#define PLUS 1
#define MINUS -1

#define SIGN_OF(number)  (((number) >= 0) ? PLUS : MINUS)





#define MAX_BRICKS 200

#define MAX_BREAKOUT_OBJECTS (MAX_BRICKS + 2)		
	// bricks + player's bat + ball

	// allow 24 bytes per primitive	for GPU work area
#define MAX_SIZEOF_PRIMITIVE 24



#define SCREEN_WIDTH 320 
#define SCREEN_HEIGHT 240

	// set up arbitrary walls
#define MIN_X 16
#define MAX_X 256
#define MIN_Y 30
#define MAX_Y 196

	// offset to make screen look right
#define SCREEN_X_OFFSET 0
#define SCREEN_Y_OFFSET 16


	// not exactly a three-D game ...
	// hence only 1 z-position in ordering table
	// ie no actual z-discrimination at all
#define BREAKOUT_OT_LENGTH 2

	// maximum speeds at which player's bat can move
#define MAX_PLAYER_X_SPEED 4
#define MAX_PLAYER_Y_SPEED	0

#define PLAYER_SPEED 8

#define MAX_BALL_X_SPEED 5
#define MAX_BALL_Y_SPEED 5


   





	  	 
typedef struct 
{
	int x, y;

	int width, height;

	int dx, dy;
} MovementData;




//#define NONE 32766


#define FULL_BRICK 100
#define HALF_BRICK 101




typedef struct
{
	int id;
	int alive;
	int type;				// FULL_BRICK, HALF_BRICK 
	int toughness;			// number of hits before destruction
	GsSPRITE sprite;
	MovementData movement;
} Brick;




typedef struct 
{
	char name[16];
	int points;
} Score;



  
   
#define BreakoutCollisionCoefficient (0.75)


extern int BreakoutAutoPilotFlag;

extern int BreakoutNumberOfPlayers;

extern Score CurrentScore;
extern Score HighScore;

extern int PlayerOneNumberOfLives;
extern int PlayerTwoNumberOfLives;

extern int Level;



	// summoning interface
void InitialiseBreakout2 (void);

void ResetBreakout (void);

//void BreakoutFontPrinting (void);		// redefined in main.c
void BreakoutMainLoopGameLogic (void);
void BreakoutDrawing (GsOT *ot);

int CountNumberOfBricks (void);

void SetUpLevel1(void);

void DestroyAllBricks (void);


#define BREAKOUT_H_INCLUDED 1

#endif