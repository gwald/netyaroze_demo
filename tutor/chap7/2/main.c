//--------------------------------------------------------------------------
// File: main.c
// Author: George Bain
// Date: July 12, 1998
// Description: Chapter 7: Scrolling Example 2 - Vertical Scrolling 
// Copyright (C) 1998 Sony Computer Entertainment Europe.,
//           All Rights Reserved.  Permission granted to whom ever.
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
// I N C L U D E S
//--------------------------------------------------------------------------

#include <libps.h>
#include "cntrl.h"
#include "main.h" 

//--------------------------------------------------------------------------
// D E F I N E S 
//-------------------------------------------------------------------------- 

// texture
#define LEFT_TIM_ADDR  (0x80090000)
#define RIGHT_TIM_ADDR (0x8009B000)

//--------------------------------------------------------------------------
// G L O B A L S
//--------------------------------------------------------------------------

int output_buffer_index;            // buffer index
GsOT world_ordering_table[2];       // ordering table headers
GsOT_TAG ordering_table[2][1<<1];   // actual ordering tables
PACKET gpu_work_area[2][24000];     // GPU packet work area
u_char prev_mode;					// previous code
int fnt_id[9];						// font id
GsSPRITE left_screen, right_screen; // screen sprites
short scroll_counter=0;				// duh!

//--------------------------------------------------------------------------
// Function: main()
// Description: Scrolling Example 2 - Vertical Scrolling
// Parameters: none
// Returns: int
// Notes: N/A
//--------------------------------------------------------------------------

int main( void )
 {
          
    int count=0;	
				
    InitGame();
	
	InitBackground(); 	

	// main loop
    while( !DONE )
      {
		 	     
		 FntPrint(fnt_id[0], "~c900Scrolling Example 2 - Vertical Scrolling");
		 
         Scroll(); 
		 UpdateScreen();

      }// end while loop

    DeInitGame();   // de-init the game

    return(0);      // success

 }// end main 





//--------------------------------------------------------------------------
// Function: DrawBackground()
// Description: Change y position of sprite based on our scroll counter and
//              register sprites in OT
// Parameters: none
// Returns: none
// Notes: N/A
//--------------------------------------------------------------------------

void DrawBackground( void )
 {
	
	left_screen.y = scroll_counter;
	GsSortFastSprite( &left_screen, &world_ordering_table[output_buffer_index], 0 );
	
	right_screen.y = scroll_counter; 
	GsSortFastSprite( &right_screen, &world_ordering_table[output_buffer_index], 0 );
	
	left_screen.y = scroll_counter + SCREEN_HEIGHT;
	GsSortFastSprite( &left_screen, &world_ordering_table[output_buffer_index], 0 );
	
	right_screen.y = scroll_counter + SCREEN_HEIGHT;
	GsSortFastSprite( &right_screen, &world_ordering_table[output_buffer_index], 0 );
 
 }// end Drawbackground





//--------------------------------------------------------------------------
// Function: Scroll()
// Description: Decrement scroll counter to move image up and wrap around
// Parameters: none
// Returns: none
// Notes: N/A
//--------------------------------------------------------------------------

void Scroll( void )
 {
	
	scroll_counter -= 1;
	
	if( scroll_counter <= -SCREEN_HEIGHT )
	    scroll_counter = 0;

 }// end Scroll

 



//--------------------------------------------------------------------------
// Function: InitBackground()
// Description: Load TIM and setup our screen sprites..left and right sides
// Parameters: none
// Returns: none
// Notes: N/A
//--------------------------------------------------------------------------

void InitBackground( void )
 {	 	
	
	GsIMAGE *left;
	GsIMAGE *right;
	u_short tpage; 	
   	
	// left side
    left = ReadTIM((u_long*)LEFT_TIM_ADDR);    
	tpage = GetTPage(1,0,left->px,left->py);		
	InitSprite( &left_screen,(1<<24),0,0,160,256,tpage,0,0,left->cx,left->cy,
	            0x80,0x80,0x80,0,0,ONE,ONE,0 );
	
	// right side
    right = ReadTIM((u_long*)RIGHT_TIM_ADDR);	
	tpage = GetTPage(1,0,right->px,right->py);		
	InitSprite( &right_screen,(1<<24),160,0,160,256,tpage,0,0,right->cx,right->cy,
	            0x80,0x80,0x80,0,0,ONE,ONE,0 );

 }// end InitBackground





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
// Function: InitGame()
// Description: Initialise the graphics mode, joypad, ordering tables,
//              textures, and objects
// Parameters: none
// Returns: void
// Notes: N/A
//--------------------------------------------------------------------------

void InitGame( void )
 {
	 int count;
     
	 printf("Starting InitGame() \n");
	
	 // load in the font pattern
	 FntLoad(960,256);
     printf("Fonts loaded: \n");

	 fnt_id[0] = FntOpen(0,10,SCREEN_WIDTH, SCREEN_HEIGHT,0,80);
	 fnt_id[1] = FntOpen(0,20,SCREEN_WIDTH, SCREEN_HEIGHT,0,80);
	 fnt_id[2] = FntOpen(0,30,SCREEN_WIDTH, SCREEN_HEIGHT,0,80);
	 fnt_id[3] = FntOpen(0,40,SCREEN_WIDTH, SCREEN_HEIGHT,0,80);

	 fnt_id[4] = FntOpen(0,120,SCREEN_WIDTH, SCREEN_HEIGHT,0,80);
	 fnt_id[5] = FntOpen(0,130,SCREEN_WIDTH, SCREEN_HEIGHT,0,80);
	 fnt_id[6] = FntOpen(0,140,SCREEN_WIDTH, SCREEN_HEIGHT,0,80);
	 fnt_id[7] = FntOpen(0,150,SCREEN_WIDTH, SCREEN_HEIGHT,0,80);
	  
   	 // save current video mode
	 prev_mode = GetVideoMode();

	 // init graphic mode
	 SetVideoMode( MODE_PAL );
	 printf("Set video mode complete: \n");

	 // init the controller buffers
	 GetPadBuf(&buffer1,&buffer2); 
	 printf("Set controller buffers complete: \n");

	 // all reset, the drawing environment and display are initialised
	 ResetGraph(0);

	 GsInitGraph( SCREEN_WIDTH, SCREEN_HEIGHT,
			      GsOFSGPU|GsNONINTER, 0, 0 );

	 printf("Screen size setup complete: \n");

	 // double buffer definition
	 GsDefDispBuff( 0, 0, 0, SCREEN_HEIGHT );	 
	 printf("Double buffer setup complete: \n");
	 
	 // set display output on t.v 
	 GsDISPENV.screen.x = 10;
	 GsDISPENV.screen.y = 18;
	 GsDISPENV.screen.w = 255;
	 GsDISPENV.screen.h = 255; 	  	
  	
	 // set up the ordering table handlers
	 for( count=0; count < 2; count++ )
	    {
		  world_ordering_table[count].length = 1;
		  world_ordering_table[count].org = ordering_table[count];
	    }

	 // initialises the ordering table
	 GsClearOt( 0, 0, &world_ordering_table[output_buffer_index]);
	 GsClearOt( 0, 0, &world_ordering_table[output_buffer_index+1]);
	 printf("WOT is setup and complete: \n");
     printf("Game setup is complete: \n");

 }// end InitGame





//--------------------------------------------------------------------------
// Function: DeInitGame()
// Description: De-init the game, sound, graphics, etc
// Parameters: none
// Returns: void
// Notes: N/A
//--------------------------------------------------------------------------

void DeInitGame( void )
 {

 	 // set previous video mode
	 SetVideoMode( prev_mode );

	 // current drawing is canvelled and the command queue is flushed
	 ResetGraph(3);	   

	 printf("Graphics flushed: \n");
	 printf("Game now de-int: \n");
 
 }// end DeInitGame





//------------------------------------------------------------------------------
// Function: UpdateScreen()
// Description: Updates all the game objects and redraws the screen
// Parameters: none
// Returns: void
// Notes: 
//------------------------------------------------------------------------------

void UpdateScreen( void )
 {

	int count;

	// get the active buffer
    output_buffer_index = GsGetActiveBuff();

    // sets drawing command storage address
    GsSetWorkBase((PACKET*)gpu_work_area[output_buffer_index]);

    // initialises the ordering table
    GsClearOt(0, 0, &world_ordering_table[output_buffer_index]);

    // rendering done here
    
	for( count =0; count <8; count++ )
         FntFlush(fnt_id[count]);         
         
    DrawBackground();               
		
    // wait for all drawing to be completed
    DrawSync(0);

    // wait for vertical synchronisation
    VSync(0);    // 0: blocking until vertical synch occurs

    // swap double buffers, (changes the display buffer and drawing buffer)
    GsSwapDispBuff();

    // registers drawing clear command in OT (e.g. clear to black)
    GsSortClear(0x0, 0x0, 0x80,
                &world_ordering_table[output_buffer_index]);
   
	// start execution of the drawing command registered in OT
    GsDrawOt(&world_ordering_table[output_buffer_index]);

 }// end UpdateScreen 




//----------------------------------EOF-------------------------------------
