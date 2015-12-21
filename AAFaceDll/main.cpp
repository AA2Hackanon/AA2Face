#include <Windows.h>
#include "ExternConstants.h"
#include "Injections.h"

BOOL WINAPI DllMain(
	_In_ HINSTANCE hinstDLL,
	_In_ DWORD     fdwReason,
	_In_ LPVOID    lpvReserved
	)
{
	g_AA2Base = (DWORD)GetModuleHandle("AA2Edit.exe");
	//change calls in code to ours
	HookDialogCreationProcParam();
	HookDialogCreation();
	HookFaceChoose();
	HookFaceLoad();
	return TRUE;
}