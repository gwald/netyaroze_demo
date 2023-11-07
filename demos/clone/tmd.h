/***************************************************************
*                                                              *
* Copyright (C) 1997 by Sony Computer Entertainment            *
*                       All rights Reserved                    *
*                                                              *
* S. Ashley 9th Jan 97                                         *
*                                                              *
***************************************************************/

#ifndef _tmd_h

#define _tmd_h

#include <libps.h>

// for decoding primative mode
#define IIP  0x10		 // gouraud shading
#define QUAD 0x08		 // 4 sided
#define TME  0x04		 // textured
#define ABE  0x02		 // semi transparency on
#define TGE  0x01		 // brightness calculation off

// flags for primatives 
#define GRD  0x04  // gradation
#define FCE  0x02  // 2 sided
#define LGT  0x01  // no light source calculation

// primative modes
#define F_3      0x20  // Triangle  No-texture 
#define F_3G     0x20  // Triangle  No-texture gradation
#define F_3T     0x24  // Triangle  Texture    
#define G_3      0x30  // triangle gouraud
#define G_3G     0x30  // triangle gouraud gradation
#define G_3T     0x34  // triangle gouraud texture
#define F_3_NL   0x21  // triangle no texture no light
#define F_3T_NL	 0x25  // triangle texture no light
#define G_3_NL	 0x31  // triangle gouraud no texture no light
#define G_3T_NL  0x35  // triangle gouraud texture no light

#define F_4      0x28  // Quadangle No-texture 
#define F_4G     0x28  // Quadangle No-texture gradation
#define F_4T     0x2c  // Quadangle Texture    
#define G_4      0x38  // quad gouraud
#define G_4G     0x38  // quad gouraud gradation
#define G_4T     0x3c  // quad gouraud texture
#define F_4_NL   0x29  // quad no texture no light
#define F_4T_NL	 0x2d  // quad texture no light
#define G_4_NL	 0x39  // quad gouraud no texture no light
#define G_4T_NL  0x3d  // quad gouraud texture no light

#define SL       0x40  // straight line no gradation
#define SL_G	 0x50  // straight line gradation

// TMD is made up of header, object tables (one for each object), 
// primative table, vertices table and normals table

typedef struct
	{
	u_long id;		// 0x00000041
	u_long flag;	// 1 if addresses real, 0 if offset
	u_long nobjs;	// number of objects in TMD
	} TMD_HDR;

#define setTMD_HDR(t,_i,_f,_n) \
	(t)->id = (_i), (t)->flag = (_f), (t)->nobjs = (_n)

typedef struct 
	{
	u_long *vert_top;	// vertices table
	u_long n_vert;		// number of vertices
	u_long *norm_top;	// normal table
	u_long n_norm;		// number of normals
	u_long *prim_top;	// primatives table
	u_long n_prim;		// number of primatives
	long   scale;		// not used
	} TMD_OBJ;

#define setTMD_OBJ(t,_vt,_vn,_nt,_nn,_pt,_pn,_s) \
	(t)->vert_top = (_vt), (t)->n_vert = (_vn),	\
	(t)->norm_top = (_nt), (t)->n_norm = (_nn),	\
	(t)->prim_top = (_pt), (t)->n_prim = (_pn),	\
	(t)->scale = (_s)

// primative table is made up of a collection of primatives headers, with there
// primative data

typedef struct
	{
	u_char olen, ilen, flag, mode;
	} PRIM_HDR;

#define setPRIM_HDR(p,_m,_f,_i,_o) \
	(p)->mode = (_m), (p)->flag = (_f), (p)->ilen = (_i), (p)->olen = (_o)

typedef struct 
	{
	short vx, vy;
	short vz, pad;
	} TMD_VERT;

#define setTMD_VERT(t,_x,_y,_z) \
	(t)->vx = (_x), (t)->vy = (_y), (t)->vz = (_z)

typedef struct 
	{
	short nx, ny;
	short nz, pad;
	} TMD_NORM;

#define setTMD_NORM(t,_x,_y,_z) \
	(t)->nx = (_x), (t)->ny = (_y), (t)->nz = (_z)

// normals and vertices are indices for the normal and vertex tables
// r0, g0, b0 are colour values
// mode2 is just a repetition of the mode data used as a filler
typedef struct 			// ilen 3 olen 4
	{
	u_char r0, g0, b0, mode2;
	u_short norm0, vert0;
	u_short vert1, vert2;
	} TMD_F_3;

#define setTMD_F_3(t,_r,_g,_b,_norm,_vert0,_vert1,_vert2) \
	(t)->r0 = (_r), (t)->g0 = (_g), (t)->b0 = (_b), \
	(t)->norm0 = (_norm), \
	(t)->vert0 = (_vert0), (t)->vert1 = (_vert1), (t)->vert2 = (_vert2)

// pad0, pad1 are just padding
typedef struct 			// ilen 5 olen 6
	{
	u_char r0, g0, b0, mode2;
	u_char r1, g1, b1, pad0;
	u_char r2, g2, b2, pad1;
	u_short norm0, vert0;
	u_short vert1, vert2;
	} TMD_F_3G;

#define setTMD_F_3G(t,_r0,_g0,_b0,_r1,_g1,_b1,_r2,_g2,_b2,_norm,_vert0,_vert1,_vert2) \
	(t)->r0 = (_r0), (t)->g0 = (_g0), (t)->b0 = (_b0), \
	(t)->r1 = (_r1), (t)->g1 = (_g1), (t)->b1 = (_b1), \
	(t)->r2 = (_r2), (t)->g2 = (_g2), (t)->b2 = (_b2), \
	(t)->norm0 = (_norm), \
	(t)->vert0 = (_vert0), (t)->vert1 = (_vert1), (t)->vert2 = (_vert2)


// CBA clutY * 64 + clutX / 16
// TSB texture page + semitrans rate (0..3) << 5 + colour mode (0..2) << 7
// u, v are texture page coordinates
typedef struct  		// ilen 5 olen 7
	{
	u_char u0, v0; u_short cba;
	u_char u1, v1; u_short tsb;
	u_char u2, v2; u_short pad;
	u_short norm0, vert0;
	u_short vert1, vert2;
	} TMD_F_3T;

#define setTMD_F_3T(t,_cba,_tsb,_u0,_v0,_u1,_v1,_u2,_v2,_norm,_vert0,_vert1,_vert2) \
	(t)->cba = (_cba), (t)->tsb = (_tsb), \
	(t)->u0 = (_u0), (t)->v0 = (_v0), \
	(t)->u1 = (_u1), (t)->v1 = (_v1), \
	(t)->u2 = (_u2), (t)->v2 = (_v2), \
	(t)->norm0 = (_norm), \
	(t)->vert0 = (_vert0), (t)->vert1 = (_vert1), (t)->vert2 = (_vert2)

typedef struct 			// ilen 4 olen 5
	{
	u_char r0, g0, b0, mode2;
	u_short norm0, vert0;
	u_short vert1, vert2;
	u_short vert3, pad;
	} TMD_F_4;

#define setTMD_F_4(t,_r,_g,_b,_norm,_vert0,_vert1,_vert2,_vert3) \
	(t)->r0 = (_r), (t)->g0 = (_g), (t)->b0 = (_b), \
	(t)->norm0 = (_norm), \
	(t)->vert0 = (_vert0), (t)->vert1 = (_vert1), \
	(t)->vert2 = (_vert2), (t)->vert3 = (_vert3)

typedef struct 			// ilen 7 olen 8
	{
	u_char r0, g0, b0, mode2;
	u_char r1, g1, b1, pad0;
	u_char r2, g2, b2, pad1;
	u_char r3, g3, b3, pad2;
	u_short norm0, vert0;
	u_short vert1, vert2;
	u_short vert3, pad3;
	} TMD_F_4G;

#define setTMD_F_4G(t,_r0,_g0,_b0,_r1,_g1,_b1,_r2,_g2,_b2,_r3,_g3,_b3,_norm,_vert0,_vert1,_vert2,_vert3) \
	(t)->r0 = (_r0), (t)->g0 = (_g0), (t)->b0 = (_b0), \
	(t)->r1 = (_r1), (t)->g1 = (_g1), (t)->b1 = (_b1), \
	(t)->r2 = (_r2), (t)->g2 = (_g2), (t)->b2 = (_b2), \
	(t)->r3 = (_r3), (t)->g3 = (_g3), (t)->b3 = (_b3), \
	(t)->norm0 = (_norm), \
	(t)->vert0 = (_vert0), (t)->vert1 = (_vert1), \
	(t)->vert2 = (_vert2), (t)->vert3 = (_vert3)

typedef struct 			// ilen 7 olen 9
	{
	u_char u0, v0; u_short cba;
	u_char u1, v1; u_short tsb;
	u_char u2, v2; u_short pad0;
	u_char u3, v3; u_short pad1;
	u_short norm0, vert0;
	u_short vert1, vert2;
	u_short vert3, pad2;
	} TMD_F_4T;

#define setTMD_F_4T(t,_cba,_tsb,_u0,_v0,_u1,_v1,_u2,_v2,_u3,_v3,_norm,_vert0,_vert1,_vert2,_vert3) \
	(t)->cba = (_cba), (t)->tsb = (_tsb), \
	(t)->u0 = (_u0), (t)->v0 = (_v0), \
	(t)->u1 = (_u1), (t)->v1 = (_v1), \
	(t)->u2 = (_u2), (t)->v2 = (_v2), \
	(t)->u3 = (_u3), (t)->v3 = (_v3), \
	(t)->norm0 = (_norm), \
	(t)->vert0 = (_vert0), (t)->vert1 = (_vert1), \
	(t)->vert2 = (_vert2), (t)->vert3 = (_vert3)

typedef struct  			// ilen 4 olen 6
	{
	u_char r0, g0, b0, mode2;
	u_short norm0, vert0;
	u_short norm1, vert1;
	u_short norm2, vert2;
	} TMD_G_3;

#define setTMD_G_3(t,_r,_g,_b,_norm0,_norm1,_norm2,_vert0,_vert1,_vert2) \
	(t)->r0 = (_r), (t)->g0 = (_g), (t)->b0 = (_b), \
	(t)->norm0 = (_norm0), (t)->norm1 = (_norm1), (t)->norm2 = (_norm2), \
	(t)->vert0 = (_vert0), (t)->vert1 = (_vert1), (t)->vert2 = (_vert2)

typedef struct 			// ilen 6 olen 6
	{
	u_char r0, g0, b0, mode2;
	u_char r1, g1, b1, pad0;
	u_char r2, g2, b2, pad1;
	u_short norm0, vert0;
	u_short norm1, vert1;
	u_short norm2, vert2;
	} TMD_G_3G;

#define setTMD_G_3G(t,_r0,_g0,_b0,_r1,_g1,_b1,_r2,_g2,_b2,_norm0,_norm1,_norm2,_vert0,_vert1,_vert2) \
	(t)->r0 = (_r0), (t)->g0 = (_g0), (t)->b0 = (_b0), \
	(t)->r1 = (_r1), (t)->g1 = (_g1), (t)->b1 = (_b1), \
	(t)->r2 = (_r2), (t)->g2 = (_g2), (t)->b2 = (_b2), \
	(t)->norm0 = (_norm0), (t)->norm1 = (_norm1), (t)->norm2 = (_norm2), \
	(t)->vert0 = (_vert0), (t)->vert1 = (_vert1), (t)->vert2 = (_vert2)

typedef struct 			// ilen 6 olen 9
	{
	u_char u0, v0; u_short cba;
	u_char u1, v1; u_short tsb;
	u_char u2, v2; u_short pad;
	u_short norm0, vert0;
	u_short norm1, vert1;
	u_short norm2, vert2;
	} TMD_G_3T;

#define setTMD_G_3T(t,_cba,_tsb,_u0,_v0,_u1,_v1,_u2,_v2,_norm0,_norm1,_norm2,_vert0,_vert1,_vert2) \
	(t)->cba = (_cba), (t)->tsb = (_tsb), \
	(t)->u0 = (_u0), (t)->v0 = (_v0), \
	(t)->u1 = (_u1), (t)->v1 = (_v1), \
	(t)->u2 = (_u2), (t)->v2 = (_v2), \
	(t)->norm0 = (_norm0), (t)->norm0 = (_norm1),	(t)->norm0 = (_norm2), \
	(t)->vert0 = (_vert0), (t)->vert1 = (_vert1),	(t)->vert2 = (_vert2)

typedef struct 			// ilen 5 olen 8
	{
	u_char r0, g0, b0, mode2;
	u_short norm0, vert0;
	u_short norm1, vert1;
	u_short norm2, vert2;
	u_short norm3, vert3;
	} TMD_G_4;

#define setTMD_G_4(t,_r,_g,_b,_norm0,_norm1,_norm2,_norm3,_vert0,_vert1,_vert2,_vert3) \
	(t)->r0 = (_r), (t)->g0 = (_g), (t)->b0 = (_b), \
	(t)->norm0 = (_norm0), (t)->norm1 = (_norm1), \
	(t)->norm2 = (_norm2), (t)->norm3 = (_norm3), \
	(t)->vert0 = (_vert0), (t)->vert1 = (_vert1), \
	(t)->vert2 = (_vert2), (t)->vert3 = (_vert3)

typedef struct 			// ilen 8 olen 8
	{
	u_char r0, g0, b0, mode2;
	u_char r1, g1, b1, pad0;
	u_char r2, g2, b2, pad1;
	u_char r3, g3, b3, pad2;
	u_short norm0, vert0;
	u_short norm1, vert1;
	u_short norm2, vert2;
	u_short norm3, vert3;
	} TMD_G_4G;

#define setTMD_G_4G(t,_r0,_g0,_b0,_r1,_g1,_b1,_r2,_g2,_b2,_r3,_g3,_b3,_norm0,_norm1,_norm2,_norm3,_vert0,_vert1,_vert2,_vert3) \
	(t)->r0 = (_r0), (t)->g0 = (_g0), (t)->b0 = (_b0), \
	(t)->r1 = (_r1), (t)->g1 = (_g1), (t)->b1 = (_b1), \
	(t)->r2 = (_r2), (t)->g2 = (_g2), (t)->b2 = (_b2), \
	(t)->r3 = (_r3), (t)->g3 = (_g3), (t)->b3 = (_b3), \
	(t)->norm0 = (_norm0), (t)->norm1 = (_norm1), \
	(t)->norm2 = (_norm2), (t)->norm3 = (_norm3), \
	(t)->vert0 = (_vert0), (t)->vert1 = (_vert1), \
	(t)->vert2 = (_vert2), (t)->vert3 = (_vert3)

typedef struct 			// ilen 8 olen 12
	{
	u_char u0, v0; u_short cba;
	u_char u1, v1; u_short tsb;
	u_char u2, v2; u_short pad0;
	u_char u3, v3; u_short pad1;
	u_short norm0, vert0;
	u_short norm1, vert1;
	u_short norm2, vert2;
	u_short norm3, vert3;
	} TMD_G_4T;

#define setTMD_G_4T(t,_cba,_tsb,_u0,_v0,_u1,_v1,_u2,_v2,_u3,_v3,_norm0,_norm1,_norm2,_norm3,_vert0,_vert1,_vert2,_vert3) \
	(t)->cba = (_cba), (t)->tsb = (_tsb), \
	(t)->u0 = (_u0), (t)->v0 = (_v0), \
	(t)->u1 = (_u1), (t)->v1 = (_v1), \
	(t)->u2 = (_u2), (t)->v2 = (_v2), \
	(t)->u3 = (_u3), (t)->v3 = (_v3), \
	(t)->norm0 = (_norm0), (t)->norm1 = (_norm1), \
	(t)->norm2 = (_norm2), (t)->norm3 = (_norm3), \
	(t)->vert0 = (_vert0), (t)->vert1 = (_vert1), \
	(t)->vert2 = (_vert2), (t)->vert3 = (_vert3)

typedef struct 			// ilen 3 olen 4
	{
	u_char r0, g0, b0, mode2;
	u_short vert0, vert1;
	u_short vert2, pad;
	} TMD_F_3_NL;

#define setTMD_F_3_NL(t,_r,_g,_b,_vert0,_vert1,_vert2) \
	(t)->r0 = (_r), (t)->g0 = (_g), (t)->b0 = (_b), \
	(t)->vert0 = (_vert0), (t)->vert1 = (_vert1), (t)->vert2 = (_vert2)

typedef struct 			// ilen 6 olen 7
	{
	u_char u0, v0; u_short cba;
	u_char u1, v1; u_short tsb;
	u_char u2, v2; u_short pad0;
	u_char r0, g0, b0, pad1;
	u_short vert0, vert1;
	u_short vert2, pad2;
	} TMD_F_3T_NL;

#define setTMD_F_3T_NL(t,_cba,_tsb,_u0,_v0,_u1,_v1,_u2,_v2,_r,_g,_b,_vert0,_vert1,_vert2) \
	(t)->cba = (_cba), (t)->tsb = (_tsb), \
	(t)->u0 = (_u0), (t)->v0 = (_v0), \
	(t)->u1 = (_u1), (t)->v1 = (_v1), \
	(t)->u2 = (_u2), (t)->v2 = (_v2), \
	(t)->r0 = (_r), (t)->g0 = (_g), (t)->b0 = (_b), \
	(t)->vert0 = (_vert0), (t)->vert1 = (_vert1), (t)->vert2 = (_vert2)

typedef struct 			// ilen 5 olen 6
	{
	u_char r0, g0, b0, mode2;
	u_char r1, g1, b1, pad0;
	u_char r2, g2, b2, pad1;
	u_short vert0, vert1;
	u_short vert2, pad2;
	} TMD_G_3_NL;

#define setTMD_G_3_NL(t,_r0,_g0,_b0,_r1,_g1,_b1,_r2,_g2,_b2,_vert0,_vert1,_vert2) \
	(t)->r0 = (_r0), (t)->g0 = (_g0), (t)->b0 = (_b0), \
	(t)->r1 = (_r1), (t)->g1 = (_g1), (t)->b1 = (_b1), \
	(t)->r2 = (_r2), (t)->g2 = (_g2), (t)->b2 = (_b2), \
	(t)->vert0 = (_vert0), (t)->vert1 = (_vert1), (t)->vert2 = (_vert2)

typedef struct 			// ilen 8 olen 9
	{
	u_char u0, v0; u_short cba;
	u_char u1, v1; u_short tsb;
	u_char u2, v2; u_short pad0;
	u_char r0, g0, b0, pad1;
	u_char r1, g1, b1, pad2;
	u_char r2, g2, b2, pad3;
	u_short vert0, vert1;
	u_short vert2, pad4;
	} TMD_G_3T_NL;

#define setTMD_G_3T_NL(t,_cba,_tsb,_u0,_v0,_u1,_v1,_u2,_v2,_r0,_g0,_b0,_r1,_g1,_b1,_r2,_g2,_b2,_vert0,_vert1,_vert2) \
	(t)->cba = (_cba), (t)->tsb = (_tsb), \
	(t)->u0 = (_u0), (t)->v0 = (_v0), \
	(t)->u1 = (_u1), (t)->v1 = (_v1), \
	(t)->u2 = (_u2), (t)->v2 = (_v2), \
	(t)->r0 = (_r0), (t)->g0 = (_g0), (t)->b0 = (_b0), \
	(t)->r1 = (_r1), (t)->g1 = (_g1), (t)->b1 = (_b1), \
	(t)->r2 = (_r2), (t)->g2 = (_g2), (t)->b2 = (_b2), \
	(t)->vert0 = (_vert0), (t)->vert1 = (_vert1), (t)->vert2 = (_vert2)

typedef struct 			// ilen 3 olen 5
	{
	u_char r0, g0, b0, mode2;
	u_short vert0, vert1;
	u_short vert2, vert3;
	} TMD_F_4_NL;

#define setTMD_F_4_NL(t,_r,_g,_b,_vert0,_vert1,_vert2,_vert3) \
	(t)->r0 = (_r), (t)->g0 = (_g), (t)->b0 = (_b), \
	(t)->vert0 = (_vert0), (t)->vert1 = (_vert1), \
	(t)->vert2 = (_vert2), (t)->vert3 = (_vert3)

typedef struct 			// ilen 7 olen 9
	{
	u_char u0, v0; u_short cba;
	u_char u1, v1; u_short tsb;
	u_char u2, v2; u_short pad0;
	u_char u3, v3; u_short pad1;
	u_char r0, g0, b0, pad2;
	u_short vert0, vert1;
	u_short vert2, vert3;
	} TMD_F_4T_NL;

#define setTMD_F_4T_NL(t,_cba,_tsb,_u0,_v0,_u1,_v1,_u2,_v2,_u3,_v3,_r,_g,_b,_vert0,_vert1,_vert2,_vert3) \
	(t)->cba = (_cba), (t)->tsb = (_tsb), \
	(t)->u0 = (_u0), (t)->v0 = (_v0), \
	(t)->u1 = (_u1), (t)->v1 = (_v1), \
	(t)->u2 = (_u2), (t)->v2 = (_v2), \
	(t)->u3 = (_u3), (t)->v3 = (_v3), \
	(t)->r0 = (_r), (t)->g0 = (_g), (t)->b0 = (_b), \
	(t)->vert0 = (_vert0), (t)->vert1 = (_vert1), \
	(t)->vert2 = (_vert2), (t)->vert3 = (_vert3)

typedef struct 			// ilen 6 olen 8
	{
	u_char r0, g0, b0, mode2;
	u_char r1, g1, b1, pad0;
	u_char r2, g2, b2, pad1;
	u_char r3, g3, b3, pad2;
	u_short vert0, vert1;
	u_short vert2, vert3;
	} TMD_G_4_NL;

#define setTMD_G_4_NL(t,_r0,_g0,_b0,_r1,_g1,_b1,_r2,_g2,_b2,_r3,_g3,_b3,_vert0,_vert1,_vert2,_vert3) \
	(t)->r0 = (_r0), (t)->g0 = (_g0), (t)->b0 = (_b0), \
	(t)->r1 = (_r1), (t)->g1 = (_g1), (t)->b1 = (_b1), \
	(t)->r2 = (_r2), (t)->g2 = (_g2), (t)->b2 = (_b2), \
	(t)->r3 = (_r3), (t)->g3 = (_g3), (t)->b3 = (_b3), \
	(t)->vert0 = (_vert0), (t)->vert1 = (_vert1), \
	(t)->vert2 = (_vert2), (t)->vert3 = (_vert3)

typedef struct 			// ilen 10 olen 12
	{
	u_char u0, v0; u_short cba;
	u_char u1, v1; u_short tsb;
	u_char u2, v2; u_short pad0;
	u_char u3, v3; u_short pad1;
	u_char r0, g0, b0, pad2;
	u_char r1, g1, b1, pad3;
	u_char r2, g2, b2, pad4;
	u_char r3, g3, b3, pad5;
	u_short vert0, vert1;
	u_short vert2, vert3;
	} TMD_G_4T_NL;

#define setTMD_G_4T_NL(t,_cba,_tsb,_u0,_v0,_u1,_v1,_u2,_v2,_u3,_v3,_r0,_g0,_b0,_r1,_g1,_b1,_r2,_g2,_b2,_r3,_g3,_b3,_vert0,_vert1,_vert2,_vert3) \
	(t)->cba = (_cba), (t)->tsb = (_tsb), \
	(t)->u0 = (_u0), (t)->v0 = (_v0), \
	(t)->u1 = (_u1), (t)->v1 = (_v1), \
	(t)->u2 = (_u2), (t)->v2 = (_v2), \
	(t)->u3 = (_u3), (t)->v3 = (_v3), \
	(t)->r0 = (_r0), (t)->g0 = (_g0), (t)->b0 = (_b0), \
	(t)->r1 = (_r1), (t)->g1 = (_g1), (t)->b1 = (_b1), \
	(t)->r2 = (_r2), (t)->g2 = (_g2), (t)->b2 = (_b2), \
	(t)->r3 = (_r3), (t)->g3 = (_g3), (t)->b3 = (_b3), \
	(t)->vert0 = (_vert0), (t)->vert1 = (_vert1), \
	(t)->vert2 = (_vert2), (t)->vert3 = (_vert3)

typedef struct 			// ilen 2 olen 3
	{
	u_char r0, g0, b0, mode2;
	u_short vert0, vert1;
	} TMD_SL;

#define setTMD_SL(t,_r,_g,_b,_vert0,_vert1) \
	(t)->r0 = (_r), (t)->g0 = (_g), (t)->b0 = (_b), \
	(t)->vert0 = (_vert0), (t)->vert1 = (_vert1)

typedef struct 			// ilen 3 olen 4
	{
	u_char r0, g0, b0, mode2;
	u_char r1, g1, b1, pad;
	u_short vert0, vert1;
	} TMD_SL_G;

#define setTMD_SL_G(t,_r0,_g0,_b0,_r1,_g1,_b1,_vert0,_vert1) \
	(t)->r0 = (_r0), (t)->g0 = (_g0), (t)->b0 = (_b0), \
	(t)->r1 = (_r1), (t)->g1 = (_g1), (t)->b1 = (_b1), \
	(t)->vert0 = (_vert0), (t)->vert1 = (_vert1)

#endif