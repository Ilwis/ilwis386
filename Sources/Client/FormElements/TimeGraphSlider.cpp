#include "Client\Headers\formelementspch.h"
#include "Client\FormElements\TimeGraphSlider.h"


BEGIN_MESSAGE_MAP(TimeGraph, CStatic)
	ON_WM_LBUTTONUP()
END_MESSAGE_MAP()

TimeGraph::TimeGraph(TimeGraphSlider *f, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID) : BaseZapp(f) 
{
	fldGraph = f;
	if (!CStatic::Create(0,dwStyle | SS_OWNERDRAW | SS_NOTIFY, rect, pParentWnd, nID))
		throw ErrorObject(TR("Could not create Time graph"));
	timePoint = CPoint(100000, 100000);
}

void TimeGraph::DrawItem(LPDRAWITEMSTRUCT lpDIS) {
	if ( !fldGraph->sourceTable.fValid() || fldGraph->sourceColumn == "")
		return;
	Column col = fldGraph->sourceTable->col(fldGraph->sourceColumn);
	//Column col = fldGraph->sourceTable->col("numbers");
	if ( !col.fValid())
		return;
	RangeReal rr = col->rrMinMax();
	CRect rct;
	
	GetClientRect(rct);
	Color bkColor = GetBkColor(lpDIS->hDC);
	CBrush brushBk(RGB(0, 50, 100));
	HGDIOBJ oldBrush = SelectObject(lpDIS->hDC, brushBk);
	::Rectangle(lpDIS->hDC, rct.left, rct.top, rct.right, rct.bottom);
	SelectObject(lpDIS->hDC, oldBrush);

	double yscale = rct.Height() / rr.rWidth();
	double y0 = rr.rWidth() * yscale;
	double xscale = rct.Width() / fldGraph->recordRange.iWidth();
	CPoint *pts = new CPoint[fldGraph->recordRange.iWidth() + 3];
	double rx = 0;
	for(int i = fldGraph->recordRange.iLo(); i <= fldGraph->recordRange.iHi(); ++i) {
		int y = y0 - ( col->rValue(i) - rr.rLo()) * yscale;
		pts[i-1] = CPoint(rx,y);
		rx += xscale;
	}
	pts[fldGraph->recordRange.iWidth() + 1] = CPoint(rct.Width(), rct.Height());
	pts[fldGraph->recordRange.iWidth() + 2] = CPoint(0, rct.Height());


	CPen pen(PS_SOLID, 1, RGB(0, 0, 200));
	HGDIOBJ oldPen =  SelectObject(lpDIS->hDC, pen);

   // and a solid red brush
    CBrush brush(RGB(50, 150, 50));
	oldBrush = SelectObject(lpDIS->hDC, brush);
	::Polygon(lpDIS->hDC,pts,fldGraph->recordRange.iWidth() + 3);

	if ( timePoint.x != 100000) {
		CPen redPen(PS_SOLID,2, RGB(255,0,0));
		SelectObject(lpDIS->hDC, redPen);
		::MoveToEx(lpDIS->hDC, timePoint.x, rct.bottom,0);
		::LineTo(lpDIS->hDC, timePoint.x, rct.top);
	}


	delete [] pts;
	SelectObject(lpDIS->hDC,oldPen);
	SelectObject(lpDIS->hDC, oldBrush);
}

void TimeGraph::OnLButtonUp(UINT nFlags, CPoint point) {	
	timePoint = point;
	CWnd *wnd =  GetParent();
	if ( wnd) {
		if ( !fldGraph->sourceTable.fValid() || fldGraph->sourceColumn == "")
			return;
		Column col = fldGraph->sourceTable->col(fldGraph->sourceColumn);
		if ( !col.fValid())
			return;
		RangeReal rr = col->rrMinMax();
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
	if ( !fldGraph->sourceTable.fValid() || fldGraph->sourceColumn == "")
		return;
	Column col = fldGraph->sourceTable->col(fldGraph->sourceColumn);
	//Column col = fldGraph->sourceTable->col("numbers");
	if ( !col.fValid())
		return;
	RangeReal rr = col->rrMinMax();
	CRect rct;
	GetClientRect(rct);
	double xscale = rct.Width() / fldGraph->recordRange.iWidth();
	timePoint = CPoint(xscale * index, (rct.bottom + rct.top)/2.0); // y doesnt matter but anyway
	Invalidate();

}
//----------------------------------------------------
TimeGraphSlider::TimeGraphSlider(FormEntry* par) :
	FormEntry(par,0,true),
    timegraph(0)
{
  psn->iMinWidth = psn->iWidth = 250;
  psn->iMinHeight = psn->iHeight = 40;
  SetIndependentPos();
}

void TimeGraphSlider::create()
{
  zPoint pntFld = zPoint(psn->iPosX,psn->iPosY);
  zDimension dimFld = zDimension(psn->iWidth,psn->iMinHeight);
  timegraph = new TimeGraph(this, WS_VISIBLE | WS_BORDER | WS_CHILD | WS_TABSTOP,
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