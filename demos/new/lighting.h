#include <libps.h>



// EXTENSIONS

// overall lighting mode: static, changing
// one for EACH type: back colour
// 3 light sources; ambient colour; fogging

// different states for each type of overall mode

// single HandleLighting function to be called by <main> once per frame


// lighting distortions: for flat lights
// circle the directions
// change the colours: blend into each other cyclically

// for fog: near <-> far, change fog colour




#define EVEN_STEVENS 0
#define COLOURED_AXES 1



#define R_X_G_Y_B_Z 0
#define R_X_B_Y_G_Z 1
#define G_X_R_Y_B_Z 2
#define G_X_B_Y_R_Z 3
#define B_X_G_Y_R_Z 4
#define B_X_R_Y_G_Z 5




	// main loop will need these
extern int BackRed, BackGreen, BackBlue;




void InitialiseLighting (void);