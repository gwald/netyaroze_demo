#include "player.h"


	 // THINK MORE ABOUT APPLICATION CODE CALLING



Player ThePlayer;		  // single one

Player ListOfPlayers[MAX_PLAYERS];

int NumberOfPlayers;







void InitialisePlayerClass (void)
{
	int i;
	Player* thisPlayer;

	NumberOfPlayers = 0;

	for (i = 0; i < MAX_PLAYERS; i++)
		{
		thisPlayer = &ListOfPlayers[i];

		InitPlayerToVoid(thisPlayer);
		}
}





void InitPlayerToVoid (Player* player)
{
	int i;

	player->status = UNUSED;
	player->type = NONE;
	player->id = -1;

	strcpy(player->name, "PLAYER_WITH_NO_NAME");

	player->score = 0;

	for (i = 0; i < MAX_ATTACHED_OBJECTS; i++)
		{
		player->objectList[i] = -1;
		}
}

	




void UpdatePlayerList (void)
{
	int i;
	Player* thisPlayer;

	for (i = 0; i < MAX_PLAYERS; i++)
		{
		thisPlayer = &ListOfPlayers[i];

		if (thisPlayer->status == LIVING)
			{
			// HANDLE
			}
		}
}





void BringPlayerToLife (Player* player, int type, 
		int startingStatus,	char* name)
{
	assert( strlen(name) < LONGEST_PLAYER_NAME);
	assert(type == HUMAN || type == COMPUTER);
	assert(startingStatus == LIVING
			|| startingStatus == DEAD
			|| startingStatus == NON_INTERACTIVE);

	player->type = type;
	strcpy(player->name, name);
	player->status = startingStatus;
}



Player* CreatePlayer (int type, 
		int startingStatus,	char* name)
{
	Player* player;

	player = GetNextFreePlayer();

	assert( strlen(name) < LONGEST_PLAYER_NAME);
	assert(type == HUMAN || type == COMPUTER);
	assert(startingStatus == LIVING
			|| startingStatus == DEAD
			|| startingStatus == NON_INTERACTIVE);

	player->type = type;
	strcpy(player->name, name);
	player->status = startingStatus;
}

	




void AddPlayerToPlayerList (Player* player)
{
}





void RemovePlayerFromPlayerList (Player* player)
{
}




	// note: count includes DEAD and NON_INTERACTIVE players
int CountNumberOfPlayers (void)
{
	int count = 0;
	int i;

	for (i = 0; i < MAX_PLAYERS; i++)
		{
		if (ListOfPlayers[i].status == UNUSED)
			count++;
		}

	return count;
}





Player* GetNextFreePlayer (void)
{
	Player* player = NULL;
	int i;

	for (i = 0; i < MAX_PLAYERS; i++)
		{
		if (ListOfPlayers[i].status == UNUSED)
			{
			player = &ListOfPlayers[i];
			break;
			}
		}

	return player;
}
