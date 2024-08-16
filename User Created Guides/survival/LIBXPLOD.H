/*
 * LIBXPLOD.H
 *
 * Copyright (C) 1993-1997 by Sony Computer Entertainment of America, Inc.
 * All rights Reserved
 * ----------------------------------------------------------------------------
 */

#ifndef	__YAROZE_LIBXPLOD_H
#define	__YAROZE_LIBXPLOD_H

/*
 * This file contains functions which handle explosion of a given object.
 * Object supplied is in a TMD format.
 * ----------------------------------------------------------------------------
 */

#include "libmain.h"

/*
 * Function templates
 * ----------------------------------------------------------------------------
 */

/*
 * prints some debugging info for the explosion data
 */

void  ExplosionDebugData(void);

/*
 * The following function initializes the particle explosion
 * system.  It also sets a bounding limit for the exploded
 * polygon's existance.  The individual TMD will cease to exist
 * once it crosses the boundary defined by the following.
 * The bouning box extends from
 *    (-ExplosionObjectXlim,       (ExplosionObjectXlim,
 *	   -ExplosionObjectYlim,   to   ExplosionObjectYlim,
 * 	   -ExplosionObjectZlim)        ExplosionObjectZlim)
 */

void  ExplosionInitData(int xlim, int ylim, int zlim);

/*
 * Draws the particle explosion system into given ot
 * with the proper shift value used in the main
 * program.
 */

void  ExplosionDrawData(GsOT *ot, long shift);

/*
 * In order to create a particle explosion system, call the
 * following routine.  It takes the ptr to the model data
 * which needs to be exploded, the super to be used for
 * the particle system and offset (x,y,z) values to be given
 * to each poly in the particle system.
 */
void  ExplosionMakeData(unsigned long *mdata, int id,
							GsCOORDINATE2 *super,
							int ox, int oy, int oz);

/*
 * The following provide routines to perform implosion effects
 * It prepares an object to be reconstituted from it's particles
 * which had been exploded earlier.
 */

/* Waits for all particles to expire */
short ImplosionWaitData(short startnow);

/* Initialize an implosion system */
short ImplosionInitData(void);

/* Prepare particle data for implosion system */
void  ImplosionMakeData(unsigned long *mdata, int id,
							GsCOORDINATE2 *super,
							int ox, int oy, int oz);

/* Set the particle implosion data ready/clear */
void  ImplosionDoneData(void);

/*
 * End of File
 * ----------------------------------------------------------------------------
 */

#endif
