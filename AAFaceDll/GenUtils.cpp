#include "GenUtils.h"
#include "Logger.h"
#include "Injections.h"
#include "ExternConstants.h"

namespace {
	HFONT GetSysFont() {
		NONCLIENTMETRICS ncm;
		ncm.cbSize = sizeof(ncm);
		SystemParametersInfo(SPI_GETNONCLIENTMETRICS,sizeof(NONCLIENTMETRICS),&ncm,0);
		return CreateFontIndirect(&(ncm.lfMenuFont));
	}
}
HFONT g_sysFont = GetSysFont();

int GetEditNumber(HWND edit) {
	wchar_t num[64];
	SendMessageW(edit,WM_GETTEXT,64,(LPARAM)num);
	num[63] = '\0';
	int ret = _wtoi(num);
	return ret;
}
void SetEditNumber(HWND edit,int number) {
	wchar_t num[64];
	num[0] = '\0';
	_itow_s(number,num,64,10);
	SendMessageW(edit,WM_SETTEXT,0,(LPARAM)num);
}

RECT GetRelativeRect(HWND wnd) {
	RECT rct;
	GetWindowRect(wnd,&rct);
	MapWindowPoints(HWND_DESKTOP,GetParent(wnd),(LPPOINT)&rct,2); //rect is 2 points, apparently
	return rct;
}

void LimitEditInt(HWND edit,int low,int high) {
	bool changed = false;
	int editNumber = GetEditNumber(edit);
	if (editNumber < low) {
		//must not be < 0
		editNumber = low;
		changed = true;
	}
	else if (editNumber > high) {
		editNumber = 255;
		changed = true;
	}
	if (changed) {
		SetEditNumber(edit,editNumber);
	}
}

bool CreateUpDownControl(HWND parent, int x,int y,int xw,int yw,_Out_ HWND& edWnd,_Out_ HWND& udWnd) {
	static int execCount = 0;
	execCount++;
	edWnd = CreateWindowExW(WS_EX_CLIENTEDGE,
		L"EDIT",L"0",WS_CHILD | WS_VISIBLE | ES_NUMBER | ES_AUTOHSCROLL,
		x,y,xw,yw,parent,0,(HINSTANCE)g_AA2Base,0);
	SendMessage(edWnd,WM_SETFONT,(WPARAM)g_sysFont,TRUE);
	if (edWnd == NULL) {
		int error = GetLastError();
		LOGPRIO(Logger::Priority::ERR) << "Could not create edit box nr " << execCount << "! error " << error << "\n";
	}
	else {
		LOGPRIO(Logger::Priority::INFO) << "Successfully created edit nr " << execCount << " with handle " << edWnd << "\n";
	}
	InitCCs(ICC_UPDOWN_CLASS);
	udWnd = CreateWindowExW(0,
		UPDOWN_CLASSW,NULL,WS_VISIBLE | WS_CHILD | UDS_ALIGNRIGHT | UDS_ARROWKEYS | UDS_SETBUDDYINT,
		0,0,0,0,parent,0,(HINSTANCE)g_AA2Base,0);
	SendMessageW(udWnd,UDM_SETBUDDY,(WPARAM)edWnd,0);
	SendMessageW(udWnd,UDM_SETRANGE,0,MAKELPARAM(255,0));
	return true;
}