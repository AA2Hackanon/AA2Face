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
		//config:
		g_config = Config("AAFace\\config.txt");
		ExternInit();
		InjectionsInit();
		//change calls in code to ours

		if(!g_config.IsDisabled(Config::DISABLE_GENERAL)) {
			HookGeneral();
		}
		if(!g_config.IsDisabled(Config::DISABLE_FACE)) {
			HookFace();
		}
		if(!g_config.IsDisabled(Config::DISABLE_HAIR)) {
			HookHair();
		}
		if(!g_config.IsDisabled(Config::DISABLE_FACEDETAILS)) {
			HookFacedetails();
		}
		if(!g_config.IsDisabled(Config::DISABLE_BODY_COLOR)) {
			HookBodycolor();
		}
		if(!g_config.IsDisabled(Config::DISABLE_LIMITS)) {
			HookLimits();
		}
		HookSystem();
		
		
		//HookSystem();
		//HookHairMaxAmount();
		//HookTEMPTEST();
		return TRUE;
	}
}