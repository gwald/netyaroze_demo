/*
 *	NY_Release_Prefix.h	- Target #DEFINEs for the Sony PlayStation
 *
 *	Copyright © 1996 metrowerks inc. All Rights Reserved.
 *
 */

#ifndef __ny_Release_prefix__
#define __ny_Release_prefix__

/* general target hardware/os definitions */
#define __dest_os	__PSXOS
#define TARGET_LITTLE_ENDIAN
#define TARGET_HEAP_ALIGNMENT	16
#define TARGET_DEVICE		__NY_PSX
#define LANGUAGE_C

#define NDEBUG

#endif