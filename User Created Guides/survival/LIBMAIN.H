/*
 * LIBMAIN.H
 *
 * Copyright (C) 1993-1997 by Sony Computer Entertainment of America, Inc.
 * All rights Reserved
 * ----------------------------------------------------------------------------
 */

#ifndef	__YAROZE_LIBMAIN_H
#define	__YAROZE_LIBMAIN_H

/*
 * This file contains generic macro definitions used throughout the sample
 * and it includes all system header files.
 *
 * This should be the first include file in all source code and header files
 * whereever any of the system macro, data definition or functions are used.
 *
 * Care has been taken in each header file to avoid multiple inclusion of
 * the same header file in a source file. So, you should always include this
 * file at the begining of all such files.  This will ensure an orderless
 * inclusion of various header files in a source code or another header file.
 * ----------------------------------------------------------------------------
 */

#include <libps.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <rand.h>
#include <r3000.h>
#include <memory.h>
#include <malloc.h>
#include <fs.h>
#include <abs.h>
#include <convert.h>
#include <ctype.h>
#include <sys/types.h>

/*
 * Generic macros used throughout
 * ----------------------------------------------------------------------------
 */

#ifndef	ON
#define	ON			1
#endif

#ifndef	OFF
#define	OFF			0
#endif

#ifndef	TRUE
#define	TRUE		1
#endif

#ifndef	FALSE
#define	FALSE		0
#endif

#ifndef	NULL
#define	NULL		(void *)0
#endif

/*
 * End of File
 * ----------------------------------------------------------------------------
 */

#endif
