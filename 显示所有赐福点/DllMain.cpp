#include <Windows.h>
#include "ModUtils.h"

using namespace ModUtils;

DWORD WINAPI MainThread(LPVOID lpParam)
{
	Log("显示所有赐福点加载........");
	std::vector<uint16_t> pattern =
	{ 0x48,0x8B,0x7C,0x24,0x58,0x84,0xC0,0x74,0x02,0xB0,0x01,0xF6,0xC3,0x01 };
	uintptr_t patchAddress = SigScan(pattern);
	if (patchAddress != 0)
	{
		patchAddress += 7;
		Replace(patchAddress, { 0x74,0x02 }, { 0x90, 0x90 });
	}
	//CloseLog();
	return 0;
}

BOOL WINAPI DllMain(HINSTANCE module, DWORD reason, LPVOID)
{
	if (reason == DLL_PROCESS_ATTACH)
	{
		DisableThreadLibraryCalls(module);
		CreateThread(0, 0, &MainThread, 0, 0, NULL);
	}
	return 1;
}