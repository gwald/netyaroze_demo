// 
// player.h
// 
// Player related functions. Deals with initilisation,
// drawing etc...
// 
// Andrew Murray - 22nd November 2002
// 

#ifndef PLAYER_H
#define PLAYER_H

// --------------------------------------------------------------------------------
// Defines
// --------------------------------------------------------------------------------

// directions for player movement direction
enum directions {S, N, E, W, SE, SW, NE, NW, NONE};

// number of coloumns of frames
#define ANIM_COLUMNS	12

// number of frames per animation
#define FPANIM	3

// number of animation frames per second
#define FRAMERATE 	10

// player sprite's priority in the OT
#define PLAYER_OT_PRI 1

// dimensions of a player frame
#define FRAME_W 16
#define FRAME_H 24

// --------------------------------------------------------------------------------
// Structures
// --------------------------------------------------------------------------------

typedef struct
{
	// holds the sprite information
	GsSPRITE spr;
	
	// used in animation to determine direction
	enum directions dir;

	// used in animation to determine which frame
	// inside each 3 frame set the sprite u,v should equal
	u_short frame;
	
	// flag to tell wither to animate or not each frame
	u_short anim;

	// the player number in multi-player
	u_char playerNum;
}player;

// --------------------------------------------------------------------------------
// Globals
// --------------------------------------------------------------------------------

extern player g_player;

// --------------------------------------------------------------------------------
// Prototypes
// --------------------------------------------------------------------------------

void 	InitPlayer			(player *pPlayer, GsIMAGE *img, u_short pNum, int x, int y);
void 	DrawPlayer			(player *pPlayer, GsOT *pOT);

#endif