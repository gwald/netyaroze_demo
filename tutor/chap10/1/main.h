//--------------------------------------------------------------------------
// File: main.h
// Author: George Bain
// Date: March 1999   
// Description: Prototypes
// Copyright (C) 1999 Sony Computer Entertainment Europe.,
//           All Rights Reserved.  
//--------------------------------------------------------------------------
   
#ifndef _MAIN_H
#define _MAIN_H

//--------------------------------------------------------------------------
// D E F I N E S
//--------------------------------------------------------------------------

// screen resolution
#define SCREEN_WIDTH  (512)
#define SCREEN_HEIGHT (256)

//#define FOG
//#define DEBUG	

#define OT_LENGTH  (8)	    // bit length of OT 
#define SCRN_Z     (1<<9)   // 512 projection distance
#define NEAR_CLIP  (1<<8)   // 256 near clipping plane
#define FAR_CLIP   (1<<13)  // far clipping plane 
#define PACKETMAX  (10000)	// num GPU packets

#define PACKETMAX2   (PACKETMAX*24)  // max GPU packets
#define OBJECT_ADDR   (0x80090000)	 // top address of texture data
#define SPRITE_ADDR   (0x80092000)	 // top address of texture data

//--------------------------------------------------------------------------
// S T R U C T U R E S
//--------------------------------------------------------------------------

typedef struct line3d_type 
 {

   GsLINE line; 
   GsCOORDINATE2 coord;				 
   VECTOR p1;							 
   VECTOR p2;							 

 }line_handler, *line_ptr; 


//--------------------------------------------------------------------------
// G L O B A L S
//--------------------------------------------------------------------------

extern int out_buf;                     			 // buffer index
extern GsOT wot[2];                				 // ordering table headers
extern GsOT_TAG ot[2][1<<OT_LENGTH];    			 // actual ordering tables
extern PACKET gpu_work_area[2][PACKETMAX2];         // GPU packet work area
extern u_char prev_mode;					         // previous mode
extern int fnt_id[9];						         // font id 
extern cntrl_data buffer[2];						 // controller buffers 

extern GsRVIEW2 view;			     				 // view handler  
extern GsF_LIGHT light[3];		  					 // 3 light handlers
extern GsFOGPARAM fog;								 // fog handler

extern GsDOBJ2 obj;								 // 3D object handler
extern GsCOORDINATE2 coord_obj;					 // 3D object coordinate handler
extern SVECTOR sv_obj;								 // vector for 3D object

extern GsSPRITE sprite;					 		     // image  

extern line_handler x_axis,y_axis, z_axis;			 // axis lines
extern line_handler h_line[10], v_line[10];		     // grid lines

//--------------------------------------------------------------------------
// P R O T O T Y P E S
//--------------------------------------------------------------------------

void InitAll( void );
void UpdateScreen( void );
void DeInitAll( void );
void ControlObject( void );

#endif

//----------------------------------EOF-------------------------------------
