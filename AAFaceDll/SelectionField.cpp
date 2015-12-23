#include "SelectionField.h"
#include <Windows.h>
#include <fstream>

HWND g_cbSelector = NULL;
HWND g_edFaceSelector = NULL;
HWND g_udFaceSelector = NULL;

namespace {
	BYTE loc_cbi2fi[256] = {(BYTE)0};
	BYTE loc_fi2cbi[256] = {(BYTE)0};
	bool loc_radioButtonClicked = false;
	int loc_lastHairTab = -1;
	int loc_chosenHairs[4] = { -1,-1,-1,-1 };
	bool loc_hairButtonClicked = false;
}

void __cdecl InitSelector(HWND parent, HINSTANCE hInst) {
	DWORD x = 250, y = 320,xw = 200,yw = 600;
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

//---------------HAIR--------------

void __cdecl InitHairSelector(HWND parent,HINSTANCE hInst) {
	//{l:1821 t : 558 r : 1863 b : 578}
	//{l:1865 t : 531 r : 1907 b : 551}
	DWORD x = 420,y = 485,xw = 33,yw = 20;
	g_edFaceSelector = CreateWindowExW(WS_EX_CLIENTEDGE,
		L"EDIT",L"0",WS_CHILD | WS_VISIBLE | ES_NUMBER,
		x,y,xw,yw,parent,0,hInst,0);
}

//similar to GetSelectorIndex (the face one)
//where tab is 0 (front), 1 (side), 2 (back) or 3 (the other one)
//and guiChosen is the value polled from the original gui function
int __cdecl GetHairSelectorIndex(int tab, int guiChosen) {
	if (tab < 0 || tab > 3) return -1; //shouldnt happen, but lets just go safe here
	if(loc_hairButtonClicked) {
		//apply gui choice to our edit box, cause apparently the user clicked on a button,
		//so he probably wants that to show up
		loc_hairButtonClicked = false;
		loc_chosenHairs[tab] = guiChosen;
		wchar_t num[64];
		_itow_s(loc_chosenHairs[tab],num,64,10);
		SendMessageW(g_edFaceSelector,WM_SETTEXT,0,(LPARAM)num);
		return -1;
	}
	else if(tab != loc_lastHairTab) {
		loc_lastHairTab = tab;
		//hair tab changed, so we should refresh our value with the hair of this tab
		wchar_t num[64];
		_itow_s(loc_chosenHairs[tab],num,64,10);
		SendMessageW(g_edFaceSelector,WM_SETTEXT,0,(LPARAM)num);
		return loc_chosenHairs[tab];
	} else {
		//lets just always return our edit number for now and keep it in sync with the other numbers
		wchar_t num[64];
		SendMessageW(g_edFaceSelector,WM_GETTEXT,64,(LPARAM)num);
		num[63] = '\0';
		int ret = _wtoi(num);
		loc_chosenHairs[tab] = ret;
		if (ret < 0 || ret > 255) ret = -1;
		return ret;
	}
	return -1;
}

//this function is called twice, once before and once after execution of the init function.
//before == true at first and false at second call.
void __cdecl InitHairTab(HairDialogClass* internclass,bool before) {
	if(before) {
		//before the call, we take note of the hair slots used
		for (int i = 0; i < 4; i++) {
			BYTE* hairPtr = internclass->HairOfTab(i);
			if(hairPtr != NULL) {
				loc_chosenHairs[i] = *hairPtr;
			}
		}
	}
	else {
		//after the call, we look at the hair, and correct them if they were changed
		for (int i = 0; i < 4; i++) {
			BYTE* hairPtr = internclass->HairOfTab(i);
			if (hairPtr != NULL && loc_chosenHairs[i] != -1 && *hairPtr != loc_chosenHairs[i]) {
				*hairPtr = loc_chosenHairs[i];
			}
		}
		loc_lastHairTab = -1; //make sure that the edit knows that this value is new and has to be put into the edit field
	}
}

BOOL __cdecl HairDialogNotification(HairDialogClass* internclass,HWND hwndDlg,UINT msg,WPARAM wparam,LPARAM lparam) {
	const DWORD idBt0 = 0x28D8; //looked like this, hope they are always like this
	const DWORD idBt134 = 0x29A0;
	//as we can see, those would make for 200 buttons.
	//theres a jump from 55 to 56 with 290F to 294A and thats probably not the only one.
	//so while this is rather interisting information, lets not rely on this.
	if(lparam != 0) {
		if(msg == WM_COMMAND) {
			DWORD ctrlId = LOWORD(wparam);
			DWORD notification = HIWORD(wparam);
			HWND wnd = (HWND)lparam;
			if(notification == BN_CLICKED) {
				loc_hairButtonClicked = true; //This method worked so fine with the face, lets just do that again
			}
			else if(wnd == g_edFaceSelector && loc_lastHairTab >= 0 && loc_lastHairTab <= 3) {
				if(notification == EN_UPDATE) {
					//edit has been changed, so draw the selected new face
					wchar_t num[64];
					SendMessageW(g_edFaceSelector,WM_GETTEXT,64,(LPARAM)num);
					num[63] = '\0';
					int ret = _wtoi(num);
					loc_chosenHairs[loc_lastHairTab] = ret;
					internclass->SetHairChangeFlags();
				}
				
			}
		}
	}
	return FALSE;
}