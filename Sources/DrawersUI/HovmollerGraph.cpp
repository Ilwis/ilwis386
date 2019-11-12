#include "Client\Headers\formelementspch.h"
#include "Client\ilwis.h"
#include "Engine\Base\DataObjects\ObjectCollection.h"
#include "Client\FormElements\FieldListView.h"
#include "Engine\Drawers\SimpleDrawer.h" 
#include "Engine\Drawers\ComplexDrawer.h"
#include "Engine\Drawers\SpatialDataDrawer.h"
#include "Drawers\SetDrawer.h"
#include "Drawers\LayerDrawer.h"
#include "Engine\Drawers\RootDrawer.h"
#include "Engine\Domain\dmcoord.h"
#include "drawers\linedrawer.h"
#include "drawers\pointdrawer.h"
#include "Client\Mapwindow\MapPaneViewTool.h"
#include "Client\MapWindow\Drawers\DrawerTool.h"
#include "DrawersUI\HovMollerTool.h"
#include "Drawers\DrawingColor.h"
#include "HovMollerGraph.h"
#include "Drawers\CollectionDrawer.h"
#include "Engine\Base\Round.h"


BEGIN_MESSAGE_MAP(HovMollerGraph, CStatic)
	ON_WM_LBUTTONUP()
	ON_NOTIFY(TTN_NEEDTEXTW, 0, OnToolTipNotify)
	ON_NOTIFY(TTN_NEEDTEXTA, 0, OnToolTipNotify)
	ON_WM_CONTEXTMENU()
END_MESSAGE_MAP()


#define CSGRPAH_SIZE 470
HovMollerGraph::HovMollerGraph(HovMollerGraphEntry *f, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID) : BaseZapp(f) 
{
	fldGraph = f;
	if (!CStatic::Create(0,dwStyle | SS_OWNERDRAW | SS_NOTIFY, rect, pParentWnd, nID))
		throw ErrorObject(TR("Could not create CS graph"));
	ModifyStyleEx(0, WS_EX_NOPARENTNOTIFY, SWP_FRAMECHANGED);
	EnableTrackingToolTips();
	xIndex = -1;
	yIndex = -1;
}

int HovMollerGraph::OnToolHitTest(CPoint point, TOOLINFO *pTI) const
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

void HovMollerGraph::OnToolTipNotify(NMHDR *pNMHDR, LRESULT *pResult)
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

void HovMollerGraph::DrawItem(LPDRAWITEMSTRUCT lpDIS) {
	bool useDefault = false;
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

	rctInner = CRect(crct.left+60, crct.top,crct.right -100, crct.bottom-30 );
	Color bkColor = GetBkColor(lpDIS->hDC);
	CBrush bbrushBk(RGB(255, 255, 255));
	SelectObject(lpDIS->hDC, bbrushBk);
	::Rectangle(lpDIS->hDC, rctInner.left,rctInner.top, rctInner.right, rctInner.bottom);
	SelectObject(lpDIS->hDC, oldBrush);


	CFont* fnt = new CFont();
	BOOL vvv = fnt->CreatePointFont(70,"Arial");
	CDC *dc = CDC::FromHandle(lpDIS->hDC);

	HGDIOBJ fntOld = dc->SelectObject(fnt);
	int points = track.size();
	if ( points == 0 || values.size() == 0 || !fldGraph->mpl.fValid())
		return;

	int oldnr = iUNDEF;
	Map mp = fldGraph->mpl[0];
	int noOfClassMaps = 0;	
	int maps = fldGraph->mpl->iSize();
	double yStep = (double)rctInner.Height() / maps;
	double xStep = (double)rctInner.Width() / points;
	ILWIS::NewDrawer *ndrw = fldGraph->tool->getDrawer();
	ILWIS::LayerDrawer *ldr = dynamic_cast<ILWIS::LayerDrawer *>(ndrw);
	ILWIS::DrawingColor *drc = ldr->getDrawingColor();
	CRect moveRect; //(rctInner.left, rctInner.top,rctInner.left + xStep,rctInner.top + yStep);
	double xoff = 0;
	double yoff = 0;
	for(int m =0; m < maps; ++m) {
		moveRect.left = rctInner.left;
		moveRect.right = rctInner.left + xStep;
		moveRect.top =  rctInner.top + yoff;
		moveRect.bottom = rctInner.top + yoff + yStep;
		for(int x=0; x < points; ++x) {
			Color clr;
			if ( x == points - 1) {
			TRACE("left %d right %d xmin %d xmax %d\n", moveRect.left, moveRect.right, rctInner.left, rctInner.right);
			}
			double v = values[m][x];
			if ( v != rUNDEF)
				clr = drc->clrVal(v);
			else
				clr = RGB(255,255,255);

			CPen pen(PS_SOLID, 1,clr);
			SelectObject(lpDIS->hDC,pen);
			CBrush brush(clr);
			SelectObject(lpDIS->hDC, brush);
			dc->Rectangle(&moveRect);
			xoff += xStep;
			moveRect.left = rctInner.left + xoff ;
			moveRect.right = rctInner.left + xoff + xStep;
			CPen penTick(PS_SOLID,1,RGB(0,0,0));
			SelectObject(lpDIS->hDC,penTick);
			int frac = 100 * (double) x / points;
			if ( frac % 10 == 0) {
				dc->MoveTo(rctInner.left + xoff,rctInner.bottom);
				dc->LineTo(rctInner.left + xoff,rctInner.bottom + 6);
			}
		}
		CPen penTick(PS_SOLID,1,RGB(0,0,0));
		SelectObject(lpDIS->hDC,penTick);
		dc->MoveTo(rctInner.left,rctInner.bottom - yoff);
		dc->LineTo(rctInner.left-4,rctInner.bottom - yoff);
		yoff += yStep;
		xoff = 0;
	}
	dc->MoveTo(rctInner.left,rctInner.bottom - yoff);
	dc->LineTo(rctInner.left-4,rctInner.bottom - yoff);

	setTimeText(1, dc);
	setTimeText(maps / 3, dc);
	setTimeText(2 * maps / 3, dc);
	setTimeText(maps, dc);

	setCoordText(0,mp, dc);
	setCoordText(points / 2,  mp, dc);
	setCoordText(points - 1,  mp, dc);

	setTextDataBlock(dc);

	fnt->DeleteObject();
	delete fnt;
}

void HovMollerGraph::setTextDataBlock(CDC *dc){
	String sValue, sTime,sLocX, sLocY;
	sValue = sTime = sLocX = sLocY = ":";
	fldGraph->tool->setMarker(Coord());
	String sIndex = (yIndex > 0 && yIndex <= values.size()) ? String("%d",yIndex) : "";
	if ( xIndex >= 0 && (yIndex > 0 && yIndex <= values.size())) {
		String st;
		if ( timeColumn.fValid()) {
			ILWIS::Time time(timeColumn->rValue(yIndex));
			st = time.toString(true,ILWIS::Time::mDATE);
		} else
			st = "?";

		sTime += st;
		Coord & crd (trackCrd[xIndex]);
		LatLon ll = fldGraph->mpl[0]->cs()->llConv(crd);
		sLocY += ll.sLat(10);
		sLocX += ll.sLon(10);
		sValue = String("%S%f",sValue, values[yIndex - 1][xIndex]);
		fldGraph->tool->setMarker(crd);
	}
	dc->TextOut(rctInner.right + 8,rctInner.top, "Latitude");
	dc->TextOut(rctInner.right + 50,rctInner.top, sLocY.c_str());
	dc->TextOut(rctInner.right + 8,rctInner.top + 14,"Longitude");
	dc->TextOut(rctInner.right + 50,rctInner.top + 14, sLocX.c_str());
	dc->TextOut(rctInner.right + 8,rctInner.top + 28,"Index");
	dc->TextOut(rctInner.right + 50,rctInner.top + 28, sIndex.c_str());
	dc->TextOut(rctInner.right + 8,rctInner.top + 42,"Time");
	dc->TextOut(rctInner.right + 50,rctInner.top + 42, sTime.c_str());
	dc->TextOut(rctInner.right + 8,rctInner.top + 56,"Value");
	dc->TextOut(rctInner.right + 50,rctInner.top + 56, sValue.c_str());
}

void HovMollerGraph::setTimeText(int index, CDC *dc) {
	if ( timeColumn.fValid()) {
		ILWIS::Time time(timeColumn->rValue(index));
		String st = time.toString(true,ILWIS::Time::mDATE);
		double frac = (double) (index - 1.5) / fldGraph->mpl->iSize();
		CSize sz = dc->GetTextExtent(st.c_str(), st.size());
		dc->TextOut(rctInner.left - sz.cx - 6,rctInner.bottom - rctInner.Height() * frac - sz.cy,st.c_str());

	} else {
		String st("%d", index);
		double frac = (double) (index - 1.5) / fldGraph->mpl->iSize();
		CSize sz = dc->GetTextExtent(st.c_str(), st.size());
		dc->TextOut(rctInner.left - sz.cx - 6,rctInner.bottom - rctInner.Height() * frac - sz.cy,st.c_str());
	}
}

void HovMollerGraph::setCoordText(int index, const Map& mp, CDC *dc) {
	double frac = (double) index / track.size();
	LatLon ll = mp->cs()->llConv(trackCrd[index]);
	String sll = ll.sValue(20);
	CSize sz = dc->GetTextExtent(sll.c_str(), sll.size());
	dc->TextOut(rctInner.left  + rctInner.Width() * frac - sz.cx / 2,rctInner.bottom + 8,sll.c_str());
}

void HovMollerGraph::OnLButtonUp(UINT nFlags, CPoint point) {
	if (fldGraph->mpl.fValid()) {
		CRect rct;
		GetClientRect(rct);
		double fractx = (double)(point.x - rctInner.left) / rctInner.Width();
		double fracty = (double)(rctInner.bottom - point.y)/ rctInner.Height();
		if ( fractx >= 0 && fractx <= 1.0 && fracty >=0 && fracty <= 1.0) {
			double dx = track.size() * fractx;
			double dy = fldGraph->mpl->iSize() * fracty + 1.0;
			xIndex = dx;
			yIndex = dy;
			TRACE("%d %d %f %f\n",xIndex, yIndex, dx, dy);
			//xIndex = std::max(xIndex, 0);
			//xIndex = std::min(
			Invalidate();
		}
	}	
}

void HovMollerGraph::PreSubclassWindow() 
{
	EnableToolTips();
	
	CStatic::PreSubclassWindow();
}

void HovMollerGraph::setTimeColumn(const String& colName) {
	if( fldGraph->mpl->tblAtt().fValid() ) {
		timeColumn = fldGraph->mpl->tblAtt()->col(colName);
	} else
		timeColumn = Column();
}

void HovMollerGraph::setTrack(const vector<Coord>& crds){
	if ( !fldGraph->mpl.fValid() || fldGraph->mpl->iSize() == 0)
		return;

	double rD = 0;
	track.clear();
	values.clear();
	trackCrd.clear();
	for(int i = 1; i < crds.size(); ++i) {
		rD += rDist(crds[i-1], crds[i]);
	}
	double step = rD / 100.0;
	double totDist = 0;
	Map mp = fldGraph->mpl[0];
	RowCol rcOld;
	for(int i = 0; i < crds.size() - 1; ++i) {
		Coord c1 = crds[i];
		Coord c2 = crds[i+1];
		double distance = rDist(c1, c2);
		double angle = atan2(c2.y - c1.y, c2.x - c1.x);
		bool notDone = true;
		double cnt = 1;
		double d;
		RowCol rc = mp->gr()->rcConv(c1);
		track.push_back(rc);
		trackCrd.push_back(c1);

		while(notDone) {
			Coord c3(c1.x + cos(angle) * step * cnt, c1.y + sin(angle) * step * cnt);
			d = rDist(c1, c3);
			if ( d < distance ) {
				RowCol rc = mp->gr()->rcConv(c3);
				if ( rc != rcOld){
					track.push_back(rc);
					trackCrd.push_back(c3);
				}
			}
			else {
				notDone = false;
			}
			++cnt;
		}
		rc = mp->gr()->rcConv(c2);
		track.push_back(rc);
		trackCrd.push_back(c2);
	}
	values.resize(fldGraph->mpl->iSize());
	for(int j =0; j < values.size(); ++j) {
		values[j].resize(track.size());
		Map mp = fldGraph->mpl[j];
		for(int  i=0; i < track.size(); ++i) {
			double v = mp->rValue(track[i]);
			values[j][i] = v;
		}
	}
	xIndex = -1;
	yIndex = -1;
}

//----------------------------------------------------
HovMollerGraphEntry::HovMollerGraphEntry(FormEntry* par, ILWIS::HovMollerTool *t) :
FormEntry(par,0,true),
graph(0),
tool(t)
{
	psn->iMinWidth = psn->iWidth = CSGRPAH_SIZE;
	psn->iMinHeight = psn->iHeight = 320;
	SetIndependentPos();
}

void HovMollerGraphEntry::create()
{
	zPoint pntFld = zPoint(psn->iPosX,psn->iPosY);
	zDimension dimFld = zDimension(psn->iWidth,psn->iMinHeight);
	graph = new HovMollerGraph(this, WS_VISIBLE | WS_CHILD ,
		CRect(pntFld, dimFld) , frm()->wnd() , Id());
	graph->SetFont(frm()->fnt);
	CreateChildren();
}


void HovMollerGraphEntry::setSource(const MapList& ml){
	mpl = ml;
	if ( graph)
		graph->Invalidate();
}

void HovMollerGraphEntry::setTrack(const vector<Coord>& crds){
	if ( graph) {
		graph->setTrack(crds);
		graph->Invalidate();
	}

}

void HovMollerGraphEntry::setTimeColumn(const String& colName) {
	if ( graph) {
		graph->setTimeColumn(colName);
		graph->Invalidate();
	}
}


void HovMollerGraphEntry::update() {
	graph->Invalidate();
}





