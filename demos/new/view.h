#include <libps.h>

#include "object.h"

#include "tuto1.h"



// EXTENSIONS

// overall view mode: static, changing
// one for the view, another for projection

// different states for each type of overall mode

// single HandlView function to be called by <main> once per frame


// views: attach to any object, then circle, flyby, local, external
// director; general spline path

// simultaneous changing view distance and projection


// proper view handler 
// (treat view as object moving like a plane in flysim)




	// NOT USED YET
	// flag: using GsRVIEW2 or GsVIEW2 ???
#define REFERENCE_VIEW 0
#define COORDINATE_VIEW 1

extern long ProjectionDistance;

extern GsRVIEW2 TheView;

extern GsVIEW2 TheOtherView;	   // NOT USED YET



#define LOCAL_VIEW 0
#define EXTERNAL_VIEW 1
#define CIRCLING_VIEW 2
#define FLYBY_VIEW 3
#define DIRECTORS_VIEW 4


#define X_Y_PLANE 0
#define X_Z_PLANE 1
#define Y_Z_PLANE 2




void InitialiseView (void);