#pragma once
#include <Windows.h>

extern HWND g_cbSelector;
extern "C" void __cdecl InitSelector(HWND parent,HINSTANCE hInst);
extern "C" int __cdecl GetSelectorIndex();
extern "C" BOOL __cdecl DialogNotification(void* internclass, HWND hwndDlg,UINT uMsg,WPARAM wParam,LPARAM lParam);
extern "C" void __cdecl LoadFace(int index);