/*
 * LIBXPLOD.C
 *
 * Copyright (C) 1993-1997 by Sony Computer Entertainment of America, Inc.
 * All rights Reserved
 * ----------------------------------------------------------------------------
 *
 * This file is self contained and will create particle explosion of a
 * given TMD model data.  For TMD data structures look into LibTmd.h.
 * LibXplod.h contains the function templates for the explosion system.
 * ----------------------------------------------------------------------------
 */

#include "libmain.h"
#include "libmath.h"
#include "libtmd.h"
#include "libxplod.h"

/*
 * Macros used in explosion - a particle explosion system is used here
 * ----------------------------------------------------------------------------
 */

#define	SPEED_MOVE_MIN 			12
#define	SPEED_MOVE_RAND			13

#define	SPEED_ROTATION_MIN		128
#define	SPEED_ROTATION_RAND		129

#define	SPEED_VANISH_MIN 		90
#define	SPEED_VANISH_RAND		91

/*
 * Explosion uses it's own object structure to create multiple TMD objects
 * out the given TMD data and applies a particle explosion system on it.
 * ----------------------------------------------------------------------------
 */
 
#define PARTICLE_MAXNO			256

typedef struct __particle {
	TMDMINE			Model;
	GsDOBJ2			Object;
	GsCOORDINATE2	Pos;
	SVECTOR			Angle;
	SVECTOR			TranslationVect;
	SVECTOR			RotationVect;
	long			NoOfTimes;
} PARTICLE;

static PARTICLE ParticleList[ PARTICLE_MAXNO ];
static int ParticleListTop;

/*
 * The following provide a bounding limit for the exploded
 * poly's existance.  The individual TMD will cease to exist
 * once it crosses the boundary defined by the following.
 * The bouning box extends from
 *    (-ParticleXlim,       (ParticleXlim,
 *	   -ParticleYlim,   to   ParticleYlim,
 * 	   -ParticleZlim)        ParticleZlim)
 */
static int ParticleXlim;
static int ParticleYlim;
static int ParticleZlim;

/*
 * They provide the total number of frames displayed for each frame
 * and the total number of new TMDs created for each particle explosion.
 * These values are printed with other debugging information
 */
static int NoParticles;
static int NoParticlesCreated;

/*
 * Static function templates
 * ----------------------------------------------------------------------------
 */

static TMDPRIM *CreatePrimFl3 (PARTICLE *particle, TMDPRIM *prim, TMDVERT *vert, TMDNORM *norm);
static TMDPRIM *CreatePrimFld3(PARTICLE *particle, TMDPRIM *prim, TMDVERT *vert, TMDNORM *norm);
static TMDPRIM *CreatePrimFlt3(PARTICLE *particle, TMDPRIM *prim, TMDVERT *vert, TMDNORM *norm);
static TMDPRIM *CreatePrimF3  (PARTICLE *particle, TMDPRIM *prim, TMDVERT *vert, TMDNORM *norm);
static TMDPRIM *CreatePrimFt3 (PARTICLE *particle, TMDPRIM *prim, TMDVERT *vert, TMDNORM *norm);

static TMDPRIM *CreatePrimGl3 (PARTICLE *particle, TMDPRIM *prim, TMDVERT *vert, TMDNORM *norm);
static TMDPRIM *CreatePrimGld3(PARTICLE *particle, TMDPRIM *prim, TMDVERT *vert, TMDNORM *norm);
static TMDPRIM *CreatePrimGlt3(PARTICLE *particle, TMDPRIM *prim, TMDVERT *vert, TMDNORM *norm);
static TMDPRIM *CreatePrimG3  (PARTICLE *particle, TMDPRIM *prim, TMDVERT *vert, TMDNORM *norm);
static TMDPRIM *CreatePrimGt3 (PARTICLE *particle, TMDPRIM *prim, TMDVERT *vert, TMDNORM *norm);

static TMDPRIM *CreatePrimFl4 (PARTICLE *particle, TMDPRIM *prim, TMDVERT *vert, TMDNORM *norm);
static TMDPRIM *CreatePrimFld4(PARTICLE *particle, TMDPRIM *prim, TMDVERT *vert, TMDNORM *norm);
static TMDPRIM *CreatePrimFlt4(PARTICLE *particle, TMDPRIM *prim, TMDVERT *vert, TMDNORM *norm);
static TMDPRIM *CreatePrimF4  (PARTICLE *particle, TMDPRIM *prim, TMDVERT *vert, TMDNORM *norm);
static TMDPRIM *CreatePrimFt4 (PARTICLE *particle, TMDPRIM *prim, TMDVERT *vert, TMDNORM *norm);

static TMDPRIM *CreatePrimGl4 (PARTICLE *particle, TMDPRIM *prim, TMDVERT *vert, TMDNORM *norm);
static TMDPRIM *CreatePrimGld4(PARTICLE *particle, TMDPRIM *prim, TMDVERT *vert, TMDNORM *norm);
static TMDPRIM *CreatePrimGlt4(PARTICLE *particle, TMDPRIM *prim, TMDVERT *vert, TMDNORM *norm);
static TMDPRIM *CreatePrimG4  (PARTICLE *particle, TMDPRIM *prim, TMDVERT *vert, TMDNORM *norm);
static TMDPRIM *CreatePrimGt4 (PARTICLE *particle, TMDPRIM *prim, TMDVERT *vert, TMDNORM *norm);

static TMDPRIM *CreatePrimL   (PARTICLE *particle, TMDPRIM *prim, TMDVERT *vert, TMDNORM *norm);
static TMDPRIM *CreatePrimLd  (PARTICLE *particle, TMDPRIM *prim, TMDVERT *vert, TMDNORM *norm);

static TMDPRIM *CreatePrimS   (PARTICLE *particle, TMDPRIM *prim, TMDVERT *vert, TMDNORM *norm);
static TMDPRIM *CreatePrimSf  (PARTICLE *particle, TMDPRIM *prim, TMDVERT *vert, TMDNORM *norm);

static TMDPRIM *CreatePrimUk  (PARTICLE *particle, TMDPRIM *prim, TMDVERT *vert, TMDNORM *norm);

static void MakeTmdPos(PARTICLE *particle, GsCOORDINATE2 *super, int ox, int oy, int oz);

/*
 * Prints explosion related debugging info on screen
 * ----------------------------------------------------------------------------
 */

void ExplosionDebugData(void)
{
//	FntPrint("No of Particles = %d (%d)\n", NoParticles, NoParticlesCreated);
//	FntPrint("\n");	
	return;
}

/*
 * Initializes particle explosion system
 * ----------------------------------------------------------------------------
 */

void ExplosionInitData(int xlim, int ylim, int zlim)
{
	PARTICLE *particle;
	TMDMINE *ntmd;
	int i;
	
	/* Initialize all object structures in particle explosion system */
	for (particle = ParticleList, i = 0; i < PARTICLE_MAXNO; i++, particle++) {
		memset((void *)particle, 0, sizeof(PARTICLE));
		
		ntmd = &particle->Model;
		ntmd->head.id    = 0x41;
		ntmd->head.flags = 1;
		ntmd->head.nobjc = 1;
		ntmd->object.primitive_top = (long)(ntmd->pack);
		ntmd->object.n_primitive   = 1;
		ntmd->object.scale         = 0;
		
		particle->Object.coord2 = &particle->Pos;
		particle->Object.attribute = (GsFOG);
		
		particle->NoOfTimes = 0;
	}
	
	/* The next particle to be created at this index */
	ParticleListTop = 0;
	
	/*
	 * Particle explosion system works in a bounding box where the
	 * following provides the distance of edges from the center of
	 * the box.  The particle created do not exist beyond this
	 * bounding box.
	 */
	ParticleXlim = xlim;
	ParticleYlim = ylim;
	ParticleZlim = zlim;

	/* Initialize number of particles active in the system */
	NoParticles = 0;
	
	/* Initialize number of additional particles created */
	NoParticlesCreated = 0;
	return;
}

/*
 * Draw any particle existing in the particle explosion system
 * ----------------------------------------------------------------------------
 */

void ExplosionDrawData(GsOT *ot, long shift)
{
	PARTICLE *particle;
	MATRIX tmp;
	int i;
	
	/* To calculate the number of particles in the system */
	NoParticles = 0;
	
	/* Rotate, translate and display the particles */
	for (particle = ParticleList, i = 0; i < PARTICLE_MAXNO; i++, particle++) {
		if (particle->NoOfTimes <= 0)
			continue;
		
		/* Rotate the particle */
		particle->Angle.vx = (particle->Angle.vx + particle->RotationVect.vx + ONE) % ONE;
		particle->Angle.vy = (particle->Angle.vy + particle->RotationVect.vy + ONE) % ONE;
		particle->Angle.vz = (particle->Angle.vz + particle->RotationVect.vz + ONE) % ONE;
		
		/* Apply the translation vector to the particle */
		tmp = GsIDMATRIX;
		tmp.t[0] = particle->Pos.coord.t[0] + particle->TranslationVect.vx;
		tmp.t[1] = particle->Pos.coord.t[1] + particle->TranslationVect.vy;
		tmp.t[2] = particle->Pos.coord.t[2] + particle->TranslationVect.vz;
		
		/* Check if the particle is in the bounding limit */
		if ((tmp.t[0] <= -ParticleXlim) || (tmp.t[0] >= ParticleXlim) ||
			(tmp.t[1] <= -ParticleYlim) || (tmp.t[1] >= ParticleYlim) ||
			(tmp.t[2] <= -ParticleZlim) || (tmp.t[2] >= ParticleZlim)) {
			particle->NoOfTimes = 0;
			continue;
		}
		
		/* Update the rotation matrix of the particle */
		RotMatrix(&particle->Angle, &tmp);
		particle->Pos.coord = tmp;
		particle->Pos.flg = 0;
		
		/* Draw the particle */
		GsGetLw(particle->Object.coord2, &tmp);
		GsSetLightMatrix(&tmp);
		GsGetLs(particle->Object.coord2, &tmp);
		GsSetLsMatrix(&tmp);
		GsSortObject4(&particle->Object, ot, shift, getScratchAddr(0));
		
		/* Decrease the life span of the particle */
		particle->NoOfTimes --;
		
		/* One more particle drawn in the particle system */
		NoParticles ++;
	}
	return;
}

/*
 * Create particles for the given object
 * ----------------------------------------------------------------------------
 */

void ExplosionMakeData(unsigned long *mdata, int id, GsCOORDINATE2 *super, int ox, int oy, int oz)
{
	TMDHEAD *head;
	TMDOBJC *objc;
	TMDVERT *vert;
	TMDNORM *norm;
	TMDPRIM *prim;
	int i,nprim;
	int j,n;
	
	/* Ptr to the TMD model header - and get the number of objects */
	head = (TMDHEAD *)mdata;
	
	/* Ptr to the Object table header */
	objc = (TMDOBJC *)((long)mdata + sizeof(TMDHEAD));
	objc += id;
		
	/* Compute the ptrs to primitive, vertex and normal lists */
	if (head->flags == 0) {
		vert = (TMDVERT *)((long)objc + objc->vert_top);
		norm = (TMDNORM *)((long)objc + objc->normal_top);
		prim = (TMDPRIM *)((long)objc + objc->primitive_top);
	}
	else {
		vert = (TMDVERT *)objc->vert_top;
		norm = (TMDNORM *)objc->normal_top;
		prim = (TMDPRIM *)objc->primitive_top;
	}
	
	/* Get the number of primitives in this object */
	nprim = objc->n_primitive;
	
//	printf("model ptr = 0x%08x, object table = 0x%08x\n", mdata, objc);
//	printf("prim = 0x%08x, norm = 0x%08x, vert = 0x%08x\n", prim, vert, norm);
//	printf("nprim = %d, nvert = %d, nnorm = %d\n", nprim, objc->n_vert, objc->n_normal);
	
	/* Initialize number of particles created for this object */
	NoParticlesCreated = 0;
	
	/* Create particles for each primitive in the object */
	for (n = ParticleListTop, i = 0; i < nprim; i++) {
		/* Find the next available particle in the system */
		for (j = 0; j < PARTICLE_MAXNO; j++, n++) {
			n %= PARTICLE_MAXNO;
			if (ParticleList[n].NoOfTimes <= 0) break;
		}
		
		/* No more particles available in the system */
		if (j >= PARTICLE_MAXNO)
			break;
		
//		printf("next prim ptr = 0x%08x (mode = 0x%04x, ilen = %d)\n",
//							(long )(prim),
//							(short)(prim->mode & TMD_PRIMTYPE_MASK),
//							(short)(prim->ilen));
		
		/* Create a particle depending upon the primitive type */
		switch(prim->mode & TMD_PRIMTYPE_MASK) {
		
		case TMD_PRIMTYPE_FL3:
			prim = CreatePrimFl3(&ParticleList[n], prim, vert, norm);
			MakeTmdPos(&ParticleList[n], super, ox, oy, oz);
			n++,
			NoParticlesCreated++;
			break;
		
		case TMD_PRIMTYPE_FLD3:
			prim = CreatePrimFld3(&ParticleList[n], prim, vert, norm);
			MakeTmdPos(&ParticleList[n], super, ox, oy, oz);
			n++,
			NoParticlesCreated++;
			break;
		
		case TMD_PRIMTYPE_FLT3:
			prim = CreatePrimFlt3(&ParticleList[n], prim, vert, norm);
			MakeTmdPos(&ParticleList[n], super, ox, oy, oz);
			n++,
			NoParticlesCreated++;
			break;
		
		case TMD_PRIMTYPE_F3:
			prim = CreatePrimF3(&ParticleList[n], prim, vert, norm);
			MakeTmdPos(&ParticleList[n], super, ox, oy, oz);
			n++,
			NoParticlesCreated++;
			break;
		
		case TMD_PRIMTYPE_FT3:
			prim = CreatePrimFt3(&ParticleList[n], prim, vert, norm);
			MakeTmdPos(&ParticleList[n], super, ox, oy, oz);
			n++,
			NoParticlesCreated++;
			break;
		
		case TMD_PRIMTYPE_GL3:
			prim = CreatePrimGl3(&ParticleList[n], prim, vert, norm);
			MakeTmdPos(&ParticleList[n], super, ox, oy, oz);
			n++,
			NoParticlesCreated++;
			break;
		
		case TMD_PRIMTYPE_GLD3:
			prim = CreatePrimGld3(&ParticleList[n], prim, vert, norm);
			MakeTmdPos(&ParticleList[n], super, ox, oy, oz);
			n++,
			NoParticlesCreated++;
			break;
		
		case TMD_PRIMTYPE_GLT3:
			prim = CreatePrimGlt3(&ParticleList[n], prim, vert, norm);
			MakeTmdPos(&ParticleList[n], super, ox, oy, oz);
			n++,
			NoParticlesCreated++;
			break;
		
		case TMD_PRIMTYPE_G3:
			prim = CreatePrimG3(&ParticleList[n], prim, vert, norm);
			MakeTmdPos(&ParticleList[n], super, ox, oy, oz);
			n++,
			NoParticlesCreated++;
			break;
		
		case TMD_PRIMTYPE_GT3:
			prim = CreatePrimGt3(&ParticleList[n], prim, vert, norm);
			MakeTmdPos(&ParticleList[n], super, ox, oy, oz);
			n++,
			NoParticlesCreated++;
			break;
		
		case TMD_PRIMTYPE_FL4:
			prim = CreatePrimFl4(&ParticleList[n], prim, vert, norm);
			MakeTmdPos(&ParticleList[n], super, ox, oy, oz);
			n++,
			NoParticlesCreated++;
			break;
		
		case TMD_PRIMTYPE_FLD4:
			prim = CreatePrimFld4(&ParticleList[n], prim, vert, norm);
			MakeTmdPos(&ParticleList[n], super, ox, oy, oz);
			n++,
			NoParticlesCreated++;
			break;
		
		case TMD_PRIMTYPE_FLT4:
			prim = CreatePrimFlt4(&ParticleList[n], prim, vert, norm);
			MakeTmdPos(&ParticleList[n], super, ox, oy, oz);
			n++,
			NoParticlesCreated++;
			break;
		
		case TMD_PRIMTYPE_F4:
			prim = CreatePrimF4(&ParticleList[n], prim, vert, norm);
			MakeTmdPos(&ParticleList[n], super, ox, oy, oz);
			n++,
			NoParticlesCreated++;
			break;
		
		case TMD_PRIMTYPE_FT4:
			prim = CreatePrimFt4(&ParticleList[n], prim, vert, norm);
			MakeTmdPos(&ParticleList[n], super, ox, oy, oz);
			n++,
			NoParticlesCreated++;
			break;
		
		case TMD_PRIMTYPE_GL4:
			prim = CreatePrimGl4(&ParticleList[n], prim, vert, norm);
			MakeTmdPos(&ParticleList[n], super, ox, oy, oz);
			n++,
			NoParticlesCreated++;
			break;
		
		case TMD_PRIMTYPE_GLD4:
			prim = CreatePrimGld4(&ParticleList[n], prim, vert, norm);
			MakeTmdPos(&ParticleList[n], super, ox, oy, oz);
			n++,
			NoParticlesCreated++;
			break;
		
		case TMD_PRIMTYPE_GLT4:
			prim = CreatePrimGlt4(&ParticleList[n], prim, vert, norm);
			MakeTmdPos(&ParticleList[n], super, ox, oy, oz);
			n++,
			NoParticlesCreated++;
			break;
		
		case TMD_PRIMTYPE_G4:
			prim = CreatePrimG4(&ParticleList[n], prim, vert, norm);
			MakeTmdPos(&ParticleList[n], super, ox, oy, oz);
			n++,
			NoParticlesCreated++;
			break;
		
		case TMD_PRIMTYPE_GT4:
			prim = CreatePrimGt4(&ParticleList[n], prim, vert, norm);
			MakeTmdPos(&ParticleList[n], super, ox, oy, oz);
			n++,
			NoParticlesCreated++;
			break;
		
		case TMD_PRIMTYPE_L:
			prim = CreatePrimL(&ParticleList[n], prim, vert, norm);
			MakeTmdPos(&ParticleList[n], super, ox, oy, oz);
			n++,
			NoParticlesCreated++;
			break;
		
		case TMD_PRIMTYPE_LD:
			prim = CreatePrimLd(&ParticleList[n], prim, vert, norm);
			MakeTmdPos(&ParticleList[n], super, ox, oy, oz);
			n++,
			NoParticlesCreated++;
			break;
		
		case TMD_PRIMTYPE_S1:
		case TMD_PRIMTYPE_S8:
		case TMD_PRIMTYPE_S16:
			prim = CreatePrimS(&ParticleList[n], prim, vert, norm);
			MakeTmdPos(&ParticleList[n], super, ox, oy, oz);
			n++,
			NoParticlesCreated++;
			break;
		
		case TMD_PRIMTYPE_SF:
			prim = CreatePrimSf(&ParticleList[n], prim, vert, norm);
			MakeTmdPos(&ParticleList[n], super, ox, oy, oz);
			n++,
			NoParticlesCreated++;
			break;
		
		default:
			prim = CreatePrimUk(&ParticleList[n], prim, vert, norm);
			break;
		}
	}
	
	/* Update the index to next available particle in the system */
	ParticleListTop = n % PARTICLE_MAXNO;
	return;
}

/*
 * The following routines create particles based on the primitive tmode.
 * Each routines works on a unique type of primitive.
 * ----------------------------------------------------------------------------
 */

static TMDPRIM *CreatePrimFl3(PARTICLE *particle, TMDPRIM *prim, TMDVERT *vert, TMDNORM *norm)
{
	TMDMINE      *ntmd    = &particle->Model;
	TMDPRIM_FL3  *dstprim = (TMDPRIM_FL3 *)ntmd->pack;
	TMDPRIM_FL3  *srcprim = (TMDPRIM_FL3 *)prim;
	
	ntmd->object.vert_top   = (long)(ntmd->vert);
	ntmd->object.n_vert     = 3;
	ntmd->object.normal_top = (long)(ntmd->norm);
	ntmd->object.n_normal   = 1;
	
	(*dstprim) = (*srcprim);
	dstprim->normal0 = 0;
	dstprim->vertex0 = 0;
	dstprim->vertex1 = 1;
	dstprim->vertex2 = 2;
	
	ntmd->vert[0] = vert[ srcprim->vertex0 ];
	ntmd->vert[1] = vert[ srcprim->vertex1 ];
	ntmd->vert[2] = vert[ srcprim->vertex2 ];
	
	ntmd->norm[0] = norm[ srcprim->normal0 ];
	return((TMDPRIM *)((char *)prim + sizeof(TMDPRIM_FL3)));
}

static TMDPRIM *CreatePrimFld3(PARTICLE *particle, TMDPRIM *prim, TMDVERT *vert, TMDNORM *norm)
{
	TMDMINE      *ntmd    = &particle->Model;
	TMDPRIM_FLD3 *dstprim = (TMDPRIM_FLD3 *)ntmd->pack;
	TMDPRIM_FLD3 *srcprim = (TMDPRIM_FLD3 *)prim;
	
	ntmd->object.vert_top   = (long)(ntmd->vert);
	ntmd->object.n_vert     = 3;
	ntmd->object.normal_top = (long)(ntmd->norm);
	ntmd->object.n_normal   = 1;
	
	(*dstprim) = (*srcprim);
	dstprim->normal0 = 0;
	dstprim->vertex0 = 0;
	dstprim->vertex1 = 1;
	dstprim->vertex2 = 2;
	
	ntmd->vert[0] = vert[ srcprim->vertex0 ];
	ntmd->vert[1] = vert[ srcprim->vertex1 ];
	ntmd->vert[2] = vert[ srcprim->vertex2 ];
	
	ntmd->norm[0] = norm[ srcprim->normal0 ];
	return((TMDPRIM *)((char *)prim + sizeof(TMDPRIM_FLD3)));
}

static TMDPRIM *CreatePrimFlt3(PARTICLE *particle, TMDPRIM *prim, TMDVERT *vert, TMDNORM *norm)
{
	TMDMINE      *ntmd    = &particle->Model;
	TMDPRIM_FLT3 *dstprim = (TMDPRIM_FLT3 *)ntmd->pack;
	TMDPRIM_FLT3 *srcprim = (TMDPRIM_FLT3 *)prim;
	
	ntmd->object.vert_top   = (long)(ntmd->vert);
	ntmd->object.n_vert     = 3;
	ntmd->object.normal_top = (long)(ntmd->norm);
	ntmd->object.n_normal   = 1;
	
	(*dstprim) = (*srcprim);
	dstprim->normal0 = 0;
	dstprim->vertex0 = 0;
	dstprim->vertex1 = 1;
	dstprim->vertex2 = 2;
	
	ntmd->vert[0] = vert[ srcprim->vertex0 ];
	ntmd->vert[1] = vert[ srcprim->vertex1 ];
	ntmd->vert[2] = vert[ srcprim->vertex2 ];
	
	ntmd->norm[0] = norm[ srcprim->normal0 ];
	return((TMDPRIM *)((char *)prim + sizeof(TMDPRIM_FLT3)));
}

static TMDPRIM *CreatePrimF3(PARTICLE *particle, TMDPRIM *prim, TMDVERT *vert, TMDNORM *norm)
{
	TMDMINE      *ntmd    = &particle->Model;
	TMDPRIM_F3   *dstprim = (TMDPRIM_F3 *)ntmd->pack;
	TMDPRIM_F3   *srcprim = (TMDPRIM_F3 *)prim;
	
	ntmd->object.vert_top   = (long)(ntmd->vert);
	ntmd->object.n_vert     = 3;
	ntmd->object.normal_top = (long)(ntmd->norm);
	ntmd->object.n_normal   = 0;
	
	(*dstprim) = (*srcprim);
	dstprim->vertex0 = 0;
	dstprim->vertex1 = 1;
	dstprim->vertex2 = 2;
	
	ntmd->vert[0] = vert[ srcprim->vertex0 ];
	ntmd->vert[1] = vert[ srcprim->vertex1 ];
	ntmd->vert[2] = vert[ srcprim->vertex2 ];
	return((TMDPRIM *)((char *)prim + sizeof(TMDPRIM_F3)));
}

static TMDPRIM *CreatePrimFt3(PARTICLE *particle, TMDPRIM *prim, TMDVERT *vert, TMDNORM *norm)
{
	TMDMINE      *ntmd    = &particle->Model;
	TMDPRIM_FT3  *dstprim = (TMDPRIM_FT3 *)ntmd->pack;
	TMDPRIM_FT3  *srcprim = (TMDPRIM_FT3 *)prim;
	
	ntmd->object.vert_top   = (long)(ntmd->vert);
	ntmd->object.n_vert     = 3;
	ntmd->object.normal_top = (long)(ntmd->norm);
	ntmd->object.n_normal   = 0;
	
	(*dstprim) = (*srcprim);
	dstprim->vertex0 = 0;
	dstprim->vertex1 = 1;
	dstprim->vertex2 = 2;
	
	ntmd->vert[0] = vert[ srcprim->vertex0 ];
	ntmd->vert[1] = vert[ srcprim->vertex1 ];
	ntmd->vert[2] = vert[ srcprim->vertex2 ];
	return((TMDPRIM *)((char *)prim + sizeof(TMDPRIM_FT3)));
}

static TMDPRIM *CreatePrimGl3(PARTICLE *particle, TMDPRIM *prim, TMDVERT *vert, TMDNORM *norm)
{
	TMDMINE      *ntmd    = &particle->Model;
	TMDPRIM_GL3  *dstprim = (TMDPRIM_GL3 *)ntmd->pack;
	TMDPRIM_GL3  *srcprim = (TMDPRIM_GL3 *)prim;
	
	ntmd->object.vert_top   = (long)(ntmd->vert);
	ntmd->object.n_vert     = 3;
	ntmd->object.normal_top = (long)(ntmd->norm);
	ntmd->object.n_normal   = 3;
	
	(*dstprim) = (*srcprim);
	dstprim->normal0 = 0;
	dstprim->vertex0 = 0;
	dstprim->normal1 = 1;
	dstprim->vertex1 = 1;
	dstprim->normal2 = 2;
	dstprim->vertex2 = 2;
	
	ntmd->vert[0] = vert[ srcprim->vertex0 ];
	ntmd->vert[1] = vert[ srcprim->vertex1 ];
	ntmd->vert[2] = vert[ srcprim->vertex2 ];
	
	ntmd->norm[0] = norm[ srcprim->normal0 ];
	ntmd->norm[1] = norm[ srcprim->normal1 ];
	ntmd->norm[2] = norm[ srcprim->normal2 ];
	return((TMDPRIM *)((char *)prim + sizeof(TMDPRIM_GL3)));
}

static TMDPRIM *CreatePrimGld3(PARTICLE *particle, TMDPRIM *prim, TMDVERT *vert, TMDNORM *norm)
{
	TMDMINE      *ntmd    = &particle->Model;
	TMDPRIM_GLD3 *dstprim = (TMDPRIM_GLD3 *)ntmd->pack;
	TMDPRIM_GLD3 *srcprim = (TMDPRIM_GLD3 *)prim;
	
	ntmd->object.vert_top   = (long)(ntmd->vert);
	ntmd->object.n_vert     = 3;
	ntmd->object.normal_top = (long)(ntmd->norm);
	ntmd->object.n_normal   = 3;
	
	(*dstprim) = (*srcprim);
	dstprim->normal0 = 0;
	dstprim->vertex0 = 0;
	dstprim->normal1 = 1;
	dstprim->vertex1 = 1;
	dstprim->normal2 = 2;
	dstprim->vertex2 = 2;
	
	ntmd->vert[0] = vert[ srcprim->vertex0 ];
	ntmd->vert[1] = vert[ srcprim->vertex1 ];
	ntmd->vert[2] = vert[ srcprim->vertex2 ];
	
	ntmd->norm[0] = norm[ srcprim->normal0 ];
	ntmd->norm[1] = norm[ srcprim->normal1 ];
	ntmd->norm[2] = norm[ srcprim->normal2 ];
	return((TMDPRIM *)((char *)prim + sizeof(TMDPRIM_GLD3)));
}

static TMDPRIM *CreatePrimGlt3(PARTICLE *particle, TMDPRIM *prim, TMDVERT *vert, TMDNORM *norm)
{
	TMDMINE      *ntmd    = &particle->Model;
	TMDPRIM_GLT3 *dstprim = (TMDPRIM_GLT3 *)ntmd->pack;
	TMDPRIM_GLT3 *srcprim = (TMDPRIM_GLT3 *)prim;
	
	ntmd->object.vert_top   = (long)(ntmd->vert);
	ntmd->object.n_vert     = 3;
	ntmd->object.normal_top = (long)(ntmd->norm);
	ntmd->object.n_normal   = 3;
	
	(*dstprim) = (*srcprim);
	dstprim->normal0 = 0;
	dstprim->vertex0 = 0;
	dstprim->normal1 = 1;
	dstprim->vertex1 = 1;
	dstprim->normal2 = 2;
	dstprim->vertex2 = 2;
	
	ntmd->vert[0] = vert[ srcprim->vertex0 ];
	ntmd->vert[1] = vert[ srcprim->vertex1 ];
	ntmd->vert[2] = vert[ srcprim->vertex2 ];
	
	ntmd->norm[0] = norm[ srcprim->normal0 ];
	ntmd->norm[1] = norm[ srcprim->normal1 ];
	ntmd->norm[2] = norm[ srcprim->normal2 ];
	return((TMDPRIM *)((char *)prim + sizeof(TMDPRIM_GLT3)));
}

static TMDPRIM *CreatePrimG3(PARTICLE *particle, TMDPRIM *prim, TMDVERT *vert, TMDNORM *norm)
{
	TMDMINE      *ntmd    = &particle->Model;
	TMDPRIM_G3   *dstprim = (TMDPRIM_G3 *)ntmd->pack;
	TMDPRIM_G3   *srcprim = (TMDPRIM_G3 *)prim;
	
	ntmd->object.vert_top   = (long)(ntmd->vert);
	ntmd->object.n_vert     = 3;
	ntmd->object.normal_top = (long)(ntmd->norm);
	ntmd->object.n_normal   = 0;
	
	(*dstprim) = (*srcprim);
	dstprim->vertex0 = 0;
	dstprim->vertex1 = 1;
	dstprim->vertex2 = 2;
	
	ntmd->vert[0] = vert[ srcprim->vertex0 ];
	ntmd->vert[1] = vert[ srcprim->vertex1 ];
	ntmd->vert[2] = vert[ srcprim->vertex2 ];
	return((TMDPRIM *)((char *)prim + sizeof(TMDPRIM_G3)));
}

static TMDPRIM *CreatePrimGt3(PARTICLE *particle, TMDPRIM *prim, TMDVERT *vert, TMDNORM *norm)
{
	TMDMINE      *ntmd    = &particle->Model;
	TMDPRIM_GT3  *dstprim = (TMDPRIM_GT3 *)ntmd->pack;
	TMDPRIM_GT3  *srcprim = (TMDPRIM_GT3 *)prim;
	
	ntmd->object.vert_top   = (long)(ntmd->vert);
	ntmd->object.n_vert     = 3;
	ntmd->object.normal_top = (long)(ntmd->norm);
	ntmd->object.n_normal   = 0;
	
	(*dstprim) = (*srcprim);
	dstprim->vertex0 = 0;
	dstprim->vertex1 = 1;
	dstprim->vertex2 = 2;
	
	ntmd->vert[0] = vert[ srcprim->vertex0 ];
	ntmd->vert[1] = vert[ srcprim->vertex1 ];
	ntmd->vert[2] = vert[ srcprim->vertex2 ];
	return((TMDPRIM *)((char *)prim + sizeof(TMDPRIM_GT3)));
}

static TMDPRIM *CreatePrimFl4(PARTICLE *particle, TMDPRIM *prim, TMDVERT *vert, TMDNORM *norm)
{
	TMDMINE      *ntmd    = &particle->Model;
	TMDPRIM_FL4  *dstprim = (TMDPRIM_FL4 *)ntmd->pack;
	TMDPRIM_FL4  *srcprim = (TMDPRIM_FL4 *)prim;
	
	ntmd->object.vert_top   = (long)(ntmd->vert);
	ntmd->object.n_vert     = 4;
	ntmd->object.normal_top = (long)(ntmd->norm);
	ntmd->object.n_normal   = 1;
	
	(*dstprim) = (*srcprim);
	dstprim->normal0 = 0;
	dstprim->vertex0 = 0;
	dstprim->vertex1 = 1;
	dstprim->vertex2 = 2;
	dstprim->vertex3 = 3;
	
	ntmd->vert[0] = vert[ srcprim->vertex0 ];
	ntmd->vert[1] = vert[ srcprim->vertex1 ];
	ntmd->vert[2] = vert[ srcprim->vertex2 ];
	ntmd->vert[3] = vert[ srcprim->vertex3 ];
	
	ntmd->norm[0] = norm[ srcprim->normal0 ];
	return((TMDPRIM *)((char *)prim + sizeof(TMDPRIM_FL4)));
}

static TMDPRIM *CreatePrimFld4(PARTICLE *particle, TMDPRIM *prim, TMDVERT *vert, TMDNORM *norm)
{
	TMDMINE      *ntmd    = &particle->Model;
	TMDPRIM_FLD4 *dstprim = (TMDPRIM_FLD4 *)ntmd->pack;
	TMDPRIM_FLD4 *srcprim = (TMDPRIM_FLD4 *)prim;
	
	ntmd->object.vert_top   = (long)(ntmd->vert);
	ntmd->object.n_vert     = 4;
	ntmd->object.normal_top = (long)(ntmd->norm);
	ntmd->object.n_normal   = 1;
	
	(*dstprim) = (*srcprim);
	dstprim->normal0 = 0;
	dstprim->vertex0 = 0;
	dstprim->vertex1 = 1;
	dstprim->vertex2 = 2;
	dstprim->vertex3 = 3;
	
	ntmd->vert[0] = vert[ srcprim->vertex0 ];
	ntmd->vert[1] = vert[ srcprim->vertex1 ];
	ntmd->vert[2] = vert[ srcprim->vertex2 ];
	ntmd->vert[3] = vert[ srcprim->vertex3 ];
	
	ntmd->norm[0] = norm[ srcprim->normal0 ];
	return((TMDPRIM *)((char *)prim + sizeof(TMDPRIM_FLD4)));
}

static TMDPRIM *CreatePrimFlt4(PARTICLE *particle, TMDPRIM *prim, TMDVERT *vert, TMDNORM *norm)
{
	TMDMINE      *ntmd    = &particle->Model;
	TMDPRIM_FLT4 *dstprim = (TMDPRIM_FLT4 *)ntmd->pack;
	TMDPRIM_FLT4 *srcprim = (TMDPRIM_FLT4 *)prim;
	
	ntmd->object.vert_top   = (long)(ntmd->vert);
	ntmd->object.n_vert     = 4;
	ntmd->object.normal_top = (long)(ntmd->norm);
	ntmd->object.n_normal   = 1;
	
	(*dstprim) = (*srcprim);
	dstprim->normal0 = 0;
	dstprim->vertex0 = 0;
	dstprim->vertex1 = 1;
	dstprim->vertex2 = 2;
	dstprim->vertex3 = 3;
	
	ntmd->vert[0] = vert[ srcprim->vertex0 ];
	ntmd->vert[1] = vert[ srcprim->vertex1 ];
	ntmd->vert[2] = vert[ srcprim->vertex2 ];
	ntmd->vert[3] = vert[ srcprim->vertex3 ];
	
	ntmd->norm[0] = norm[ srcprim->normal0 ];
	return((TMDPRIM *)((char *)prim + sizeof(TMDPRIM_FLT4)));
}

static TMDPRIM *CreatePrimF4(PARTICLE *particle, TMDPRIM *prim, TMDVERT *vert, TMDNORM *norm)
{
	TMDMINE      *ntmd    = &particle->Model;
	TMDPRIM_F4   *dstprim = (TMDPRIM_F4 *)ntmd->pack;
	TMDPRIM_F4   *srcprim = (TMDPRIM_F4 *)prim;
	
	ntmd->object.vert_top   = (long)(ntmd->vert);
	ntmd->object.n_vert     = 4;
	ntmd->object.normal_top = (long)(ntmd->norm);
	ntmd->object.n_normal   = 0;
	
	(*dstprim) = (*srcprim);
	dstprim->vertex0 = 0;
	dstprim->vertex1 = 1;
	dstprim->vertex2 = 2;
	dstprim->vertex3 = 3;
	
	ntmd->vert[0] = vert[ srcprim->vertex0 ];
	ntmd->vert[1] = vert[ srcprim->vertex1 ];
	ntmd->vert[2] = vert[ srcprim->vertex2 ];
	ntmd->vert[3] = vert[ srcprim->vertex3 ];
	return((TMDPRIM *)((char *)prim + sizeof(TMDPRIM_F4)));
}

static TMDPRIM *CreatePrimFt4(PARTICLE *particle, TMDPRIM *prim, TMDVERT *vert, TMDNORM *norm)
{
	TMDMINE      *ntmd    = &particle->Model;
	TMDPRIM_FT4  *dstprim = (TMDPRIM_FT4 *)ntmd->pack;
	TMDPRIM_FT4  *srcprim = (TMDPRIM_FT4 *)prim;
	
	ntmd->object.vert_top   = (long)(ntmd->vert);
	ntmd->object.n_vert     = 4;
	ntmd->object.normal_top = (long)(ntmd->norm);
	ntmd->object.n_normal   = 0;
	
	(*dstprim) = (*srcprim);
	dstprim->vertex0 = 0;
	dstprim->vertex1 = 1;
	dstprim->vertex2 = 2;
	dstprim->vertex3 = 3;
	
	ntmd->vert[0] = vert[ srcprim->vertex0 ];
	ntmd->vert[1] = vert[ srcprim->vertex1 ];
	ntmd->vert[2] = vert[ srcprim->vertex2 ];
	ntmd->vert[3] = vert[ srcprim->vertex3 ];
	return((TMDPRIM *)((char *)prim + sizeof(TMDPRIM_FT4)));
}

static TMDPRIM *CreatePrimGl4(PARTICLE *particle, TMDPRIM *prim, TMDVERT *vert, TMDNORM *norm)
{
	TMDMINE      *ntmd    = &particle->Model;
	TMDPRIM_GL4  *dstprim = (TMDPRIM_GL4 *)ntmd->pack;
	TMDPRIM_GL4  *srcprim = (TMDPRIM_GL4 *)prim;
	
	ntmd->object.vert_top   = (long)(ntmd->vert);
	ntmd->object.n_vert     = 4;
	ntmd->object.normal_top = (long)(ntmd->norm);
	ntmd->object.n_normal   = 4;
	
	(*dstprim) = (*srcprim);
	dstprim->normal0 = 0;
	dstprim->vertex0 = 0;
	dstprim->normal1 = 1;
	dstprim->vertex1 = 1;
	dstprim->normal2 = 2;
	dstprim->vertex2 = 2;
	dstprim->normal3 = 3;
	dstprim->vertex3 = 3;
	
	ntmd->vert[0] = vert[ srcprim->vertex0 ];
	ntmd->vert[1] = vert[ srcprim->vertex1 ];
	ntmd->vert[2] = vert[ srcprim->vertex2 ];
	ntmd->vert[3] = vert[ srcprim->vertex3 ];
	
	ntmd->norm[0] = norm[ srcprim->normal0 ];
	ntmd->norm[1] = norm[ srcprim->normal1 ];
	ntmd->norm[2] = norm[ srcprim->normal2 ];
	ntmd->norm[3] = norm[ srcprim->normal3 ];
	return((TMDPRIM *)((char *)prim + sizeof(TMDPRIM_GL4)));
}

static TMDPRIM *CreatePrimGld4(PARTICLE *particle, TMDPRIM *prim, TMDVERT *vert, TMDNORM *norm)
{
	TMDMINE      *ntmd    = &particle->Model;
	TMDPRIM_GLD4 *dstprim = (TMDPRIM_GLD4 *)ntmd->pack;
	TMDPRIM_GLD4 *srcprim = (TMDPRIM_GLD4 *)prim;
	
	ntmd->object.vert_top   = (long)(ntmd->vert);
	ntmd->object.n_vert     = 4;
	ntmd->object.normal_top = (long)(ntmd->norm);
	ntmd->object.n_normal   = 4;
	
	(*dstprim) = (*srcprim);
	dstprim->normal0 = 0;
	dstprim->vertex0 = 0;
	dstprim->normal1 = 1;
	dstprim->vertex1 = 1;
	dstprim->normal2 = 2;
	dstprim->vertex2 = 2;
	dstprim->normal3 = 3;
	dstprim->vertex3 = 3;
	
	ntmd->vert[0] = vert[ srcprim->vertex0 ];
	ntmd->vert[1] = vert[ srcprim->vertex1 ];
	ntmd->vert[2] = vert[ srcprim->vertex2 ];
	ntmd->vert[3] = vert[ srcprim->vertex3 ];
	
	ntmd->norm[0] = norm[ srcprim->normal0 ];
	ntmd->norm[1] = norm[ srcprim->normal1 ];
	ntmd->norm[2] = norm[ srcprim->normal2 ];
	ntmd->norm[3] = norm[ srcprim->normal3 ];
	return((TMDPRIM *)((char *)prim + sizeof(TMDPRIM_GLD4)));
}

static TMDPRIM *CreatePrimGlt4(PARTICLE *particle, TMDPRIM *prim, TMDVERT *vert, TMDNORM *norm)
{
	TMDMINE      *ntmd    = &particle->Model;
	TMDPRIM_GLT4 *dstprim = (TMDPRIM_GLT4 *)ntmd->pack;
	TMDPRIM_GLT4 *srcprim = (TMDPRIM_GLT4 *)prim;
	
	ntmd->object.vert_top   = (long)(ntmd->vert);
	ntmd->object.n_vert     = 4;
	ntmd->object.normal_top = (long)(ntmd->norm);
	ntmd->object.n_normal   = 4;
	
	(*dstprim) = (*srcprim);
	dstprim->normal0 = 0;
	dstprim->vertex0 = 0;
	dstprim->normal1 = 1;
	dstprim->vertex1 = 1;
	dstprim->normal2 = 2;
	dstprim->vertex2 = 2;
	dstprim->normal3 = 3;
	dstprim->vertex3 = 3;
	
	ntmd->vert[0] = vert[ srcprim->vertex0 ];
	ntmd->vert[1] = vert[ srcprim->vertex1 ];
	ntmd->vert[2] = vert[ srcprim->vertex2 ];
	ntmd->vert[3] = vert[ srcprim->vertex3 ];
	
	ntmd->norm[0] = norm[ srcprim->normal0 ];
	ntmd->norm[1] = norm[ srcprim->normal1 ];
	ntmd->norm[2] = norm[ srcprim->normal2 ];
	ntmd->norm[3] = norm[ srcprim->normal3 ];
	return((TMDPRIM *)((char *)prim + sizeof(TMDPRIM_GLT4)));
}

static TMDPRIM *CreatePrimG4(PARTICLE *particle, TMDPRIM *prim, TMDVERT *vert, TMDNORM *norm)
{
	TMDMINE      *ntmd    = &particle->Model;
	TMDPRIM_G4   *dstprim = (TMDPRIM_G4 *)ntmd->pack;
	TMDPRIM_G4   *srcprim = (TMDPRIM_G4 *)prim;
	
	ntmd->object.vert_top   = (long)(ntmd->vert);
	ntmd->object.n_vert     = 4;
	ntmd->object.normal_top = (long)(ntmd->norm);
	ntmd->object.n_normal   = 0;
	
	(*dstprim) = (*srcprim);
	dstprim->vertex0 = 0;
	dstprim->vertex1 = 1;
	dstprim->vertex2 = 2;
	dstprim->vertex3 = 3;
	
	ntmd->vert[0] = vert[ srcprim->vertex0 ];
	ntmd->vert[1] = vert[ srcprim->vertex1 ];
	ntmd->vert[2] = vert[ srcprim->vertex2 ];
	ntmd->vert[3] = vert[ srcprim->vertex3 ];
	return((TMDPRIM *)((char *)prim + sizeof(TMDPRIM_G4)));
}

static TMDPRIM *CreatePrimGt4(PARTICLE *particle, TMDPRIM *prim, TMDVERT *vert, TMDNORM *norm)
{
	TMDMINE      *ntmd    = &particle->Model;
	TMDPRIM_GT4  *dstprim = (TMDPRIM_GT4 *)ntmd->pack;
	TMDPRIM_GT4  *srcprim = (TMDPRIM_GT4 *)prim;
	
	ntmd->object.vert_top   = (long)(ntmd->vert);
	ntmd->object.n_vert     = 4;
	ntmd->object.normal_top = (long)(ntmd->norm);
	ntmd->object.n_normal   = 0;
	
	(*dstprim) = (*srcprim);
	dstprim->vertex0 = 0;
	dstprim->vertex1 = 1;
	dstprim->vertex2 = 2;
	dstprim->vertex3 = 3;
	
	ntmd->vert[0] = vert[ srcprim->vertex0 ];
	ntmd->vert[1] = vert[ srcprim->vertex1 ];
	ntmd->vert[2] = vert[ srcprim->vertex2 ];
	ntmd->vert[3] = vert[ srcprim->vertex3 ];
	return((TMDPRIM *)((char *)prim + sizeof(TMDPRIM_GT4)));
}

static TMDPRIM *CreatePrimL(PARTICLE *particle, TMDPRIM *prim, TMDVERT *vert, TMDNORM *norm)
{
	TMDMINE      *ntmd    = &particle->Model;
	TMDPRIM_L    *dstprim = (TMDPRIM_L *)ntmd->pack;
	TMDPRIM_L    *srcprim = (TMDPRIM_L *)prim;
	
	ntmd->object.vert_top   = (long)(ntmd->vert);
	ntmd->object.n_vert     = 2;
	ntmd->object.normal_top = (long)(ntmd->norm);
	ntmd->object.n_normal   = 0;
	
	(*dstprim) = (*srcprim);
	dstprim->vertex0 = 0;
	dstprim->vertex1 = 1;
	
	ntmd->vert[0] = vert[ srcprim->vertex0 ];
	ntmd->vert[1] = vert[ srcprim->vertex1 ];
	return((TMDPRIM *)((char *)prim + sizeof(TMDPRIM_L)));
}

static TMDPRIM *CreatePrimLd(PARTICLE *particle, TMDPRIM *prim, TMDVERT *vert, TMDNORM *norm)
{
	TMDMINE      *ntmd    = &particle->Model;
	TMDPRIM_LD   *dstprim = (TMDPRIM_LD *)ntmd->pack;
	TMDPRIM_LD   *srcprim = (TMDPRIM_LD *)prim;
	
	ntmd->object.vert_top   = (long)(ntmd->vert);
	ntmd->object.n_vert     = 2;
	ntmd->object.normal_top = (long)(ntmd->norm);
	ntmd->object.n_normal   = 0;
	
	(*dstprim) = (*srcprim);
	dstprim->vertex0 = 0;
	dstprim->vertex1 = 1;
	
	ntmd->vert[0] = vert[ srcprim->vertex0 ];
	ntmd->vert[1] = vert[ srcprim->vertex1 ];
	return((TMDPRIM *)((char *)prim + sizeof(TMDPRIM_LD)));
}

static TMDPRIM *CreatePrimS(PARTICLE *particle, TMDPRIM *prim, TMDVERT *vert, TMDNORM *norm)
{
	TMDMINE      *ntmd    = &particle->Model;
	TMDPRIM_S    *dstprim = (TMDPRIM_S *)ntmd->pack;
	TMDPRIM_S    *srcprim = (TMDPRIM_S *)prim;
	
	ntmd->object.vert_top   = (long)(ntmd->vert);
	ntmd->object.n_vert     = 1;
	ntmd->object.normal_top = (long)(ntmd->norm);
	ntmd->object.n_normal   = 0;
	
	(*dstprim) = (*srcprim);
	dstprim->vertex0 = 0;
	
	ntmd->vert[0] = vert[ srcprim->vertex0 ];
	return((TMDPRIM *)((char *)prim + sizeof(TMDPRIM_S)));
}

static TMDPRIM *CreatePrimSf(PARTICLE *particle, TMDPRIM *prim, TMDVERT *vert, TMDNORM *norm)
{
	TMDMINE      *ntmd    = &particle->Model;
	TMDPRIM_SF   *dstprim = (TMDPRIM_SF *)ntmd->pack;
	TMDPRIM_SF   *srcprim = (TMDPRIM_SF *)prim;
	
	ntmd->object.vert_top   = (long)(ntmd->vert);
	ntmd->object.n_vert     = 1;
	ntmd->object.normal_top = (long)(ntmd->norm);
	ntmd->object.n_normal   = 0;
	
	(*dstprim) = (*srcprim);
	dstprim->vertex0 = 0;
	
	ntmd->vert[0] = vert[ srcprim->vertex0 ];
	return((TMDPRIM *)((char *)prim + sizeof(TMDPRIM_SF)));
}

static TMDPRIM *CreatePrimUk(PARTICLE *particle, TMDPRIM *prim, TMDVERT *vert, TMDNORM *norm)
{
	printf("Unknown primitive - mode is 0x%04x (len = %d)\n",
				(short)(prim->mode & TMD_PRIMTYPE_MASK),
				(short)(prim->ilen));
	return((TMDPRIM *)((long *)prim + prim->ilen + 1));
}

/*
 * Update various data structures common to all particles in the system
 * ----------------------------------------------------------------------------
 */

static void MakeTmdPos(PARTICLE *particle, GsCOORDINATE2 *super, int ox, int oy, int oz)
{
	TMDMINE	*ntmd = &particle->Model;
	TMDVERT *vert;
	int	i, nvert, mx, my, mz;
	int	r1, r2, spd;
	
	/* Update the modelling data for the new TMD created */
	GsMapModelingData((unsigned long *)((long)ntmd+sizeof(long)));
	
	/*
	 * Find the center of the primitive created and use it
	 * as the origin.  All vertices are updated so that
	 * they are relative to the new origin.
	 */
	vert = (TMDVERT *)ntmd->object.vert_top;
	nvert = ntmd->object.n_vert;
	mx = 0, my = 0, mz = 0;
	for (i = 0; i < nvert; i++) {
		mx += vert[i].vx, my += vert[i].vy, mz += vert[i].vz;
	}
	mx = mx/nvert, my = my/nvert, mz = mz/nvert;
	for (i = 0; i < nvert; i++) {
		vert[i].vx -= mx, vert[i].vy -= my, vert[i].vz -= mz;
	}
	
	/*
	 * Initialize PlayStation library coordinate system for this
	 * object and initialize it's position in the world
	 */
	GsInitCoordinate2(super, &particle->Pos);
	particle->Pos.coord.t[0] = ox + mx;
	particle->Pos.coord.t[1] = oy + my;
	particle->Pos.coord.t[2] = oz + mz;
	
	/* Link the object to the TMD just created */
	GsLinkObject4(((unsigned long)ntmd)+sizeof(TMDHEAD), &particle->Object, 0);
	
	/* Initialize the angular inclination of the particle */
	particle->Angle.vx = 0;
	particle->Angle.vy = 0;
	particle->Angle.vz = 0;
	
	/* Create the translation vector for this particle */
	spd = SPEED_MOVE_MIN + rand() % SPEED_MOVE_RAND;
	r1 = rand() % ONE, r2 = rand() % ONE;
	particle->TranslationVect.vx = (MySin(r1) * spd) >> 10;
	particle->TranslationVect.vy = (MyCos(r1) * spd) >> 10;
	particle->TranslationVect.vz = (MySin(r2) * spd) >> 10;
	
	/* Create the rotation vector for this particle */
	spd = SPEED_ROTATION_MIN + rand() % SPEED_ROTATION_RAND;
	r1 = rand() % ONE, r2 = rand() % ONE;
	particle->RotationVect.vx = (MySin(r1) * spd) >> 10;
	particle->RotationVect.vy = (MyCos(r1) * spd) >> 10;
	particle->RotationVect.vz = (MySin(r2) * spd) >> 10;
	
	/*
	 *Initialize the life span of the particle in terms of
	 * the number of frames.
	 */
	particle->NoOfTimes = SPEED_VANISH_MIN + rand() % SPEED_VANISH_RAND;
	return;
}

/*
 * The following provide routines to perform implosion effects
 * It prepares an object to be reconstituted from it's particles
 * which had been exploded earlier.
 *
 * Call ImplosionWaitData() first to finish all Explosion/Implosion
 * particles to expire.
 *
 * Call ImplosionInitData() next to prepare for Implosion.
 *
 * Call ImplosionMakeData() prepares particle data.  It can be called
 * multiple times for different TMD models to create more particles.
 *
 * Call ImplosionDoneData() to start the Implosion process.  It basically
 * sets a local flag to TRUE whenever an implosion is happening and it will
 * not allow another implosion to be activated while another implosion is
 * already on.
 *
 * The routines can be used as follows :-
 *
 *		* Wait for all particles to expire *
 *		if (ImplosionWaitData())
 *			return;
 *		
 *		* Initialize the implosion data *
 *		ImplosionInitData();
 *		
 *		* Prepare the implosion data *
 *		ImplosionMakeData(Robo.Model->ModelPtr, 0,
 *							&Cube[0].Pos, 
 *							Robo.Pos.coord.t[0],
 *							Robo.Pos.coord.t[1],
 *							Robo.Pos.coord.t[2]);
 *		
 *		* Prepare the implosion data *
 *		ImplosionMakeData(Prop.Model->ModelPtr, 0,
 *							&Cube[0].Pos, 
 *							Prop.Pos.coord.t[0],
 *							Prop.Pos.coord.t[1],
 *							Prop.Pos.coord.t[2]);
 *		
 *		* Finish the implosion data initialization *
 *		ImplosionDoneData();
 *		
 * ----------------------------------------------------------------------------
 */

/* Implosion flag to stop another implosion to start while one is already on */

static short ImplosionOnFlag = FALSE;
		
/* Wait for particle system to expire data */

short ImplosionWaitData(short startnow)
{
	/* If the user wants the implosion to start immediately */
	if (startnow) {
		/* kill all particles by forcing their lifespan to zero */
		short i;
		for (i = 0; i < PARTICLE_MAXNO; i++) {
			/* Set the life span of the particle to zero */
			ParticleList[i].NoOfTimes = 0;
		}
		
		/* There are no active particles */
		NoParticles = 0;
		
		/* Initialize the top index in the particle list */
		ParticleListTop = 0;
	}
	
	/*
	 * If there are any particle still active in the system
	 * it returns TRUE, otherwise it returns FALSE.
	 */
	return((NoParticles > 0) ? TRUE : FALSE);
}

/* Initialize implosion data */

short ImplosionInitData(void)
{
	/* Quit if another implosion system is on */
	if (ImplosionOnFlag)
		return(FALSE);
	
	/*
	 * Initialize the top if the implosion queue to
 	 * the begining of the particle list
 	 */
	ParticleListTop = 0;
	return(TRUE);
}

/* Create Implosion data */

void ImplosionMakeData(unsigned long *mdata, int id, GsCOORDINATE2 *super, int ox, int oy, int oz)
{
	PARTICLE *particle;
	MATRIX tmp;
	int oldListTop;
	int i,k;
		
	/* Quit if another implosion system is currently on */
	if (ImplosionOnFlag)
		return;
	
	/* Prepare data for implosion */
	oldListTop = ParticleListTop;
	ExplosionMakeData(mdata, id, super, ox, oy, oz);
	
	/* Move/rotate each particle to it's minimum life span possible times */
	for (k = 0; k < SPEED_VANISH_MIN; k++) {
		/* Rotate, translate and display the particles */
		for (particle = &ParticleList[ oldListTop ], i = oldListTop; i < ParticleListTop; i++, particle++) {
			/* Rotate the particle */
			particle->Angle.vx = (particle->Angle.vx + particle->RotationVect.vx + ONE) % ONE;
			particle->Angle.vy = (particle->Angle.vy + particle->RotationVect.vy + ONE) % ONE;
			particle->Angle.vz = (particle->Angle.vz + particle->RotationVect.vz + ONE) % ONE;
			
			/* Apply the translation vector to the particle */
			tmp = GsIDMATRIX;
			tmp.t[0] = particle->Pos.coord.t[0] + particle->TranslationVect.vx;
			tmp.t[1] = particle->Pos.coord.t[1] + particle->TranslationVect.vy;
			tmp.t[2] = particle->Pos.coord.t[2] + particle->TranslationVect.vz;
			
			/* Update the rotation matrix of the particle */
			RotMatrix(&particle->Angle, &tmp);
			particle->Pos.coord = tmp;
			particle->Pos.flg = 0;
		}
	}
	
	/*
	 * Set the translation and rotation vector to opposite of what
	 * we used to create particles in the above loop
	 */
	for (particle = &ParticleList[ oldListTop ], i = oldListTop; i < ParticleListTop; i++, particle++) {
		/* Reverse the rotation vector */
		particle->RotationVect.vx = -particle->RotationVect.vx;
		particle->RotationVect.vy = -particle->RotationVect.vy;
		particle->RotationVect.vz = -particle->RotationVect.vz;
		
		/* Reverse the translation vector */
		particle->TranslationVect.vx = -particle->TranslationVect.vx;
		particle->TranslationVect.vy = -particle->TranslationVect.vy;
		particle->TranslationVect.vz = -particle->TranslationVect.vz;
			
		/* Set the life span of the particle */
		particle->NoOfTimes = SPEED_VANISH_MIN;
	}
	return;
}	

/*
 * Finish the implosion data preparation - toggles the flag to set
 * if implosion system routines are available or not
 */

void ImplosionDoneData(void)
{
	/* Toggle the implosion system flag */
	ImplosionOnFlag = (ImplosionOnFlag ? FALSE : TRUE);
	return;
}	

/*
 * End of File
 * ----------------------------------------------------------------------------
 */
