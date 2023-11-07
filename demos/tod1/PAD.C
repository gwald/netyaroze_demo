/************************************************************
 *															*
 *						pad.c								*
 *			   											    *															*
 *				LPGE     1997								*		
 *															*
 *	Copyright (C) 1996 Sony Computer Entertainment Inc.		*
 *	All Rights Reserved										*
 *															*
 ***********************************************************/



// standard controller pad interface module




/****************************************************************************
					includes
****************************************************************************/


#include <libps.h>

#include "pad.h"



/****************************************************************************
					globals
****************************************************************************/


         

	// buffers
volatile u_char *bb0, *bb1;




/****************************************************************************
					functions
****************************************************************************/




  
	// set up buffers
void PadInit (int dummy)
{
	GetPadBuf(&bb0, &bb1);
}


	// nice and comprehensible ...
u_long PadRead (int dummy)
{
	return(~(*(bb0+3) | *(bb0+2) << 8 | *(bb1+3) << 16 | *(bb1+2) << 24));
}


	


	// on left side of PSX	

u_short ReadFirstPad (int dummy)
{
	u_short firstPadStatus;
	u_long wholePad;
	
	wholePad = PadRead(0);
	
	firstPadStatus = wholePad & 0xffff;
	
	return firstPadStatus;
}	



	// on right side of PSX

u_short ReadSecondPad (int dummy)
{
	u_short secondPadStatus;
	u_long wholePad;
	
	wholePad = PadRead(0);
	
	secondPadStatus = wholePad >> 16;
	
	return secondPadStatus;
}	


   
