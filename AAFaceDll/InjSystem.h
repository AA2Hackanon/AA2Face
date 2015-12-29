#pragma once
#include "Injections.h"

extern "C" void __cdecl SystemDialogCharacterCreated();
extern "C" void __cdecl SystemDialogCharacterLoaded();

extern "C" void __cdecl SystemDialogNotification(void* internclass,HWND wnd,UINT msg,WPARAM wparam,LPARAM lparam);