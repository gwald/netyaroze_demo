/************************************************************
 *															*
 *						profile.c 							*
 *															*
 *	Copyright (C) 1996 Sony Computer Entertainment Inc.		*
 *	All Rights Reserved										*
 *															*
 *	S Ashley  23/12/96										*
 *															*
 ***********************************************************/

#include "profile.h"

//******************************************************************************
// VSync is used as the pause in sampling, to use a single vsync as the point
// to grab is not certain, so a range is needed this means that more than one
// grab may be done before the next pause so the waiting time needs to be split
// timeFlg 0-grab first time          1-grab other times / print times next reset
//         2-wait for end of grab time   3-wait for next grab time
void resetTimeSample(void)
	{
	if (ProfileTimeFlag == 0)  // grab time
		{
		ProfileTimeCount[ProfileTimeNumber++] = GetRCnt(1);
		ProfileTimeFlag = 1;
		}
	else if (ProfileTimeFlag == 1)  // print times
		{
		int i;
		for (i = 0; i < (ProfileTimeNumber - 1); i++)
	 		printf("%d ",ProfileTimeCount[i+1] - ProfileTimeCount[i] + (ProfileTimeCount[i+1] - ProfileTimeCount[i] < 0 ? 65536 : 0));
		printf("\n");
		ProfileTimeFlag = 2;
		}
	else if ((VSync(-1) & 0xff) < 128)  // wait
		ProfileTimeFlag = 3;
	else if ((VSync(-1) & 0xff) > 128 && ProfileTimeFlg == 3)  // reset
		{
		ProfileTimeNumber = 0;
		ProfileTimeFlag = 0;
		}
	}


//******************************************************************************
// once set up only a simple time catching routine is needed
void timeSample(void)
	{
	if (ProfileTimeFlag == 1)  // grab times
		ProfileTimeCount[ProfileTimeNumber++] = GetRCnt(1);
	}
