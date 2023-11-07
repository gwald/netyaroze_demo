/***************************************************************
*                                                              *
* Copyright (C) 1997 by Sony Computer Entertainment            *
*                       All rights Reserved                    *
*                                                              *
* S. Ashley 9th Jan 97                                         *
*                                                              *
***************************************************************/

#include "tmdutil.h"

// ************************************************************************
// this gives the current limits of the tmd
// if the local screen matrix has been calculated, then the limits are in the
// world coordinate system, otherwise the local coordinates are used
void TMDLimits(GsDOBJ2 obj,VECTOR *min,VECTOR *max)
	{
	TMD_OBJ	*object;
	PRIM_HDR *prim, *next;
	TMD_VERT *vert;
	u_short vertex[4];
	int i,j;
	u_char nvert;
	u_char first = 1;
	SVECTOR v1;
	VECTOR v2;

	object = (TMD_OBJ *)obj.tmd;

	vert = (TMD_VERT *)object->vert_top;
	prim = (PRIM_HDR *)object->prim_top;

	for (j = 0; j < object->n_prim; j++)
		{	
		nvert = getTMDVertices(prim,vertex,&next);
		prim = next;

		if (first && nvert > 0)		 // initial values
			{
			v1.vx = vert[vertex[0]].vx;	
			v1.vy = vert[vertex[0]].vy;	
			v1.vz = vert[vertex[0]].vz;

			if (obj.coord2->flg)
				ApplyMatrix(&(obj.coord2->workm),&v1,&v2);
			else
				{
				v2.vx = v1.vx; v2.vy = v1.vy; v2.vz = v1.vz;
				}

			*min = v2;
			*max = v2;

			first = 0;
			}

		for (i = 1; i < nvert; i++)	   // check max and min
			{
			v1.vx = vert[vertex[i]].vx;	
			v1.vy = vert[vertex[i]].vy;	
			v1.vz = vert[vertex[i]].vz;

			if (obj.coord2->flg)
				ApplyMatrix(&(obj.coord2->workm),&v1,&v2);
			else
				{
				v2.vx = v1.vx; v2.vy = v1.vy; v2.vz = v1.vz;
				}

			if (v2.vx < min->vx)
				min->vx = v2.vx;
			else if (v2.vx > max->vx)
				max->vx = v2.vx;

			if (v2.vy < min->vy)
				min->vy = v2.vy;
			else if (v2.vy > max->vy)
				max->vy = v2.vy;

			if (v2.vz < min->vz)
				min->vz = v2.vz;
			else if (v2.vz > max->vz)
				max->vz = v2.vz;
			}
		}
	}


// ************************************************************************
// returns the number of vertices indices acquired
// and puts the index values of the vertices in vertex
// from these index values the vertices area of the TMD can be
// used to obtain the actual vertex values
// prim is the pointer to the primitive and next the pointer to the next
// in sequence
u_char getTMDVertices(PRIM_HDR *prim, u_short *vertex, PRIM_HDR *(*next))
{
	u_char nvert = 0;
	u_long *p;

	p = (u_long *)(prim + 1);

	switch(prim->mode)
		{
		case F_3: // also case F_3G: as they have the same mode value
			if (prim->flag & GRD)	 // gradation performed
				{
				vertex[0] = ((TMD_F_3G *)p)->vert0;
				vertex[1] = ((TMD_F_3G *)p)->vert1;
				vertex[2] = ((TMD_F_3G *)p)->vert2;
				nvert = 3;

				*next = (PRIM_HDR *)((TMD_F_3G *)p + 1);
				}
			else					 // no gradation
				{
				vertex[0] = ((TMD_F_3 *)p)->vert0;
				vertex[1] = ((TMD_F_3 *)p)->vert1;
				vertex[2] = ((TMD_F_3 *)p)->vert2;
				nvert = 3;

				*next = (PRIM_HDR *)((TMD_F_3 *)p + 1);
				}
			break;
		case F_3T:
			vertex[0] = ((TMD_F_3T *)p)->vert0;
			vertex[1] = ((TMD_F_3T *)p)->vert1;
			vertex[2] = ((TMD_F_3T *)p)->vert2;
			nvert = 3;

			*next = (PRIM_HDR *)((TMD_F_3T *)p + 1);
			break;
		case F_4: // also case F_4G:
			if (prim->flag & GRD)
				{
				vertex[0] = ((TMD_F_4G *)p)->vert0;
				vertex[1] = ((TMD_F_4G *)p)->vert1;
				vertex[2] = ((TMD_F_4G *)p)->vert2;
				vertex[3] = ((TMD_F_4G *)p)->vert3;
				nvert = 4;

				*next = (PRIM_HDR *)((TMD_F_4G *)p + 1);
				}
			else
				{
				vertex[0] = ((TMD_F_4 *)p)->vert0;
				vertex[1] = ((TMD_F_4 *)p)->vert1;
				vertex[2] = ((TMD_F_4 *)p)->vert2;
				vertex[3] = ((TMD_F_4 *)p)->vert3;
				nvert = 4;

				*next = (PRIM_HDR *)((TMD_F_4 *)p + 1);
				}
			break;
		case F_4T:
			vertex[0] = ((TMD_F_4T *)p)->vert0;
			vertex[1] = ((TMD_F_4T *)p)->vert1;
			vertex[2] = ((TMD_F_4T *)p)->vert2;
			vertex[3] = ((TMD_F_4T *)p)->vert3;
			nvert = 4;

			*next = (PRIM_HDR *)((TMD_F_4T *)p + 1);
			break;
		case G_3: // also case G_3G:
			if (prim->flag & GRD)
				{
				vertex[0] = ((TMD_G_3G *)p)->vert0;
				vertex[1] = ((TMD_G_3G *)p)->vert1;
				vertex[2] = ((TMD_G_3G *)p)->vert2;
				nvert = 3;

				*next = (PRIM_HDR *)((TMD_G_3G *)p + 1);
				}
			else
				{
				vertex[0] = ((TMD_G_3 *)p)->vert0;
				vertex[1] = ((TMD_G_3 *)p)->vert1;
				vertex[2] = ((TMD_G_3 *)p)->vert2;
				nvert = 3;

				*next = (PRIM_HDR *)((TMD_G_3 *)p + 1);
				}
			break;
		case G_3T:
			vertex[0] = ((TMD_G_3T *)p)->vert0;
			vertex[1] = ((TMD_G_3T *)p)->vert1;
			vertex[2] = ((TMD_G_3T *)p)->vert2;
			nvert = 3;

			*next = (PRIM_HDR *)((TMD_G_3T *)p + 1);
			break;
		case G_4: // also case G_4G:
			if (prim->flag & GRD)
				{
				vertex[0] = ((TMD_G_4G *)p)->vert0;
				vertex[1] = ((TMD_G_4G *)p)->vert1;
				vertex[2] = ((TMD_G_4G *)p)->vert2;
				vertex[3] = ((TMD_G_4G *)p)->vert3;
				nvert = 4;

				*next = (PRIM_HDR *)((TMD_G_4G *)p + 1);
				}
			else
				{
				vertex[0] = ((TMD_G_4 *)p)->vert0;
				vertex[1] = ((TMD_G_4 *)p)->vert1;
				vertex[2] = ((TMD_G_4 *)p)->vert2;
				vertex[3] = ((TMD_G_4 *)p)->vert3;
				nvert = 4;

				*next = (PRIM_HDR *)((TMD_G_4 *)p + 1);
				}
			break;
		case G_4T:
			vertex[0] = ((TMD_G_4T *)p)->vert0;
			vertex[1] = ((TMD_G_4T *)p)->vert1;
			vertex[2] = ((TMD_G_4T *)p)->vert2;
			vertex[3] = ((TMD_G_4T *)p)->vert3;
			nvert = 4;

			*next = (PRIM_HDR *)((TMD_G_4T *)p + 1);
			break;
		case F_3_NL:
			vertex[0] = ((TMD_F_3_NL *)p)->vert0;
			vertex[1] = ((TMD_F_3_NL *)p)->vert1;
			vertex[2] = ((TMD_F_3_NL *)p)->vert2;
			nvert = 3;

			*next = (PRIM_HDR *)((TMD_F_3_NL *)p + 1);
			break;
		case F_3T_NL:
			vertex[0] = ((TMD_F_3T_NL *)p)->vert0;
			vertex[1] = ((TMD_F_3T_NL *)p)->vert1;
			vertex[2] = ((TMD_F_3T_NL *)p)->vert2;
			nvert = 3;

			*next = (PRIM_HDR *)((TMD_F_3T_NL *)p + 1);
			break;
		case F_4_NL:
			vertex[0] = ((TMD_F_4_NL *)p)->vert0;
			vertex[1] = ((TMD_F_4_NL *)p)->vert1;
			vertex[2] = ((TMD_F_4_NL *)p)->vert2;
			vertex[3] = ((TMD_F_4_NL *)p)->vert3;
			nvert = 4;

			*next = (PRIM_HDR *)((TMD_F_4_NL *)p + 1);
			break;
		case F_4T_NL:
			vertex[0] = ((TMD_F_4T_NL *)p)->vert0;
			vertex[1] = ((TMD_F_4T_NL *)p)->vert1;
			vertex[2] = ((TMD_F_4T_NL *)p)->vert2;
			vertex[3] = ((TMD_F_4T_NL *)p)->vert3;
			nvert = 4;

			*next = (PRIM_HDR *)((TMD_F_4T_NL *)p + 1);
			break;
		case G_3_NL: 
			vertex[0] = ((TMD_G_3_NL *)p)->vert0;
			vertex[1] = ((TMD_G_3_NL *)p)->vert1;
			vertex[2] = ((TMD_G_3_NL *)p)->vert2;
			nvert = 3;

			*next = (PRIM_HDR *)((TMD_G_3_NL *)p + 1);
			break;
		case G_3T_NL:
			vertex[0] = ((TMD_G_3T_NL *)p)->vert0;
			vertex[1] = ((TMD_G_3T_NL *)p)->vert1;
			vertex[2] = ((TMD_G_3T_NL *)p)->vert2;
			nvert = 3;

			*next = (PRIM_HDR *)((TMD_G_3T_NL *)p + 1);
			break;
		case G_4_NL: 
			vertex[0] = ((TMD_G_4_NL *)p)->vert0;
			vertex[1] = ((TMD_G_4_NL *)p)->vert1;
			vertex[2] = ((TMD_G_4_NL *)p)->vert2;
			vertex[3] = ((TMD_G_4_NL *)p)->vert3;
			nvert = 4;

			*next = (PRIM_HDR *)((TMD_G_4_NL *)p + 1);
			break;
		case G_4T_NL:
			vertex[0] = ((TMD_G_4T_NL *)p)->vert0;
			vertex[1] = ((TMD_G_4T_NL *)p)->vert1;
			vertex[2] = ((TMD_G_4T_NL *)p)->vert2;
			vertex[3] = ((TMD_G_4T_NL *)p)->vert3;
			nvert = 4;

			*next = (PRIM_HDR *)((TMD_G_4T_NL *)p + 1);
			break;
		case SL:
			vertex[0] = ((TMD_SL *)p)->vert0;
			vertex[1] = ((TMD_SL *)p)->vert1;
			nvert = 2;

			*next = (PRIM_HDR *)((TMD_SL *)p + 1);
			break;
		case SL_G:
			vertex[0] = ((TMD_SL_G *)p)->vert0;
			vertex[1] = ((TMD_SL_G *)p)->vert1;
			nvert = 2;

			*next = (PRIM_HDR *)((TMD_SL_G *)p + 1);
			break;
		case SP_3D:
			vertex[0] = ((TMD_SP_3D *)p)->vert0;
			nvert = 1;

			*next = (PRIM_HDR *)((TMD_SP_3D *)p + 1);
			break;
		case SP_3D_1:
			vertex[0] = ((TMD_SP_3D_1 *)p)->vert0;
			nvert = 1;

			*next = (PRIM_HDR *)((TMD_SP_3D_1 *)p + 1);
			break;
		case SP_3D_8:
			vertex[0] = ((TMD_SP_3D_8 *)p)->vert0;
			nvert = 1;

			*next = (PRIM_HDR *)((TMD_SP_3D_8 *)p + 1);
			break;
		case SP_3D_16:
			vertex[0] = ((TMD_SP_3D_16 *)p)->vert0;
			nvert = 1;

			*next = (PRIM_HDR *)((TMD_SP_3D_16 *)p + 1);
			break;
		}
	return nvert;
}


// ************************************************************************
// use of this means knowing the primitive type, or at least the number of
// vertices otherwise incorrectly setting the vertices is likely.
u_char setTMDVertices(PRIM_HDR *prim, u_short *vertex, PRIM_HDR *(*next))
{
	u_char nvert = 0;
	u_long *p;

	p = (u_long *)(prim + 1);

	switch(prim->mode)
		{
		case F_3: // also case F_3G: as they have the same mode value
			if (prim->flag & GRD)	 // gradation performed
				{
				((TMD_F_3G *)p)->vert0 = vertex[0];
				((TMD_F_3G *)p)->vert1 = vertex[1];
				((TMD_F_3G *)p)->vert2 = vertex[2];
				nvert = 3;

				*next = (PRIM_HDR *)((TMD_F_3G *)p + 1);
				}
			else					 // no gradation
				{
				((TMD_F_3 *)p)->vert0 = vertex[0];
				((TMD_F_3 *)p)->vert1 = vertex[1];
				((TMD_F_3 *)p)->vert2 = vertex[2];
				nvert = 3;

				*next = (PRIM_HDR *)((TMD_F_3 *)p + 1);
				}
			break;
		case F_3T:
			((TMD_F_3T *)p)->vert0 = vertex[0];
			((TMD_F_3T *)p)->vert1 = vertex[1];
			((TMD_F_3T *)p)->vert2 = vertex[2];
			nvert = 3;

			*next = (PRIM_HDR *)((TMD_F_3T *)p + 1);
			break;
		case F_4: // also case F_4G:
			if (prim->flag & GRD)
				{
				((TMD_F_4G *)p)->vert0 = vertex[0];
				((TMD_F_4G *)p)->vert1 = vertex[1];
				((TMD_F_4G *)p)->vert2 = vertex[2];
				((TMD_F_4G *)p)->vert3 = vertex[3];
				nvert = 4;

				*next = (PRIM_HDR *)((TMD_F_4G *)p + 1);
				}
			else
				{
				((TMD_F_4 *)p)->vert0 = vertex[0];
				((TMD_F_4 *)p)->vert1 = vertex[1];
				((TMD_F_4 *)p)->vert2 = vertex[2];
				((TMD_F_4 *)p)->vert3 = vertex[3];
				nvert = 4;

				*next = (PRIM_HDR *)((TMD_F_4 *)p + 1);
				}
			break;
		case F_4T:
			((TMD_F_4T *)p)->vert0 = vertex[0];
			((TMD_F_4T *)p)->vert1 = vertex[1];
			((TMD_F_4T *)p)->vert2 = vertex[2];
			((TMD_F_4T *)p)->vert3 = vertex[3];
			nvert = 4;

			*next = (PRIM_HDR *)((TMD_F_4T *)p + 1);
			break;
		case G_3: // also case G_3G:
			if (prim->flag & GRD)
				{
				((TMD_G_3G *)p)->vert0 = vertex[0];
				((TMD_G_3G *)p)->vert1 = vertex[1];
				((TMD_G_3G *)p)->vert2 = vertex[2];
				nvert = 3;

				*next = (PRIM_HDR *)((TMD_G_3G *)p + 1);
				}
			else
				{
				((TMD_G_3 *)p)->vert0 = vertex[0];
				((TMD_G_3 *)p)->vert1 = vertex[1];
				((TMD_G_3 *)p)->vert2 = vertex[2];
				nvert = 3;

				*next = (PRIM_HDR *)((TMD_G_3 *)p + 1);
				}
			break;
		case G_3T:
			((TMD_G_3T *)p)->vert0 = vertex[0];
			((TMD_G_3T *)p)->vert1 = vertex[1];
			((TMD_G_3T *)p)->vert2 = vertex[2];
			nvert = 3;

			*next = (PRIM_HDR *)((TMD_G_3T *)p + 1);
			break;
		case G_4: // also case G_4G:
			if (prim->flag & GRD)
				{
				((TMD_G_4G *)p)->vert0 = vertex[0];
				((TMD_G_4G *)p)->vert1 = vertex[1];
				((TMD_G_4G *)p)->vert2 = vertex[2];
				((TMD_G_4G *)p)->vert3 = vertex[3];
				nvert = 4;

				*next = (PRIM_HDR *)((TMD_G_4G *)p + 1);
				}
			else
				{
				((TMD_G_4 *)p)->vert0 = vertex[0];
				((TMD_G_4 *)p)->vert1 = vertex[1];
				((TMD_G_4 *)p)->vert2 = vertex[2];
				((TMD_G_4 *)p)->vert3 = vertex[3];
				nvert = 4;

				*next = (PRIM_HDR *)((TMD_G_4 *)p + 1);
				}
			break;
		case G_4T:
			((TMD_G_4T *)p)->vert0 = vertex[0];
			((TMD_G_4T *)p)->vert1 = vertex[1];
			((TMD_G_4T *)p)->vert2 = vertex[2];
			((TMD_G_4T *)p)->vert3 = vertex[3];
			nvert = 4;

			*next = (PRIM_HDR *)((TMD_G_4T *)p + 1);
			break;
		case F_3_NL:
			((TMD_F_3_NL *)p)->vert0 = vertex[0];
			((TMD_F_3_NL *)p)->vert1 = vertex[1];
			((TMD_F_3_NL *)p)->vert2 = vertex[2];
			nvert = 3;

			*next = (PRIM_HDR *)((TMD_F_3_NL *)p + 1);
			break;
		case F_3T_NL:
			((TMD_F_3T_NL *)p)->vert0 = vertex[0];
			((TMD_F_3T_NL *)p)->vert1 = vertex[1];
			((TMD_F_3T_NL *)p)->vert2 = vertex[2];
			nvert = 3;

			*next = (PRIM_HDR *)((TMD_F_3T_NL *)p + 1);
			break;
		case F_4_NL:
			((TMD_F_4_NL *)p)->vert0 = vertex[0];
			((TMD_F_4_NL *)p)->vert1 = vertex[1];
			((TMD_F_4_NL *)p)->vert2 = vertex[2];
			((TMD_F_4_NL *)p)->vert3 = vertex[3];
			nvert = 4;

			*next = (PRIM_HDR *)((TMD_F_4_NL *)p + 1);
			break;
		case F_4T_NL:
			((TMD_F_4T_NL *)p)->vert0 = vertex[0];
			((TMD_F_4T_NL *)p)->vert1 = vertex[1];
			((TMD_F_4T_NL *)p)->vert2 = vertex[2];
			((TMD_F_4T_NL *)p)->vert3 = vertex[3];
			nvert = 4;

			*next = (PRIM_HDR *)((TMD_F_4T_NL *)p + 1);
			break;
		case G_3_NL: 
			((TMD_G_3_NL *)p)->vert0 = vertex[0];
			((TMD_G_3_NL *)p)->vert1 = vertex[1];
			((TMD_G_3_NL *)p)->vert2 = vertex[2];
			nvert = 3;

			*next = (PRIM_HDR *)((TMD_G_3_NL *)p + 1);
			break;
		case G_3T_NL:
			((TMD_G_3T_NL *)p)->vert0 = vertex[0];
			((TMD_G_3T_NL *)p)->vert1 = vertex[1];
			((TMD_G_3T_NL *)p)->vert2 = vertex[2];
			nvert = 3;

			*next = (PRIM_HDR *)((TMD_G_3T_NL *)p + 1);
			break;
		case G_4_NL: 
			((TMD_G_4_NL *)p)->vert0 = vertex[0];
			((TMD_G_4_NL *)p)->vert1 = vertex[1];
			((TMD_G_4_NL *)p)->vert2 = vertex[2];
			((TMD_G_4_NL *)p)->vert3 = vertex[3];
			nvert = 4;

			*next = (PRIM_HDR *)((TMD_G_4_NL *)p + 1);
			break;
		case G_4T_NL:
			((TMD_G_4T_NL *)p)->vert0 = vertex[0];
			((TMD_G_4T_NL *)p)->vert1 = vertex[1];
			((TMD_G_4T_NL *)p)->vert2 = vertex[2];
			((TMD_G_4T_NL *)p)->vert3 = vertex[3];
			nvert = 4;

			*next = (PRIM_HDR *)((TMD_G_4T_NL *)p + 1);
			break;
		case SL:
			((TMD_SL *)p)->vert0 = vertex[0];
			((TMD_SL *)p)->vert1 = vertex[1];
			nvert = 2;

			*next = (PRIM_HDR *)((TMD_SL *)p + 1);
			break;
		case SL_G:
			((TMD_SL_G *)p)->vert0 = vertex[0];
			((TMD_SL_G *)p)->vert1 = vertex[1];
			nvert = 2;

			*next = (PRIM_HDR *)((TMD_SL_G *)p + 1);
			break;
		case SP_3D:
			((TMD_SP_3D *)p)->vert0 = vertex[0];
			nvert = 1;

			*next = (PRIM_HDR *)((TMD_SP_3D *)p + 1);
			break;
		case SP_3D_1:
			((TMD_SP_3D_1 *)p)->vert0 = vertex[0];
			nvert = 1;

			*next = (PRIM_HDR *)((TMD_SP_3D_1 *)p + 1);
			break;
		case SP_3D_8:
			((TMD_SP_3D_8 *)p)->vert0 = vertex[0];
			nvert = 1;

			*next = (PRIM_HDR *)((TMD_SP_3D_8 *)p + 1);
			break;
		case SP_3D_16:
			((TMD_SP_3D_16 *)p)->vert0 = vertex[0];
			nvert = 1;

			*next = (PRIM_HDR *)((TMD_SP_3D_16 *)p + 1);
			break;
		}
	return nvert;
}

// ************************************************************************
// just like getTMDVertices except this gets the normals
u_char getTMDNormals(PRIM_HDR *prim, u_short *normal, PRIM_HDR *(*next))
{
	u_char nnorm = 0;
	u_long *p;

	p = (u_long *)(prim + 1);

	switch(prim->mode)
		{
		case F_3: // also case F_3G: as they have the same mode value
			if (prim->flag & GRD)	 // gradation performed
				{
				normal[0] = ((TMD_F_3G *)p)->norm0;
				nnorm = 1;

				*next = (PRIM_HDR *)((TMD_F_3G *)p + 1);
				}
			else					 // no gradation
				{
				normal[0] = ((TMD_F_3 *)p)->norm0;
				nnorm = 1;

				*next = (PRIM_HDR *)((TMD_F_3 *)p + 1);
				}
			break;
		case F_3T:
			normal[0] = ((TMD_F_3T *)p)->norm0;
			nnorm = 1;

			*next = (PRIM_HDR *)((TMD_F_3T *)p + 1);
			break;
		case F_4: // also case F_4G:
			if (prim->flag & GRD)
				{
				normal[0] = ((TMD_F_4G *)p)->norm0;
				nnorm = 1;

				*next = (PRIM_HDR *)((TMD_F_4G *)p + 1);
				}
			else
				{
				normal[0] = ((TMD_F_4 *)p)->norm0;
				nnorm = 1;

				*next = (PRIM_HDR *)((TMD_F_4 *)p + 1);
				}
			break;
		case F_4T:
			normal[0] = ((TMD_F_4T *)p)->norm0;
			nnorm = 1;

			*next = (PRIM_HDR *)((TMD_F_4T *)p + 1);
			break;
		case G_3: // also case G_3G:
			if (prim->flag & GRD)
				{
				normal[0] = ((TMD_G_3G *)p)->norm0;
				normal[1] = ((TMD_G_3G *)p)->norm1;
				normal[2] = ((TMD_G_3G *)p)->norm2;
				nnorm = 3;

				*next = (PRIM_HDR *)((TMD_G_3G *)p + 1);
				}
			else
				{
				normal[0] = ((TMD_G_3 *)p)->norm0;
				normal[1] = ((TMD_G_3 *)p)->norm1;
				normal[2] = ((TMD_G_3 *)p)->norm2;
				nnorm = 3;

				*next = (PRIM_HDR *)((TMD_G_3 *)p + 1);
				}
			break;
		case G_3T:
			normal[0] = ((TMD_G_3T *)p)->norm0;
			normal[1] = ((TMD_G_3T *)p)->norm1;
			normal[2] = ((TMD_G_3T *)p)->norm2;
			nnorm = 3;

			*next = (PRIM_HDR *)((TMD_G_3T *)p + 1);
			break;
		case G_4: // also case G_4G:
			if (prim->flag & GRD)
				{
				normal[0] = ((TMD_G_4G *)p)->norm0;
				normal[1] = ((TMD_G_4G *)p)->norm1;
				normal[2] = ((TMD_G_4G *)p)->norm2;
				normal[3] = ((TMD_G_4G *)p)->norm3;
				nnorm = 4;

				*next = (PRIM_HDR *)((TMD_G_4G *)p + 1);
				}
			else
				{
				normal[0] = ((TMD_G_4 *)p)->norm0;
				normal[1] = ((TMD_G_4 *)p)->norm1;
				normal[2] = ((TMD_G_4 *)p)->norm2;
				normal[3] = ((TMD_G_4 *)p)->norm3;
				nnorm = 4;

				*next = (PRIM_HDR *)((TMD_G_4 *)p + 1);
				}
			break;
		case G_4T:
			normal[0] = ((TMD_G_4T *)p)->norm0;
			normal[1] = ((TMD_G_4T *)p)->norm1;
			normal[2] = ((TMD_G_4T *)p)->norm2;
			normal[3] = ((TMD_G_4T *)p)->norm3;
			nnorm = 4;

			*next = (PRIM_HDR *)((TMD_G_4T *)p + 1);
			break;
		}
	return nnorm;
}		   


// ************************************************************************
// as with setTMDVerticies knowledge of the primitive type is necessary
u_char setTMDNormals(PRIM_HDR *prim, u_short *normal, PRIM_HDR *(*next))
{
	u_char nnorm = 0;
	u_long *p;

	p = (u_long *)(prim + 1);

	switch(prim->mode)
		{
		case F_3: // also case F_3G: as they have the same mode value
			if (prim->flag & GRD)	 // gradation performed
				{
				((TMD_F_3G *)p)->norm0 = normal[0];
				nnorm = 1;

				*next = (PRIM_HDR *)((TMD_F_3G *)p + 1);
				}
			else					 // no gradation
				{
				((TMD_F_3 *)p)->norm0 = normal[0];
				nnorm = 1;

				*next = (PRIM_HDR *)((TMD_F_3 *)p + 1);
				}
			break;
		case F_3T:
			((TMD_F_3T *)p)->norm0 = normal[0];
			nnorm = 1;

			*next = (PRIM_HDR *)((TMD_F_3T *)p + 1);
			break;
		case F_4: // also case F_4G:
			if (prim->flag & GRD)
				{
				((TMD_F_4G *)p)->norm0 = normal[0];
				nnorm = 1;

				*next = (PRIM_HDR *)((TMD_F_4G *)p + 1);
				}
			else
				{
				((TMD_F_4 *)p)->norm0 = normal[0];
				nnorm = 1;

				*next = (PRIM_HDR *)((TMD_F_4 *)p + 1);
				}
			break;
		case F_4T:
			((TMD_F_4T *)p)->norm0 = normal[0];
			nnorm = 1;

			*next = (PRIM_HDR *)((TMD_F_4T *)p + 1);
			break;
		case G_3: // also case G_3G:
			if (prim->flag & GRD)
				{
				((TMD_G_3G *)p)->norm0 = normal[0];
				((TMD_G_3G *)p)->norm1 = normal[1];
				((TMD_G_3G *)p)->norm2 = normal[2];
				nnorm = 3;

				*next = (PRIM_HDR *)((TMD_G_3G *)p + 1);
				}
			else
				{
				((TMD_G_3 *)p)->norm0 = normal[0];
				((TMD_G_3 *)p)->norm1 = normal[1];
				((TMD_G_3 *)p)->norm2 = normal[2];
				nnorm = 3;

				*next = (PRIM_HDR *)((TMD_G_3 *)p + 1);
				}
			break;
		case G_3T:
			((TMD_G_3T *)p)->norm0 = normal[0];
			((TMD_G_3T *)p)->norm1 = normal[1];
			((TMD_G_3T *)p)->norm2 = normal[2];
			nnorm = 3;

			*next = (PRIM_HDR *)((TMD_G_3T *)p + 1);
			break;
		case G_4: // also case G_4G:
			if (prim->flag & GRD)
				{
				((TMD_G_4G *)p)->norm0 = normal[0];
				((TMD_G_4G *)p)->norm1 = normal[1];
				((TMD_G_4G *)p)->norm2 = normal[2];
				((TMD_G_4G *)p)->norm3 = normal[3];
				nnorm = 4;

				*next = (PRIM_HDR *)((TMD_G_4G *)p + 1);
				}
			else
				{
				((TMD_G_4 *)p)->norm0 = normal[0];
				((TMD_G_4 *)p)->norm1 = normal[1];
				((TMD_G_4 *)p)->norm2 = normal[2];
				((TMD_G_4 *)p)->norm3 = normal[3];
				nnorm = 4;

				*next = (PRIM_HDR *)((TMD_G_4 *)p + 1);
				}
			break;
		case G_4T:
			((TMD_G_4T *)p)->norm0 = normal[0];
			((TMD_G_4T *)p)->norm1 = normal[1];
			((TMD_G_4T *)p)->norm2 = normal[2];
			((TMD_G_4T *)p)->norm3 = normal[3];
			nnorm = 4;

			*next = (PRIM_HDR *)((TMD_G_4T *)p + 1);
			break;
		}
	return nnorm;
}


// ************************************************************************
// this gets the colours used in the primitive, unlike the vertices and
// normals this is the actual value not an index
u_char getTMDColors(PRIM_HDR *prim, CVECTOR *col, PRIM_HDR *(*next))
{
	u_char ncol = 0;
	u_long *p;

	p = (u_long *)(prim + 1);

	switch(prim->mode)
		{
		case F_3: // also case F_3G: as they have the same mode value
			if (prim->flag & GRD)	 // gradation performed
				{
				col[0].r = ((TMD_F_3G *)p)->r0;	col[0].g = ((TMD_F_3G *)p)->g0;	col[0].b = ((TMD_F_3G *)p)->b0;
				col[1].r = ((TMD_F_3G *)p)->r1;	col[1].g = ((TMD_F_3G *)p)->g1;	col[1].b = ((TMD_F_3G *)p)->b1;
				col[2].r = ((TMD_F_3G *)p)->r2;	col[2].g = ((TMD_F_3G *)p)->g2;	col[2].b = ((TMD_F_3G *)p)->b2;
				ncol = 3;

				*next = (PRIM_HDR *)((TMD_F_3G *)p + 1);
				}
			else					 // no gradation
				{
				col[0].r = ((TMD_F_3 *)p)->r0; col[0].g = ((TMD_F_3 *)p)->g0; col[0].b = ((TMD_F_3 *)p)->b0;
				ncol = 1;

				*next = (PRIM_HDR *)((TMD_F_3 *)p + 1);
				}
			break;
		case F_4: // also case F_4G:
			if (prim->flag & GRD)
				{
				col[0].r = ((TMD_F_4G *)p)->r0; col[0].g = ((TMD_F_4G *)p)->g0; col[0].b = ((TMD_F_4G *)p)->b0;
				col[1].r = ((TMD_F_4G *)p)->r1; col[1].g = ((TMD_F_4G *)p)->g1; col[1].b = ((TMD_F_4G *)p)->b1;
				col[2].r = ((TMD_F_4G *)p)->r2; col[2].g = ((TMD_F_4G *)p)->g2; col[2].b = ((TMD_F_4G *)p)->b2;
				col[3].r = ((TMD_F_4G *)p)->r3; col[3].g = ((TMD_F_4G *)p)->g3; col[3].b = ((TMD_F_4G *)p)->b3;
				ncol = 4;

				*next = (PRIM_HDR *)((TMD_F_4G *)p + 1);
				}
			else
				{
				col[0].r = ((TMD_F_4 *)p)->r0; col[0].g = ((TMD_F_4 *)p)->g0; col[0].b = ((TMD_F_4 *)p)->b0;
				ncol = 1;

				*next = (PRIM_HDR *)((TMD_F_4 *)p + 1);
				}
			break;
		case G_3: // also case G_3G:
			if (prim->flag & GRD)
				{
				col[0].r = ((TMD_G_3G *)p)->r0; col[0].g = ((TMD_G_3G *)p)->g0; col[0].b = ((TMD_G_3G *)p)->b0;
				col[1].r = ((TMD_G_3G *)p)->r1; col[1].g = ((TMD_G_3G *)p)->g1; col[1].b = ((TMD_G_3G *)p)->b1;
				col[2].r = ((TMD_G_3G *)p)->r2; col[2].g = ((TMD_G_3G *)p)->g2; col[2].b = ((TMD_G_3G *)p)->b2;
				ncol = 3;

				*next = (PRIM_HDR *)((TMD_G_3G *)p + 1);
				}
			else
				{
				col[0].r = ((TMD_G_3 *)p)->r0; col[0].g = ((TMD_G_3 *)p)->g0; col[0].b = ((TMD_G_3 *)p)->b0;
				ncol = 1;

				*next = (PRIM_HDR *)((TMD_G_3 *)p + 1);
				}
			break;
		case G_4: // also case G_4G:
			if (prim->flag & GRD)
				{
				col[0].r = ((TMD_G_4G *)p)->r0; col[0].g = ((TMD_G_4G *)p)->g0; col[0].b = ((TMD_G_4G *)p)->b0;
				col[1].r = ((TMD_G_4G *)p)->r1; col[1].g = ((TMD_G_4G *)p)->g1; col[1].b = ((TMD_G_4G *)p)->b1;
				col[2].r = ((TMD_G_4G *)p)->r2; col[2].g = ((TMD_G_4G *)p)->g2; col[2].b = ((TMD_G_4G *)p)->b2;
				col[3].r = ((TMD_G_4G *)p)->r3; col[3].g = ((TMD_G_4G *)p)->g3; col[3].b = ((TMD_G_4G *)p)->b3;
				ncol = 4;

				*next = (PRIM_HDR *)((TMD_G_4G *)p + 1);
				}
			else
				{
				col[0].r = ((TMD_G_4 *)p)->r0; col[0].g = ((TMD_G_4 *)p)->g0; col[0].b = ((TMD_G_4 *)p)->b0;
				ncol = 1;

				*next = (PRIM_HDR *)((TMD_G_4 *)p + 1);
				}
			break;
		case F_3_NL:
			col[0].r = ((TMD_F_3_NL *)p)->r0; col[0].g = ((TMD_F_3_NL *)p)->g0; col[0].b = ((TMD_F_3_NL *)p)->b0;
			ncol = 1;

			*next = (PRIM_HDR *)((TMD_F_3_NL *)p + 1);
			break;
		case F_3T_NL:
			col[0].r = ((TMD_F_3T_NL *)p)->r0; col[0].g = ((TMD_F_3T_NL *)p)->g0; col[0].b = ((TMD_F_3T_NL *)p)->b0;
			ncol = 1;

			*next = (PRIM_HDR *)((TMD_F_3T_NL *)p + 1);
			break;
		case F_4_NL:
			col[0].r = ((TMD_F_4_NL *)p)->r0; col[0].g = ((TMD_F_4_NL *)p)->g0; col[0].b = ((TMD_F_4_NL *)p)->b0;
			ncol = 1;

			*next = (PRIM_HDR *)((TMD_F_4_NL *)p + 1);
			break;
		case F_4T_NL:
			col[0].r = ((TMD_F_4T_NL *)p)->r0; col[0].g = ((TMD_F_4T_NL *)p)->g0; col[0].b = ((TMD_F_4T_NL *)p)->b0;
			ncol = 1;

			*next = (PRIM_HDR *)((TMD_F_4T_NL *)p + 1);
			break;
		case G_3_NL: 
			col[0].r = ((TMD_G_3_NL *)p)->r0; col[0].g = ((TMD_G_3_NL *)p)->g0; col[0].b = ((TMD_G_3_NL *)p)->b0;
			col[1].r = ((TMD_G_3_NL *)p)->r1; col[1].g = ((TMD_G_3_NL *)p)->g1; col[1].b = ((TMD_G_3_NL *)p)->b1;
			col[2].r = ((TMD_G_3_NL *)p)->r2; col[2].g = ((TMD_G_3_NL *)p)->g2; col[2].b = ((TMD_G_3_NL *)p)->b2;
			ncol = 3;

			*next = (PRIM_HDR *)((TMD_G_3_NL *)p + 1);
			break;
		case G_3T_NL:
			col[0].r = ((TMD_G_3T_NL *)p)->r0; col[0].g = ((TMD_G_3T_NL *)p)->g0; col[0].b = ((TMD_G_3T_NL *)p)->b0;
			col[1].r = ((TMD_G_3T_NL *)p)->r1; col[1].g = ((TMD_G_3T_NL *)p)->g1; col[1].b = ((TMD_G_3T_NL *)p)->b1;
			col[2].r = ((TMD_G_3T_NL *)p)->r2; col[2].g = ((TMD_G_3T_NL *)p)->g2; col[2].b = ((TMD_G_3T_NL *)p)->b2;
			ncol = 3;

			*next = (PRIM_HDR *)((TMD_G_3T_NL *)p + 1);
			break;
		case G_4_NL: 
			col[0].r = ((TMD_G_4_NL *)p)->r0; col[0].g = ((TMD_G_4_NL *)p)->g0; col[0].b = ((TMD_G_4_NL *)p)->b0;
			col[1].r = ((TMD_G_4_NL *)p)->r1; col[1].g = ((TMD_G_4_NL *)p)->g1; col[1].b = ((TMD_G_4_NL *)p)->b1;
			col[2].r = ((TMD_G_4_NL *)p)->r2; col[2].g = ((TMD_G_4_NL *)p)->g2; col[2].b = ((TMD_G_4_NL *)p)->b2;
			col[3].r = ((TMD_G_4_NL *)p)->r3; col[3].g = ((TMD_G_4_NL *)p)->g3; col[3].b = ((TMD_G_4_NL *)p)->b3;
			ncol = 4;

			*next = (PRIM_HDR *)((TMD_G_4_NL *)p + 1);
			break;
		case G_4T_NL:
			col[0].r = ((TMD_G_4T_NL *)p)->r0; col[0].g = ((TMD_G_4T_NL *)p)->g0; col[0].b = ((TMD_G_4T_NL *)p)->b0;
			col[1].r = ((TMD_G_4T_NL *)p)->r1; col[1].g = ((TMD_G_4T_NL *)p)->g1; col[1].b = ((TMD_G_4T_NL *)p)->b1;
			col[2].r = ((TMD_G_4T_NL *)p)->r2; col[2].g = ((TMD_G_4T_NL *)p)->g2; col[2].b = ((TMD_G_4T_NL *)p)->b2;
			col[3].r = ((TMD_G_4T_NL *)p)->r3; col[3].g = ((TMD_G_4T_NL *)p)->g3; col[3].b = ((TMD_G_4T_NL *)p)->b3;
			ncol = 4;

			*next = (PRIM_HDR *)((TMD_G_4T_NL *)p + 1);
			break;
		case SL:
			col[0].r = ((TMD_SL *)p)->r0; col[0].g = ((TMD_SL *)p)->g0; col[0].b = ((TMD_SL *)p)->b0;
			ncol = 1;

			*next = (PRIM_HDR *)((TMD_SL *)p + 1);
			break;
		case SL_G:
			col[0].r = ((TMD_SL_G *)p)->r0; col[0].g = ((TMD_SL_G *)p)->g0; col[0].b = ((TMD_SL_G *)p)->b0;
			ncol = 1;

			*next = (PRIM_HDR *)((TMD_SL_G *)p + 1);
			break;
		}
	return ncol;
}


// ************************************************************************
// this sets the colour in the primitive as with setTMDNormals and
// setTMDVertices knowledge of the primitive is required
u_char setTMDColors(PRIM_HDR *prim, CVECTOR *col, PRIM_HDR *(*next))
{
	u_char ncol = 0;
	u_long *p;

	p = (u_long *)(prim + 1);

	switch(prim->mode)
		{
		case F_3: // also case F_3G: as they have the same mode value
			if (prim->flag & GRD)	 // gradation performed
				{
				((TMD_F_3G *)p)->r0 = col[0].r;	((TMD_F_3G *)p)->g0 = col[0].g;	((TMD_F_3G *)p)->b0 = col[0].b;
				((TMD_F_3G *)p)->r1 = col[1].r;	((TMD_F_3G *)p)->g1 = col[1].g;	((TMD_F_3G *)p)->b1 = col[1].b;
				((TMD_F_3G *)p)->r2 = col[2].r;	((TMD_F_3G *)p)->g2 = col[2].g;	((TMD_F_3G *)p)->b2 = col[2].b;
				ncol = 3;

				*next = (PRIM_HDR *)((TMD_F_3G *)p + 1);
				}
			else					 // no gradation
				{
				((TMD_F_3 *)p)->r0 = col[0].r; ((TMD_F_3 *)p)->g0 = col[0].g; ((TMD_F_3 *)p)->b0 = col[0].b;
				ncol = 1;

				*next = (PRIM_HDR *)((TMD_F_3 *)p + 1);
				}
			break;
		case F_4: // also case F_4G:
			if (prim->flag & GRD)
				{
				((TMD_F_4G *)p)->r0 = col[0].r; ((TMD_F_4G *)p)->g0 = col[0].g; ((TMD_F_4G *)p)->b0 = col[0].b;
				((TMD_F_4G *)p)->r1 = col[1].r; ((TMD_F_4G *)p)->g1 = col[1].g; ((TMD_F_4G *)p)->b1 = col[1].b;
				((TMD_F_4G *)p)->r2 = col[2].r; ((TMD_F_4G *)p)->g2 = col[2].g; ((TMD_F_4G *)p)->b2 = col[2].b;
				((TMD_F_4G *)p)->r3 = col[3].r; ((TMD_F_4G *)p)->g3 = col[3].g; ((TMD_F_4G *)p)->b3 = col[3].b;
				ncol = 4;

				*next = (PRIM_HDR *)((TMD_F_4G *)p + 1);
				}
			else
				{
				((TMD_F_4 *)p)->r0 = col[0].r; ((TMD_F_4 *)p)->g0 = col[0].g; ((TMD_F_4 *)p)->b0 = col[0].b;
				ncol = 1;

				*next = (PRIM_HDR *)((TMD_F_4 *)p + 1);
				}
			break;
		case G_3: // also case G_3G:
			if (prim->flag & GRD)
				{
				((TMD_G_3G *)p)->r0 = col[0].r; ((TMD_G_3G *)p)->g0 = col[0].g; ((TMD_G_3G *)p)->b0 = col[0].b;
				((TMD_G_3G *)p)->r1 = col[1].r; ((TMD_G_3G *)p)->g1 = col[1].g; ((TMD_G_3G *)p)->b1 = col[1].b;
				((TMD_G_3G *)p)->r2 = col[2].r; ((TMD_G_3G *)p)->g2 = col[2].g; ((TMD_G_3G *)p)->b2 = col[2].b;
				ncol = 3;

				*next = (PRIM_HDR *)((TMD_G_3G *)p + 1);
				}
			else
				{
				((TMD_G_3 *)p)->r0 = col[0].r; ((TMD_G_3 *)p)->g0 = col[0].g; ((TMD_G_3 *)p)->b0 = col[0].b;
				ncol = 1;

				*next = (PRIM_HDR *)((TMD_G_3 *)p + 1);
				}
			break;
		case G_4: // also case G_4G:
			if (prim->flag & GRD)
				{
				((TMD_G_4G *)p)->r0 = col[0].r; ((TMD_G_4G *)p)->g0 = col[0].g; ((TMD_G_4G *)p)->b0 = col[0].b;
				((TMD_G_4G *)p)->r1 = col[1].r; ((TMD_G_4G *)p)->g1 = col[1].g; ((TMD_G_4G *)p)->b1 = col[1].b;
				((TMD_G_4G *)p)->r2 = col[2].r; ((TMD_G_4G *)p)->g2 = col[2].g; ((TMD_G_4G *)p)->b2 = col[2].b;
				((TMD_G_4G *)p)->r3 = col[3].r; ((TMD_G_4G *)p)->g3 = col[3].g; ((TMD_G_4G *)p)->b3 = col[3].b;
				ncol = 4;

				*next = (PRIM_HDR *)((TMD_G_4G *)p + 1);
				}
			else
				{
				((TMD_G_4 *)p)->r0 = col[0].r; ((TMD_G_4 *)p)->g0 = col[0].g; ((TMD_G_4 *)p)->b0 = col[0].b;
				ncol = 1;

				*next = (PRIM_HDR *)((TMD_G_4 *)p + 1);
				}
			break;
		case F_3_NL:
			((TMD_F_3_NL *)p)->r0 = col[0].r; ((TMD_F_3_NL *)p)->g0 = col[0].g; ((TMD_F_3_NL *)p)->b0 = col[0].b;
			ncol = 1;

			*next = (PRIM_HDR *)((TMD_F_3_NL *)p + 1);
			break;
		case F_3T_NL:
			((TMD_F_3T_NL *)p)->r0 = col[0].r; ((TMD_F_3T_NL *)p)->g0 = col[0].g; ((TMD_F_3T_NL *)p)->b0 = col[0].b;
			ncol = 1;

			*next = (PRIM_HDR *)((TMD_F_3T_NL *)p + 1);
			break;
		case F_4_NL:
			((TMD_F_4_NL *)p)->r0 = col[0].r; ((TMD_F_4_NL *)p)->g0 = col[0].g; ((TMD_F_4_NL *)p)->b0 = col[0].b;
			ncol = 1;

			*next = (PRIM_HDR *)((TMD_F_4_NL *)p + 1);
			break;
		case F_4T_NL:
			((TMD_F_4T_NL *)p)->r0 = col[0].r; ((TMD_F_4T_NL *)p)->g0 = col[0].g; ((TMD_F_4T_NL *)p)->b0 = col[0].b;
			ncol = 1;

			*next = (PRIM_HDR *)((TMD_F_4T_NL *)p + 1);
			break;
		case G_3_NL: 
			((TMD_G_3_NL *)p)->r0 = col[0].r; ((TMD_G_3_NL *)p)->g0 = col[0].g; ((TMD_G_3_NL *)p)->b0 = col[0].b;
			((TMD_G_3_NL *)p)->r1 = col[1].r; ((TMD_G_3_NL *)p)->g1 = col[1].g; ((TMD_G_3_NL *)p)->b1 = col[1].b;
			((TMD_G_3_NL *)p)->r2 = col[2].r; ((TMD_G_3_NL *)p)->g2 = col[2].g; ((TMD_G_3_NL *)p)->b2 = col[2].b;
			ncol = 3;

			*next = (PRIM_HDR *)((TMD_G_3_NL *)p + 1);
			break;
		case G_3T_NL:
			((TMD_G_3T_NL *)p)->r0 = col[0].r; ((TMD_G_3T_NL *)p)->g0 = col[0].g; ((TMD_G_3T_NL *)p)->b0 = col[0].b;
			((TMD_G_3T_NL *)p)->r1 = col[1].r; ((TMD_G_3T_NL *)p)->g1 = col[1].g; ((TMD_G_3T_NL *)p)->b1 = col[1].b;
			((TMD_G_3T_NL *)p)->r2 = col[2].r; ((TMD_G_3T_NL *)p)->g2 = col[2].g; ((TMD_G_3T_NL *)p)->b2 = col[2].b;
			ncol = 3;

			*next = (PRIM_HDR *)((TMD_G_3T_NL *)p + 1);
			break;
		case G_4_NL: 
			((TMD_G_4_NL *)p)->r0 = col[0].r; ((TMD_G_4_NL *)p)->g0 = col[0].g; ((TMD_G_4_NL *)p)->b0 = col[0].b;
			((TMD_G_4_NL *)p)->r1 = col[1].r; ((TMD_G_4_NL *)p)->g1 = col[1].g; ((TMD_G_4_NL *)p)->b1 = col[1].b;
			((TMD_G_4_NL *)p)->r2 = col[2].r; ((TMD_G_4_NL *)p)->g2 = col[2].g; ((TMD_G_4_NL *)p)->b2 = col[2].b;
			((TMD_G_4_NL *)p)->r3 = col[3].r; ((TMD_G_4_NL *)p)->g3 = col[3].g; ((TMD_G_4_NL *)p)->b3 = col[3].b;
			ncol = 4;

			*next = (PRIM_HDR *)((TMD_G_4_NL *)p + 1);
			break;
		case G_4T_NL:
			((TMD_G_4T_NL *)p)->r0 = col[0].r; ((TMD_G_4T_NL *)p)->g0 = col[0].g; ((TMD_G_4T_NL *)p)->b0 = col[0].b;
			((TMD_G_4T_NL *)p)->r1 = col[1].r; ((TMD_G_4T_NL *)p)->g1 = col[1].g; ((TMD_G_4T_NL *)p)->b1 = col[1].b;
			((TMD_G_4T_NL *)p)->r2 = col[2].r; ((TMD_G_4T_NL *)p)->g2 = col[2].g; ((TMD_G_4T_NL *)p)->b2 = col[2].b;
			((TMD_G_4T_NL *)p)->r3 = col[3].r; ((TMD_G_4T_NL *)p)->g3 = col[3].g; ((TMD_G_4T_NL *)p)->b3 = col[3].b;
			ncol = 4;

			*next = (PRIM_HDR *)((TMD_G_4T_NL *)p + 1);
			break;
		case SL:
			((TMD_SL *)p)->r0 = col[0].r; ((TMD_SL *)p)->g0 = col[0].g; ((TMD_SL *)p)->b0 = col[0].b;
			ncol = 1;

			*next = (PRIM_HDR *)((TMD_SL *)p + 1);
			break;
		case SL_G:
			((TMD_SL_G *)p)->r0 = col[0].r; ((TMD_SL_G *)p)->g0 = col[0].g; ((TMD_SL_G *)p)->b0 = col[0].b;
			ncol = 1;

			*next = (PRIM_HDR *)((TMD_SL_G *)p + 1);
			break;
		}
	return ncol;
}


// ************************************************************************
// return 0 if no texture info otherwise the CBA and TSB of	the primitive are
// found
u_char getTMDTextureInfo(PRIM_HDR *prim, u_short *cba, u_short *tsb, PRIM_HDR *(*next))
{
	u_char info = 0;
	u_long *p;

	p = (u_long *)(prim + 1);

	switch(prim->mode)
		{
		case F_3T:
			*cba = ((TMD_F_3T *)p)->cba;
			*tsb = ((TMD_F_3T *)p)->tsb;
			info = 1;

			*next = (PRIM_HDR *)((TMD_F_3T *)p + 1);
			break;
		case F_4T:
			*cba = ((TMD_F_4T *)p)->cba;
			*tsb = ((TMD_F_4T *)p)->tsb;
			info = 1;

			*next = (PRIM_HDR *)((TMD_F_4T *)p + 1);
			break;
		case G_3T:
			*cba = ((TMD_G_3T *)p)->cba;
			*tsb = ((TMD_G_3T *)p)->tsb;
			info = 1;

			*next = (PRIM_HDR *)((TMD_G_3T *)p + 1);
			break;
		case G_4T:
			*cba = ((TMD_G_4T *)p)->cba;
			*tsb = ((TMD_G_4T *)p)->tsb;
			info = 1;

			*next = (PRIM_HDR *)((TMD_G_4T *)p + 1);
			break;
		case F_3T_NL:
			*cba = ((TMD_F_3T_NL *)p)->cba;
			*tsb = ((TMD_F_3T_NL *)p)->tsb;
			info = 1;

			*next = (PRIM_HDR *)((TMD_F_3T_NL *)p + 1);
			break;
		case F_4T_NL:
			*cba = ((TMD_F_4T_NL *)p)->cba;
			*tsb = ((TMD_F_4T_NL *)p)->tsb;
			info = 1;

			*next = (PRIM_HDR *)((TMD_F_4T_NL *)p + 1);
			break;
		case G_3T_NL:
			*cba = ((TMD_G_3T_NL *)p)->cba;
			*tsb = ((TMD_G_3T_NL *)p)->tsb;
			info = 1;

			*next = (PRIM_HDR *)((TMD_G_3T_NL *)p + 1);
			break;
		case G_4T_NL:
			*cba = ((TMD_G_4T_NL *)p)->cba;
			*tsb = ((TMD_G_4T_NL *)p)->tsb;
			info = 1;

			*next = (PRIM_HDR *)((TMD_G_4T_NL *)p + 1);
			break;
		case SP_3D:
			*cba = ((TMD_SP_3D *)p)->cba;
			*tsb = ((TMD_SP_3D *)p)->tsb;
			info = 1;

			*next = (PRIM_HDR *)((TMD_SP_3D *)p + 1);
			break;
		case SP_3D_1:
			*cba = ((TMD_SP_3D_1 *)p)->cba;
			*tsb = ((TMD_SP_3D_1 *)p)->tsb;
			info = 1;

			*next = (PRIM_HDR *)((TMD_SP_3D_1 *)p + 1);
			break;
		case SP_3D_8:
			*cba = ((TMD_SP_3D_8 *)p)->cba;
			*tsb = ((TMD_SP_3D_8 *)p)->tsb;
			info = 1;

			*next = (PRIM_HDR *)((TMD_SP_3D_8 *)p + 1);
			break;
		case SP_3D_16:
			*cba = ((TMD_SP_3D_16 *)p)->cba;
			*tsb = ((TMD_SP_3D_16 *)p)->tsb;
			info = 1;

			*next = (PRIM_HDR *)((TMD_SP_3D_16 *)p + 1);
			break;
		}
	return info;
}


// ************************************************************************
// sets the CBA and TSB of a textured primitive.
u_char setTMDTextureInfo(PRIM_HDR *prim, u_short cba, u_short tsb, PRIM_HDR *(*next))
{
	u_char info = 0;
	u_long *p;

	p = (u_long *)(prim + 1);

	switch(prim->mode)
		{
		case F_3T:
			((TMD_F_3T *)p)->cba = cba;
			((TMD_F_3T *)p)->tsb = tsb;
			info = 1;

			*next = (PRIM_HDR *)((TMD_F_3T *)p + 1);
			break;
		case F_4T:
			((TMD_F_4T *)p)->cba = cba;
			((TMD_F_4T *)p)->tsb = tsb;
			info = 1;

			*next = (PRIM_HDR *)((TMD_F_4T *)p + 1);
			break;
		case G_3T:
			((TMD_G_3T *)p)->cba = cba;
			((TMD_G_3T *)p)->tsb = tsb;
			info = 1;

			*next = (PRIM_HDR *)((TMD_G_3T *)p + 1);
			break;
		case G_4T:
			((TMD_G_4T *)p)->cba = cba;
			((TMD_G_4T *)p)->tsb = tsb;
			info = 1;

			*next = (PRIM_HDR *)((TMD_G_4T *)p + 1);
			break;
		case F_3T_NL:
			((TMD_F_3T_NL *)p)->cba = cba;
			((TMD_F_3T_NL *)p)->tsb = tsb;
			info = 1;

			*next = (PRIM_HDR *)((TMD_F_3T_NL *)p + 1);
			break;
		case F_4T_NL:
			((TMD_F_4T_NL *)p)->cba = cba;
			((TMD_F_4T_NL *)p)->tsb = tsb;
			info = 1;

			*next = (PRIM_HDR *)((TMD_F_4T_NL *)p + 1);
			break;
		case G_3T_NL:
			((TMD_G_3T_NL *)p)->cba = cba;
			((TMD_G_3T_NL *)p)->tsb = tsb;
			info = 1;

			*next = (PRIM_HDR *)((TMD_G_3T_NL *)p + 1);
			break;
		case G_4T_NL:
			((TMD_G_4T_NL *)p)->cba = cba;
			((TMD_G_4T_NL *)p)->tsb = tsb;
			info = 1;

			*next = (PRIM_HDR *)((TMD_G_4T_NL *)p + 1);
			break;
		case SP_3D:
			((TMD_SP_3D *)p)->cba = cba;
			((TMD_SP_3D *)p)->tsb = tsb;
			info = 1;

			*next = (PRIM_HDR *)((TMD_SP_3D *)p + 1);
			break;
		case SP_3D_1:
			((TMD_SP_3D_1 *)p)->cba = cba;
			((TMD_SP_3D_1 *)p)->tsb = tsb;
			info = 1;

			*next = (PRIM_HDR *)((TMD_SP_3D_1 *)p + 1);
			break;
		case SP_3D_8:
			((TMD_SP_3D_8 *)p)->cba = cba;
			((TMD_SP_3D_8 *)p)->tsb = tsb;
			info = 1;

			*next = (PRIM_HDR *)((TMD_SP_3D_8 *)p + 1);
			break;
		case SP_3D_16:
			((TMD_SP_3D_16 *)p)->cba = cba;
			((TMD_SP_3D_16 *)p)->tsb = tsb;
			info = 1;

			*next = (PRIM_HDR *)((TMD_SP_3D_16 *)p + 1);
			break;
		}
	return info;
}


// ************************************************************************
// returns the length of the primitive inc header in words
// a return value of 0 means that the primitive specified is invalid
u_char getTMDPrimLength(PRIM_HDR *prim, PRIM_HDR *(*next))
{
	u_char len = 0;
	u_long *p;

	p = (u_long *)(prim + 1);

	switch(prim->mode)
		{
		case F_3: // also case F_3G: as they are the same value
			if (prim->flag & GRD)	 // gradation performed
				{
				len = 6;

				*next = (PRIM_HDR *)((TMD_F_3G *)p + 1);
				}
			else					 // no gradation
				{
				len = 4;

				*next = (PRIM_HDR *)((TMD_F_3 *)p + 1);
				}
			break;
		case F_3T:
			len = 6;

			*next = (PRIM_HDR *)((TMD_F_3T *)p + 1);
			break;
		case F_4: // also case F_4G:
			if (prim->flag & GRD)
				{
				len = 8;

				*next = (PRIM_HDR *)((TMD_F_4G *)p + 1);
				}
			else
				{
				len = 5;

				*next = (PRIM_HDR *)((TMD_F_4 *)p + 1);
				}
			break;
		case F_4T:
			len = 8;

			*next = (PRIM_HDR *)((TMD_F_4T *)p + 1);
			break;
		case G_3: // also case G_3G:
			if (prim->flag & GRD)
				{
				len = 7;

				*next = (PRIM_HDR *)((TMD_G_3G *)p + 1);
				}
			else
				{
				len = 5;

				*next = (PRIM_HDR *)((TMD_G_3 *)p + 1);
				}
			break;
		case G_3T:
			len = 7;

			*next = (PRIM_HDR *)((TMD_G_3T *)p + 1);
			break;
		case G_4: // also case G_4G:
			if (prim->flag & GRD)
				{
				len = 9;

				*next = (PRIM_HDR *)((TMD_G_4G *)p + 1);
				}
			else
				{
				len = 6;

				*next = (PRIM_HDR *)((TMD_G_4 *)p + 1);
				}
			break;
		case G_4T:
			len = 9;

			*next = (PRIM_HDR *)((TMD_G_4T *)p + 1);
			break;
		case F_3_NL:
			len = 4;

			*next = (PRIM_HDR *)((TMD_F_3_NL *)p + 1);
			break;
		case F_3T_NL:
			len = 7;

			*next = (PRIM_HDR *)((TMD_F_3T_NL *)p + 1);
			break;
		case F_4_NL:
			len = 4;

			*next = (PRIM_HDR *)((TMD_F_4_NL *)p + 1);
			break;
		case F_4T_NL:
			len = 8;

			*next = (PRIM_HDR *)((TMD_F_4T_NL *)p + 1);
			break;
		case G_3_NL: 
			len = 6;

			*next = (PRIM_HDR *)((TMD_G_3_NL *)p + 1);
			break;
		case G_3T_NL:
			len = 9;

			*next = (PRIM_HDR *)((TMD_G_3T_NL *)p + 1);
			break;
		case G_4_NL: 
			len = 7;

			*next = (PRIM_HDR *)((TMD_G_4_NL *)p + 1);
			break;
		case G_4T_NL:
			len = 11;

			*next = (PRIM_HDR *)((TMD_G_4T_NL *)p + 1);
			break;
		case SL:
			len = 3;

			*next = (PRIM_HDR *)((TMD_SL *)p + 1);
			break;
		case SL_G:
			len = 4;

			*next = (PRIM_HDR *)((TMD_SL_G *)p + 1);
			break;
		case SP_3D:
			len = 4;

			*next = (PRIM_HDR *)((TMD_SP_3D *)p + 1);
			break;
		case SP_3D_1:
			len = 3;

			*next = (PRIM_HDR *)((TMD_SP_3D_1 *)p + 1);
			break;
		case SP_3D_8:
			len = 3;

			*next = (PRIM_HDR *)((TMD_SP_3D_8 *)p + 1);
			break;
		case SP_3D_16:
			len = 3;

			*next = (PRIM_HDR *)((TMD_SP_3D_16 *)p + 1);
			break;
		}
	return len;
}


// ************************************************************************
// this simply copies from the source primitive to the destination
// pointers to the next primitive in the source and destination tmd
// are also set. the return value is the length of the primitive
u_char copyTMDPrim(PRIM_HDR *src, PRIM_HDR *dst, PRIM_HDR *(*srcnext), PRIM_HDR *(*dstnext))
{
	u_char len = 0;
	u_long *p, *q;

	p = (u_long *)(src + 1);
	q = (u_long *)(dst + 1);

	// copy header
	*dst = *src;

	switch(src->mode)
		{
		case F_3: // also case F_3G: as they are the same value
			if (src->flag & GRD)	 // gradation performed
				{
				*(TMD_F_3G *)q = *(TMD_F_3G *)p;
				len = 6;

				*srcnext = (PRIM_HDR *)((TMD_F_3G *)p + 1);
				*dstnext = (PRIM_HDR *)((TMD_F_3G *)q + 1);
				}
			else					 // no gradation
				{
				*(TMD_F_3 *)q = *(TMD_F_3 *)p;
				len = 4;

				*srcnext = (PRIM_HDR *)((TMD_F_3 *)p + 1);
				*dstnext = (PRIM_HDR *)((TMD_F_3 *)q + 1);
				}
			break;
		case F_3T:
			*(TMD_F_3T *)q = *(TMD_F_3T *)p;
			len = 6;

			*srcnext = (PRIM_HDR *)((TMD_F_3T *)p + 1);
			*dstnext = (PRIM_HDR *)((TMD_F_3T *)q + 1);
			break;
		case F_4: // also case F_4G:
			if (src->flag & GRD)
				{
				*(TMD_F_4G *)q = *(TMD_F_4G *)p;
				len = 8;

				*srcnext = (PRIM_HDR *)((TMD_F_4G *)p + 1);
				*dstnext = (PRIM_HDR *)((TMD_F_4G *)q + 1);
				}
			else
				{
				*(TMD_F_4 *)q = *(TMD_F_4 *)p;
				len = 5;

				*srcnext = (PRIM_HDR *)((TMD_F_4 *)p + 1);
				*dstnext = (PRIM_HDR *)((TMD_F_4 *)q + 1);
				}
			break;
		case F_4T:
			*(TMD_F_4T *)q = *(TMD_F_4T *)p;
			len = 8;

			*srcnext = (PRIM_HDR *)((TMD_F_4T *)p + 1);
			*dstnext = (PRIM_HDR *)((TMD_F_4T *)q + 1);
			break;
		case G_3: // also case G_3G:
			if (src->flag & GRD)
				{
				*(TMD_G_3G *)q = *(TMD_G_3G *)p;
				len = 7;

				*srcnext = (PRIM_HDR *)((TMD_G_3G *)p + 1);
				*dstnext = (PRIM_HDR *)((TMD_G_3G *)q + 1);
				}
			else
				{
				*(TMD_G_3 *)q = *(TMD_G_3 *)p;
				len = 5;

				*srcnext = (PRIM_HDR *)((TMD_G_3 *)p + 1);
				*dstnext = (PRIM_HDR *)((TMD_G_3 *)q + 1);
				}
			break;
		case G_3T:
			*(TMD_G_3T *)q = *(TMD_G_3T *)p;
			len = 7;

			*srcnext = (PRIM_HDR *)((TMD_G_3T *)p + 1);
			*dstnext = (PRIM_HDR *)((TMD_G_3T *)q + 1);
			break;
		case G_4: // also case G_4G:
			if (src->flag & GRD)
				{
				*(TMD_G_4G *)q = *(TMD_G_4G *)p;
				len = 9;

				*srcnext = (PRIM_HDR *)((TMD_G_4G *)p + 1);
				*dstnext = (PRIM_HDR *)((TMD_G_4G *)q + 1);
				}
			else
				{
				*(TMD_G_4 *)q = *(TMD_G_4 *)p;
				len = 6;

				*srcnext = (PRIM_HDR *)((TMD_G_4 *)p + 1);
				*dstnext = (PRIM_HDR *)((TMD_G_4 *)q + 1);
				}
			break;
		case G_4T:
			*(TMD_G_4T *)q = *(TMD_G_4T *)p;
			len = 9;

			*srcnext = (PRIM_HDR *)((TMD_G_4T *)p + 1);
			*dstnext = (PRIM_HDR *)((TMD_G_4T *)q + 1);
			break;
		case F_3_NL:
			*(TMD_F_3_NL *)q = *(TMD_F_3_NL *)p;
			len = 4;

			*srcnext = (PRIM_HDR *)((TMD_F_3_NL *)p + 1);
			*dstnext = (PRIM_HDR *)((TMD_F_3_NL *)q + 1);
			break;
		case F_3T_NL:
			*(TMD_F_3T_NL *)q = *(TMD_F_3T_NL *)p;
			len = 7;

			*srcnext = (PRIM_HDR *)((TMD_F_3T_NL *)p + 1);
			*dstnext = (PRIM_HDR *)((TMD_F_3T_NL *)q + 1);
			break;
		case F_4_NL:
			*(TMD_F_4_NL *)q = *(TMD_F_4_NL *)p;
			len = 4;

			*srcnext = (PRIM_HDR *)((TMD_F_4_NL *)p + 1);
			*dstnext = (PRIM_HDR *)((TMD_F_4_NL *)q + 1);
			break;
		case F_4T_NL:
			*(TMD_F_4T_NL *)q = *(TMD_F_4T_NL *)p;
			len = 8;

			*srcnext = (PRIM_HDR *)((TMD_F_4T_NL *)p + 1);
			*dstnext = (PRIM_HDR *)((TMD_F_4T_NL *)q + 1);
			break;
		case G_3_NL: 
			*(TMD_G_3_NL *)q = *(TMD_G_3_NL *)p;
			len = 6;

			*srcnext = (PRIM_HDR *)((TMD_G_3_NL *)p + 1);
			*dstnext = (PRIM_HDR *)((TMD_G_3_NL *)q + 1);
			break;
		case G_3T_NL:
			*(TMD_G_3T_NL *)q = *(TMD_G_3T_NL *)p;
			len = 9;

			*srcnext = (PRIM_HDR *)((TMD_G_3T_NL *)p + 1);
			*dstnext = (PRIM_HDR *)((TMD_G_3T_NL *)q + 1);
			break;
		case G_4_NL: 
			*(TMD_G_4_NL *)q = *(TMD_G_4_NL *)p;
			len = 7;

			*srcnext = (PRIM_HDR *)((TMD_G_4_NL *)p + 1);
			*dstnext = (PRIM_HDR *)((TMD_G_4_NL *)q + 1);
			break;
		case G_4T_NL:
			*(TMD_G_4T_NL *)q = *(TMD_G_4T_NL *)p;
			len = 11;

			*srcnext = (PRIM_HDR *)((TMD_G_4T_NL *)p + 1);
			*dstnext = (PRIM_HDR *)((TMD_G_4T_NL *)q + 1);
			break;
		case SL:
			*(TMD_SL *)q = *(TMD_SL *)p;
			len = 3;

			*srcnext = (PRIM_HDR *)((TMD_SL *)p + 1);
			*dstnext = (PRIM_HDR *)((TMD_SL *)q + 1);
			break;
		case SL_G:
			*(TMD_SL_G *)q = *(TMD_SL_G *)p;
			len = 4;

			*srcnext = (PRIM_HDR *)((TMD_SL_G *)p + 1);
			*dstnext = (PRIM_HDR *)((TMD_SL_G *)q + 1);
			break;
		case SP_3D:
			*(TMD_SP_3D *)q = *(TMD_SP_3D *)p;
			len = 4;

			*srcnext = (PRIM_HDR *)((TMD_SP_3D *)p + 1);
			*dstnext = (PRIM_HDR *)((TMD_SP_3D *)q + 1);
			break;
		case SP_3D_1:
			*(TMD_SP_3D_1 *)q = *(TMD_SP_3D_1 *)p;
			len = 3;

			*srcnext = (PRIM_HDR *)((TMD_SP_3D_1 *)p + 1);
			*dstnext = (PRIM_HDR *)((TMD_SP_3D_1 *)q + 1);
			break;
		case SP_3D_8:
			*(TMD_SP_3D_8 *)q = *(TMD_SP_3D_8 *)p;
			len = 3;

			*srcnext = (PRIM_HDR *)((TMD_SP_3D_8 *)p + 1);
			*dstnext = (PRIM_HDR *)((TMD_SP_3D_8 *)q + 1);
			break;
		case SP_3D_16:
			*(TMD_SP_3D_16 *)q = *(TMD_SP_3D_16 *)p;
			len = 3;

			*srcnext = (PRIM_HDR *)((TMD_SP_3D_16 *)p + 1);
			*dstnext = (PRIM_HDR *)((TMD_SP_3D_16 *)q + 1);
			break;
		}
	return len;
}
