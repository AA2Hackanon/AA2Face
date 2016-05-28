#pragma once
#include <Windows.h>
#include "Injections.h"

extern HWND g_edGlassesSelector;
extern HWND g_udGlassesSelector;
extern HWND g_edLipColorSelector;
extern HWND g_udLipColorSelector;

extern "C" void __cdecl InitFacedetailsTab(FacedetailsDialogClass* internclass,bool before);
extern "C" void __cdecl InitGlassesSelector(HWND parent,HINSTANCE hInst);
extern "C" void __cdecl FacedetailsDialogNotification(FacedetailsDialogClass* internclass,HWND wnd,UINT msg,WPARAM wparam,LPARAM lparam);
extern "C" void __cdecl FacedetailsAfterDialogInit(FacedetailsDialogClass* internclass, HWND wnd);
extern "C" void __cdecl FacedetailsAfterInit(void* internclass);
extern "C" int __cdecl GetGlassesSelectorIndex(FacedetailsDialogClass* internclass,int guiChosen);
extern "C" int __cdecl GetLipColorSelectorIndex(FacedetailsDialogClass* internclass,int guiChosen);

void FacedetailsDialogOnCharacterLoad();