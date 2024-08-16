/*
 * LIBDATA.H
 *
 * Copyright (C) 1993-1997 by Sony Computer Entertainment of America, Inc.
 * All rights Reserved
 * ----------------------------------------------------------------------------
 */

#ifndef	__YAROZE_LIBDATA_H
#define	__YAROZE_LIBDATA_H

/*
 * This file contains information about TMDs, TIMs, SEQs, VABs used in this
 * sample.  Various macros are defined for memory adresses where these data
 * files are loaded.
 *
 * We load these data files on the lower memory area just after the area
 * where PlayStation runtime libraries are loaded, i.e. we start at the
 * memory location 0x80090000.  The executable for the sample gets loaded
 * just after this data area.
 * ----------------------------------------------------------------------------
 */

/* Model and textures for cube faces */

#define	ADR_CUBE_MODEL		(unsigned long *)0x80090000 /*   2744 = 0x00AB8 = 0x00B00 */
#define	ADR_CUBE_TEXTURE	(unsigned long *)0x80090B00 /*  16928 = 0x04220 = 0x04300 */

/* Model and textures for wooden slabs */

#define ADR_SLAB_MODEL		(unsigned long *)0x80094E00 /*    304 = 0x00130 = 0x00200 */
#define ADR_SLAB_TEXTURE	(unsigned long *)0x80095000 /*  16928 = 0x04220 = 0x04300 */

/* Model and textures for the robot */

#define	ADR_ROBO_MODEL		(unsigned long *)0x80099300 /*   3528 = 0x00DC8 = 0x00E00 */
#define	ADR_PROP_MODEL		(unsigned long *)0x8009A100 /*    616 = 0x00268 = 0x00300 */
#define	ADR_ROBO_TEXTURE1	(unsigned long *)0x8009A400 /*    856 = 0x00358 = 0x00400 */
#define	ADR_ROBO_TEXTURE2	(unsigned long *)0x8009A800 /*   4640 = 0x01220 = 0x01300 */

/* Model and textures for the enemy ships */

#define	ADR_SHIP_MODEL		(unsigned long *)0x8009BB00 /*   2696 = 0x00A88 = 0x00B00 */
#define	ADR_SHIP_TEXTURE	(unsigned long *)0x8009C600 /*  49696 = 0x0C220 = 0x0C300 */
												  
/* Model and textures for the remains of the enemy ships */

#define	ADR_BALL_MODEL		(unsigned long *)0x800A8900 /*    304 = 0x00130 = 0x00200 */
#define	ADR_BALL_TEXTURE	ADR_SHIP_TEXTURE

/* Music/SoundFX data addresses in main memory - VAB header, VAB data and SEQ data */

#define	ADR_MUSIC_VH		(unsigned long *)0x800A8B00 /*   6176 = 0x01820 = 0x01900 */
#define	ADR_MUSIC_VB		(unsigned long *)0x800AA400 /* 269552 = 0x41CF0 = 0x41D00 */
#define	ADR_MUSIC_SEQ		(unsigned long *)0x800EC100 /*  16801 = 0x041A1 = 0x04200 */

/* Source code to be loaded at ------------- 0x800F0300 */

/*
 * End of File
 * ----------------------------------------------------------------------------
 */

#endif
