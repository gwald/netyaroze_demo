/*
 *		"mime.c"
 *
 *	Copyright (C) 1996 Sony Computer Entertainment Inc.
 *	All Rights Reserved
 *
 */


#include <libps.h>
#include "mime.h"  
#include "addr.h"

TMDOBJECT tmdobj0[OBJMAX];	
MDFOBJECT mdfvtx0[MIMEMAX];	
MDFOBJECT mdfnrm0[MIMEMAX];	
long mimepr[MIMEMAX];		


TMDDATA tmddt0={1,(SVECTOR *)ORGVTX_BUF_ADDR,(SVECTOR *)0,tmdobj0};

MDFDATA vtxdt0={1,mdfvtx0,mimepr};


void set_tmd_data();		
void set_vertex_mime_data();	
void vertex_mime(); 	   


static void set_tmd_info(u_long *model, TMDDATA *tmddt);
				
static void set_mdf_vertex(u_long *mdfdtvtx, MDFDATA *vtxdt);
				
static void original_vertex_save(TMDDATA *tmddt, SVECTOR *orgvtxbf);
				
static void reset_mime_vertex(TMDDATA *tmddt);
				
static void set_mime_vertex(TMDDATA *tmddt, MDFDATA *vtxdt);
				

/*==MIME Function==================*/


void set_tmd_data()
{
	set_tmd_info((u_long *)MD0_TMD_ADDR, &tmddt0);	
}


void set_vertex_mime_data()
{
	original_vertex_save(&tmddt0, (SVECTOR *)ORGVTX_BUF_ADDR);
						
	set_mdf_vertex((u_long *)MD0_VDF_ADDR, &vtxdt0);
						
}

void vertex_mime()
{
	reset_mime_vertex(&tmddt0);			
	set_mime_vertex(&tmddt0,&vtxdt0);	   
}

static void set_tmd_info(u_long *model, TMDDATA *tmddt)
{
	u_long size,*dop;
	int i,n;

	dop= model;
	dop++;   		   
	dop++;   		  
	n = tmddt->objtotal = *dop;  	
	dop++;
	for(i = 0; i < n; i++){
		tmddt->tmdobj[i].vtxtop = (SVECTOR *)*dop++ ;		
		tmddt->tmdobj[i].vtxtotal = *dop++ ;		
		tmddt->tmdobj[i].nrmtop = (SVECTOR *)*dop++ ;	
		tmddt->tmdobj[i].nrmtotal = *dop++ ;		
		tmddt->tmdobj[i].prmtop = (u_long *)*dop++ ;	
		tmddt->tmdobj[i].prmtotal = *dop++ ;	   
		tmddt->tmdobj[i].scale  = *dop++ ;	   
	} 
        printf("objtotal = %d\n", tmddt->objtotal);
	for(i = 0; i < n; i++){
		printf("v_top%d   = %x\n", 	i, 	tmddt->tmdobj[i].vtxtop );
		printf("v_num%d   = %d\n",	i,  	tmddt->tmdobj[i].vtxtotal );
		printf("nrm_top%d = %x\n",	i,	tmddt->tmdobj[i].nrmtop );
		printf("nrm_num%d = %d\n",	i, 	tmddt->tmdobj[i].nrmtotal );
		printf("prm_top%d = %x\n",	i, 	tmddt->tmdobj[i].prmtop );
		printf("prm_num%d = %d\n",	i, 	tmddt->tmdobj[i].prmtotal );
		printf("scale%d   = %d\n",	i, 	tmddt->tmdobj[i].scale );
	} 
}


static void set_mdf_vertex(u_long *mdfdtvtx, MDFDATA *vtxdt)
{
	int i,n;
	u_long *dop2;

	dop2 = mdfdtvtx;
	n = vtxdt->objtotal = *dop2++;
	for (i = 0; i < n; i++){
		vtxdt->mdfvct[i].object = *dop2++;	    		
		vtxdt->mdfvct[i].offset = *dop2++;	    	
		vtxdt->mdfvct[i].total 	= *dop2++;		 	
		vtxdt->mdfvct[i].top 	= (SVECTOR *)dop2; 
		dop2 += vtxdt->mdfvct[i].total*2;
	}
}


static void original_vertex_save(TMDDATA *tmddt, SVECTOR *orgvtxbf)
{
	SVECTOR *otp,*bsp,*dfp;
	int i,j,n,m;

	bsp = tmddt->orgvtx = orgvtxbf;
	m = tmddt->objtotal;
	n = 0;
	for (j = 0; j < m; j++){
  		otp = tmddt->tmdobj[j].vtxtop;
		bsp += n;
		n = tmddt->tmdobj[j].vtxtotal;
		for (i = 0; i < n; i++) *(bsp+i) = *(otp+i);
	}
}


static void reset_mime_vertex(TMDDATA *tmddt)
{
	SVECTOR *otp,*bsp;
	int i,j,n,m;

	bsp = tmddt->orgvtx;
	m = tmddt->objtotal;
	n = 0;
	for (j = 0; j < m; j++){
  		otp = tmddt->tmdobj[j].vtxtop;
		bsp += n;
		n = tmddt->tmdobj[j].vtxtotal;
		for (i = 0; i < n; i++) *(otp+i) = *(bsp+i);
	}
}


static void set_mime_vertex(TMDDATA *tmddt, MDFDATA *vtxdt)
{
	SVECTOR *otp,*bsp,*dfp;
	int i,n;

	n = vtxdt->objtotal;
	for (i = 0; i < n; i++){
  		otp = tmddt->tmdobj[vtxdt->mdfvct[i].object].vtxtop +
			vtxdt->mdfvct[i].offset;
		dfp = vtxdt->mdfvct[i].top;
		if (vtxdt->mime[i] != 0) 
			gteMIMefunc(otp,dfp, vtxdt->mdfvct[i].total,
				vtxdt->mime[i]);  
	}
}
