#include "amxxmodule.h"
#include "pm_defs.h"
#include "pm_shared.h"
#include "entity_state.h"
#include "cbase.h"
#include "bits32.h"

//AMXX forward
enum
{
	FW_Semiclip = 0,
	TOTAL_FORWARDS
};

int g_iForwards[TOTAL_FORWARDS];

//here we backup the physent array
physent_t g_sBackupPhysent[MAX_PHYSENTS];
int g_iNumBackupPhysent;

unsigned g_bitsSolid = 0, g_bitsRestore = 0;

int PM_ShouldCollide(int playerindex, int entindex)
{
	if(g_iForwards[FW_Semiclip] != NULL)
	{
		return !MF_ExecuteForward(g_iForwards[FW_Semiclip], (cell)playerindex, (cell)entindex);
	}
	return 0;
}

void PM_Move(struct playermove_s *ppmove, int server)
{
	int i;
	int iPlayerIndex = ppmove->player_index + 1;

	//backup physent
	g_iNumBackupPhysent = ppmove->numphysent;
	memcpy(g_sBackupPhysent, ppmove->physents, sizeof(physent_t) * ppmove->numphysent);

	//rebuild
	ppmove->numphysent = 0;

	for(i = 0; i < g_iNumBackupPhysent; i++)
	{
		if(!PM_ShouldCollide(iPlayerIndex, g_sBackupPhysent[i].info))
			continue;

		memcpy(&ppmove->physents[ppmove->numphysent ++], &g_sBackupPhysent[i], sizeof(physent_t));
	}

	RETURN_META(MRES_HANDLED);
}

void PM_Move_Post(struct playermove_s *ppmove, int server)
{
	ppmove->numphysent = g_iNumBackupPhysent;
	memcpy(ppmove->physents, g_sBackupPhysent, sizeof(physent_t)*g_iNumBackupPhysent);

	RETURN_META(MRES_HANDLED);
}

void PlayerPreThink(edict_t * pPlayer)
{
	static int Client = 0U;
	static int Iterator = 0U;
	static int Flag = 0U;

	Client = ENTINDEX(pPlayer);

	if (Flag > Client)
	{
		for (Iterator = 1U; Iterator <= gpGlobals->maxClients; Iterator++)
		{
			if (!MF_IsPlayerAlive(Iterator))
			{
				BitsUnSet(g_bitsSolid, Iterator);
				continue;
			}

			pPlayer = MF_GetPlayerEdict(Iterator);

			//g_Player[Iterator].Team = MF_GetPlayerTeamID(Iterator);
			//g_Player[Iterator].Solid = pPlayer->v.solid == SOLID_SLIDEBOX ? true : false;
			if(pPlayer->v.solid == SOLID_SLIDEBOX)
			{
				BitsSet(g_bitsSolid, Iterator);
			}
			else
			{
				BitsUnSet(g_bitsSolid, Iterator);
			}
		}
	}

	Flag = Client;
	
	if (!BitsGet(g_bitsSolid, Client))
	{
		RETURN_META(MRES_IGNORED);
	}

	for (Iterator = 1U; Iterator <= gpGlobals->maxClients; Iterator++)
	{
		
		if (!BitsGet(g_bitsSolid, Iterator) || Client == Iterator || PM_ShouldCollide(Iterator, Client))
		{
			continue;
		}

		pPlayer = MF_GetPlayerEdict(Iterator);

		pPlayer->v.solid = SOLID_NOT;

		BitsSet(g_bitsRestore, Iterator);
	}

	RETURN_META(MRES_IGNORED);
}

void PlayerPostThink(edict_t *)
{
	static int Iterator = 0U;

	static edict_t * pPlayer = NULL;

	for (Iterator = 1U; Iterator <= gpGlobals->maxClients; Iterator++)
	{
		if (BitsGet(g_bitsRestore, Iterator))
		{
			pPlayer = MF_GetPlayerEdict(Iterator);

			pPlayer->v.solid = SOLID_SLIDEBOX;

			BitsUnSet(g_bitsRestore, Iterator);
		}
	}

	RETURN_META(MRES_IGNORED);
}

int FN_AddToFullPack_Post(struct entity_state_s *state, int e, edict_t *ent, edict_t *host, int hostflags, int player, unsigned char *pSet)
{
	if(player && (ent != host))
	{
		if(!PM_ShouldCollide(ENTINDEX(host),ENTINDEX(ent)))
		{
			state->solid = SOLID_NOT;
			
			entvars_t *pevHost = &(host->v);
			entvars_t *pevEntity = &(ent->v);
			if(pevHost && pevEntity && pevHost->deadflag == DEAD_NO)
			{
				float flDistance = (pevEntity->origin - pevHost->origin).Length();
				if(flDistance < 85.0)
				{
					state->renderamt = (int)(flDistance * 3.0);
					state->rendermode = kRenderTransAlpha;
				}
			}
			
		}
	}

	return MRES_IGNORED;
}


//Register forward

void Engine_InstallHook(void);

void ServerDeactivate_Post(void)
{
	memset(g_iForwards, 0, TOTAL_FORWARDS * sizeof(int));
}

void OnAmxxAttach(void)
{
	memset(g_iForwards, 0, TOTAL_FORWARDS * sizeof(int));

	Engine_InstallHook();
}

void OnPluginsLoaded(void)
{
	g_iForwards[FW_Semiclip] = MF_RegisterForward("x_fw_api_semiclip", ET_CONTINUE, FP_CELL, FP_CELL, FP_DONE);
}