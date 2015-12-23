#include <Windows.h>
#include "ExternConstants.h"
#include "Injections.h"

BOOL WINAPI DllMain(
	_In_ HINSTANCE hinstDLL,
	_In_ DWORD     fdwReason,
	_In_ LPVOID    lpvReserved
	)
{
	ExternInit();
	//change calls in code to ours
	HookFace();

	HookHair();

	//HookTEMPTEST();
	return TRUE;
}