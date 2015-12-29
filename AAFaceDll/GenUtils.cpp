#include "GenUtils.h"

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