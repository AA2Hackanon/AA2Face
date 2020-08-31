#include "InjPersonality.h"
#include "GenUtils.h"

PageControl g_pcPersonality;

void __cdecl InitPersonalityTab(PersonalityDialogClass* internclass, bool before) {
	if (before) {
		RefreshPersonalitySelectorPosition(internclass);
	}
}

void __cdecl InitPersonalitySelector(HWND parent, HINSTANCE hInst)
{
	g_pcPersonality = PageControl(POINT{ 380, 570 }, parent, 2);
}

void __cdecl PersonalityDialogNotification(FacedetailsDialogClass* internclass, HWND wnd, UINT msg, WPARAM wparam, LPARAM lparam) {
	if (msg == WM_COMMAND && lparam != 0) {
		DWORD ctrlId = LOWORD(wparam);
		DWORD notification = HIWORD(wparam);
		HWND wnd = (HWND)lparam;
		if (g_pcPersonality.OnWmCmd(wnd, ctrlId, notification) != PageControl::None) {
		}
	}
}

void __cdecl PersonalityAfterDialogInit(FacedetailsDialogClass* internclass, HWND wnd) {
	
}

int __cdecl GetPersonalitySelectorIndex(FacedetailsDialogClass* internclass, int guiChosen) {
	return -1;
}

int __cdecl RandomPersonalitySelect(FacedetailsDialogClass* internclass) {
	return -1;
}

void RefreshPersonalitySelectorPosition(PersonalityDialogClass* internclass) {
	int btnCnt = internclass->GetPersonalityButtonCount();
	HWND lastButton = internclass->GetPersonalityButtonHwnd(btnCnt - 1);
	RECT rct = GetRelativeRect(lastButton);
	DWORD x, y, xw, yw;
	x = rct.left;
	y = rct.top;
	xw = rct.right - rct.left, yw = rct.bottom - rct.top;
	y += yw;

	g_pcPersonality.MoveTo(POINT{ (LONG)x, (LONG)y });
}