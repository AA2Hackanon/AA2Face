#pragma once
#include "Injections.h"

extern HWND g_cbSystemKeepCardFace;
extern HWND g_gbSystemPose;
extern HWND g_edSystemPose;
extern HWND g_udSystemPose;
extern HWND g_btSystemPose;
extern HWND g_cbSystemLockPose;
extern HWND g_cbSystemEyeTrack;

void ApplyClothesChange();

extern "C" void __cdecl SystemDialogAfterDialogInit(void* internclass,HWND wnd);

extern "C" void __cdecl SystemDialogCharacterCreated();
extern "C" void __cdecl SystemDialogCharacterLoaded();

extern "C" void __cdecl BeforeCharacterOverwrite(const wchar_t* path);
extern "C" void __cdecl AfterCharacterOverwrite();

extern "C" void __cdecl SystemDialogNotification(void* internclass,HWND wnd,UINT msg,WPARAM wparam,LPARAM lparam);
extern "C" void __cdecl TopBarDialogNotification(void* internclass,HWND wnd,UINT msg,WPARAM wparam,LPARAM lparam);

extern "C" int __cdecl GetClothingState(int original);
extern "C" int __cdecl GetOutfitToDisplay(int original);
extern "C" int __cdecl GetOverwriteOutfitToDisplay(int original);
extern "C" int __cdecl GetOutfitShirtState(int original);
extern "C" int __cdecl GetOutfitSkirtState(int original);
extern "C" void __cdecl AfterOutfitCallback();

extern "C" int __cdecl GetPlayPosePosition();
extern "C" int __cdecl GetEyeTrackState();