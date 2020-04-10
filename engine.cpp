#include "amxxmodule.h"
#include "engine.h"
#include "hook.h"

DWORD g_dwEngineBase = 0;
DWORD g_dwEngineSize = 0;
DWORD g_dwEngineBuildnum = 0;

hook_funcs_t gHookFuncs;

void Engine_InstallHook(void)
{
	DWORD addr;
	HMODULE hEngine;
	
	DWORD dwDataSize = 0x02FFFFFF-0x01D00000;

	hEngine = GetModuleHandle("swds.dll");
	if(!hEngine || hEngine == INVALID_HANDLE_VALUE)
	{
		hEngine = GetModuleHandle("hw.dll");
	}
	if(!hEngine || hEngine == INVALID_HANDLE_VALUE)
	{
		g_dwEngineBase = 0x1D01000;
		g_dwEngineSize = 0x1000000;
	}
	else
	{
		g_dwEngineBase = MH_GetModuleBase(hEngine);
		g_dwEngineSize = MH_GetModuleSize(hEngine);
	}

#define BUILD_NUMBER_SIG "\xA1\x2A\x2A\x2A\x2A\x83\xEC\x08\x2A\x33\x2A\x85\xC0"
#define BUILD_NUMBER_SIG_NEW "\x55\x8B\xEC\x83\xEC\x08\xA1\x2A\x2A\x2A\x2A\x56\x33\xF6\x85\xC0\x0F\x85\x2A\x2A\x2A\x2A\x53\x33\xDB\x8B\x04\x9D"

	gHookFuncs.buildnum = (int (*)(void))MH_SIGFind(g_dwEngineBase, g_dwEngineSize, BUILD_NUMBER_SIG, sizeof(BUILD_NUMBER_SIG)-1);
	if(!gHookFuncs.buildnum)
		gHookFuncs.buildnum = (int (*)(void))MH_SIGFind(g_dwEngineBase, g_dwEngineSize, BUILD_NUMBER_SIG_NEW, sizeof(BUILD_NUMBER_SIG_NEW)-1);
	if(!gHookFuncs.buildnum)
	{
		SERVER_PRINT("[Thanatos] API:Could NOT find buildnum\n");
		return;
	}

	g_dwEngineBuildnum = gHookFuncs.buildnum();

	//Here is a bug in engine so we can not only just hook FM_ShouldCollide but need to fix some bug in engine:
	/*

	void SV_ClipToLinks(areanode_t *node, moveclip_t *clip)
	{
		....
		for (l = node->solid_edicts.next; l != &node->solid_edicts; l = next)
		{
		...
			if (gNewDLLFunctions.pfnShouldCollide)
			{
				if (gNewDLLFunctions.pfnShouldCollide(touch, clip->passedict) == 0)
					return;  <- this "return" caused some phys problems, and this "return" should be "continue".
			}
		...

	}

	*/

	//SV_ClipToLinks
	//We need to find "Trigger in clipping list" first;
	addr = MH_SIGFind(g_dwEngineBase, dwDataSize, "Trigger in clipping list", sizeof("Trigger in clipping list")-1);
	if(!addr)
	{
		SERVER_PRINT("[Thanatos] API:Could NOT find \"Trigger in clipping list\"\n");
		return;
	}

	//Setup the signature so we can find Sys_Error("Trigger in clipping list");
	//2A means indeterminate bytes
	//  68 2A 2A 2A 2A	push    offset aTriggerInClipp ; "Trigger in clipping list"
	//  E8 2A 2A 2A 2A	call    Sys_Error
	//  83 C4 04		add     esp, 4
	byte Sys_Error_SIG[14] = "\x68\x2A\x2A\x2A\x2A\xE8\x2A\x2A\x2A\x2A\x83\xC4\x04";

	*(DWORD *)(Sys_Error_SIG+1) = addr;

	//Let's go find
	addr = MH_SIGFind(g_dwEngineBase, g_dwEngineSize, (char *)Sys_Error_SIG, 13);
	if(!addr)
	{
		SERVER_PRINT("[Thanatos] API:Could NOT find SV_ClipToLinks->Sys_Error(\"Trigger in clipping list\")\n");
		return;
	}

	//Here we found Sys_Error("Trigger in clipping list");

	//FF D0					call    eax ; gNewDLLFunctions.pfnShouldCollide
	//83 C4 08				add     esp, 8
	//85 C0					test    eax, eax
	//0F 84 2A 2A 2A 2A		jz      loc_XXXXXX

#define SV_CLIPTOLINKS_SHOULDCOLLIDE_SIG "\xFF\xD0\x83\xC4\x08\x85\xC0\x0F\x84"
	addr = MH_SIGFind(addr, 0x200, SV_CLIPTOLINKS_SHOULDCOLLIDE_SIG, sizeof(SV_CLIPTOLINKS_SHOULDCOLLIDE_SIG)-1);
	if(!addr)
	{
		SERVER_PRINT("[Thanatos] API:Could NOT find SV_ClipToLinks->continue\n");
		return;
	}

	//Save the address of "0F 84 2A 2A 2A 2A jz loc_XXXXXX"
	//it will be modified by us later.

	DWORD addr_shouldcollide_jz = addr + 7;

	//Now we find a "continue" in this for() loop

	//D8 1D 2A 2A 2A 2A                                               fcomp   flt:xxxxxx (0.0)
	//DF E0                                                           fnstsw  ax
	//F6 C4 44                                                        test    ah, 44h
	//0F 8B 2A 2A 2A 2A                                               jnp     loc_xxxxxx

#define SV_CLIPTOLINKS_SOLID_SIG "\x83\xF9\x03\x74"
	addr = MH_SIGFind(addr, 0x200, SV_CLIPTOLINKS_SOLID_SIG, sizeof(SV_CLIPTOLINKS_SOLID_SIG)-1);
	if(!addr)
	{
		SERVER_PRINT("[Thanatos] API:Could NOT find SV_ClipToLinks->SOLID_SLIDEBOX\n");
		return;
	}
	char byNew = SOLID_TRIGGER;
	MH_WriteMemory((void *)(addr + 2), (BYTE *)&byNew, 1);


#define SV_CLIPTOLINKS_CONTINUE_SIG "\xD8\x1D\x2A\x2A\x2A\x2A\xDF\xE0\xF6\xC4\x44\x0F\x8B"
	addr = MH_SIGFind(addr, 0x200, SV_CLIPTOLINKS_CONTINUE_SIG, sizeof(SV_CLIPTOLINKS_CONTINUE_SIG)-1);
	if(!addr)
	{
		SERVER_PRINT("[Thanatos] API:Could NOT find SV_ClipToLinks->continue\n");
		return;
	}

	//This jnp loc_xxxxxx is a conditional jump code and we need to trace to it's destination address,
	//jnp's operand is an offset from it's next opcode's address,
	//which means opcode ".text:04000000 0F 8B 02 00 00 00 jnp loc_xxxxxx" jumps to 0x04000008 (0x04000000 + 6 + 0x02)
	
	//Now we calcluate the destination address

	//Move to "0F 8B 2A 2A 2A 2A jnp loc_xxxxxx" first
	DWORD addr_jnp = addr + 11;

	//Add the offset to the jnp's address, just like "0x04000000 + 6 + 0x02", this is the destionation address we need to jump to.
	DWORD addr_destloc = addr_jnp + (*(DWORD *)(addr_jnp + 2)) + 6;

	//And what is the offset? (jz_address + 6 + offset = jump_destination_address)
	//it's very easy to know that : offset = jump_destination_address - jz_address - 6;
	//just basic math problem.
	DWORD jz_offset = addr_destloc - addr_shouldcollide_jz - 6;

	//Write this offset back to "0F 84 2A 2A 2A 2A jz loc_XXXXXX"
	MH_WriteMemory((void *)(addr_shouldcollide_jz + 2), (BYTE *)&jz_offset, 4);


	SERVER_PRINT("[Thanatos] API:Engine hook installed.\n");
}