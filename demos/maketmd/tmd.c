/***************************************************************
*                                                              *
* Copyright (C) 1997 by Sony Computer Entertainment            *
*                       All rights Reserved                    *
*                                                              *
* S. Ashley 7th Aug 97                                         *
*                                                              *
***************************************************************/

#include "tmd.h"

static unsigned long *addr;
static unsigned long *vert_table, *prim_table, *norm_table;
static TMD_OBJ *obj_table;
static long vert_num, prim_num, norm_num;
static char primLength[30] = {4,4,0,0,6,7,0,0,5,4,0,0,8,8,0,0,5,6,0,0,7,9,0,0,6,7,0,0,9,11};

void TMD_start(unsigned long *tmd_addr)
	{
	vert_num = prim_num = norm_num = 0;
	addr = tmd_addr;
	setTMD_HDR((TMD_HDR *)addr);
	addr = (unsigned long *)((TMD_HDR *)addr + 1);
	obj_table = (TMD_OBJ *)addr;
	addr = (unsigned long *)((TMD_OBJ *)addr + 1);
	prim_table = addr;
	}

void TMD_end()
	{
	vert_table = addr;
	addr = (unsigned long *)((TMD_VERT *)addr + vert_num);
	norm_table = addr;
	setTMD_OBJ(obj_table, vert_table, vert_num, norm_table, norm_num, prim_table, prim_num);
	}

TMD_F3 *TMD_addPolyF3()
	{
	TMD_F3 *p = (TMD_F3 *)addr;

	setPRIM_HDR(p,F3,0,3,4);
	p->mode2 = F3;
	p->norm0 = norm_num++;
	p->vert0 = vert_num++;
	p->vert1 = vert_num++;
	p->vert2 = vert_num++;

	addr = (unsigned long *)(p + 1);
	prim_num++;

	return p;
	}

TMD_FT3 *TMD_addPolyFT3()
	{
	TMD_FT3 *p = (TMD_FT3 *)addr;

	setPRIM_HDR(p,FT3,0,5,7);
	p->norm0 = norm_num++;
	p->vert0 = vert_num++;
	p->vert1 = vert_num++;
	p->vert2 = vert_num++;

	addr = (unsigned long *)(p + 1);
	prim_num++;

	return p;
	}

TMD_F4 *TMD_addPolyF4()
	{
	TMD_F4 *p = (TMD_F4 *)addr;

	setPRIM_HDR(p,F4,0,4,5);
	p->mode2 = F4;
	p->norm0 = norm_num++;
	p->vert0 = vert_num++;
	p->vert1 = vert_num++;
	p->vert2 = vert_num++;
	p->vert3 = vert_num++;

	addr = (unsigned long *)(p + 1);
	prim_num++;

	return p;
	}

TMD_FT4 *TMD_addPolyFT4()
	{
	TMD_FT4 *p = (TMD_FT4 *)addr;

	setPRIM_HDR(p,FT4,0,7,9);
	p->norm0 = norm_num++;
	p->vert0 = vert_num++;
	p->vert1 = vert_num++;
	p->vert2 = vert_num++;
    p->vert3 = vert_num++;

	addr = (unsigned long *)(p + 1);
	prim_num++;

	return p;
	}

TMD_G3 *TMD_addPolyG3()
	{
	TMD_G3 *p = (TMD_G3 *)addr;

	setPRIM_HDR(p,G3,0,4,6);
	p->mode2 = G3;
	p->norm0 = norm_num++;
	p->norm1 = norm_num++;
	p->norm2 = norm_num++;
	p->vert0 = vert_num++;
	p->vert1 = vert_num++;
	p->vert2 = vert_num++;

	addr = (unsigned long *)(p + 1);
	prim_num++;

	return p;
	}

TMD_GT3 *TMD_addPolyGT3()
	{
	TMD_GT3 *p = (TMD_GT3 *)addr;

	setPRIM_HDR(p,GT3,0,6,9);
	p->norm0 = norm_num++;
	p->norm1 = norm_num++;
	p->norm2 = norm_num++;
	p->vert0 = vert_num++;
	p->vert1 = vert_num++;
    p->vert2 = vert_num++;

	addr = (unsigned long *)(p + 1);
	prim_num++;

	return p;
	}

TMD_G4 *TMD_addPolyG4()
	{
	TMD_G4 *p = (TMD_G4 *)addr;

	setPRIM_HDR(p,G4,0,5,8);
	p->mode2 = G4;
	p->norm0 = norm_num++;
	p->norm1 = norm_num++;
	p->norm2 = norm_num++;
	p->norm3 = norm_num++;
	p->vert0 = vert_num++;
	p->vert1 = vert_num++;
	p->vert2 = vert_num++;
	p->vert3 = vert_num++;

	addr = (unsigned long *)(p + 1);
	prim_num++;

	return p;
	}

TMD_GT4 *TMD_addPolyGT4()
	{
	TMD_GT4 *p = (TMD_GT4 *)addr;

	setPRIM_HDR(p,GT4,0,8,12);
	p->norm0 = norm_num++;
	p->norm1 = norm_num++;
	p->norm2 = norm_num++;
	p->norm3 = norm_num++;
	p->vert0 = vert_num++;
	p->vert1 = vert_num++;
	p->vert2 = vert_num++;
	p->vert3 = vert_num++;

	addr = (unsigned long *)(p + 1);
	prim_num++;

	return p;
	}

TMD_F3N *TMD_addPolyF3N()
	{
	TMD_F3N *p = (TMD_F3N *)addr;

	setPRIM_HDR(p,F3N,1,3,4);
	p->mode2 = F3N;
	p->vert0 = vert_num++;
	p->vert1 = vert_num++;
	p->vert2 = vert_num++;

	addr = (unsigned long *)(p + 1);
	prim_num++;

	return p;
	}

TMD_FT3N *TMD_addPolyFT3N()
	{
	TMD_FT3N *p = (TMD_FT3N *)addr;

	setPRIM_HDR(p,FT3N,1,6,7);
	p->vert0 = vert_num++;
	p->vert1 = vert_num++;
	p->vert2 = vert_num++;

	addr = (unsigned long *)(p + 1);
	prim_num++;

	return p;
	}

TMD_G3N *TMD_addPolyG3N()
	{
	TMD_G3N *p = (TMD_G3N *)addr;

	setPRIM_HDR(p,G3N,1,5,6);
	p->mode2 = G3N;
	p->vert0 = vert_num++;
	p->vert1 = vert_num++;
	p->vert2 = vert_num++;

	addr = (unsigned long *)(p + 1);
	prim_num++;

	return p;
	}

TMD_GT3N *TMD_addPolyGT3N()
	{
	TMD_GT3N *p = (TMD_GT3N *)addr;

	setPRIM_HDR(p,GT3N,1,8,9);
	p->vert0 = vert_num++;
	p->vert1 = vert_num++;
	p->vert2 = vert_num++;

	addr = (unsigned long *)(p + 1);
	prim_num++;

	return p;
	}

TMD_F4N *TMD_addPolyF4N()
	{
	TMD_F4N *p = (TMD_F4N *)addr;

	setPRIM_HDR(p,F4N,1,3,5);
	p->mode2 = F4N;
	p->vert0 = vert_num++;
	p->vert1 = vert_num++;
	p->vert2 = vert_num++;
	p->vert3 = vert_num++;

	addr = (unsigned long *)(p + 1);
	prim_num++;

	return p;
	}

TMD_FT4N *TMD_addPolyFT4N()
	{
	TMD_FT4N *p = (TMD_FT4N *)addr;

	setPRIM_HDR(p,FT4N,1,7,9);
	p->vert0 = vert_num++;
	p->vert1 = vert_num++;
	p->vert2 = vert_num++;
	p->vert3 = vert_num++;

	addr = (unsigned long *)(p + 1);
	prim_num++;

	return p;
	}

TMD_G4N *TMD_addPolyG4N()
	{
	TMD_G4N *p = (TMD_G4N *)addr;

	setPRIM_HDR(p,G4N,1,6,8);
	p->mode2 = G4N;
	p->vert0 = vert_num++;
	p->vert1 = vert_num++;
	p->vert2 = vert_num++;
	p->vert3 = vert_num++;

	addr = (unsigned long *)(p + 1);
	prim_num++;

	return p;
	}

TMD_GT4N *TMD_addPolyGT4N()
	{
	TMD_GT4N *p = (TMD_GT4N *)addr;

	setPRIM_HDR(p,GT4N,1,10,12);
	p->vert0 = vert_num++;
	p->vert1 = vert_num++;
	p->vert2 = vert_num++;
	p->vert3 = vert_num++;

	addr = (unsigned long *)(p + 1);
	prim_num++;

	return p;
	}

TMD_VERT *TMD_getVertAddr(unsigned long *tmd, unsigned char polyIdx)
	{
	int i;
	TMD_VERT *v;

	tmd += 3;
	v = (TMD_VERT *)(((TMD_OBJ *)tmd)->vert_top);
	tmd += 7;

	for(i = 0; i < polyIdx; i++)
		{
		if (((PRIM_HDR *)tmd)->mode & QUAD)
			v += 4;
		else
			v += 3;

		tmd += primLength[((PRIM_HDR *)tmd)->mode - 0x20];
		}

	return v;
	}

void TMD_setVert0(unsigned long *tmd, unsigned char polyIdx, short x, short y, short z)
	{
	int i;
	TMD_VERT *v;

	tmd += 3;
	v = (TMD_VERT *)(((TMD_OBJ *)tmd)->vert_top);
	tmd += 7;

	for(i = 0; i < polyIdx; i++)
		{
		if (((PRIM_HDR *)tmd)->mode & QUAD)
			v += 4;
		else
			v += 3;

		tmd += primLength[((PRIM_HDR *)tmd)->mode - 0x20];
		}

	v->vx = x;
	v->vy = y;
	v->vz = z;
	}

void TMD_setVert1(unsigned long *tmd, unsigned char polyIdx, short x, short y, short z)
	{
	int i;
	TMD_VERT *v;

	tmd += 3;
	v = (TMD_VERT *)(((TMD_OBJ *)tmd)->vert_top);
	tmd += 7;

	for(i = 0; i < polyIdx; i++)
		{
		if (((PRIM_HDR *)tmd)->mode & QUAD)
			v += 4;
		else
			v += 3;

		tmd += primLength[((PRIM_HDR *)tmd)->mode - 0x20];
		}

	v++;
	v->vx = x;
	v->vy = y;
	v->vz = z;
	}

void TMD_setVert2(unsigned long *tmd, unsigned char polyIdx, short x, short y, short z)
	{
	int i;
	TMD_VERT *v;

	tmd += 3;
	v = (TMD_VERT *)((TMD_OBJ *)tmd)->vert_top;
	tmd += 7;

	for(i = 0; i < polyIdx; i++)
		{
		if (((PRIM_HDR *)tmd)->mode & QUAD)
			v += 4;
		else
			v += 3;

		tmd += primLength[((PRIM_HDR *)tmd)->mode - 0x20];
		}

	v += 2;
	v->vx = x;
	v->vy = y;
	v->vz = z;
	}

void TMD_setVert3(unsigned long *tmd, unsigned char polyIdx, short x, short y, short z)
	{
	int i;
	TMD_VERT *v;

	tmd += 3;
	v = (TMD_VERT *)((TMD_OBJ *)tmd)->vert_top;
	tmd += 7;

	for(i = 0; i < polyIdx; i++)
		{
		if (((PRIM_HDR *)tmd)->mode & QUAD)
			v += 4;
		else
			v += 3;

		tmd += primLength[((PRIM_HDR *)tmd)->mode - 0x20];
		}

	v += 3;
	v->vx = x;
	v->vy = y;
	v->vz = z;
	}


TMD_NORM *TMD_getNormAddr(unsigned long *tmd, unsigned char polyIdx)
	{
	int i;
	TMD_NORM *n;

	tmd += 3;
	n = (TMD_NORM *)((TMD_OBJ *)tmd)->norm_top;
	tmd += 7;

	for(i = 0; i < polyIdx; i++)
		{
		if (((PRIM_HDR *)tmd)->mode & 0x10)
			{
			if (((PRIM_HDR *)tmd)->mode & ~0x01)
				if (((PRIM_HDR *)tmd)->mode & QUAD)
					n += 4;
				else
					n += 3;
			}
		else
			if (((PRIM_HDR *)tmd)->mode & ~0x01)
				n++;

		tmd += primLength[((PRIM_HDR *)tmd)->mode - 0x20];
		}

	return n;
	}


void TMD_setNorm0(unsigned long *tmd, unsigned char polyIdx, short x, short y, short z)
	{
	int i;
	TMD_NORM *n;

	tmd += 3;
	n = (TMD_NORM *)((TMD_OBJ *)tmd)->norm_top;
	tmd += 7;

	for(i = 0; i < polyIdx; i++)
		{
		if (((PRIM_HDR *)tmd)->mode & 0x10)
			{
			if (((PRIM_HDR *)tmd)->mode & ~0x01)
				if (((PRIM_HDR *)tmd)->mode & QUAD)
					n += 4;
				else
					n += 3;
			}
		else
			if (((PRIM_HDR *)tmd)->mode & ~0x01)
				n++;

		tmd += primLength[((PRIM_HDR *)tmd)->mode - 0x20];
		}

	n->nx = x;
	n->ny = y;
	n->nz = z;
	}

void TMD_setNorm1(unsigned long *tmd, unsigned char polyIdx, short x, short y, short z)
	{
	int i;
	TMD_NORM *n;

	tmd += 3;
	n = (TMD_NORM *)((TMD_OBJ *)tmd)->norm_top;
	tmd += 7;

	for(i = 0; i < polyIdx; i++)
		{
		if (((PRIM_HDR *)tmd)->mode & 0x10)
			{
			if (((PRIM_HDR *)tmd)->mode & ~0x01)
				if (((PRIM_HDR *)tmd)->mode & QUAD)
					n += 4;
				else
					n += 3;
			}
		else
			if (((PRIM_HDR *)tmd)->mode & ~0x01)
				n++;

		tmd += primLength[((PRIM_HDR *)tmd)->mode - 0x20];
		}

	n++;
	n->nx = x;
	n->ny = y;
	n->nz = z;
	}

void TMD_setNorm2(unsigned long *tmd, unsigned char polyIdx, short x, short y, short z)
	{
	int i;
	TMD_NORM *n;

	tmd += 3;
	n = (TMD_NORM *)((TMD_OBJ *)tmd)->norm_top;
	tmd += 7;

	for(i = 0; i < polyIdx; i++)
		{
		if (((PRIM_HDR *)tmd)->mode & 0x10)
			{
			if (((PRIM_HDR *)tmd)->mode & ~0x01)
				if (((PRIM_HDR *)tmd)->mode & QUAD)
					n += 4;
				else
					n += 3;
			}
		else
			if (((PRIM_HDR *)tmd)->mode & ~0x01)
				n++;

		tmd += primLength[((PRIM_HDR *)tmd)->mode - 0x20];
		}

	n += 2;
	n->nx = x;
	n->ny = y;
	n->nz = z;
	}

void TMD_setNorm3(unsigned long *tmd, unsigned char polyIdx, short x, short y, short z)
	{
	int i;
	TMD_NORM *n;

	tmd += 3;
	n = (TMD_NORM *)((TMD_OBJ *)tmd)->norm_top;
	tmd += 7;

	for(i = 0; i < polyIdx; i++)
		{
		if (((PRIM_HDR *)tmd)->mode & 0x10)
			{
			if (((PRIM_HDR *)tmd)->mode & ~0x01)
				if (((PRIM_HDR *)tmd)->mode & QUAD)
					n += 4;
				else
					n += 3;
			}
		else
			if (((PRIM_HDR *)tmd)->mode & ~0x01)
				n++;

		tmd += primLength[((PRIM_HDR *)tmd)->mode - 0x20];
		}

	n += 3;
	n->nx = x;
	n->ny = y;
	n->nz = z;
	}

void TMD_setRGB0(unsigned long *tmd, unsigned char polyIdx, short r, short g, short b)
	{
	int i;
	TMD_NORM *n;

	tmd += 10;

	for(i = 0; i < polyIdx; i++)
		{
		tmd += primLength[((PRIM_HDR *)tmd)->mode - 0x20];
		}

	switch (((PRIM_HDR *)tmd)->mode)
		{
		case F3:
			setRGB0((TMD_F3 *)tmd,r,g,b);
			break;
		case F3N:
			setRGB0((TMD_F3N *)tmd,r,g,b);
			break;
		case FT3N:
			setRGB0((TMD_FT3N *)tmd,r,g,b);
			break;
		case F4:
			setRGB0((TMD_F4 *)tmd,r,g,b);
			break;
		case F4N:
			setRGB0((TMD_F4N *)tmd,r,g,b);
			break;
		case FT4N:
			setRGB0((TMD_FT4N *)tmd,r,g,b);
			break;
		case G3:
			setRGB0((TMD_G3 *)tmd,r,g,b);
			break;
		case G3N:
			setRGB0((TMD_G3N *)tmd,r,g,b);
			break;
		case GT3N:
			setRGB0((TMD_GT3N *)tmd,r,g,b);
			break;
		case G4:
			setRGB0((TMD_G4 *)tmd,r,g,b);
			break;
		case G4N:
			setRGB0((TMD_G4N *)tmd,r,g,b);
			break;
		case GT4N:
			setRGB0((TMD_GT4N *)tmd,r,g,b);
			break;
		}
	}


void TMD_setRGB1(unsigned long *tmd, unsigned char polyIdx, short r, short g, short b)
	{
	int i;
	TMD_NORM *n;

	tmd += 10;

	for(i = 0; i < polyIdx; i++)
		{
		tmd += primLength[((PRIM_HDR *)tmd)->mode - 0x20];
		}

	switch (((PRIM_HDR *)tmd)->mode)
		{
		case G3N:
			setRGB1((TMD_G3N *)tmd,r,g,b);
			break;
		case GT3N:
			setRGB1((TMD_GT3N *)tmd,r,g,b);
			break;
		case G4N:
			setRGB1((TMD_G4N *)tmd,r,g,b);
			break;
		case GT4N:
			setRGB1((TMD_GT4N *)tmd,r,g,b);
			break;
		}

	}

void TMD_setRGB2(unsigned long *tmd, unsigned char polyIdx, short r, short g, short b)
	{
	int i;
	TMD_NORM *n;

	tmd += 10;

	for(i = 0; i < polyIdx; i++)
		{
		tmd += primLength[((PRIM_HDR *)tmd)->mode - 0x20];
		}

	switch (((PRIM_HDR *)tmd)->mode)
		{
		case G3N:
			setRGB2((TMD_G3N *)tmd,r,g,b);
			break;
		case GT3N:
			setRGB2((TMD_GT3N *)tmd,r,g,b);
			break;
		case G4N:
			setRGB2((TMD_G4N *)tmd,r,g,b);
			break;
		case GT4N:
			setRGB2((TMD_GT4N *)tmd,r,g,b);
			break;
		}
	}

void TMD_setRGB3(unsigned long *tmd, unsigned char polyIdx, short r, short g, short b)
	{
	int i;
	TMD_NORM *n;

	tmd += 10;

	for(i = 0; i < polyIdx; i++)
		{
		tmd += primLength[((PRIM_HDR *)tmd)->mode - 0x20];
		}

	switch (((PRIM_HDR *)tmd)->mode)
		{
		case G4N:
			setRGB3((TMD_G4N *)tmd,r,g,b);
			break;
		case GT4N:
			setRGB3((TMD_GT4N *)tmd,r,g,b);
			break;
		}
	}


void TMD_setUV3(unsigned long *tmd, unsigned char polyIdx, unsigned char u0, unsigned char v0, unsigned char u1, unsigned char v1, unsigned char u2, unsigned char v2)
	{
	int i;
	TMD_NORM *n;

	tmd += 10;

	for(i = 0; i < polyIdx; i++)
		{
		tmd += primLength[((PRIM_HDR *)tmd)->mode - 0x20];
		}

	switch (((PRIM_HDR *)tmd)->mode)
		{
		case FT3:
			setUV3((TMD_FT3 *)tmd,u0,v0,u1,v1,u2,v2);
			break;
		case FT3N:
			setUV3((TMD_FT3N *)tmd,u0,v0,u1,v1,u2,v2);
			break;
		case FT4:
			setUV3((TMD_FT4 *)tmd,u0,v0,u1,v1,u2,v2);
			break;
		case FT4N:
			setUV3((TMD_FT4N *)tmd,u0,v0,u1,v1,u2,v2);
			break;
		case GT3:
			setUV3((TMD_GT3 *)tmd,u0,v0,u1,v1,u2,v2);
			break;
		case GT3N:
			setUV3((TMD_GT3N *)tmd,u0,v0,u1,v1,u2,v2);
			break;
		case GT4:
			setUV3((TMD_GT4 *)tmd,u0,v0,u1,v1,u2,v2);
			break;
		case GT4N:
			setUV3((TMD_GT4N *)tmd,u0,v0,u1,v1,u2,v2);
			break;
		}
	}

void TMD_setUV4(unsigned long *tmd, unsigned char polyIdx, unsigned char u0, unsigned char v0, unsigned char u1, unsigned char v1, unsigned char u2, unsigned char v2, unsigned char u3, unsigned char v3)
	{
	int i;
	TMD_NORM *n;

	tmd += 10;

	for(i = 0; i < polyIdx; i++)
		{
		tmd += primLength[((PRIM_HDR *)tmd)->mode - 0x20];
		}

	switch (((PRIM_HDR *)tmd)->mode)
		{
		case FT4:
			setUV4((TMD_FT4 *)tmd,u0,v0,u1,v1,u2,v2,u3,v3);
			break;
		case FT4N:
			setUV4((TMD_FT4N *)tmd,u0,v0,u1,v1,u2,v2,u3,v3);
			break;
		case GT4:
			setUV4((TMD_GT4 *)tmd,u0,v0,u1,v1,u2,v2,u3,v3);
			break;
		case GT4N:
			setUV4((TMD_GT4N *)tmd,u0,v0,u1,v1,u2,v2,u3,v3);
			break;
		}
	}

void TMD_setClut(unsigned long *tmd, unsigned char polyIdx, unsigned short clut)
	{
	int i;
	TMD_NORM *n;

	tmd += 10;

	for(i = 0; i < polyIdx; i++)
		{
		tmd += primLength[((PRIM_HDR *)tmd)->mode - 0x20];
		}

	switch (((PRIM_HDR *)tmd)->mode)
		{
		case FT3:
			setClut((TMD_FT3 *)tmd,clut);
			break;
		case FT3N:
			setClut((TMD_FT3N *)tmd,clut);
			break;
		case FT4:
			setClut((TMD_FT4 *)tmd,clut);
			break;
		case FT4N:
			setClut((TMD_FT4N *)tmd,clut);
			break;
		case GT3:
			setClut((TMD_GT3 *)tmd,clut);
			break;
		case GT3N:
			setClut((TMD_GT3N *)tmd,clut);
			break;
		case GT4:
			setClut((TMD_GT4 *)tmd,clut);
			break;
		case GT4N:
			setClut((TMD_GT4N *)tmd,clut);
			break;
		}
	}

void TMD_setTPage(unsigned long *tmd, unsigned char polyIdx, unsigned short tpage)
	{
	int i;
	TMD_NORM *n;

	tmd += 10;

	for(i = 0; i < polyIdx; i++)
		{
		tmd += primLength[((PRIM_HDR *)tmd)->mode - 0x20];
		}

	switch (((PRIM_HDR *)tmd)->mode)
		{
		case FT3:
			setTPage((TMD_FT3 *)tmd,tpage);
			break;
		case FT3N:
			setTPage((TMD_FT3N *)tmd,tpage);
			break;
		case FT4:
			setTPage((TMD_FT4 *)tmd,tpage);
			break;
		case FT4N:
			setTPage((TMD_FT4N *)tmd,tpage);
			break;
		case GT3:
			setTPage((TMD_GT3 *)tmd,tpage);
			break;
		case GT3N:
			setTPage((TMD_GT3N *)tmd,tpage);
			break;
		case GT4:
			setTPage((TMD_GT4 *)tmd,tpage);
			break;
		case GT4N:
			setTPage((TMD_GT4N *)tmd,tpage);
			break;
		}
	}


