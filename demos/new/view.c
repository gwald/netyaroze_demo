#include "view.h"




long ProjectionDistance;

GsRVIEW2 TheView;



GsVIEW2 TheOtherView;	   // NOT USED YET
int CurentViewType;		   // NOT USED YET



void InitialiseView (void)
{
	ProjectionDistance = 192;			
	GsSetProjection(ProjectionDistance);  

	TheView.vrx = 0; 
	TheView.vry = 0; 
	TheView.vrz = 0; 

	TheView.vpx = 0; 
	TheView.vpy = 0; 
	TheView.vpz = 220;
	
	TheView.rz = 0;

	TheView.super = &TheShip.coord;

	GsSetRefView2(&TheView);
}


