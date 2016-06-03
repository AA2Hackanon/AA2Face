#pragma once
#include <Windows.h>
/* god dammit, i hate common controlls in win32 applications */
#include <commctrl.h>
#pragma comment(lib, "ComCtl32.Lib")
#pragma comment(linker,"\"/manifestdependency:type                  = 'win32' \
                                              name                  = 'Microsoft.Windows.Common-Controls' \
                                              version               = '6.0.0.0' \
                                              processorArchitecture = '*' \
                                              publicKeyToken        = '6595b64144ccf1df' \
                                              language              = '*'\"")
#include "ExternConstants.h"
#include "Config.h"

constexpr UINT HAIRMESSAGE_FLIPHAIR = WM_APP + 1;
constexpr UINT HAIRMESSAGE_ADDHAIR = WM_APP + 2;
constexpr UINT HAIRMESSAGE_SETHAIR = WM_APP + 3;
constexpr UINT HAIRMESSAGE_HAIRSIZEADD = WM_APP + 4;
constexpr UINT HAIRMESSAGE_HAIRSIZESET = WM_APP + 5;
constexpr UINT FACEDETAILSMESSAGE_ADDGLASSES = WM_APP + 6;
constexpr UINT FACEDETAILSMESSAGE_SETGLASSES = WM_APP + 7;
constexpr UINT BODYCOLORMESSAGE_ADDTAN = WM_APP + 8;
constexpr UINT BODYCOLORMESSAGE_SETTAN = WM_APP + 9;
constexpr UINT POSEMESSAGE_ADDPOSE = WM_APP + 10;
constexpr UINT POSEMESSAGE_APPLYPOSE = WM_APP + 11;

extern INITCOMMONCONTROLSEX g_commonControllsStruct;
extern HACCEL g_acKeys;

void InitCCs(DWORD flags);
//general
void InjectionsInit();

void CharacterLoadedNotifier();

