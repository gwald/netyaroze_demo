// 
// pad.c
// 
// Pad related functions for setting up and getting pad input
// 

#include <libps.h>
#include "pad.h"

volatile u_char *bb[2];

// gets the area of memory that the pad buffers are stored in
void InitPads(void) 
{
	GetPadBuf(&bb[0],&bb[1]);
}
	
// checks to see if a pad is connected on the specified port
int PadConnected(int port) 
{ 
	return *bb[port]; 
}

// returns the type of pad connected to the specified port
// this must be checked against an ID at the calling function	
int PadType(int port) 
{ 
	return (*(bb[port]+1))>>4; 
}

// query the pad buffers as to which keys are pressed
// used with bit masks to determine if a button is pressed	
int PadState(int port) 
{ 
	return ~(*(bb[port]+3) | *(bb[port]+2)<<8); 
}
	
// get the degree to which the analog sticks are moving
// on each axis for both sticks
int PadLHorz(int port) 
{ 
	return *(bb[port]+6); 
}
	
int PadLVert(int port) 
{ 
	return *(bb[port]+7); 
}
	
int PadRHorz(int port) 
{ 
	return *(bb[port]+4); 
}
	
int PadRVert(int port) 
	{
	return *(bb[port]+5); 
	}
	
int GetPadData(int port, PadInfo *pPadData)
{
	// set the key that was pressed last frame
	pPadData->old 		= pPadData->state;
	
	// set the pad's current state
	pPadData->state 	= ~(*(bb[port]+3) | *(bb[port]+2)<<8);
	
	// the key has changed state between frames
	pPadData->toggle 	= pPadData->old ^ pPadData->state;
	
	// the key has just been pressed this frame
	pPadData->press 	= pPadData->toggle & pPadData->state;
	
	// the key has just been released this frame
	pPadData->release 	= pPadData->toggle & pPadData->old;
	
	// return the pad state
	return(pPadData->state);
}

void ClearPadData(PadInfo *pPadData)
{
	// zero the pad structure
	pPadData->old = 0;
	pPadData->state = 0;
	pPadData->toggle = 0;
	pPadData->press = 0;
	pPadData->release =  0;
}
