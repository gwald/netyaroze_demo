/***************************************************************
*                                                              *
* Copyright (C) 1997 by Sony Computer Entertainment            *
*                       All rights Reserved                    *
*                                                              *
* S. Ashley 7th Aug 97                                         *
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
#define F3    0x20  // Triangle  No-texture 
#define FT3   0x24  // Triangle  Texture    
#define G3    0x30  // triangle gouraud
#define GT3   0x34  // triangle gouraud texture
#define F3N   0x21  // triangle no texture no light
#define FT3N  0x25  // triangle texture no light
#define G3N	  0x31  // triangle gouraud no texture no light
#define GT3N  0x35  // triangle gouraud texture no light

#define F4    0x28  // Quadangle No-texture 
#define FT4   0x2c  // Quadangle Texture    
#define G4    0x38  // quad gouraud
#define GT4   0x3c  // quad gouraud texture
#define F4N   0x29  // quad no texture no light
#define FT4N  0x2d  // quad texture no light
#define G4N	  0x39  // quad gouraud no texture no light
#define GT4N  0x3d  // quad gouraud texture no light

#ifndef setRGB0
	#define setRGB0(p, r, g, b) \
		(p)->r0 = (r), (p)->g0 = (g), (p)->b0 = (b)
#endif

#ifndef setRGB1
	#define setRGB1(p, r, g, b) \
		(p)->r1 = (r), (p)->g1 = (g), (p)->b1 = (b)
#endif
	
#ifndef setRGB2
	#define setRGB2(p, r, g, b) \
		(p)->r2 = (r), (p)->g2 = (g), (p)->b2 = (b)
#endif

#ifndef setRGB3
	#define setRGB3(p, r, g, b) \
		(p)->r3 = (r), (p)->g3 = (g), (p)->b3 = (b)
#endif

#ifndef setUV0
	#define setUV0(p, u, v) \
		(p)->u0 = (u), (p)->v0 = (v)
#endif

#ifndef setUV3
	#define setUV3(p,_u0,_v0,_u1,_v1,_u2,_v2) \
		(p)->u0 = (_u0), (p)->v0 = (_v0), \
		(p)->u1 = (_u1), (p)->v1 = (_v1), \
		(p)->u2 = (_u2), (p)->v2 = (_v2)
#endif

#ifndef setUV4
	#define setUV4(p,_u0,_v0,_u1,_v1,_u2,_v2,_u3,_v3) \
		(p)->u0 = (_u0), (p)->v0 = (_v0), \
		(p)->u1 = (_u1), (p)->v1 = (_v1), \
		(p)->u2 = (_u2), (p)->v2 = (_v2), \
		(p)->u3 = (_u3), (p)->v3 = (_v3)
#endif

#define setClut(p,_cba) \
	(p)->cba = (_cba)

#define setTPage(p,_tsb) \
	(p)->tsb = (_tsb)

// TMD is made up of header, object tables (one for each object), 
// primative table, vertices table and normals table

typedef struct
	{
	u_long id;
	u_long flag;
	u_long nobjs;
	} TMD_HDR;

#define setTMD_HDR(t) \
	(t)->id = 0x00000041, (t)->flag = 1, (t)->nobjs = 1

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

#define setTMD_OBJ(t,_vt,_vn,_nt,_nn,_pt,_pn) \
	(t)->vert_top = (_vt), (t)->n_vert = (_vn),	\
	(t)->norm_top = (_nt), (t)->n_norm = (_nn),	\
	(t)->prim_top = (_pt), (t)->n_prim = (_pn),	\
	(t)->scale = 0

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

// primative table is made up of a collection of primatives headers, with there
// primative data

typedef struct
	{
	u_char olen, ilen, flag, mode;
	} PRIM_HDR;

#define setPRIM_HDR(p,_m,_f,_i,_o) \
	(p)->mode = (_m), (p)->flag = (_f), (p)->ilen = (_i), (p)->olen = (_o)

// normals and vertices are indices for the normal and vertex tables
// r0, g0, b0 are colour values
// mode2 is just a repetition of the mode data used as a filler
typedef struct
	{
	u_char olen, ilen, flag, mode;
	u_char r0, g0, b0, mode2;
	u_short norm0, vert0;
	u_short vert1, vert2;
	} TMD_F3;

#define setTMD_F3(t,_r,_g,_b,_norm,_vert0,_vert1,_vert2) \
	(t)->mode = F3, (t)->flag = 0, (t)->ilen = 3, (t)->olen = 4, \
	(t)->mode2 = F3, \
	(t)->r0 = (_r), (t)->g0 = (_g), (t)->b0 = (_b), \
	(t)->norm0 = (_norm), \
	(t)->vert0 = (_vert0), (t)->vert1 = (_vert1), (t)->vert2 = (_vert2)

// CBA clutY * 64 + clutX / 16
// TSB texture page + semitrans rate (0..3) << 5 + colour mode (0..2) << 7
// u, v are texture page coordinates
typedef struct
	{
	u_char olen, ilen, flag, mode;
	u_char u0, v0; u_short cba;
	u_char u1, v1; u_short tsb;
	u_char u2, v2; u_short pad;
	u_short norm0, vert0;
	u_short vert1, vert2;
	} TMD_FT3;

#define setTMD_FT3(t,_cba,_tsb,_u0,_v0,_u1,_v1,_u2,_v2,_norm,_vert0,_vert1,_vert2) \
	(t)->mode = FT3, (t)->flag = 0, (t)->ilen = 5, (t)->olen = 7, \
	(t)->cba = (_cba), (t)->tsb = (_tsb), \
	(t)->u0 = (_u0), (t)->v0 = (_v0), \
	(t)->u1 = (_u1), (t)->v1 = (_v1), \
	(t)->u2 = (_u2), (t)->v2 = (_v2), \
	(t)->norm0 = (_norm), \
	(t)->vert0 = (_vert0), (t)->vert1 = (_vert1), (t)->vert2 = (_vert2)

typedef struct
	{
	u_char olen, ilen, flag, mode;
	u_char r0, g0, b0, mode2;
	u_short norm0, vert0;
	u_short vert1, vert2;
	u_short vert3, pad;
	} TMD_F4;

#define setTMD_F4(t,_r,_g,_b,_norm,_vert0,_vert1,_vert2,_vert3) \
	(t)->mode = F4, (t)->flag = 0, (t)->ilen = 4, (p)->olen = 5, \
	(t)->mode2 = F4, \
	(t)->r0 = (_r), (t)->g0 = (_g), (t)->b0 = (_b), \
	(t)->norm0 = (_norm), \
	(t)->vert0 = (_vert0), (t)->vert1 = (_vert1), \
	(t)->vert2 = (_vert2), (t)->vert3 = (_vert3)

typedef struct 			// ilen 7 olen 9
	{
	u_char olen, ilen, flag, mode;
	u_char u0, v0; u_short cba;
	u_char u1, v1; u_short tsb;
	u_char u2, v2; u_short pad0;
	u_char u3, v3; u_short pad1;
	u_short norm0, vert0;
	u_short vert1, vert2;
	u_short vert3, pad2;
	} TMD_FT4;

#define setTMD_FT4(t,_cba,_tsb,_u0,_v0,_u1,_v1,_u2,_v2,_u3,_v3,_norm,_vert0,_vert1,_vert2,_vert3) \
	(t)->mode = FT4, (t)->flag = 0, (t)->ilen = 7, (t)->olen = 9, \
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
	u_char olen, ilen, flag, mode;
	u_char r0, g0, b0, mode2;
	u_short norm0, vert0;
	u_short norm1, vert1;
	u_short norm2, vert2;
	} TMD_G3;

#define setTMD_G3(t,_r,_g,_b,_norm0,_norm1,_norm2,_vert0,_vert1,_vert2) \
	(t)->mode = G3, (t)->flag = 0, (t)->ilen = 4, (t)->olen = 6, \
	(t)->mode2 = G3, \
	(t)->r0 = (_r), (t)->g0 = (_g), (t)->b0 = (_b), \
	(t)->norm0 = (_norm0), (t)->norm1 = (_norm1), (t)->norm2 = (_norm2), \
	(t)->vert0 = (_vert0), (t)->vert1 = (_vert1), (t)->vert2 = (_vert2)

typedef struct 			// ilen 6 olen 9
	{
	u_char olen, ilen, flag, mode;
	u_char u0, v0; u_short cba;
	u_char u1, v1; u_short tsb;
	u_char u2, v2; u_short pad;
	u_short norm0, vert0;
	u_short norm1, vert1;
	u_short norm2, vert2;
	} TMD_GT3;

#define setTMD_GT3(t,_cba,_tsb,_u0,_v0,_u1,_v1,_u2,_v2,_norm0,_norm1,_norm2,_vert0,_vert1,_vert2) \
	(t)->mode = GT3, (t)->flag = 0, (t)->ilen = 6, (t)->olen = 9, \
	(t)->cba = (_cba), (t)->tsb = (_tsb), \
	(t)->u0 = (_u0), (t)->v0 = (_v0), \
	(t)->u1 = (_u1), (t)->v1 = (_v1), \
	(t)->u2 = (_u2), (t)->v2 = (_v2), \
	(t)->norm0 = (_norm0), (t)->norm0 = (_norm1),	(t)->norm0 = (_norm2), \
	(t)->vert0 = (_vert0), (t)->vert1 = (_vert1),	(t)->vert2 = (_vert2)

typedef struct 			// ilen 5 olen 8
	{
	u_char olen, ilen, flag, mode;
	u_char r0, g0, b0, mode2;
	u_short norm0, vert0;
	u_short norm1, vert1;
	u_short norm2, vert2;
	u_short norm3, vert3;
	} TMD_G4;

#define setTMD_G4(t,_r,_g,_b,_norm0,_norm1,_norm2,_norm3,_vert0,_vert1,_vert2,_vert3) \
	(t)->mode = G4, (t)->flag = 0, (t)->ilen = 5, (t)->olen = 8, \
	(t)->mode2 = G4, \
	(t)->r0 = (_r), (t)->g0 = (_g), (t)->b0 = (_b), \
	(t)->norm0 = (_norm0), (t)->norm1 = (_norm1), \
	(t)->norm2 = (_norm2), (t)->norm3 = (_norm3), \
	(t)->vert0 = (_vert0), (t)->vert1 = (_vert1), \
	(t)->vert2 = (_vert2), (t)->vert3 = (_vert3)

typedef struct 			// ilen 8 olen 12
	{
	u_char olen, ilen, flag, mode;
	u_char u0, v0; u_short cba;
	u_char u1, v1; u_short tsb;
	u_char u2, v2; u_short pad0;
	u_char u3, v3; u_short pad1;
	u_short norm0, vert0;
	u_short norm1, vert1;
	u_short norm2, vert2;
	u_short norm3, vert3;
	} TMD_GT4;

#define setTMD_GT4(t,_cba,_tsb,_u0,_v0,_u1,_v1,_u2,_v2,_u3,_v3,_norm0,_norm1,_norm2,_norm3,_vert0,_vert1,_vert2,_vert3) \
	(t)->mode = GT4, (t)->flag = 0, (t)->ilen = 8, (t)->olen = 12, \
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
	u_char olen, ilen, flag, mode;
	u_char r0, g0, b0, mode2;
	u_short vert0, vert1;
	u_short vert2, pad;
	} TMD_F3N;

#define setTMD_F3N(t,_r,_g,_b,_vert0,_vert1,_vert2) \
	(t)->mode = F3N, (t)->flag = 1, (t)->ilen = 3, (t)->olen = 4, \
	(t)->mode2 = F3N, \
	(t)->r0 = (_r), (t)->g0 = (_g), (t)->b0 = (_b), \
	(t)->vert0 = (_vert0), (t)->vert1 = (_vert1), (t)->vert2 = (_vert2)

typedef struct 			// ilen 6 olen 7
	{
	u_char olen, ilen, flag, mode;
	u_char u0, v0; u_short cba;
	u_char u1, v1; u_short tsb;
	u_char u2, v2; u_short pad0;
	u_char r0, g0, b0, pad1;
	u_short vert0, vert1;
	u_short vert2, pad2;
	} TMD_FT3N;

#define setTMD_FT3N(t,_cba,_tsb,_u0,_v0,_u1,_v1,_u2,_v2,_r,_g,_b,_vert0,_vert1,_vert2) \
	(t)->mode = FT3N, (t)->flag = 1, (t)->ilen = 6, (t)->olen = 7, \
	(t)->cba = (_cba), (t)->tsb = (_tsb), \
	(t)->u0 = (_u0), (t)->v0 = (_v0), \
	(t)->u1 = (_u1), (t)->v1 = (_v1), \
	(t)->u2 = (_u2), (t)->v2 = (_v2), \
	(t)->r0 = (_r), (t)->g0 = (_g), (t)->b0 = (_b), \
	(t)->vert0 = (_vert0), (t)->vert1 = (_vert1), (t)->vert2 = (_vert2)

typedef struct 			// ilen 5 olen 6
	{
	u_char olen, ilen, flag, mode;
	u_char r0, g0, b0, mode2;
	u_char r1, g1, b1, pad0;
	u_char r2, g2, b2, pad1;
	u_short vert0, vert1;
	u_short vert2, pad2;
	} TMD_G3N;

#define setTMD_G3N(t,_r0,_g0,_b0,_r1,_g1,_b1,_r2,_g2,_b2,_vert0,_vert1,_vert2) \
	(t)->mode = G3N, (t)->flag = 1, (t)->ilen = 5, (t)->olen = 6, \
	(t)->mode2 = G3N, \
	(t)->r0 = (_r0), (t)->g0 = (_g0), (t)->b0 = (_b0), \
	(t)->r1 = (_r1), (t)->g1 = (_g1), (t)->b1 = (_b1), \
	(t)->r2 = (_r2), (t)->g2 = (_g2), (t)->b2 = (_b2), \
	(t)->vert0 = (_vert0), (t)->vert1 = (_vert1), (t)->vert2 = (_vert2)

typedef struct 			// ilen 8 olen 9
	{
	u_char olen, ilen, flag, mode;
	u_char u0, v0; u_short cba;
	u_char u1, v1; u_short tsb;
	u_char u2, v2; u_short pad0;
	u_char r0, g0, b0, pad1;
	u_char r1, g1, b1, pad2;
	u_char r2, g2, b2, pad3;
	u_short vert0, vert1;
	u_short vert2, pad4;
	} TMD_GT3N;

#define setTMD_GT3N(t,_cba,_tsb,_u0,_v0,_u1,_v1,_u2,_v2,_r0,_g0,_b0,_r1,_g1,_b1,_r2,_g2,_b2,_vert0,_vert1,_vert2) \
	(t)->mode = GT3N, (t)->flag = 1, (t)->ilen = 8, (t)->olen = 9, \
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
	u_char olen, ilen, flag, mode;
	u_char r0, g0, b0, mode2;
	u_short vert0, vert1;
	u_short vert2, vert3;
	} TMD_F4N;

#define setTMD_F4N(t,_r,_g,_b,_vert0,_vert1,_vert2,_vert3) \
	(t)->mode = F4N, (t)->flag = 1, (t)->ilen = 3, (t)->olen = 5, \
	(t)->mode2 = F4N, \
	(t)->r0 = (_r), (t)->g0 = (_g), (t)->b0 = (_b), \
	(t)->vert0 = (_vert0), (t)->vert1 = (_vert1), \
	(t)->vert2 = (_vert2), (t)->vert3 = (_vert3)

typedef struct 			// ilen 7 olen 9
	{
	u_char olen, ilen, flag, mode;
	u_char u0, v0; u_short cba;
	u_char u1, v1; u_short tsb;
	u_char u2, v2; u_short pad0;
	u_char u3, v3; u_short pad1;
	u_char r0, g0, b0, pad2;
	u_short vert0, vert1;
	u_short vert2, vert3;
	} TMD_FT4N;

#define setTMD_FT4N(t,_cba,_tsb,_u0,_v,_u1,_v1,_u2,_v2,_u3,_v3,_r,_g,_b,_vert0,_vert1,_vert2,_vert3) \
	(t)->mode = FT4N, (t)->flag = 1, (t)->ilen = 7, (t)->olen = 9, \
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
	u_char olen, ilen, flag, mode;
	u_char r0, g0, b0, mode2;
	u_char r1, g1, b1, pad0;
	u_char r2, g2, b2, pad1;
	u_char r3, g3, b3, pad2;
	u_short vert0, vert1;
	u_short vert2, vert3;
	} TMD_G4N;

#define setTMD_G4N(t,_r0,_g0,_b0,_r1,_g1,_b1,_r2,_g2,_b2,_r3,_g3,_b3,_vert0,_vert1,_vert2,_vert3) \
	(t)->mode = G4N, (t)->flag = 1, (t)->ilen = 6, (t)->olen = 8, \
	(t)->mode2 = G4N, \
	(t)->r0 = (_r0), (t)->g0 = (_g0), (t)->b0 = (_b0), \
	(t)->r1 = (_r1), (t)->g1 = (_g1), (t)->b1 = (_b1), \
	(t)->r2 = (_r2), (t)->g2 = (_g2), (t)->b2 = (_b2), \
	(t)->r3 = (_r3), (t)->g3 = (_g3), (t)->b3 = (_b3), \
	(t)->vert0 = (_vert0), (t)->vert1 = (_vert1), \
	(t)->vert2 = (_vert2), (t)->vert3 = (_vert3)

typedef struct 			// ilen 10 olen 12
	{
	u_char olen, ilen, flag, mode;
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
	} TMD_GT4N;

#define setTMD_GT4N(t,_cba,_tsb,_u0,_v0,_u1,_v1,_u2,_v2,_u3,_v3,_r0,_g0,_b0,_r1,_g1,_b1,_r2,_g2,_b2,_r3,_g3,_b3,_vert0,_vert1,_vert2,_vert3) \
	(t)->mode = GT4N, (t)->flag = 1, (t)->ilen = 10, (t)->olen = 12, \
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

void TMD_start(unsigned long *tmd_addr);
void TMD_end();
TMD_F3 *TMD_addPolyF3();
TMD_FT3 *TMD_addPolyFT3();
TMD_F4 *TMD_addPolyF4();
TMD_FT4 *TMD_addPolyFT4();
TMD_G3 *TMD_addPolyG3();
TMD_GT3 *TMD_addPolyGT3();
TMD_G4 *TMD_addPolyG4();
TMD_GT4 *TMD_addPolyGT4();
TMD_F3N *TMD_addPolyF3N();
TMD_FT3N *TMD_addPolyFT3N();
TMD_F4N *TMD_addPolyF4N();
TMD_FT4N *TMD_addPolyFT4N();
TMD_G3N *TMD_addPolyG3N();
TMD_GT3N *TMD_addPolyGT3N();
TMD_G4N *TMD_addPolyG4N();
TMD_GT4N *TMD_addPolyGT4N();
TMD_VERT *TMD_getVertAddr(unsigned long *tmd, unsigned char polyIdx);
void TMD_setVert0(unsigned long *tmd, unsigned char polyIdx, short x, short y, short z);
void TMD_setVert1(unsigned long *tmd, unsigned char polyIdx, short x, short y, short z);
void TMD_setVert2(unsigned long *tmd, unsigned char polyIdx, short x, short y, short z);
void TMD_setVert3(unsigned long *tmd, unsigned char polyIdx, short x, short y, short z);
TMD_NORM *TMD_getNormAddr(unsigned long *tmd, unsigned char polyIdx);
void TMD_setNorm0(unsigned long *tmd, unsigned char polyIdx, short x, short y, short z);
void TMD_setNorm1(unsigned long *tmd, unsigned char polyIdx, short x, short y, short z);
void TMD_setNorm2(unsigned long *tmd, unsigned char polyIdx, short x, short y, short z);
void TMD_setNorm3(unsigned long *tmd, unsigned char polyIdx, short x, short y, short z);
void TMD_setRGB0(unsigned long *tmd, unsigned char polyIdx, short r, short g, short b);
void TMD_setRGB1(unsigned long *tmd, unsigned char polyIdx, short r, short g, short b);
void TMD_setRGB2(unsigned long *tmd, unsigned char polyIdx, short r, short g, short b);
void TMD_setRGB3(unsigned long *tmd, unsigned char polyIdx, short r, short g, short b);
void TMD_setUV3(unsigned long *tmd, unsigned char polyIdx, unsigned char u0, unsigned char v0, unsigned char u1, unsigned char v1, unsigned char u2, unsigned char v2);
void TMD_setUV4(unsigned long *tmd, unsigned char polyIdx, unsigned char u0, unsigned char v0, unsigned char u1, unsigned char v1, unsigned char u2, unsigned char v2, unsigned char u3, unsigned char v3);
void TMD_setClut(unsigned long *tmd, unsigned char polyIdx, unsigned short clut);
void TMD_setTPage(unsigned long *tmd, unsigned char polyIdx, unsigned short tpage);

#endif