/*
 * LIBMATH.H
 *
 * Copyright (C) 1993-1997 by Sony Computer Entertainment of America, Inc.
 * All rights Reserved
 * ----------------------------------------------------------------------------
 */

#ifndef	__YAROZE_LIBMATH_H
#define	__YAROZE_LIBMATH_H

/*
 * The header file for math data tables and functions.
 * ----------------------------------------------------------------------------
 */

/*
 * The resolution of the table is 257 where 256 is equal to 360 degrees and
 * 0 means 0 degrees.
 */

extern short SinTable[];
extern short CosTable[];

/*
 * The angle passed to Sine and Cosine should be in PlayStation format
 * i.e. 4096 is 360 degrees.  The range of angle should be -4096 to
 * any positive value.
 */

extern short MySin(short angle);
extern short MyCos(short angle);

/*
 * Absolute value functions
 */

extern short MySabs(short a);
extern int MyAbs(int a);
extern long MyLabs(long a);

/*
 * End of File
 * ----------------------------------------------------------------------------
 */

#endif
