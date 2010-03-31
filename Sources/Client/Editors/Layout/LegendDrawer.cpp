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
// LegendDrawer.cpp: implementation of the LegendDrawer class.
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
#include "Client\Base\IlwisDocument.h"
#include "Client\MainWindow\Catalog\CatalogDocument.h"
#include "Engine\Map\Mapview.h"
#include "Client\Mapwindow\MapCompositionDoc.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif



LegendBaseDrawer::LegendBaseDrawer(LegendLayoutItem& item)
: lli(item)
{
}

LegendBaseDrawer::~LegendBaseDrawer()
{
}
						 
void LegendBaseDrawer::ReadElements(ElementContainer& ec, const char* sSection)
{
}

void LegendBaseDrawer::WriteElements(ElementContainer& ec, const char* sSection)
{
}

void LegendBaseDrawer::OnDraw(CDC* cdc)
{
}

bool LegendBaseDrawer::fConfigure()
{
	return false;
}

bool LegendBaseDrawer::fValid() const
{
  return 0 != bmd();
}

String LegendBaseDrawer::sName()
{
  if (0 == bmd())
    return String("Invalid");
  String sN = bmd()->sName();
  sN = sN.sLeft(sN.length()-4); // chop extension
  String sName("%S %S.lgnd", SDCNameClassLegend, sN);
  return sName;
}

String LegendBaseDrawer::sTitle()
{
  if (0 == bmd())
    return String("Invalid");
  String sN = bmd()->sName();
  sN = sN.sLeft(sN.length()-4); // chop extension
  String sTitl("%S %S %S", SDCTitleDisplayOptions, SDCNameClassLegend, sN);
  return sTitl;
}

MapLayerSelector::MapLayerSelector(FormEntry* feParent, MapCompositionDoc* doc,
  BaseMapDrawer*& drwr)
  : BaseSelector(feParent), mcd(doc), bmd(drwr)
{
  SetWidth(100);
  SetHeight(100);
  style= WS_VSCROLL;
}

void MapLayerSelector::create()
{
  BaseSelector::create();
  for (list<Drawer*>::iterator iter = mcd->dl.begin(); iter != mcd->dl.end(); ++iter) 
	{
    BaseMapDrawer* bmd = dynamic_cast<BaseMapDrawer*>(*iter);
    if (bmd)
      if (0 != dynamic_cast<MapListColorCompDrawer*>(bmd))
        bmd = 0;
		if (bmd && LegendLayoutItem::fLegend(bmd))
		{
			String s = bmd->sName();
			int id = lb->AddString(s.scVal());
			lb->SetItemDataPtr(id, bmd);
		}
  }
  lb->SetCurSel(0);
  //lb->setNotifyDoubleClick(this,(NotifyProc)&MapLayerSelector::DblClk);
}

String MapLayerSelector::sName(int id)
{
  if (id < 0)
    return "?";
  Drawer* drwr = (Drawer*)lb->GetItemDataPtr(id);
  String s = drwr->sName();
  return s;
}

/*
int MapLayerSelector::DblClk(Event*)
{
  frm()->endOk(0);
  return 1;
}
*/

void MapLayerSelector::StoreData()
{
  int id = lb->GetCurSel();
  bmd = (BaseMapDrawer*) SendMessage(*lb, LB_GETITEMDATA, id, 0L);
}

SelectMapForm::SelectMapForm(CWnd* wParent, MapCompositionDoc* mcd)
: FormWithDest(wParent, SDCTitleMapSelection)
{
  new MapLayerSelector(root, mcd, bmdr);
  SetMenHelpTopic(htpCnfAnnLegendSelect);
  create();
}


