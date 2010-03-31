#include "Client\Headers\formelementspch.h"
#include "CalendarListCtrl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#include "math.h"
#include "memdc.h"

/////////////////////////////////////////////////////////////////////////////
// CalendarCtrlListCtrl


CalendarCtrlListCtrl::CalendarCtrlListCtrl()
{
	SetCalendar(NULL);
	SetItemsPerPage(7);
	SetMiddleMonthYear(COleDateTime::GetCurrentTime().GetMonth(), COleDateTime::GetCurrentTime().GetYear());
	SetBackColor((COLORREF)::GetSysColor(COLOR_WINDOW));
	SetTextColor((COLORREF)::GetSysColor(COLOR_BTNTEXT));

	SetBackColor(RGB(255,255,255));
	SetTextColor(RGB(0,0,0));

	m_iSelMonth = COleDateTime::GetCurrentTime().GetMonth();
	m_iSelYear = COleDateTime::GetCurrentTime().GetYear();

	m_iUpFactor = 0;
	m_iDownFactor = 0;
}

CalendarCtrlListCtrl::~CalendarCtrlListCtrl()
{
}


BEGIN_MESSAGE_MAP(CalendarCtrlListCtrl, CWnd)
	//{{AFX_MSG_MAP(CalendarCtrlListCtrl)
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


// utility function for converting long to CString 
CString CalendarCtrlListCtrl::CStr(long lValue)
{
	char szValue[20];

	_ltoa(lValue, szValue, 10);

	return szValue;
}

/////////////////////////////////////////////////////////////////////////////
// CalendarCtrlListCtrl message handlers

BOOL CalendarCtrlListCtrl::OnEraseBkgnd(CDC* pDC) 
{
	return FALSE;
}

void CalendarCtrlListCtrl::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	CMemDC memdc(&dc);
	CRect rectClient;

	// calendar must be set for this to work correctly
	ASSERT(m_pwndCalendar);

	// if needed, create font
	if (!m_FontInfo.m_pFont)
		m_FontInfo.CreateFont(&memdc);

	// determine client area
	GetClientRect(rectClient);

	// first, determine half of the items on the page
	// we use this to reverse calculate the top item
	int iHalfCount = m_iItemsPerPage / 2;
	if ((iHalfCount * 2) > m_iItemsPerPage)
		iHalfCount--;

	int iMonth = m_iMiddleMonth;
	int iYear = m_iMiddleYear;

	for (int iItem = 0; iItem < iHalfCount; iItem++)
	{
		iMonth--;
		if (iMonth < 1)
		{
			iMonth = 12;
			iYear--;
		}
	}	
	
	// prepare DC
	//CFont* pOldFont = memdc.SelectObject(m_FontInfo.m_pFont);
	CFont* pOldFont = memdc.SelectObject(m_pwndCalendar->GetDaysFont().m_pFont);
	int iPosY = 0;
	int iPosX = 10;
	
	// fill background
	memdc.FillSolidRect(0, 0, rectClient.Width(), rectClient.Height(), m_cBackColor);
	memdc.SetBkMode(TRANSPARENT);

	for (int iItem = 0; iItem < m_iItemsPerPage; iItem++)
	{
		CString strDisplayText;
		CRect rectItem;

		if (m_pwndCalendar)
			strDisplayText = m_pwndCalendar->GetMonthName(iMonth);
		strDisplayText += " ";
		strDisplayText += CStr(iYear);

		CSize szItem = memdc.GetTextExtent("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789");

		rectItem.SetRect(iPosX, iPosY, iPosX + szItem.cx, iPosY + szItem.cy);

		if (IsSelected(iItem, rectItem))
		{
			// this could be done better, but it is a very simple control so
			// I am just going to set the selected month/year right here in
			// the draw code
			m_iSelMonth = iMonth;
			m_iSelYear = iYear;

			memdc.SetTextColor(m_cBackColor);
			memdc.SetBkColor(m_cTextColor);

			memdc.FillSolidRect(0, iPosY, rectClient.Width(), szItem.cy, memdc.GetBkColor());
		}
		else
		{
			memdc.SetTextColor(m_cTextColor);
			memdc.SetBkColor(m_cBackColor);
		}

		memdc.DrawText(strDisplayText, rectItem, DT_SINGLELINE | DT_LEFT | DT_TOP);
				
		iMonth++;
		if (iMonth > 12)
		{
			iMonth = 1;
			iYear++;
		}

		iPosY += szItem.cy;
	}

	memdc.SelectObject(pOldFont);
}

void CalendarCtrlListCtrl::ForwardMessage(MSG *pMSG)
{
	if (pMSG->message == WM_MOUSEMOVE || pMSG->message == WM_TIMER)
	{
		CRect rectClient;

		// determine cursor position
		DWORD dwPos = ::GetMessagePos();
		POINTS ptsPos = MAKEPOINTS(dwPos);
		CPoint ptPos;
		ptPos.x = ptsPos.x;
		ptPos.y = ptsPos.y;

		//ScreenToClient(&ptPos);

		GetClientRect(rectClient);
		ClientToScreen(rectClient);

		if (ptPos.y < rectClient.top)
		{
			int iDiff = rectClient.top - ptPos.y;

			if (iDiff <= 15)
				ScrollUp(1);
			else
				ScrollUp(2);
		}
		else if (ptPos.y > rectClient.bottom)
		{
			int iDiff = ptPos.y - rectClient.bottom;

			if (iDiff <= 15)
				ScrollDown(1);
			else
				ScrollDown(2);
		}
		else
		{
			m_iUpFactor = 0;
			m_iDownFactor = 0;
		}

		RedrawWindow();
	}
}

void CalendarCtrlListCtrl::SetMiddleMonthYear(int iMonth, int iYear)
{
	ASSERT(iYear >= 100);
	ASSERT(iYear <= 9999);
	ASSERT(iMonth >= 1);
	ASSERT(iMonth <= 12);

	if (iMonth >= 1 && iMonth <= 12)
		m_iMiddleMonth = iMonth;

	if (iYear >= 100 && iYear <= 9999)
		m_iMiddleYear = iYear;
}

void CalendarCtrlListCtrl::SetItemsPerPage(int iValue)
{
	// we require at least 3 items per page and the count
	// must be odd (same number of items on either side
	// of middle)
	ASSERT(iValue >= 3);
	ASSERT(fmod((double)iValue, (double)2) != 0);

	if (iValue >= 3 && fmod((double)iValue, (double)2) != 0)
		m_iItemsPerPage = iValue;
}

void CalendarCtrlListCtrl::SetFont(LPCTSTR lpszFont, int iSize, BOOL bBold, BOOL bItalic, BOOL bUnderline, COLORREF cColor)
{
	ASSERT(iSize > 0);
	ASSERT(iSize <= 72);
	ASSERT(lpszFont);
	ASSERT(AfxIsValidString(lpszFont));

	if (iSize > 0 && iSize <= 72 && lpszFont && AfxIsValidString(lpszFont))
	{
		m_FontInfo.m_bBold = bBold;
		m_FontInfo.m_bItalic = bItalic;
		m_FontInfo.m_bUnderline = bUnderline;
		m_FontInfo.m_iFontSize = iSize;
		m_FontInfo.m_strFontName = lpszFont;
		m_FontInfo.m_cColor = cColor;

		// make sure font object gets recreated
		if (m_FontInfo.m_pFont)
			delete m_FontInfo.m_pFont;
		m_FontInfo.m_pFont = NULL;
	}
}

void CalendarCtrlListCtrl::SetCalendar(CalendarCtrl* pWnd) 
{
	ASSERT(!pWnd || ::IsWindow(pWnd->GetSafeHwnd()));

	if (!pWnd || ::IsWindow(pWnd->GetSafeHwnd()))
		m_pwndCalendar = pWnd;
}

BOOL CalendarCtrlListCtrl::IsSelected(int iX, CRect rectItem)
{
	BOOL bReturn = FALSE;

	CRect rectClient;

	// determine cursor position
	DWORD dwPos = ::GetMessagePos();
	POINTS ptsPos = MAKEPOINTS(dwPos);
	CPoint ptPos;
	ptPos.x = ptsPos.x;
	ptPos.y = ptsPos.y;

	ScreenToClient(&ptPos);

	GetClientRect(rectClient);

	if (ptPos.y < 0 && iX == 0)
		bReturn = TRUE;
	else if (ptPos.y > rectClient.Height() && iX == (m_iItemsPerPage-1))
		bReturn = TRUE;
	else if (ptPos.y >= rectItem.top && ptPos.y <= rectItem.bottom-1)
		bReturn = TRUE;

	return bReturn;
}

void CalendarCtrlListCtrl::AutoConfigure()
{
	ASSERT(::IsWindow(GetSafeHwnd()));

	if (::IsWindow(GetSafeHwnd()))
	{
		// determine cursor position
		CSize szMaxItem(0,0);
		DWORD dwPos = ::GetMessagePos();
		POINTS ptsPos = MAKEPOINTS(dwPos);
		CPoint ptPos;
		ptPos.x = ptsPos.x;
		ptPos.y = ptsPos.y;

		CClientDC dc(this);

		// calendar must be set for this to work correctly
		ASSERT(m_pwndCalendar);

		// if needed, create font
		if (!m_FontInfo.m_pFont)
			m_FontInfo.CreateFont(&dc);

		//CFont* pOldFont = dc.SelectObject(m_FontInfo.m_pFont);
		CFont* pOldFont = dc.SelectObject(m_pwndCalendar->GetDaysFont().m_pFont);

		szMaxItem.cy = dc.GetTextExtent("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789").cy;

		for (int iMonth = 1; iMonth <= 12; iMonth++)
		{
			CString strText;

			if (m_pwndCalendar)
				strText = m_pwndCalendar->GetMonthName(iMonth);
			strText += " 0000";

			int iWidth = dc.GetTextExtent(strText).cx;

			if (iWidth > szMaxItem.cx)
				szMaxItem.cx = iWidth;
		}

		dc.SelectObject(pOldFont);

		// calculate rectangle
		CSize szCalendar((szMaxItem.cx + 20), ((szMaxItem.cy * m_iItemsPerPage) + 3));

		int iLeftX = ptPos.x - (szCalendar.cx / 2);
		int iTopY = ptPos.y - (szCalendar.cy / 2);
		int iRightX = ptPos.x + (szCalendar.cx / 2);
		int iBottomY = ptPos.y + (szCalendar.cy / 2);

		SetWindowPos(NULL, iLeftX, iTopY, (iRightX-iLeftX), (iBottomY-iTopY), 0);
		RedrawWindow();
	}
}

void CalendarCtrlListCtrl::ScrollUp(int iCount)
{
	m_iDownFactor = 0;
	
	m_iUpFactor += iCount;

	if (m_iUpFactor >= 10)
	{
		for (int iX = 0; iX < iCount; iX++)
		{
			if (m_iMiddleYear == 100)
				break;

			m_iMiddleMonth--;

			if (m_iMiddleMonth < 1)
			{
				m_iMiddleMonth = 12;
				m_iMiddleYear--;
			}
		}
		
		m_iUpFactor = 0;
	}
}

void CalendarCtrlListCtrl::ScrollDown(int iCount)
{
	m_iUpFactor = 0;

	m_iDownFactor += iCount;

	if (m_iDownFactor >= 10)
	{
		for (int iX = 0; iX < iCount; iX++)
		{
			if (m_iMiddleYear == 9999)
				break;

			m_iMiddleMonth++;

			if (m_iMiddleMonth > 12)
			{
				m_iMiddleMonth = 1;
				m_iMiddleYear++;
			}
		}

		m_iDownFactor = 0;
	}
}

