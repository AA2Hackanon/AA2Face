#pragma once
#include <Windows.h>

#include "ExternConstants.h"
#include "PageControl.h"

extern PageControl g_pcPersonality;

void RefreshPersonalitySelectorPosition(PersonalityDialogClass* internclass);
void RefreshPersonalityButtonText(PersonalityDialogClass* internclass);
void RefreshPersonalityFakeButton(PersonalityDialogClass* internclass, DWORD selSlot);

extern "C" void __cdecl InitPersonalityTab(PersonalityDialogClass* internclass, bool before);
extern "C" void __cdecl InitPersonalitySelector(HWND parent, HINSTANCE hInst);
extern "C" void __cdecl PersonalityDialogNotification(PersonalityDialogClass* internclass, HWND wnd, UINT msg, WPARAM wparam, LPARAM lparam);
extern "C" void __cdecl PersonalityAfterDialogInit(PersonalityDialogClass* internclass, HWND wnd);
extern "C" int __cdecl GetPersonalitySelectorIndex(PersonalityDialogClass* internclass, int guiChosen);
extern "C" int __cdecl RandomPersonalitySelect(PersonalityDialogClass* internclass);