/*
 * LIBPAD.C
 *
 * Copyright (C) 1993-1997 by Sony Computer Entertainment of America, Inc.
 * All rights Reserved
 * ----------------------------------------------------------------------------
 *
 * Controller related routines for Net Yaroze development system
 * ----------------------------------------------------------------------------
 */

#include "libmain.h"
#include "libpad.h"

/* Controller related variables and functions */

volatile unsigned char	*PadDataBuff[8];

/* Last controller status saved here */

unsigned long			old_PadData = 0;

/* Current controller status read here */

unsigned long			PadData = 0;

/* Controller initialization - Get controller reception buffer */

void PadInit(void)
{
	GetPadBuf(&PadDataBuff[0], &PadDataBuff[1]);
	old_PadData = PadData = 0;
	return;
}

/* Controller state reading */

unsigned long PadRead(void)
{
	old_PadData = PadData;
	return(PadData = ~(	(PadDataBuff[0][3] <<  0) |
						(PadDataBuff[0][2] <<  8) |
						(PadDataBuff[1][3] << 16) |
						(PadDataBuff[1][2] << 24)));
}

/* Dumps the controller related information out to the debug console */

void PadDump(void)
{
	short i;
	for (i = 0; i < 2; i++) {
		printf("Controller PORT #%d :-\n", i);
		if (PadDataBuff[i][0] == PADNone) {
			printf("\tNo Controller present on port.\n");
		}
		else  {
			printf("\tController present on port.\n");
			switch (PadDataBuff[i][1] & PADTerminalTypeMask) {
			case PADMouse:
				printf("\tPlayStation Mouse on port.\n");
				break;
			
			case PADNeGCon:
				printf("\tNeGCon on port.\n");
				break;
			
			case PADStandardController:
				printf("\tStandard controller on port.\n");
				break;
			
			case PADJoystick:
				printf("\tJoystick on port.\n");
				break;
			}
			printf("\tData size received on port is %d.\n",
					(PadDataBuff[i][1] & PADReceivedDataSizeMask)*2);
		}
		printf("\t\n");
	}
	return;
}

/* Waits for a key from the controller to continue processing */

void PadWait(void)
{
	printf("Press any key on the controller to continue...\n");
	while(!PadRead())
	;
	printf("\n");
}

/*
 * End of File
 * ----------------------------------------------------------------------------
 */
