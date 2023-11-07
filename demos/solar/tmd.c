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



#include <libps.h>

#include "tmd.h"		// prototypes header



// MORE TO DO:
// business on attributes



	// link object handler (GsDOBJ2 structure) to an object within
	// TMD data, so that a three-D model can be manipulated
void LinkObjectHandlerToTmdObject (GsDOBJ2 *objectHandler, 
					int whichObjectInTmdFile, u_long tmdAddress)
{
	u_long *pointer;

		// set pointer to top of TMD in main memory
	pointer = (u_long *) tmdAddress;
	pointer++;			// skip TMD header

		// Map TMD offset addresses to real addresses 
	GsMapModelingData(pointer);

	pointer += 2;	  // skip some more
	
		// link TMD data to object handler	
	GsLinkObject4( (u_long)pointer, objectHandler, whichObjectInTmdFile);

		// mini-initialisation of object handler
	objectHandler->attribute = 0;
}





	// link an array of TMDs to an array of object handlers
	// NOTE: this function assumes that it is the first object
	// in each TMD file which is to be linked
	// if this assumption is incorrect, code is easily copied and modified
void LinkArrayOfTmdsToObjectHandlerArray (int numberOfObjects,
		GsDOBJ2* handlerArray, u_long* addressesArray)
{
	int i;	
	u_long *pointer;

	for (i = 0; i < numberOfObjects; i++)
		{
			// set pointer to top of TMD in main memory
		pointer = (u_long *) addressesArray[i];
		pointer++;			// skip TMD header

			// Map TMD offset addresses to real addresses 
		GsMapModelingData(pointer);

		pointer += 2;	  // skip some more
	
			// link TMD data to object handler	
		GsLinkObject4( (u_long)pointer, &(handlerArray[i]), 0);

			// mini-initialisation of object handler
		handlerArray[i].attribute = 0;
		}
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