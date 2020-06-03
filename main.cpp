#include "amxxmodule.h"
#include "forwards.h"
#include "immenu.hpp"

#include <algorithm>

int g_iForwards[TOTAL_FORWARDS];

//Register forward
void ServerDeactivate_Post(void)
{
	std::fill_n(g_iForwards, TOTAL_FORWARDS, 0);
}

void OnAmxxAttach(void)
{
	std::fill_n(g_iForwards, TOTAL_FORWARDS, 0);
}

void OnPluginsLoaded(void)
{
	g_iForwards[FW_Semiclip] = MF_RegisterForward("x_fw_api_semiclip", ET_CONTINUE, FP_CELL, FP_CELL, FP_DONE);
	MenuCreate(1, [](auto) {});
}