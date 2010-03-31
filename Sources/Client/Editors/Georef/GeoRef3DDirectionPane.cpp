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
#include "Client\Headers\formelementspch.h"
#include "Client\ilwis.h"
#include "Engine\Map\Segment\Seg.h"
#include "engine\map\polygon\POL.H"
#include "Client\Base\datawind.h"
#include "Client\Mapwindow\MapWindow.h"
#include "Client\Base\IlwisDocument.h"
#include "Client\MainWindow\Catalog\CatalogDocument.h"
#include <afxole.h>
#include "Engine\Map\Mapview.h"
#include "Client\Mapwindow\MapCompositionDoc.h"
#include "Client\Mapwindow\MapPaneView.h"
#include "Client\Mapwindow\Positioner.h"
#include "Client\Mapwindow\Drawers\BaseDrawer.h"
#include "Client\Mapwindow\Drawers\Drawer.h"
#include "Engine\Base\System\RegistrySettings.h"
#include "Engine\SpatialReference\GR3D.H"
#include "Headers\constant.h"
#include "Client\Mapwindow\MapPaneViewTool.h"
#include "Client\Mapwindow\AreaSelector.h"
#include "Client\Mapwindow\MapPaneView.h"
#include "Client\Mapwindow\MiniMapPaneView.h"
#include "Client\Mapwindow\Drawers\BaseDrawer.h"
#include "Client\Mapwindow\Drawers\Drawer.h"
#include "Client\Mapwindow\Drawers\BaseMapDrawer.h"
#include "Client\Editors\Editor.h"
#include "Client\Editors\Georef\GeoRef3DEditor.h"
#include "Client\Editors\Georef\GeoRef3DDirectionPane.h"
#include "Client\Editors\Georef\GeoRef3DDirectionTool.h"

BEGIN_MESSAGE_MAP(GeoRef3DDirectionPane, MiniMapPaneView)
	ON_WM_CONTEXTMENU()
	ON_COMMAND(ID_SETFROMVIEWPOINT, OnSetFromViewPoint)
	ON_COMMAND(ID_EXTENDBOUNDARIES, OnExtendBoundaries)	
	ON_COMMAND(ID_CONFIGURE, OnConfigure)	
END_MESSAGE_MAP()

GeoRef3DDirectionPane::GeoRef3DDirectionPane(GeoRef3D *_grf, GeoRef3DEditorBar *bar) :
	grf3D(_grf),
	gr3DBar(bar),
	crdView(Coord(rUNDEF, rUNDEF)),
	crdFocal(Coord(rUNDEF, rUNDEF))
{
	if ( grf3D == NULL) return;
	
	crdView = grf3D->crdViewPoint();
	crdFocal = grf3D->crdFocalPoint();
}

GeoRef3DDirectionPane::~GeoRef3DDirectionPane()
{
}

#define sMen(ID) ILWSF("men",ID).scVal()
#define add(ID) men.AppendMenu(MF_STRING, ID, sMen(ID)); 

void GeoRef3DDirectionPane::OnContextMenu(CWnd* pWnd, CPoint point) 
{
  CMenu men, menSub;
	men.CreatePopupMenu();
	add(ID_SETFROMVIEWPOINT);
	add(ID_EXTENDBOUNDARIES);	
  men.AppendMenu(MF_SEPARATOR);
	add(ID_CONFIGURE);
  men.AppendMenu(MF_SEPARATOR);
  menSub.CreateMenu();
	GetDocument()->menLayers(menSub, ID_LAYFIRST);
  men.AppendMenu(MF_POPUP, (UINT)menSub.GetSafeHmenu(), sMen(ID_LAYEROPTIONS)); 
	menSub.Detach();    
  men.TrackPopupMenu(TPM_LEFTALIGN|TPM_RIGHTBUTTON, point.x, point.y, pWnd);
}

void GeoRef3DDirectionPane::OnSetFromViewPoint()
{
	OnNoTool();
	as = new GeoRef3DDirectionTool(this, gr3DBar);
}

void GeoRef3DDirectionPane::OnDraw(CDC* cdc)
{
	MiniMapPaneView::OnDraw(cdc);

	DrawDirectionTriangle();

}

void GeoRef3DDirectionPane::OnConfigure()
{
	GeoRef3DConfigForm frm(this);
	if ( frm.fOkClicked() )
		Invalidate();
		
}

GeoRef3D *GeoRef3DDirectionPane::GetGeoRef3D()
{
	return grf3D;
}

void GeoRef3DDirectionPane::SetDirectionPoints(CPoint _pFrom, CPoint _pTo)
{
	crdView = crdPnt(_pFrom);
	crdFocal= crdPnt(_pTo);
	DrawDirectionTriangle();
}

void GeoRef3DDirectionPane::DrawDirectionTriangle()
{
	zPoint pFrom = pntPos(crdView)	;
	zPoint pTo = 	pntPos(crdFocal);
	
	if ( pFrom == pTo || pFrom == CPoint(iUNDEF, iUNDEF))
		return;	
	IlwisSettings settings("GeoRef3DEditor");
	Color clrViewAxis = settings.clrValue("ViewAxis", RGB(255,255,255));
	Color clrAngle = settings.clrValue("ViewAngle", RGB(0,255,0));	

	CClientDC cdc(this);
	int iROP = cdc.SetROP2(R2_NOTXORPEN);
	CPen pen(PS_SOLID, 1, clrViewAxis);
	CGdiObject* pn = cdc.SelectObject(&pen);	
//	CGdiObject* pn = cdc.SelectStockObject(WHITE_PEN);

	cdc.MoveTo(pFrom);
	cdc.LineTo(pTo);
	double rD = sqrt((double)(( pFrom.x - pTo.x) *( pFrom.x - pTo.x) + ( pFrom.y - pTo.y) * ( pFrom.y - pTo.y)));
	if ( rD < 10.0 )
		return;
	
	GeoRef3D *grf = GetGeoRef3D();
	double rViewAngle = grf->rGetViewAngle() * M_PI / 180.0;
	double rAt = ( pFrom.x - pTo.x) != 0 ? atan2((double)( pTo.y - pFrom.y ) , ( pTo.x - pFrom.x)) : 0; 
	double rAAA = rAt * 180 /M_PI;
	double rAt1 = rAt + rViewAngle / 2.0;	
	double rAt2 = rAt - rViewAngle / 2.0;		
	double rEndAngle1X = pFrom.x + cos ( rAt1) * rD / 2.0;
	double rEndAngle1Y = pFrom.y + sin ( rAt1) * rD / 2.0;
	CPen pen2(PS_DOT, 1, clrAngle);
	cdc.SelectObject(&pen2);		
	cdc.MoveTo(pFrom);
	cdc.LineTo((int)rEndAngle1X, (int)rEndAngle1Y);	
	
	rEndAngle1X = pFrom.x + cos ( rAt2) * rD / 2.0;
	rEndAngle1Y = pFrom.y + sin ( rAt2) * rD / 2.0;	
	cdc.MoveTo(pFrom);
	cdc.LineTo((int)rEndAngle1X, (int)rEndAngle1Y);	

	
	cdc.SelectObject(pn);
	cdc.SetROP2(iROP);	
}

void GeoRef3DDirectionPane::SetViewPoint(const Coord& _crdView)
{
	crdView = _crdView;
	MinMax mm = mmBounds();
	CoordBounds cb;
	MapCompositionDoc *doc = GetDocument();
	if ( doc == NULL) return;
  cb += doc->georef->cConv(mm.rcMin);
  cb += doc->georef->cConv(mm.rcMax);
	if ( cb.fContains( crdView))
		return;
	// corners
	Coord cCenter( (cb.cMin.x + cb.cMax.x)/2, (cb.cMin.y + cb.cMax.y)/2	);
	double rDX = abs(cCenter.x - crdView.x) * 1.10;
	double rDY = abs(cCenter.y - crdView.y) * 1.05;
	Coord crd1(cCenter.x + rDX, cCenter.y + rDY);
	Coord crd2(cCenter.x - rDX, cCenter.y + rDY);
	Coord crd3(cCenter.x + rDX, cCenter.y - rDY);
	Coord crd4(cCenter.x - rDX, cCenter.y - rDY);	
	cb += crd1;
	cb += crd2;
	cb += crd3;
	cb += crd4;	
  mm.rcMin = doc->georef->rcConv(cb.cMin );
  mm.rcMax = doc->georef->rcConv(cb.cMax);
  mm.Check();
  doc->SetBounds(mm);
	if	 ( GetSafeHwnd())
		Invalidate();
}

void GeoRef3DDirectionPane::SetFocalPoint(const Coord& _crdFocal)
{
	crdFocal = _crdFocal;
	CoordBounds cb;	
	MinMax mm = mmBounds();
	MapCompositionDoc *doc = GetDocument();
	if ( doc == NULL) return;	
  cb += doc->georef->cConv(mm.rcMin);
  cb += doc->georef->cConv(mm.rcMax);
	if ( cb.fContains( crdFocal))
		return;
	cb += crdFocal;
  mm.rcMin = doc->georef->rcConv(cb.cMin );
  mm.rcMax = doc->georef->rcConv(cb.cMax );
  mm.Check();
  doc->SetBounds(mm);	
	if	 ( GetSafeHwnd())	
		Invalidate();	
}

void GeoRef3DDirectionPane::OnInitialUpdate()
{
	MiniMapPaneView::OnInitialUpdate();
	SetViewPoint(crdView);
	SetFocalPoint(crdFocal);	
}

void GeoRef3DDirectionPane::OnExtendBoundaries()
{
	gr3DBar->OnExtend();	
}
