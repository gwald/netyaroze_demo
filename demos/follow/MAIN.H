// what is exported from main: what submodules need to know

// typically very very little



extern int frameNumber;

extern int GameHasFinishedAndResultsNeedDisplayingFlag;

#define PLAY_ON_INNER_SIMPLE_OUTER_TEXTURE 1
#define PLAY_ON_INNER_FLASH_OUTER_TEXTURE 2
#define PLAY_ON_OUTER_ONLY 3
#define PLAY_ON_BOTH_SURFACES 4

extern int BackgroundGraphicsFlag;
