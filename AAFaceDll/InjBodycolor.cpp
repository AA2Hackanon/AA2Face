#include "InjBodycolor.h"
#include "Logger.h"
#include "GenUtils.h"

HWND g_edTanSelector = NULL;
HWND g_udTanSelector = NULL;
HWND g_edNipTypeSelector = NULL;
HWND g_udNipTypeSelector = NULL;
HWND g_edNipColorSelector = NULL;
HWND g_udNipColorSelector = NULL;
HWND g_edMosaicSelector = NULL;
HWND g_udMosaicSelector = NULL;
HWND g_edPubSelector = NULL;
HWND g_udPubSelector = NULL;

namespace {
	bool loc_tanButtonClicked = false;
	bool loc_nipTypeButtonClicked = false;
	bool loc_nipColorButtonClicked = false;
	bool loc_pubHairButtonClicked = false;
	bool loc_mosaicButtonClicked = false;
	bool loc_ignoreNextChange = false;
	bool loc_characterLoaded = false;
	BYTE loc_nipTypeSlot = 0;
	BYTE loc_nipColorSlot = 0;
	BYTE loc_tanSlot = 0;
	BYTE loc_mosaicSlot = 0;
	BYTE loc_pubHairSlot = 0;
};

void __cdecl BodycolorDialogNotification(BodycolorDialogClass* internclass,HWND wnd,UINT msg,WPARAM wparam,LPARAM lparam) {
	if(msg == BODYCOLORMESSAGE_ADDTAN) {
		int diff = (int)wparam;
		LOGPRIO(Logger::Priority::SPAM) << "recieved ADDTAN message with diff " << diff << "\n";
		SetEditNumber(g_edTanSelector,GetEditNumber(g_edTanSelector) + diff);
	}
	else if(msg == BODYCOLORMESSAGE_SETTAN) {
		int slot = (int)wparam;
		LOGPRIO(Logger::Priority::SPAM) << "recieved ADDTAN message with slot " << slot << "\n";
		SetEditNumber(g_edTanSelector,slot);
	}
	else if (msg == WM_COMMAND && lparam != 0) {
		DWORD ctrlId = LOWORD(wparam);
		DWORD notification = HIWORD(wparam);
		HWND wnd = (HWND)lparam;
		if (notification == BN_CLICKED) {
			//check if the pressed button was a tan-button
			bool found = 
				FindButtonInList(wnd,internclass,&BodycolorDialogClass::GetTanButtonHwnd,internclass->GetTanButtonCount()) != -1;
			if (found) {
				//it was atan button, so respect that change
				LOGPRIO(Logger::Priority::SPAM) << "Tan button was clicked\n";
				loc_tanButtonClicked = true;
			}
			//or nip stuff
			found =
				FindButtonInList(wnd,internclass,&BodycolorDialogClass::GetNipTypeButtonHwnd,internclass->GetNipTypeButtonCount()) != -1;
			if (found) {
				//it was atan button, so respect that change
				LOGPRIO(Logger::Priority::SPAM) << "Nip Type button was clicked\n";
				loc_nipTypeButtonClicked = true;
			}
			found =
				FindButtonInList(wnd,internclass,&BodycolorDialogClass::GetNipColorButtonHwnd,internclass->GetNipColorButtonCount()) != -1;
			if (found) {
				//it was atan button, so respect that change
				LOGPRIO(Logger::Priority::SPAM) << "Nip color button was clicked\n";
				loc_nipColorButtonClicked = true;
			}
			//pub stuff
			found =
				FindButtonInList(wnd,internclass,&BodycolorDialogClass::GetPubHairButtonHwnd,internclass->GetPubHairButtonCount()) != -1;
			if (found) {
				//it was atan button, so respect that change
				LOGPRIO(Logger::Priority::SPAM) << "Nip color button was clicked\n";
				loc_pubHairButtonClicked = true;
			}
			found =
				FindButtonInList(wnd,internclass,&BodycolorDialogClass::GetMosiacButtonHwnd,internclass->GetMosaicButtonCount()) != -1;
			if (found) {
				//it was atan button, so respect that change
				LOGPRIO(Logger::Priority::SPAM) << "Nip color button was clicked\n";
				loc_mosaicButtonClicked = true;
			}
		}
		if (wnd == g_edTanSelector) {
			if (notification == EN_UPDATE) {
				LOGPRIO(Logger::Priority::SPAM) << "tan edit got changed to " << GetEditNumber(g_edTanSelector) << "\n";
				LimitEditInt(g_edTanSelector,0,255);
				internclass->SetChangeFlags();
			}
		}
		else if(wnd == g_edNipColorSelector) {
			if (loc_ignoreNextChange) loc_ignoreNextChange = false;
			else if(notification == EN_UPDATE) {
				LOGPRIO(Logger::Priority::SPAM) << "nip color edit got changed to " << GetEditNumber(g_edNipColorSelector) << "\n";
				LimitEditInt(g_edNipColorSelector,0,255);
				internclass->SetChangeFlags();
			}
		}
		else if(wnd == g_edNipTypeSelector) {
			if (loc_ignoreNextChange) loc_ignoreNextChange = false;
			else if (notification == EN_UPDATE) {
				LOGPRIO(Logger::Priority::SPAM) << "nip type edit got changed to " << GetEditNumber(g_edNipTypeSelector) << "\n";
				LimitEditInt(g_edNipTypeSelector,0,255);
				internclass->SetChangeFlags();
			}
		}
		else if(wnd == g_edMosaicSelector) {
			if (loc_ignoreNextChange) loc_ignoreNextChange = false;
			else if(notification == EN_UPDATE) {
				LOGPRIO(Logger::Priority::SPAM) << "mosaic edit got changed to " << GetEditNumber(g_edMosaicSelector) << "\n";
				LimitEditInt(g_edMosaicSelector,0,255);
				internclass->SetChangeFlags();
			}
		}
		else if(wnd == g_edPubSelector) {
			if (loc_ignoreNextChange) loc_ignoreNextChange = false;
			else if(notification == EN_UPDATE) {
				LOGPRIO(Logger::Priority::SPAM) << "pub hair edit got changed to " << GetEditNumber(g_edPubSelector) << "\n";
				LimitEditInt(g_edPubSelector,0,255);
				internclass->SetChangeFlags();
			}
		}
	}
}

void __cdecl BodycolorAfterDialogInit(BodycolorDialogClass* internclass,HWND wnd) {
	//nip type and color edits
	int btnCnt = internclass->GetNipTypeButtonCount();
	HWND lastButton = internclass->GetNipTypeButtonHwnd(btnCnt - 1);
	RECT rct = GetRelativeRect(lastButton);
	DWORD x,y,xw,yw;
	x = rct.right;
	y = rct.top;
	xw = rct.right - rct.left,yw = rct.bottom - rct.top;
	CreateUpDownControl(wnd,x,y,xw,yw,g_edNipTypeSelector,g_udNipTypeSelector);
	x += xw;
	CreateUpDownControl(wnd,x,y,xw,yw,g_edNipColorSelector,g_udNipColorSelector);

	//tan edit
	btnCnt = internclass->GetTanButtonCount();
	lastButton = internclass->GetTanButtonHwnd(btnCnt - 1);
	rct = GetRelativeRect(lastButton);
	x = rct.right;
	y = rct.top;
	xw = rct.right - rct.left,yw = rct.bottom - rct.top;
	CreateUpDownControl(wnd,x,y,xw,yw,g_edTanSelector,g_udTanSelector);

	//mosaic
	btnCnt = internclass->GetMosaicButtonCount();
	lastButton = internclass->GetMosiacButtonHwnd(btnCnt - 1);
	rct = GetRelativeRect(lastButton);
	x = rct.right;
	y = rct.top;
	xw = rct.right - rct.left,yw = rct.bottom - rct.top;
	CreateUpDownControl(wnd,x,y,xw,yw,g_edMosaicSelector,g_udMosaicSelector);

	//pub hair
	btnCnt = internclass->GetPubHairButtonCount();
	lastButton = internclass->GetPubHairButtonHwnd(btnCnt - 1);
	rct = GetRelativeRect(lastButton);
	x = rct.right;
	y = rct.top;
	xw = rct.right - rct.left,yw = rct.bottom - rct.top;
	CreateUpDownControl(wnd,x,y,xw,yw,g_edPubSelector,g_udPubSelector);
}

int __cdecl GetTanSelectorIndex(BodycolorDialogClass* internclass,int guiChosen) {
	if (loc_tanButtonClicked) {
		loc_tanButtonClicked = false;
		LOGPRIO(Logger::Priority::SPAM) << "selecting tan index after button click to " << guiChosen << "\n";
		loc_ignoreNextChange = true;
		SetEditNumber(g_edTanSelector,guiChosen);
		return -1;
	}
	else {
		int ret = GetEditNumber(g_edTanSelector);
		LOGPRIO(Logger::Priority::SPAM) << "selecting tan index from edit to " << ret << "\n";
		if (ret < 0 || ret > 255) ret = -1;
		return ret;
	}
}

int __cdecl GetNipTypeSelectorIndex(BodycolorDialogClass* internclass,int guiChosen) {
	if (loc_nipTypeButtonClicked) {
		loc_nipTypeButtonClicked = false;
		LOGPRIO(Logger::Priority::SPAM) << "selecting nip type index after button click to " << guiChosen << "\n";
		loc_ignoreNextChange = true;
		SetEditNumber(g_edNipTypeSelector,guiChosen);
		return -1;
	}
	else {
		int ret = GetEditNumber(g_edNipTypeSelector);
		LOGPRIO(Logger::Priority::SPAM) << "selecting nip type index from edit to " << ret << "\n";
		if (ret < 0 || ret > 255) ret = -1;
		return ret;
	}
	return 0;
}
int __cdecl GetNipColorSelectorIndex(BodycolorDialogClass* internclass,int guiChosen) {
	if (loc_nipColorButtonClicked) {
		loc_nipColorButtonClicked = false;
		LOGPRIO(Logger::Priority::SPAM) << "selecting nip color index after button click to " << guiChosen << "\n";
		loc_ignoreNextChange = true;
		SetEditNumber(g_edNipColorSelector,guiChosen);
		return -1;
	}
	else {
		int ret = GetEditNumber(g_edNipColorSelector);
		LOGPRIO(Logger::Priority::SPAM) << "selecting nip color index from edit to " << ret << "\n";
		if (ret < 0 || ret > 255) ret = -1;
		return ret;
	}
	return 0;
}

int __cdecl GetMosaicSelectorIndex(BodycolorDialogClass* internclass,int guiChosen) {
	if (loc_mosaicButtonClicked) {
		loc_mosaicButtonClicked = false;
		LOGPRIO(Logger::Priority::SPAM) << "selecting mosaic index after button click to " << guiChosen << "\n";
		loc_ignoreNextChange = true;
		SetEditNumber(g_edMosaicSelector,guiChosen);
		return -1;
	}
	else {
		int ret = GetEditNumber(g_edMosaicSelector);
		LOGPRIO(Logger::Priority::SPAM) << "selecting mosaic index from edit to " << ret << "\n";
		if (ret < 0 || ret > 255) ret = -1;
		return ret;
	}
	return 0;
}
int __cdecl GetPubHairSelectorIndex(BodycolorDialogClass* internclass,int guiChosen) {
	if (loc_pubHairButtonClicked) {
		loc_pubHairButtonClicked = false;
		LOGPRIO(Logger::Priority::SPAM) << "selecting pub hair index after button click to " << guiChosen << "\n";
		loc_ignoreNextChange = true;
		SetEditNumber(g_edPubSelector,guiChosen);
		return -1;
	}
	else {
		int ret = GetEditNumber(g_edPubSelector);
		LOGPRIO(Logger::Priority::SPAM) << "selecting pub hair index from edit to " << ret << "\n";
		if (ret < 0 || ret > 255) ret = -1;
		return ret;
	}
	return 0;
}

void __cdecl InitBodycolorTab(BodycolorDialogClass* internclass,bool before) {
	if (before) {
		//before the call, we take note of the tan slots used
		loc_nipTypeSlot = internclass->GetCurrentNipTypeSlot();
		LOGPRIO(Logger::Priority::SPAM) << "nip type loaded and initialized to " << loc_nipTypeSlot << "\n";
		loc_nipColorSlot = internclass->GetCurrentNipColorSlot();
		LOGPRIO(Logger::Priority::SPAM) << "nip color loaded and initialized to " << loc_nipColorSlot << "\n";
		loc_tanSlot = internclass->GetCurrentTanSlot();
		LOGPRIO(Logger::Priority::SPAM) << "tan loaded and initialized to " << loc_tanSlot << "\n";
		loc_mosaicSlot = internclass->GetCurrentMosaicSlot();
		LOGPRIO(Logger::Priority::SPAM) << "mosaic loaded and initialized to " << loc_mosaicSlot << "\n";
		loc_pubHairSlot = internclass->GetCurrentPubHairSlot();
		LOGPRIO(Logger::Priority::SPAM) << "pub loaded and initialized to " << loc_pubHairSlot << "\n";
	}
	else {
		//after the call, we look at the tan, and correct them if they were changed
		BYTE newslot = internclass->GetCurrentNipTypeSlot();
		internclass->SetCurrentNipTypeSlot(loc_nipTypeSlot);
		SetEditNumber(g_edNipTypeSelector,loc_nipTypeSlot);
		LOGPRIO(Logger::Priority::SPAM) << "nip type loaded and corrected from " << newslot << "\n";
		newslot = internclass->GetCurrentNipColorSlot();
		internclass->SetCurrentNipColorSlot(loc_nipColorSlot);
		SetEditNumber(g_edNipColorSelector,loc_nipColorSlot);
		LOGPRIO(Logger::Priority::SPAM) << "nip color loaded and corrected from " << newslot << "\n";
		newslot = internclass->GetCurrentTanSlot();
		internclass->SetCurrentTanSlot(loc_tanSlot);
		SetEditNumber(g_edTanSelector,loc_tanSlot);
		LOGPRIO(Logger::Priority::SPAM) << "tan loaded and corrected from " << newslot << "\n";
		newslot = internclass->GetCurrentMosaicSlot();
		internclass->SetCurrentMosaicSlot(loc_mosaicSlot);
		SetEditNumber(g_edMosaicSelector,loc_mosaicSlot);
		LOGPRIO(Logger::Priority::SPAM) << "mosaic loaded and corrected from " << newslot << "\n";
		newslot = internclass->GetCurrentPubHairSlot();
		internclass->SetCurrentPubHairSlot(loc_pubHairSlot);
		SetEditNumber(g_edPubSelector,loc_pubHairSlot);
		LOGPRIO(Logger::Priority::SPAM) << "pub loaded and corrected from " << newslot << "\n";
	}
}

void BodycolorDialogOnCharacterLoad() {
	loc_characterLoaded = true;
}