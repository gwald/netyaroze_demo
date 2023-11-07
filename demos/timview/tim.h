/************************************************************
 *															*
 *						Tim.h								*
 *															*
 *															*
 *	Copyright (C) 1996 Sony Computer Entertainment Inc.		*
 *	All Rights Reserved										*
 *															*
 ***********************************************************/


	// prototypes
void LoadTimFile (u_long mainMemoryAddress);

void LoadTimFileToGivenDestinations (u_long mainMemoryAddress,
				RECT* pixelDestination, RECT* clutDestination);
