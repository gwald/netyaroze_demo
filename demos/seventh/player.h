#include <libps.h>

#include <stdio.h>

#include "general.h"

#include "asssert.h"




	// player type
#define HUMAN 0
#define COMPUTER 1


	// player status
#define LIVING 0
#define DEAD 1
#define NON_INTERACTIVE 2
#define UNUSED 3


#define LONGEST_PLAYER_NAME 64


	// associated objects
#define MAX_ATTACHED_OBJECTS 8


#define MAX_PLAYERS 8

typedef struct
{
	int status;
	int type;
	int id;

	char name[LONGEST_PLAYER_NAME];

	int score;

	int objectList[MAX_ATTACHED_OBJECTS];
} Player;


	 


	// prototypes

void InitialisePlayerClass (void);

void InitPlayerToVoid (Player* player);

void UpdatePlayerList (void);


void BringPlayerToLife (Player* player, int type, 
		int startingStatus,	char* name);


Player* CreatePlayer (int type, 
		int startingStatus,	char* name);



void AddPlayerToPlayerList (Player* player);

void RemovePlayerFromPlayerList (Player* player);

int CountNumberOfPlayers (void);


Player* GetNextFreePlayer (void);