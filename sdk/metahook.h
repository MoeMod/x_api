typedef struct hook_s
{
	void *pOldFuncAddr;
	void *pNewFuncAddr;
	void *pClass;
	int iTableIndex;
	int iFuncIndex;
	HMODULE hModule;
	const char *pszModuleName;
	const char *pszFuncName;
	struct hook_s *pNext;
	void *pInfo;
}
hook_t;

hook_t *MH_FindInlineHooked(void *pOldFuncAddr);
hook_t *MH_FindVFTHooked(void *pClass, int iTableIndex, int iFuncIndex);
hook_t *MH_FindIATHooked(HMODULE hModule, const char *pszModuleName, const char *pszFuncName);
BOOL MH_UnHook(hook_t *pHook);
hook_t *MH_InlineHook(void *pOldFuncAddr, void *pNewFuncAddr, void *&pCallBackFuncAddr);
hook_t *MH_VFTHook(void *pClass, int iTableIndex, int iFuncIndex, void *pNewFuncAddr, void *&pCallBackFuncAddr);
hook_t *MH_IATHook(HMODULE hModule, const char *pszModuleName, const char *pszFuncName, void *pNewFuncAddr, void *&pCallBackFuncAddr);
void *MH_GetClassFuncAddr(...);
DWORD MH_GetModuleBase(HMODULE hModule);
DWORD MH_GetModuleSize(HMODULE hModule);
void *MH_SearchPattern(void *pStartSearch, DWORD dwSearchLen, char *pPattern, DWORD dwPatternLen);
void MH_WriteDWORD(void *pAddress, DWORD dwValue);
DWORD MH_ReadDWORD(void *pAddress);
DWORD MH_WriteMemory(void *pAddress, BYTE *pData, DWORD dwDataSize);
DWORD MH_ReadMemory(void *pAddress, BYTE *pData, DWORD dwDataSize);
void MH_FreeAllHook(void);