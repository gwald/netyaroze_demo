// 
// pad.h
// 
// Pad related defines structures and prototypes
// 

#ifndef PAD_H
#define PAD_H

// ----------------------------------------------
// Defines
// ----------------------------------------------
#define PadUp        (1<<12)
#define PadDown      (1<<14)
#define PadLeft      (1<<15)
#define PadRight     (1<<13)
#define PadTriangle  (1<<4)
#define PadCross     (1<<6)
#define PadSquare    (1<<7)
#define PadCircle    (1<<5)
#define PadStart     (1<<11)
#define PadSelect    (1<<8)
#define PadL1        (1<<2)
#define PadL2        (1<<0)
#define PadL3        (1<<9)       //analogue pushbutton
#define PadR1        (1<<3)
#define PadR2        (1<<1)
#define PadR3        (1<<10)      //analogue pushbutton

#define PadIn   	 (0x00)
#define PadOut  	 (0xff)

#define PadStandard  (0x4)
#define PadAnalogue  (0x5)
#define PadDualShock (0x7)

// ----------------------------------------------
// Structures
// ----------------------------------------------

// this structure implements the advanced control
// pad functionality
typedef struct
{
	int old,
		state,
		toggle,
		release,
		press;
}PadInfo;

// ----------------------------------------------
// Prototypes
// ----------------------------------------------
void 	InitPads		(void);
int 	PadConnected	(int port);
int 	PadType			(int port);
int 	PadState		(int port);
int 	PadLHorz		(int port);
int 	PadLVert		(int port);
int 	PadRHorz		(int port);
int 	PadRVert		(int port);
int 	GetPadData		(int port, PadInfo *pPadData);
void 	ClearPadData	(PadInfo *pPadData);

// horz analogue values: 0 left, 255 right
// vert analogue values: 0 up, 255 down

#endif 
