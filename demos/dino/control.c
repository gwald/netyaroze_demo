/*
 *		"control.c"
 *
 *	Copyright (C) 1996 Sony Computer Entertainment Inc.
 *	All Rights Reserved
 */


#include <libps.h>
#include "control.h"


int cntrlarry[CTLMAX][CTLTIME]; 


CTLFUNC ctlfc[CTLMAX];

void init_cntrlarry(int *cnvwave, int number);
			
void set_cntrl(u_long frm);
			


void init_cntrlarry(int *cnvwave, int number)
{
	int i,j;

	for(i = 0 ; i < CTLMAX; i++) {
		ctlfc[i].cnv = cnvwave;
		ctlfc[i].num = number;
		ctlfc[i].in = 0;
		ctlfc[i].out = 0;
		for(j = 0 ; j < CTLTIME; j++) cntrlarry[i][j]=0;
	}
}


void set_cntrl(u_long frm)
{
	int i,j;

	for(i = 0 ; i < CTLMAX; i++) {
		cntrlarry[i][ (frm) % ctlfc[i].num ] = ctlfc[i].in;
							
		ctlfc[i].out=0;
		for(j = 0 ; j < ctlfc[i].num; j++) {
		   ctlfc[i].out += cntrlarry[i][(frm - j ) % ctlfc[i].num] * 
				(*(ctlfc[i].cnv+j));
		}
		ctlfc[i].out = ctlfc[i].out >> 12;	
	}
}
