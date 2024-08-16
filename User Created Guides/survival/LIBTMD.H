/*
 * LIBTMD.H
 *
 * Copyright (C) 1993-1997 by Sony Computer Entertainment of America, Inc.
 * All rights Reserved
 * ----------------------------------------------------------------------------
 */

#ifndef	__YAROZE_LIBTMD_H
#define	__YAROZE_LIBTMD_H

/*
 * This file contains data structures for a TMD file and the data
 * structures used to create explosions.
 * ----------------------------------------------------------------------------
 */

/*
 * TMD file header
 * ----------------------------------------------------------------------------
 */

typedef struct __tmd_header {
	long	id;
	long	flags;
	long	nobjc;
} TMDHEAD;

/*
 * TMD object table
 * ----------------------------------------------------------------------------
 */

typedef	struct __tmd_objtable {
	long	vert_top;
	long	n_vert;
	long	normal_top;
	long	n_normal;
	long	primitive_top;
	long	n_primitive;
	long	scale;
} TMDOBJC;

/*
 * Vertex list
 * ----------------------------------------------------------------------------
 */

typedef struct __tmd_vertex {
	short	vx;
	short	vy;
	short	vz;
	short	none;
} TMDVERT;

/*
 * Normal List
 * ----------------------------------------------------------------------------
 */

typedef struct __tmd_normal {
	short	nx;
	short	ny;
	short	nz;
	short	none;
} TMDNORM;

/*
 * First four bytes of all primitive types are same
 * Macros for the primitive type in a TMD file
 * ----------------------------------------------------------------------------
 */

#define	TMD_PRIMTYPE_MASK	0xFD05

typedef struct __tmdprim {
	unsigned char	olen;
	unsigned char	ilen;
	unsigned short	mode;
} TMDPRIM;

/*
 * TMD primitives
 * ----------------------------------------------------------------------------
 */

/* 3 sided polygon - Flat with Solid Color, No Texture, Light Source On */

#define	TMD_PRIMTYPE_FL3	0x2000

typedef struct __tmdprim_fl3 {
	unsigned char	olen;
	unsigned char	ilen;
	unsigned short	mode;
	unsigned char	r;
	unsigned char	g;
	unsigned char	b;
	unsigned char	note;
	unsigned short	normal0;
	unsigned short	vertex0;
	unsigned short	vertex1;
	unsigned short	vertex2;
} TMDPRIM_FL3;

/* 3 sided polygon - Flat with Gradation, No Texture, Light Source On */

#define	TMD_PRIMTYPE_FLD3	0x2004

typedef struct __tmdprim_fld3 {
	unsigned char	olen;
	unsigned char	ilen;
	unsigned short	mode;
	unsigned char	r0;
	unsigned char	g0;
	unsigned char	b0;
	unsigned char	note;
	unsigned char	r1;
	unsigned char	g1;
	unsigned char	b1;
	unsigned char	rgbnone1;
	unsigned char	r2;
	unsigned char	g2;
	unsigned char	b2;
	unsigned char	rgbnone2;
	unsigned short	normal0;
	unsigned short	vertex0;
	unsigned short	vertex1;
	unsigned short	vertex2;
} TMDPRIM_FLD3;

/* 3 sided polygon - Flat, Texture, Light Source On */

#define	TMD_PRIMTYPE_FLT3	0x2400

typedef struct __tmdprim_flt3 {
	unsigned char	olen;
	unsigned char	ilen;
	unsigned short	mode;
	unsigned char	u0;
	unsigned char	v0;
	unsigned short	cba;
	unsigned char	u1;
	unsigned char	v1;
	unsigned short	tsb;
	unsigned char	u2;
	unsigned char	v2;
	unsigned short	uvnone2;
	unsigned short	normal0;
	unsigned short	vertex0;
	unsigned short	vertex1;
	unsigned short	vertex2;
} TMDPRIM_FLT3;

/* 3 sided polygon - Flat, No Texture, Light Source Off */

#define	TMD_PRIMTYPE_F3		0x2101

typedef struct __tmdprim_f3 {
	unsigned char	olen;
	unsigned char	ilen;
	unsigned short	mode;
	unsigned char	r;
	unsigned char	g;
	unsigned char	b;
	unsigned char	note;
	unsigned short	vertex0;
	unsigned short	vertex1;
	unsigned short	vertex2;
	unsigned short	vertexnone;
} TMDPRIM_F3;

/* 3 sided polygon - Flat, Texture, Light Source Off */

#define	TMD_PRIMTYPE_FT3	0x2501

typedef struct __tmdprim_ft3 {
	unsigned char	olen;
	unsigned char	ilen;
	unsigned short	mode;
	unsigned char	u0;
	unsigned char	v0;
	unsigned short	cba;
	unsigned char	u1;
	unsigned char	v1;
	unsigned short	tsb;
	unsigned char	u2;
	unsigned char	v2;
	unsigned short	uvnone2;
	unsigned char	r;
	unsigned char	g;
	unsigned char	b;
	unsigned char	rgbnone;
	unsigned short	vertex0;
	unsigned short	vertex1;
	unsigned short	vertex2;
	unsigned short	vertexnone;
} TMDPRIM_FT3;

/* 3 sided polygon - Gouraud with Solid Color, No Texture, Light Source On */

#define	TMD_PRIMTYPE_GL3	0x3000

typedef struct __tmdprim_gl3 {
	unsigned char	olen;
	unsigned char	ilen;
	unsigned short	mode;
	unsigned char	r;
	unsigned char	g;
	unsigned char	b;
	unsigned char	note;
	unsigned short	normal0;
	unsigned short	vertex0;
	unsigned short	normal1;
	unsigned short	vertex1;
	unsigned short	normal2;
	unsigned short	vertex2;
} TMDPRIM_GL3;

/* 3 sided polygon - Gouraud with Gradation, No Texture, Light Source On */

#define	TMD_PRIMTYPE_GLD3  	0x3004

typedef struct __tmdprim_gld3 {
	unsigned char	olen;
	unsigned char	ilen;
	unsigned short	mode;
	unsigned char	r0;
	unsigned char	g0;
	unsigned char	b0;
	unsigned char	note;
	unsigned char	r1;
	unsigned char	g1;
	unsigned char	b1;
	unsigned char	rgbnone1;
	unsigned char	r2;
	unsigned char	g2;
	unsigned char	b2;
	unsigned char	rgbnone2;
	unsigned short	normal0;
	unsigned short	vertex0;
	unsigned short	normal1;
	unsigned short	vertex1;
	unsigned short	normal2;
	unsigned short	vertex2;
} TMDPRIM_GLD3;

/* 3 sided polygon - Gouraud, Texture, Light Source On */

#define	TMD_PRIMTYPE_GLT3	0x3400

typedef struct __tmdprim_glt3 {
	unsigned char	olen;
	unsigned char	ilen;
	unsigned short	mode;
	unsigned char	u0;
	unsigned char	v0;
	unsigned short	cba;
	unsigned char	u1;
	unsigned char	v1;
	unsigned short	tsb;
	unsigned char	u2;
	unsigned char	v2;
	unsigned short	uvnone2;
	unsigned short	normal0;
	unsigned short	vertex0;
	unsigned short	normal1;
	unsigned short	vertex1;
	unsigned short	normal2;
	unsigned short	vertex2;
} TMDPRIM_GLT3;

/* 3 sided polygon - Gouraud, No Texture, Light Source Off */

#define	TMD_PRIMTYPE_G3		0x3101

typedef struct __tmdprim_g3 {
	unsigned char	olen;
	unsigned char	ilen;
	unsigned short	mode;
	unsigned char	r0;
	unsigned char	g0;
	unsigned char	b0;
	unsigned char	note;
	unsigned char	r1;
	unsigned char	g1;
	unsigned char	b1;
	unsigned char	rgbnone1;
	unsigned char	r2;
	unsigned char	g2;
	unsigned char	b2;
	unsigned char	rgbnone2;
	unsigned short	vertex0;
	unsigned short	vertex1;
	unsigned short	vertex2;
	unsigned short	vertexnone;
} TMDPRIM_G3;

/* 3 sided polygon - Gouraud, Texture, Light Source Off */

#define	TMD_PRIMTYPE_GT3	0x3501

typedef struct __tmdprim_gt3 {
	unsigned char	olen;
	unsigned char	ilen;
	unsigned short	mode;
	unsigned char	u0;
	unsigned char	v0;
	unsigned short	cba;
	unsigned char	u1;
	unsigned char	v1;
	unsigned short	tsb;
	unsigned char	u2;
	unsigned char	v2;
	unsigned short	uvnone2;
	unsigned char	r0;
	unsigned char	g0;
	unsigned char	b0;
	unsigned char	rgbnone0;
	unsigned char	r1;
	unsigned char	g1;
	unsigned char	b1;
	unsigned char	rgbnone1;
	unsigned char	r2;
	unsigned char	g2;
	unsigned char	b2;
	unsigned char	rgbnone2;
	unsigned short	vertex0;
	unsigned short	vertex1;
	unsigned short	vertex2;
	unsigned short	vertexnone;
} TMDPRIM_GT3;

/* 4 sided polygon - Flat with Solid Color, No Texture, Light Source On */

#define	TMD_PRIMTYPE_FL4 	0x2800

typedef struct __tmdprim_fl4 {
	unsigned char	olen;
	unsigned char	ilen;
	unsigned short	mode;
	unsigned char	r;
	unsigned char	g;
	unsigned char	b;
	unsigned char	note;
	unsigned short	normal0;
	unsigned short	vertex0;
	unsigned short	vertex1;
	unsigned short	vertex2;
	unsigned short	vertex3;
	unsigned short	vertexnone;
} TMDPRIM_FL4;

/* 4 sided polygon - Flat with Gradation, No Texture, Light Source On */

#define	TMD_PRIMTYPE_FLD4	0x2804

typedef struct __tmdprim_fld4 {
	unsigned char	olen;
	unsigned char	ilen;
	unsigned short	mode;
	unsigned char	r0;
	unsigned char	g0;
	unsigned char	b0;
	unsigned char	note;
	unsigned char	r1;
	unsigned char	g1;
	unsigned char	b1;
	unsigned char	rgbnone1;
	unsigned char	r2;
	unsigned char	g2;
	unsigned char	b2;
	unsigned char	rgbnone2;
	unsigned char	r3;
	unsigned char	g3;
	unsigned char	b3;
	unsigned char	rgbnone3;
	unsigned short	normal0;
	unsigned short	vertex0;
	unsigned short	vertex1;
	unsigned short	vertex2;
	unsigned short	vertex3;
	unsigned short	vertexnone;
} TMDPRIM_FLD4;

/* 4 sided polygon - Flat, Texture, Light Source On */

#define	TMD_PRIMTYPE_FLT4	0x2c00

typedef struct __tmdprim_flt4 {
	unsigned char	olen;
	unsigned char	ilen;
	unsigned short	mode;
	unsigned char	u0;
	unsigned char	v0;
	unsigned short	cba;
	unsigned char	u1;
	unsigned char	v1;
	unsigned short	tsb;
	unsigned char	u2;
	unsigned char	v2;
	unsigned short	uvnone2;
	unsigned char	u3;
	unsigned char	v3;
	unsigned short	uvnone3;
	unsigned short	normal0;
	unsigned short	vertex0;
	unsigned short	vertex1;
	unsigned short	vertex2;
	unsigned short	vertex3;
	unsigned short	vertexnone;
} TMDPRIM_FLT4;

/* 4 sided polygon - Flat, No Texture, Light Source Off */

#define	TMD_PRIMTYPE_F4		0x2901

typedef struct __tmdprim_f4 {
	unsigned char	olen;
	unsigned char	ilen;
	unsigned short	mode;
	unsigned char	r;
	unsigned char	g;
	unsigned char	b;
	unsigned char	note;
	unsigned short	vertex0;
	unsigned short	vertex1;
	unsigned short	vertex2;
	unsigned short	vertex3;
} TMDPRIM_F4;

/* 4 sided polygon - Flat, Texture, Light Source On */

#define	TMD_PRIMTYPE_FT4	0x2d01

typedef struct __tmdprim_ft4 {
	unsigned char	olen;
	unsigned char	ilen;
	unsigned short	mode;
	unsigned char	u0;
	unsigned char	v0;
	unsigned short	cba;
	unsigned char	u1;
	unsigned char	v1;
	unsigned short	tsb;
	unsigned char	u2;
	unsigned char	v2;
	unsigned short	uvnone2;
	unsigned char	u3;
	unsigned char	v3;
	unsigned short	uvnone3;
	unsigned char	r;
	unsigned char	g;
	unsigned char	b;
	unsigned char	rgbnone;
	unsigned short	vertex0;
	unsigned short	vertex1;
	unsigned short	vertex2;
	unsigned short	vertex3;
} TMDPRIM_FT4;

/* 4 sided polygon - Gouraud with Solid Color, No Texture, Light Source On */

#define	TMD_PRIMTYPE_GL4	0x3800

typedef struct __tmdprim_gl4 {
	unsigned char	olen;
	unsigned char	ilen;
	unsigned short	mode;
	unsigned char	r;
	unsigned char	g;
	unsigned char	b;
	unsigned char	note;
	unsigned short	normal0;
	unsigned short	vertex0;
	unsigned short	normal1;
	unsigned short	vertex1;
	unsigned short	normal2;
	unsigned short	vertex2;
	unsigned short	normal3;
	unsigned short	vertex3;
} TMDPRIM_GL4;

/* 4 sided polygon - Gouraud with Gradation, No Texture, Light Source On */

#define	TMD_PRIMTYPE_GLD4	0x3804

typedef struct __tmdprim_gld4 {
	unsigned char	olen;
	unsigned char	ilen;
	unsigned short	mode;
	unsigned char	r0;
	unsigned char	g0;
	unsigned char	b0;
	unsigned char	note;
	unsigned char	r1;
	unsigned char	g1;
	unsigned char	b1;
	unsigned char	rgbnone1;
	unsigned char	r2;
	unsigned char	g2;
	unsigned char	b2;
	unsigned char	rgbnone2;
	unsigned char	r3;
	unsigned char	g3;
	unsigned char	b3;
	unsigned char	rgbnone3;
	unsigned short	normal0;
	unsigned short	vertex0;
	unsigned short	normal1;
	unsigned short	vertex1;
	unsigned short	normal2;
	unsigned short	vertex2;
	unsigned short	normal3;
	unsigned short	vertex3;
} TMDPRIM_GLD4;

/* 4 sided polygon - Gouraud, Texture, Light Source On */

#define	TMD_PRIMTYPE_GLT4	0x3c00

typedef struct __tmdprim_glt4 {
	unsigned char	olen;
	unsigned char	ilen;
	unsigned short	mode;
	unsigned char	u0;
	unsigned char	v0;
	unsigned short	cba;
	unsigned char	u1;
	unsigned char	v1;
	unsigned short	tsb;
	unsigned char	u2;
	unsigned char	v2;
	unsigned short	uvnone2;
	unsigned char	u3;
	unsigned char	v3;
	unsigned short	uvnone3;
	unsigned short	normal0;
	unsigned short	vertex0;
	unsigned short	normal1;
	unsigned short	vertex1;
	unsigned short	normal2;
	unsigned short	vertex2;
	unsigned short	normal3;
	unsigned short	vertex3;
} TMDPRIM_GLT4;

/* 4 sided polygon - Gouraud, No Texture, Light Source Off */

#define	TMD_PRIMTYPE_G4		0x3901

typedef struct __tmdprim_g4 {
	unsigned char	olen;
	unsigned char	ilen;
	unsigned short	mode;
	unsigned char	r0;
	unsigned char	g0;
	unsigned char	b0;
	unsigned char	note;
	unsigned char	r1;
	unsigned char	g1;
	unsigned char	b1;
	unsigned char	rgbnone1;
	unsigned char	r2;
	unsigned char	g2;
	unsigned char	b2;
	unsigned char	rgbnone2;
	unsigned char	r3;
	unsigned char	g3;
	unsigned char	b3;
	unsigned char	rgbnone3;
	unsigned short	vertex0;
	unsigned short	vertex1;
	unsigned short	vertex2;
	unsigned short	vertex3;
} TMDPRIM_G4;

/* 4 sided polygon - Gouraud, Texture, Light Source Off */

#define	TMD_PRIMTYPE_GT4	0x3d01

typedef struct __tmdprim_gt4 {
	unsigned char	olen;
	unsigned char	ilen;
	unsigned short	mode;
	unsigned char	u0;
	unsigned char	v0;
	unsigned short	cba;
	unsigned char	u1;
	unsigned char	v1;
	unsigned short	tsb;
	unsigned char	u2;
	unsigned char	v2;
	unsigned short	uvnone2;
	unsigned char	u3;
	unsigned char	v3;
	unsigned short	uvnone3;
	unsigned char	r0;
	unsigned char	g0;
	unsigned char	b0;
	unsigned char	rgbnone0;
	unsigned char	r1;
	unsigned char	g1;
	unsigned char	b1;
	unsigned char	rgbnone1;
	unsigned char	r2;
	unsigned char	g2;
	unsigned char	b2;
	unsigned char	rgbnone2;
	unsigned char	r3;
	unsigned char	g3;
	unsigned char	b3;
	unsigned char	rgbnone3;
	unsigned short	vertex0;
	unsigned short	vertex1;
	unsigned short	vertex2;
	unsigned short	vertex3;
} TMDPRIM_GT4;

/* Line - No Gradation */

#define	TMD_PRIMTYPE_L		0x4001

typedef struct __tmdprim_l {
	unsigned char	olen;
	unsigned char	ilen;
	unsigned short	mode;
	unsigned char	r;
	unsigned char	g;
	unsigned char	b;
	unsigned char	note;
	unsigned short	vertex0;
	unsigned short	vertex1;
} TMDPRIM_L;

/* Line - Gradation */

#define	TMD_PRIMTYPE_LD		0x5001

typedef struct __tmdprim_ld {
	unsigned char	olen;
	unsigned char	ilen;
	unsigned short	mode;
	unsigned char	r0;
	unsigned char	g0;
	unsigned char	b0;
	unsigned char	note;
	unsigned char	r1;
	unsigned char	g1;
	unsigned char	b1;
	unsigned char	rgbnone1;
	unsigned short	vertex0;
	unsigned short	vertex1;
} TMDPRIM_LD;

/* Sprites */

#define	TMD_PRIMTYPE_S1		0x6C01
#define	TMD_PRIMTYPE_S8		0x7401
#define	TMD_PRIMTYPE_S16	0x7C01

typedef struct __tmdprim_s {
	unsigned char	olen;
	unsigned char	ilen;
	unsigned short	mode;
	unsigned short	vertex0;
	unsigned short	tsb;
	unsigned char	u0;
	unsigned char	v0;
	unsigned short	cba;
} TMDPRIM_S;

#define	TMD_PRIMTYPE_SF		0x6401

typedef struct __tmdprim_sf {
	unsigned char	olen;
	unsigned char	ilen;
	unsigned short	mode;
	unsigned short	vertex0;
	unsigned short	tsb;
	unsigned char	u0;
	unsigned char	v0;
	unsigned short	cba;
	unsigned short	w;
	unsigned short	h;
} TMDPRIM_SF;

/*
 * The following defines the data structure for a self contained TMD
 * with one primitive of each of the above described primitive type.
 * ----------------------------------------------------------------------------
 */

typedef struct __tmd_mine {
	TMDHEAD		head;
	TMDOBJC		object;
	char		pack[ sizeof(TMDPRIM_GT4) ];
	TMDVERT		vert[4];
	TMDNORM		norm[4];
} TMDMINE;

/*
 * End of File
 * ----------------------------------------------------------------------------
 */

#endif
