#include "lighting.h"








int BackRed, BackGreen, BackBlue;

GsF_LIGHT TheLights[3];

int AmbientRed, AmbientGreen, AmbientBlue;

GsFOGPARAM TheFogging;

int OverallLightMode;




void InitialiseLighting (void)
{
		// 'back-colour': what we clear the ordering table with
	BackRed = BackGreen = BackBlue = 0;


		// three flat light sources
	TheLights[0].vx = ONE; TheLights[0].vy = 0; TheLights[0].vz = 0;
	TheLights[0].r = 128; TheLights[0].g = 0; TheLights[0].b = 0;
	GsSetFlatLight(0, &TheLights[0]);

	TheLights[1].vx = 0; TheLights[1].vy = ONE; TheLights[1].vz = 0;
	TheLights[1].r = 0; TheLights[1].g = 128; TheLights[1].b = 0;
	GsSetFlatLight(1, &TheLights[1]);

	TheLights[2].vx = 0; TheLights[2].vy = 0; TheLights[2].vz = ONE;
	TheLights[2].r = 0; TheLights[2].g = 0; TheLights[2].b = 128;
	GsSetFlatLight(2, &TheLights[2]);

		
		// background lighting
	AmbientRed = AmbientGreen = AmbientBlue = ONE/4;
	GsSetAmbient(AmbientRed, AmbientGreen, AmbientBlue);

  
		// distance colour blending
  	TheFogging.dqa = -960;
	TheFogging.dqb = 5120*5120;
	TheFogging.rfc = 0; 
	TheFogging.gfc = 0; 
	TheFogging.bfc = 4;
	GsSetFogParam( &TheFogging);


		// overall lighting conditions
	OverallLightMode = 0;			// no fogging 
	GsSetLightMode(OverallLightMode);
}

