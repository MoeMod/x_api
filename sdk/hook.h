class JMPHook
{
public:
	JMPHook(void *src, void *dst)
	{
		m_dwAddress = (DWORD)src;
		memcpy(m_rgbSourceCode, src , 5);
		*(BYTE *)&m_rgbDestCode[0] = 0xE9;
		*(DWORD *)&m_rgbDestCode[1] = (DWORD)dst - (DWORD)src - 0x5;
	}

public:
	void Attach(void)
	{
		static DWORD dwProtect;

		if (!VirtualProtect((void *)m_dwAddress, 5, PAGE_EXECUTE_READWRITE, &dwProtect))
			return;

		memcpy((void *)m_dwAddress, m_rgbDestCode, 5);
		VirtualProtect((void *)m_dwAddress, 5, dwProtect, &dwProtect);
	}

	void Detach(void)
	{
		static DWORD dwProtect;

		if (!VirtualProtect((void *)m_dwAddress, 5, PAGE_EXECUTE_READWRITE, &dwProtect))
			return;

		memcpy((void *)m_dwAddress, m_rgbSourceCode, 5);
		VirtualProtect((void *)m_dwAddress, 5, dwProtect, &dwProtect);
	}

public:
	DWORD m_dwAddress;
	BYTE m_rgbSourceCode[5], m_rgbDestCode[5];
};

class IATHook
{
public:
	IATHook(HMODULE module, const char *pszModuleName, const char *pszFuncName, void *dst)
	{
		IMAGE_NT_HEADERS *hdr = (IMAGE_NT_HEADERS *)((DWORD)module + ((IMAGE_DOS_HEADER *)module)->e_lfanew);
		IMAGE_IMPORT_DESCRIPTOR *import = (IMAGE_IMPORT_DESCRIPTOR *)((DWORD)module + hdr->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress);

		while (import->Name && stricmp((const char *)((DWORD)module + import->Name), pszModuleName))
			import++;

		DWORD dwFunction = (DWORD)GetProcAddress(GetModuleHandle(pszModuleName), pszFuncName);
		IMAGE_THUNK_DATA *thunk = (IMAGE_THUNK_DATA *)((DWORD)module + import->FirstThunk);

		while (thunk->u1.Function != dwFunction)
			thunk++;

		m_dwAddress = (DWORD)&thunk->u1.Function;
		m_dwSourceAddress = thunk->u1.Function;
		m_dwDestAddress = (DWORD)dst;
	}

public:
	void Attach(void)
	{
		static DWORD dwProtect;

		if (!VirtualProtect((void *)m_dwAddress, 4, PAGE_EXECUTE_READWRITE, &dwProtect))
			return;

		*(DWORD *)m_dwAddress = m_dwDestAddress;
		VirtualProtect((void *)m_dwAddress, 4, dwProtect, &dwProtect);
	}

	void Detach(void)
	{
		static DWORD dwProtect;

		if (!VirtualProtect((void *)m_dwAddress, 4, PAGE_EXECUTE_READWRITE, &dwProtect))
			return;

		*(DWORD *)m_dwAddress = m_dwSourceAddress;
		VirtualProtect((void *)m_dwAddress, 4, dwProtect, &dwProtect);
	}

public:
	DWORD m_dwAddress;
	DWORD m_dwSourceAddress, m_dwDestAddress;
};

class VFTHook
{
public:
	VFTHook(void **table, int index, void *dst)
	{
		m_dwAddress = (DWORD)&table[index];
		m_dwSourceAddress = (DWORD)table[index];
		m_dwDestAddress = (DWORD)dst;
	}

public:
	void Attach(void)
	{
		static DWORD dwProtect;

		if (!VirtualProtect((void *)m_dwAddress, 4, PAGE_EXECUTE_READWRITE, &dwProtect))
			return;

		*(DWORD *)m_dwAddress = m_dwDestAddress;
		VirtualProtect((void *)m_dwAddress, 4, dwProtect, &dwProtect);
	}

	void Detach(void)
	{
		static DWORD dwProtect;

		if (!VirtualProtect((void *)m_dwAddress, 4, PAGE_EXECUTE_READWRITE, &dwProtect))
			return;

		*(DWORD *)m_dwAddress = m_dwSourceAddress;
		VirtualProtect((void *)m_dwAddress, 4, dwProtect, &dwProtect);
	}

public:
	DWORD m_dwAddress;
	DWORD m_dwSourceAddress, m_dwDestAddress;
};