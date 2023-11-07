//--------------------------------------------------------------------------
// File: init.c
// Author: George Bain
// Date: March 1999 
// Description: 
// Copyright (C) 1999 Sony Computer Entertainment Europe.,
//           All Rights Reserved.  
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
// I N C L U D E S
//--------------------------------------------------------------------------


				  
				   
				   
#include <libps.h>


#include "cntrl.h"
#include "main.h"
#include "init.h"



//--------------------------------------------------------------------------
// Function: InitLights()
// Description: Setup 3 parallel lights and ambient lighting.
// Parameters: void
// Returns: void
// Notes: A parallel light source is a light source in which the brightness
//		  is determined only by the light source and the angle of the polygon.
//       
//        Ambient light is the surrounding light.  Even though light does not
//        directly strike it, the shape of an object may be seen with the 
//        surrounding light. 
//--------------------------------------------------------------------------

void InitLights( void )
 {		   	
	 
	// max 3 lights plus ambient

	// set first light
	light[0].vx = 20; 
	light[0].vy = -100; 
	light[0].vz = -100;	
	light[0].r = 0x80;
	light[0].g = 0x80;
	light[0].b = 0x80;
	GsSetFlatLight(0,&light[0]);

	// set second light
	light[1].vx = 20; 
	light[1].vy = -50; 
	light[1].vz = 100;	
	light[1].r = 0x80;
	light[1].g = 0x80;
	light[1].b = 0x80;
	GsSetFlatLight(1,&light[1]);

	// set third light
	light[2].vx = -20; 
	light[2].vy = 20; 
	light[2].vz = -100;	
	light[2].r = 0x80;
	light[2].g = 0x80;
	light[2].b = 0x80;
	GsSetFlatLight(2,&light[2]);

	// set RGB ambient lighting
	GsSetAmbient( ONE/2, ONE/2, ONE/2 ); 


 }// end InitLights	 




//--------------------------------------------------------------------------
// Function: InitFog()
// Description: Setup fog and lighting mode.  
// Parameters: void
// Returns: void
// Notes: In order to execute depth cueing, call GsSetLightMode(1)
//
//        SCRN_Z    = projection distance from screen and viewpoint (eye)
//        FAR_CLIP  = any Z values greater then 16-BIT are clipped
//        NEAR_CLIP = any Z values less then half of SCRN_Z are clipped
//-------------------------------------------------------------------------- 						  

void InitFog( void )
 {
  
#ifdef FOG
     
    // set fog parameters	
	fog.dqa = (-FAR_CLIP * ONE) / 64 / SCRN_Z;
	fog.dqb = (1.25 * ONE * ONE); 
	fog.rfc = 0;
	fog.gfc = 0;
	fog.bfc = 0;
	GsSetFogParam(&fog);		  
	// set 0: normal lighting 1: sets fog
	GsSetLightMode(1); 
#else
	GsSetLightMode(0);
#endif
 	
   
 }// end InitFog	
 



//--------------------------------------------------------------------------
// Function: InitView()
// Description: Setup projection distance and viewing coordinates.  
// Parameters: void
// Returns: void
// Notes: SCRN_Z is defined as 1000. A larger value gives more
//        of a parallel type look and smaller gives a perspective look.
//--------------------------------------------------------------------------

void InitView( void )			
 {

	GsSetProjection(SCRN_Z); 	
	
	// set view point coordinates 
	view.vpx = 0;
	view.vpy = 0;
	view.vpz = -2000;

	// set focus point coordinates	
	view.vrx = 0;
	view.vry = 0;
	view.vrz = 0;

	// set the roll parameter of viewpoint
	view.rz = 0;

	// set origin to WORLD 
	view.super = WORLD;	 

    GsSetRefView2(&view); 

 }// end InitView




//--------------------------------------------------------------------------
// Function: InitCoordinates()
// Description: Setting hierarchy of coordinate systems  
// Parameters: void
// Returns: void
// Notes: WORLD is the parent coordinate system, all others are childs
//--------------------------------------------------------------------------

void InitCoordinates( void )			
 { 	
   
   int cnt=0;  
         
   GsInitCoordinate2(WORLD,&coord_obj);

   for( cnt=0; cnt<10; cnt++)
      {	         
		GsInitCoordinate2(WORLD,&v_line[cnt].coord);
		GsInitCoordinate2(WORLD,&h_line[cnt].coord);
	  }

   GsInitCoordinate2(WORLD,&x_axis.coord);
   GsInitCoordinate2(WORLD,&y_axis.coord);
   GsInitCoordinate2(WORLD,&z_axis.coord);

 }// end InitCoordinates



//--------------------------------------------------------------------------
// Function: InitGrid()
// Description: Sets 3D grid  
// Parameters: void
// Returns: void
// Notes: N/A
//-------------------------------------------------------------------------- 

void InitGrid( void )
 {
	
	int cnt=0;
	int x=0,z=0;
	  
	// z
    for( cnt=0; cnt<10; cnt++)
	   {
		 v_line[cnt].p1.vx = x;
		 v_line[cnt].p1.vy = 0;
		 v_line[cnt].p1.vz = 0;

		 v_line[cnt].p2.vx = x;
		 v_line[cnt].p2.vy = 0;
		 v_line[cnt].p2.vz = 9000;
		 
		 v_line[cnt].line.r = 0x80;
		 v_line[cnt].line.g = 0x80; 
		 v_line[cnt].line.b = 0x80;

         x += 1000;

	   } 

	// x  
	for( cnt=0; cnt<10; cnt++)
	   {

		 h_line[cnt].p1.vx = 0;
		 h_line[cnt].p1.vy = 0;
		 h_line[cnt].p1.vz = z;

		 h_line[cnt].p2.vx = 9000;
		 h_line[cnt].p2.vy = 0;
		 h_line[cnt].p2.vz = z;

		 h_line[cnt].line.r = 0x80;
		 h_line[cnt].line.g = 0x80; 
		 h_line[cnt].line.b = 0x80;

         z += 1000;
		 
	   }
  
 }// end InitGrid




//--------------------------------------------------------------------------
// Function: InitAxis()
// Description: Sets 3D points of the axis  
// Parameters: void
// Returns: void
// Notes: N/A
//--------------------------------------------------------------------------

void InitAxis( void )
 {

    y_axis.line.r = 0x80,y_axis.line.g = 0x0,y_axis.line.b = 0x0;
    	
	y_axis.p1.vx = 0;
	y_axis.p1.vy = 1000;
	y_axis.p1.vz = 0;

	y_axis.p2.vx = 0;
	y_axis.p2.vy = -1000;
	y_axis.p2.vz = 0; 
    

    x_axis.line.r = 0x80,x_axis.line.g = 0x0,x_axis.line.b = 0x0;

	x_axis.p1.vx = -1000;
	x_axis.p1.vy = 0;
	x_axis.p1.vz = 0;

	x_axis.p2.vx = 1000;
	x_axis.p2.vy = 0;
	x_axis.p2.vz = 0; 
 

    z_axis.line.r = 0x80,z_axis.line.g = 0x0,z_axis.line.b = 0x0;	

	z_axis.p1.vx = 0;
	z_axis.p1.vy = 0;
	z_axis.p1.vz = -1000;

	z_axis.p2.vx = 0;
	z_axis.p2.vy = 0;
	z_axis.p2.vz = 1000;


}// end InitAxis




//--------------------------------------------------------------------------
// Function: InitImage()
// Description: Reads a tim image and sets up the sprite.  
// Parameters: void
// Returns: void
// Notes: N/A
//--------------------------------------------------------------------------

void InitImage( void )
 {
    
   GsIMAGE *tim;
   u_short tpage;
	
   tim    = (GsIMAGE*)ReadTIM((u_long*)SPRITE_ADDR);    
   tpage  = GetTPage(1,0,tim->px,tim->py);
			
   InitSprite( &sprite,(1<<24),-(SCREEN_WIDTH/2),-10,150,150,
               tpage,0,0,tim->cx,tim->cy,
               0x80,0x80,0x80,0,0,ONE,ONE,0 );

 }


//----------------------------------EOF-------------------------------------

	 