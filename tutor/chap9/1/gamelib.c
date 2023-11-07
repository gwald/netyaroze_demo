//--------------------------------------------------------------------------
// File: gamelib.c
// Author: George Bain
// Date: July 23, 1998
// Description: game setup related routines 
// Copyright (C) 1998 Sony Computer Entertainment Europe.,
//           All Rights Reserved.  Permission granted to whom ever.
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
// I N C L U D E S
//--------------------------------------------------------------------------

#include <libps.h>
#include "main.h"
#include "gamelib.h"  

//--------------------------------------------------------------------------
// Function: BoxCollision()
// Description: Checks the bounding box of each sprite.
// Parameters: none
// Returns: 1: if there is a hit 0: no hit
// Notes: N/A
//--------------------------------------------------------------------------

int BoxCollision( GsSPRITE *sprite, GsSPRITE *sprite_two )
 {

    if( sprite->x > (sprite_two->x + sprite_two->w-1) ||
		sprite_two->x > (sprite->x + sprite->w-1) ||
       
		sprite->y > (sprite_two->y + sprite_two->h-1) ||
		sprite_two->y > (sprite->y + sprite->h-1) )
	  {
        return(0);  // NO HIT

	  }
	else
	  {
        return(1);  // HIT
		
	  }
	
 }// BoxCollision





//--------------------------------------------------------------------------
// Function: ReadFILE()
// Description: This routine searches for a single file, and stores that file
//              at the address passed in 
// Parameters: *file: address of file information, *addr: address where file
//             is stored
// Returns: none
// Notes: N/A
//--------------------------------------------------------------------------

void ReadFILE( file_data *file, u_long *addr )
 { 		

	// get location and size from filename on CD-ROM
	if( CdSearchFile(&file->file_info, file->file_name) == 0 ) 
	  {
		printf(" ERROR: %s not found \n", file->file_name);
		
	  }
   
   	// read in file on CD-ROM
	if( CdReadFile(file->file_name, addr, 0) == 0)
	  {
		printf(" ERROR: reading file \n");
	  }		
    
  	// wait for termination of CdRead
	while( CdReadSync(1, 0) > 0 ); 
	
 }// end ReadFILE 




//--------------------------------------------------------------------------
// Function: ReadTIM()
// Description: Read a TIM and CLUT(if 4 bit or 8 bit) and transfer to VRAM
// Parameters: u_long *addr: address of TIM
// Returns: GsIMAGE: address of tim variable
// Notes: N/A
//-------------------------------------------------------------------------- 

GsIMAGE *ReadTIM( u_long *addr )
 { 	
   
	static GsIMAGE tim;	 
	RECT rect; 			
			
	// skip id and initialize image structure 
	addr ++;
	GsGetTimInfo(addr, &tim);	

	// transfer pixel data to VRAM 
	rect.x = tim.px;
	rect.y = tim.py;
	rect.w = tim.pw;
	rect.h = tim.ph;
	LoadImage(&rect, tim.pixel);

	DrawSync(0);

    // check if CLUT exists and transfer it to VRAM 
 	if( (tim.pmode >> 3) & 0x01 ) 
 	  {
 	  	rect.x = tim.cx;
	  	rect.y = tim.cy;
	  	rect.w = tim.cw;
	  	rect.h = tim.ch;
	  	LoadImage(&rect, tim.clut);
	  }	
	  
    DrawSync(0);

    printf(" IMAGE - x:(%d), y:(%d), w:(%d), h:(%d) \n", tim.px, tim.py,tim.pw,tim.ph );  
    printf(" CLUT - x:(%d), y:(%d), w:(%d), h:(%d) \n", tim.cx, tim.cy,tim.cw,tim.ch );
    printf(" image mode:%d \n", tim.pmode);   
	printf(" tim addr: %p \n",&tim);

    return(&tim);	

 }// end ReadTIM 




   
//--------------------------------------------------------------------------
// Function: InitSprite()
// Description: Setup sprite structure
// Parameters: Lots!  Look at the Library Reference Manual
// Returns: none
// Notes: N/A
//--------------------------------------------------------------------------
   
void InitSprite( GsSPRITE *sprite, u_long attribute, short x, short y, u_short w, u_short h,
                 u_short tpage, u_char u, u_char v, short cx, short cy, u_char r, u_char g,
                 u_char b, short mx, short my, short scalex, short scaley, long rotate )
 {

	sprite->attribute = attribute;
	
	sprite->x = x;
	sprite->y = y;

	sprite->w = w;
	sprite->h = h;

	sprite->tpage = tpage;

	sprite->u = u;
	sprite->v = v;

	sprite->cx = cx;
	sprite->cy = cy;;

	sprite->r = r;
	sprite->g = g;
	sprite->b = b;

	sprite->mx = mx;
	sprite->my = my;

	sprite->scalex = scalex;
	sprite->scaley = scaley;
	sprite->rotate = rotate;


 }// end InitSprite	


//----------------------------------EOF-------------------------------------
