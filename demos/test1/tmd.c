/************************************************************
 *															*
 *						Tmd.c								*
 *															*
 *		Routines for linking TMD three-D models				*
 *			with object handlers (GsDOBJ2 structures)		*
 *															*
 *	Copyright (C) 1996 Sony Computer Entertainment Inc.		*
 *	All Rights Reserved										*
 *															*
 ***********************************************************/


#include "tmd.h"		



 



	// link object handler (GsDOBJ2 structure) to an object within
	// TMD data, so that a three-D model can be manipulated
void LinkObjectHandlerToTmdObject (GsDOBJ2 *objectHandler, 
					int whichObjectInTmdFile, u_long tmdAddress)
{
	u_long *pointer;

	assert(CountNumberOfObjectsInTmdFile(tmdAddress) 
				>= whichObjectInTmdFile);

		// set pointer to top of TMD in main memory
	pointer = (u_long *) tmdAddress;
	pointer++;			// skip TMD header

		// Map TMD offset addresses to real addresses 
	GsMapModelingData(pointer);

	pointer += 2;	  // skip some more
	
		// link TMD data to object handler	
	GsLinkObject4( (u_long)pointer, objectHandler, whichObjectInTmdFile);
}




	

	// how many objects in TMD file ?
int CountNumberOfObjectsInTmdFile (u_long tmdAddress)
{
	int numberOfObjects;		// function return value
	u_long* pointer;

	pointer = (u_long *) tmdAddress;
	pointer++;				// skip TMD header

		// Map TMD offset addresses to real addresses
	GsMapModelingData(pointer);
	pointer++;				// skip next part of TMD

		// this part of TMD file tells how many objects it holds
	numberOfObjects = *pointer;

	return numberOfObjects;
}