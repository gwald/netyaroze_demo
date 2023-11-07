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

#include "general.h"

#include "tmdutil.h"


  

// NOTE: room for improvement here: 
// this module does most basic linking, and whenever the 
// GsDOBJ2 - TMD linker is called, it initialises TMD
// (address mapping); might as well just do that once, and know
// that for every TMD, its 'effective' (call to GsLinkObject4)
// address is (memoryAddress+3)



	// always links to object # zero in TMD file
void LinkObjectHandlerToSingleTMD (GsDOBJ2 *objectHandler, 
					u_long tmdAddress);


	// link object handler (GsDOBJ2 structure) to an object within
	// TMD data, so that a three-D model can be manipulated
void LinkObjectHandlerToTmdObject (GsDOBJ2 *objectHandler, 
					int whichObjectInTmdFile, u_long tmdAddress);

   
	// how many objects in TMD file ?
int CountNumberOfObjectsInTmdFile (u_long tmdAddress);



void makeTMD_F_4T(u_long *addr, u_short CBA, u_short TSB, 
					u_char u0, u_char v0,
					u_char u1, u_char v1, 
					u_char u2, u_char v2, 
					u_char u3, u_char v3,
					TMD_NORM* norm0, 
					TMD_VERT* vert0, 
					TMD_VERT* vert1, 
					TMD_VERT* vert2, 
					TMD_VERT* vert3);


void CreateSimpleTMD (u_long* addr, int clutX, int clutY, int tPageID,
					int pixelMode, 
					u_char u0, u_char v0,
					u_char u1, u_char v1, 
					u_char u2, u_char v2, 
					u_char u3, u_char v3,
					TMD_NORM* norm0, 
					TMD_VERT* vert0, 
					TMD_VERT* vert1, 
					TMD_VERT* vert2, 
					TMD_VERT* vert3);


void AssignVerticesAndNormal(u_long *addr,
					TMD_NORM* norm0, TMD_VERT* vert0, 
						TMD_VERT* vert1, TMD_VERT* vert2, TMD_VERT* vert3);

void AssignVerticesNormalsAndUVs(u_long *addr, u_char u0, u_char v0,
					u_char u1, u_char v1, 
					u_char u2, u_char v2, 
					u_char u3, u_char v3,
					TMD_NORM* norm0, 
					TMD_VERT* vert0, 
					TMD_VERT* vert1,
					TMD_VERT* vert2, 
					TMD_VERT* vert3);