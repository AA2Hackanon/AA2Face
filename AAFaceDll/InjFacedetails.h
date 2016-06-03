#pragma once
#include <Windows.h>
#include "Injections.h"
#include "BoundUpDownControl.h"

extern BoundUpDownControl<FacedetailsDialogClass,
							&FacedetailsDialogClass::GetGlassesButtonWnd,
							&FacedetailsDialogClass::GetGlassButtonCount,
							&FacedetailsDialogClass::GetGlassesSlot,
							&FacedetailsDialogClass::SetGlassesSlot,
							&FacedetailsDialogClass::SetChangeFlags>
	g_budGlasses;
extern BoundUpDownControl<FacedetailsDialogClass,
							&FacedetailsDialogClass::GetLipColorButtonWnd,
							&FacedetailsDialogClass::GetLipColorButtonCount,
							&FacedetailsDialogClass::GetLipColorSlot,
							&FacedetailsDialogClass::SetLipColorSlot,
							&FacedetailsDialogClass::SetChangeFlags>
	g_budLipColor;

extern "C" void __cdecl InitFacedetailsTab(FacedetailsDialogClass* internclass,bool before);
extern "C" void __cdecl InitGlassesSelector(HWND parent,HINSTANCE hInst);
extern "C" void __cdecl FacedetailsDialogNotification(FacedetailsDialogClass* internclass,HWND wnd,UINT msg,WPARAM wparam,LPARAM lparam);
extern "C" void __cdecl FacedetailsAfterDialogInit(FacedetailsDialogClass* internclass, HWND wnd);
extern "C" void __cdecl FacedetailsAfterInit(void* internclass);
extern "C" int __cdecl GetGlassesSelectorIndex(FacedetailsDialogClass* internclass,int guiChosen);
extern "C" int __cdecl GetLipColorSelectorIndex(FacedetailsDialogClass* internclass,int guiChosen);

void FacedetailsDialogOnCharacterLoad();