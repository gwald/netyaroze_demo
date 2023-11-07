//--------------------------------------------------------------------------
// File: draw.c
// Author: George Bain
// Date: July 23, 1998
// Description: Drawing related routines 
// Copyright (C) 1998 Sony Computer Entertainment Europe.,
//           All Rights Reserved.  Permission granted to whom ever.
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
// I N C L U D E S
//--------------------------------------------------------------------------

#include <libps.h>
#include "main.h"
#include "draw.h"

//--------------------------------------------------------------------------
// Function: DrawPlayer()
// Description: If the player is alive register the sprite into the 
//              Ordering Table.
// Parameters: none
// Returns: none
// Notes: N/A
//--------------------------------------------------------------------------

void DrawPlayer( void )
 {

   if( player.health_state == PLAYER_ALIVE )
       GsSortSprite(&player.sprite,
                    &world_ordering_table[output_buffer_index],0);

 }// end DrawPlayer





//--------------------------------------------------------------------------
// Function: DrawBlackSpider()
// Description: If the black spider is alive register the sprite into the 
//              Ordering Table.
// Parameters: none
// Returns: none
// Notes: N/A
//--------------------------------------------------------------------------

void DrawBlackSpider( void )
 {

   if( black_spider.health_state == ENEMY_ALIVE )
       GsSortSprite(&black_spider.sprite,
                    &world_ordering_table[output_buffer_index],0);


 }// end DrawBlackSpider






//--------------------------------------------------------------------------
// Function: DrawYellowSpider()
// Description: If the yellow spider is alive register the sprite into the 
//              Ordering Table.
// Parameters: none
// Returns: none
// Notes: N/A
//--------------------------------------------------------------------------

void DrawYellowSpider( void )
 {

   if( yellow_spider.health_state == ENEMY_ALIVE )
       GsSortSprite(&yellow_spider.sprite,
                    &world_ordering_table[output_buffer_index],0);


 }// end DrawYellowSpider





//--------------------------------------------------------------------------
// Function: DrawStartScreen()
// Description: Register the screen sprites into the Ordering Table.
// Parameters: none
// Returns: none
// Notes: N/A
//--------------------------------------------------------------------------

void DrawStartScreen( void )
 {

	GsSortSprite(&screen.start_l,
                    &world_ordering_table[output_buffer_index],0);
	GsSortSprite(&screen.start_r,
                    &world_ordering_table[output_buffer_index],0);

 }// end DrawStartScreen






//--------------------------------------------------------------------------
// Function: DrawStoryScreen()
// Description: Register the screen sprites into the Ordering Table.
// Parameters: none
// Returns: none
// Notes: N/A
//--------------------------------------------------------------------------

void DrawStoryScreen( void )
 {

	GsSortSprite(&screen.story_l,
                    &world_ordering_table[output_buffer_index],0);
	GsSortSprite(&screen.story_r,
                    &world_ordering_table[output_buffer_index],0);

 }// end DrawStoryScreen






//--------------------------------------------------------------------------
// Function: DrawOverScreen()
// Description: Register the screen sprites into the Ordering Table.
// Parameters: none
// Returns: none
// Notes: N/A
//--------------------------------------------------------------------------

void DrawOverScreen( void )
 {

	GsSortSprite(&screen.over_l,
                    &world_ordering_table[output_buffer_index],0);
	GsSortSprite(&screen.over_r,
                    &world_ordering_table[output_buffer_index],0);

 }// end DrawOverScreen





//--------------------------------------------------------------------------
// Function: DrawCompleteScreen()
// Description: Register the screen sprites into the Ordering Table.
// Parameters: none
// Returns: none
// Notes: N/A
//--------------------------------------------------------------------------

void DrawCompleteScreen( void )
 {

	GsSortSprite(&screen.done_l,
                    &world_ordering_table[output_buffer_index],0);
	GsSortSprite(&screen.done_r,
                    &world_ordering_table[output_buffer_index],0);

 }// end DrawCompleteScreen



//----------------------------------EOF-------------------------------------
