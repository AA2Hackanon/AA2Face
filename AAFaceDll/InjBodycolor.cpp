#include "InjBodycolor.h"
#include "Logger.h"
#include "GenUtils.h"

HWND g_edTanSelector = NULL;
HWND g_udTanSelector = NULL;

namespace {
	bool loc_tanButtonClicked = false;
	bool loc_characterLoaded = false;
	BYTE loc_tanslot = 0;
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
			int nButtons = internclass->GetTanButtonCount();
			bool found = false;
			for (int i = 0; i < nButtons; i++) {
				if (wnd == internclass->GetTanButtonHwnd(i)) {
					found = true;
					break;
				}
			}
			if (found) {
				//it was atan button, so respect that change
				LOGPRIO(Logger::Priority::SPAM) << "Tan button was clicked\n";
				loc_tanButtonClicked = true;
			}
		}
		if (wnd == g_edTanSelector) {
			if (notification == EN_UPDATE) {
				int ret = GetEditNumber(g_edTanSelector);
				bool changed = false;
				LOGPRIO(Logger::Priority::SPAM) << "tan edit got changed to " << ret << "\n";
				if (ret < 0) {
					//must not be < 0
					ret = 0;
					changed = true;
				}
				else if (ret > 255) {
					ret = 255;
					changed = true;
				}
				if (changed) {
					SetEditNumber(g_edTanSelector,ret);
				}
				internclass->SetChangeFlags();
			}
		}
	}
}

void __cdecl BodycolorAfterDialogInit(BodycolorDialogClass* internclass,HWND wnd) {
	//tan edit
	int btnCnt = internclass->GetTanButtonCount();
	HWND lastButton = internclass->GetTanButtonHwnd(btnCnt - 1);
	RECT rct = GetRelativeRect(lastButton);
	DWORD x,y,xw,yw;
	x = rct.right;
	y = rct.top;
	xw = rct.right - rct.left,yw = rct.bottom - rct.top;
	g_edTanSelector = CreateWindowExW(WS_EX_CLIENTEDGE,
		L"EDIT",L"0",WS_CHILD | WS_VISIBLE | ES_NUMBER | ES_AUTOHSCROLL,
		x,y,xw,yw,wnd,0,(HINSTANCE)g_AA2Base,0);
	SendMessage(g_edTanSelector,WM_SETFONT,(WPARAM)g_sysFont,TRUE);
	if (g_edTanSelector == NULL) {
		int error = GetLastError();
		LOGPRIO(Logger::Priority::ERR) << "Could not create Tan edit box! error " << error << "\n";
	}
	else {
		LOGPRIO(Logger::Priority::INFO) << "Successfully created Tan edit with handle " << g_edTanSelector << "\n";
	}
	InitCCs(ICC_UPDOWN_CLASS);
	g_udTanSelector = CreateWindowExW(0,
		UPDOWN_CLASSW,NULL,WS_VISIBLE | WS_CHILD | UDS_ALIGNRIGHT | UDS_ARROWKEYS | UDS_SETBUDDYINT,
		0,0,0,0,wnd,0,(HINSTANCE)g_AA2Base,0);
	SendMessageW(g_udTanSelector,UDM_SETBUDDY,(WPARAM)g_edTanSelector,0);
	SendMessageW(g_udTanSelector,UDM_SETRANGE,0,MAKELPARAM(255,0));
}

int __cdecl GetTanSelectorIndex(BodycolorDialogClass* internclass,int guiChosen) {
	if (loc_tanButtonClicked) {
		loc_tanButtonClicked = false;
		LOGPRIO(Logger::Priority::SPAM) << "selecting tan index after button click to " << guiChosen << "\n";
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

void __cdecl InitBodycolorTab(BodycolorDialogClass* internclass,bool before) {
	if (before) {
		//before the call, we take note of the hair slots used
		loc_tanslot = internclass->GetCurrentTanSlot();
		LOGPRIO(Logger::Priority::SPAM) << "tan loaded and initialized to " << loc_tanslot << "\n";
	}
	else {
		//after the call, we look at the hair, and correct them if they were changed
		BYTE newslot = internclass->GetCurrentTanSlot();
		internclass->SetCurrentTanSlot(loc_tanslot);
		SetEditNumber(g_edTanSelector,loc_tanslot);
		LOGPRIO(Logger::Priority::SPAM) << "tan loaded and corrected from " << newslot << "\n";
	}
}

void BodycolorDialogOnCharacterLoad() {
	loc_characterLoaded = true;
}