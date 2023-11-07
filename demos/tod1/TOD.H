/* $PSLibId: Run-time Library Release 4.0$ */
/*
 *	tod.h: TOD play back package definitions
 *
 *
 *		Version 1.00	Feb, 3, 1995
 *
 *		Copyright (C) 1995 by Sony Corporation
 *			All rights Reserved
 */

/*****************/
/* Define Macros */
/*****************/
#define TOD_ATTR	(0)
#define TOD_COORD	(1)
#define TOD_TMDID	(2)
#define TOD_PARENT	(3)
#define TOD_MATRIX	(4)
#define TOD_TMDDATA	(5)
#define TOD_LIGHT	(6)
#define TOD_CAMERA	(7)
#define TOD_OBJCTL	(8)
#define TOD_USER0	(9)
#define TOD_USER1	(10)
#define TOD_USER2	(11)
#define TOD_USER3	(12)
#define TOD_USER4	(13)
#define TOD_SYSFUNC	(15)

#define TOD_CREATE	(0)
#define TOD_NOCREATE	(1)
#define TOD_COORDONLY	(2)

#define TOD_OBJ_UNDEF	(0xffffffff)

/*************/
/* Structure */
/*************/
typedef struct {
    GsDOBJ2 *top;
    int nobj;
    int maxobj;
} TodOBJTABLE;

/*************************/
/* Prototype definitions */
/*************************/
void		TodInitObjTable( TodOBJTABLE *, GsDOBJ2 *, GsCOORDINATE2 *, 
				 GsCOORD2PARAM *, int );
GsDOBJ2		*TodSearchObjByID( TodOBJTABLE *, unsigned long );
GsDOBJ2		*TodCreateNewObj( TodOBJTABLE *, unsigned long );
GsDOBJ2		*TodRemoveObj( TodOBJTABLE *, unsigned long );
unsigned long	*TodSearchTMDByID( unsigned long *, int *, unsigned long );
unsigned long *	TodSetFrame( int, unsigned long *, TodOBJTABLE *,
			     int *, unsigned long *, int );
unsigned long	*TodSetPacket( unsigned long *, TodOBJTABLE *, int *,
			       unsigned long *, int );
