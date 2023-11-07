//--------------------------------------------------------------------------
// File: collide.c
// Author: George Bain
// Date: July 23, 1998
// Description: Collision related routines 
// Copyright (C) 1998 Sony Computer Entertainment Europe.,
//           All Rights Reserved.  Permission granted to whom ever.
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
// I N C L U D E S
//--------------------------------------------------------------------------

#include <libps.h>
#include "main.h"
#include "collide.h" 

//--------------------------------------------------------------------------
// Function: PlayerCircleCollide()
// Description: Check whether the player lands on a circle.  If a collision
//              has occured change circle to the new color
// Parameters: none
// Returns: none
// Notes: N/A
//--------------------------------------------------------------------------

void PlayerCircleCollide( void )
 {

   short cell_x = 0,cell_y = 0;

   cell_x = (player.sprite.x+12) / 16;
   cell_y = (player.sprite.y+12) / 16;
   
   // check circle hit counter with number of circles
   // and change color accordingly

   if( game.num_circle_hit < game.num_circle )
     {
       if( map_array[cell_x+(cell_y*20)] == COLOR_PURPLE )
         {
	   
	       map_data.index[cell_x+(cell_y*20)] = COLOR_GREEN; 
	       game.num_circle_hit++;
		   PlaySFX(&land);
         }
	 }


   if( (game.num_circle_hit >= game.num_circle) &&  
       (game.num_circle_hit <= (game.num_circle*2) ) )
     { 
       
	   if( map_array[cell_x+(cell_y*20)] == COLOR_GREEN )
         {
	       
	       map_data.index[cell_x+(cell_y*20)] = COLOR_YELLOW; 
	       game.num_circle_hit++;
		   PlaySFX(&land);
         }
     }


   if( game.num_circle_hit >= (game.num_circle*2) )
     { 
       
	   if( map_array[cell_x+(cell_y*20)] == COLOR_YELLOW )
         {
	       
	       map_data.index[cell_x+(cell_y*20)] = COLOR_PURPLE; 
	       game.num_circle_hit++;
		   PlaySFX(&land);
         }
     }

     
 }// end PlayerCircleCollide





//--------------------------------------------------------------------------
// Function: EnemyCircleCollide()
// Description: Check whether the black spider lands on a circle. If a 
//              collision has occured change circle to the OLD color
// Parameters: none
// Returns: none
// Notes: N/A
//-------------------------------------------------------------------------- 

void EnemyCircleCollide( void )
 {

   short cell_x = 0,cell_y = 0;

   cell_x = (black_spider.sprite.x+12) / 16;
   cell_y = (black_spider.sprite.y+12) / 16;
   
   // check circle hit counter with number of circles
   // and change color accordingly


   if( game.num_circle_hit < game.num_circle )
     {
       if( map_array[cell_x+(cell_y*20)] == COLOR_GREEN )
         {
	   
	       map_data.index[cell_x+(cell_y*20)] = COLOR_PURPLE; 
	       game.num_circle_hit--;
         }
	 }
	
   if( (game.num_circle_hit >= game.num_circle) &&  
       (game.num_circle_hit <= (game.num_circle*2) ) )
     { 
       
	   if( map_array[cell_x+(cell_y*20)] == COLOR_YELLOW )
         {
	       
	       map_data.index[cell_x+(cell_y*20)] = COLOR_GREEN; 
	       game.num_circle_hit--;
         }
     }

	
   if( game.num_circle_hit >= (game.num_circle*2) )
     { 
       
	   if( map_array[cell_x+(cell_y*20)] == COLOR_PURPLE )
         {
	       
	       map_data.index[cell_x+(cell_y*20)] = COLOR_YELLOW; 
	       game.num_circle_hit--;
         }
     }
   
 	
     
 }// end EnemyCircleCollide






//--------------------------------------------------------------------------
// Function: PlayerBorderCollide()
// Description: Check to see if player has collided with the border.  If a 
//              collision has occured back player to previous position
// Parameters: none
// Returns: none
// Notes: Basically make sure the sprite stays within bounds.
//-------------------------------------------------------------------------- 

void PlayerBorderCollide( void )
 {

   short cell_x = 0,cell_y = 0;
   
   // find player cell location	   
   switch( player.direction )
      {
		case PLAYER_RIGHT:
		  {
		    cell_x = (player.sprite.x+20) / 16;
			cell_y = (player.sprite.y+12) / 16;
		  }
		break;
		case PLAYER_LEFT:
		  {
		    cell_x = (player.sprite.x+4)  / 16;
			cell_y = (player.sprite.y+12) / 16;
		  }	
		break;
		case PLAYER_UP:
		  {
		    cell_x = (player.sprite.x+12) / 16;
			cell_y = (player.sprite.y+4)  / 16;
		  }	  
		break;
		case PLAYER_DOWN:
		  {
		    cell_x = (player.sprite.x+12) / 16;
			cell_y = (player.sprite.y+20) / 16;
		  }	
		break;

      }// end switch 
 
   //FntPrint(fnt_id[1],"cell_x:%d, cell_y:%d",cell_x,cell_y); 
   
   // check for player border collision
   if( map_array[cell_x+(cell_y*20)] == 5 )
     {
       
       player.sprite.x -= player.x_speed;
	   player.sprite.y -= player.y_speed;
	 }

 }// end PlayerBorderCollide
  





//--------------------------------------------------------------------------
// Function: BlackSpiderBorderCollide()
// Description: Check to see if black spider has collided with the border.  
//              If a collision has occured back black spider to previous 
//              position. 
// Parameters: none
// Returns: none
// Notes: Basically make sure the sprite stays within bounds.
//--------------------------------------------------------------------------

void BlackSpiderBorderCollide( void )
 {

   short cell_x = 0,cell_y = 0;		 
		
   // find enemy cell location	   
   switch( black_spider.direction )
      {
		case ENEMY_RIGHT:
		  {
		    cell_x = (black_spider.sprite.x+20) / 16;
			cell_y = (black_spider.sprite.y+12) / 16;
		  }
		break;
		case ENEMY_LEFT:
		  {
		    cell_x = (black_spider.sprite.x+4)  / 16;
			cell_y = (black_spider.sprite.y+12) / 16;
		  }	
		break;
		case ENEMY_UP:
		  {
		    cell_x = (black_spider.sprite.x+12) / 16;
			cell_y = (black_spider.sprite.y+4)  / 16;
		  }	  
		break;
		case ENEMY_DOWN:
		  {
		    cell_x = (black_spider.sprite.x+12) / 16;
			cell_y = (black_spider.sprite.y+20) / 16;
		  }	
		break;

      }// end switch
      
    if( map_array[cell_x+(cell_y*20)] == 5 )
     {
       //FntPrint(fnt_id[2]," BORDER HIT");
       black_spider.sprite.x -= black_spider.x_speed;
	   black_spider.sprite.y -= black_spider.y_speed;

       black_spider.direction = rand() % 4;
	 }
	
	 
 }// end BlackSpiderBorderCollide






//--------------------------------------------------------------------------
// Function: YellowSpiderBorderCollide()
// Description: Check to see if yellow spider has collided with the border.  
//              If a collision has occured back yellow spider to previous 
//              position. 
// Parameters: none
// Returns: none
// Notes: Basically make sure the sprite stays within bounds.
//--------------------------------------------------------------------------

void YellowSpiderBorderCollide( void )
 {

   short cell_x = 0,cell_y = 0;		 
		
   // find enemy cell location	   
   switch( yellow_spider.direction )
      {
		case ENEMY_RIGHT:
		  {
		    cell_x = (yellow_spider.sprite.x+20) / 16;
			cell_y = (yellow_spider.sprite.y+12) / 16;
		  }
		break;
		case ENEMY_LEFT:
		  {
		    cell_x = (yellow_spider.sprite.x+4)  / 16;
			cell_y = (yellow_spider.sprite.y+12) / 16;
		  }	
		break;
		case ENEMY_UP:
		  {
		    cell_x = (yellow_spider.sprite.x+12) / 16;
			cell_y = (yellow_spider.sprite.y+4)  / 16;
		  }	  
		break;
		case ENEMY_DOWN:
		  {
		    cell_x = (yellow_spider.sprite.x+12) / 16;
			cell_y = (yellow_spider.sprite.y+20) / 16;
		  }	
		break;

      }// end switch
      
    if( map_array[cell_x+(cell_y*20)] == 5 )
     {
       //FntPrint(fnt_id[2]," BORDER HIT");
       yellow_spider.sprite.x -= yellow_spider.x_speed;
	   yellow_spider.sprite.y -= yellow_spider.y_speed;

       yellow_spider.direction = rand() % 4;
	 }
	
	 
 }// end YellowSpiderBorderCollide






//--------------------------------------------------------------------------
// Function: EnemyPlayerCollide()
// Description: Check to see if the player and yellow spider collide.  If
//              a collision has occured, the player loses 1 life and gets
//              put back to a safe location to start over. 
// Parameters: none
// Returns: none
// Notes: Check gamelib.c for the BoxCollision routine.
//--------------------------------------------------------------------------  

void EnemyPlayerCollide( void )
 {

	if( BoxCollision(&player.sprite,&yellow_spider.sprite) )
	  {
	    game.player_lifes--;
		player.sprite.x = 70;
		player.sprite.y = 147;
	  }  

 }// end EnemyPlayerCollide



//----------------------------------EOF-------------------------------------
