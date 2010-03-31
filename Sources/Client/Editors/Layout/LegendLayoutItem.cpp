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
// LegendLayoutItem.cpp: implementation of the LegendLayoutItem class.
//
//////////////////////////////////////////////////////////////////////

#include "Client\Headers\formelementspch.h"
#include "Client\Editors\Layout\LayoutItem.h"
#include "Client\Editors\Layout\TextLayoutItem.h"
#include "Client\Editors\Layout\LegendLayoutItem.h"
#include "Client\Editors\Layout\MapLayoutItem.h"
#include "Headers\constant.h"
#include "Client\Base\IlwisDocument.h"
#include "Client\Editors\Layout\LayoutDoc.h"
#include "Headers\Hs\Drwforms.hs"
#include "Headers\Hs\Editor.hs"
#include "Client\FormElements\fldcolor.h"
#include "Client\FormElements\fldfontn.h"
#include "Engine\Map\Segment\Seg.h"
#include "Engine\Map\Polygon\POL.H"
#include "Engine\Map\Point\PNT.H"
#include "Client\Editors\Utils\SYMBOL.H"
#include "Client\Editors\Utils\Smbext.h"
#include "Client\Editors\Utils\line.h"
#include "Client\Editors\Utils\Pattern.h"
#include "Client\Editors\Utils\MULTICOL.H"
#include "Client\Editors\Layout\LegendDrawer.h"
#include "Client\Mapwindow\Drawers\BaseDrawer.h"
#include "Client\Mapwindow\Drawers\Drawer.h"
#include "Client\Mapwindow\Drawers\BaseMapDrawer.h"
#include "Client\Mapwindow\Drawers\MapDrawer.h"
#include "Client\Mapwindow\Drawers\MapListDrawer.h"
#include "Client\Mapwindow\Drawers\MapListColorCompDrawer.h"
#include "Client\Mapwindow\Drawers\SegmentMapDrawer.h"
#include "Client\Mapwindow\Drawers\PolygonMapDrawer.h"
#include "Client\Mapwindow\Drawers\PointMapDrawer.h"
#include "Client\Editors\Layout\LegendClassDrawer.h"
#include "Client\Editors\Layout\LegendSegmentClassDrawer.h"
#include "Client\Editors\Layout\LegendPolygonClassDrawer.h"
#include "Client\Editors\Layout\LegendRasterClassDrawer.h"
#include "Client\Editors\Layout\LegendValueDrawer.h"
#include "Client\MainWindow\Catalog\CatalogDocument.h"
#include "Engine\Map\Mapview.h"
#include "Client\Mapwindow\MapCompositionDoc.h"
#include "Client\Editors\Layout\LegendPointDrawers.h"
#include "Client\Editors\Layout\LegendBlockDrawer.h"
#include "Headers\Hs\Layout.hs"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

BEGIN_MESSAGE_MAP(LegendLayoutItem, TextLayoutItem)
	//{{AFX_MSG_MAP(LegendLayoutItem)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


LegendLayoutItem::LegendLayoutItem(LayoutDoc* ld, MapLayoutItem* mapli)
	: TextLayoutItem(ld)
	, mli(mapli)
	, lbd(0), m_bmd(0)
{
	MinMax mm = mli->mmPosition();
	mm.MaxRow() = mm.MinRow() + 400;
	mm.MinCol() = mm.MaxCol() + 50; // 0.5 cm right of map.
	mm.MaxCol() = mm.MinCol() + 400;
	LayoutItem::SetPosition(mm,-1);
}

LegendLayoutItem::LegendLayoutItem(LayoutDoc* ld)
	: TextLayoutItem(ld)
	, mli(0)
	, lbd(0), m_bmd(0)
{
}

LegendLayoutItem::~LegendLayoutItem()
{
	if (lbd)
		delete lbd;
}

String LegendLayoutItem::sType() const
{
	return "Legend";
}

void LegendLayoutItem::Setup()
{
	// empty
}

void LegendLayoutItem::OnDraw(CDC* cdc)
{
	if (0 == bmd()) {
		CPen pen(PS_SOLID,30,Color(255,64,64)); // 3 mm RED pen 
		CPen* penOld = cdc->SelectObject(&pen);

		CRect rect = rectPos();
		const CPoint ptl = rect.TopLeft();
		const CPoint pbl(rect.left, rect.bottom);
		const CPoint ptr(rect.right, rect.top);
		const CPoint pbr = rect.BottomRight();
		cdc->MoveTo(ptl);
		cdc->LineTo(pbr);
		cdc->MoveTo(ptr);
		cdc->LineTo(pbl);

		cdc->SelectObject(penOld);
		return;
	}

	InitFont init(this,cdc);
	cdc->SetTextAlign(TA_LEFT|TA_TOP);

	CPen pen(PS_SOLID,3,clr); // 0.3 mm pen 
	CPen* penOld = cdc->SelectObject(&pen);

	if (lbd && lbd->fValid())
		lbd->OnDraw(cdc);
	
	cdc->SelectObject(penOld);
}

BaseMapDrawer* LegendLayoutItem::bmd() const
{
	if (0 == mli)
		return 0;
	MapCompositionDoc* mcd = mli->mcd();
	if (0 == mcd)
		return 0;
  for (list<Drawer*>::iterator iter = mcd->dl.begin(); iter != mcd->dl.end(); ++iter) 
	{
		Drawer* dr = *iter;
		if (dr == m_bmd)
			return m_bmd;
  }
	return 0;
}

class SizeOrColorForm: public FormWithDest
{
public:
  SizeOrColorForm(CWnd* w)
    : FormWithDest(w, SDCTitleCreateLegend)
  {
    iSizeColor = 0;
    RadioGroup* rg = new RadioGroup(root, "", &iSizeColor);
    new RadioButton(rg, SDCUiSizeLegend);
    new RadioButton(rg, SDCUiColorLegend);
    SetMenHelpTopic(htpCnfAnnLegendSizeOrColor);
    create();
  }
  int iSizeColor;
};

bool LegendLayoutItem::fConfigure()
{
	if (0 == bmd()) {
		delete lbd;
		lbd = 0;
	}
	if (lbd) {
		if (lbd->fValid())
			return lbd->fConfigure();
		else {
			delete lbd;
			lbd = 0;
		}
	}
	m_bmd = 0;
	if (mli == 0)
		return false;
	
	MapCompositionDoc* mcd = mli->mcd();
  int iNumMapDrw = 0;
  Drawer* drw = 0;
  for (list<Drawer*>::iterator iter = mcd->dl.begin(); iter != mcd->dl.end(); ++iter) 
	{
		Drawer* dr = *iter;
    if (fLegend(dr)) {
      drw = dr;
      iNumMapDrw++;
    }
  }
  if (iNumMapDrw > 1) {
    SelectMapForm form(ld->wndGetActiveView(), mcd);
    bool fOk = form.fOkClicked();
    if (fOk)
      drw = form.bmdr;
    else
      drw = 0;
  }
  if (drw) {
		m_bmd = dynamic_cast<BaseMapDrawer*>(drw);

		if (m_bmd) {
			Domain dm = m_bmd->dm();

			MapDrawer* mapdr = dynamic_cast<MapDrawer*>(drw);
			SegmentMapDrawer* segdr = dynamic_cast<SegmentMapDrawer*>(drw);
			PolygonMapDrawer* poldr = dynamic_cast<PolygonMapDrawer*>(drw);
		  PointMapDrawer* pntdr = dynamic_cast<PointMapDrawer*>(drw);
  		// Class
			if (dm->pdc()) {
				if (mapdr)
					lbd = new LegendRasterClassDrawer(*this);
				else if (segdr)
					lbd = new LegendSegmentClassDrawer(*this);
				else if (poldr)
					lbd = new LegendPolygonClassDrawer(*this);
				else if (pntdr) 
					lbd = new LegendPointClassDrawer(*this);
			}
			else if (pntdr)
			{
				switch (pntdr->drmMethod())
				{ 
					case drmARROW:
						lbd = new LegendPointArrowDrawer(*this);
						break;
					case drmGRAPH:
			      switch (pntdr->eST) {
			        case PointMapDrawer::stPIECHART: 
								if (pntdr->fStretch) {
									SizeOrColorForm frm(ld->wndGetActiveView());
									if (!frm.fOkClicked())
										return false;
									if (0 == frm.iSizeColor)
										lbd = new LegendPointCircleDrawer(*this);
								}
								break;
			        case PointMapDrawer::stBARGRAPH:
							case PointMapDrawer::stCOMPBAR: 
								SizeOrColorForm frm(ld->wndGetActiveView());
								if (!frm.fOkClicked())
									return false;
								if (0 == frm.iSizeColor)
									lbd = new LegendPointBarDrawer(*this);
								break;
						}
						if (0 == lbd)
							lbd = new LegendPointGraphDrawer(*this);
						break;
					default:
				    if ((dm->pdvi() || dm->pdvr() || dm->pdi()) && pntdr->fStretch)
							lbd = new LegendPointValueDrawer(*this);
				}
			}
		  // Value
			else if (dm->pdvi() || dm->pdvr() || dm->pdi())
				lbd = new LegendValueDrawer(*this);
		}
	}
	if (lbd && lbd->fValid())
		return lbd->fConfigure();
  return false;
}

void LegendLayoutItem::ReadElements(ElementContainer& ec, const char* sSection)
{
	TextLayoutItem::ReadElements(ec, sSection);		 
	sText = "";
	int iNr;
	ObjectInfo::ReadElement(sSection, "Map View", ec, iNr);
	mli = dynamic_cast<MapLayoutItem*>(ld->liFindID(iNr));
	int iLayer = 0;
	ObjectInfo::ReadElement(sSection, "Layer", ec, iLayer);
	iNr = 1;
	m_bmd = 0;
	MapCompositionDoc* mcd = mli->mcd();
  for (list<Drawer*>::iterator iter = mcd->dl.begin(); iter != mcd->dl.end(); ++iter, ++iNr) 
	{
		if (iNr == iLayer) {
			m_bmd = dynamic_cast<BaseMapDrawer*>(*iter);
			break;
		}
  }
	lbd = 0;
	String sType;	
	ObjectInfo::ReadElement(sSection, "LegendType", ec, sType);
  if ("SegmentClass" == sType)
    lbd = new LegendSegmentClassDrawer(*this);
  else if ("PolygonClass" == sType)
    lbd = new LegendPolygonClassDrawer(*this);
  else if ("RasterClass" == sType)
    lbd = new LegendRasterClassDrawer(*this);
  else if ("Value" == sType)
		lbd = new LegendValueDrawer(*this);
  else if ("PointClass" == sType)
    lbd = new LegendPointClassDrawer(*this);
  else if ("PointGraph" == sType)
    lbd = new LegendPointGraphDrawer(*this);
  else if ("PointValue" == sType)
    lbd = new LegendPointValueDrawer(*this);
  else if ("PointCircle" == sType)
    lbd = new LegendPointCircleDrawer(*this);
  else if ("PointBar" == sType)
    lbd = new LegendPointBarDrawer(*this);
  else if ("PointArrow" == sType)
    lbd = new LegendPointArrowDrawer(*this);
	if (lbd)
		lbd->ReadElements(ec, sSection);
}

void LegendLayoutItem::WriteElements(ElementContainer& ec, const char* sSection)
{
	TextLayoutItem::WriteElements(ec, sSection);
	ObjectInfo::WriteElement(sSection, "Map View", ec, mli->iID());

	int iNr = 1;
	MapCompositionDoc* mcd = mli->mcd();
  for (list<Drawer*>::iterator iter = mcd->dl.begin(); iter != mcd->dl.end(); ++iter, ++iNr) 
	{
		Drawer* dr = *iter;
		if (dr == m_bmd) {
			ObjectInfo::WriteElement(sSection, "Layer", ec, iNr);
			break;
		}
	}
	if (lbd)
		lbd->WriteElements(ec, sSection);
}

bool LegendLayoutItem::fLegend(Drawer* drw)
{
  BaseMapDrawer* bmd = dynamic_cast<BaseMapDrawer*>(drw);
  if (0 == bmd)
    return false;
  Domain dm = bmd->dm();
  // Class
  if (dm->pdc())
    return drmRPR == bmd->drmMethod();
  PointMapDrawer* pdr = dynamic_cast<PointMapDrawer*>(drw);
  // Points
  if (pdr) {
    switch (pdr->drmMethod()) {
      case drmGRAPH:
        switch (pdr->eST) {
          case PointMapDrawer::stPIECHART:
          case PointMapDrawer::stBARGRAPH:
          case PointMapDrawer::stCOMPBAR:
          case PointMapDrawer::stVOLCUBES:
            return true;
          default:
            return false;
        }
      case drmARROW:
        return pdr->colClr.fValid() && pdr->colClr->dm()->pdc();
      case drmSINGLE:
        return dm->pdv() && pdr->fStretch;
      default:
        return false;
    }
  }
  // Value
  if (dm->pdvi() || dm->pdvr() || dm->pdi())
    return drmRPR == bmd->drmMethod() || drmIMAGE == bmd->drmMethod();

  return false;
}

void LegendLayoutItem::SetPosition(MinMax mm, int iHit)
{
	switch (iHit) {
		case CRectTracker::hitTop:
		case CRectTracker::hitBottom:
		{
			MinMax mmOldPos = mmPosition();
			int iHeight = mmOldPos.height();
			int iNewHeight = mm.height();
			LegendItemDrawer* lid = dynamic_cast<LegendItemDrawer*>(lbd);
			if (0 != lid && iHeight != iNewHeight) 
			{
				int& iRowExtra = lid->iRowExtra;
				int iItems = lid->iItems();
				if (iItems > 1)
					iItems -= 1;
				if (iNewHeight > iHeight) {
					iRowExtra += 1 + (iNewHeight - iHeight - 1) / iItems;					
				}
				else {
					iRowExtra -= 1 + (iHeight - iNewHeight - 1) / iItems;					
					if (iRowExtra < 0)
						iRowExtra = 0;
				}
			}
		}	break;
	}
	TextLayoutItem::SetPosition(mm, iHit);
}

String LegendLayoutItem::sName() const
{
	Drawer* dr = bmd();
	if (dr)
		return String(SLONameLegend_SS.scVal(), dr->sName(), mli->sName());
	else
		return SLONameLegend;
}

bool LegendLayoutItem::fDependsOn(LayoutItem* li)
{
	return li == mli;
}

MapCompositionDoc *LegendLayoutItem::mcd() const
{
	return mli->mcd();
}



/*
LegendBaseDrawer* LegendLayoutItem::drLegend(Drawer* drw)
{
  BaseMapDrawer* bmd = dynamic_cast<BaseMapDrawer*>(drw);
  if (0 == bmd)
    return 0;
  Domain dm = bmd->dm();

  MapDrawer* mapdr = dynamic_cast<MapDrawer*>(drw);
  SegmentMapDrawer* segdr = dynamic_cast<SegmentMapDrawer*>(drw);
  PolygonMapDrawer* poldr = dynamic_cast<PolygonMapDrawer*>(drw);
  PointMapDrawer* pntdr = dynamic_cast<PointMapDrawer*>(drw);

  // Class
  if (dm->pdc()) {
    if (mapdr)
      return new LegendRasterClassDrawer(mapdr);
    else if (segdr)
      return new LegendSegmentClassDrawer(segdr);
    else if (poldr)
      return new LegendPolygonClassDrawer(poldr);
	}

  // Points
  if (pntdr) {
    if (dm->pdc())
      return new LegendPointClassDrawer(pntdr);
    if (drmARROW == pntdr->drm)
      return new LegendPointArrowDrawer(pntdr);
    if (drmGRAPH == pntdr->drmMethod()) {
      switch (pntdr->eST) {
        case PointDrawer::stPIECHART: {
          if (pntdr->fStretch) {
            SizeOrColorForm frm(drw->mppn->parent());
            if (!frm.fOkClicked())
              return 0;
            if (0 == frm.iSizeColor)
              return new LegendPointCircleDrawer(pntdr);
          }
        } break;
        case PointDrawer::stBARGRAPH:
        case PointDrawer::stCOMPBAR: {
          SizeOrColorForm frm(drw->mppn->parent());
          if (!frm.fOkClicked())
            return 0;
          if (0 == frm.iSizeColor)
            return new LegendPointBarDrawer(pntdr);
        } break;
      }
      return new LegendPointGraphDrawer(pntdr);
    }
    if ((dm->pdvi() || dm->pdvr() || dm->pdi()) && pntdr->fStretch)
      return new LegendPointValueDrawer(pntdr);
  }

  // Value
  if (dm->pdvi() || dm->pdvr() || dm->pdi())
    return new LegendValueDrawer(bmd);

  return 0;
}

LegendBaseDrawer* LegendLayoutItem::drLegend(MapCompositionDoc* mcd)
{
  int iNumMapDrw = 0;
  Drawer* drw = 0;
  for (list<Drawer*>::iterator iter = mcd->dl.begin(); iter != mcd->dl.end(); ++iter) 
	{
		Drawer* dr = *iter;
    Drawer* dr = iter();
    if (fLegend(dr)) {
      drw = dr;
      iNumMapDrw++;
    }
  }
  if (iNumMapDrw > 1) {
    SelectMapForm form(pane);
    bool fOk = form.fOkClicked();
    if (fOk)
      drw = form.bmdr;
    else
      drw = 0;
  }
  if (drw)
    return drLegend(drw);
  else
    return 0;
}
*/
