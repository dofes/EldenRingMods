#include <Windows.h>
#include <algorithm>

#include "ModUtils.h"

using namespace ModUtils;
using namespace mINI;

static float fpsLimit = 300;

void ReadConfig()
{
	INIFile config(GetModuleFolderPath() + "\\config.ini");
	INIStructure ini;

	if (config.read(ini))
	{
		fpsLimit = std::stof(ini["unlockthefps"].get("limit"));
	}
	else
	{
		ini["unlockthefps"]["limit"] = "300";
		config.write(ini, true);
	}

	Log("FPS limit: %f", fpsLimit);
}

DWORD WINAPI MainThread(LPVOID lpParam)
{
	Log("Activating UnlockTheFps...");
	std::vector<uint16_t> pattern = { 0xc7, MASKED, MASKED, 0x89, 0x88, 0x88, 0x3c, 0xeb, MASKED, 0x89, MASKED, 0x18, 0xeb, MASKED, 0x89, MASKED, 0x18, 0xc7 };
	std::vector<uint16_t> originalBytes = { 0x89, 0x88, 0x88, 0x3c };
	std::vector<uint8_t> newBytes(4, 0x90);

	ReadConfig();
	float frametime = (1000 / fpsLimit) / 1000;
	Log("Frametime: %f", frametime);
	memcpy(&newBytes[0], &frametime, 4);

	uintptr_t patchAddress = SigScan(pattern);
	if (patchAddress == 0)
	{
		return 1;
	}
	patchAddress += 0x3;

	if (!Replace(patchAddress, originalBytes, newBytes))
	{
		return 1;
	}
	
	Log("Removing 60 FPS fullscreen limit...");
	originalBytes = { 0xc7, MASKED, 0xef, 0x3c, 0x00, 0x00, 0x00, 0xc7, MASKED, 0xf3, 0x01, 0x00, 0x00, 0x00 };
	newBytes = { 0xc7, 0x45, 0xef, 0x00, 0x00, 0x00, 0x00 };
	patchAddress = SigScan(originalBytes);
	if (patchAddress != 0)
	{
		Replace(patchAddress, originalBytes, newBytes);
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