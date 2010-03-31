#include "Client\Headers\formelementspch.h"
#include "Client\ilwis.h"

BEGIN_MESSAGE_MAP(OwnerDrawListBox, CListBox)
	ON_CONTROL_REFLECT(LBN_DBLCLK, OnLBDblClk)
	ON_CONTROL_REFLECT(LBN_SELCHANGE, OnSelChange)
	ON_WM_KEYDOWN()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_CONTEXTMENU()
	ON_WM_KILLFOCUS()
	ON_WM_SETFOCUS()
END_MESSAGE_MAP()

OwnerDrawListBox::OwnerDrawListBox(FormEntry *f, DWORD iStyle, const CRect& rct, CWnd *parent, int id) :
	CListBox(),
	BaseZapp(f),
	idw(false),
	m_fShowPopup(false)
{
	Create(LBS_NOTIFY | iStyle | LBS_DISABLENOSCROLL | WS_BORDER | WS_TABSTOP, rct, parent, id);  
}

void OwnerDrawListBox::SetShowPopup(bool fShowPopup)
{
	m_fShowPopup = fShowPopup;
}

void OwnerDrawListBox::DrawItem(DRAWITEMSTRUCT* dis)
{
	if (!fProcess(DrawItemEvent(dis->CtlID, dis)))
	{
		CString s;
		int id = dis->itemID;
		if (id >= 0)
		{
			GetText(id, s);
			BOOL fSel = GetSel(id) > 0;
			BOOL fUpNgbSel = FALSE;
			CWnd * wnd = GetFocus();
			bool fHasFocus = wnd != NULL ? wnd->m_hWnd == m_hWnd : false;
			if (fSel && id > 0)
				fUpNgbSel = GetSel(id - 1) > 0;
			idw.DrawItem(dis, String(s), fSel, fHasFocus, fUpNgbSel);
		}
	}
}

void OwnerDrawListBox::MeasureItem( LPMEASUREITEMSTRUCT lpMeasureItemStruct )
{
	zDisplay canvas(this);
	// const cast allowed here because it is only needed for read access
	canvas.pushFont(const_cast<CFont *>(FormBase::fntDflt));
	zFontInfo info = zFontInfo(&canvas);
	lpMeasureItemStruct->itemHeight = info.iPixHeight();
	canvas.popFont();
}

void OwnerDrawListBox::OnMouseMove( UINT nFlags, CPoint point )
{
	CListBox::OnMouseMove(nFlags, point);

	fProcess(MouseMoveEvent(nFlags, point));
}

void OwnerDrawListBox::OnLButtonDown( UINT nFlags, CPoint point )
{
	CListBox::OnLButtonDown(nFlags, point);
	OnSelChange();
	fProcess(MouseLBDownEvent(nFlags, point));
}

void OwnerDrawListBox::OnLButtonUp( UINT nFlags, CPoint point )
{
	CListBox::OnLButtonUp(nFlags, point);
	fProcess(MouseLBUpEvent(nFlags, point));
}

void OwnerDrawListBox::OnLBDblClk()
{
	fProcess(LBDoubleClickEvent(GetDlgCtrlID(), m_hWnd));
}

void OwnerDrawListBox::OnSelChange()
{
	fProcess(NotificationEvent(GetDlgCtrlID(), LBN_SELCHANGE, (LPARAM)m_hWnd));
}   

void OwnerDrawListBox::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	CListBox::OnKeyDown(nChar, nRepCnt, nFlags);
	fProcess(Event(WM_KEYDOWN, nChar, MAKELONG(nRepCnt, nFlags)));
}

void OwnerDrawListBox::OnSetFocus(CWnd *old)
{
	CListBox::OnSetFocus(old);
	int iIndex = GetCurSel();
	if (iIndex < 0 ) return;

	CRect rct;

	if ( GetItemRect(iIndex, rct) != LB_ERR)
		InvalidateRect(rct);
}

void OwnerDrawListBox::OnKillFocus(CWnd *old)
{
	CListBox::OnKillFocus(old);
	int iIndex = GetCurSel();
	if (iIndex < 0 ) return;

	CRect rct;

	if ( GetItemRect(iIndex, rct) != LB_ERR)
		InvalidateRect(rct);
}

void OwnerDrawListBox::OnContextMenu(CWnd* pWnd, CPoint point) 
{
	if (m_fShowPopup)
	{
		CPoint pt = point;
		ScreenToClient(&pt);

		int iOutside;
		// ItemFromPoint() works for all windows versions with the 
		// macro WINVER >= 0x400 (see in winctrl1.cpp), that is: Win95 and higher
		// although MSDN says win95 only
		int iIndex = ItemFromPoint(pt, iOutside);
		if (iOutside == TRUE)
			return;

		SetCurSel(iIndex); // move the selection to iIndex in single select listbox
		CString s;
		GetText(iIndex, s);

		// Special code: for MapList bands the string is:
		// "display_name|filename". The filename is extracted here
		String sFileName(s);
		int iPos = sFileName.find('|');
		if (iPos != string::npos)
			sFileName = sFileName.substr(iPos + 1);

		FileName fn(sFileName);
		if (fn.fValid())
			IlwWinApp()->ShowPopupMenu(pWnd, point, fn);
	}
	else
		CListBox::Default();
}

//--[ OwnerDrawCB ]--------------------------------------------------------------------------------------
BEGIN_MESSAGE_MAP(OwnerDrawCB, CComboBox)
  ON_CONTROL_REFLECT(CBN_DBLCLK, OnCBDblClk)
  ON_CONTROL_REFLECT(CBN_CLOSEUP, OnCBCloseUp)
  ON_CONTROL_REFLECT(CBN_SELCHANGE, OnSelChange)
  ON_CONTROL_REFLECT(CBN_SELENDOK, OnSelAndOK)
END_MESSAGE_MAP()

OwnerDrawCB::OwnerDrawCB(FormEntry *f, DWORD iStyle, const CRect& rct, CWnd *parent, int id) :
  CComboBox(),
  BaseZapp(f),
  idw(false)
{
  Create(CBS_OWNERDRAWFIXED | iStyle, rct, parent, id); 
}

void OwnerDrawCB::DrawItem(DRAWITEMSTRUCT* dis)
{
  if (!fProcess(DrawItemEvent(dis->CtlID, dis)))
  {
    int id = dis->itemID;
    CString s;
    if (id < 0) return;
    
    GetLBText(id, s);
    bool fSel = GetCurSel() == id;
    DWORD tmp = GetItemData(id);
		string s1 = s;
		String str(s1);
    idw.DrawItem(dis, str, fSel, true, false);
  }
}

void OwnerDrawCB::MeasureItem( LPMEASUREITEMSTRUCT lpMeasureItemStruct )
{
  zDisplay canvas(this);
	// const cast allowed here because it is only needed for read access
  canvas.pushFont(const_cast<CFont *>(FormBase::fntDflt));
  zFontInfo info = zFontInfo(&canvas);
  lpMeasureItemStruct->itemHeight = info.iPixHeight();
  canvas.popFont();
}

void OwnerDrawCB::OnCBDblClk()
{
  fProcess(CBDoubleClickEvent(GetDlgCtrlID(), m_hWnd));
}

void OwnerDrawCB::OnCBCloseUp()
{
  fProcess(NotificationEvent(GetDlgCtrlID(), CBN_CLOSEUP, (LPARAM)m_hWnd));
}

void OwnerDrawCB::OnSelChange()
{
  fProcess(NotificationEvent(GetDlgCtrlID(), CBN_SELCHANGE, (LPARAM)m_hWnd));
}

void OwnerDrawCB::OnSelAndOK()
{
	int iIndex = GetCurSel();

}

