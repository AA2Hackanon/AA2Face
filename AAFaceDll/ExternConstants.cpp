#include <Windows.h>
#include "ExternConstants.h"
#include "Logger.h"

const char* g_strModuleName = "AA2Edit.exe";
DWORD g_AA2Base = 0;
DWORD g_AA2RedrawFlagTable;
const HWND* g_AA2MainWndHandle = NULL; //points at 0 if not yet at main window
const HWND* g_AA2DialogHandles = NULL; //this is an array, [n] with n being the tab (0=system, 1 = figure, 9=hair etc)
const HWND* g_AA2UpperDialogHandle = NULL; //the dialog that contains the system/figure etc buttons to switch tabs
float* g_AA2CurrZoom = NULL;
float* g_AA2MaxTilt;

float*  g_AA2GetTilt() {
	//current tilt, not always loaded
	BYTE* ptilt = *(BYTE**)(g_AA2Base + 0x3526B8);
	if (ptilt == NULL) return NULL;
	ptilt = *(BYTE**)(ptilt + 0x4);
	if (ptilt == NULL) return NULL;
	ptilt = *(BYTE**)(ptilt + 0x198);
	if (ptilt == NULL) return NULL;
	float* currTilt = (float*)(ptilt + 0xc);
	return currTilt;
}

int g_AA2GetCurrentDialogTab() {
	//note that HWND -> class is done by a call to GetPropW(HWND, L"__WND_BASE_CLASS__") (AA2Edit.exe+3100A4)
	HWND wnd = *g_AA2UpperDialogHandle;
	wchar_t* obstr = (wchar_t*)(g_AA2Base + 0x3100A4);
	BYTE* classptr = (BYTE*)GetPropW(wnd,obstr);
	if (classptr == NULL) return -1; //probably not loaded right now
	//AA2Edit.exe+341A6 - 88 46 74              - mov[esi+74],al
	return *(classptr + 0x74);
}

void ExternInit() {
	g_AA2Base = (DWORD)GetModuleHandle("AA2Edit.exe");
	g_AA2RedrawFlagTable = g_AA2Base + 0x353160;
	g_AA2MainWndHandle = (const HWND*)(g_AA2Base + 0x381A6C);
	g_AA2UpperDialogHandle = (const HWND*)(g_AA2Base + 0x353180);
	g_AA2DialogHandles = (const HWND*)(g_AA2Base + 0x353184);
	g_AA2CurrZoom = (float*)(g_AA2Base + 0x343654);
	g_AA2MaxTilt = (float*)(g_AA2Base + 0x314D30);
	LOGPRIO(Logger::Priority::INFO) << "AA2Base found as " << (void*)g_AA2Base << "\n";
}

float* GetMinZoom() {
	DWORD ptr = *(DWORD*)(g_AA2Base + 0x344F40 + 0xE314);
	if (ptr == NULL) return NULL;
	ptr = *(DWORD*)((BYTE*)ptr + 0x20);
	ptr = *(DWORD*)((BYTE*)ptr + 0x4);
	float* ret = (float*)((BYTE*)ptr + 0x18C);
	return ret;
}
float* GetMaxZoom() {
	DWORD ptr = *(DWORD*)(g_AA2Base + 0x344F40 + 0xE314);
	if (ptr == NULL) return NULL;
	ptr = *(DWORD*)((BYTE*)ptr + 0x20);
	ptr = *(DWORD*)((BYTE*)ptr + 0x4);
	float* ret = (float*)((BYTE*)ptr + 0x188);
	return ret;
}
