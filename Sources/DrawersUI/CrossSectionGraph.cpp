#include "Client\Headers\formelementspch.h"
#include "Client\ilwis.h"
#include "Engine\Base\DataObjects\ObjectCollection.h"
#include "Client\FormElements\FieldListView.h"
#include "CrossSectionGraph.h"


BEGIN_MESSAGE_MAP(CrossSectionGraph, CStatic)
	ON_WM_LBUTTONUP()
	ON_NOTIFY(TTN_NEEDTEXTW, 0, OnToolTipNotify)
	ON_NOTIFY(TTN_NEEDTEXTA, 0, OnToolTipNotify)
END_MESSAGE_MAP()

#define CSGRPAH_SIZE 450
CrossSectionGraph::CrossSectionGraph(CrossSectionGraphEntry *f, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID) : BaseZapp(f) 
{
	fldGraph = f;
	if (!CStatic::Create(0,dwStyle | SS_OWNERDRAW | SS_NOTIFY, rect, pParentWnd, nID))
		throw ErrorObject(TR("Could not create CS graph"));
	ModifyStyleEx(0, WS_EX_NOPARENTNOTIFY, SWP_FRAMECHANGED);
	EnableTrackingToolTips();
}

int CrossSectionGraph::OnToolHitTest(CPoint point, TOOLINFO *pTI) const
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

void CrossSectionGraph::OnToolTipNotify(NMHDR *pNMHDR, LRESULT *pResult)
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


int CrossSectionGraph::getNumberOfMaps(long i) {
	IlwisObject obj = fldGraph->sources[i];
	if ( IOTYPE(obj->fnObj) == IlwisObject::iotMAPLIST) {
		MapList mpl(obj->fnObj);
		return mpl->iSize();

	}
	else if ( IOTYPE(obj->fnObj) == IlwisObject::iotOBJECTCOLLECTION) {
		ObjectCollection oc(obj->fnObj);
		oc->iNrObjects();

	}
	return iUNDEF;
}

BaseMap CrossSectionGraph::getBaseMap(long i, long m) {
	IlwisObject obj = fldGraph->sources[m];
	if ( IOTYPE(obj->fnObj) == IlwisObject::iotMAPLIST) {
		MapList mpl(obj->fnObj);
		return mpl[i];

	}
	else if ( IOTYPE(obj->fnObj) == IlwisObject::iotOBJECTCOLLECTION) {
		ObjectCollection oc(obj->fnObj);
		BaseMap bmp(oc->fnObject(i));
		return bmp;

	}
	return BaseMap();
}

void CrossSectionGraph::DrawItem(LPDRAWITEMSTRUCT lpDIS) {
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
	CBrush bbrushBk(RGB(0, 50, 100));
	SelectObject(lpDIS->hDC, bbrushBk);
	::Rectangle(lpDIS->hDC, rct.left,rct.top, rct.right, rct.bottom);
	SelectObject(lpDIS->hDC, oldBrush);


	CFont* fnt = new CFont();
	BOOL vvv = fnt->CreatePointFont(80,"Arial");
	CDC *dc = CDC::FromHandle(lpDIS->hDC);

	HGDIOBJ fntOld = dc->SelectObject(fnt);
	if ( fldGraph->crdSelect == Coord())
		return;

	int oldnr = iUNDEF;
	values.clear();
	for(int m =0; m < fldGraph->sources.size(); ++m) {
		values.resize(fldGraph->sources.size());
		RangeReal rr = fldGraph->getRange(m);
		int numberOfMaps = getNumberOfMaps(m);

		double yscale = rct.Height() / rr.rWidth();
		double y0 = rr.rWidth() * yscale;
		double xscale = (double)rct.Width() / numberOfMaps;
		double rx = 0;
		CPen bpen(PS_SOLID, 1, Representation::clrPrimary(m < 2 ? m + 1 : m + 2));
		SelectObject(lpDIS->hDC, bpen);
		for(int i = 0; i < numberOfMaps; ++i) {
			BaseMap bmp = getBaseMap(i, m);
			double v = bmp->rValue(fldGraph->crdSelect);
			values[m].push_back(v);
			int y = y0 - ( v - rr.rLo()) * yscale;
			if ( i == 0)
				dc->MoveTo(rx,y);
			else 
				dc->LineTo(rx,y);
			if ( i % 5 == 0) {
				String s("%d", i);
				CSize sz = dc->GetTextExtent(s.scVal(), s.size());
				dc->TextOut(rx - sz.cx / 2, crct.bottom - 16,s.scVal(),s.size());
			}
				rx += xscale;
		}
		if ( oldnr != numberOfMaps) {
			rx = 0;
			for(int i = 0; i < numberOfMaps; ++i) {
		
				if ( i % 5 == 0) {
					dc->MoveTo(rx,rct.bottom);
					dc->LineTo(rx,rct.bottom + 3);
				}
				rx += xscale;
			}
		}
		oldnr = numberOfMaps;
		if ( fldGraph->currentIndex >= 0) {
			double value = values[m].at(fldGraph->currentIndex);
			fldGraph->setIndex(m, value);
		}
	}
	SelectObject(lpDIS->hDC,oldPen);
	SelectObject(lpDIS->hDC, oldBrush);
	SelectObject(lpDIS->hDC, fntOld);
	fnt->DeleteObject();
	delete fnt;
}

void CrossSectionGraph::OnLButtonUp(UINT nFlags, CPoint point) {	
	CWnd *wnd =  GetParent();
	if ( wnd) {
		for(int m =0; m < fldGraph->sources.size(); ++m) {
			RangeReal rr = fldGraph->getRange(m);
			int numberOfMaps = getNumberOfMaps(m);
			CRect rct;
			GetClientRect(rct);
			double xscale = rct.Width() / numberOfMaps;
			fldGraph->currentIndex = point.x / xscale;
			if ( fldGraph->currentIndex >= values[m].size())
				continue;
			double value = values[m].at(fldGraph->currentIndex);
			fldGraph->setIndex(m, value);
		}
	}
}
void CrossSectionGraph::PreSubclassWindow() 
{
	EnableToolTips();
	
	CStatic::PreSubclassWindow();
}
//----------------------------------------------------
CrossSectionGraphEntry::CrossSectionGraphEntry(FormEntry* par) :
FormEntry(par,0,true),
crossSectionGraph(0),
listview(0),
currentIndex(iUNDEF)
{
	psn->iMinWidth = psn->iWidth = CSGRPAH_SIZE;
	psn->iMinHeight = psn->iHeight = 120;
	SetIndependentPos();
}

void CrossSectionGraphEntry::create()
{
	zPoint pntFld = zPoint(psn->iPosX,psn->iPosY);
	zDimension dimFld = zDimension(psn->iWidth,psn->iMinHeight);
	crossSectionGraph = new CrossSectionGraph(this, WS_VISIBLE | WS_CHILD ,
		CRect(pntFld, dimFld) , frm()->wnd() , Id());
	crossSectionGraph->SetFont(frm()->fnt);
	CreateChildren();
}

void CrossSectionGraphEntry::setIndex(int sourceIndex, double value) {
	vector<String> v;
	IlwisObject obj = sources[sourceIndex];
	v.push_back(obj->fnObj.sFile + obj->fnObj.sExt);
	if ( IOTYPE(obj->fnObj) == IlwisObject::iotMAPLIST) {
		MapList mpl(obj->fnObj);
		v.push_back(String("%d:%d",mpl->iLower(), mpl->iUpper()));
		RangeReal rr = getRange(sources.size() - 1);
		v.push_back(String("%S", rr.s()));
		v.push_back(String("%d", currentIndex));
		v.push_back(String("%g",value));
	}
	listview->setData(sourceIndex, v);
	listview->update();
}

bool CrossSectionGraphEntry::isUnique(const FileName& fn) {
	
	for(int i=0; i < sources.size(); ++i) {
		if ( sources.at(i)->fnObj == fn)
			return false;
	}
	return true;
}

void CrossSectionGraphEntry::addSourceSet(const IlwisObject& obj){
	if ( isUnique(obj->fnObj)) {
		sources.push_back(obj);
		if (listview) {
			vector<String> v;
			v.push_back(obj->fnObj.sFile + obj->fnObj.sExt);
			if ( IOTYPE(obj->fnObj) == IlwisObject::iotMAPLIST) {
				MapList mpl(obj->fnObj);
				v.push_back(String("%d:%d",mpl->iLower(), mpl->iUpper()));
				RangeReal rr = getRange(sources.size() - 1);
				v.push_back(String("%S", rr.s()));
				v.push_back("?");
				v.push_back("?");

			} else if (IOTYPE(obj->fnObj) == IlwisObject::iotOBJECTCOLLECTION) {
				ObjectCollection oc(obj->fnObj);
			}
			listview->AddData(v);
		}
		if ( crossSectionGraph)
			crossSectionGraph->Invalidate();
	}
}

void CrossSectionGraphEntry::setCoord(const Coord& crd){
	crdSelect = crd;
	if ( crossSectionGraph) {
		crossSectionGraph->Invalidate();
	}

}

void CrossSectionGraphEntry::setListView(FieldListView *v) {
	listview = v;
	v->psn->iMinWidth = v->psn->iWidth = CSGRPAH_SIZE;
	v->psn->iMinHeight = v->psn->iHeight = 100;
}


RangeReal CrossSectionGraphEntry::getRange(long i) {
	IlwisObject obj = sources[i];
	if ( IOTYPE(obj->fnObj) == IlwisObject::iotMAPLIST) {
		if ( ranges.size() <= i) {
			MapList mpl(obj->fnObj);
			RangeReal rr;
			for(int j=0; j < mpl->iSize(); ++j) {
				rr += mpl->map(i)->rrMinMax(BaseMapPtr::mmmCALCULATE);
			}
			ranges.push_back(rr);
		}

	}
	else if ( IOTYPE(obj->fnObj) == IlwisObject::iotOBJECTCOLLECTION) {
		if ( ranges.size() <= i) {
			ObjectCollection oc(obj->fnObj);
			RangeReal rr;
			for(int j=0; j < oc->iNrObjects(); ++j) {
				BaseMap bmp(oc->fnObject(i));
				rr += bmp->rrMinMax(BaseMapPtr::mmmCALCULATE);
			}
			ranges.push_back(rr);
		}
	}
	return ranges[i];
}




