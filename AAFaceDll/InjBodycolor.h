#pragma once
#include "Injections.h"
#include "ExternConstants.h"

extern HWND g_edNipTypeSelector;
extern HWND g_udNipTypeSelector;
extern HWND g_edNipColorSelector;
extern HWND g_udNipColorSelector;
extern HWND g_edTanSelector;
extern HWND g_udTanSelector;
extern HWND g_edMosaicSelector;
extern HWND g_udMosaicSelector;
extern HWND g_edPubSelector;
extern HWND g_udPubSelector;

extern "C" void __cdecl InitBodycolorTab(BodycolorDialogClass* internclass,bool before);
extern "C" void __cdecl BodycolorDialogNotification(BodycolorDialogClass* internclass,HWND wnd,UINT msg,WPARAM wparam,LPARAM lparam);
extern "C" void __cdecl BodycolorAfterDialogInit(BodycolorDialogClass* internclass,HWND wnd);
extern "C" int __cdecl GetTanSelectorIndex(BodycolorDialogClass* internclass,int guiChosen);
extern "C" int __cdecl GetNipTypeSelectorIndex(BodycolorDialogClass* internclass,int guiChosen);
extern "C" int __cdecl GetNipColorSelectorIndex(BodycolorDialogClass* internclass,int guiChosen);
extern "C" int __cdecl GetMosaicSelectorIndex(BodycolorDialogClass* internclass,int guiChosen);
extern "C" int __cdecl GetPubHairSelectorIndex(BodycolorDialogClass* internclass,int guiChosen);

void BodycolorDialogOnCharacterLoad();