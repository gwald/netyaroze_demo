//--------------------------------------------------------------------------
// File: main.c
// Author: George Bain
// Date: July 23, 1998
// Description: Black Widow - Net Yaroze Game Demo 
// Copyright (C) 1998 Sony Computer Entertainment Europe.,
//           All Rights Reserved.  Permission granted to whom ever.
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
// I N C L U D E S
//--------------------------------------------------------------------------

#include <libps.h>
#include "main.h" 
#include "game.h"

//--------------------------------------------------------------------------
// G L O B A L S
//--------------------------------------------------------------------------

int output_buffer_index;            // buffer index
GsOT world_ordering_table[2];       // ordering table headers
GsOT_TAG ordering_table[2][1<<1];   // actual ordering tables
PACKET gpu_work_area[2][24000];     // GPU packet work area
u_char prev_mode;					// previous video mode
int fnt_id[9];						// font id
GsBG bg_data;						// bg info
GsMAP map_data;						// map info
GsCELL cell_data[20];				// cell info
game_data game;						// game data
player_data player;					// player
enemy_data black_spider;            // black spider
enemy_data yellow_spider;			// yellow spider
screen_data screen;					// screens
int frame_count;				    // frame counter
file_data cd_file[3];				// cd files
short vab_id;					    // vab id number 0-15
short seq_id;					    // seq id number 0-32 
voice_data land;			        // sound fx

// formula for work area
u_long bg_work_area[(((SCREEN_WIDTH/TILE_WIDTH+1)*
                    (SCREEN_HEIGHT/TILE_HEIGHT+1+1)*6+4)*2+2)];  

// our map
u_short map_array[NUM_TILES] = {

5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,
5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,
5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,
5,5,2,0,0,2,0,0,2,0,0,2,0,0,2,0,0,2,5,5,
5,5,1,5,5,1,5,5,1,5,5,5,5,5,1,5,5,1,5,5,
5,5,1,5,5,1,5,5,1,5,5,5,5,5,1,5,5,1,5,5,
5,5,2,5,5,2,5,5,2,0,0,2,0,0,2,0,0,2,5,5,
5,5,1,5,5,1,5,5,1,5,5,5,5,5,5,5,5,5,5,5,
5,5,1,5,5,1,5,5,1,5,5,5,5,5,5,5,5,5,5,5,
5,5,2,0,0,2,5,5,2,0,0,2,0,0,2,0,0,2,5,5,
5,5,1,5,5,1,5,5,5,5,5,1,5,5,1,5,5,1,5,5,
5,5,1,5,5,1,5,5,5,5,5,1,5,5,1,5,5,1,5,5,
5,5,2,0,0,2,0,0,2,0,0,2,0,0,2,0,0,2,5,5,
5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,
5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,
5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5	

};	

//--------------------------------------------------------------------------
// Function: main()
// Description: Black Widow - Net Yaroze Game Demo
// Parameters: none
// Returns: int
// Notes: N/A
//--------------------------------------------------------------------------

int main( void )
 {	    	
	
	// init all			
    InitGame();	
     
	// start game loop
	PlayGame();

    // de-init all    
    DeInitGame();   

    return(0);      // success

 }// end main  



//----------------------------------EOF-------------------------------------
