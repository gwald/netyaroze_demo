/***************************************************************
*                                                              *
* Copyright (C) 1995 by Sony Computer Entertainment            *
*                       All rights Reserved                    *
*                                                              *
* S. Ashley 29/11/96                                           *
*                                                              *
***************************************************************/

/* LIMITATIONS:
	Calculation of work base is done on the basis that a text print
	function is called every frame. If this is not the case the results
	can be erratic		
*/

	// maximum number of text sprites 
#define MAX_spriteStringLength 256

	// the text sprite. Gs makes a copy of the sprite sent to the ordering
	// table therefore only need one sprite for the whole string
GsSPRITE spriteString;
PACKET spriteStringPacketArea[2][MAX_spriteStringLength * 24];
PACKET *spriteStringPacketPointer;
int spriteStringCurrentBuffer;

	// move font texture from timAdress into video ram and initialise
	// text sprites
void initTextFont (u_long *timAddress);
	// make text fit a given rectangle
void textPrintToBox(RECT stringBox, u_char r, u_char g, u_char b, 
								char *string, GsOT *otp, u_short pri);
	// display text at a position at a given size
void textPrintToSize(u_short x, u_short y, u_short sizeInPixels,
	 u_char r, u_char g, u_char b, char *string, GsOT *otp, u_short pri);
