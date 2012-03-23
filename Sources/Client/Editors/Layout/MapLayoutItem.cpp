/***************************************************************
 ILWIS integrates image, vector and thematic data in one unique 
 and powerful package on the desktop. ILWIS delivers a wide 
 range of feautures including import/export, digitizing, editing, 
 analysis and display of data as well as production of 
 quality mapsinformation about the sensor mounting platform
 
 Exclusive rights of use by 52°North Initiative for Geospatial 
 Open Source Software GmbH 2007, Germany

 Copyright (C) 2007 by 52°North Initiative for Geospatial
 Open Source Software GmbH

 Author: Jan Hendrikse, Willem Nieuwenhuis,Wim Koolhoven 
 Bas Restsios, Martin Schouwenburg, Lichun Wang, Jelle Wind 

 Contact: Martin Schouwenburg; schouwenburg@itc.nl; 
 tel +31-534874371

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 version 2 as published by the Free Software Foundation.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program (see gnu-gpl v2.txt); if not, write to
 the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 Boston, MA 02111-1307, USA or visit the web page of the Free
 Software Foundation, http://www.fsf.org.

 Created on: 2007-02-8
 ***************************************************************/
// MapLayoutItem.cpp: implementation of the MapLayoutItem class.
//
//////////////////////////////////////////////////////////////////////

#include "Client\Headers\formelementspch.h"
#include "Client\Editors\Layout\LayoutItem.h"
#include "Client\Editors\Layout\MapLayoutItem.h"
#include "Client\ilwis.h"
#include "Client\Base\datawind.h"
#include "Client\Base\IlwisDocument.h"
#include "Client\MainWindow\Catalog\CatalogDocument.h"
#include "Engine\Map\Mapview.h"
#include "Client\Mapwindow\MapCompositionDoc.h"
#include "Client\Mapwindow\MapCompositionSrvItem.h"
#include "Headers\constant.h"
#include "Client\Editors\Layout\ScaleTextLayoutItem.h"
#include "Client\Editors\Layout\ScaleBarLayoutItem.h"
#include "Client\Editors\Layout\NorthArrowLayoutItem.h"
#include "Client\Editors\Layout\MapBorderItem.h"
#include "Client\Editors\Layout\LayoutDoc.h"
#include "Headers\Hs\Layout.hs"
#include "Headers\Hs\Mapwind.hs"
#include "Client\Mapwindow\ZoomableView.h"
#include "Client\Mapwindow\AreaSelector.h"
#include "Client\Mapwindow\PanTool.h"
#include "Client\Mapwindow\LayerTreeView.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

BEGIN_MESSAGE_MAP(MapLayoutItem, LayoutItem)
	//{{AFX_MSG_MAP(MapLayoutItem)
	ON_COMMAND(ID_ADD_SCALETEXT, OnAddScaleText)
	ON_COMMAND(ID_ADD_SCALEBAR, OnAddScaleBar)
	ON_COMMAND(ID_ADD_NORTHARROW, OnAddNorthArrow)
	ON_COMMAND(ID_ADD_LEGEND, OnAddLegend)
	ON_COMMAND(ID_ADD_MAPBORDER, OnAddMapBorder)
	ON_COMMAND(ID_ENTIREMAP, OnEntireMap)
	ON_COMMAND(ID_ITEM_REPLACEMAPVIEW, OnItemReplaceMapView)
	ON_COMMAND(ID_REDRAW, OnRedraw)
	ON_COMMAND(ID_SET_SCALE, OnSetScale) 
	ON_UPDATE_COMMAND_UI(ID_ADD_SCALETEXT, OnUpdateValidScale)
	ON_UPDATE_COMMAND_UI(ID_ADD_SCALEBAR, OnUpdateValidScale)
	ON_UPDATE_COMMAND_UI(ID_SET_SCALE, OnUpdateValidScale)
	ON_UPDATE_COMMAND_UI(ID_ADD_NORTHARROW, OnUpdateValidGeoRef)
	ON_UPDATE_COMMAND_UI(ID_ADD_MAPBORDER, OnUpdateValidGeoRef)
	ON_COMMAND(ID_KEEPSCALE, OnKeepScale)
	ON_UPDATE_COMMAND_UI(ID_KEEPSCALE, OnUpdateKeepScale)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


class MapLayoutItemSetScaleForm : public FormWithDest
{
public:
  MapLayoutItemSetScaleForm(CWnd* wnd, double* rScale) 
		: FormWithDest(wnd, TR("Set Scale"))
  {
			new FieldReal(root, TR("&Scale 1:      "), rScale, ValueRange(0,1e9));
//      SetMenHelpTopic(htpCnfAnnMap);
      create();
  }
};

MapLayoutItem::MapLayoutItem(LayoutDoc* ld, MapCompositionDoc* mcd)
	: LayoutItem(ld)
	, fCreatedLayerTreeBar(false)
	, hMF(0)
	, fKeepScale(true)
{
	mcsi = mcd->GetEmbeddedItem();
	mcsi->ExternalAddRef();
	Init();
	double rSc = rScale();
	if (rSc > 0)
	{
/*		CSize size, size2;
		mcsi->OnGetExtent(DVASPECT_CONTENT, size);
		size2 = ld->szGetPrintableSize();
		if ( size.cx != 0 && size.cy != 0 )
		{
			double rXF = (double)size.cx / size2.cx;
			double rYF = (double)size.cy / size2.cy;
			double rScY = rUNDEF, rScX = rUNDEF;
			if ( rXF > 1 ) rScX = rSc * rXF;
			if ( rYF > 1 ) rScY = rSc * rYF;
			if ( rScX > 0 || rScY > 0 )
				rSc = max(rScX, rScY);
		}*/
		MapLayoutItemSetScaleForm	frm(ld->wndGetActiveView(), &rSc);
		if (frm.fOkClicked())
			SetScale(rSc);
		else
			DummyError();
	}
}

MapLayoutItem::~MapLayoutItem()
{
	if (!fCreatedLayerTreeBar) 
		mcsi->ExternalRelease();
	DeleteMetaFile(hMF);	
	// prevent that grey area from LayerTreeBar remains
	if (ltb && IsWindow(ltb.m_hWnd)) {
		CFrameWnd* fw = ltb.GetParentFrame();
		if (fw && IsWindow(fw->m_hWnd)) {
			fw->ShowControlBar(&ltb,FALSE,FALSE);
			fw->RecalcLayout();
		}
	}
}

void MapLayoutItem::Init()
{
	fBusyDrawing = false;
	m_rScale = mcd()->rPrefScale();
	CSize szMF(0, 0);
	mcsi->OnGetExtent(DVASPECT_CONTENT, szMF);
	if (hMF)
		DeleteMetaFile(hMF);
	hMF = 0;
//	InitMetafile();
	fnMapView = mcd()->GetPathName();

	// convert from 0.01 to 0.1 mm units
	szMF.cx /= 10;
	szMF.cy /= 10;
	CRect rectMF(0,0,szMF.cx,szMF.cy);
	MinMax mm = mmPosition();
	mm.MaxCol() = mm.MinCol() + szMF.cx;
	mm.MaxRow() = mm.MinRow() + szMF.cy;
	LayoutItem::SetPosition(mm,-1);
	fInitialized = true;
}


int CALLBACK EnumMetafileProc( HDC hDC,
                                  HANDLETABLE *lpHTable,
                                  METARECORD *lpMFR,
                                  int nObj, LPARAM lpData )
{
		int iR = PlayMetaFileRecord(hDC, lpHTable, lpMFR, nObj);
    return 1;
} 


void MapLayoutItem::OnDraw(CDC* cdc)
{
	if (0 == hMF) {
		InitMetafile();
		fInitialized = true;
	}
	if (hMF) 
	{
		if ( fBusyDrawing )
			return;
		fBusyDrawing = true;
		TRACE("entering Thread ID = 0x%X\n", AfxGetThread()->m_nThreadID);
	/*	if ( cdc->IsPrinting()) // odd, if this is not done only the first band of a sufficiently
			                      //large rastermap will print. printing will take longer but it prints
			EnumMetaFile(*cdc, hMF, EnumMetafileProc, NULL);*/
		cdc->PlayMetaFile(hMF);
		TRACE("leaving Thread ID = 0x%X\n", AfxGetThread()->m_nThreadID);	
		fBusyDrawing = false;
	}
}

void MapLayoutItem::SetPosition(MinMax mm, int iHit)
{
	throw ErrorObject(String("To Be Done %d %s", __LINE__, __FILE__));
	//MinMax mmOld = mmPosition();
	//if (fKeepScale) {
	//	int iDiffWidth = mm.width() - mmOld.width();
	//	int iDiffHeight = mm.height() - mmOld.height();
	//	if (iDiffWidth == 0 && iDiffHeight == 0) 
	//	{
	//		LayoutItem::SetPosition(mm, iHit);
	//		return;
	//	}

	//	MapCompositionDoc* mcd = mcsi->GetDocument();
	//	if (0 == mcd ) 
	//		return;
	//	double rPixSize = mcd->georef->rPixSize();
	//	MinMax mmBnds = mcd->mmBounds();
	//	RowCol rcMaxSize(mmBnds.height(), mmBnds.width());
	//	RowCol rcOffset = mcsi->rcOffset();
	//	RowCol rcSize; 
	//	double rScale = m_rScale;
	//	if (rUNDEF == rPixSize || rScale <= 0) 
	//	{
	//		LayoutItem::SetPosition(mm, iHit);
	//		return;
	//	}
	//	rScale /= 10000; // 0.1 mm units per m
	//	rcSize.Row = rounding(mm.height() / rPixSize * rScale);
	//	rcSize.Col = rounding(mm.width() / rPixSize * rScale);
	//	if (rcSize.Row > rcMaxSize.Row)
	//		rcSize.Row = rcMaxSize.Row;
	//	if (rcSize.Col > rcMaxSize.Col)
	//		rcSize.Col = rcMaxSize.Col;

	//	if (iHit == CRectTracker::hitLeft ||
	//	    iHit == CRectTracker::hitTopLeft ||
	//	    iHit == CRectTracker::hitBottomLeft)
	//	{
	//		int iDiffCol = rounding(iDiffWidth / rPixSize * rScale);
	//		rcOffset.Col -= iDiffCol;
	//	}

	//	if (iHit == CRectTracker::hitTop ||
	//	    iHit == CRectTracker::hitTopLeft ||
	//	    iHit == CRectTracker::hitTopRight)
	//	{
	//		int iDiffRow = rounding(iDiffHeight / rPixSize * rScale);
	//		rcOffset.Row -= iDiffRow;
	//	}

	//	// if (rcOffset.Row + rcSize.Row > rcMaxSize.Row)
	//	// 	rcOffset.Row = rcMaxSize.Row - rcSize.Row;
	//	// if (rcOffset.Col + rcSize.Col > rcMaxSize.Col)
	//	// 	rcOffset.Col = rcMaxSize.Col - rcSize.Col;

	//	// guard against invalid offset
	//	if (rcOffset.Row + rcSize.Row > mmBnds.MaxRow())
	//		rcOffset.Row = mmBnds.MaxRow() - rcSize.Row;
	//	if (rcOffset.Row < mmBnds.MinRow())
	//		rcOffset.Row = mmBnds.MinRow();
	//	if (rcOffset.Col + rcSize.Col > mmBnds.MaxCol())
	//		rcOffset.Col = mmBnds.MaxCol() - rcSize.Col;
	//	if (rcOffset.Col < mmBnds.MinCol())
	//		rcOffset.Col = mmBnds.MinCol();

	//	mcsi->SetOffsetSize(rcOffset, rcSize);

	//	double rWidth = rcSize.Col * rPixSize / rScale;
	//	double rHeight = rcSize.Row * rPixSize / rScale;

	//	mm.MaxCol() = mm.MinCol() + rounding(rWidth);
	//	mm.MaxRow() = mm.MinRow() + rounding(rHeight);

	//	LayoutItem::SetPosition(mm, iHit);
	//	InitMetafile();
	//}
	//else {
	//	LayoutItem::SetPosition(mm, iHit);
	//	// on move do not change scale
	//	if (iHit != CRectTracker::hitMiddle && mmOld.width() != mm.width())
	//		InitScale();
	//}
}

bool MapLayoutItem::fIsotropic() const
{
	if (fKeepScale)
		return false;
	else
		return true;
}

double MapLayoutItem::rHeightFact() const
{
	RowCol rcSize = mcsi->rcSize();
	return double(rcSize.Row) / rcSize.Col;
}


double MapLayoutItem::rScale() const
{
	return m_rScale;
}

void MapLayoutItem::InitScale()
{
	throw ErrorObject(String("To Be Done %d %s", __LINE__, __FILE__));
	//MapCompositionDoc* mcd = mcsi->GetDocument();
	//if (0 == mcd || !mcd->georef.fValid())
	//	return;
	//double rPixSize = mcd->georef->rPixSize();
	//if (rUNDEF == rPixSize)
	//	return;
	//RowCol rcSize = mcsi->rcSize();
	//MinMax mm = mmPosition();
	//m_rScale = rcSize.Col * rPixSize / mm.width();
	//m_rScale *= 10000; // 0.1 mm units per m
}

void MapLayoutItem::SetScale(double rScale)
{
	throw ErrorObject(String("To Be Done %d %s", __LINE__, __FILE__));
	//MapCompositionDoc* mcd = mcsi->GetDocument();
	//if (0 == mcd || !mcd->georef.fValid())
	//	return;
	//double rPixSize = mcd->georef->rPixSize();
	//if (rUNDEF == rPixSize)
	//	return;
	//mcd->SetScale(rScale);
	//m_rScale = rScale;
	//RowCol rcSize = mcsi->rcSize();
	//MinMax mm = mmPosition();
	//
	//rScale /= 10000; // 0.1 mm units per m
	//double rWidth = ( rcSize.Col - mcd->rcPrefOffset().Col) * rPixSize / rScale;
	//double rHeight = ( rcSize.Row - mcd->rcPrefOffset().Row ) * rPixSize / rScale;

	//mm.MaxCol() = mm.MinCol() + rounding(rWidth);
	//mm.MaxRow() = mm.MinRow() + rounding(rHeight);
	//LayoutItem::SetPosition(mm,-1);
	//ld->UpdateAllViews(0, LayoutDoc::hintITEM, this);
}

double MapLayoutItem::rAzimuth() const
{
	throw ErrorObject(String("To Be Done %d %s", __LINE__, __FILE__));
	//MapCompositionDoc* mcd = mcsi->GetDocument();
	//if (0 == mcd || !mcd->georef.fValid())
	//	return rUNDEF;
	//GeoRef grf = mcd->georef;
	//if (grf->fNorthOriented())
	//	return 0;
	//RowCol rc = grf->rcSize();
	//rc.Row /= 2;
	//rc.Col /= 2;
	//Coord c1 = grf->cConv(rc);
	//rc.Row += 1;
	//Coord c2 = grf->cConv(rc);
	//double rDX = c2.x - c1.x;
	//double rDY = c2.y - c1.y;
	//double rAz = atan2(rDX, -rDY);
	//if (rAz == 0 && rDY < 0)
	//	rAz = M_PI_2;
	//return rAz;
}

void MapLayoutItem::OnAddScaleText()
{
	ScaleTextLayoutItem* slo = new ScaleTextLayoutItem(ld, this);
	if (slo->fConfigure())
		ld->AddItem(slo);
	else
		delete slo;
}

void MapLayoutItem::OnAddScaleBar()
{
	ScaleBarLayoutItem* slo = new ScaleBarLayoutItem(ld, this);
	if (slo->fConfigure())
		ld->AddItem(slo);
	else
		delete slo;
}

void MapLayoutItem::OnAddNorthArrow()
{
	NorthArrowLayoutItem* nli = new NorthArrowLayoutItem(ld, this);
	if (nli->fConfigure())
		ld->AddItem(nli);
	else
		delete nli;
}

void MapLayoutItem::OnAddLegend()
{
	//LegendLayoutItem* lli = new LegendLayoutItem(ld, this);
	//if (lli->fConfigure())
	//	ld->AddItem(lli);
	//else
	//	delete lli;
}

void MapLayoutItem::OnEntireMap()
{
	bool fSaveKeepScale = fKeepScale;
	fKeepScale = false;
	mcsi->ResetOffsetAndSize();
	InitScale();
	SetPosition(mmPosition(), -1);
	InitMetafile();
	fKeepScale = fSaveKeepScale;
	ld->SetModifiedFlag();
	ld->UpdateAllViews(0, LayoutDoc::hintITEM, this);
}

bool MapLayoutItem::fClip() const
{
	return true;
}

String MapLayoutItem::sType() const
{
	return "MapView";
}

void MapLayoutItem::ReadElements(ElementContainer& en, const char* sSection)
{
	LayoutItem::ReadElements(en, sSection);
	String sMapView;
	ObjectInfo::ReadElement(sSection, "MapView", en, sMapView);
	ObjectInfo::ReadElement(sSection, "Scale", en, m_rScale);
	RowCol rcOffset;
	RowCol rcSize;
	ObjectInfo::ReadElement(sSection, "Offset", en, rcOffset);
	ObjectInfo::ReadElement(sSection, "Size", en, rcSize);
	ObjectInfo::ReadElement(sSection, "Keep Scale", en, fKeepScale);
	fnMapView = sMapView;
	String sOldDir = IlwWinApp()->sGetCurDir();
	IlwWinApp()->SetCurDir(fnMapView.sPath());
	mcd()->OnOpenDocument(sMapView.c_str());
	IlwWinApp()->SetCurDir(sOldDir);
	if (m_rScale <= 0)
		InitScale();
	mcd()->SetScale(m_rScale);
	mcsi->SetOffsetSize(rcOffset, rcSize);
}

void MapLayoutItem::WriteElements(ElementContainer& en, const char* sSection)
{
	LayoutItem::WriteElements(en, sSection);
	CString str = mcsi->GetDocument()->GetPathName();
	FileName fn(str);
	ObjectInfo::WriteElement(sSection, "MapView", en, fn);
	ObjectInfo::WriteElement(sSection, "Scale", en, m_rScale);
	RowCol rcOffset = mcsi->rcOffset();
	RowCol rcSize = mcsi->rcSize();
	ObjectInfo::WriteElement(sSection, "Offset", en, rcOffset);
	ObjectInfo::WriteElement(sSection, "Size", en, rcSize);
	ObjectInfo::WriteElement(sSection, "Keep Scale", en, fKeepScale);
}

MapCompositionDoc* MapLayoutItem::mcd()	
{
	return mcsi->GetDocument();
}

const MapCompositionDoc* MapLayoutItem::mcd()	const
{
	return mcsi->GetDocument();
}

String MapLayoutItem::sName() const
{
	if (0 == mcsi)
		return TR("Map View");
	else
		return String(TR("Map View %S").c_str(), fnMapView.sFile);
}

void MapLayoutItem::InitMetafile()
{
	CWaitCursor wc;
	if (hMF)
		DeleteMetaFile(hMF);
	hMF = 0;

	CSize szMF(0, 0);
	mcsi->OnGetExtent(DVASPECT_CONTENT, szMF);
	CClientDC dc(ld->wndGetActiveView());
	CMetaFileDC dcMF;
	if (!dcMF.Create())
		return;
	dcMF.SetAttribDC(dc.GetSafeHdc());
	if (mcsi->OnDraw(&dcMF, szMF)) 
		hMF = dcMF.Close();
}

void MapLayoutItem::OnAddMapBorder()
{
	MapBorderItem* mbi = new MapBorderItem(ld, this);
	if (mbi->fConfigure())
		ld->AddItem(mbi);
	else
		delete mbi;
}

CoordBounds MapLayoutItem::cbBounds() const
{
	return mcd()->rootDrawer->getCoordinateSystem()->cb;
}

Coord MapLayoutItem::cConv(CPoint pt) const
{
	throw ErrorObject(String("To Be Done %d %s", __LINE__, __FILE__));
	//RowCol rcSize = mcsi->rcSize();
	//RowCol rcOffset = mcsi->rcOffset();
	//MinMax mm = mmPosition();
	//double rX = pt.x - mm.MinCol();
	//double rY = pt.y - mm.MinRow();
	//rX /= mm.width();
	//rY /= mm.height();
	//rX *= rcSize.Col;
	//rY *= rcSize.Row;
	//rX += rcOffset.Col;
	//rY += rcOffset.Row;
	//Coord crd;
	//mcd()->georef->RowCol2Coord(rY,rX,crd);
	return Coord();
}
												
LatLon MapLayoutItem::llConv(const Coord& crd) const
{
	return mcd()->rootDrawer->getCoordinateSystem()->llConv(crd);
}

LatLon MapLayoutItem::llConv(CPoint pt) const	// pt in 0.1 mm units
{
	Coord crd = cConv(pt);
	return mcd()->rootDrawer->getCoordinateSystem()->llConv(crd);
}

CPoint MapLayoutItem::ptConv(double rRow, double rCol) const
{
	throw ErrorObject(String("To Be Done %d %s", __LINE__, __FILE__));
	/*RowCol rcSize = mcsi->rcSize();
	RowCol rcOffset = mcsi->rcOffset();
	MinMax mm = mmPosition();
	rRow -= rcOffset.Row;
	rCol -= rcOffset.Col;
	rRow /= rcSize.Row;
	rCol /= rcSize.Col;
	rRow *= mm.height();
	rCol *= mm.width();
	rRow += mm.MinRow();
	rCol += mm.MinCol();*/
	return CPoint(rounding(rCol), rounding(rRow));
}

struct ValFinder
{
	ValFinder(const GeoRef& georef, const Coord& coord, bool fXcoord, bool fRow, double rGoalVal) 
		: grf(georef), crd(coord), fX(fXcoord), fFindRow(fRow), rGoal(rGoalVal)
		{}
	virtual double operator()(double rX)
	{
		if (fX)
			crd.x = rX;
		else
			crd.y = rX;
		return rCalc();
	}
	double rCalc() 
	{
		double rRow, rCol;
		grf->Coord2RowCol(crd, rRow, rCol);
		if (fFindRow) {
			rVal = rRow;
			return rCol - rGoal;
		}
		else {
			rVal = rCol;
			return rRow - rGoal;
		}
	}
	double rValue() const 
		{ return rVal; }
protected:
	bool fX, fFindRow;
	Coord crd;
	GeoRef grf;
	double rGoal;
	double rVal;
};

struct ValFinderLatLon: public ValFinder
{
	ValFinderLatLon(const GeoRef& georef, const LatLon& latlon, bool fXcoord, bool fRow, double rGoalVal) 
		: ValFinder(georef, Coord(), fXcoord, fRow, rGoalVal)
		, ll(latlon)
		{}
	virtual double operator()(double rX) 
	{
		if (fX)
			ll.Lat = rX / 1e6;
		else
			ll.Lon = rX / 1e6;
		crd = grf->cs()->cConv(ll);
		return rCalc();
	}
protected:
	LatLon ll;
};

// this iterative function can probably be optimized
double rFindNull(ValFinder& vf, double rDflt)
{
	double rX = rDflt;
	double rStep = 1e100;
	int iIters = 0;
	while (true) {
		double rY1 = vf(rX+1);
		double rY = vf(rX);
		double rDY = (rY1 - rY);
		if (abs(rDY) < 1e-100) // y does not depend on x ???
			return rUNDEF;
		double rDX = rY / rDY;
		if (abs(rDX) < 1e-6 || ++iIters > 10)
			return vf.rValue();
		if (abs(rDX) > abs(rStep) * 0.6) {
			rDX = sign(rDX) * abs(rStep) / 2;
			rStep *= 0.9;
		}
		else {
			rStep = rDX;
		}
		rX -= rDX;
	}
}

CPoint MapLayoutItem::ptBorderX(Side side, double rX) const
{
	throw ErrorObject(String("To Be Done %d %s", __LINE__, __FILE__));
	/*GeoRef grf = mcd()->georef;
	RowCol rcSize = mcsi->rcSize();
	RowCol rcOffset = mcsi->rcOffset();
	RowCol rc;
	rc.Row = rcOffset.Row + rcSize.Row/2;
	rc.Col = rcOffset.Col + rcSize.Col/2;
	Coord crd = grf->cConv(rc);
	crd.x = rX;
	double r;

	switch (side)
	{
		case sideTOP: 
			r = rFindNull(ValFinder(grf,crd,false,false,rcOffset.Row), crd.y);
			return ptConv(rcOffset.Row,r);
		case sideBOTTOM:
			r = rFindNull(ValFinder(grf,crd,false,false,rcOffset.Row+rcSize.Row), crd.y);
			return ptConv(rcOffset.Row+rcSize.Row,r);
		case sideLEFT:
			r = rFindNull(ValFinder(grf,crd,false,true,rcOffset.Col), crd.y);
			return ptConv(r,rcOffset.Col);
		case sideRIGHT:
			r = rFindNull(ValFinder(grf,crd,false,true,rcOffset.Col+rcSize.Col), crd.y);
			return ptConv(r,rcOffset.Col+rcSize.Col);
	}*/
	return CPoint(0,0);
}

CPoint MapLayoutItem::ptBorderY(Side side, double rY) const
{
	throw ErrorObject(String("To Be Done %d %s", __LINE__, __FILE__));
	//GeoRef grf = mcd()->georef;
	//RowCol rcSize = mcsi->rcSize();
	//RowCol rcOffset = mcsi->rcOffset();
	//RowCol rc;
	//rc.Row = rcOffset.Row + rcSize.Row/2;
	//rc.Col = rcOffset.Col + rcSize.Col/2;
	//Coord crd = grf->cConv(rc);
	//crd.y = rY;
	//double r;

	//switch (side)
	//{
	//	case sideTOP: 
	//		r = rFindNull(ValFinder(grf,crd,true,false,rcOffset.Row), crd.x);
	//		return ptConv(rcOffset.Row,r);
	//	case sideBOTTOM:
	//		r = rFindNull(ValFinder(grf,crd,true,false,rcOffset.Row+rcSize.Row), crd.x);
	//		return ptConv(rcOffset.Row+rcSize.Row,r);
	//	case sideLEFT:
	//		r = rFindNull(ValFinder(grf,crd,true,true,rcOffset.Col), crd.x);
	//		return ptConv(r,rcOffset.Col);
	//	case sideRIGHT:
	//		r = rFindNull(ValFinder(grf,crd,true,true,rcOffset.Col+rcSize.Col), crd.x);
	//		return ptConv(r,rcOffset.Col+rcSize.Col);
	//}
	return CPoint(0,0);
}

CPoint MapLayoutItem::ptBorderLat(Side side, double rLat) const
{
	throw ErrorObject(String("To Be Done %d %s", __LINE__, __FILE__));
	//GeoRef grf = mcd()->georef;
	//RowCol rcSize = mcsi->rcSize();
	//RowCol rcOffset = mcsi->rcOffset();
	//RowCol rc;
	//rc.Row = rcOffset.Row + rcSize.Row/2;
	//rc.Col = rcOffset.Col + rcSize.Col/2;
	//Coord crd = grf->cConv(rc);
	//LatLon ll = grf->cs()->llConv(crd);
	//ll.Lat = rLat;
	//double r;

	//switch (side)
	//{
	//	case sideTOP: 
	//		r = rFindNull(ValFinderLatLon(grf,ll,false,false,rcOffset.Row), ll.Lon*1e6);
	//		return ptConv(rcOffset.Row,r);
	//	case sideBOTTOM:
	//		r = rFindNull(ValFinderLatLon(grf,ll,false,false,rcOffset.Row+rcSize.Row), ll.Lon*1e6);
	//		return ptConv(rcOffset.Row+rcSize.Row,r);
	//	case sideLEFT:
	//		r = rFindNull(ValFinderLatLon(grf,ll,false,true,rcOffset.Col), ll.Lon*1e6);
	//		return ptConv(r,rcOffset.Col);
	//	case sideRIGHT:
	//		r = rFindNull(ValFinderLatLon(grf,ll,false,true,rcOffset.Col+rcSize.Col), ll.Lon*1e6);
	//		return ptConv(r,rcOffset.Col+rcSize.Col);
	//}
	return CPoint(0,0);
}

CPoint MapLayoutItem::ptBorderLon(Side side, double rLon) const
{
	throw ErrorObject(String("To Be Done %d %s", __LINE__, __FILE__));
	//GeoRef grf = mcd()->georef;
	//RowCol rcSize = mcsi->rcSize();
	//RowCol rcOffset = mcsi->rcOffset();
	//RowCol rc;
	//rc.Row = rcOffset.Row + rcSize.Row/2;
	//rc.Col = rcOffset.Col + rcSize.Col/2;
	//Coord crd = grf->cConv(rc);
	//LatLon ll = grf->cs()->llConv(crd);
	//ll.Lon = rLon;
	//double r;

	//switch (side)
	//{
	//	case sideTOP: 
	//		r = rFindNull(ValFinderLatLon(grf,ll,true,false,rcOffset.Row), ll.Lat*1e6);
	//		return ptConv(rcOffset.Row,r);
	//	case sideBOTTOM:
	//		r = rFindNull(ValFinderLatLon(grf,ll,true,false,rcOffset.Row+rcSize.Row), ll.Lat*1e6);
	//		return ptConv(rcOffset.Row+rcSize.Row,r);
	//	case sideLEFT:
	//		r = rFindNull(ValFinderLatLon(grf,ll,true,true,rcOffset.Col), ll.Lat*1e6);
	//		return ptConv(r,rcOffset.Col);
	//	case sideRIGHT:
	//		r = rFindNull(ValFinderLatLon(grf,ll,true,true,rcOffset.Col+rcSize.Col), ll.Lat*1e6);
	//		return ptConv(r,rcOffset.Col+rcSize.Col);
	//}
	return CPoint(0,0);
}

bool MapLayoutItem::fConfigure()
{
	CFrameWnd* fw = ld->wndGetActiveView()->GetParentFrame();
	if (!fCreatedLayerTreeBar) {
		fCreatedLayerTreeBar = true;
  	ltb.Create(fw, 124, CSize(150,200));
    ltb.SetWindowText(TR("Layer Management").c_str());
  	ltb.EnableDocking(CBRS_ALIGN_LEFT|CBRS_ALIGN_RIGHT);
    ltb.view = new LayerTreeView;
  	ltb.view->Create(NULL, NULL, AFX_WS_DEFAULT_VIEW|TVS_HASLINES|TVS_LINESATROOT|TVS_HASBUTTONS,
  			CRect(0,0,0,0), &ltb, 100, 0);
		mcd()->AddView(ltb.view);
		ltb.view->OnInitialUpdate();
		fw->DockControlBar(&ltb,AFX_IDW_DOCKBAR_LEFT);
	}
	if (!ltb.IsWindowVisible())
		fw->ShowControlBar(&ltb,TRUE,FALSE);

	return false; // nothing changed
}

void MapLayoutItem::OnRedraw()
{
	InitMetafile();
	ld->UpdateAllViews(0, LayoutDoc::hintITEM, this);
}

#define sMen(ID) ILWSF("men",ID).c_str()
#define pmadd(ID) men.AppendMenu(MF_STRING, ID, sMen(ID)); 

void MapLayoutItem::OnContextMenu(CWnd* wnd, CPoint pt) 
{
	ZoomableView* zv = dynamic_cast<ZoomableView*>(wnd);
	CMenu men;
	men.CreatePopupMenu();
	if (zv) {
		pmadd(ID_NORMAL);
		pmadd(ID_ZOOMIN);
		pmadd(ID_ZOOMOUT);
		men.AppendMenu(MF_SEPARATOR);
	}
	pmadd(ID_ITEM_REDRAW);
	pmadd(ID_ITEM_REPLACEMAPVIEW);
  pmadd(ID_SAVEVIEW);
  pmadd(ID_SAVEVIEWAS);
	men.AppendMenu(MF_SEPARATOR);
	pmadd(ID_ENTIREMAP);
	if (rScale() > 0) 
		pmadd(ID_SET_SCALE);
	if (zv) {
		pmadd(ID_SELECTAREA);
		pmadd(ID_PANAREA);
	}
	men.AppendMenu(MF_SEPARATOR);
	pmadd(ID_EDIT_CLEAR);
	men.SetDefaultItem(ID_ITEM_EDIT);
  int iCmd = men.TrackPopupMenu(TPM_LEFTALIGN|TPM_RIGHTBUTTON|TPM_NONOTIFY|TPM_RETURNCMD, pt.x, pt.y, wnd);
	switch (iCmd) 
	{
		case ID_ITEM_EDIT:
			OnItemEdit();
			return;
		case ID_ITEM_REDRAW:
			OnRedraw();
			return;
		case ID_ITEM_REPLACEMAPVIEW:
			OnItemReplaceMapView();
			return;
		case ID_ENTIREMAP:
			OnEntireMap();
			return;
		case ID_SELECTAREA:
			OnSelectArea(zv);
			return;
		case ID_PANAREA:
			OnPanRect(zv);
			return;
		case ID_SAVEVIEW: {
			String sOldDir = IlwWinApp()->sGetCurDir();
			mcd()->OnSaveView();
			IlwWinApp()->SetCurDir(sOldDir);
			return; }
		case ID_SAVEVIEWAS: {
			String sOldDir = IlwWinApp()->sGetCurDir();
			mcd()->OnSaveViewAs();
			IlwWinApp()->SetCurDir(sOldDir);
			return; }
		case ID_SET_SCALE:
			OnSetScale();
			return;
		default:
			wnd->SendMessage(WM_COMMAND, iCmd);
	}
}

void MapLayoutItem::OnSelectArea(ZoomableView* zvw)
{
	zv = zvw;
	if (0 == zv || zv->tools.size() > 0)
		return;
	CSize sz = rectPos().Size();
	zv->tools[ID_ZOOMIN] = new AreaSelector(zv, this, (NotifyRectProc)&MapLayoutItem::AreaSelected, sz);
}

void MapLayoutItem::AreaSelected(CRect rect)
{
	throw ErrorObject(String("To Be Done %d %s", __LINE__, __FILE__));
 // if (rect.Width() < 3 || rect.Height() < 3)
	//	return;
	//MinMax mm = zv->mmRect(rect); // mm in 0.1 mm units
	//MinMax mmOldPos = mmPosition();

	//MapCompositionDoc* mcd = mcsi->GetDocument();
	//if (0 == mcd || ) 
	//	return;
	//double rPixSize = mcd->georef->rPixSize();
	//if (rUNDEF == rPixSize) 
	//	rPixSize = 1;
	//RowCol rcMaxSize = mcd->georef->rcSize();
	//RowCol rcOffset = mcsi->rcOffset();

	//RowCol rcSize; 
	//double rScale = m_rScale;
	//rScale /= 10000; // 0.1 mm units per m

	//int iDiffTop = mm.MinRow() - mmOldPos.MinRow();
	//int iDiffLeft = mm.MinCol() - mmOldPos.MinCol();
	//rcOffset.Row += rounding(iDiffTop / rPixSize * rScale);
	//rcOffset.Col += rounding(iDiffLeft / rPixSize * rScale);
	//if (rcOffset.Col > rcMaxSize.Col - 2 || 
	//	  rcOffset.Row > rcMaxSize.Row - 2) 
	//{
	//	OnEntireMap();
	//	return;
	//}
	//if (rcOffset.Col < 0)
	//	rcOffset.Col = 0;
	//if (rcOffset.Row < 0)
	//	rcOffset.Row = 0;

	//rcSize.Row = rounding(mm.height() / rPixSize * rScale);
	//rcSize.Col = rounding(mm.width() / rPixSize * rScale);

	//if (rcOffset.Row + rcSize.Row > rcMaxSize.Row)
	//	rcOffset.Row = rcMaxSize.Row - rcSize.Row;
	//if (rcOffset.Col + rcSize.Col > rcMaxSize.Col)
	//	rcOffset.Col = rcMaxSize.Col - rcSize.Col;
	//if (rcOffset.Row < 0 || rcOffset.Col < 0) 
	//{
	//	OnEntireMap();
	//	return;
	//}

	//mcsi->SetOffsetSize(rcOffset, rcSize);
	//InitScale();
	//InitMetafile();
	//ld->SetModifiedFlag();
	//ld->UpdateAllViews(0, LayoutDoc::hintITEM, this);
}

void MapLayoutItem::OnPanRect(ZoomableView* zvw)
{
	zv = zvw;
	if (0 == zv)
		return;
	zv->OnNoTool();
	zv->tools[ID_PANAREA] = new PanTool(zv, this, (NotifyMoveProc)&MapLayoutItem::PanMove);
}

void MapLayoutItem::PanMove(CPoint pt)
{
	CRect rect = zv->rctPos(mmPosition());
	rect -= pt;
	AreaSelected(rect);
}

class MapLayoutItemMapViewForm : public FormWithDest
{
public:
  MapLayoutItemMapViewForm(CWnd* wnd, String* sName) 
		: FormWithDest(wnd, TR("Replace Map View")),
		  sN(sName),
			fldView(NULL)
  {
		fldView = new FieldView(root, TR("&Map View"), sName);
		fldView->SetCallBack((NotifyProc)&MapLayoutItemMapViewForm::iCallBackLegaleChoice);
		SetMenHelpTopic("ilwismen\\layout_editor_replace_map_view.htm");
		create();
  }

	int iCallBackLegaleChoice(Event *ev)
	{
		if ( fldView == NULL)
			return 0;
		fldView->StoreData();
		FileName fn(*sN);
		if ( IlwisObject::iotObjectType( fn) != IlwisObject::iotMAPVIEW )
			DisableOK();
		else
			EnableOK();
		return 0;		
	}


private:
	FieldView *fldView;
	String *sN;
};

void MapLayoutItem::OnItemReplaceMapView()
{
	String sName = fnMapView.sFullName();
	MapLayoutItemMapViewForm frm(ld->wndGetActiveView(), &sName);
	if (frm.fOkClicked()) {	
		sName = sName.sUnQuote();
		if (sName != fnMapView.sFullName()) {
			fnMapView = sName;
			mcd()->OnOpenDocument(sName.c_str());
			Init();			 
			OnSetScale();
			mcd()->UpdateAllViews(0);
			ld->UpdateAllViews(0, 0, this);
		}
	}
}

void MapLayoutItem::OnUpdateValidGeoRef(CCmdUI* pCmdUI)
{
	throw ErrorObject(String("To Be Done %d %s", __LINE__, __FILE__));
	//MapCompositionDoc* mcd = mcsi->GetDocument();
	//if (0 == mcd || !mcd->georef.fValid() || mcd->georef->fGeoRefNone()) 
	//	pCmdUI->Enable(FALSE);
	//else
	//	pCmdUI->Enable(TRUE);
}

void MapLayoutItem::OnUpdateValidScale(CCmdUI* pCmdUI)
{
	if (rScale() < 0) 
		pCmdUI->Enable(FALSE);
	else
		pCmdUI->Enable(TRUE);
}

BOOL MapLayoutItem::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo) 
{
	MapCompositionDoc* mcd = mcsi->GetDocument();
	if (mcd && mcd->OnCmdMsg(nID, nCode, pExtra, pHandlerInfo))
		return TRUE;
	return LayoutItem::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}

void MapLayoutItem::OnSetScale()
{
	double rSc = rScale();
	MapLayoutItemSetScaleForm	frm(ld->wndGetActiveView(), &rSc);
	if (frm.fOkClicked())
		SetScale(rSc);
}

void MapLayoutItem::OnKeepScale()
{
	ld->SetModifiedFlag();
	fKeepScale = !fKeepScale;
}

void MapLayoutItem::OnUpdateKeepScale(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(fKeepScale?1:0);
}

void MapLayoutItem::SaveModified()
{
	MapCompositionDoc* mcd = mcsi->GetDocument();
	if (mcd)
		mcd->fSaveModified(false);
}
