#include "Client\Headers\formelementspch.h"
#include <iomanip>
#include "Client\ilwis.h"
#include "Engine\Base\DataObjects\ObjectCollection.h"
#include "Client\FormElements\FieldListView.h"
#include "Engine\Domain\DmValue.h"
#include "Engine\Domain\dmcoord.h"
#include "Engine\Drawers\ComplexDrawer.h"
#include "Drawers\LayerDrawer.h"
#include "Client\Mapwindow\MapPaneViewTool.h"
#include "Client\MapWindow\Drawers\DrawerTool.h"
#include "DrawersUI\CrossSectionTool.h"
#include "CrossSectionGraph.h"


BEGIN_MESSAGE_MAP(CrossSectionGraph, CStatic)
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
	ON_WM_CONTEXTMENU()
END_MESSAGE_MAP()

#define CSGRPAH_SIZE 650
CrossSectionGraph::CrossSectionGraph(CrossSectionGraphEntry *f, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID)
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

#define ID_GR_COPY 5100
#define ID_SAVE_AS_CSV 5101
#define ID_SAVE_AS_TABLE 5102

void CrossSectionGraph::OnContextMenu(CWnd* pWnd, CPoint point) 
{
	//if (tools.size() == 0)
	//	return;
	//if (edit && edit->OnContextMenu(pWnd, point))
	//	return;
	CMenu men;
	men.CreatePopupMenu();
	men.AppendMenu(MF_STRING, ID_SAVE_AS_TABLE, TR("Open as Table").c_str());
	int cmd = men.TrackPopupMenu(TPM_LEFTALIGN|TPM_RIGHTBUTTON|TPM_RETURNCMD, point.x, point.y, pWnd);
	switch (cmd) {
		case ID_GR_COPY:
			break;
		case ID_SAVE_AS_TABLE:
			saveAsTbl();
			break;
	}
}

class SpectrumForm : public FormWithDest {
public:
	SpectrumForm(CWnd *par,String *name, int *method, bool aggregate) : FormWithDest(par,TR("Save as Spectrum"),fbsSHOWALWAYS | fbsMODAL) {
		FieldString *fs = new FieldString(root,TR("spectrum name"),name);
		fs->SetWidth(100);
		fs->SetIndependentPos();
		RadioGroup *rg = new RadioGroup(root,TR("Aggregate method"), method,true);
		new RadioButton(rg,TR("Average"));
		new RadioButton(rg,TR("Min"));
		new RadioButton(rg,TR("Max"));
	}

	int exec() {
		FormWithDest::exec();
		return 1;
	}
};
void CrossSectionGraph::saveAsSpectrum(){
	if ( fldGraph->sources.size() != 1)
		throw ErrorObject(TR("Spectrum only works with one data source"));
	if ( IOTYPE(fldGraph->sources[0]->fnObj) != IlwisObject::iotMAPLIST)
		throw ErrorObject(TR("Spectrum only works with map lists"));

	String name = fldGraph->sources[0]->fnObj.sFile;

	int method = 0;
	int nmaps = getNumberOfMaps(0);
	MapList mpl(fldGraph->sources[0]->fnObj);
	if ( SpectrumForm(this, &name, &method,true).DoModal() == IDOK) {
		vector<double> values(nmaps);
		vector< vector<double> >  allValues(nmaps);
		for(int i=0;i < nmaps ; ++i)
			allValues[i].resize(fldGraph->crdSelect.size());

		for(int i = 0; i < fldGraph->crdSelect.size(); ++i) {
			for(long j = 0; j < nmaps; ++j) {
				BaseMap bmp;
				if ( mpl.fValid()) {
					bmp = mpl[j];
				} 
				if ( bmp.fValid()) {
					Coord crd = fldGraph->crdSelect[i];
					if ( bmp->cs() != fldGraph->csy)
						crd = bmp->cs()->cConv(fldGraph->csy, crd);
					double v = bmp->rValue(crd,0);
					allValues[j][i] = v;
				}
			}
		}
		vector<double> finalResult(allValues.size());
		for(int i =0; i < allValues.size(); ++i) {
			double result = rUNDEF;
			int count = 0;
			for(int j=0; j < allValues[i].size(); ++j) {
				double v = allValues[i][j];
				if ( v == rUNDEF)
					continue;
				switch(method) {
					case 0:
						result = result != rUNDEF ? result + v : v; break;
					case 1:
						result = rMIN(result, v); break;
					case 2:
						result = rMAX(result, v); break;
				}
				++count;
			}
			if ( method == 0) {
				if ( count != 0)
					result /= count;
			}
			if ( result == rUNDEF)
				throw ErrorObject("Undefined values in spectrum detected; Choose different probe points");
			finalResult[i] = result;
		}
		String path = IlwWinApp()->sGetCurDir() + name + ".spec";
		ofstream outfile(path.c_str(),std::ofstream::out);
		for(int i = 0; i < finalResult.size(); ++i) 
			outfile << "band" << (i + 1) << "=" << std::setprecision(5) << finalResult[i] << "\n";
		outfile.close();
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

void CrossSectionGraph::saveAsTbl() {
	if (values.size() == 0)
		return;
	String fname("CrossSection");
	if ( TableNameForm(this, &fname).DoModal() == IDOK) {
		int maxNo = values[0][0].size();
		FileName fnTable = FileName::fnUnique(FileName(fname,".tbt"));
		Table tbl(fnTable,Domain("none"));
		DomainValueRangeStruct dvInt(1, maxNo);
		Column colIndex = tbl->colNew("Index",dvInt);
		colIndex->SetOwnedByTable();
		//Column colMap = tbl->colNew("BaseMap",Domain("String"));
		//colMap->SetOwnedByTable();
		MapList mpl;
		ObjectCollection oc;
		int nMaps = 0;
		for(int i = 0; i < fldGraph->crdSelect.size(); ++i) {
			int maxmaps = 0;
			for(int m =0; m < fldGraph->sources.size(); ++m) {
				maxmaps = max(getNumberOfMaps(m), maxmaps);
			}
			nMaps = maxmaps;
		}
		tbl->iRecNew(nMaps);
		int shift = 0;
		for(int i = 0; i < fldGraph->crdSelect.size(); ++i) {
			for(int m =0; m < fldGraph->sources.size(); ++m) {
				int count = 0;
				Column colValue;
				IlwisObject obj = fldGraph->sources[m];
				String name =  String("%S", obj->fnObj.sFile);
				FileName fn = obj->fnObj;
				if ( IOTYPE(fn) == IlwisObject::iotMAPLIST) {
					mpl.SetPointer(obj.pointer());
					RangeReal rr = mpl->getRange();
					ValueRange vr( rr,0);
					colValue = tbl->colNew(String("%S_%d", name,i), Domain("value"),vr );
				} else if ( IOTYPE(fn) == IlwisObject::iotOBJECTCOLLECTION) {
					oc.SetPointer(obj.pointer());
					RangeReal rr = oc->getRange();
					ValueRange vr( rr,0);
					colValue = tbl->colNew(String("%S_%d", name,i), Domain("value"),vr );
				}

				colValue->SetOwnedByTable();
				int noMaps = getNumberOfMaps(m);
				for(long j = 0; j < noMaps; ++j) {
					BaseMap bmp;
					if ( mpl.fValid()) {
						bmp = mpl[j];
					} else if ( oc.fValid()) {
						IlwisObject objc = oc->ioObj(j);
						if ( IOTYPEBASEMAP(objc->fnObj))
							bmp = BaseMap(objc->fnObj);
					}
					if ( bmp.fValid()) {
						Coord crd = fldGraph->crdSelect[i];
						if ( bmp->cs() != fldGraph->csy)
							crd = bmp->cs()->cConv(fldGraph->csy, crd);
						String v = bmp->sValue(crd,0);
						colIndex->PutVal(count + 1, j + 1);
						colValue->PutVal(count + 1, v);
						//colMap->PutVal(count+1, bmp->fnObj.sFile + bmp->fnObj.sExt);
					}
					++count;
				}
			}
		}
		tbl->Store();
		IlwWinApp()->Execute(String("Open %S",fnTable.sRelative())); 
	}
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
	markerXposOld = iUNDEF;
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

	RangeReal rrTotal;
	String sTop;
	String sBottom;
	if (!yStretch) {
		for(int m =0; m < fldGraph->sources.size(); ++m) {
			rrTotal += fldGraph->getRange(m);
		}
		if (fldGraph->sources.size() > 0) {
			const DomainValueRangeStruct & dvrs = fldGraph->getDvrs(0);
			sTop = dvrs.sValue(rrTotal.rHi()).sTrimSpaces();
			sBottom = dvrs.sValue(rrTotal.rLo()).sTrimSpaces();
		}
	} else if (fldGraph->sources.size() == 1) {
		const DomainValueRangeStruct & dvrs = fldGraph->getDvrs(0);
		sTop = dvrs.sValue(fldGraph->getRange(0).rHi()).sTrimSpaces();
		sBottom = dvrs.sValue(fldGraph->getRange(0).rLo()).sTrimSpaces();
	}
	CDC *dc = CDC::FromHandle(lpDIS->hDC);
	CSize szTop = dc->GetTextExtent(sTop.c_str(), sTop.size());
	CSize szBottom = dc->GetTextExtent(sBottom.c_str(), sBottom.size());
	int iLeft = max(szTop.cx, szBottom.cx);
	rct = CRect(crct.left + (iLeft > 0 ? iLeft + 1 : 0), crct.top, crct.right, crct.bottom-20);

	Color bkColor = GetBkColor(lpDIS->hDC);
	CBrush bbrushBk(RGB(255, 255, 255));
	SelectObject(lpDIS->hDC, bbrushBk);
	::Rectangle(lpDIS->hDC, rct.left,rct.top, rct.right, rct.bottom);
	SelectObject(lpDIS->hDC, oldBrush);

	if ( fldGraph->crdSelect.size() == 0)
		return;

	CFont* fnt = new CFont();
	BOOL vvv = fnt->CreatePointFont(80,"Arial");
	HGDIOBJ fntOld = dc->SelectObject(fnt);

	if (!yStretch || fldGraph->sources.size() == 1) {
		dc->TextOut(0, rct.top,sTop.c_str(),sTop.size());
		dc->TextOut(0, rct.bottom - szBottom.cy / 2,sBottom.c_str(),sBottom.size());
	}

	int maxNr = -1;
	for(int m =0; m < min(fldGraph->sources.size(), values.size()); ++m) {
		int penStyle = m % 4;
		RangeReal rr = yStretch ? fldGraph->getRange(m) : rrTotal;
		int numberOfMaps = getNumberOfMaps(m);
		maxNr = max(maxNr,numberOfMaps);

		double yscale = rct.Height() / rr.rWidth();
		double y0 = rr.rWidth() * yscale;
		double xscale = (numberOfMaps > 1) ? (double)rct.Width() / (numberOfMaps - 1) : rct.Width();
		for(int p=0; p < min(fldGraph->crdSelect.size(), values[m].size()); ++p) {
			Color clr = Representation::clrPrimary(p == 0 ? 1 : p + 3);
			CPen bpen(penStyle, 1, clr);
			SelectObject(lpDIS->hDC, bpen);
			double rx = rct.left;
			for(int i = 0; i < min(numberOfMaps, values[m][p].size()); ++i) {
				double v = values[m][p][i];
				int y = y0 - ( v - rr.rLo()) * yscale;
				if ( i == 0)
					dc->MoveTo(round(rx),y);
				else 
					dc->LineTo(round(rx),y);
				rx += xscale;
			}
		}
	}

	CPen bpen(PS_SOLID, 1, RGB(150,150,150));
	SelectObject(lpDIS->hDC, bpen);

	double rx = rct.left;
	double xscale = (maxNr > 1) ? (double)rct.Width() / (maxNr - 1) : rct.Width();
	int modval = max(1, 1 + maxNr / 15); // max. 15 numbers will be drawn individually on the x-axis; a larger number will be drawn every 2, every 3 etc.
	for(int i = 0; i < maxNr; ++i) {
		if ( i % modval == 0) {
			String s("%d", i + 1);
			CSize sz = dc->GetTextExtent(s.c_str(), s.size());
			int xpos;
			if (i == 0)
				xpos = rct.left;
			else if (i == maxNr - 1)
				xpos = min(rx - sz.cx / 2, rct.right - sz.cx);
			else
				xpos = rx - sz.cx / 2;
			dc->TextOut(xpos, rct.bottom + 4,s.c_str(),s.size());
			if (rx > rct.right - 1)
				rx = rct.right - 1;
			dc->MoveTo(rx,rct.bottom);
			dc->LineTo(rx,rct.bottom + 3);
		}
		rx += xscale;
	}

	fldGraph->fillList();

	SelectObject(lpDIS->hDC,oldPen);
	SelectObject(lpDIS->hDC, oldBrush);
	SelectObject(lpDIS->hDC, fntOld);
	fnt->DeleteObject();
	delete fnt;
}

void CrossSectionGraph::recomputeValues() {
	values.clear();
	fldGraph->currentIndex.clear();
	if ( fldGraph->crdSelect.size() == 0)
		return;
	values.resize(fldGraph->sources.size());
	fldGraph->currentIndex.resize(fldGraph->sources.size());
	for(int m =0; m < fldGraph->sources.size(); ++m) {
		fldGraph->currentIndex[m] = iUNDEF;
		int numberOfMaps = getNumberOfMaps(m);
		values[m].resize(fldGraph->crdSelect.size());
		for(int p=0; p < fldGraph->crdSelect.size(); ++p) {
			for(int i = 0; i < numberOfMaps; ++i) {
				BaseMap bmp = getBaseMap(i, m);
				Coord crd = fldGraph->crdSelect[p];
				if ( bmp->cs() != fldGraph->csy)
					crd = bmp->cs()->cConv(fldGraph->csy, crd);
				double v = bmp->rValue(crd);
				values[m][p].push_back(v);
			}
		}
	}
}

void CrossSectionGraph::DrawMarker(int xposOld, int xpos) {
	CDC * pDC = GetDC();
	int dmOld = pDC->SetROP2(R2_XORPEN);
	CPen bpen(PS_SOLID, 1, RGB(55,55,55)); // the XOR color
	CPen* pOldPen = pDC->SelectObject(&bpen);
	if (xposOld != iUNDEF) {
		pDC->MoveTo(xposOld, rct.bottom - 2);
		pDC->LineTo(xposOld, rct.top);
	}
	if (xpos != iUNDEF) {
		pDC->MoveTo(xpos, rct.bottom - 2);
		pDC->LineTo(xpos, rct.top);
	}
	pDC->SelectObject(pOldPen);
	pDC->SetROP2(dmOld);
	ReleaseDC(pDC);
}

void CrossSectionGraph::OnLButtonDown(UINT nFlags, CPoint point) {	
	fDown = true;
	SetCapture();
	CWnd *wnd =  GetParent();
	if ( wnd && values.size() > 0) {
		point.x = min(rct.right - 1, max(rct.left, point.x));
		DrawMarker(markerXposOld, point.x);
		markerXposOld = point.x;
		double fract = (double)(point.x - rct.left) / (rct.Width() - 1);
		for(int m =0; m < fldGraph->sources.size(); ++m) {
			int numberOfMaps = getNumberOfMaps(m) - 1;
			fldGraph->currentIndex[m] = min(numberOfMaps, max(0, round(numberOfMaps * fract)));
			if ( fldGraph->currentIndex[m] >= values[m][0].size())
				continue;
		}
		fldGraph->fillList();
	}
}

void CrossSectionGraph::OnMouseMove(UINT nFlags, CPoint point) {	
	if (fDown) {
		CWnd *wnd =  GetParent();
		if ( wnd && values.size() > 0) {
			point.x = min(rct.right - 1, max(rct.left, point.x));
			DrawMarker(markerXposOld, point.x);
			markerXposOld = point.x;
			double fract = (double)(point.x - rct.left) / (rct.Width() - 1);
			for(int m =0; m < fldGraph->sources.size(); ++m) {
				int numberOfMaps = getNumberOfMaps(m) - 1;
				fldGraph->currentIndex[m] = min(numberOfMaps, max(0, round(numberOfMaps * fract)));
				if ( fldGraph->currentIndex[m] >= values[m][0].size())
					continue;
			}
			fldGraph->fillList();
		}
	}
}

void CrossSectionGraph::OnLButtonUp(UINT nFlags, CPoint point) {	
	fDown = false;
	CWnd *wnd =  GetParent();
	if ( wnd && values.size() > 0) {
		point.x = min(rct.right - 1, max(rct.left, point.x));
		DrawMarker(markerXposOld, point.x);
		markerXposOld = point.x;
		double fract = (double)(point.x - rct.left) / (rct.Width() - 1);
		for(int m =0; m < fldGraph->sources.size(); ++m) {
			int numberOfMaps = getNumberOfMaps(m) - 1;
			fldGraph->currentIndex[m] = min(numberOfMaps, max(0, round(numberOfMaps * fract)));
			if ( fldGraph->currentIndex[m] >= values[m][0].size())
				continue;
		}
		fldGraph->fillList();
	}
	ReleaseCapture();
}

void CrossSectionGraph::PreSubclassWindow() 
{
	EnableToolTips();

	CStatic::PreSubclassWindow();
}
//----------------------------------------------------
CrossSectionGraphEntry::CrossSectionGraphEntry(FormEntry* par, vector<IlwisObject>& _sources, const CoordSystem& cys, ILWIS::CrossSectionTool *t) :
FormEntry(par,0,true),
crossSectionGraph(0),
listview(0),
yStretchOnSamples(false),
sources(_sources),
csy(cys),
tool(t)
{
	psn->iMinWidth = psn->iWidth = CSGRPAH_SIZE;
	psn->iMinHeight = psn->iHeight = 250;
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

void CrossSectionGraphEntry::fillList() {
	listview->disableRedraw();
	listview->clear();
	listview->SetRowCount(0);
	if (crdSelect.size() == 0) {
		for(int m =0; m < sources.size(); ++m) {
			IlwisObject obj = sources[m];
			vector<String> v;
			v.push_back(String("%S%S",obj->fnObj.sFile,obj->fnObj.sExt));
			v.push_back(String(""));
			if ( IOTYPE(obj->fnObj) == IlwisObject::iotMAPLIST) {
				MapList mpl(obj->fnObj);
				v.push_back(String("%d:%d",mpl->iLower() + 1, mpl->iUpper() + 1));
			} else if (IOTYPE(obj->fnObj) == IlwisObject::iotOBJECTCOLLECTION) {
				ObjectCollection oc(obj->fnObj);
				v.push_back(String("1:%d",oc->iNrObjects()));
			}
			const DomainValueRangeStruct & dvrs = getDvrs(m);
			RangeReal rr = getRange(m);
			String range = dvrs.sValue(rr.rLo()).sTrimSpaces() + " : " + dvrs.sValue(rr.rHi()).sTrimSpaces();
			v.push_back(range);
			v.push_back("");
			v.push_back("");
			listview->AddData(v);
		}
	} else {
		int count = crdSelect.size();
		for(int m =0; m < sources.size(); ++m) {
			IlwisObject obj = sources[m];
			for(int i = 0 ; i < count; ++i) {
				vector<String> v;
				v.push_back(String("%S%S",obj->fnObj.sFile,obj->fnObj.sExt));
				v.push_back(String("%d",i + 1));
				if ( IOTYPE(obj->fnObj) == IlwisObject::iotMAPLIST) {
					MapList mpl(obj->fnObj);
					v.push_back(String("%d:%d",mpl->iLower() + 1, mpl->iUpper() + 1));
				} else if (IOTYPE(obj->fnObj) == IlwisObject::iotOBJECTCOLLECTION) {
					ObjectCollection oc(obj->fnObj);
					v.push_back(String("1:%d",oc->iNrObjects()));
				}
				const DomainValueRangeStruct & dvrs = getDvrs(m);
				RangeReal rr = getRange(m);
				String range = dvrs.sValue(rr.rLo()).sTrimSpaces() + " : " + dvrs.sValue(rr.rHi()).sTrimSpaces();
				v.push_back(range);
				if ( crossSectionGraph->values.size() > 0 && currentIndex[m] != iUNDEF) {
					v.push_back(String("%d", currentIndex[m] + 1));
					v.push_back(dvrs.sValue(crossSectionGraph->values[m][i][currentIndex[m]]).sTrimSpaces());
				}
				else{
					v.push_back("");
					v.push_back("");
				}
				listview->AddData(v);
			}
		}
	}
	listview->enableRedraw();
	listview->update();
}

bool CrossSectionGraphEntry::isUnique(const FileName& fn) {
	for(int i=0; i < sources.size(); ++i) {
		if ( sources.at(i)->fnObj == fn)
			return false;
	}
	return true;
}

void CrossSectionGraphEntry::update() {
	if ( crossSectionGraph)
		crossSectionGraph->Invalidate();
	if (crdSelect.size() == 0)
		fillList();
	listview->update();

}

void CrossSectionGraphEntry::reset() {
	crdSelect.clear();
	listview->clear();
	listview->SetRowCount(0);
}
//void CrossSectionGraphEntry::addSourceSet(const IlwisObject& obj){
//	//if ( isUnique(obj->fnObj)) {
//		
//		if (listview) {
//			int count = max(1, crdSelect.size());
//			for(int j = 0 ; j < count; ++j) {
//				sources.push_back(SourceInfo(obj, j));
//				vector<String> v;
//				v.push_back(String("%SS",obj->fnObj.sFile,obj->fnObj.sExt));
//				v.push_back(String("%d",j));
//				if ( IOTYPE(obj->fnObj) == IlwisObject::iotMAPLIST) {
//					MapList mpl(obj->fnObj);
//					v.push_back(String("%d:%d",mpl->iLower(), mpl->iUpper()));
//					RangeReal rr = getRange(sources.size() - 1);
//					v.push_back(String("%S", rr.s()));
//					v.push_back("?");
//					v.push_back("?");
//
//				} else if (IOTYPE(obj->fnObj) == IlwisObject::iotOBJECTCOLLECTION) {
//					ObjectCollection oc(obj->fnObj);
//				}
//				listview->AddData(v);
//			}
//		}
//		if ( crossSectionGraph)
//			crossSectionGraph->Invalidate();
////	}
//}

void CrossSectionGraphEntry::addCoord(const Coord& crd){
	if ( crd == Coord())
		return;

	crdSelect.push_back(crd);
	//int n = sources.size() / crdSelect.size();
	//for(int i=0; i < crdSelect.size(); ++i) {
	//	if ( i != 0) {
	//		int groupStart = n * crdSelect.size();
	//		if (i + groupStart < sources.size())
	//			sources.insert(sources.begin() + i + groupStart, SourceInfo(sources[i * n].obj, crdSelect.size()));
	//		else
	//			sources.push_back(SourceInfo(sources[i * n].obj, crdSelect.size()));
	//	}
	//	update();
	//}
	if ( crossSectionGraph) {
		crossSectionGraph->recomputeValues();
		crossSectionGraph->Invalidate();
	} else
		fillList();
}

void CrossSectionGraphEntry::setLastCoord(const Coord& crd){
	if ( crd == Coord())
		return;

	int sz = crdSelect.size();
	if (sz > 0)
		crdSelect[sz - 1] = crd;
	//int n = sources.size() / crdSelect.size();
	//for(int i=0; i < crdSelect.size(); ++i) {
	//	if ( i != 0) {
	//		int groupStart = n * crdSelect.size();
	//		if (i + groupStart < sources.size())
	//			sources.insert(sources.begin() + i + groupStart, SourceInfo(sources[i * n].obj, crdSelect.size()));
	//		else
	//			sources.push_back(SourceInfo(sources[i * n].obj, crdSelect.size()));
	//	}
	//	update();
	//}
	if ( crossSectionGraph) {
		crossSectionGraph->recomputeValues();
		crossSectionGraph->Invalidate();
	} else
		fillList();
}

void CrossSectionGraphEntry::setYStretch(bool stretch) {
	if (crossSectionGraph) {
		crossSectionGraph->yStretch = stretch;
		crossSectionGraph->Invalidate();
	}
}

void CrossSectionGraphEntry::setYStretchOnSamples(bool stretch) {
	overruleRanges.clear();
	yStretchOnSamples = stretch;
	if (crossSectionGraph) {
		crossSectionGraph->Invalidate();
	}
}

void CrossSectionGraphEntry::setListView(FieldListView *v) {
	listview = v;
	v->psn->iMinWidth = v->psn->iWidth = CSGRPAH_SIZE;
	v->psn->iMinHeight = v->psn->iHeight = 100;
}


RangeReal CrossSectionGraphEntry::getRange(long i) {
	if (i < overruleRanges.size()) {
		RangeReal rr = overruleRanges[i];
		if ( rr.fValid())
			return rr;
	}

	if (yStretchOnSamples) { // from the "values" array
		RangeReal rr;
		if (crossSectionGraph && crossSectionGraph->values.size() > i) {
			for (int j = 0; j < crossSectionGraph->values[i].size(); ++j) {
				for (int k = 0; k < crossSectionGraph->values[i][j].size(); ++k)
					rr += crossSectionGraph->values[i][j][k];
			}
		}
		return rr;
	} else { // from the entire maps
		IlwisObject obj = sources[i];
		if ( IOTYPE(obj->fnObj) == IlwisObject::iotMAPLIST) {
			if ( ranges.size() <= i) {
				MapList mpl(obj->fnObj);
				RangeReal rr = getRange(mpl);
				DomainValue *pdv =  mpl[i]->dm()->pdv();
			/*	if ( pdv ) {
					if ( !pdv->fSystemObject()) {
						RangeReal rr1 = pdv->rrMinMax();
						if ( rr.rWidth() < 1e8)
							rr = rr1;
					}
				}*/
				ranges.push_back(rr);
			}
		}
		else if ( IOTYPE(obj->fnObj) == IlwisObject::iotOBJECTCOLLECTION) {
			if ( ranges.size() <= i) {
				ObjectCollection oc(obj->fnObj);
				ranges.push_back(oc->getRange());
			}
		}
	}
	return ranges[i];
}

const RangeReal CrossSectionGraphEntry::getRange(const MapList& mpl) const {
	RangeReal rrMinMax (0, 255);
	if (mpl.fValid()) {
		Domain dm = mpl->dm();
		if (dm.fValid() && (dm->pdbit() || dm->pdbool()))
			rrMinMax = RangeReal(1,2);
		else if ( dm->pdv()) {
			for (int i = 0; i < mpl->iSize(); ++i) {
				RangeReal rrMinMaxMap = mpl->map(i)->rrMinMax(BaseMapPtr::mmmCALCULATE);
				if (rrMinMaxMap.rLo() > rrMinMaxMap.rHi())
					rrMinMaxMap = mpl->map(i)->vr()->rrMinMax();
				if (i > 0)
					rrMinMax += rrMinMaxMap;
				else
					rrMinMax = rrMinMaxMap;
			}
		}
	}
	return rrMinMax;
}

void CrossSectionGraphEntry::setOverruleRange(int row, int col, const String& value) {
	RangeReal rr(value);
	if ( rr.fValid()) {
		if ( row < tool->sources.size() ) {
			tool->setCustomRange();
			overruleRanges[row] = rr;
			crossSectionGraph->Invalidate();
		}
	}
}

void CrossSectionGraphEntry::setCustomRange() {
	if (overruleRanges.size() < tool->sources.size())
		overruleRanges.resize(tool->sources.size());
	for (int i = 0; i < tool->sources.size(); ++i)
		overruleRanges[i] = getRange(i);
}

const DomainValueRangeStruct & CrossSectionGraphEntry::getDvrs(int i) const{
	IlwisObject obj = sources[i];
	if ( IOTYPE(obj->fnObj) == IlwisObject::iotMAPLIST) {
		MapList mpl(obj->fnObj);
		return mpl[0]->dvrs();
	}
	else if ( IOTYPE(obj->fnObj) == IlwisObject::iotOBJECTCOLLECTION) {
		ObjectCollection oc(obj->fnObj);
		BaseMap bmp(oc->fnObject(0));
		return bmp->dvrs();
	}
	else
		return BaseMap(obj->fnObj)->dvrs(); // shouldn't happen; we only have maplists and objectcollections
}

#define ID_ADD_ITEMS 5103
#define ID_REMOVE_ITEMS 5104

void CrossSectionGraphEntry::onContextMenu(CWnd* pWnd, CPoint point) {
	vector<int> rows;
	if (listview)
		listview->getSelectedRowNumbers(rows);
	CMenu men;
	men.CreatePopupMenu();
	men.AppendMenu(MF_STRING, ID_ADD_ITEMS, TR("Add").c_str());
	men.AppendMenu(MF_STRING | (rows.size() > 0) ? 0 : MF_GRAYED, ID_REMOVE_ITEMS, TR("Remove").c_str());
	int cmd = men.TrackPopupMenu(TPM_LEFTALIGN|TPM_RIGHTBUTTON|TPM_RETURNCMD, point.x, point.y, pWnd);
	switch (cmd) {
		case ID_ADD_ITEMS:
			tool->displayOptionAddList();
			break;
		case ID_REMOVE_ITEMS:
			{
				int count = max(1, crdSelect.size());
				for (vector<int>::reverse_iterator it = rows.rbegin(); it != rows.rend(); ++it) {
					int sourceNr = *it / count;
					for (int j = sourceNr * count - 1; j >= sourceNr * count; --j)
						listview->RemoveData(j);
					sources.erase(sources.begin() + sourceNr);
					ranges.erase(ranges.begin() + sourceNr);
					if (sourceNr < overruleRanges.size())
						overruleRanges.erase(overruleRanges.begin() + sourceNr);
				}
				if (crossSectionGraph)
					crossSectionGraph->recomputeValues();
				update();
				tool->updateCbStretch();
			}
			break;
	}
}
