/************************************************************
 *															*
 *						profile.h 							*
 *															*
 *	Copyright (C) 1996 Sony Computer Entertainment Inc.		*
 *	All Rights Reserved										*
 *															*
 *	S Ashley  23/12/96										*
 *															*
 ***********************************************************/

#ifndef _profile_h

#include <libps.h>

#define _profile_h
#define MaxNumberOfProfileTimes 50	 // max number of samples that can be taken

u_long ProfileTimeCount[MaxNumberOfProfileTimes];
u_char ProfileTimeFlag = 3;
u_short ProfileTimeNumber = 0;

void timeSample(void);
void resetTimeSample(void);

#endif