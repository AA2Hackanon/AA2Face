#pragma once
#include <Windows.h>
#include "Injections.h"
extern HWND g_cbFaceSelector;

extern "C" void __cdecl InitFaceSelector(HWND parent,HINSTANCE hInst);
extern "C" int __cdecl GetFaceSelectorIndex();
extern "C" BOOL __cdecl FaceDialogNotification(FaceDialogClass* internclass,HWND hwndDlg,UINT uMsg,WPARAM wParam,LPARAM lParam);
extern "C" void __cdecl FaceDialogAfterInit(FaceDialogClass* internclass, HWND wnd);
extern "C" void __cdecl LoadFace(int index);