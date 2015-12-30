#pragma once
#include <Windows.h>

extern HFONT g_sysFont;

int GetEditNumber(HWND edit);
void SetEditNumber(HWND edit,int number);
RECT GetRelativeRect(HWND wnd);

template <typename T>
RECT GetNextButtonPosition(const T* internclass, //this-object used
							int nButtons, //total amount of buttons that allready exist
							HWND(T::*GetButtonWnd)(BYTE slot) const, //function that gives HWND of a specific button
							int rowsize) //size of a row of buttons
{
	DWORD dx = 0,dy = 0;
	RECT ret;
	ret.bottom = ret.top = ret.left = ret.right = 0;
	if (nButtons == 0) return ret;
	if(nButtons < rowsize) {
		//not even 1 row, just put it behind it
		HWND lastBtn = (internclass->*GetButtonWnd)(nButtons-1);
		RECT rctLastBtn = GetRelativeRect(lastBtn);
		DWORD xw = rctLastBtn.right - rctLastBtn.left,
			  yw = rctLastBtn.bottom - rctLastBtn.top;
		//find free space between buttons
		if(nButtons > 1) {
			HWND secondLastButton = (internclass->*GetButtonWnd)(nButtons-2);
			RECT rctSecondLastButton = GetRelativeRect(secondLastButton);
			dx = rctLastBtn.left - rctSecondLastButton.right;
		}
		ret.left = rctLastBtn.right + dx;
		ret.top = rctLastBtn.top;
		ret.right = ret.left + xw;
		ret.bottom = ret.top + yw;
		return ret;
	}
	else {
		//first, find out which spot in the 2-dimensional grid the new button would be put on
		int gridX = nButtons % rowsize;
		int gridY = nButtons / rowsize;
		//the x button has to exist
		HWND xbutton = (internclass->*GetButtonWnd)(gridX);
		RECT xrect = GetRelativeRect(xbutton);
		DWORD xw = xrect.right - xrect.left;
		//the y button does not have to exist as the last row can be just full)
		RECT yrect;
		if(gridX != 0) {
			HWND ybutton = (internclass->*GetButtonWnd)(nButtons - 1);
			yrect = GetRelativeRect(ybutton);
		}
		else {
			//we are the first of a new line, we will have to caculate our y based on the space between the previous 2 lines
			DWORD dy = 0;
			HWND y1 = (internclass->*GetButtonWnd)(rowsize * (gridY-1));
			RECT yrect1 = GetRelativeRect(y1);
			DWORD yw = yrect1.bottom - yrect1.top;
			//calculate difference in y between the previous 2 lines
			if(gridY >= 2) {
				HWND y2 = (internclass->*GetButtonWnd)(rowsize * (gridY-2));
				RECT yrect2 = GetRelativeRect(y2);
				dy = yrect1.top - yrect2.bottom;
			}
			yrect;
			yrect.top = yrect1.bottom + dy;
			yrect.bottom = yrect.top + yw;
		}

		ret.left = xrect.left;
		ret.right = xrect.right;
		ret.top = yrect.top;
		ret.bottom = yrect.bottom;
		return ret;
	}
}