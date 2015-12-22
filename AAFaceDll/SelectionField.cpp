#include "SelectionField.h"
#include <Windows.h>
#include <fstream>

HWND g_cbSelector = NULL;

namespace {
	BYTE loc_cbi2fi[256] = {(BYTE)0};
	BYTE loc_fi2cbi[256] = {(BYTE)0};
	bool loc_radioButtonClicked = false;
}

void __cdecl InitSelector(HWND parent, HINSTANCE hInst) {
	DWORD x = 250, y = 420,xw = 200,yw = 600;
	g_cbSelector = CreateWindowExW(WS_EX_CLIENTEDGE,L"COMBOBOX",L"None",
		CBS_DROPDOWN | CBS_HASSTRINGS | WS_CHILD | WS_VISIBLE | WS_VSCROLL ,x,y,xw,yw,
		parent,0,hInst,0);
	
	wchar_t wline[256] = L"--Select Above--";
	SendMessageW(g_cbSelector,CB_ADDSTRING,0,(LPARAM)wline);
	std::ifstream in("items.txt");
	
	if(!in.good()) {
		MessageBox(NULL,"Could not open items.txt","Error",0);
		return;
	}

	memset(loc_cbi2fi,0,sizeof(loc_cbi2fi));
	memset(loc_fi2cbi,0,sizeof(loc_fi2cbi));

	char line[256];
	while(in.good()) {
		in.getline(line,256);
		long fi = strtol(line,NULL,10); //index of this face
		if (fi > 255 || fi < 0 || fi == LONG_MAX || fi == LONG_MIN || fi == 0) fi = -1;
		//copy line to wline (i hate wchars, but its a nip game)
		int i = 0;
		for (i; line[i] != '\0'; i++) {
			wline[i] = line[i];
		}
		wline[i] = line[i];
		long cbi = SendMessageW(g_cbSelector,CB_ADDSTRING,0,(LPARAM)wline);
		loc_fi2cbi[fi] = (BYTE)cbi;
		loc_cbi2fi[cbi] = (BYTE)fi;
	}
	SendMessageW(g_cbSelector,CB_SETCURSEL,0,0);
}

//return -1 if radio button choice should be used
int __cdecl GetSelectorIndex() {
	if(g_cbSelector == NULL) {
		return -1;
	}
	if(loc_radioButtonClicked) {
		//radio button has been clicked before this, so we should use that one
		loc_radioButtonClicked = false;
		return -1;
	}
	LRESULT index = SendMessageW(g_cbSelector,CB_GETCURSEL,0,0);
	if (index == CB_ERR || index == 0 || index < 0 || index > 255)  return -1;

	return loc_cbi2fi[index];
}

BOOL __cdecl DialogNotification(void* internclass, HWND hwndDlg,UINT msg,WPARAM wparam,LPARAM lparam) {
	if(msg == WM_COMMAND) {
		if(HIWORD(wparam) == CBN_SELCHANGE) {
			//our edit has a selection change
			BYTE* classptr = (BYTE*)internclass;
			//apparently internclass+0x80 is a bool that is set to 1 if faces change, so lets just do that
			*(classptr+0x80) = 1;
			//this one also has to be set to 1 it seems. no idea why. lets hope it doesnt break.
			BYTE* someVal = *(BYTE**)(classptr+0x3C);
			*(someVal+0x12) = 1;
		}
		else if(HIWORD(wparam) == BN_CLICKED) {
			//radio button has been clicked
			loc_radioButtonClicked = true;
		}
	}
	return FALSE;
}

void __cdecl LoadFace(int findex) {
	BYTE cbindex = loc_fi2cbi[findex];
	SendMessageW(g_cbSelector,CB_SETCURSEL,cbindex,0);
}