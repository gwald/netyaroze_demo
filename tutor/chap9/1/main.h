//--------------------------------------------------------------------------
// File: main.h
// Author: George Bain
// Date: July 24, 1998
// Description: Defines, structures, globals and prototypes
// Copyright (C) 1998 Sony Computer Entertainment Europe.,
//           All Rights Reserved.  Permission granted to whom ever.
//-------------------------------------------------------------------------- 

#ifndef _MAIN_H
#define _MAIN_H

//--------------------------------------------------------------------------
// D E F I N E S 
//--------------------------------------------------------------------------

// screen resolution defines
#define SCREEN_WIDTH  320
#define SCREEN_HEIGHT 256

// other related defines
#define SPRITE_ADDR  (0x80090000)
#define START_L_ADDR (0x80094230)
#define START_R_ADDR (0x8009E450)
#define STORY_L_ADDR (0x800A8670)
#define STORY_R_ADDR (0x800B2890)
#define OVER_L_ADDR  (0x800BCAB0)
#define OVER_R_ADDR  (0x800C6CD0)
#define DONE_L_ADDR  (0x800D0EF0)
#define DONE_R_ADDR  (0x800DB110)

// map related defines
#define TILE_WIDTH  16
#define TILE_HEIGHT 16
#define NUM_X_TILES 20	// 320/16=20
#define NUM_Y_TILES 16  // 256/16=16
#define NUM_TILES   (NUM_X_TILES * NUM_SCREENS_X) * (NUM_Y_TILES * NUM_SCREENS_Y)
#define NUM_SCREENS_X   1	// 2 screens wide
#define NUM_SCREENS_Y   1	// 2 screens height	

// game related defines
#define MODE_START     0
#define MODE_STORY     1
#define MODE_PLAY      2
#define MODE_DONE      3
#define MODE_OVER      4
#define MODE_QUIT      5

// player related defines
#define PLAYER_SPEED  1
#define PLAYER_ALIVE  1
#define PLAYER_DEAD   0
#define PLAYER_WIDTH  24
#define PLAYER_HEIGHT 24 
#define PLAYER_RIGHT  0
#define PLAYER_LEFT   1
#define PLAYER_UP     2
#define PLAYER_DOWN   3

// enemy related defines
#define ENEMY_SPEED  1
#define ENEMY_ALIVE  1
#define ENEMY_DEAD   0
#define ENEMY_WIDTH  24
#define ENEMY_HEIGHT 24
#define ENEMY_RIGHT  0
#define ENEMY_DOWN   1
#define ENEMY_LEFT   2
#define ENEMY_UP     3

// tile related defines
#define HORZ_LINE    0
#define VERT_LINE    1
#define COLOR_PURPLE 2
#define COLOR_GREEN  3
#define COLOR_YELLOW 4
#define BLACK_CUBE   5

// sound related defines
#define VH_ADDR	 (0x800E5340)
#define VB_ADDR	 (0x800F3F70)
#define SEQ_ADDR (0x80155F40)
#define MAIN_VOL (127)
#define SEQ_VOL  (60)


//--------------------------------------------------------------------------
// S T R U C T U R E S
//--------------------------------------------------------------------------

typedef struct voice_tag
 { 	
 	
	short vabid;
	short program;
	short tone;
	short note;
	short fine;
	short vol_l;
	short vol_r;

 }voice_data, *voice_ptr;


typedef struct file_tag
 {

	char file_name[32];
	CdlFILE file_info;

 }file_data, *file_ptr;



typedef struct game_tag
 {

   short game_mode;	      // game mode
   short time;			  // clock timer
   short player_lifes;	  // number of player lifes
   short score;			  // players score
   short num_circle;	  // number of circles
   short num_circle_hit;  // number of circles hit
   short cheat_speed;           // cheat
 }game_data, *game_ptr;



typedef struct player_tag
 {

	GsSPRITE sprite;
	short health_state;
	short ai_state;
	short count_1;
	short count_2;
	short x_speed;
	short y_speed;
	short direction;

 }player_data, *player_ptr;
															   

typedef struct enemy_tag
 {

	GsSPRITE sprite;
	short health_state;
	short ai_state;
	short count_1;
	short count_2;
	short x_speed;
	short y_speed;
	short direction;
	short frame;

 }enemy_data, *enemy_ptr;

typedef struct screen_tag
 {

   GsSPRITE start_l;
   GsSPRITE start_r;

   GsSPRITE story_l;
   GsSPRITE story_r;

   GsSPRITE over_l;
   GsSPRITE over_r;

   GsSPRITE done_l;
   GsSPRITE done_r;	 

 }screen_data, *screen_ptr;


//--------------------------------------------------------------------------
// G L O B A L S
//--------------------------------------------------------------------------

extern int output_buffer_index;            // buffer index
extern GsOT world_ordering_table[2];       // ordering table headers
extern GsOT_TAG ordering_table[2][1<<1];   // actual ordering tables
extern PACKET gpu_work_area[2][24000];     // GPU packet work area
extern u_char prev_mode;				   // previous video mode
extern int fnt_id[9];					   // font id
extern GsBG bg_data;					   // bg info
extern GsMAP map_data;					   // map info
extern GsCELL cell_data[20];			   // cell info
extern game_data game;					   // game data
extern player_data player;				   // player
extern enemy_data black_spider;            // black spider
extern enemy_data yellow_spider;		   // yellow spider
extern screen_data screen;				   // screens
extern int frame_count;				       // frame counter
extern file_data cd_file[3];			   // cd files
extern short vab_id;					   // vab id number 0-15
extern short seq_id;					   // seq id number 0-32 
extern voice_data land;			           // sound fx

// formula for work area
extern u_long bg_work_area[(((SCREEN_WIDTH/TILE_WIDTH+1)*
                    (SCREEN_HEIGHT/TILE_HEIGHT+1+1)*6+4)*2+2)];  

// our map
extern u_short map_array[NUM_TILES];

#endif

//----------------------------------EOF-------------------------------------
