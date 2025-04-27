//--------------------------------------------------------------------------
// File: main.c
// Author: George Bain
// Date: March 1999 
// Description: Vertical Split Screens using multiple ordering tables.
// Copyright (C) 1999 Sony Computer Entertainment Europe.,
//           All Rights Reserved.  
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
// I N C L U D E S
//--------------------------------------------------------------------------



	
	
#include <libps.h>

#include "cntrl.h"
#include "main.h"
#include "genlib.h" 

//--------------------------------------------------------------------------
// G L O B A L S
//--------------------------------------------------------------------------

int out_buf;                     			 // buffer index
GsOT wot[2];                				 // ordering table headers
GsOT_TAG ot[2][1<<OT_LENGTH];    			 // actual ordering tables
PACKET gpu_work_area[2][PACKETMAX2];         // GPU packet work area
u_char prev_mode;					         // previous mode
int fnt_id[9];						         // font id 
cntrl_data *buffer1=NULL, *buffer2=NULL;						 // controller buffers 

GsRVIEW2 view;			     				 // view handler  
GsF_LIGHT light[3];		  					 // 3 light handlers
GsFOGPARAM fog;								 // fog handler

GsDOBJ2 obj;								 // 3D object handler
GsCOORDINATE2 coord_obj;					 // 3D object coordinate handler
SVECTOR sv_obj;								 // vector for 3D object

GsDOBJ2 obj2;								 // 3D object handler
GsCOORDINATE2 coord_obj2;					 // 3D object coordinate handler
SVECTOR sv_obj2;							 // vector for 3D object


GsSPRITE sprite;					 		 // image

line_handler x_axis,y_axis, z_axis;			 // axis lines
line_handler h_line[10], v_line[10];		 // grid lines

GsOT wot2[2];                				 // ordering table headers
GsOT_TAG ot2[2][1<<OT_LENGTH];    			 // actual ordering tables



//--------------------------------------------------------------------------
// Function: main()
// Description: GsSetClip() and GsSetOffset() are used to demonstrate split
//              screens in 3D.  Two ordering tables are also used, one for
//              the left and right screens.
// Parameters: none
// Returns: int
// Notes: N/A
//--------------------------------------------------------------------------


int main( void )
 {

	InitAll();
	InitImage(); 									  
	InitGrid();
	InitAxis();									  

	InitCoordinates(); 	

    // texture cube
	InitTMD( &obj, &coord_obj, (u_long*)OBJECT_ADDR );
	// flat cube
	InitTMD( &obj2, &coord_obj2, (u_long*)OBJECT2_ADDR );
	 
	InitView(); 
    InitLights();   
    InitFog();

	ReadTIM( (u_long*)TEX_ADDR );

    view.vpx = 550;
	view.vpy = -3500;
	view.vpz = -4000;
	
    view.vrx = 550;
	view.vry = 0;
	view.vrz = 1300;

    while( !DONE )
      {	 		 	     
 		 FntPrint("\t\t\t\t~c009Vertical Split Screen~c999 \n");
		 FntPrint("\t\t\t\t~c009using mutilple OT's~c999 \n\n");
		 FntPrint("\t\t\t\tLoc x:(%d) \n",coord_obj.coord.t[0]);
		 FntPrint("\t\t\t\tLoc y:(%d) \n",coord_obj.coord.t[1]);
		 FntPrint("\t\t\t\tLoc z:(%d) \n\n",coord_obj.coord.t[2]);

		 FntPrint("\t\t\t\tLoc2 x:(%d) \n",coord_obj2.coord.t[0]);
		 FntPrint("\t\t\t\tLoc2 y:(%d) \n",coord_obj2.coord.t[1]);	
		 FntPrint("\t\t\t\tLoc2 z:(%d) \n\n",coord_obj2.coord.t[2]);
    
		 ControlObject(); 
		  
   		 UpdateCoordinates(&sv_obj,&coord_obj);
		 UpdateCoordinates(&sv_obj2,&coord_obj2);

		 UpdateScreen();

      }// end while

    DeInitAll();   

    return(0);      

 }// end main 


//------------------------------------------------------------------------------
// Function: ControlObject()
// Description: Control object with button presses
// Parameters: void
// Returns: void
// Notes: 
//------------------------------------------------------------------------------ 

void ControlObject( void )
 {
     
     if( PAD_PRESS(buffer1,PAD_L1) )
	   {
	     coord_obj.coord.t[2] -= 25;
	   } 

     if( PAD_PRESS(buffer1,PAD_L2) )
	   {
	     coord_obj.coord.t[2] += 25;
	   }

     if( PAD_PRESS(buffer1,PAD_LU) )
	   {
	     coord_obj.coord.t[1] -= 25;
	   } 

     if( PAD_PRESS(buffer1,PAD_LD) )
	   {
	     coord_obj.coord.t[1] += 25;
	   }

     if( PAD_PRESS(buffer1,PAD_LL) )
	   {
	     coord_obj.coord.t[0] -= 25;
	   } 
	   		    
     if( PAD_PRESS(buffer1,PAD_LR) )
	   {
	     coord_obj.coord.t[0] += 25;
	   }



     if( PAD_PRESS(buffer1,PAD_R1) )
	   {
	     coord_obj2.coord.t[2] -= 25;
	   } 

     if( PAD_PRESS(buffer1,PAD_R2) )
	   {
	     coord_obj2.coord.t[2] += 25;
	   }

     if( PAD_PRESS(buffer1,PAD_RU) )
	   {
	     coord_obj2.coord.t[1] -= 25;
	   } 

     if( PAD_PRESS(buffer1,PAD_RD) )
	   {
	     coord_obj2.coord.t[1] += 25;
	   }

     if( PAD_PRESS(buffer1,PAD_RL) )
	   {
	     coord_obj2.coord.t[0] -= 25;
	   } 
	   		    
     if( PAD_PRESS(buffer1,PAD_RR) )
	   {
	     coord_obj2.coord.t[0] += 25;
	   }

     if( PAD_PRESS(buffer1,PAD_START) )
	   {
	     	        
	   }

     if( PAD_PRESS(buffer1,PAD_SELECT) )
	   {
 	           
	   }
	   	   	   	   	   	   	   	    
 }// end ControlObject






//--------------------------------------------------------------------------
// Function: InitAll()
// Description: Initialise the graphics mode, joypad, ordering tables,
//              textures, and objects
// Parameters: none
// Returns: void
// Notes: GsInit3D() added
//--------------------------------------------------------------------------

void InitAll( void )
 {

	 

	 
	 out_buf = 0;

	 ResetGraph(0);

	 FntLoad(960,256);

	 FntOpen(15-(SCREEN_WIDTH/2),5-(SCREEN_HEIGHT/2), 
	         SCREEN_WIDTH, SCREEN_HEIGHT, 0, 512);
	         	  
	 prev_mode = GetVideoMode();

	 SetVideoMode( MODE_PAL );

	 GsInitGraph( SCREEN_WIDTH, SCREEN_HEIGHT,
			      GsOFSGPU|GsNONINTER, 0, 0 ); 
	 
	 GsDefDispBuff( 0, 0, 0, SCREEN_HEIGHT );  
	  
	 GsDISPENV.screen.x = 6;
	 GsDISPENV.screen.y = 16;
	 GsDISPENV.screen.w = 255;
	 GsDISPENV.screen.h = 255;
	
	 GsInit3D();

     wot[0].length = OT_LENGTH;
	 wot[0].org = ot[0];

	 wot[1].length = OT_LENGTH;
	 wot[1].org = ot[1];


     wot2[0].length = OT_LENGTH;
	 wot2[0].org = ot2[0];

	 wot2[1].length = OT_LENGTH;
	 wot2[1].org = ot2[1];


	 GsClearOt( 0, 0, &wot[out_buf]);
	 GsClearOt( 0, 0, &wot[out_buf+1]);

     GetPadBuf((volatile u_char**)&buffer1,(volatile u_char**)&buffer2);

 }// end InitGame





//--------------------------------------------------------------------------
// Function: DeInitAll()
// Description: De-init the demo, sound, graphics, etc
// Parameters: none
// Returns: void
// Notes: N/A
//--------------------------------------------------------------------------

void DeInitAll( void )
 {

	 SetVideoMode( prev_mode );

	 
	 
	 ResetGraph(0);	   

	 
 
 }// end DeInitGame




//------------------------------------------------------------------------------
// Function: UpdateScreen()
// Description: Updates all objects and redraws the screen
// Parameters: none
// Returns: void
// Notes: 4 -> 14-OT_LENGTH
//------------------------------------------------------------------------------

void UpdateScreen( void )
 {
	RECT clip;
    int cnt=0;
    
    // calculate World-Screen Matrix
	GsSetRefView2(&view);	

    out_buf = GsGetActiveBuff();

    GsSetWorkBase((PACKET*)gpu_work_area[out_buf]);

    GsClearOt(0, 0, &wot[out_buf]);
	GsClearOt(0, 0, &wot2[out_buf]);

    // sort
	SortObject(&obj,  &wot[out_buf],  14-OT_LENGTH);
	SortObject(&obj2, &wot2[out_buf], 14-OT_LENGTH);
      
    
    
    
    DrawSync(0); 
    
    VSync(0);    

    GsSwapDispBuff();

	 
	
	 // Note: Only called once by the first split screen.
	 // GsSortClear clears the whole screen (actual VRAM) with no regards to Offsets or Ordering Table it's called in.
	// So calling GsSortClear again for the 2nd screen would just clear the whole screen again, removing the 1st split screen draw calls.
    GsSortClear(0x0, 0x0, 0x0, &wot[out_buf]);		



	// Set offset of right screen 
	GsSetOffset(SCREEN_WIDTH / 4, SCREEN_HEIGHT / 2);
	// Set clipping for right screen
	clip.x = 0;
	clip.y = 0;
	clip.w = SCREEN_WIDTH / 2;
	clip.h = SCREEN_HEIGHT;
	GsSetClip(&clip);
	// Start Drawing right screen 
	GsDrawOt(&wot[out_buf]);  

	// Set offset of left screen 
	GsSetOffset(SCREEN_WIDTH * 3 / 4, SCREEN_HEIGHT / 2); 
	// Set clipping for left screen  
	clip.x = SCREEN_WIDTH/2;
	clip.y = 0;
	clip.w = SCREEN_WIDTH/2;
	clip.h = SCREEN_HEIGHT;
	GsSetClip(&clip);
	// Start Drawing left screen 		
	GsDrawOt(&wot2[out_buf]);

	FntFlush(-1);

 }// end UpdateScreen


//----------------------------------EOF-------------------------------------

	 
