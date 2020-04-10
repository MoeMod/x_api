#pragma once

#define FM_CHECK_ENTITY(x) \
	if (x < 0 || x > gpGlobals->maxEntities) { \
		MF_LogError(pAmx, AMX_ERR_NATIVE, "Entity out of range (%d)", x); \
		return 0; \
	} else { \
		if (x <= gpGlobals->maxClients) { \
			if (!MF_IsPlayerIngame(x)) { \
				MF_LogError(pAmx, AMX_ERR_NATIVE, "Invalid player %d (not in-game)", x); \
				return 0; \
			} \
		} else { \
			if (x != 0 && FNullEnt(INDEXENT(x))) { \
				MF_LogError(pAmx, AMX_ERR_NATIVE, "Invalid entity %d", x); \
				return 0; \
			} \
		} \
	}

#define FM_CHECK_ENTITY2(x) \
	if (x < 0 || x > gpGlobals->maxEntities) { \
		MF_LogError(pAmx, AMX_ERR_NATIVE, "Entity out of range (%d)", x); \
		return 0; \
	} else { \
		if (x != 0 && FNullEnt(INDEXENT(x))) { \
			MF_LogError(pAmx, AMX_ERR_NATIVE, "Invalid entity %d", x); \
			return 0; \
		} \
	}