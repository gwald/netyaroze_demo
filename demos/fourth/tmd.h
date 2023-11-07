/************************************************************
 *															*
 *						Tmd.h								*
 *															*
 *		Routines for linking TMD three-D models				*
 *			with object handlers (GsDOBJ2 structures)		*
 *															*
 *	Copyright (C) 1996 Sony Computer Entertainment Inc.		*
 *	All Rights Reserved										*
 *															*
 ***********************************************************/



#include <libps.h>

#include <stdio.h>

#include "asssert.h"



// NOTE: room for improvement here: 
// this module does most basic linking, and whenever the 
// GsDOBJ2 - TMD linker is called, it initialises TMD
// (address mapping); might as well just do that once, and know
// that for every TMD, its 'effective' (call to GsLinkObject4)
// address is (memoryAddress+3)




long* InitialiseTMD (long* topAddress);


	// link object handler (GsDOBJ2 structure) to an object within
	// TMD data, so that a three-D model can be manipulated
void LinkObjectHandlerToTmdObject (GsDOBJ2 *objectHandler, 
					int whichObjectInTmdFile, u_long tmdAddress);

   
	// how many objects in TMD file ?
int CountNumberOfObjectsInTmdFile (u_long tmdAddress);