//--------------------------------------------------------------------------
// File: main.c
// Author: George Bain
// Date: March 1999 
// Description: Rotation
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

GsSPRITE sprite;					 		 // image

line_handler x_axis,y_axis, z_axis;			 // axis lines
line_handler h_line[10], v_line[10];		 // grid lines


//--------------------------------------------------------------------------
// Function: main()
// Description: 3D object is loaded and displayed.  Rotation of the X, Y, and
//              Z axis are adjusted via button presses.
//         
//              UpdateCoordinates() updates the rotation and therefore the
//              3D coordinates of the model.
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
	InitTMD( &obj, &coord_obj, (u_long*)OBJECT_ADDR ); 
	InitView(); 
    InitLights();   
    InitFog();

    view.vpx = 550;
	view.vpy = -3500;
	view.vpz = -4000;
	
    view.vrx = 550;
	view.vry = 0;
	view.vrz = 1300;

    while( !DONE )
      {	 		 	     
 		 FntPrint("~c009Rotation~c999 \n\n");
		 FntPrint("Loc x:(%d) \n",coord_obj.coord.t[0]);
		 FntPrint("Loc y:(%d) \n",coord_obj.coord.t[1]);
		 FntPrint("Loc z:(%d) \n\n",coord_obj.coord.t[2]);

	 	 FntPrint("Rot x:(%d) \n",sv_obj.vx);
		 FntPrint("Rot y:(%d) \n",sv_obj.vy);
		 FntPrint("Rot z:(%d) \n",sv_obj.vz);
  
		 ControlObject(); 
		  
   		 UpdateCoordinates(&sv_obj,&coord_obj);

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
// Notes: sv_obj is a short vector in relation with the 3D model.  The +ONE)&(ONE-1)
//        makes certain that we stay in the range of 0-4095.  Remember that
//        the GTE knows ONE as 1.0.	So therefore ONE equals 360 degrees.
//------------------------------------------------------------------------------ 

void ControlObject( void )
 {
     
     if( PAD_PRESS(buffer1,PAD_R1) )
	   {
		 		 
		 sv_obj.vz = ((sv_obj.vz-50)+ONE)&(ONE-1);
	   } 

     if( PAD_PRESS(buffer1,PAD_R2) )
	   {
		 		 
		 sv_obj.vz = ((sv_obj.vz+50)+ONE)&(ONE-1);
		 
	   }  

     if( PAD_PRESS(buffer1,PAD_RU) )
	   {	     
		 		 
		 sv_obj.vx = ((sv_obj.vx-50)+ONE)&(ONE-1);
	   }

     if( PAD_PRESS(buffer1,PAD_RD) )
	   {
	     
	     sv_obj.vx = ((sv_obj.vx+50)+ONE)&(ONE-1);
	   } 	   

     if( PAD_PRESS(buffer1,PAD_RR) )
	   {
	     
	     sv_obj.vy = ((sv_obj.vy-50)+ONE)&(ONE-1);
	        
	   }
     if( PAD_PRESS(buffer1,PAD_RL) )
	   {
	     
	     sv_obj.vy = ((sv_obj.vy+50)+ONE)&(ONE-1); 	        
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
// Notes: 
//------------------------------------------------------------------------------

void UpdateScreen( void )
 {

    int cnt=0;
    
    // calculate World-Screen Matrix
	GsSetRefView2(&view);	

    out_buf = GsGetActiveBuff();

    GsSetWorkBase((PACKET*)gpu_work_area[out_buf]);

    GsClearOt(0, 0, &wot[out_buf]);



	// sort
	GsSortFastSprite(&sprite,&wot[out_buf],0);
    
	SortObject(&obj, &wot[out_buf], 4);
    
    for( cnt=0; cnt<10; cnt++)
         Sort3DLine( &v_line[cnt].line, &v_line[cnt].coord,
                     &v_line[cnt].p1, &v_line[cnt].p2 );

    for( cnt=0; cnt<10; cnt++)
         Sort3DLine( &h_line[cnt].line, &h_line[cnt].coord,
                     &h_line[cnt].p1, &h_line[cnt].p2 );

	Sort3DLine( &x_axis.line, &x_axis.coord, &x_axis.p1, &x_axis.p2 );
	Sort3DLine( &y_axis.line, &y_axis.coord, &y_axis.p1, &y_axis.p2 );
	Sort3DLine( &z_axis.line, &z_axis.coord, &z_axis.p1, &z_axis.p2 );




    DrawSync(0); 
    
    VSync(0);    

    GsSwapDispBuff();
    
    GsSortClear(0x0, 0x0, 0x0, &wot[out_buf]);

    GsDrawOt(&wot[out_buf]);

	FntFlush(-1);

 }// end UpdateScreen


//----------------------------------EOF-------------------------------------

	 