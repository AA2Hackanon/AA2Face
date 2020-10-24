#include "PageControl.h"
#include "ExternConstants.h"

#include <stdio.h>
#include "GenUtils.h"

PageControl::PageControl(POINT loc, HWND parent, int nPages)
{
	m_pos = loc;
	m_parent = parent;
	m_currPage = 0;
	m_nPages = nPages;
	m_btnPrev = CreateWindowExW(0,
		L"BUTTON", L"<", WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
		0,0,0,0, parent, 0, (HINSTANCE)g_AA2Base, NULL);
	m_stPageIndicator = CreateWindowExW(0,
		L"STATIC", L"", WS_VISIBLE | WS_CHILD,
		0, 0, 0, 0, parent, 0, (HINSTANCE)g_AA2Base, NULL);
	m_btnNext = CreateWindowExW(0,
		L"BUTTON", L">", WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
		0, 0, 0, 0, parent, 0, (HINSTANCE)g_AA2Base, NULL);
	SendMessage(m_btnPrev, WM_SETFONT, (WPARAM)g_sysFont, TRUE);
	SendMessage(m_stPageIndicator, WM_SETFONT, (WPARAM)g_sysFont, TRUE);
	SendMessage(m_btnNext, WM_SETFONT, (WPARAM)g_sysFont, TRUE);
	MoveTo(loc);
	UpdatePageText();
}

PageControl::Action PageControl::OnWmCmd(HWND hwndDlg, DWORD ctrlId, DWORD notification)
{
	if (notification == BN_CLICKED) {
		if (hwndDlg == m_btnNext) {
			if (NextPage()) {
				return Action::PageChange;
			}
		}
		else if (hwndDlg == m_btnPrev) {
			if (PrevPage()) {
				return Action::PageChange;
			}
		}
	}
	return Action::None;
}

void PageControl::MoveTo(POINT newLoc)
{
	m_pos = newLoc;
	MoveWindow(m_btnPrev, newLoc.x, newLoc.y, 17, 17, TRUE);
	MoveWindow(m_stPageIndicator, newLoc.x + 20, newLoc.y, 50, 20, TRUE);
	MoveWindow(m_btnNext, newLoc.x + 40, newLoc.y, 17, 17, TRUE);
	EnableWindow(m_btnNext, m_currPage + 1 < m_nPages);
	EnableWindow(m_btnPrev, m_currPage > 0);
}

void PageControl::SetPage(int pageNr)
{
	if (pageNr < 0) pageNr = 0;
	if (pageNr >= m_nPages) pageNr = m_nPages - 1;
	if (pageNr == m_currPage) return;

	m_currPage = pageNr;
	UpdatePageText();

	EnableWindow(m_btnNext, m_currPage + 1 < m_nPages);
	EnableWindow(m_btnPrev, m_currPage > 0);
}

bool PageControl::NextPage()
{
	if (m_currPage + 1 >= m_nPages) return false;
	m_currPage++;
	UpdatePageText();

	if (m_currPage + 1 >= m_nPages) {
		EnableWindow(m_btnNext, FALSE);
	}
	if (m_currPage > 0) {
		EnableWindow(m_btnPrev, TRUE);
	}

	return true;
}

bool PageControl::PrevPage()
{
	if (m_currPage <= 0) return false;
	m_currPage--;
	UpdatePageText();

	if (m_currPage + 1 < m_nPages) {
		EnableWindow(m_btnNext, TRUE);
	}
	if (m_currPage <= 0) {
		EnableWindow(m_btnPrev, FALSE);
	}

	return true;
}

void PageControl::UpdatePageText() {
	wchar_t text[256];
	_swprintf_c(text, 256, L"%d/%d", m_currPage + 1, m_nPages);
	SetWindowTextW(m_stPageIndicator, text);
}

void PageControl::SetPageCount(int newNPages) {
	m_nPages = newNPages;	
	if (m_currPage >= newNPages) {
		SetPage(newNPages - 1);
	}
	else {
		UpdatePageText();
		EnableWindow(m_btnNext, m_currPage + 1 < m_nPages);
		EnableWindow(m_btnPrev, m_currPage > 0);
	}
}