#include <Windows.h>
#include <fcntl.h>
#include "ModUtils.h"
#include <io.h>

using namespace ModUtils;

BOOL WINAPI DllMain(HINSTANCE module, DWORD reason, LPVOID)
{
	if (reason == DLL_PROCESS_ATTACH)
	{
		AllocConsole();						//打开控制台窗口以显示调试信息
		SetConsoleTitleA("Debug Win Output");			//设置标题
		HANDLE hCon = GetStdHandle(STD_OUTPUT_HANDLE);		//获取控制台输出句柄
		INT hCrt = _open_osfhandle((INT)hCon, _O_TEXT);		//转化为C文件描述符
		FILE* hf = _fdopen(hCrt, "w");			//转化为C文件流
		setvbuf(hf, NULL, _IONBF, 0);				//无缓冲
		*stdout = *hf;						//重定向标准输出
		freopen("CONOUT$", "w+t", stdout);
		DisableThreadLibraryCalls(module);
	}
	return 1;
}
