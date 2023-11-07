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






	// main loop will need these
extern int BackRed, BackGreen, BackBlue;




void InitialiseLighting (void);