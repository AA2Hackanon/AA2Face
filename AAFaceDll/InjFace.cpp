#include "InjFace.h"
#include "Logger.h"
#include "GenUtils.h"
#include "SlotFile.h"
#include <fstream>

HWND g_cbFaceSelector = NULL;

namespace {
	BYTE loc_cbi2fi[256] = { (BYTE)0 };
	BYTE loc_fi2cbi[256] = { (BYTE)0 };
	bool loc_radioButtonClicked = false;
};

void __cdecl InitFaceSelector(HWND parent,HINSTANCE hInst) {
	/*DWORD x = 250,y = 320,xw = 200,yw = 600;
	g_cbFaceSelector = CreateWindowExW(WS_EX_CLIENTEDGE,L"COMBOBOX",L"None",
		CBS_DROPDOWN | CBS_HASSTRINGS | WS_CHILD | WS_VISIBLE | WS_VSCROLL,x,y,xw,yw,
		parent,0,hInst,0);
	if(g_cbFaceSelector == NULL) {
		int error = GetLastError();
		g_Logger << Logger::Priority::ERR << "Could not create face combobox window: error code " << error << "\r\n";
	}
	else {
		g_Logger << Logger::Priority::INFO << "Successfully created face combobox with handle " << g_cbFaceSelector << "\r\n";
	}

	wchar_t wline[256] = L"--Select Above--";
	SendMessageW(g_cbFaceSelector,CB_ADDSTRING,0,(LPARAM)wline);
	std::ifstream in("items.txt");

	if (!in.good()) {
		g_Logger << Logger::Priority::ERR << "Could not populate combo box because items.txt could not be opened.\r\n";
		return;
	}

	memset(loc_cbi2fi,0,sizeof(loc_cbi2fi));
	memset(loc_fi2cbi,0,sizeof(loc_fi2cbi));

	char line[256];
	while (in.good()) {
		in.getline(line,256);
		long fi = strtol(line,NULL,10); //index of this face
		if (fi > 255 || fi < 0 || fi == LONG_MAX || fi == LONG_MIN || fi == 0) fi = -1;
		//copy line to wline (i hate wchars, but its a nip game)
		int i = 0;
		for (i; line[i] != '\0'; i++) {
			wline[i] = line[i];
		}
		wline[i] = line[i];
		long cbi = SendMessageW(g_cbFaceSelector,CB_ADDSTRING,0,(LPARAM)wline);
		loc_fi2cbi[fi] = (BYTE)cbi;
		loc_cbi2fi[cbi] = (BYTE)fi;
	}
	SendMessageW(g_cbFaceSelector,CB_SETCURSEL,0,0);*/
}

//return -1 if radio button choice should be used
int __cdecl GetFaceSelectorIndex() {
	if (g_cbFaceSelector == NULL) {
		LOGPRIO(Logger::Priority::WARN) << "Getting Face index, but face selector is NULL\n";
		return -1;
	}
	if (loc_radioButtonClicked) {
		//radio button has been clicked before this, so we should use that one
		loc_radioButtonClicked = false;
		return -1;
	}
	LRESULT index = SendMessageW(g_cbFaceSelector,CB_GETCURSEL,0,0);
	if (index == CB_ERR || index == 0 || index < 0 || index > 255)  {
		LOGPRIO(Logger::Priority::WARN) << "Invalid Face index " << index << " was polled\n";
		return -1;
	}

	return loc_cbi2fi[index];
}

BOOL __cdecl FaceDialogNotification(FaceDialogClass* internclass,HWND hwndDlg,UINT msg,WPARAM wparam,LPARAM lparam) {
	if (msg == WM_COMMAND) {
		if (HIWORD(wparam) == CBN_SELCHANGE) {
			LOGPRIO(Logger::Priority::SPAM) << "Face selection changed; setting face flags.\n";
			internclass->SetChangeFlags();
		}
		else if (HIWORD(wparam) == BN_CLICKED) {
			//radio button has been clicked
			LOGPRIO(Logger::Priority::SPAM) << "Vanilla Face button was clicekd\n";
			loc_radioButtonClicked = true;
		}
	}
	return FALSE;
}

void __cdecl FaceDialogAfterInit(FaceDialogClass* internclass, HWND wnd) {
	DWORD x = 250,y = 320,xw = 200,yw = 600;
	g_cbFaceSelector = CreateWindowExW(WS_EX_CLIENTEDGE,L"COMBOBOX",L"None",
		CBS_DROPDOWN | CBS_HASSTRINGS | WS_CHILD | WS_VISIBLE | WS_VSCROLL,x,y,xw,yw,
		wnd,0,(HINSTANCE)g_AA2Base,0);
	SendMessage(g_cbFaceSelector,WM_SETFONT,(WPARAM)g_sysFont,TRUE);
	if (g_cbFaceSelector == NULL) {
		int error = GetLastError();
		LOGPRIO(Logger::Priority::ERR) << "Could not create face combobox window: error code " << error << "\n";
	}
	else {
		LOGPRIO(Logger::Priority::INFO) << "Successfully created face combobox with handle " << g_cbFaceSelector << "\n";
	}

	wchar_t wline[256] = L"--Select Above--";
	SendMessageW(g_cbFaceSelector,CB_ADDSTRING,0,(LPARAM)wline);
	std::ifstream in("AAFace\\items.txt");

	if (!in.good()) {
		LOGPRIO(Logger::Priority::ERR) << "Could not populate combo box because items.txt could not be opened.\n";
		return;
	}

	memset(loc_cbi2fi,0,sizeof(loc_cbi2fi));
	memset(loc_fi2cbi,0,sizeof(loc_fi2cbi));

	char line[256];
	while (in.good()) {
		in.getline(line,256);
		long fi = strtol(line,NULL,10); //index of this face
		if (fi > 255 || fi < 0 || fi == LONG_MAX || fi == LONG_MIN || fi == 0) fi = -1;
		//copy line to wline (i hate wchars, but its a nip game)
		int i = 0;
		for (i; line[i] != '\0'; i++) {
			wline[i] = line[i];
		}
		wline[i] = line[i];
		long cbi = SendMessageW(g_cbFaceSelector,CB_ADDSTRING,0,(LPARAM)wline);
		loc_fi2cbi[fi] = (BYTE)cbi;
		loc_cbi2fi[cbi] = (BYTE)fi;
	}
	SendMessageW(g_cbFaceSelector,CB_SETCURSEL,0,0);

	//only 7 buttons are visible.
	RECT rct;
	if(internclass->GetButtonCount() < 7) {
		//not sure what happened here.
		rct = GetNextButtonPosition<FaceDialogClass>(internclass,7,&FaceDialogClass::GetFaceSlotButton,3);
	}
	else {
		//we are going to give it the x start of the 5th button, the xend of the 6th and the y of the 7th button
		HWND bt5 = internclass->GetFaceSlotButton(4);
		HWND bt6 = internclass->GetFaceSlotButton(5);
		HWND bt7 = internclass->GetFaceSlotButton(6);
		RECT rct5 = GetRelativeRect(bt5);
		RECT rct6 = GetRelativeRect(bt6);
		RECT rct7 = GetRelativeRect(bt7);
		rct.left = rct5.left;
		rct.right = rct6.right;
		rct.top = rct7.top;
		rct.bottom = rct.top + yw;
	}
	MoveWindow(g_cbFaceSelector,rct.left,rct.top,rct.right-rct.left,rct.bottom-rct.top,TRUE);
}

void __cdecl LoadFace(int findex) {
	BYTE cbindex = loc_fi2cbi[findex];
	LOGPRIO(Logger::Priority::SPAM) << "Loading face " << findex << "(mapped to " << (int)cbindex << ")\n";
	SendMessageW(g_cbFaceSelector,CB_SETCURSEL,cbindex,0);
}

void __cdecl RandomFaceSelect(FaceDialogClass* internclass) {
	BYTE randSlot;
	if (g_slotFile.ValidSlotCount(SlotFile::FACE)) {		
		do {
				randSlot = rand()%256; 
		} while (!g_slotFile.SlotExists(SlotFile::FACE,randSlot));
		internclass->SetFaceSlot(randSlot); 
		SendMessage(g_cbFaceSelector,CB_SETCURSEL,loc_fi2cbi[randSlot],0);
	}
}