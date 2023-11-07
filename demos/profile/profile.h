#include <libps.h>
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


#define _profile_h
#define MaxNumberOfProfileTimes 50

u_long ProfileTimeCount[MaxNumberOfProfileTimes];
u_char ProfileTimeFlag = 3;
u_short ProfileTimeNumber = 0;

void timeSample(void);
void resetTimeSample(void);
