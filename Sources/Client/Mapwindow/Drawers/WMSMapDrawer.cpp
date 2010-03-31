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
// WMSMapDrawer.cpp: implementation of the WMSMapDrawer class.
//
//////////////////////////////////////////////////////////////////////

#include "Client\Headers\formelementspch.h"
#include "Engine\Base\System\RegistrySettings.h"
#include "Engine\Map\Segment\Seg.h"
#include "engine\map\polygon\POL.H"
#include "Client\Mapwindow\Positioner.h"
#include "Client\Mapwindow\Drawers\BaseDrawer.h"
#include "Client\Mapwindow\Drawers\Drawer.h"
#include "Client\Mapwindow\Drawers\BaseMapDrawer.h"
#include "Client\Mapwindow\Drawers\MapDrawer.h"
#include "Client\ilwis.h"
#include "Client\Base\datawind.h"
#include "Client\Base\IlwisDocument.h"
#include "Client\MainWindow\Catalog\CatalogDocument.h"
#include "Client\Mapwindow\MapWindow.h"
#include "Client\FormElements\syscolor.h"
#include "Engine\Map\Mapview.h"
#include "Client\Mapwindow\MapCompositionDoc.h"
#include "Client\Mapwindow\ZoomableView.h"
#include "Client\Mapwindow\Drawers\WMSMapDrawer.h"
#include "Client\Mapwindow\MapPaneView.h"
#include "Engine\SpatialReference\Gr.h"
#include "Engine\SpatialReference\Grsmpl.h"
#include "Engine\SpatialReference\Grcornrs.h"
#include "Engine\SpatialReference\GrcWMS.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

WMSMapDrawer::WMSMapDrawer(MapCompositionDoc* mapcd, const Map& _map)
: MapDrawer(mapcd, _map)
{
	drm = drmCOLOR;
	drawCase = ZoomableView::cNone;
}

WMSMapDrawer::WMSMapDrawer(MapCompositionDoc* mapcd, const MapView& view, const char* sSection)
: MapDrawer(mapcd, view, sSection)
{
	drawCase = ZoomableView::cNone;
}


void WMSMapDrawer::WriteLayer(MapView& view, const char* sSection)
{
  //view->WriteElement(sSection, "Type", "WMSMapDrawer");
  //view->WriteElement(sSection, "Map", mp);
  //view->WriteElement(sSection, "Text", fText);
  //if (fText) 
  //{
  //  view->WriteElement(sSection, "TextColor", clrText);
  //}  
  MapDrawer::WriteLayer(view, sSection);
}

WMSMapDrawer::~WMSMapDrawer()
{
}

int WMSMapDrawer::draw(CDC* cdc, zRect rect, Positioner* psn, volatile bool* fDrawStop)
{
//	if ( rect.width() < 150 && rect.height() < 150)
//		return 0;

	ILWISSingleLock csl(&cs, TRUE, SOURCE_LOCATION);
	POSITION pos = mcd->GetFirstViewPosition();
	CView* pFirstView = mcd->GetNextView( pos );
	ZoomableView *view = dynamic_cast<ZoomableView *>(pFirstView);
	if ( !view )
		return 0;
	if (!fAct) return 0;
	// restrict to invalidated area

	GeoRefCornersWMS *grfwms = mp->gr()->pgWMS();

	MinMax mmZoom = mm1;
	MinMax mmMap = mm2;

	bool recalcCB = drawCase != ZoomableView::cNone;
	grfwms->AdjustBounds(mmMap, mmZoom, oldBounds, recalcCB);
	mcd->initBounds(mm2);

	drawCase = ZoomableView::cNone; // reset drawstate; unless a new zoom action takes place no zooming happens

	double rFact = 1.0; //psn->rSc();


	long iMapLine;
	short iDspLine;

	//grfwms->setMinMax(mm2);
	mp->SetSize(RowCol(mmMap.height(),mmMap.width()));

	long iCols = mmMap.width();
	long iLines = mmMap.height();
	

	lpbi->bmiHeader.biHeight = rct.height(); // no show solution
	lpbi->bmiHeader.biWidth  = rct.width(); // no show solution

	if (*fDrawStop)
		return 0;
	mp->KeepOpen(true);
	LongBuf buf(mmMap.width());
	riTranquilizer = RangeInt(0,rect.Height());
	for (int i = 0; i < buf.iSize(); ++i)  // to prevent NAN values in buf.
				buf[i]= 0;
	for (iDspLine = 0, iMapLine = mmMap.MinRow();
		iDspLine <= rect.height() && iMapLine < iLines;
		iDspLine++, iMapLine++) 
	{
		iTranquilizer = iDspLine;
		if (*fDrawStop)
			break;
		if (fValue && !fAttTable)
			mp->GetLineVal(iMapLine, buf, mmMap.MinCol(), mmMap.width());
		else
			mp->GetLineRaw(iMapLine, buf, mmMap.MinCol(), mmMap.width());
		ConvLine(buf);
		PutLine(cdc, rct, iDspLine, buf);
	}
	

	mp->KeepOpen(false);
	return 0;
}

bool WMSMapDrawer::SetDrawerState(CRect rect, zDimension& dim, ZoomableView::Case c) {
	Map& mp = const_cast<Map&>(mpGet());
	GeoRefCornersWMS *grfwms = mp->gr()->pgWMS();
	if ( grfwms) {
		drawCase = c;
		RowCol rc1, rc2; 
		bool initial = grfwms->initial();
		if ( initial || c == ZoomableView::cEntire) {
			grfwms->reset();
			CoordBounds cb(grfwms->crdMin, grfwms->crdMax);
			double f = cb.width()/ cb.height();
			if ( f <= 1.0) {
				iXsize = rect.Height() * f;
				iYsize = rect.Height();
			} else {
				iXsize = rect.Width();
				iYsize = rect.Width() / f;
			}
			rc2.Col =  iXsize;
			rc2.Row =  iYsize;
			rc1.Col =  0; 
			rc1.Row =  0; 
			oldBounds = MinMax(rc1, rc2);
		} else if ( c != ZoomableView::cResize) {
			double f1 = (double)rect.Width()/dim.width();
			double f2 = (double)rect.Height()/dim.height();
			double f = f1 > f2 ? 1.0/f1 : 1.0/f2;
			iXsize =  rect.Width() * f;
			iYsize =  rect.Height() * f;
			rc1.Col =  rect.TopLeft().x - iXmax /2;
			rc1.Row =  rect.TopLeft().y - iYmax /2;
			rc2.Col =  rc1.Col + rect.Width();
			rc2.Row =  rc1.Row + rect.Height();
			oldBounds = mm2;
		} else {
			rc1 = mm1.rcMin;
			rc2 = mm1.rcMax;
		}

		int minx = dim.width() / 2.0 - iXsize/2.0;
		int maxx = dim.width() / 2.0 + iXsize/2.0;
		int miny = dim.height() / 2.0 - iYsize / 2.0;
		int maxy = dim.height() / 2.0 + iYsize / 2.0;
		rct = zRect(minx, miny, maxx, maxy);
	
		mm1  = MinMax(rc1, rc2);
		mm2 = MinMax(RowCol(0,0), RowCol(iYsize, iXsize));
		mcd->initBounds(mm2);
		iXmax = dim.width() - mm2.width();
		iYmax = dim.height() - mm2.height();
		return true;
	}

	return false;
}



