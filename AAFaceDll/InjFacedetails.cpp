#include "InjFacedetails.h"
#include "Logger.h"
#include "GenUtils.h"

HWND g_edGlassesSelector = NULL;
HWND g_udGlassesSelector = NULL;

namespace {
	bool loc_glassesButtonClicked = false;
	bool loc_characterLoaded = false;
	BYTE loc_glassesslot = 0;
};

void __cdecl InitGlassesSelector(HWND parent,HINSTANCE hInst) {
	/*DWORD x = 420,y = 485,xw = 33,yw = 20;
	g_edGlassesSelector = CreateWindowExW(WS_EX_CLIENTEDGE,
		L"EDIT",L"0",WS_CHILD | WS_VISIBLE | ES_NUMBER,
		x,y,xw,yw,parent,0,hInst,0);
	InitCCs(ICC_UPDOWN_CLASS);
	g_udGlassesSelector = CreateWindowExW(0,
		UPDOWN_CLASSW,NULL,WS_VISIBLE | WS_CHILD | UDS_ALIGNRIGHT | UDS_ARROWKEYS | UDS_SETBUDDYINT,
		0,0,0,0,parent,0,hInst,0);
	SendMessageW(g_udGlassesSelector,UDM_SETBUDDY,(WPARAM)g_edGlassesSelector,0);
	SendMessageW(g_udGlassesSelector,UDM_SETRANGE,0,MAKELPARAM(255,0));
	ShowWindow(g_udGlassesSelector,SW_HIDE);*/
}

void __cdecl FacedetailsDialogNotification(FacedetailsDialogClass* internclass,HWND wnd,UINT msg,WPARAM wparam,LPARAM lparam) {
	if(msg == FACEDETAILSMESSAGE_ADDGLASSES) {
		int diff = (int)wparam;
		int ret = GetEditNumber(g_edGlassesSelector);
		ret += diff;
		SetEditNumber(g_edGlassesSelector,ret);
	}
	else if(msg == FACEDETAILSMESSAGE_SETGLASSES) {
		int set = (int)wparam;
		SetEditNumber(g_edGlassesSelector,set);
	}
	else if (msg == WM_COMMAND && lparam != 0) {
		DWORD ctrlId = LOWORD(wparam);
		DWORD notification = HIWORD(wparam);
		HWND wnd = (HWND)lparam;
		if (notification == BN_CLICKED) {
			//check if the pressed button was a glasses-button
			int nButtons = internclass->GetGlassButtonCount();
			bool found = false;
			for (int i = 0; i < nButtons; i++) {
				if (wnd == internclass->GetGlassesButtonWnd(i)) {
					found = true;
					break;
				}
			}
			if (found) {
				//it was a glasses button, so respect that change
				loc_glassesButtonClicked = true;
			}
		}
		if (wnd == g_edGlassesSelector) {
			if (notification == EN_UPDATE) {
				int ret = GetEditNumber(g_edGlassesSelector);
				bool changed = false;
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
					SetEditNumber(g_edGlassesSelector,ret);
				}
				internclass->SetChangeFlags();
			}
		}
	}
}

void __cdecl FacedetailsAfterDialogInit(FacedetailsDialogClass* internclass, HWND wnd) {
	//glasses edit
	int btnCnt = internclass->GetGlassButtonCount();
	HWND lastButton = internclass->GetGlassesButtonWnd(btnCnt-1);
	RECT rct = GetRelativeRect(lastButton);
	DWORD x,y,xw,yw;
	x = rct.right;
	y = rct.top;
	xw = rct.right - rct.left,yw = rct.bottom - rct.top;
	g_edGlassesSelector = CreateWindowExW(WS_EX_CLIENTEDGE,
		L"EDIT",L"0",WS_CHILD | WS_VISIBLE | ES_NUMBER,
		x,y,xw,yw,wnd,0,(HINSTANCE)g_AA2Base,0);
	if(g_edGlassesSelector == NULL) {
		int error = GetLastError();
		g_Logger << Logger::Priority::ERR << "Could not create Glasses edit box! error " << error << "\r\n";
	}
	else {
		g_Logger << Logger::Priority::INFO << "Successfully created Glasses edit with handle " << g_edGlassesSelector << "\r\n";
	}
	InitCCs(ICC_UPDOWN_CLASS);
	g_udGlassesSelector = CreateWindowExW(0,
		UPDOWN_CLASSW,NULL,WS_VISIBLE | WS_CHILD | UDS_ALIGNRIGHT | UDS_ARROWKEYS | UDS_SETBUDDYINT,
		0,0,0,0,wnd,0,(HINSTANCE)g_AA2Base,0);
	SendMessageW(g_udGlassesSelector,UDM_SETBUDDY,(WPARAM)g_edGlassesSelector,0);
	SendMessageW(g_udGlassesSelector,UDM_SETRANGE,0,MAKELPARAM(255,0));

}

void __cdecl FacedetailsAfterInit(void* internclass) {
	//this is the "wrong" class, where *internclass+0xB8 is the detailsclass
}

int __cdecl GetGlassesSelectorIndex(FacedetailsDialogClass* internclass,int guiChosen) {
	if(loc_glassesButtonClicked) {
		loc_glassesButtonClicked = false;
		SetEditNumber(g_edGlassesSelector,guiChosen);
		return -1;
	}
	else {
		int ret = GetEditNumber(g_edGlassesSelector);
		if (ret < 0 || ret > 255) ret = -1;
		return ret;
	}
}

void __cdecl InitFacedetailsTab(FacedetailsDialogClass* internclass,bool before) {
	if(before) {
		//before the call, we take note of the hair slots used
		loc_glassesslot = internclass->GetGlassesSlot();
	}
	else {
		//after the call, we look at the hair, and correct them if they were changed
		BYTE newslot = internclass->GetGlassesSlot();
		SetEditNumber(g_edGlassesSelector,loc_glassesslot);
		internclass->SetGlassesSlot(loc_glassesslot);
	}
}

void FacedetailsDialogOnCharacterLoad() {
	
}