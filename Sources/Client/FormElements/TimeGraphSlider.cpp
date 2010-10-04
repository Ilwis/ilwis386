#include "Client\Headers\formelementspch.h"
#include "Client\ilwis.h"
#include "Client\FormElements\TimeGraphSlider.h"


BEGIN_MESSAGE_MAP(TimeGraph, CStatic)
	ON_WM_LBUTTONUP()
	ON_NOTIFY(TTN_NEEDTEXTW, 0, OnToolTipNotify)
	ON_NOTIFY(TTN_NEEDTEXTA, 0, OnToolTipNotify)
END_MESSAGE_MAP()

TimeGraph::TimeGraph(TimeGraphSlider *f, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID) : BaseZapp(f) 
{
	fldGraph = f;
	if (!CStatic::Create(0,dwStyle | SS_OWNERDRAW | SS_NOTIFY, rect, pParentWnd, nID))
		throw ErrorObject(TR("Could not create Time graph"));
	ModifyStyleEx(0, WS_EX_NOPARENTNOTIFY, SWP_FRAMECHANGED);
	timePoint = CPoint(100000, 100000);
	EnableTrackingToolTips();
}

int TimeGraph::OnToolHitTest(CPoint point, TOOLINFO *pTI) const
{
	pTI->hwnd = m_hWnd;
	pTI->uFlags = 0; // we need to differ tools by ID not window handle
	pTI->lpszText = LPSTR_TEXTCALLBACK; // tell tooltips to send TTN_NEEDTEXT
	pTI->uId = GetDlgCtrlID();
	CRect rct;
	GetClientRect(rct);
	pTI->rect = rct;
	return GetDlgCtrlID();
}

void TimeGraph::OnToolTipNotify(NMHDR *pNMHDR, LRESULT *pResult)
{
	char m_szTipBufA[MAX_PATH];
	*pResult = 1;	
	// need to handle both ANSI and UNICODE versions of the message
	TOOLTIPTEXTA* pTTTA = (TOOLTIPTEXTA*)pNMHDR;

	CString csToolText;

	UINT uiID = pNMHDR->idFrom;

	int iID = GetDlgCtrlID();

	//return TRUE;
}

void TimeGraph::DrawItem(LPDRAWITEMSTRUCT lpDIS) {
	bool useDefault = false;
	Column col ;
	if ( !fldGraph->sourceTable.fValid() || fldGraph->sourceColumn == "")
		useDefault = true;;
	if (!useDefault) {
		col = fldGraph->sourceTable->col(fldGraph->sourceColumn);
		if ( !col.fValid())
			useDefault = true;
	}
	RangeReal rr = !useDefault ? col->rrMinMax() : RangeReal(0.0, 1.0);
	CRect crct;
	GetClientRect(crct);
	Color c(GetSysColor(COLOR_3DFACE));
	CBrush brushBk(c);
	CPen pen(PS_SOLID,1,c);
	HGDIOBJ oldBrush = SelectObject(lpDIS->hDC, brushBk);
	HGDIOBJ oldPen = SelectObject(lpDIS->hDC, pen);
	::Rectangle(lpDIS->hDC, crct.left, crct.top, crct.right, crct.bottom);
	SelectObject(lpDIS->hDC, oldBrush);
	SelectObject(lpDIS->hDC, oldPen);

	CRect rct = CRect(crct.left, crct.top,crct.right, crct.bottom-20 );
	Color bkColor = GetBkColor(lpDIS->hDC);
	CBrush bbrushBk(RGB(0, 50, 100));
	SelectObject(lpDIS->hDC, bbrushBk);
	::Rectangle(lpDIS->hDC, rct.left,rct.top, rct.right, rct.bottom);
	SelectObject(lpDIS->hDC, oldBrush);

	CFont* fnt = IlwWinApp()->GetFont(IlwisWinApp::sfFORM);
	HGDIOBJ fntOld = SelectObject(lpDIS->hDC, fnt);
	 String s("%d", fldGraph->recordRange.iLo());
	 ::TextOut(lpDIS->hDC,crct.left, crct.bottom - 16,s.scVal(),s.size());
	double yscale = rct.Height() / rr.rWidth();
	double y0 = rr.rWidth() * yscale;
	double xscale = (double)rct.Width() / fldGraph->recordRange.iWidth();
	CPoint *pts = new CPoint[fldGraph->recordRange.iWidth() + 3];
	double rx = 0;
	for(int i = fldGraph->recordRange.iLo(); i <= fldGraph->recordRange.iHi(); ++i) {
		double v = !useDefault ?  col->rValue(i) : (double)i / fldGraph->recordRange.iWidth();
		int y = y0 - ( v - rr.rLo()) * yscale;
		pts[!useDefault ? i-1 : i] = CPoint(rx,y);
		rx += xscale;
	}
	s = String("%d", fldGraph->recordRange.iHi());
	::TextOut(lpDIS->hDC,crct.right-14, crct.bottom - 16, s.scVal(),s.size());
	pts[fldGraph->recordRange.iWidth() + 1] = CPoint(rct.Width(), rct.Height());
	pts[fldGraph->recordRange.iWidth() + 2] = CPoint(0, rct.Height());


	CPen bpen(PS_SOLID, 1, RGB(0, 0, 200));
	SelectObject(lpDIS->hDC, bpen);

	// and a solid red brush
	CBrush brush(RGB(50, 150, 50));
	oldBrush = SelectObject(lpDIS->hDC, brush);
	::Polygon(lpDIS->hDC,pts,fldGraph->recordRange.iWidth() + 3);

	if ( timePoint.x != 100000) {
		CPen redPen(PS_SOLID,2, RGB(255,0,0));
		SelectObject(lpDIS->hDC, redPen);
		::MoveToEx(lpDIS->hDC, timePoint.x, rct.bottom,0);
		::LineTo(lpDIS->hDC, timePoint.x, rct.top);
		int index = (int)(0.5 + (double)timePoint.x / xscale);
		s = String("%d", index);
		::TextOut(lpDIS->hDC,timePoint.x -4, crct.bottom - 16, s.scVal(),s.size());
	}

	delete [] pts;
	SelectObject(lpDIS->hDC,oldPen);
	SelectObject(lpDIS->hDC, oldBrush);
	SelectObject(lpDIS->hDC, fntOld);
}

void TimeGraph::OnLButtonUp(UINT nFlags, CPoint point) {	
	timePoint = point;
	CWnd *wnd =  GetParent();
	if ( wnd) {
		bool useDefault = false;
		Column col;
		if ( !fldGraph->sourceTable.fValid() || fldGraph->sourceColumn == "")
				useDefault = true;
		if ( !useDefault) {
			Column col = fldGraph->sourceTable->col(fldGraph->sourceColumn);
			if ( !col.fValid())
				useDefault = true;
		}
		RangeReal rr = !useDefault ? col->rrMinMax() : RangeReal(0.0, 1.0);
		CRect rct;
		GetClientRect(rct);
		double xscale = rct.Width() / fldGraph->recordRange.iWidth();
		int index = timePoint.x / xscale;
		wnd->PostMessageA(ID_TIME_TICK, index, FALSE);
	}
	Invalidate();
	fProcess(MouseLBUpEvent(nFlags,point)); 

}
void TimeGraph::setIndex(int index) {
	CRect rct;
	GetClientRect(rct);
	double xscale = (double)rct.Width() / fldGraph->recordRange.iWidth();
	timePoint = CPoint(xscale * index, (rct.bottom + rct.top)/2.0); // y doesnt matter but anyway
	Invalidate();

}

void TimeGraph::PreSubclassWindow() 
{
	EnableToolTips();
	
	CStatic::PreSubclassWindow();
}
//----------------------------------------------------
TimeGraphSlider::TimeGraphSlider(FormEntry* par, RangeInt defaultRange) :
FormEntry(par,0,true),
timegraph(0)
{
	psn->iMinWidth = psn->iWidth = 250;
	psn->iMinHeight = psn->iHeight = 60;
	SetIndependentPos();
	recordRange = defaultRange;
}

void TimeGraphSlider::create()
{
	zPoint pntFld = zPoint(psn->iPosX,psn->iPosY);
	zDimension dimFld = zDimension(psn->iWidth,psn->iMinHeight);
	timegraph = new TimeGraph(this, WS_VISIBLE | WS_CHILD ,
		CRect(pntFld, dimFld) , frm()->wnd() , Id());
	timegraph->SetFont(frm()->fnt);
	CreateChildren();
}

void TimeGraphSlider::setSourceTable(const Table& tbl) {
	sourceTable = tbl;
	recordRange = RangeInt(1,tbl->iRecs());
	if ( timegraph)
		timegraph->Invalidate();
}

void TimeGraphSlider::setSourceColumn(const String& sName) {
	sourceColumn = sName;
	if ( timegraph)
		timegraph->Invalidate();
}

void TimeGraphSlider::setRecordRange(const RangeInt& rng) {
	recordRange = rng;
	if ( timegraph)
		timegraph->Invalidate();
}

void TimeGraphSlider::setIndex(int index) {
	if ( timegraph)
		timegraph->setIndex(index);
}