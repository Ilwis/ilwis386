#include "Client\Headers\formelementspch.h"
#include "Client\ilwis.h"
#include "Engine\Base\DataObjects\ObjectCollection.h"
#include "Client\FormElements\FieldListView.h"
#include "Engine\Drawers\SimpleDrawer.h" 
#include "drawers\linedrawer.h"
#include "drawers\pointdrawer.h"
#include "Client\Mapwindow\MapPaneViewTool.h"
#include "Client\MapWindow\Drawers\DrawerTool.h"
#include "DrawersUI\TrackProfileTool.h"
#include "TrackProfileGraph.h"
#include "Engine\Base\Round.h"


BEGIN_MESSAGE_MAP(TrackProfileGraph, CStatic)
	ON_WM_LBUTTONUP()
	ON_NOTIFY(TTN_NEEDTEXTW, 0, OnToolTipNotify)
	ON_NOTIFY(TTN_NEEDTEXTA, 0, OnToolTipNotify)
END_MESSAGE_MAP()

#define CSGRPAH_SIZE 450
TrackProfileGraph::TrackProfileGraph(TrackProfileGraphEntry *f, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID) : BaseZapp(f) 
{
	fldGraph = f;
	if (!CStatic::Create(0,dwStyle | SS_OWNERDRAW | SS_NOTIFY, rect, pParentWnd, nID))
		throw ErrorObject(TR("Could not create CS graph"));
	ModifyStyleEx(0, WS_EX_NOPARENTNOTIFY, SWP_FRAMECHANGED);
	EnableTrackingToolTips();
}

LRESULT TrackProfileGraph::tick(WPARAM wp, LPARAM lp) {
	return 0;
}
int TrackProfileGraph::OnToolHitTest(CPoint point, TOOLINFO *pTI) const
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

void TrackProfileGraph::OnToolTipNotify(NMHDR *pNMHDR, LRESULT *pResult)
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


void TrackProfileGraph::DrawItem(LPDRAWITEMSTRUCT lpDIS) {
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

	CRect rct = CRect(crct.left, crct.top,crct.right, crct.bottom-20 );
	Color bkColor = GetBkColor(lpDIS->hDC);
	CBrush bbrushBk(RGB(50, 50, 50));
	SelectObject(lpDIS->hDC, bbrushBk);
	::Rectangle(lpDIS->hDC, rct.left,rct.top, rct.right, rct.bottom);
	SelectObject(lpDIS->hDC, oldBrush);


	CFont* fnt = new CFont();
	BOOL vvv = fnt->CreatePointFont(80,"Arial");
	CDC *dc = CDC::FromHandle(lpDIS->hDC);

	HGDIOBJ fntOld = dc->SelectObject(fnt);
	if ( track.size() == 0)
		return;

	int oldnr = iUNDEF;
	values.clear();
	double totDist = track[track.size() - 1].dist;
	int  noOfClassMaps = 0;
	int numberOfPoints = track.size();
	vector<double> markers;
	for(int m =0; m < fldGraph->tool->sources.size(); ++m) {
		values.resize(fldGraph->tool->sources.size());
		BaseMap bmp = fldGraph->tool->sources[m]->getMap();
		double xscale = (double)rct.Width() / numberOfPoints;
		double rx = 0;
		if ( bmp->dm()->pdv() )  {
			RangeReal rr = bmp->rrMinMax(BaseMapPtr::mmmCALCULATE);

			double yscale = rct.Height() / rr.rWidth();
			double y0 = rr.rWidth() * yscale;
			CPen bpen(PS_SOLID, 1, Representation::clrPrimary(m < 2 ? m + 1 : m + 2));
			SelectObject(lpDIS->hDC, bpen);
			for(int i = 0; i < numberOfPoints; ++i) {
				double v = bmp->rValue(track[i].crd);
				values[m].push_back(GraphInfo(i,v,track[i].crd));

				int y = v == rUNDEF ? rct.bottom : y0 - ( v - rr.rLo()) * yscale;
				if ( i == 0)
					dc->MoveTo(rx,y);
				else 
					dc->LineTo(rx,y);
				if ( track[i].marker)
					markers.push_back(rx);
				rx += xscale;
			}
			oldnr = numberOfPoints;
			if ( fldGraph->currentIndex >= 0) {
				GraphInfo gi = values[m].at(fldGraph->currentIndex);
				fldGraph->setIndex(m, gi.value, gi.crd);
			}
		} else if ( bmp->dm()->pdsrt()) {

			long oldRaw = iUNDEF;
			double oldX = 0;
			for(int i = 0; i <= numberOfPoints; ++i) {
				long raw = i < numberOfPoints ? bmp->iRaw(track[i].crd) : -1;
				values[m].push_back(GraphInfo(i,raw,track[i].crd));
				if ( raw != oldRaw && oldRaw != iUNDEF) {
					if ( raw != rUNDEF) {
						Color clr = bmp->dm()->rpr()->clrRaw(oldRaw);
						CBrush brush(clr);
						SelectObject(lpDIS->hDC, brush);
						::Rectangle(lpDIS->hDC,oldX,5 + 20 * noOfClassMaps ,rx, 25 + 20 * noOfClassMaps);
					}
					oldX= rx;
				}
				oldRaw = raw;
				rx += xscale;
			}
			noOfClassMaps++;

		}
	}
	CPen bpen(PS_SOLID, 1, RGB(150,150,150));
	SelectObject(lpDIS->hDC, bpen);
	for(int i = 0; i< markers.size(); ++i) {
		dc->MoveTo(markers[i], rct.bottom);
		dc->LineTo(markers[i], rct.top);

	}

	double limit = totDist;
	double rx = 0;
	double n = 4.0;
	double rStep = rRound(limit / n);
	if ( n * rStep < totDist - rStep)
		n++;

	if ( fldGraph->tool->sources[0]->getMap()->cs()->pcsProjection()) {
		for(int i = 0; i < n; ++i) {
			dc->MoveTo(rx,rct.bottom);
			dc->LineTo(rx,rct.bottom + 3);
			String s("%3.1f", rStep * i);
			CSize sz = dc->GetTextExtent(s.scVal(), s.size());
			dc->TextOut(rx - sz.cx / 2, crct.bottom - 16,s.scVal(),s.size());
			rx +=  rct.Width() *  rStep / totDist;
		}
	}

	SelectObject(lpDIS->hDC,oldPen);
	SelectObject(lpDIS->hDC, oldBrush);
	SelectObject(lpDIS->hDC, fntOld);
	fnt->DeleteObject();
	delete fnt;
}

void TrackProfileGraph::OnLButtonUp(UINT nFlags, CPoint point) {	
	CWnd *wnd =  GetParent();
	if ( wnd) {
		for(int m =0; m < fldGraph->tool->sources.size(); ++m) {
			BaseMap bmp = fldGraph->tool->sources[m]->getMap();
			RangeReal rr = bmp->rrMinMax(BaseMapPtr::mmmCALCULATE);
			int numberOfPoints = track.size();
			CRect rct;
			GetClientRect(rct);
			double fract = (double)point.x / rct.Width();
			fldGraph->currentIndex = numberOfPoints * fract;
			if ( fldGraph->currentIndex >= values[m].size())
				continue;
			double value = values[m].at(fldGraph->currentIndex).value;
			fldGraph->setIndex(m, value,values[m].at(fldGraph->currentIndex).crd);
		}
	}
}
void TrackProfileGraph::PreSubclassWindow() 
{
	EnableToolTips();
	
	CStatic::PreSubclassWindow();
}

void TrackProfileGraph::setTrack(const vector<Coord>& crds){
	double rD = 0;
	track.clear();
	values.clear();
	for(int i = 1; i < crds.size(); ++i) {
		rD += rDist(crds[i-1], crds[i]);
	}
	double step = rD / 100.0;
	double totDist = 0;
	for(int i = 0; i < crds.size() - 1; ++i) {
		Coord c1 = crds[i];
		Coord c2 = crds[i+1];
		double distance = rDist(c1, c2);
		double angle = atan2(c2.y - c1.y, c2.x - c1.x);
		bool notDone = true;
		double cnt = 1;
		double d;
		while(notDone) {
			Coord c3(c1.x + cos(angle) * step * cnt, c1.y + sin(angle) * step * cnt);
			d = rDist(c1, c3);
			if ( d < distance ) {
					track.push_back(LocInfo(c3,totDist + d));
			}
			else {
				track[track.size() - 1].marker = true;
				notDone = false;
			}
			++cnt;
		}
		totDist += d;
	}
}

//----------------------------------------------------
TrackProfileGraphEntry::TrackProfileGraphEntry(FormEntry* par, ILWIS::TrackProfileTool *t) :
FormEntry(par,0,true),
graph(0),
listview(0),
currentIndex(iUNDEF),
tool(t)
{
	psn->iMinWidth = psn->iWidth = CSGRPAH_SIZE;
	psn->iMinHeight = psn->iHeight = 120;
	SetIndependentPos();
}

void TrackProfileGraphEntry::create()
{
	zPoint pntFld = zPoint(psn->iPosX,psn->iPosY);
	zDimension dimFld = zDimension(psn->iWidth,psn->iMinHeight);
	graph = new TrackProfileGraph(this, WS_VISIBLE | WS_CHILD ,
		CRect(pntFld, dimFld) , frm()->wnd() , Id());
	graph->SetFont(frm()->fnt);
	CreateChildren();
}

void TrackProfileGraphEntry::setIndex(int sourceIndex, double value, const Coord& crd) {
	if ( sourceIndex == iUNDEF) { // switch marker off
		currentIndex = iUNDEF;
		tool->setMarker(Coord());
		return;

	}
	vector<String> v;
	BaseMap bmp = tool->sources[sourceIndex]->getMap();
	v.push_back(bmp->fnObj.sFile + bmp->fnObj.sExt);
	if ( bmp->cs()->pcsProjection())
		v.push_back(String("%g", graph->track[currentIndex].dist));
	else
		v.push_back("?");
	if ( bmp->dm()->pdv()) {
		v.push_back(String("%S", bmp->rrMinMax().s()));
		v.push_back(String("%g",value));
	} else if ( bmp->dm()->pdsrt()) {
		v.push_back(String("%S", bmp->dm()->sName()));
		v.push_back(bmp->dm()->sValueByRaw(value));
	}
	listview->setData(sourceIndex, v);
	tool->setMarker(crd);
	listview->update();
}

void TrackProfileGraphEntry::addSource(const BaseMap& bmp){
	if (listview) {
		vector<String> v;
		v.push_back(bmp->fnObj.sFile + bmp->fnObj.sExt);
		v.push_back(bmp->rrMinMax().s());
		v.push_back("?");
		v.push_back("?");

		listview->AddData(v);
	}
	if ( graph)
		graph->Invalidate();
}

void TrackProfileGraphEntry::setTrack(const vector<Coord>& crds){
	if ( graph) {
		graph->setTrack(crds);
		graph->Invalidate();
	}

}

void TrackProfileGraphEntry::setListView(FieldListView *v) {
	listview = v;
	v->psn->iMinWidth = v->psn->iWidth = CSGRPAH_SIZE;
	v->psn->iMinHeight = v->psn->iHeight = 100;
}

void TrackProfileGraphEntry::update() {
	listview->update();
	graph->Invalidate();
}





