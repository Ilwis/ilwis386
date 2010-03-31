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
// MapListDrawer.cpp: implementation of the MapListDrawer class.
//
//////////////////////////////////////////////////////////////////////

#pragma warning(disable:4786)

#include "Client\Headers\formelementspch.h"
#include "Engine\Base\System\RegistrySettings.h"
#include "Engine\Map\Segment\Seg.h"
#include "Engine\Map\Polygon\POL.H"
#include "Engine\SpatialReference\GR3D.H"
#include "Client\Mapwindow\Positioner.h"
#include "Client\Mapwindow\Drawers\BaseDrawer.h"
#include "Client\Mapwindow\Drawers\Drawer.h"
#include "Headers\Hs\Drwforms.hs"
#include "Client\FormElements\fldcol.h"
#include "Client\FormElements\fldrpr.h"
#include "Client\FormElements\fldcolor.h"
#include "Client\Mapwindow\Drawers\BaseMapDrawer.h"
#include "Client\ilwis.h"
#include "Client\Base\datawind.h"
#include "Client\Base\IlwisDocument.h"
#include "Client\MainWindow\Catalog\CatalogDocument.h"
#include "Client\Mapwindow\MapWindow.h"
#include "Engine\Map\Mapview.h"
#include "Client\Mapwindow\MapCompositionDoc.h"
#include "Client\Mapwindow\MapPaneView.h"
#include "Client\FormElements\syscolor.h"
#include "Engine\Domain\dmsort.h"
#include "Client\Mapwindow\Drawers\MapDrawer.h"
#include "Client\Mapwindow\Drawers\MapListDrawer.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

BEGIN_MESSAGE_MAP(MapListDrawer, MapDrawer)
	//{{AFX_MSG_MAP(MapListDrawer)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

static map<HWND, MapListDrawer*> mapTimer;

void CALLBACK MapListDrawer::TimerProc(HWND hWnd, UINT, UINT nID, DWORD)
{
	MapListDrawer* mld = mapTimer[hWnd];
	if (mld) {
		mld->OnTimer();
	}
}

MapListDrawer::MapListDrawer(MapCompositionDoc* mcd, const MapList& ml)
: MapDrawer(mcd, ml[ml->iLower()]), iTimer(0)
{
  maplist = ml;
  iInterval = 60;
  iCurrMap = maplist->iLower();  
  SetMap(maplist[iCurrMap]);
  fNew = true;  
	fInTimer = false;
}

MapListDrawer::MapListDrawer(MapCompositionDoc* mapcd, const MapView& view, const char* sSection)
: MapDrawer(mapcd, view, sSection), iTimer(0)
{
  view->ReadElement(sSection, "MapList", maplist);
  view->ReadElement(sSection, "Refresh Rate", iInterval);
  iCurrMap = maplist->iLower();  
  SetMap(maplist[iCurrMap]);
}

MapListDrawer::~MapListDrawer()
{
  ILWISSingleLock csl(&cs, TRUE, SOURCE_LOCATION);
  if (iTimer) {
		CWnd* wnd = mcd->mpvGetView();
		if (wnd) {
			mapTimer.erase(wnd->m_hWnd);
			KillTimer(wnd->m_hWnd,iTimer);
		}
	}
}

void MapListDrawer::WriteLayer(MapView& view, const char* sSection)
{
  MapDrawer::WriteLayer(view, sSection);
  view->WriteElement(sSection, "Type", "MapListDrawer");
  view->WriteElement(sSection, "MapList", maplist);
  view->WriteElement(sSection, "Refresh Rate", iInterval);
}

String MapListDrawer::sName()
{
  return maplist->sName(true);
}

void MapListDrawer::SetTimer()
{
	if (0 == AfxGetThread()->GetMainWnd())
		return;
	if (iInterval > 0) 
	{
		CWnd* wnd = mcd->mpvGetView();
		if (wnd) {
			iTimer = wnd->SetTimer(1, 60000.0/iInterval, MapListDrawer::TimerProc);
			mapTimer[wnd->m_hWnd] = this;
		}
	}
	else {
		if (iTimer) {
			CWnd* wnd = mcd->mpvGetView();
			KillTimer(wnd->m_hWnd,iTimer);
			mapTimer.erase(wnd->m_hWnd);
		}
		iTimer = 0;
	}
}

int MapListDrawer::Setup()
{
  return MapDrawer::Setup();
}

void MapListDrawer::OnTimer()
{
	if (fInTimer)
		return;
	fInTimer = true;
  iCurrMap += 1;
  if (iCurrMap > maplist->iUpper())
    iCurrMap = maplist->iLower();
  SetMap(maplist[iCurrMap]);
	mcd->UpdateAllViews(0,4);
/*
  String s = maplist->sName();
  if (maplist->sDescription.length()) {
    s &= ": ";
    s &= maplist->sDescription;
  }
  String str("%S (%S)", s, mp->sName());
//  ((zFrameWin*)mppn->parent())->caption(str);
*/
	fInTimer = false;
}

int MapListDrawer::draw(CDC* cdc, zRect rct, Positioner* psn, volatile bool* fDrawStop)
{
  ILWISSingleLock csl(&cs, TRUE, SOURCE_LOCATION);
  MapDrawer::draw(cdc, rct, psn, fDrawStop);
  return 0;  
}

class MapListDrawerForm: public FormWithDest
{
public:
  MapListDrawerForm(MapListDrawer* mdr, bool fShow);
private:  
  MapListDrawer* mdw;
  int MapDrawerConfigure(Event*);
};

int MapListDrawer::Configure(bool fShowForm)
{
  if (fNew) {
    if (!MapDrawer::Configure(fShowForm))
      return 0;
  }
  MapListDrawerForm frm(this, fShowForm);
	SetTimer();
  return frm.fOkClicked();
}

MapListDrawerForm::MapListDrawerForm(MapListDrawer* mdr, bool fShowForm)
: FormWithDest(mdr->mcd->wndGetActiveView(), SDCTitleMapList), mdw(mdr)
{
	iImg = IlwWinApp()->iImage(".mpl");

  if (!fShowForm) 
    NotShowAlways();
  new FieldInt(root, SDCUiRefreshRate, &mdw->iInterval, ValueRange(0,600), true);
  new PushButton(root, SDCUiDspOptions, (NotifyProc)&MapListDrawerForm::MapDrawerConfigure);
  SetMenHelpTopic(htpCnfMapList);
  create();
}

int MapListDrawerForm::MapDrawerConfigure(Event*)
{
  return mdw->MapDrawer::Configure();
}

IlwisObject MapListDrawer::obj() const
{
  return maplist;
}
/*
void MapListDrawer::Prop()
{
  zFrameWin* win = static_cast<zFrameWin*>(mppn->parent());
  PropInfForm(win, maplist->fnObj);
}
*/
zIcon MapListDrawer::icon() const
{
	return zIcon("MapListIcon");
}

bool MapListDrawer::fEditable()
{
	return false;
}

