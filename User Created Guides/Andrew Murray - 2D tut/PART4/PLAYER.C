// 
// player.c
// 
// Player related functions. Deals with initilisation,
// drawing etc...
// 
// Andrew Murray - 22nd November 2002
// 

#include <libps.h>
#include <stdlib.h>

#include "sprite.h"
#include "player.h"

// --------------------------------------------------------------------------------
// Globals
// --------------------------------------------------------------------------------

player g_player;

// --------------------------------------------------------------------------------
// Function Definitions
// --------------------------------------------------------------------------------

// --------------------------------------------------------------------------------
// Name:	InitPlayer()
// Desc:	Initialise the sprite structure and the animation settings.
// --------------------------------------------------------------------------------
void InitPlayer(player *pPlayer, GsIMAGE *img, u_short pNum, int x, int y)
{	
	InitSprite(&pPlayer->spr, img);
	
	// initial x, y starting position
	pPlayer->spr.x = x;
	pPlayer->spr.y = y;
	
	// the height and width of the 
	// charcter on the game gfx image
	pPlayer->spr.w = FRAME_W;
	pPlayer->spr.h = FRAME_H;
	
	// set the direction
	pPlayer->dir = S;
	
	// initial frame of the animation
	pPlayer->frame = 0;
	
	// set the sprite to initially not animate
	pPlayer->anim = 0;
}

// --------------------------------------------------------------------------------
// Name:	DrawPlayer()
// Desc:	This function will decide which frame of the player animation should
// 			be displayed then draw it.
// --------------------------------------------------------------------------------
void DrawPlayer(player *pPlayer, GsOT *pOT)
{
	// used to hold the current animation frame increment
	static int frameCounter = 0;
	
	// increase the animation counter 
	// to dislay the next frame
	pPlayer->frame = ((frameCounter++) / FRAMERATE) % FPANIM;
	
	// check to see if the character should be animated this frame
	if(pPlayer->anim)
	{
		// the frame index inside the image that the uv offsets should point to
		int uvFrame = (pPlayer->dir * FPANIM) + (pPlayer->frame);
		
		// set the current animation frame 
		// that should be displayed
		pPlayer->spr.u = (uvFrame % ANIM_COLUMNS) * pPlayer->spr.w;
		pPlayer->spr.v = (uvFrame / ANIM_COLUMNS) * pPlayer->spr.h;

		// account for the fact that there are multiple players
		// represented by different coloured avatars
		pPlayer->spr.v += (48 * pPlayer->playerNum);

		// set flag back
		pPlayer->anim = 0;
	}
	else
	{
		// the frame inside the image that the uv offsets should point to
		int defaultFrame = (pPlayer->dir * FPANIM);

		// set the frame back to the default stance for each direction
		pPlayer->spr.u = (defaultFrame % ANIM_COLUMNS) * pPlayer->spr.w;
		pPlayer->spr.v = (defaultFrame / ANIM_COLUMNS) * pPlayer->spr.h;
		
		// account for the fact that there are 2 players 
		// represented by different coloured avatars
		pPlayer->spr.v += (48 * pPlayer->playerNum);
	}
		
	// enter the player sprite into the ordering table
	GsSortFastSprite(&pPlayer->spr, pOT, PLAYER_OT_PRI);
}