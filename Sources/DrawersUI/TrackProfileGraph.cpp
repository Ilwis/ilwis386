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
#include "DrawersUI\TrackProfileTool.h"
#include "Drawers\DrawingColor.h"
#include "TrackProfileGraph.h"
#include "Drawers\CollectionDrawer.h"
#include "Engine\Domain\DmValue.h"
#include "Engine\Base\Round.h"


BEGIN_MESSAGE_MAP(TrackProfileGraph, CStatic)
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
	ON_NOTIFY(TTN_NEEDTEXTW, 0, OnToolTipNotify)
	ON_NOTIFY(TTN_NEEDTEXTA, 0, OnToolTipNotify)
	ON_WM_CONTEXTMENU()
END_MESSAGE_MAP()

#define CSGRPAH_SIZE 650
TrackProfileGraph::TrackProfileGraph(TrackProfileGraphEntry *f, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID)
: BaseZapp(f)
, yStretch(false)
, fDown(false)
, markerXposOld(iUNDEF)
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

ILWIS::LayerDrawer *TrackProfileGraph::getLayerDrawer(ILWIS::NewDrawer *ndr) const{

	ILWIS::ComplexDrawer *cdr = (ILWIS::ComplexDrawer *)ndr;
	ILWIS::LayerDrawer *ldr = 0;
	if ( !cdr->isSet()) {
		ldr = (ILWIS::LayerDrawer *)cdr;

	} else {
		ILWIS::SetDrawer *spdr = (ILWIS::SetDrawer *)fldGraph->tool->getDrawer();
		int activeIndex = spdr->getCurrentIndex();
		if ( activeIndex != iUNDEF)
			ldr = (ILWIS::LayerDrawer *)spdr->getDrawer(activeIndex);
	}
	return ldr;
}

double TrackProfileGraph::getValue(int i, const BaseMap& bmp, const Coord& c1) const{
	Coord crd = fldGraph->tool->getDrawer()->getRootDrawer()->glToWorld(bmp->cs(), c1);
	if ( bmp->dm()->pdv())
		return bmp->rValue(crd);
	return bmp->iRaw(crd);
}

Domain TrackProfileGraph::getDomain(int i) const{
	if ( i == 0) {
		ILWIS::LayerDrawer *ldr = getLayerDrawer(fldGraph->tool->getDrawer());
		if ( ldr->useAttributeColumn()) {
			return ldr->getAtttributeColumn()->dm();
		}
	}
	IlwisObject obj = fldGraph->tool->sources[i]->getSource();
	if ( IOTYPE(obj->fnObj) == IlwisObject::iotMAPLIST) {
		MapList mpl(obj->fnObj);
		return mpl[0]->dm();
	}
	else if (IOTYPE(obj->fnObj) == IlwisObject::iotOBJECTCOLLECTION) {
		ObjectCollection oc(obj->fnObj);
		BaseMap bmp(oc->fnObject(0));
		return bmp->dm();
	}
	else  
		return fldGraph->tool->sources[i]->getMap()->dm();
}

RangeReal TrackProfileGraph::getRange(int i) const {
	RangeReal rr = fldGraph->tool->sources[i]->getRange();
	if ( rr.fValid())
		return rr;

	if ( i == 0) {
		ILWIS::CollectionDrawer *cdr = dynamic_cast<ILWIS::CollectionDrawer *>(fldGraph->tool->getDrawer());
		if ( cdr) {
			return cdr->getStretchRangeReal();
		}
		ILWIS::LayerDrawer *ldr = getLayerDrawer(fldGraph->tool->getDrawer());
		if ( ldr->useAttributeColumn()) {
			return ldr->getAtttributeColumn()->rrMinMax();
		}
	}
	IlwisObject obj = fldGraph->tool->sources[i]->getSource();
	if ( IOTYPE(obj->fnObj) == IlwisObject::iotMAPLIST) {
		MapList mpl(obj->fnObj);
		return mpl->getRange();
	}
	else if (IOTYPE(obj->fnObj) == IlwisObject::iotOBJECTCOLLECTION) {
		ObjectCollection oc(obj->fnObj);
		return oc->getRange();
	}
	else  
		return fldGraph->tool->sources[i]->getMap()->rrMinMax(BaseMapPtr::mmmCALCULATE);
}

Color TrackProfileGraph::getColor(int i, const BaseMap&bmp, long raw) const {
	if ( i == 0) {
		ILWIS::LayerDrawer *ldr = getLayerDrawer(fldGraph->tool->getDrawer());
		if ( ldr && ldr->useAttributeColumn()) {
			ILWIS::DrawingColor dc(ldr);
			Column col = ldr->getAtttributeColumn();
			dc.setDataColumn(col);
			return dc.clrRaw(raw, ILWIS::NewDrawer::drmRPR);
		}
	}
	if (bmp->dm()->pdid()) {
		if (i == 0) {
			ILWIS::LayerDrawer *ldr = getLayerDrawer(fldGraph->tool->getDrawer());
			if (ldr) {
				ILWIS::DrawingColor dc(ldr);
				return dc.clrRaw(raw, ILWIS::NewDrawer::drmMULTIPLE);
			}
		}
		return Representation::clrPrimary(raw);
	} else
		return bmp->dm()->rpr()->clrRaw(raw);
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
	CBrush bbrushBk(RGB(255, 255, 255));
	SelectObject(lpDIS->hDC, bbrushBk);
	::Rectangle(lpDIS->hDC, rct.left,rct.top, rct.right, rct.bottom); // MSDN: rectangle does not include the "right" and "bottom" coordinate
	SelectObject(lpDIS->hDC, oldBrush);

	CFont* fnt = new CFont();
	BOOL vvv = fnt->CreatePointFont(80,"Arial");
	CDC *dc = CDC::FromHandle(lpDIS->hDC);

	HGDIOBJ fntOld = dc->SelectObject(fnt);
	if ( track.size() == 0) {
		if (markerXposOld != iUNDEF)
			DrawMarker(iUNDEF, markerXposOld, crct);
		return;
	}

	int oldnr = iUNDEF;
	values.clear();
	double totDist = track[track.size() - 1].dist;
	int  noOfClassMaps = 0;
	int numberOfPoints = track.size();
	double xscale = (numberOfPoints > 1) ? (double)(rct.Width() + 1) / (numberOfPoints - 1) : (rct.Width() + 1);
	vector<double> markers;
	RangeReal rrTotal;
	if (!yStretch) {
		for(int m =0; m < fldGraph->tool->sources.size(); ++m) {
			rrTotal += getRange(m);
		}
	}
	for(int m =0; m < fldGraph->tool->sources.size(); ++m) {
		values.resize(fldGraph->tool->sources.size());
		double rx = 0;
		if ( getDomain(m)->pdv() )  {

			RangeReal rr = yStretch ? getRange(m) : rrTotal;

			ILWIS::LayerDrawer *ldr = getLayerDrawer(fldGraph->tool->getDrawer());
			Column col;
			bool fUseAttribute = false;
			if ( ldr && ldr->useAttributeColumn()) {
				col = ldr->getAtttributeColumn();
				if ( col->dm()->pdv())
					fUseAttribute = true;
			}

			double yscale = (rct.Height() - 1) / rr.rWidth();
			double y0 = rct.bottom - 1;
			CPen bpen(PS_SOLID, 1, Representation::clrPrimary(m < 1 ? m + 1 : m + 2)); // skip yellow
			SelectObject(lpDIS->hDC, bpen);
			for(int i = 0; i < numberOfPoints; ++i) {
				BaseMap bmp = fldGraph->tool->sources[m]->getMap(track[i].crd);
				if (!bmp.fValid())
					continue;
				double v = getValue(m, bmp, track[i].crd);
				values[m].push_back(GraphInfo((i + 1),v,track[i].crd));
				if (fUseAttribute)
					v = col->rValue(v);
				int y = v == rUNDEF ? (rct.bottom - 1) : y0 - ( v - rr.rLo()) * yscale;
				y = min(y, rct.bottom - 1);
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
				fldGraph->tool->setMarker(gi.crd);
			}
		} else if ( getDomain(m)->pdsrt()) {
			long oldRaw = iUNDEF;
			double oldX = 0;
			for(int i = 0; i <= numberOfPoints; ++i) {
				BaseMap bmp = fldGraph->tool->sources[m]->getMap(track[i].crd);
				long raw = i < numberOfPoints ? getValue(m,bmp,track[i].crd) : -1;
				if (i < numberOfPoints)
					values[m].push_back(GraphInfo((i + 1),raw,track[i].crd));
				if ( raw != oldRaw && oldRaw != iUNDEF) {
					if ( raw != rUNDEF) {
						Color clr = getColor(m, bmp, oldRaw);
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
		dc->MoveTo(markers[i], rct.bottom - 2);
		dc->LineTo(markers[i], rct.top);
	}

	double rx = 0;
	double n = 4.0;
	double rStep = rRound(totDist / n);
	while(n * rStep <= totDist)
		++n;

	if ( (track.size() > 0) && fldGraph->tool->sources[0]->getMap(track[0].crd)->cs()->pcsProjection()) {
		for(int i = 0; i < n; ++i) {
			dc->MoveTo(rx,rct.bottom);
			dc->LineTo(rx,rct.bottom + 3);
			String s("%3.1f", rStep * i);
			CSize sz = dc->GetTextExtent(s.c_str(), s.size());
			int xpos;
			if (i == 0)
				xpos = 0;
			else if (i == n - 1)
				xpos = min(rx - sz.cx / 2, rct.right - sz.cx);
			else
				xpos = rx - sz.cx / 2;
			dc->TextOut(xpos, crct.bottom - 16,s.c_str(),s.size());
			rx += (rct.Width() + 1) * rStep / totDist;
		}
	}

	SelectObject(lpDIS->hDC,oldPen);
	SelectObject(lpDIS->hDC, oldBrush);
	SelectObject(lpDIS->hDC, fntOld);
	fnt->DeleteObject();
	delete fnt;
	if (markerXposOld != iUNDEF)
		DrawMarker(iUNDEF, markerXposOld, crct);
}

void TrackProfileGraph::DrawMarker(int xposOld, int xpos, CRect & rect) {
	CDC * pDC = GetDC();
	int dmOld = pDC->SetROP2(R2_XORPEN);
	CPen bpen(PS_SOLID, 1, RGB(55,55,55)); // the XOR color
	CPen* pOldPen = pDC->SelectObject(&bpen);	
	if (xposOld != iUNDEF) {
		pDC->MoveTo(xposOld, rect.bottom - 22);
		pDC->LineTo(xposOld, rect.top);
	}
	if (xpos != iUNDEF) {
		pDC->MoveTo(xpos, rect.bottom - 22);
		pDC->LineTo(xpos, rect.top);
	}
	pDC->SelectObject(pOldPen);
	pDC->SetROP2(dmOld);
	ReleaseDC(pDC);
}

void TrackProfileGraph::OnLButtonDown(UINT nFlags, CPoint point) {
	fDown = true;
	CWnd *wnd =  GetParent();
	if ( wnd && values.size() > 0) {
		int numberOfPoints = track.size();
		CRect rct;
		GetClientRect(rct);
		DrawMarker(markerXposOld, point.x, rct);
		markerXposOld = point.x;
		double fract = (double)point.x / rct.Width();
		fldGraph->currentIndex = numberOfPoints * fract;
		for(int m =0; m < fldGraph->tool->sources.size(); ++m) {
			if ( fldGraph->currentIndex >= values[m].size())
				continue;
			double value = values[m].at(fldGraph->currentIndex).value;
			fldGraph->setIndex(m, value,values[m].at(fldGraph->currentIndex).crd);
		}
		fldGraph->tool->setMarker(values[0].at(fldGraph->currentIndex).crd);
	}
}

void TrackProfileGraph::OnMouseMove(UINT nFlags, CPoint point) {
	if (fDown) {
		CWnd *wnd =  GetParent();
		if ( wnd && values.size() > 0) {
			int numberOfPoints = track.size();
			CRect rct;
			GetClientRect(rct);
			DrawMarker(markerXposOld, point.x, rct);
			markerXposOld = point.x;
			double fract = (double)point.x / rct.Width();
			fldGraph->currentIndex = numberOfPoints * fract;
			for(int m =0; m < fldGraph->tool->sources.size(); ++m) {
				if ( fldGraph->currentIndex >= values[m].size())
					continue;
				double value = values[m].at(fldGraph->currentIndex).value;
				fldGraph->setIndex(m, value,values[m].at(fldGraph->currentIndex).crd);
			}
			fldGraph->tool->setMarker(values[0].at(fldGraph->currentIndex).crd);
		}
	}
}

void TrackProfileGraph::OnLButtonUp(UINT nFlags, CPoint point) {
	fDown = false;
	CWnd *wnd =  GetParent();
	if ( wnd && values.size() > 0) {
		int numberOfPoints = track.size();
		CRect rct;
		GetClientRect(rct);
		DrawMarker(markerXposOld, point.x, rct);
		markerXposOld = point.x;
		double fract = (double)point.x / rct.Width();
		fldGraph->currentIndex = numberOfPoints * fract;
		for(int m =0; m < fldGraph->tool->sources.size(); ++m) {
			if ( fldGraph->currentIndex >= values[m].size())
				continue;
			double value = values[m].at(fldGraph->currentIndex).value;
			fldGraph->setIndex(m, value,values[m].at(fldGraph->currentIndex).crd);
		}
		fldGraph->tool->setMarker(values[0].at(fldGraph->currentIndex).crd);
	}
}

void TrackProfileGraph::PreSubclassWindow() 
{
	EnableToolTips();	
	CStatic::PreSubclassWindow();
}

void TrackProfileGraph::setTrack(const vector<Coord>& crds){
	track.clear();
	values.clear();
	double rTotalDistance = 0;
	for(int i = 1; i < crds.size(); ++i) {
		rTotalDistance += rDist(crds[i-1], crds[i]);
	}
	double step = rTotalDistance / 99.0;
	double remain = 0; // the remaining distance to complete a multiple of "step", after each intermediate node
	double totDist = 0;
	for(int i = 0; i < crds.size() - 1; ++i) {
		if ( track.size() > 0)
			track[track.size() - 1].marker = true;
		Coord c1 = crds[i];
		Coord c2 = crds[i+1];
		double rPartialDistance = rDist(c1, c2);
		double angle = atan2(c2.y - c1.y, c2.x - c1.x);
		bool notDone = true;
		double cnt = 0;
		double d;
		while(notDone) {
			Coord c3(c1.x + cos(angle) * (remain + step * cnt), c1.y + sin(angle) * (remain + step * cnt));
			d = rDist(c1, c3);
			if ( d < rPartialDistance * 0.99999 ) { // if the distance is smaller than 99.999% of the partial distance, then we are not yet at the last point
				track.push_back(LocInfo(c3, totDist + d - remain));
			}
			else {
				remain = d - rPartialDistance;
				notDone = false;
			}
			++cnt;
		}
		totDist += d;
	}
	track.push_back(LocInfo(crds[crds.size() - 1], rTotalDistance));
}

#define ID_GR_COPY 5000
#define ID_SAVE_AS_CSV 5001
#define ID_SAVE_AS_TABLE 5002

void TrackProfileGraph::OnContextMenu(CWnd* pWnd, CPoint point) 
{
	//if (tools.size() == 0)
	//	return;
	//if (edit && edit->OnContextMenu(pWnd, point))
	//	return;
	CMenu men;
	men.CreatePopupMenu();
	//men.AppendMenu(MF_STRING, ID_SAVE_AS_CSV, TR("Save as CSV").c_str());
	men.AppendMenu(MF_STRING, ID_SAVE_AS_TABLE, TR("Open as Table").c_str());
	int cmd = men.TrackPopupMenu(TPM_LEFTALIGN|TPM_RIGHTBUTTON|TPM_RETURNCMD, point.x, point.y, pWnd);
	switch (cmd) {
		case ID_GR_COPY:
			break;
		case ID_SAVE_AS_CSV:
			saveAsCsv();
			break;
		case ID_SAVE_AS_TABLE:
			saveAsTbl();
			break;
	}
}

void TrackProfileGraph::saveAsCsv() {
	char strFilter[] = { "Comma separated Files (*.csv)|*.csv|All Files (*.*)|*.*||" };
	CFileDialog saveForm(FALSE,".csv",0,0,strFilter);
	if ( saveForm.DoModal() == IDOK) {
		FileName fn(saveForm.GetPathName());
		ofstream out(fn.sFullPath().c_str());
		if ( out.is_open()) {
			out<<"map name" << "coordinate index" << "x coordinate" << "y coordinate" << "value" << "\n";
			if (values.size() > 0) {
				for(int m =0; m < fldGraph->tool->sources.size(); ++m) {
					BaseMap bmp =  fldGraph->tool->sources[m]->getMap();
					String name = bmp->sName();
					for(int i=0; i < values[m].size(); ++i) {
						GraphInfo info= values[m][i];
						Coord crd = fldGraph->tool->getDrawer()->getRootDrawer()->glToWorld(bmp->cs(), info.crd);
						String v = bmp->sValue(crd,0);
						out << name.c_str() << "," << info.index <<"," << info.crd.x << "," << info.crd.y << "," << v.c_str() << "\n";
					}
				}
			}
			out.close();
		}		
	}
}

class TableNameForm : public FormWithDest {
public:
	TableNameForm(CWnd *par,String *name) : FormWithDest(par,TR("Open as table"),fbsSHOWALWAYS | fbsMODAL) {
		new FieldString(root,TR("Table name"),name);
		//create();
	}

	int exec() {
		FormWithDest::exec();
		return 1;
	}
};

void TrackProfileGraph::saveAsTbl() {
	String fname("TrackProfile");
	if ( TableNameForm(this, &fname).DoModal() == IDOK) {
		FileName fnTable = FileName::fnUnique(FileName(fname,".tbt"));
		Table tbl(fnTable,Domain("none"));
		DomainValueRangeStruct dvInt(1, (values.size() > 0) ? values[0].size() : 100);
		Column colIndex = tbl->colNew("Coordinate Index",dvInt);
		colIndex->SetOwnedByTable();
		Domain dmcrd;
		dmcrd.SetPointer(new DomainCoord(fldGraph->tool->getDrawer()->getRootDrawer()->getCoordinateSystem()->fnObj));
		Column colCrd = tbl->colNew("Coordinate", dmcrd, ValueRange());
		colCrd->SetOwnedByTable();
		if (values.size() > 0) {
			tbl->iRecNew(values[0].size());
			for(int m =0; m < fldGraph->tool->sources.size(); ++m) {
				BaseMap bmp =  fldGraph->tool->sources[m]->getMap();
				String name = bmp->sName();
				Column colValue = tbl->colNew(name.sQuote(), bmp->dvrs());
				colValue->SetOwnedByTable();
				for(int i=0; i < values[m].size(); ++i) {
					GraphInfo info = values[m][i];
					Coord crd = fldGraph->tool->getDrawer()->getRootDrawer()->glToWorld(bmp->cs(), info.crd);
					String v = bmp->sValue(crd,0);
					colIndex->PutVal(i + 1, info.index);
					colCrd->PutVal(i + 1, info.crd);
					colValue->PutVal(i + 1, v);
				}
			}
		}
		tbl->Store();
		IlwWinApp()->Execute(String("Open %S",fnTable.sRelative())); 
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
	psn->iMinHeight = psn->iHeight = 250;
	SetIndependentPos();
}

void TrackProfileGraphEntry::setOverruleRange(int col, int row, const String& value) {
	RangeReal rr(value);
	if ( rr.fValid()) {
		if ( col < tool->sources.size() ) {
			tool->sources[col]->setRange(rr);
			graph->Invalidate();
		}
	}
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
		for(int i=0; i < tool->sources.size(); ++i) {
			vector<String> v;
			v.push_back(tool->sources[i]->getSource()->fnObj.sFile + tool->sources[i]->getSource()->fnObj.sExt);
			v.push_back("?");
			v.push_back(graph->getRange(i).s());
			v.push_back("?");
			listview->setData(i, v);
		}
		listview->update();
		if ( graph) {
			if (graph->markerXposOld != iUNDEF) {
				CRect rect;
				graph->GetClientRect(rect);
				graph->DrawMarker(graph->markerXposOld, iUNDEF, rect);
				graph->markerXposOld = iUNDEF;
			}
		}
		return;
	}
	vector<String> v;
	BaseMap bmp = tool->sources[sourceIndex]->getMap(crd);
	if ( !bmp.fValid())
		return;
	v.push_back(bmp->fnObj.sFile + bmp->fnObj.sExt);
	if ( bmp->cs()->pcsProjection())
		v.push_back(String("%g", graph->track[currentIndex].dist));
	else
		v.push_back("?");
	if ( bmp->dm()->pdv()) {
		v.push_back(String("%S", graph->getRange(sourceIndex).s()));
		v.push_back(String("%g",value));
	} else if ( bmp->dm()->pdsrt()) {
		v.push_back(String("%S", bmp->dm()->sName()));
		ILWIS::LayerDrawer *ldr = (ILWIS::LayerDrawer *)tool->getDrawer();
		if ( ldr->useAttributeColumn()) {
			v.push_back(ldr->getAtttributeColumn()->sValue(value,0));
		}
		else
			v.push_back(bmp->dm()->sValueByRaw(value,0));
	}
	listview->setData(sourceIndex, v);
	listview->update();
}

void TrackProfileGraphEntry::addSource(const IlwisObject& bmp){
	if (listview && bmp.fValid()) {
		vector<String> v;
		v.push_back(bmp->fnObj.sFile + bmp->fnObj.sExt);
		v.push_back("?");
		v.push_back(graph->getRange(tool->sources.size()-1).s());
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

void TrackProfileGraphEntry::setYStretch(bool stretch) {
	if (graph) {
		graph->yStretch = stretch;
		graph->Invalidate();
	}
}

void TrackProfileGraphEntry::setListView(FieldListView *v) {
	listview = v;
	v->psn->iMinWidth = v->psn->iWidth = CSGRPAH_SIZE;
	v->psn->iMinHeight = v->psn->iHeight = 70;
}

void TrackProfileGraphEntry::update() {
	listview->update();
	graph->Invalidate();
}

void TrackProfileGraphEntry::openAsTable() {
	graph->saveAsTbl();
}

#define ID_ADD_ITEMS 5003
#define ID_REMOVE_ITEMS 5004

void TrackProfileGraphEntry::onContextMenu(CWnd* pWnd, CPoint point) {
	vector<int> rows;
	if (listview)
		listview->getSelectedRowNumbers(rows);
	bool fDisabled = (rows.size() == 0) || ((rows.size() == 1) && (rows[0] == 0));
	String menuText = ((rows.size() == 1) && (rows[0] == 0)) ? TR("Can't remove first item") : TR("Remove");
	CMenu men;
	men.CreatePopupMenu();
	men.AppendMenu(MF_STRING, ID_ADD_ITEMS, TR("Add").c_str());
	men.AppendMenu(MF_STRING | fDisabled ? MF_GRAYED : 0, ID_REMOVE_ITEMS, menuText.c_str());
	int cmd = men.TrackPopupMenu(TPM_LEFTALIGN|TPM_RIGHTBUTTON|TPM_RETURNCMD, point.x, point.y, pWnd);
	switch (cmd) {
		case ID_ADD_ITEMS:
			tool->displayOptionAddList();
			break;
		case ID_REMOVE_ITEMS:
			{
				for (vector<int>::reverse_iterator it = rows.rbegin(); it != rows.rend(); ++it) {
					if (*it == 0) // can't remove the first source, because it is the only one associated with the layer drawer
						continue;
					listview->RemoveData(*it);
					tool->sources.erase(tool->sources.begin() + *it);
				}
				update();
				tool->updateCbStretch();
			}
			break;
	}
}