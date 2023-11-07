//--------------------------------------------------------------------------
// File: main.c
// Author: George Bain
// Date: July 6, 1998
// Description: Chapter 3: CD Example 2 - Playing CD-DA Tracks
// Copyright (C) 1998 Sony Computer Entertainment Europe.,
//           All Rights Reserved.  Permission granted to whom ever.
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
// I N C L U D E S
//--------------------------------------------------------------------------

#include <libps.h>
#include "main.h" 
#include "game.h"
#include "cntrl.h"

//--------------------------------------------------------------------------
// G L O B A L S
//--------------------------------------------------------------------------

int output_buffer_index;            // buffer index
GsOT world_ordering_table[2];       // ordering table headers
GsOT_TAG ordering_table[2][1<<1];   // actual ordering tables
PACKET gpu_work_area[2][24000];     // GPU packet work area
u_char prev_mode;					// previous code
int fnt_id[9];						// font id 
short vsync_count; 

//--------------------------------------------------------------------------
// Function: main()
// Description: CD Example 2 - Playing CD-DA Tracks
// Parameters: none
// Returns: int
// Notes: N/A
//--------------------------------------------------------------------------

int main( void )
 {
          
   	int tracks[11] = {1,2,3,4,5,6,7,8,9,0};
	int mode=1;
	int offset=0;
	int count=0;

   	InitGame();
   	
	// set-up for CD-DA tracks
   	SsSetSerialAttr(SS_CD,SS_MIX,SS_SON);
   	SsSetSerialVol(SS_CD,127,127); 
	
				 
	// main loop
    while( !DONE )
      {
		 	     
		 FntPrint(fnt_id[0], "~c900 Playing CD-DA Tracks Example ");
		 FntPrint(fnt_id[1], "~c990 Insert a Music disk and press any of the following: ");
		 FntPrint(fnt_id[2], " Press O to stop \n Press X to play \n Press L1 to cycle tracks");
         FntPrint(fnt_id[5]," Current Music Track: %d", offset+1);	 

		 if( PAD_PRESS(buffer1,PAD_L1) )
		   { 
		     if( (count+=7) > vsync_count )
			   {
		         
		         count=0;

		         if( offset <9 )
			         offset++;
			     else
			         offset =0;
			   }

		     CdPlay(0,tracks,offset);  // cycle track
		     		     
		   }

	  	 if( PAD_PRESS(buffer1,PAD_CIRCLE) )
		   { 
		     CdPlay(0,tracks,offset);  // stop playing		     
		   }

		 if( PAD_PRESS(buffer1,PAD_CROSS) )
		   { 
		     CdPlay(1,tracks,offset);  // start play		     
		   }
	  	

		 UpdateScreen();

      }// end while loop     

    DeInitGame();   // de-init the game

    return(0);      // success

 }// end main 		   



//-----------------------------------EOF----------------------------------------

   