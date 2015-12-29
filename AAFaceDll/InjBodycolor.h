#pragma once
#include "Injections.h"
#include "ExternConstants.h"

extern HWND g_edTanSelector;
extern HWND g_udTanSelector;

extern "C" void __cdecl InitBodycolorTab(BodycolorDialogClass* internclass,bool before);
extern "C" void __cdecl BodycolorDialogNotification(BodycolorDialogClass* internclass,HWND wnd,UINT msg,WPARAM wparam,LPARAM lparam);
extern "C" void __cdecl BodycolorAfterDialogInit(BodycolorDialogClass* internclass,HWND wnd);
extern "C" int __cdecl GetTanSelectorIndex(BodycolorDialogClass* internclass,int guiChosen);

void BodycolorDialogOnCharacterLoad();