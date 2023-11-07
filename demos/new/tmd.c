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




#define setVector(v,_x,_y,_z) \
		(v)->vx = (_x), (v)->vy = (_y), (v)->vz = (_z)

#define setNormal(n,_x,_y,_z) \
		(n)->nx = (_x), (n)->ny = (_y), (n)->nz = (_z)


 


	// always links to object # zero in TMD file
void LinkObjectHandlerToSingleTMD (GsDOBJ2 *objectHandler, 
					u_long tmdAddress)
{
	u_long *pointer;

		// set pointer to top of TMD in main memory
	pointer = (u_long *) tmdAddress;
	pointer++;			// skip TMD header

		// Map TMD offset addresses to real addresses 
	GsMapModelingData(pointer);

	pointer += 2;	  // skip some more
	
		// link TMD data to object handler	
	GsLinkObject4( (u_long)pointer, objectHandler, 0);
}




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







//*****************************************************************************
//	order of vertices
//		0 +----+ 1
// 		 |     |
//     2 +----+ 3

// see tmdutil.h if confused

// arguments:
// addr is where created-TMD should go
// CBA is clut ID: use (clutX / 16) + (clutY * 64)
// CBA: three parts: (i) 0-31 tpage ID (ii) semi-trans rate
// (iii) pixel mode: add 0 for 4bit, add 128 for 8bit, add 256 for 16bit
// (see notes on graphic file formats)
// TSB: tpage id: laid out on frame buffer: 0-15 top row, 16-31 bottom row,
// each one is 64 wide by 256 down
// u0, v0 <-> u3,v3 are four points which specify the rectangle on the tpage
// which is used as texture; 
// norm0 is the normal (structure like SVECTOR)
// vert0-vert3 are the 4 corners of the quadrilateral
void makeTMD_F_4T(u_long *addr, u_short CBA, u_short TSB, u_char u0, u_char v0,
					u_char u1, u_char v1, u_char u2, u_char v2, u_char u3, u_char v3,
					TMD_NORM* norm0, TMD_VERT* vert0, 
						TMD_VERT* vert1, TMD_VERT* vert2, TMD_VERT* vert3)
{
	((TMD_HDR *)addr)->id = 0x00000041;
	((TMD_HDR *)addr)->flag = 1;
	((TMD_HDR *)addr)->nobjs = 1;

	addr = (u_long *)((TMD_HDR *)addr + 1);

	((TMD_OBJ *)addr)->vert_top = addr + 15;
	((TMD_OBJ *)addr)->n_vert = 4;
	((TMD_OBJ *)addr)->norm_top = addr + 23;
	((TMD_OBJ *)addr)->n_norm = 1;
	((TMD_OBJ *)addr)->prim_top = addr + 7;
	((TMD_OBJ *)addr)->n_prim = 1;
	((TMD_OBJ *)addr)->scale = 0;

	addr = (u_long *)((TMD_OBJ *)addr + 1);

	((PRIM_HDR *)addr)->olen = 9;
	((PRIM_HDR *)addr)->ilen = 7;
	((PRIM_HDR *)addr)->flag = 0;
	((PRIM_HDR *)addr)->mode = 0x2c;

	addr = (u_long *)((PRIM_HDR *)addr + 1);

	((TMD_F_4T *)addr)->cba = CBA;
	((TMD_F_4T *)addr)->tsb = TSB;
	((TMD_F_4T *)addr)->u0 = u0;     ((TMD_F_4T *)addr)->v0 = v0;
	((TMD_F_4T *)addr)->u1 = u1;     ((TMD_F_4T *)addr)->v1 = v1;
	((TMD_F_4T *)addr)->u2 = u2;     ((TMD_F_4T *)addr)->v2 = v2;
	((TMD_F_4T *)addr)->u3 = u3;     ((TMD_F_4T *)addr)->v3 = v3;
	((TMD_F_4T *)addr)->norm0 = 0;	 
	((TMD_F_4T *)addr)->vert0 = 0;   ((TMD_F_4T *)addr)->vert1 = 1;
	((TMD_F_4T *)addr)->vert2 = 2;   ((TMD_F_4T *)addr)->vert3 = 3;

	addr = (u_long *)((TMD_F_4T *)addr + 1);
	setVector((TMD_VERT *)addr, vert0->vx, vert0->vy, vert0->vz);

	addr = (u_long *)((TMD_VERT *)addr + 1);
	setVector((TMD_VERT *)addr, vert1->vx, vert1->vy, vert1->vz);

	addr = (u_long *)((TMD_VERT *)addr + 1);
	setVector((TMD_VERT *)addr, vert2->vx, vert2->vy, vert2->vz);

	addr = (u_long *)((TMD_VERT *)addr + 1);
	setVector((TMD_VERT *)addr, vert3->vx, vert3->vy, vert3->vz);

	addr = (u_long *)((TMD_VERT *)addr + 1);

	// see how big it is: VERY small: size about 0x00000070
	//printf("addr: %d ie %08x\n", (int)addr, addr);
	setNormal((TMD_NORM *)addr, norm0->nx, norm0->ny, norm0->nz);
}




	// note: no transparency used yet ...
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
					TMD_VERT* vert3)
{
	u_short CBA, TSB;

	CBA = (clutX / 16) + (clutY * 64);

	TSB = tPageID;
	switch (pixelMode)
		{
		case 4:		/* do nowt */ 		break;
		case 8:		TSB += 128; 	   	break;
		case 16:	TSB += 256; 		break;
		default:						assert(FALSE);
		}

	makeTMD_F_4T(addr, CBA, TSB, 
						u0, v0,
				 		u1, v1, 
				 		u2, v2, 
				 		u3, v3,
				 		norm0, 
				 		vert0, 
					 	vert1, 
					 	vert2, 
					 	vert3);
}









	// TWO BELOW UNTESTED

void AssignVerticesAndNormal(u_long *addr,
					TMD_NORM* norm0, TMD_VERT* vert0, 
						TMD_VERT* vert1, TMD_VERT* vert2, TMD_VERT* vert3)
{
	addr = (u_long *)((TMD_HDR *)addr + 1);

	addr = (u_long *)((TMD_OBJ *)addr + 1);

	addr = (u_long *)((PRIM_HDR *)addr + 1);

	addr = (u_long *)((TMD_F_4T *)addr + 1);
	setVector((TMD_VERT *)addr, vert0->vx, vert0->vy, vert0->vz);

	addr = (u_long *)((TMD_VERT *)addr + 1);
	setVector((TMD_VERT *)addr, vert1->vx, vert1->vy, vert1->vz);

	addr = (u_long *)((TMD_VERT *)addr + 1);
	setVector((TMD_VERT *)addr, vert2->vx, vert2->vy, vert2->vz);

	addr = (u_long *)((TMD_VERT *)addr + 1);
	setVector((TMD_VERT *)addr, vert3->vx, vert3->vy, vert3->vz);

	addr = (u_long *)((TMD_VERT *)addr + 1);

	setNormal((TMD_NORM *)addr, norm0->nx, norm0->ny, norm0->nz);
}




void AssignVerticesNormalsAndUVs(u_long *addr, u_char u0, u_char v0,
					u_char u1, u_char v1, 
					u_char u2, u_char v2, 
					u_char u3, u_char v3,
					TMD_NORM* norm0, 
					TMD_VERT* vert0, 
					TMD_VERT* vert1,
					TMD_VERT* vert2, 
					TMD_VERT* vert3)
{
	addr = (u_long *)((TMD_HDR *)addr + 1);

	addr = (u_long *)((TMD_OBJ *)addr + 1);

	addr = (u_long *)((PRIM_HDR *)addr + 1);

	((TMD_F_4T *)addr)->u0 = u0;     ((TMD_F_4T *)addr)->v0 = v0;
	((TMD_F_4T *)addr)->u1 = u1;     ((TMD_F_4T *)addr)->v1 = v1;
	((TMD_F_4T *)addr)->u2 = u2;     ((TMD_F_4T *)addr)->v2 = v2;
	((TMD_F_4T *)addr)->u3 = u3;     ((TMD_F_4T *)addr)->v3 = v3;

	addr = (u_long *)((TMD_F_4T *)addr + 1);
	setVector((TMD_VERT *)addr, vert0->vx, vert0->vy, vert0->vz);

	addr = (u_long *)((TMD_VERT *)addr + 1);
	setVector((TMD_VERT *)addr, vert1->vx, vert1->vy, vert1->vz);

	addr = (u_long *)((TMD_VERT *)addr + 1);
	setVector((TMD_VERT *)addr, vert2->vx, vert2->vy, vert2->vz);

	addr = (u_long *)((TMD_VERT *)addr + 1);
	setVector((TMD_VERT *)addr, vert3->vx, vert3->vy, vert3->vz);

	addr = (u_long *)((TMD_VERT *)addr + 1);

	setNormal((TMD_NORM *)addr, norm0->nx, norm0->ny, norm0->nz);
}

