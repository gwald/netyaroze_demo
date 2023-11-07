#ifndef VID_MODE_H_INCLUDED





/****************************************************************************
					includes
****************************************************************************/


#include <libps.h>

#include <stdio.h>

#include "asssert.h"

#include "general.h"

#include "dump.h"



extern int DefaultVideoMode;


extern int ScreenWidth;
extern int ScreenHeight;


extern int MaximumHSyncsPerSingleFrame;
extern int MaxFramesPerSecond;




void InitialiseVideoMode (void);



void PrintCurrentVideoMode (void);

	// simplest overall initialiser
void SortVideoMode (void);






#define VID_MODE_H_INCLUDED	1

#endif