/************************************************************
 *															*
 *						storescr.h							*
 *			   											    *															*
 *				LPGE     29/10/96							*		
 *															*
 *	Copyright (C) 1996 Sony Computer Entertainment Inc.		*
 *	All Rights Reserved										*
 *															*
 ***********************************************************/


// screen shot module header



// no initialiser
// just a single call when exiting program




#include <libps.h>

#include "address.h"

#include "test1.h"			// know screen width and height



	// compile time switch
#define STORING_SCREEN 0




	// prototypes
		
void StoreScreen (void);

void StoreScreen2 (u_long *destination, int x, int y, int w, int h);

