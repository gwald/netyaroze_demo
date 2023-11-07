//--------------------------------------------------------------------------
// File: genlib.c
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
#include "genlib.h"



//--------------------------------------------------------------------------
// Function: InitTMD
// Description:  Setup a single TMD object from a *.TMD model
// Parameters: *obj: address of 3D handler GsDOBJ2
//             *coord: address of objects coordinate handler
//             *addr: address of TMD model
// Returns: void
// Notes: N/A
//--------------------------------------------------------------------------

void InitTMD( GsDOBJ2 *obj, GsCOORDINATE2 *coord, u_long *addr )
 { 
   
   // header skip				  
   addr++;
   			 
   // mapping real address
   GsMapModelingData(addr);	 
    
   // link ObjectHandler and TMD FORMAT MODELING DATA 
   GsLinkObject4((u_long)(addr+2),obj,0);			
	
   // init coordinate system of object
   obj->coord2 = coord;

   // set translation
   coord->coord.t[0] = 0;
   coord->coord.t[1] = 0;
   coord->coord.t[2] = 0;  
  		
   // clear flag and attributes
   coord->flg = 0;
   obj->attribute = 0;
        
 }// end InitTMD




//--------------------------------------------------------------------------
// Function: UpdateCoordinates
// Description: Updates the coordinate of object
// Parameters: *svec: addr of short vector used for rotation matrix
//             *coord: addr of coordinate handler for 3D object
// Returns: void
// Notes: Rotation order is Z->Y->X when using RotMatrix().
//        
//        Flag clearing:
//
//        When requesting local-to-world matrix from the hierarchical
//        coordinate system, optimization is accomplished by setting the flg 
//		  member of a previously calculated coordinate system to 1 and
//        preserving the results stored in the member workm.
//
//        If the parameters of GsCOORDINATE2 have been rewritten, recalculation        
//        will NOT be performed unless the flags are 0-cleared, indicating
//        that the contents of workm have already been used.  Therefore, when
//        parameters are rewritten, always remember to 0-clear the flg member
//        of the rewritten GsCOORDINATE2.
//
//        If parent coordinates are modified, this is automatically reflected
//        in all the child coordinates, so there is no need to clear the child
//        coordinate's flg.
//--------------------------------------------------------------------------


void UpdateCoordinates( SVECTOR *svec, GsCOORDINATE2 *coord ) 		
 {

   MATRIX tmp;
   
   // set translation 
   tmp.t[0] = coord->coord.t[0];
   tmp.t[1] = coord->coord.t[1];
   tmp.t[2] = coord->coord.t[2];
   
   // rotate Matrix 
   RotMatrix(svec,&tmp);
   
   // set Matrix to Coordinate
   coord->coord = tmp;

   // clear flag because of changing parameter 
   coord->flg = 0;

 }// end UpdateCoordinates





//--------------------------------------------------------------------------
// Function: SortObject
// Description: Z sorts the 3D object into the Ordering table after calculating
//              local and screen matrices, etc.. 
// Parameters: *obj: address of 3D object handler
//             *wot: address of world ordering table
//             shift: the shift value into the ordering table
// Returns: void
// Notes: N/A
//--------------------------------------------------------------------------
 

void SortObject( GsDOBJ2 *obj, GsOT *wot, int shift )
 {

	MATRIX local_world, local_screen;

	// calculate both local world and local screen matrices
	GsGetLws(obj->coord2, &local_world, &local_screen);

	// set LWMATRIX to GTE Lighting Registers
	GsSetLightMatrix(&local_world);	

	// sSet LSMATRIX to GTE Registers
	GsSetLsMatrix(&local_screen);

	// set perspective and light calculations, Z sort and send to OT
	GsSortObject4(obj, wot, shift, (u_long*)getScratchAddr(0));
 
 	 
  }// end SortObject 





//--------------------------------------------------------------------------
// Function: Sort3DLine
// Description: Z sort the line into the ordering table
// Parameters: *line: address of line handler
//             *coord: coordinate handler for the line
//             *point1: address of first point of the 3D line
//             *point2: address of second point of the 3D line
// Returns: void
// Notes: Due to z-sorting problems and the near clipping plane, the lines 
//        may not appear properly.
//        
//--------------------------------------------------------------------------

void Sort3DLine( GsLINE *line, GsCOORDINATE2 *coord, VECTOR *point1, VECTOR *point2) 
 {

    VECTOR transPos1,transPos2;
	MATRIX local_screen;
	int otz;
	GsOT *otp;
	otp = wot;

	GsGetLs(coord, &local_screen); 

    ApplyMatrixLV(&local_screen,point1,&transPos1);
    transPos1.vx += local_screen.t[0];
    transPos1.vy += local_screen.t[1];
    transPos1.vz += local_screen.t[2];

    ApplyMatrixLV(&local_screen,point2,&transPos2);
    transPos2.vx += local_screen.t[0];
    transPos2.vy += local_screen.t[1];
    transPos2.vz += local_screen.t[2];

    if( !(transPos1.vz <=0) && !(transPos2.vz <=0) ) 
	  {
		line->x0 = (transPos1.vx * SCRN_Z) / transPos1.vz;
		line->y0 = (transPos1.vy * SCRN_Z) / transPos1.vz;
		
		line->x1 = (transPos2.vx * SCRN_Z) / transPos2.vz;
		line->y1 = (transPos2.vy * SCRN_Z) / transPos2.vz;

	  	otz = (transPos1.vz + transPos2.vz) / 2;

	   	if( (otz >=0) && (otz <16384) ) 
	   	  {
			otz >>= 14 - OT_LENGTH;	
	        GsSortLine(line,&wot[out_buf], otz);
	   	  }			   

      }

 }// end Sort3DLine

				 



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

#ifdef DEBUG
    printf(" IMAGE - x:(%d), y:(%d), w:(%d), h:(%d) \n", tim.px, tim.py,tim.pw,tim.ph );  
    printf(" CLUT - x:(%d), y:(%d), w:(%d), h:(%d) \n", tim.cx, tim.cy,tim.cw,tim.ch );
    printf(" image mode:%d \n", tim.pmode);   
	printf(" tim addr: %p \n",&tim);
#endif

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
