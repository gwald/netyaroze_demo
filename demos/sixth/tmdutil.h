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

/* for mode decoding */
#define IIP  0x10
#define QUAD 0x08
#define TME  0x04
#define ABE  0x02
#define TGE  0x01

/* flags */
#define GRD  0x04  // 1 gradation, 0 no gradation
#define FCE  0x02  // 1 2-faces, 0 1-face
#define LGT  0x01  // 0 light source calculation, 1 no light source calculation

/* mode */
#define F_3      0x20  // Triangle  No-texture 
#define F_4      0x28  // Quadangle No-texture 
#define F_3G     0x20  // Triangle  No-texture gradation
#define F_4G     0x28  // Quadangle No-texture gradation
#define F_3T     0x24  // Triangle  Texture    
#define F_4T     0x2c  // Quadangle Texture    
#define G_3      0x30  // triangle gouraud
#define G_3G     0x30  // triangle gouraud gradation
#define G_3T     0x34  // triangle gouraud texture
#define G_4      0x38  // quad gouraud
#define G_4G     0x38  // quad gouraud gradation
#define G_4T     0x3c  // quad gouraud texture
#define F_3_NL   0x21  // triangle no texture no light
#define F_3T_NL	 0x25  // triangle texture no light
#define G_3_NL	 0x31  // triangle gouraud no texture no light
#define G_3T_NL  0x35  // triangle gouraud texture no light
#define F_4_NL   0x29  // quad no texture no light
#define F_4T_NL	 0x2d  // quad texture no light
#define G_4_NL	 0x39  // quad gouraud no texture no light
#define G_4T_NL  0x3d  // quad gouraud texture no light

/* non polygon modes */
#define SL       0x40  // straight line no gradation
#define SL_G	 0x50  // straight line gradation
#define SP_3D	 0x64  // 3d sprite free size
#define SP_3D_1  0x6c  // 3d sprite 1x1
#define SP_3D_8  0x74  // 3d sprite 8x8
#define SP_3D_16 0x7c  // 3d sprite 16x16

typedef struct {
  u_long id;
  u_long flag;
  u_long nobjs;
} TMD_HDR;

typedef struct {
  u_long *vert_top;
  u_long n_vert;
  u_long *norm_top;
  u_long n_norm;
  u_long *prim_top;
  u_long n_prim;
  long   scale;
} TMD_OBJ;

typedef struct{
  u_char olen, ilen, flag, mode;
} PRIM_HDR;

typedef struct 
	{
	short vx, vy;
	short vz, pad;
	} TMD_VERT;

typedef struct 
	{
	short nx, ny;
	short nz, pad;
	} TMD_NORM;

typedef struct 
	{
	u_char r0, g0, b0, mode2;
	u_short norm0, vert0;
	u_short vert1, vert2;
	} TMD_F_3;

typedef struct
	{
	u_char r0, g0, b0, mode2;
	u_char r1, g1, b1, pad0;
	u_char r2, g2, b2, pad1;
	u_short norm0, vert0;
	u_short vert1, vert2;
	} TMD_F_3G;

typedef struct 
	{
	u_char u0, v0; u_short cba;
	u_char u1, v1; u_short tsb;
	u_char u2, v2; u_short pad;
	u_short norm0, vert0;
	u_short vert1, vert2;
	} TMD_F_3T;

typedef struct 
	{
	u_char r0, g0, b0, mode2;
	u_short norm0, vert0;
	u_short vert1, vert2;
	u_short vert3, pad;
	} TMD_F_4;

typedef struct 
	{
	u_char r0, g0, b0, mode2;
	u_char r1, g1, b1, pad0;
	u_char r2, g2, b2, pad1;
	u_char r3, g3, b3, pad2;
	u_short norm0, vert0;
	u_short vert1, vert2;
	u_short vert3, pad3;
	} TMD_F_4G;

typedef struct 
	{
	u_char u0, v0; u_short cba;
	u_char u1, v1; u_short tsb;
	u_char u2, v2; u_short pad0;
	u_char u3, v3; u_short pad1;
	u_short norm0, vert0;
	u_short vert1, vert2;
	u_short vert3, pad2;
	} TMD_F_4T;

typedef struct 
	{
	u_char r0, g0, b0, mode2;
	u_short norm0, vert0;
	u_short norm1, vert1;
	u_short norm2, vert2;
	} TMD_G_3;

typedef struct 
	{
	u_char r0, g0, b0, mode2;
	u_char r1, g1, b1, pad0;
	u_char r2, g2, b2, pad1;
	u_short norm0, vert0;
	u_short norm1, vert1;
	u_short norm2, vert2;
	} TMD_G_3G;

typedef struct 
	{
	u_char u0, v0; u_short cba;
	u_char u1, v1; u_short tsb;
	u_char u2, v2; u_short pad;
	u_short norm0, vert0;
	u_short norm1, vert1;
	u_short norm2, vert2;
	} TMD_G_3T;

typedef struct 
	{
	u_char r0, g0, b0, mode2;
	u_short norm0, vert0;
	u_short norm1, vert1;
	u_short norm2, vert2;
	u_short norm3, vert3;
	} TMD_G_4;

typedef struct 
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

typedef struct 
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

typedef struct 
	{
	u_char r0, g0, b0, mode2;
	u_short vert0, vert1;
	u_short vert2, pad;
	} TMD_F_3_NL;

typedef struct 
	{
	u_char u0, v0; u_short cba;
	u_char u1, v1; u_short tsb;
	u_char u2, v2; u_short pad0;
	u_char r0, g0, b0, pad1;
	u_short vert0, vert1;
	u_short vert2, pad2;
	} TMD_F_3T_NL;

typedef struct 
	{
	u_char r0, g0, b0, mode2;
	u_char r1, g1, b1, pad0;
	u_char r2, g2, b2, pad1;
	u_short vert0, vert1;
	u_short vert2, pad2;
	} TMD_G_3_NL;

typedef struct 
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

typedef struct 
	{
	u_char r0, g0, b0, mode2;
	u_short vert0, vert1;
	u_short vert2, vert3;
	} TMD_F_4_NL;

typedef struct 
	{
	u_char u0, v0; u_short cba;
	u_char u1, v1; u_short tsb;
	u_char u2, v2; u_short pad0;
	u_char u3, v3; u_short pad1;
	u_char r0, g0, b0, pad2;
	u_short vert0, vert1;
	u_short vert2, vert3;
	} TMD_F_4T_NL;

typedef struct 
	{
	u_char r0, g0, b0, mode2;
	u_char r1, g1, b1, pad0;
	u_char r2, g2, b2, pad1;
	u_char r3, g3, b3, pad2;
	u_short vert0, vert1;
	u_short vert2, vert3;
	} TMD_G_4_NL;

typedef struct 
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

typedef struct
	{
	u_char r0, g0, b0, mode2;
	u_short vert0, vert1;
	} TMD_SL;

typedef struct
	{
	u_char r0, g0, b0, mode2;
	u_char r1, g1, b1, pad;
	u_short vert0, vert1;
	} TMD_SL_G;

typedef struct
	{
	u_short vert0, tsb;
	u_char u0, v0; u_short cba;
	u_short w, h;
	} TMD_SP_3D;

typedef struct
	{
	u_short vert0, tsb;
	u_char u0, v0; u_short cba;
	} TMD_SP_3D_1;

typedef struct
	{
	u_short vert0, tsb;
	u_char u0, v0; u_short cba;
	} TMD_SP_3D_8;
	
typedef struct
	{
	u_short vert0, tsb;
	u_char u0, v0; u_short cba;
	} TMD_SP_3D_16;

#endif