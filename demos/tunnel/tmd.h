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





	// Prototypes for Tmd.c





	// link object handler (GsDOBJ2 structure) to an object within
	// TMD data, so that a three-D model can be manipulated
void LinkObjectHandlerToTmdObject (GsDOBJ2 *objectHandler, 
					int whichObjectInTmdFile, u_long tmdAddress);

  	// link an array of TMDs to an array of object handlers
	// NOTE: this function assumes that it is the first object
	// in each TMD file which is to be linked
	// if this assumption is incorrect, code is easily copied and modified
void LinkArrayOfTmdsToObjectHandlerArray (int numberOfObjects,
		GsDOBJ2* handlerArray, u_long* addressesArray);


	// how many objects in TMD file ?
int CountNumberOfObjectsInTmdFile (u_long tmdAddress);