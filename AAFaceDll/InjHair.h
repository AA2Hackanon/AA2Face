#pragma once
#include "Injections.h"
#include "PageControl.h"

extern INITCOMMONCONTROLSEX g_commonControllsStruct;
extern HWND g_edHairSelector;
extern HWND g_udHairSelector;
extern HWND g_edHairFlipSelector;
extern HWND g_udHairFlipSelector;
extern PageControl g_pcHairs;

void RefreshHairSelectorPosition(HairDialogClass* internclass);
void RefreshButtonText(HairDialogClass* internclass);

extern "C" void __cdecl InitHairSelector(HWND parent,HINSTANCE hInst);
extern "C" int __cdecl GetHairSelectorIndex(HairDialogClass* internclass,int tab,int guiChosen);
extern "C" int __cdecl GetHairFlipSelectorIndex(HairDialogClass* internclass, int tab, int guiChosen);
extern "C" void __cdecl InitHairTab(HairDialogClass* internclass,bool before);
extern "C" int __cdecl HairDialogNotification(HairDialogClass* internclass,HWND hwndDlg,UINT msg,WPARAM wpara,LPARAM lparam);
extern "C" INT_PTR g_HairDialogProcReturnValue;
extern "C" void __cdecl HairDialogAfterInit(HairDialogClass* internclass);
extern "C" void __cdecl InvalidHairNotifier();
extern "C" void __cdecl HairInfoNotifier(int tab, TempHairInfos* info);

extern "C" void __cdecl RandomHairSelect(HairDialogClass* internclass);