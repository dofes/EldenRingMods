#include <Windows.h>
#include "ModUtils.h"
#include <xinput.h>

using namespace ModUtils;
using namespace mINI;

uintptr_t patchAddress = 0;
static unsigned int key = 0x50;
static unsigned int modifier = HK_NONE;
static bool flag = true;

DWORD WINAPI MainThread(LPVOID lpParam)
{
	Log("显示所有赐福点加载........");
	std::vector<uint16_t> pattern =
	{ 0x48,0x8B,0x7C,0x24,0x58,0x84,0xC0,0x74,0x02,0xB0,0x01,0xF6,0xC3,0x01 };
	patchAddress = SigScan(pattern);
	if (patchAddress == 0)
	{
		return 1;
	}
	while (true)
	{
		if (CheckHotkey(key, modifier))
		{
			if (flag)
			{
				Log("open");
				Replace(patchAddress + 7, { 0x90, 0x90 }, { 0x74,0x02 });
				flag = false;
			}
			else
			{
				Log("close");
				Replace(patchAddress + 7, { 0x74,0x02 }, { 0x90, 0x90 });
				flag = true;
			}
		}
		Sleep(5);
	}
	CloseLog();
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