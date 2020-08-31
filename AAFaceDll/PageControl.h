#pragma once
#include <Windows.h>

class PageControl
{
public:
	HWND m_stPageIndicator;
	HWND m_btnPrev;
	HWND m_btnNext;

	HWND m_parent;
	POINT m_pos;

	PageControl() = default;
	PageControl(POINT loc, HWND parent, int nPages);

	enum Action {
		None,
		PageChange
	};
	Action OnWmCmd(HWND hwndDlg, DWORD ctrlId, DWORD notification);

	inline int CurrPage() { return m_currPage; }
	void SetPage(int pageNr);
	void MoveTo(POINT newLoc);
private:
	int m_currPage;
	int m_nPages;

	
	bool NextPage();
	bool PrevPage();
	void UpdatePageText();
};

