/***************************************************************
*                                                              *
* Copyright (C) 1997 by Sony Computer Entertainment            *
*                       All rights Reserved                    *
*                                                              *
* S. Ashley 9th Jan 97                                         *
*                                                              *
***************************************************************/

#include "blowup.h"

// ************************************************************************
// this bit blows up the object, tmdAddr is the address of the tmd to blow up
// objectNumber is the object in the tmd the speed is the speed, 1 fastest,
// 13 does nothing, count is a counter used to setup the colours of the polygons
// as count increases the polygons turn white, yellow, red and black this 
// happens between 0 and 314, not stepping from 0-314 has various outcomes.
// this has the limitation of one texture to an object, any other textures
// will remain the original colour
void blowVertices(GsDOBJ2 obj, u_char speed, int count)
{
	TMD_OBJ	*object;
	PRIM_HDR *prim, *next;
	TMD_VERT *vert;
	TMD_NORM *norm;
	u_short vertex[4];
	u_short normal[4];
	CVECTOR col[4];
	int i,j,k;
	short avx,avy,avz;
	long snx,sny,snz;
	u_short sd;
	u_short clutX, clutY;
	u_short cba, tsb;
	short clutMode = -1;
	u_short clut[256];
	RECT rect;
	u_char nvert, nnorm, ncol;
	u_char doneTex = 0;

	if (speed < 1) 	   // limit values for speed
		speed = 1;
	else if (speed > 13) 
		speed = 13;

	object = (TMD_OBJ *)obj.tmd;

	vert = (TMD_VERT *)object->vert_top;
	norm = (TMD_NORM *)object->norm_top;
	prim = (PRIM_HDR *)object->prim_top;

	for (j = 0; j < object->n_prim; j++)
		{	
		nvert = getTMDVertices(prim,vertex,&next);
		if(!nvert)	  // if it hasn't any vertices its not a TMD primitive
			{
			printf("\nUnsupported TMD type\n\n");
			exit(1);
			}

		nnorm = getTMDNormals(prim,normal,&next);

		// sum normals
		snx = 0; sny = 0; snz = 0;

		for (i=0;i<nnorm;i++)
			{
			snx += norm[normal[i]].nx;
			sny += norm[normal[i]].ny;
			snz += norm[normal[i]].nz;
			}

		// reduce size of normal ready to add to vertices
		sd = (1 << speed) * (j % ((speed >> 1) + 1) + 1);
		avx = snx / (sd * nnorm);	 // nnorm bit does average
		avy = sny / (sd * nnorm);
		avz = snz / (sd * nnorm);

		for(i = 0; i < nvert; i++)
			{
			vert[vertex[i]].vx += avx;
			vert[vertex[i]].vy += avy;
			vert[vertex[i]].vz += avz;
			}

		// alter colours
		ncol = getTMDColors(prim,col,&next);
		if (ncol)
			{
			for (i = 0; i < ncol; i++)
				explosionColor(count,&col[i]);
			setTMDColors(prim,col,&next);
			}
		else if (!doneTex && getTMDTextureInfo(prim,&cba,&tsb,&next))
			{
			clutMode = (tsb  & 0x180) >> 7;
			clutX = (cba & 63) << 4;
			clutY = cba >> 6;

			if (clutMode == 0)
				{
				rect.x = clutX;
				rect.y = clutY;
				rect.w = 16;
				rect.h = 1;
				StoreImage(&rect,(u_long *)clut);
				DrawSync(0);
				for(i = 0; i< 16; i++)
					explosionClut(count,&clut[i]);
				LoadImage(&rect,(u_long *)clut);
				DrawSync(0);
				}
			else if (clutMode == 1)
				{
				rect.x = clutX;
				rect.y = clutY;
				rect.w = 256;
				rect.h = 1;
				StoreImage(&rect,(u_long *)clut);
				DrawSync(0);
				for(i = 0; i< 256; i++)
					explosionClut(count,&clut[i]);
				LoadImage(&rect,(u_long *)clut);
				DrawSync(0);
				}
			doneTex = 1;
			}

		prim = next;  // move to next primitive
		}
}


// ***************************************************************************
// set the r g b values dependent on c, fade to white, yellow then red and black
void explosionColor(int c, CVECTOR *col)
{
	if (c < 25)
		{
		if (col->r < 230)
			col->r += 25;
		if (col->g < 230)
			col->g += 25;
		if (col->b < 230)
			col->b += 25;
		}
	else if (c < 60)
		{
		if (col->b > 9)
			col->b -= 10;
		if (col->g > 2)
			col->g -= 3;
		}
	else
		{
		if (col->r > 0)
			col->r -= 1;
		if (col->g > 1)
			col->g -= 2;
		}

}


// ***************************************************************************
// set the r g b values of a clut value dependent on c, fade to white, yellow 
// then red and black
void explosionClut(int c, u_short *clut)
{
	if (c < 25)
		{
		if ((*clut & 0x1f) < 29)
			*clut += 3;
		if ((*clut & 0x3e0) < 928)
			*clut += 96;
		if ((*clut & 0x7c00) < 29696)
			*clut += 3072;
		}
	else if (c < 60)
		{
		if ((*clut & 0x7c00) > 0)
			*clut -= 1024;
		}
	else if (c < 95)
		{
		if ((*clut & 0x3e0) > 0)
			*clut -= 32;
		}
	else
		{
		if ((*clut & 0x1f) > 0)
			*clut -= 1;
		}

}


// **************************************************************************
// take a tmd and create a copy that has separate vertices for each primitive
// because most tmds have shared vertices this routine is needed to create 
// primitives within the tmd that can be moved independently.
// also as it copies the TMD, the copy can be destroyed, and then the
// vertices etc can be reset from the original
void separateVertices(u_long *tmdAddrIn, u_long *tmdAddrOut)
{
	TMD_HDR *header;
	TMD_OBJ	*object;
	TMD_OBJ *newObj;
	TMD_OBJ *tempObj;
	PRIM_HDR *prim, *next;
	PRIM_HDR *newPrim,*newNext;
	TMD_VERT *vert;
	TMD_NORM *norm;
	u_long nv;
	u_long nn;
	u_long *p;
	u_short vertex[4];
	u_short vtex[4];
	u_short normal[4];
	int i,j,k;
	u_char nvert, nnorm;
	
	header = (TMD_HDR *)tmdAddrIn;

	// copy header
	*(TMD_HDR *)tmdAddrOut = *header;
	tmdAddrOut = (u_long *)((TMD_HDR *)tmdAddrOut + 1);

	object = (TMD_OBJ *)((TMD_HDR *)tmdAddrIn + 1);	 // 1st object table

	newObj = (TMD_OBJ *)tmdAddrOut;	 // setup pointer to object table copy

	// copy all object tables
	for(k = 0; k < header->nobjs; k++)
		{
		*(TMD_OBJ *)tmdAddrOut = *object;
		tmdAddrOut = (u_long *)((TMD_OBJ *)tmdAddrOut + 1);
		object++;
		}

	object = (TMD_OBJ *)((TMD_HDR *)tmdAddrIn + 1);	 // reset to 1st object table

	// copy primitive data for all objects
	tempObj = newObj;
	for(k = 0; k < header->nobjs; k++)
		{
		prim = (PRIM_HDR *)object->prim_top;
		newPrim = (PRIM_HDR *)tmdAddrOut;

		// update object table, primitive top, number remains the same
		tempObj->prim_top = tmdAddrOut;

		// copy primitive data
		for (j = 0; j < object->n_prim; j++)
			{
			if (!copyTMDPrim(prim,newPrim,&next,&newNext))
				{
				printf("\nUnsupported TMD type\n\n");
				exit(1);
				}

			prim = next;	   // move to next primitive
			newPrim = newNext;
			}
		tmdAddrOut = (u_long *)newPrim;  // move out pointer to end of primitive data

		object++;
		tempObj++;
		}

	object = (TMD_OBJ *)((TMD_HDR *)tmdAddrIn + 1);	 // reset to 1st object table

	// copy vertices data for all objects
	tempObj = newObj;
	for(k = 0; k < header->nobjs; k++)
		{
		nv = 0;

		vert = (TMD_VERT *)object->vert_top;
		prim = (PRIM_HDR *)object->prim_top;

		tempObj->vert_top = tmdAddrOut;	 // update object table, vertices pointer

		newPrim = (PRIM_HDR *)tempObj->prim_top;

		for (j = 0; j < object->n_prim; j++)  // copy vertices data, no shared vertices
			{
			nvert = getTMDVertices(prim,vertex,&next);  // get vertices index
			if (!nvert)
				{
				printf("\nUnsupported TMD type\n\n");
				exit(1);
				}

			for (i = 0; i < nvert; i++)    // copy vertices data
				{
				*(TMD_VERT *)tmdAddrOut = vert[vertex[i]];
				tmdAddrOut = (u_long *)((TMD_VERT *)tmdAddrOut + 1);

				vertex[i] = nv++;
				}

			if (!setTMDVertices(newPrim,vertex,&newNext))   // set new vertices index
				{
				printf("\nUnsupported TMD type\n\n");
				exit(1);
				}


			prim = next;       // next primitive
			newPrim = newNext;
			}

		tempObj->n_vert = nv;  // update object table, number of vertices
			
		object++;
		tempObj++;
		}

	object = (TMD_OBJ *)((TMD_HDR *)tmdAddrIn + 1);	  // reset to 1st object table

	// copy all normals data for all objects
	tempObj = newObj;
	for(k = 0; k < header->nobjs; k++)
		{
		nn = 0;

		norm = (TMD_NORM *)object->norm_top;
		prim = (PRIM_HDR *)object->prim_top;

		tempObj->norm_top = tmdAddrOut;	 // update object table, normals pointer

		newPrim = (PRIM_HDR *)tempObj->prim_top;

		for (j = 0; j < object->n_prim; j++)  // copy normal data, no shared normals
			{
			nnorm = getTMDNormals(prim,normal,&next);  // get normals index
			if (!nnorm)
				{
				printf("\nUnsupported TMD type\n\n");
				exit(1);
				}

			for (i = 0; i < nnorm; i++)	  // copy normal data
				{
				*(TMD_NORM *)tmdAddrOut = norm[normal[i]];
				tmdAddrOut = (u_long *)((TMD_NORM *)tmdAddrOut + 1);

				normal[i] = nn++;
				}
		
			if (!setTMDNormals(newPrim,normal,&newNext))  // set new normals index
				{
				printf("\nUnsupported TMD type\n\n");
				exit(1);
				}

			prim = next;	  // next primitive
			newPrim = newNext;
			}

		tempObj->n_norm = nn; // update object table, number of normals

		object++;
		tempObj++;
		}
}


//*************************************************************************
// reset the vertices of the 'out' tmd with those from the 'in' tmd
// but only for the given object.
// if the 'in' tmd is not the one that set the 'out' tmd to start
// then there will be problems
void resetVertices(u_long *tmdAddrIn, u_long *tmdAddrOut, u_long objectNumber)
{
	TMD_OBJ	*object;
	TMD_OBJ *newObj;
	PRIM_HDR *prim, *next;
	TMD_HDR *header;
	TMD_VERT *vert;
	TMD_NORM *norm;
	u_short vertex[4];
	int i,j,k;
	u_char nvert;

	header = (TMD_HDR *)tmdAddrIn;
	
	object = (TMD_OBJ *)((TMD_HDR *)tmdAddrIn + 1);	 // 1st object table

	newObj = (TMD_OBJ *)((TMD_HDR *)tmdAddrOut + 1);  // setup pointer to object table copy

	for(k = 0; k < header->nobjs && k < objectNumber; k++)
		{
		object++;
		newObj++;
		}

	vert = (TMD_VERT *)object->vert_top;
	prim = (PRIM_HDR *)object->prim_top;

	tmdAddrOut = newObj->vert_top;	// move to vertices

	for (j = 0; j < object->n_prim; j++)  // copy vertices data for each primitive
		{
		nvert = getTMDVertices(prim,vertex,&next);  // get vertices index
		if (!nvert)
			{
			printf("\nUnsupported TMD type\n\n");
			exit(1);
			}

		for (i = 0; i < nvert; i++)    // copy vertices data
			{
			*(TMD_VERT *)tmdAddrOut = vert[vertex[i]];
			tmdAddrOut = (u_long *)((TMD_VERT *)tmdAddrOut + 1);
			}

		prim = next;       // next primitive
		}
}


//***************************************************************************
// recopy the colour data for a single object in a tmd
void resetColors(u_long *tmdAddrIn, u_long *tmdAddrOut, u_long objectNumber)
{
	TMD_HDR *header;
	TMD_OBJ	*object, *newObj;
	PRIM_HDR *prim, *next;
	PRIM_HDR *newPrim, *newNext;
	CVECTOR col[4];
	int i;
	
	header = (TMD_HDR *)tmdAddrIn;

	object = (TMD_OBJ *)((TMD_HDR *)tmdAddrIn + 1);	 // 1st object table
	newObj = (TMD_OBJ *)((TMD_HDR *)tmdAddrOut + 1);  // pointer to copy object table

	for(i = 0; i < header->nobjs && i < objectNumber; i++)
		{
		newObj++;
		object++;
		}

	prim = (PRIM_HDR *)object->prim_top;	// set pointers to primitive data
	newPrim = (PRIM_HDR *)newObj->prim_top;

	// copy original primitive data
	for (i = 0; i < object->n_prim; i++)
		{
		getTMDColors(prim,col,&next);
		setTMDColors(newPrim,col,&newNext);

		prim = next;		// move to next primitive
		newPrim = newNext;
		}
}