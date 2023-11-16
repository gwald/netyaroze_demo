/*Original file:	*/
/*Copyright (C) 1996 Sony Computer Entertainment Inc.*/
/*All Rights Reserved*/

/*Use:

		 Call PadInit(); as part of initialisation in main

		 Each frame (VSync loop), call PadRead() this puts pad status into an unsigned long variable

		 E.g. 
     u_long padStatus;
     padStatus = PadRead();

		 Test individual buttons by masking the pad status variable with the #defined constants in pad.h

		 E.g. 
    if (padStatus & PADLleft)	 // Pad L left button is pressed
    {
				 // do actions here
				 }
*/

#include <libps.h>
#include "pad.h"

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



