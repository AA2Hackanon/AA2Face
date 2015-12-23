#pragma once
#include <Windows.h>
#include "ExternConstants.h"

extern HWND g_cbSelector;
extern HWND g_edFaceSelector;
extern HWND g_udFaceSelector;

extern "C" void __cdecl InitSelector(HWND parent,HINSTANCE hInst);
extern "C" int __cdecl GetSelectorIndex();
extern "C" BOOL __cdecl DialogNotification(void* internclass, HWND hwndDlg,UINT uMsg,WPARAM wParam,LPARAM lParam);
extern "C" void __cdecl LoadFace(int index);

extern "C" void __cdecl InitHairSelector(HWND parent,HINSTANCE hInst);
extern "C" int __cdecl GetHairSelectorIndex(int tab, int guiChosen);
extern "C" void __cdecl InitHairTab(HairDialogClass* internclass,bool before);
extern "C" BOOL __cdecl HairDialogNotification(HairDialogClass* internclass,HWND hwndDlg,UINT msg,WPARAM wpara,LPARAM lparam);