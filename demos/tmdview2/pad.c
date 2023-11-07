/************************************************************
 *															*
 *						Pad.c								*
 *															*
 *															*
 *		Set of pad routines to present a similar interface 	*
 *		to the controller pad as dtlh2500					*
 *															*
 *	Copyright (C) 1996 Sony Computer Entertainment Inc.		*
 *	All Rights Reserved										*
 *															*
 ***********************************************************/




/***************************************
			Use:

		call PadInit();	as part of initialisation in main

		each frame (VSync loop), 
		call PadRead(): puts pad status into an unsigned long variable

		e.g. padStatus = PadRead();

		then, test individual buttons by masking the pad status variable
		with the #defined constants in pad.h

		e.g. if (padStatus & PADLleft)	 // Pad L left button is pressed
				{
				// do actions here
				}


***************************************/



#include <libps.h>

#include "pad.h"			// prototypes header


 
	// low-level pad buffers: never need to touch
volatile u_char *bb0, *bb1;



   
  
	// call once only in program initialisation
void PadInit (void)
{
	GetPadBuf(&bb0, &bb1);
}





	// call once per VSync(0)
	// puts controller pad status into unsigned long integer
	  
	// please refer to the manuals if you want explanation 
	// of the internals of this function
u_long PadRead(void)
{
	return(~(*(bb0+3) | *(bb0+2) << 8 | *(bb1+3) << 16 | *(bb1+2) << 24));
}
	


