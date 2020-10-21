#include "InjPersonality.h"

#include <vector>
#include <algorithm>

#include "GenUtils.h"
#include "Logger.h"

PageControl g_pcPersonality;

namespace {
	struct PersonalityName {
		DWORD slot;
		const wchar_t* name;
		PersonalityDialogClass::PersonalityListNode* node;
	};
	std::vector<PersonalityName> loc_personalityNames;
	DWORD loc_chosenPersonalitySlot;

	HWND loc_fakeButton;
	HWND loc_hiddenOrigButton = NULL;
	HWND loc_naturallyhiddenOrigButton = NULL; //if no hidden orig button logic is required because the button is hidden on the current page
	int loc_hiddenOrigButtonN = 0;
	DWORD loc_hiddenOirgMeaningMapValue = 0;

	void LocUnhideOrigButton(PersonalityDialogClass* internclass) {
		if (loc_hiddenOrigButton != NULL) {
			ShowWindow(loc_hiddenOrigButton, SW_SHOW);
			loc_hiddenOrigButton = NULL;
			internclass->GetButtonIndexPersonalityMap()[loc_hiddenOrigButtonN] = loc_hiddenOirgMeaningMapValue;
		}
	}
	void LocHideOrigButton(PersonalityDialogClass* internclass, int buttonN, DWORD fixOnSlot) {
		if (loc_hiddenOrigButton == internclass->GetPersonalityButtonHwnd(buttonN)) {
			return;
		}
		if (loc_hiddenOrigButton != NULL) {
			ShowWindow(loc_fakeButton, SW_SHOW);
			loc_hiddenOrigButton = NULL;
		}
		loc_hiddenOrigButton = internclass->GetPersonalityButtonHwnd(buttonN);
		loc_hiddenOrigButtonN = buttonN;
		DWORD* meaningMap = internclass->GetButtonIndexPersonalityMap();
		loc_hiddenOirgMeaningMapValue = meaningMap[buttonN];
		meaningMap[buttonN] = fixOnSlot;
	}
}

void __cdecl InitPersonalityTab(PersonalityDialogClass* internclass, bool before) {
	if (before) {
		HWND origButton = internclass->GetPersonalityButtonHwnd(0);
		WINDOWINFO info;
		ZeroMemory(&info, sizeof(info));
		info.cbSize = sizeof(WINDOWINFO);
		GetWindowInfo(origButton, &info);
		HFONT origFont = (HFONT)SendMessageW(origButton, WM_GETFONT, 0, 0);
		loc_fakeButton = CreateWindowExW(0, L"BUTTON", L"", WS_CHILD | BS_PUSHBUTTON,
			0, 0, 1, 1, GetParent(origButton), 0, (HINSTANCE)g_AA2Base, NULL);
		int error = GetLastError();
		SendMessageW(loc_fakeButton, WM_SETFONT, (WPARAM)origFont, FALSE);
		RefreshPersonalitySelectorPosition(internclass);
		RefreshPersonalityButtonText(internclass);
		loc_chosenPersonalitySlot = internclass->GetPersonalitySlot();
		LOGPRIO(Logger::Priority::SPAM) << "hair loaded and initialized to " << loc_chosenPersonalitySlot << "\r\n";
	}
	else {
		//reselect
		for (int i = 0; i < loc_personalityNames.size(); i++) {
			if (loc_personalityNames[i].slot == loc_chosenPersonalitySlot) {
				const int nButtons = internclass->GetPersonalityButtonCount();
				int buttonPage = i / nButtons;
				if (buttonPage != g_pcPersonality.CurrPage()) {
					g_pcPersonality.SetPage(buttonPage);
					RefreshPersonalityButtonText(internclass);
					RefreshPersonalityFakeButton(internclass, internclass->GetPersonalitySlot());
					int buttonIndex = i % nButtons;
					for (int i = 0; i < nButtons; i++) {
						SendMessageW(internclass->GetPersonalityButtonHwnd(i), BM_SETCHECK, BST_UNCHECKED, 0);
					}
					SendMessageW(internclass->GetPersonalityButtonHwnd(buttonIndex), BM_SETCHECK, BST_CHECKED, 0);
				}
				
				break;
			}
		}
		internclass->SetPersonalitySlot(loc_chosenPersonalitySlot);
	}
}

void __cdecl InitPersonalitySelector(HWND parent, HINSTANCE hInst)
{
	g_pcPersonality = PageControl(POINT{ 380, 570 }, parent, 2);
}

void __cdecl PersonalityDialogNotification(PersonalityDialogClass* internclass, HWND wnd, UINT msg, WPARAM wparam, LPARAM lparam) {
	if (msg == WM_COMMAND && lparam != 0) {
		DWORD ctrlId = LOWORD(wparam);
		DWORD notification = HIWORD(wparam);
		HWND wnd = (HWND)lparam;
		if (g_pcPersonality.OnWmCmd(wnd, ctrlId, notification) != PageControl::None) {
			RefreshPersonalityButtonText(internclass);
			RefreshPersonalityFakeButton(internclass, internclass->GetPersonalitySlot());
		}
		if (notification == BN_CLICKED && wnd == loc_fakeButton) {
			if (loc_hiddenOrigButton == NULL) {
				LOGPRIO(Logger::Priority::ERR) << "Personality fake button was clicked, but no original button was present";
			}
			else {
				ShowWindow(loc_fakeButton, SW_HIDE);
				HWND origButton = loc_hiddenOrigButton;
				LocUnhideOrigButton(internclass);
				SendMessageW(origButton, BM_CLICK, 0, 0);
			}
		}
		else if (notification == BN_CLICKED) {
			for (int i = 0; i < internclass->GetPersonalityButtonCount(); i++) {
				if (internclass->GetPersonalityButtonHwnd(i) == wnd) {
					int pageChosen = i + internclass->GetPersonalityButtonCount() * g_pcPersonality.CurrPage();
					if (pageChosen < loc_personalityNames.size()) {
						if (loc_naturallyhiddenOrigButton != NULL) {
							SendMessageW(loc_naturallyhiddenOrigButton, BM_SETCHECK, BST_UNCHECKED, FALSE);
							loc_naturallyhiddenOrigButton = NULL;
						}
						RefreshPersonalityFakeButton(internclass, loc_personalityNames[pageChosen].slot);
					}
					break;
				}
			}
			
		}
	}
}

void __cdecl PersonalityAfterDialogInit(PersonalityDialogClass* internclass, HWND wnd) {
	loc_personalityNames.clear();
	auto* listFile = internclass->GetPersonalityListFile();
	listFile->InOrderTraverse([](PersonalityDialogClass::PersonalityListNode* node) {
		loc_personalityNames.push_back({ node->slot, node->label, node });
	});

	std::sort(loc_personalityNames.begin(), loc_personalityNames.end(), 
		[](const auto& lhs, const auto& rhs) -> bool { 
			//return wcstoul(lhs.node->aNumber + 1, NULL, 0) < wcstoul(rhs.node->aNumber + 1, NULL, 0);
			return lhs.slot < rhs.slot;
		});
	DWORD* meaningMap = internclass->GetButtonIndexPersonalityMap();
	for (int i = 0; i < loc_personalityNames.size(); i++) {
		meaningMap[i] = loc_personalityNames[i].slot;
	}
}

int __cdecl GetPersonalitySelectorIndex(PersonalityDialogClass* internclass, int guiChosen) {
	int pageChosen = guiChosen + internclass->GetPersonalityButtonCount() * g_pcPersonality.CurrPage();
	if (pageChosen < loc_personalityNames.size()) {
		return pageChosen;
	}
	return -1;
}

int __cdecl RandomPersonalitySelect(PersonalityDialogClass* internclass) {
	return -1;
}

void RefreshPersonalitySelectorPosition(PersonalityDialogClass* internclass) {
	int btnCnt = internclass->GetPersonalityButtonCount();
	HWND rightmostButton = internclass->GetPersonalityButtonHwnd(4);
	RECT rct = GetRelativeRect(rightmostButton);
	DWORD x, y, xw, yw;
	x = rct.left;
	y = rct.top;
	xw = rct.right - rct.left, yw = rct.bottom - rct.top;
	y -= yw;

	g_pcPersonality.MoveTo(POINT{ (LONG)x, (LONG)y });
}

void RefreshPersonalityButtonText(PersonalityDialogClass* internclass) {
	//change button text and meanings
	DWORD* meaningMap = internclass->GetButtonIndexPersonalityMap();
	int buttonN = internclass->GetPersonalityButtonCount();
	for (int i = 0; i < buttonN; i++) {
		HWND btn = internclass->GetPersonalityButtonHwnd(i);
		int realN = i + g_pcPersonality.CurrPage() * buttonN;
		if (realN < loc_personalityNames.size()) {
			auto data = loc_personalityNames[realN];
			meaningMap[i] = data.slot;
			SetWindowTextW(btn, data.name);
			ShowWindow(btn, SW_SHOW);
		}
		else {
			ShowWindow(btn, SW_HIDE);
		}
	}
	loc_hiddenOrigButton = NULL; //if one was hidden, its not anymore
}

void RefreshPersonalityFakeButton(PersonalityDialogClass* internclass, DWORD currSlot) {
	int buttonOfSlot = -1;
	for (int i = 0; i < loc_personalityNames.size(); i++) {
		if (loc_personalityNames[i].slot == currSlot) {
			buttonOfSlot = i;
			break;
		}
	}
	if (buttonOfSlot == -1) {
		LOGPRIO(Logger::Priority::WARN) << "Personality slot " << currSlot << " as selected, but not found in personality list";
		return;
	}

	int page = buttonOfSlot / internclass->GetPersonalityButtonCount();
	if (page == g_pcPersonality.CurrPage()) {
		if (buttonOfSlot != loc_hiddenOrigButtonN) {
			SendMessageW(loc_hiddenOrigButton, BM_SETCHECK, BST_UNCHECKED, 0);
		}
		LocUnhideOrigButton(internclass);
		ShowWindow(loc_fakeButton, SW_HIDE);
	}
	else {
		int buttonN = buttonOfSlot % internclass->GetPersonalityButtonCount();
		int replacementButton = buttonN + g_pcPersonality.CurrPage() * internclass->GetPersonalityButtonCount();
		bool allreadyHidden = replacementButton >= loc_personalityNames.size();
		if (!allreadyHidden) {
			LocHideOrigButton(internclass, buttonN, currSlot);
			RECT rct = GetRelativeRect(loc_hiddenOrigButton);
			wchar_t text[128];
			text[0] = 0;
			GetWindowTextW(loc_hiddenOrigButton, text, 128);
			ShowWindow(loc_hiddenOrigButton, SW_HIDE);
			MoveWindow(loc_fakeButton, rct.left, rct.top, rct.right - rct.left, rct.bottom - rct.top, TRUE);
			SetWindowTextW(loc_fakeButton, text);
			ShowWindow(loc_fakeButton, SW_SHOW);
		}
		else {
			loc_naturallyhiddenOrigButton = internclass->GetPersonalityButtonHwnd(buttonN);
		}
	}
}