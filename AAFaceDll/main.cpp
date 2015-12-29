#include <Windows.h>
#include "Config.h"
#include "ExternConstants.h"
#include "Hooks.h"
#include "Injections.h"

BOOL WINAPI DllMain(
	_In_ HINSTANCE hinstDLL,
	_In_ DWORD     fdwReason,
	_In_ LPVOID    lpvReserved
	)
{
	if(fdwReason == DLL_PROCESS_ATTACH) {
		ExternInit();
		InjectionsInit();
		//change calls in code to ours

		if(!g_config.GetDisabledGeneral()) {
			HookGeneral();
		}
		if(!g_config.GetDisabledFace()) {
			HookFace();
		}
		if(!g_config.GetDisabledHair()) {
			HookHair();
		}
		if(!g_config.GetDisabledGlasses()) {
			HookFacedetails();
		}
		if(!g_config.GetDisabledBodycolor()) {
			HookBodycolor();
		}
		
		
		//HookSystem();
		//HookHairMaxAmount();
		//HookTEMPTEST();
		return TRUE;
	}
}