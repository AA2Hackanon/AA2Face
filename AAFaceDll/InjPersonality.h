#pragma once
#include <Windows.h>

#include "ExternConstants.h"
#include "PageControl.h"

extern PageControl g_pcPersonality;

void RefreshPersonalitySelectorPosition(PersonalityDialogClass* internclass);

extern "C" void __cdecl InitPersonalityTab(PersonalityDialogClass* internclass, bool before);
extern "C" void __cdecl InitPersonalitySelector(HWND parent, HINSTANCE hInst);
extern "C" void __cdecl PersonalityDialogNotification(FacedetailsDialogClass* internclass, HWND wnd, UINT msg, WPARAM wparam, LPARAM lparam);
extern "C" void __cdecl PersonalityAfterDialogInit(FacedetailsDialogClass* internclass, HWND wnd);
extern "C" int __cdecl GetPersonalitySelectorIndex(FacedetailsDialogClass* internclass, int guiChosen);
extern "C" int __cdecl RandomPersonalitySelect(FacedetailsDialogClass* internclass);