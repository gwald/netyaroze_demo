/*
 *		"mime.h"
 *
 *	Copyright (C) 1996 Sony Computer Entertainment Inc.
 *	All Rights Reserved
 */

#define MIMEMAX	16
#define OBJMAX	16


typedef struct {
	SVECTOR *vtxtop;	
	u_long vtxtotal;   
	SVECTOR *nrmtop;  
	u_long nrmtotal;   
	u_long *prmtop;		
	u_long prmtotal;	
	u_long scale;	   
} TMDOBJECT;


typedef struct {
	SVECTOR		*top;	
	u_long		offset;	
	u_long		total;	
	u_long		object;	
} MDFOBJECT;


typedef struct {
	u_long 		objtotal;	
	SVECTOR		*orgvtx;	
	SVECTOR		*orgnrm;	
	TMDOBJECT 	*tmdobj;	
} TMDDATA;


typedef struct {
	u_long 		objtotal;	
	MDFOBJECT	*mdfvct;			
	long		*mime;		
} MDFDATA;
