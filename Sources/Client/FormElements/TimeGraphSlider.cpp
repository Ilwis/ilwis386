#include "Client\Headers\formelementspch.h"
#include "Client\ilwis.h"
#include "Client\FormElements\TimeGraphSlider.h"


BEGIN_MESSAGE_MAP(TimeGraph, CStatic)
	ON_WM_LBUTTONUP()
	ON_NOTIFY(TTN_NEEDTEXTW, 0, OnToolTipNotify)
	ON_NOTIFY(TTN_NEEDTEXTA, 0, OnToolTipNotify)
END_MESSAGE_MAP()

TimeGraph::TimeGraph(TimeGraphSlider *f, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID) : BaseZapp(f),threshold(rUNDEF), toBeNotified(0), isAbove(true)
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
	//char m_szTipBufA[MAX_PATH];
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
	CBrush bbrushBk(GetSysColor(COLOR_3DFACE));
	SelectObject(lpDIS->hDC, bbrushBk);
	::Rectangle(lpDIS->hDC, rct.left,rct.top, rct.right, rct.bottom);
	SelectObject(lpDIS->hDC, oldBrush);

	//CFont* fnt = IlwWinApp()->GetFont(IlwisWinApp::sfFORM);

	CFont* fnt = new CFont();
	BOOL vvv = fnt->CreatePointFont(80,"Arial");
	CDC *dc = CDC::FromHandle(lpDIS->hDC);

	HGDIOBJ fntOld = dc->SelectObject(fnt);
	String s;
	if ( fldGraph->interval.fValid() == false)
		s = String("%d", fldGraph->recordRange.iLo());
	else {
		s = fldGraph->interval.getBegin().toString(true, ILWIS::Time::mDATE);
	}
	 ::TextOut(lpDIS->hDC,crct.left, crct.bottom - 16,s.c_str(),s.size());
	double yscale = rct.Height() / rr.rWidth();
	double y0 = rr.rWidth() * yscale;
	double xscale = (double)rct.Width() / fldGraph->recordRange.iWidth();
	CPoint *pts = new CPoint[fldGraph->recordRange.iWidth() + 3];
	marked.clear();
	double rx = 0;
	for(int i = fldGraph->recordRange.iLo(); i <= fldGraph->recordRange.iHi(); ++i) {
		double v = !useDefault ?  col->rValue(i) : (double)i / fldGraph->recordRange.iWidth();
		if ( threshold != rUNDEF) {
			if ( toBeNotified ) {
				marked.push_back(isAbove ? threshold < v : threshold >= v);
			} 
		}
		int y = y0 - ( v - rr.rLo()) * yscale;
		pts[!useDefault ? i-1 : i] = CPoint(rx,y);
		rx += xscale;
	}
	if ( fldGraph->interval.fValid() == false)
		s = String("%d", fldGraph->recordRange.iHi());
	else {
		s = fldGraph->interval.getEnd().toString(true, ILWIS::Time::mDATE);
	}
	CSize szTxt = dc->GetTextExtent(s.c_str());
	::TextOut(lpDIS->hDC,crct.right - szTxt.cx, crct.bottom - 16, s.c_str(),s.size());
	//pts[fldGraph->recordRange.iWidth() + 1] = CPoint(rct.Width(), rct.Height());
	//pts[fldGraph->recordRange.iWidth() + 2] = CPoint(0, rct.Height());



	CPen bpen(PS_SOLID, 1, RGB(0, 0, 200));
	SelectObject(lpDIS->hDC, bpen);

	// and a solid red brush
	CBrush brush(RGB(0, 0, 0));
	oldBrush = SelectObject(lpDIS->hDC, brush);
	::Polyline(lpDIS->hDC,pts,fldGraph->recordRange.iWidth());
	if ( threshold != rUNDEF) {
		int y = y0 - ( threshold - rr.rLo()) * yscale;
		CPen redPen(PS_SOLID,2, RGB(0,0,255));
		dc->SelectObject(redPen);
		dc->MoveTo(0, y);
		dc->LineTo(rct.Width(), y);
	}

	if ( timePoint.x != 100000) {
		CPen redPen(PS_SOLID,2, RGB(255,0,0));
		dc->SelectObject(redPen);
		dc->MoveTo(timePoint.x, rct.bottom);
		dc->LineTo(timePoint.x, rct.top);
		int index = (int)(0.5 + (double)timePoint.x / xscale);
		s = String("%d", index);
		if ( fldGraph->interval.fValid() == false)
			s = s = String("%d", index);
		else {
			ILWIS::Time time(fldGraph->timeCol->rValue(index));
			ILWIS::Time::Mode m = fldGraph->interval.rWidth() > 3 ? ILWIS::Time::mDATE : ILWIS::Time::mTIME;
			s = time.toString(true,m);
		}
		CSize szTxt = dc->GetTextExtent(s.c_str());
		::TextOut(lpDIS->hDC,timePoint.x - szTxt.cx/2, crct.top+2, s.c_str(),s.size());
	}

	delete [] pts;
	SelectObject(lpDIS->hDC,oldPen);
	SelectObject(lpDIS->hDC, oldBrush);
	SelectObject(lpDIS->hDC, fntOld);
	fnt->DeleteObject();
	delete fnt;
}

void TimeGraph::setLinkedWindow(CWnd *wnd){
	toBeNotified = wnd;
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
			col = fldGraph->sourceTable->col(fldGraph->sourceColumn);
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
	if ( threshold != rUNDEF) {
		if ( toBeNotified ) {
			if ( index < marked.size())
				toBeNotified->SendMessage(ID_NOTIFY_ME,marked[index]);
		}
	}
	Invalidate();


}

void TimeGraph::PreSubclassWindow() 
{
	EnableToolTips();
	
	CStatic::PreSubclassWindow();
}

void TimeGraph::setThreshold(double v, bool above){
	threshold = v;
	isAbove = above;
}

double TimeGraph::getThreshold() const{
	return threshold;
}
//----------------------------------------------------
TimeGraphSlider::TimeGraphSlider(FormEntry* par, RangeInt defaultRange) :
FormEntry(par,0,true),
timegraph(0)
{
	psn->iMinWidth = psn->iWidth = 250;
	psn->iMinHeight = psn->iHeight = 80;
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
	if ( tbl.fValid())
		recordRange = RangeInt(1,tbl->iRecs());
	else
		recordRange = RangeInt();
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

void TimeGraphSlider::setTimeInterval(ILWIS::TimeInterval in) {
	interval = in;
}

void TimeGraphSlider::setTimes(const Column& col) {
	timeCol = col;
}

void TimeGraphSlider::setThreshold(double v, bool above){
	timegraph->setThreshold(v, above);
}

double TimeGraphSlider::getThreshold() const{
	return timegraph->getThreshold();
}

void TimeGraphSlider::setLinkedWindow(CWnd *wnd){
	timegraph->setLinkedWindow(wnd);
}