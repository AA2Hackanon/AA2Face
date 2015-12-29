#pragma once
#include "Injections.h"

extern HWND g_dummyHotkeyWnd;

extern "C" int __cdecl GetMessageHook(MSG* msg);
extern "C" void __cdecl PreGetMessageHook();
LRESULT CALLBACK DummyHotkeyWndProc(_In_ HWND   hwnd,_In_ UINT   uMsg,_In_ WPARAM wParam,_In_ LPARAM lParam);
