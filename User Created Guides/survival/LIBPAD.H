/*
 * LIBPAD.H
 *
 * Copyright (C) 1993-1997 by Sony Computer Entertainment of America, Inc. 
 * All rights Reserved
 * ----------------------------------------------------------------------------
 */

#ifndef	__YAROZE_LIBPAD_H
#define	__YAROZE_LIBPAD_H

/*
 * Controller related routines for Net Yaroze development system :-
 *
 * PadInit() should always call before using any other function from
 * this library.
 *
 * PadRead() reads the controller and sets PadData. Lower 16 bits of
 * PadData is the first controller reading and higher 16 bits provide
 * the second controller status.  PadRead() also saves the last controller
 * status in old_PadData.  PadRead() also returns the current controller
 * status.
 *
 * PadDump() provides the debug information related to the controllers.
 * PadWait() waits until any button on either of the controller is pressed.
 * ----------------------------------------------------------------------------
 */

#define PADNone					0xFF	// No controller present
#define PADPresent				0x00	// Controller present

#define PADTerminalTypeMask		0xF0	// Type of controller
#define PADReceivedDataSizeMask	0x0F	// A maximum of 32 bytes of data

#define PADMouse				0x10	// Mouse present
#define PADNeGCon				0x20	// NeGCon present
#define PADStandardController	0x40	// Standard controller present
#define PADJoystick				0x50	// Joystick present

#define PADo					0X0001
#define PADm					0X0002
#define PADn					0X0004
#define PADl					0X0008

#define PADk					0x0100
#define PADi					0x0200
#define PADj					0x0400
#define PADh					0x0800

#define PADRup					0x0010	// Triangle
#define PADRright				0x0020	// Circle
#define PADRdown				0x0040	// X mark
#define PADRleft				0x0080	// Square

#define PADLup					0x1000	// Pad up
#define PADLright				0x2000	// Pad right
#define PADLdown				0x4000	// Pad down
#define PADLleft				0x8000	// Pad left

#define PADL1					PADn	// Pad left button one
#define PADL2					PADo	// Pad left button two

#define PADR1					PADl	// Pad right button one
#define PADR2					PADm	// pad right button two

#define PADStart				PADh	// Start button
#define PADSelect				PADk	// Select button

extern volatile unsigned char *PadDataBuff[8];	// pointers to internal buffers to store controller status 

extern unsigned long	old_PadData;			// Last controller status saved here
extern unsigned long	PadData;				// Current controller status read here

extern void PadInit(void);
extern unsigned long PadRead(void);
extern void PadDump(void);
extern void PadWait(void);

/*
 * End of File
 * ----------------------------------------------------------------------------
 */

#endif
