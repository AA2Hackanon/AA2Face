#pragma once
#include <Windows.h>

#include "Logger.h"
#include "GenUtils.h"

enum class BoundUpDownPosition { RIGHT_OF_LAST,BELOW_LAST };

/*
 * General class that represents an up-down-control that is bound to buttons
 * created by the editor itself
 */
template<
		 typename T,						//internclass where the updown controll is being inserted into
		 HWND(T::*GetWndFunc)(BYTE) const,	//memberfunctionpointer to its GetXYZButtonWnd
		 int(T::*GetWndCnt)() const,		//memberfucntionpointer to its GetXYZButtonCnt function
		 BYTE(T::*GetSlot)() const,			//memberfunctionpointer to the function that gets its current slot
		 void(T::*SetSlot)(BYTE slot),		//memberfunctionpointer to the function that sets its current slot
		 void(T::*SetChangeFlags)()			//memberfunctionpointer to its SetChangeFlags function
		>	
class BoundUpDownControl {
	HWND m_upDown;
	HWND m_edit;
	bool m_buttonPressed;
	bool m_editChanged;
	bool m_ignoreNext;
	const char* m_name;

	BYTE m_loggedSlot;
public:
	BoundUpDownControl(const char* name) : m_buttonPressed(false), m_editChanged(false), m_ignoreNext(false), m_name(name) {}
	
	/*
	 * Creates an up-down-control in the given rect
	 */
	void Initialize(HWND parent, RECT rct) {
		DWORD x,y,xw,yw;
		x = rct.left;
		y = rct.top;
		xw = rct.right - rct.left,yw = rct.bottom - rct.top;
		CreateUpDownControl(parent,x,y,xw,yw,m_edit,m_upDown);
	}
	/*
	 * Creates an up-down-control on the given position with given size
	 */
	void Initialize(HWND parent,DWORD x, DWORD y, DWORD xw, DWORD yw) {
		CreateUpDownControl(parent,x,y,xw,yw,m_edit,m_upDown);
	}
	/*
	 * Creates an up-down-control with given alignment in comparision to other buttons
	 */
	void Initialize(HWND parent,T* internclass,BoundUpDownPosition pos) {
		int btnCnt = (internclass->*GetWndCnt)();
		HWND lastButton = (internclass->*GetWndFunc)(btnCnt-1);
		RECT rct = GetRelativeRect(lastButton);
		DWORD x,y,xw,yw;
		x = rct.left;
		y = rct.top;
		xw = rct.right - rct.left,yw = rct.bottom - rct.top;
		switch(pos) {
		case BoundUpDownPosition::RIGHT_OF_LAST: {
			x += xw;
			break; }
		case BoundUpDownPosition::BELOW_LAST: {
			y += yw;
			break; }
		};
		CreateUpDownControl(parent,x,y,xw,yw,m_edit,m_upDown);
	}

	/*
	 * Has to be called on the hooked notification function of the internclass; handles WM_COMMAND
	 */
	void HookedWmCommandNotification(T* internclass, HWND wnd,UINT msg,WPARAM wparam,LPARAM lparam) {
		if (msg != WM_COMMAND || lparam == 0) return;
		HWND targetwnd = (HWND)lparam;
		DWORD ctrlId = LOWORD(wparam);
		DWORD notification = HIWORD(wparam);
		if (notification == BN_CLICKED) {
			//if a button from our associated buttons has been clicked, find it
			bool found =
				FindButtonInList(targetwnd,internclass,GetWndFunc,(internclass->*GetWndCnt)()) != -1;
			if (found) {
				LOGPRIO(Logger::Priority::SPAM) << m_name << " button was clicked!\n";
				m_buttonPressed = true;
			}
		}
		else if(targetwnd == m_edit && notification == EN_UPDATE) {
			if(m_ignoreNext) {
				m_ignoreNext = false;
			}
			else {
				//if our edit has been modified, take care of that
				LimitEditInt(m_edit,0,255);
				int btnCnt = (internclass->*GetWndCnt)();
				int newSlot = GetEditNumber(m_edit);
				if(newSlot < btnCnt && IsWindowEnabled((internclass->*GetWndFunc)(newSlot))) {
					//check button to make it look in sync
					BYTE currHair = (internclass->*GetSlot)();
					SendMessage((internclass->*GetWndFunc)(currHair),BM_SETCHECK,BST_UNCHECKED,0);
					SendMessage((internclass->*GetWndFunc)(newSlot),BM_SETCHECK,BST_CHECKED,0);
				}
				LOGPRIO(Logger::Priority::SPAM) << m_name << " edit got changed to " << newSlot << "\n";
				(internclass->*SetChangeFlags)();
				m_editChanged = true;
			}
		}
	}

	/*
	 * Has to be called on the GetSelectorIndex of its associated data
	 */
	int GetSelectorIndex(T* internclass, int guiChosen) {
		if (m_buttonPressed) {
			m_buttonPressed = false;
			LOGPRIO(Logger::Priority::SPAM) << "selecting " << m_name << " index after button click to " << guiChosen << "\n";
			m_ignoreNext = true;
			SetEditNumber(m_edit,guiChosen);
			m_loggedSlot = guiChosen;
			return -1;
		}
		else if (m_editChanged) {
			m_editChanged = false;
			int ret = GetEditNumber(m_edit);
			LOGPRIO(Logger::Priority::SPAM) << "selecting " << m_name << " index from edit to " << ret << "\n";
			if (ret < 0 || ret > 255) ret = -1;
			m_loggedSlot = ret == -1 ? guiChosen : ret;
			return ret;
		}
		else if((internclass->*GetSlot)() == m_loggedSlot) {
			//if this is just polling again and nothing has changed meanwhile, return old value
			return m_loggedSlot;
		}
		else {
			//selection has been changed from unknown sources (maybe random button?)
			m_ignoreNext = true;
			SetEditNumber(m_edit,guiChosen); //refresh edit field
			return -1;
		}
		
	}

	/*
	 * Has to be called on InitTab before and after init to make it able to load things
	 */
	void InitTab(T* internclass, bool before) {
		if (before) {
			//before the call, we take note of slot used
			m_loggedSlot = (internclass->*GetSlot)();
			LOGPRIO(Logger::Priority::SPAM) << m_name << " loaded and initialized to " << m_loggedSlot << "\n";
		}
		else {
			//after the call, we look at the slot, and correct them if they were changed
			BYTE newslot = (internclass->*GetSlot)();
			if(m_loggedSlot != newslot) {
				SetEditNumber(m_edit,m_loggedSlot);
				(internclass->*SetSlot)(m_loggedSlot);
				LOGPRIO(Logger::Priority::SPAM) << m_name << " loaded and corrected from " << newslot << "\n";
			}
		}
	}

	void IncChosenSlot(int diff) {
		int ret = GetEditNumber(m_edit);
		ret += diff;
		SetEditNumber(m_edit,ret);
	}

	void SetChosenSlot(int slot) {
		SetEditNumber(m_edit,slot);
	}
};
