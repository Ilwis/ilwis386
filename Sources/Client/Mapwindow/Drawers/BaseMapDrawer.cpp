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
// BaseMapDrawer.cpp: implementation of the BaseMapDrawer class.
//
//////////////////////////////////////////////////////////////////////

#include "Client\Headers\formelementspch.h"
#include "Engine\Base\System\RegistrySettings.h"
#include "Client\Mapwindow\MapPaneView.h"
#include "Engine\Map\Segment\Seg.h"
#include "Engine\Map\Polygon\POL.H"
#include "Engine\SpatialReference\GR3D.H"
#include "Client\Mapwindow\Positioner.h"
#include "Client\Mapwindow\Drawers\BaseDrawer.h"
#include "Client\Mapwindow\Drawers\Drawer.h"
#include "Headers\Hs\Drwforms.hs"
#include "Client\FormElements\syscolor.h"
#include "Client\FormElements\fldcol.h"
#include "Client\FormElements\fldrpr.h"
#include "Client\FormElements\fldcolor.h"
#include "Client\Mapwindow\Drawers\BaseMapDrawer.h"
#include "Client\Base\datawind.h"
#include "Client\Base\IlwisDocument.h"
#include "Client\MainWindow\Catalog\CatalogDocument.h"
#include "Engine\Map\Mapview.h"
#include "Client\Mapwindow\MapCompositionDoc.h"
#include "Client\Forms\FormGeneralPreferences.h"
#include "Client\ilwis.h"
//#include "Client\Editors\Utils\BaseBar.h"
#include "Client\Editors\Utils\sizecbar.h"
#include "Engine\Table\Rec.h"
#include "Engine\Representation\RPRVALUE.H"
#include "Engine\Base\Algorithm\Random.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


BEGIN_MESSAGE_MAP(BaseMapDrawer, Drawer)
	//{{AFX_MSG_MAP(BaseMapDrawer)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

BaseMapDrawer::BaseMapDrawer(MapCompositionDoc* mapcd)
: Drawer(mapcd)
, m_fCustomLegendValueRange(false)
{
  fSelectPossible = fSelectable = true;
  fAttTable = false;
  drm = drmRPR;
  drc = drcNORMAL;
  fValue = false;
  fStretch = true;
  iMultColors = 0;
  sm = smLINEAR;
  riStretch = RangeInt(0,255);
  rrStretch = RangeReal(0,100);
  clr = Color(0,176,20);
  clr0 = Color(168,168,168);
	IlwisSettings settings("DefaultSettings");
	clr0 = settings.clrValue("NoColor", clr0);
	clr = settings.clrValue("YesColor", clr);
}

BaseMapDrawer::BaseMapDrawer(MapCompositionDoc* mapcd, const MapView& view, 
                const char* sSection, const char* sType)
: Drawer(mapcd, view, sSection)
, m_fCustomLegendValueRange(false)
{													 
  fSelectPossible = true;
  FileName fn;
  view->ReadElement(sSection, sType, fn);
  BaseMap bm(fn);
  _dm = bm->dm();
  fValue = 0 != _dm->pdvi() || 0 != _dm->pdvr();
  sm = smLINEAR;
  if (bm->fTblAtt()) {
		tblAtt = bm->tblAtt();
    String sAttCol;
    view->ReadElement(sSection, "Attribute", sAttCol);
    if (sAttCol[0]) 
      colAtt = tblAtt->col(sAttCol);
  }    
  fAttTable = colAtt.fValid();
  if (fAttTable) {
    _dm = colAtt->dm();
		switch (_dm->dmt()) 
		{
      case dmtCOLOR:
        drm = drmCOLOR;
        break;
			default:
				; // is arranged in follwing statements
		}
	}
  String sStretchMethod;
  view->ReadElement(sSection, "Stretch Method", sStretchMethod);
  if ("Linear" == sStretchMethod)
    sm = smLINEAR;
  else if ("Logarithmic" == sStretchMethod)
    sm = smLOGARITHMIC;
  String sMethod;
  view->ReadElement(sSection, "Method", sMethod);
  if ("Representation" == sMethod) {
    drm = drmRPR;
    FileName fn;
    view->ReadElement(sSection, "Representation", fn);
    if ("" == fn.sExt)
      fn.sExt = ".RPR";
    _rpr = Representation(fn);
		RepresentationGradual* rpg = _rpr->prg();
		if (rpg) {
			rrStretch.rLo() = rpg->rGetLimitValue(0);
			rrStretch.rHi() = rpg->rGetLimitValue(rpg->iLimits()-1);
			riStretch.iLo() = (long)(rrStretch.rLo());
			riStretch.iHi() =(long)( rrStretch.rHi());
		}
    view->ReadElement(sSection, "Stretch", fStretch);
    if (fStretch) {
      view->ReadElement(sSection, "Range", riStretch);
      view->ReadElement(sSection, "Range", rrStretch);
    }  
    if (!_rpr.fValid())
      drm = drmSINGLE;
  }
  else if ("Image" == sMethod) {
    drm = drmIMAGE;
    view->ReadElement(sSection, "Stretch", fStretch);
    _rpr = Representation(FileName("gray.rpr"));
    view->ReadElement(sSection, "Range", riStretch);
    view->ReadElement(sSection, "Range", rrStretch);
  }
  else if ("SingleColor" == sMethod) {
    drm = drmSINGLE;
    view->ReadElement(sSection, "Color", (Color&)clr);
  }
  else if ("MultipleColor" == sMethod) {
    drm = drmMULTIPLE;
    iMultColors = view->iReadElement(sSection, "Colors");
  }
  else if ("DualColor" == sMethod) {
    drm = drmBOOL;
    view->ReadElement(sSection, "Color", (Color&)clr);
    view->ReadElement(sSection, "Color0", (Color&)clr0);
  }    
  else if ("ByAttribute" == sMethod) {
    drm = drmBYATTRIBUTE;
    FileName fn;
    view->ReadElement(sSection, "Representation", fn);
    fn.sExt = ".RPR";
    _rpr = Representation(fn);
    view->ReadElement(sSection, "Stretch", fStretch);
    if (fStretch) {
      view->ReadElement(sSection, "Range", rrStretch);
    }  
    if (!_rpr.fValid())
      drm = drmSINGLE;
  }
  else if ("Arrow" == sMethod) {
    drm = drmARROW;
  }
  else if ("Graph" == sMethod) {
    drm = drmGRAPH;
    view->ReadElement(sSection, "Stretch", fStretch);
    view->ReadElement(sSection, "Range", rrStretch);
  }
  String sColoring;
  view->ReadElement(sSection, "Coloring", sColoring);
  if ("Dark" == sColoring)
    drc = drcDARK;
  else if ("Light" == sColoring) 
    drc = drcLIGHT;
  else if ("Grey" == sColoring) 
    drc = drcGREY;
  else
    drc = drcNORMAL;

	view->ReadElement(sSection, "UseCustomLegendRange", m_fCustomLegendValueRange);
	view->ReadElement(sSection, "CustomLegendValueRange", m_vrLegend);

  if (drm == drmMULTIPLE && iMultColors == 3)
    InitClrRandom();
}

BaseMapDrawer::~BaseMapDrawer()
{
  bmap = 0;
}

Domain BaseMapDrawer::dm() const
{
  return _dm;
}

Representation BaseMapDrawer::rpr() const
{
  return _rpr;
}

void BaseMapDrawer::SetCustomLegendValueRange(ValueRange vr)
{
	m_vrLegend = vr;
	m_fCustomLegendValueRange = true;
}

DomainValueRangeStruct BaseMapDrawer::dvrs() const
{
	if (fAttTable)
		return colAtt->dvrs();
	else if (m_fCustomLegendValueRange)
		return DomainValueRangeStruct(m_vrLegend);
	else
		return bmap->dvrs();
}

RangeReal BaseMapDrawer::rrStretchRange() const
{
  return rrStretch;
}

RangeReal BaseMapDrawer::rrLegendRange() const
{
	if (m_fCustomLegendValueRange)
		return m_vrLegend->rrMinMax();
	else
		return rrStretchRange();
}

Color BaseMapDrawer::clrRaw(long iRaw) const
{
  if (iUNDEF == iRaw)
    return mcd->colBackground;
  Color cRet;
  switch (drm) {
    case drmRPR:
      if (_dm->pdv()) {
        double rVal = bmap->dvrs().rValue(iRaw);
        return clrVal(rVal);
      }
      else
        cRet = Color(rpr()->clrRaw(iRaw));
      break;
    case drmSINGLE:
      if ((long)clr == -1)
        cRet = SysColor(COLOR_WINDOWTEXT);
      else
        cRet = clr;
      break;
    case drmMULTIPLE: 
      if (3 == iMultColors)
        cRet = clrRandom(iRaw);
      else {
        int iStep = 7;
        switch (iMultColors) {
          case 0: iStep = 7; break;
          case 1: iStep = 15; break;
          case 2: iStep = 31; break;
        }
        cRet = clrPrimary(1+iRaw%iStep);
      }  
      break;
    case drmIMAGE: {
      int iMin = 0, iMax = 255;
      if (fStretch) {
        iMin = riStretch.iLo();
        iMax = riStretch.iHi();
      }
      int iDiff = iMax - iMin;
      if (iRaw < iMin)
        iRaw = 0;
      else if (iRaw > iMax)
        iRaw = iMax;
      int iVal = (int)(floor(255 * float(iRaw - iMin) / iDiff));
      cRet = Color(iVal,iVal,iVal);
    } break;
    case drmCOLOR:
      cRet = Color(iRaw);
      break;
    case drmBOOL:
      switch (iRaw) {
        case 0: return mcd->colBackground;
        case 1: cRet = clr0; break;
        case 2: cRet = clr;  break;
      }
      break;
  }
  if ((long)cRet < 0)
    return mcd->colBackground;
  else
    return clrDraw(cRet);
}

Color BaseMapDrawer::clrVal(double rVal) const
{
  Color cRet;
  if (!_rpr.fValid())
    return cRet;
  if (fStretch) {
    switch (sm)
    {
      case smLINEAR:
        cRet = (Color)_rpr->clr(rVal, rrStretchRange());
        break;
      case smLOGARITHMIC:
      {
        RangeReal rr = rrStretchRange();
        double rMax = 1 + rr.rHi() - rr.rLo();
        rMax = log(rMax);
        rVal -= rr.rLo();
        rVal = log(rVal);
        rr = RangeReal(0, rMax);
        cRet = (Color)_rpr->clr(rVal, rr);
      } break;
    }
  }
  else if (drmIMAGE == drm)
    cRet = (Color)_rpr->clr(rVal,RangeReal(0,255));
  else
    cRet = (Color)_rpr->clr(rVal);
  return clrDraw(cRet);
}

void BaseMapDrawer::WriteLayer(MapView& view, const char* sSection)
{
  Drawer::WriteLayer(view,sSection);
  String sStretchMethod;
  switch (sm)
  {
    case smLINEAR: sStretchMethod = "Linear"; break;
    case smLOGARITHMIC: sStretchMethod = "Logarithmic"; break;
  }
  if (fAttTable) 
    view->WriteElement(sSection, "Attribute", colAtt->sName());
  else
    view->WriteElement(sSection, "Attribute", (char*)0);
  switch (drm) {
    case drmRPR:
      view->WriteElement(sSection, "Method", "Representation");
      view->WriteElement(sSection, "Representation", _rpr->sName());
      view->WriteElement(sSection, "Stretch", fStretch);
      if (fStretch)
        if (_dm->pdvi())
          view->WriteElement(sSection, "Range", riStretch);
        else  
          view->WriteElement(sSection, "Range", rrStretch);
      view->WriteElement(sSection, "Stretch Method", sStretchMethod);
      break;
    case drmIMAGE:
      view->WriteElement(sSection, "Method", "Image");
      view->WriteElement(sSection, "Stretch", fStretch);
      view->WriteElement(sSection, "Range", riStretch);
      view->WriteElement(sSection, "Stretch Method", sStretchMethod);
      break;
    case drmSINGLE:
      view->WriteElement(sSection, "Method", "SingleColor");
      view->WriteElement(sSection, "Color", (Color)clr);
      break;
    case drmMULTIPLE:
      view->WriteElement(sSection, "Method", "MultipleColor");
      view->WriteElement(sSection, "Colors", iMultColors);
      break;
    case drmBOOL:  
      view->WriteElement(sSection, "Method", "DualColor");
      view->WriteElement(sSection, "Color", (Color)clr);
      view->WriteElement(sSection, "Color0", (Color)clr0);
      break;
    case drmBYATTRIBUTE:
      view->WriteElement(sSection, "Method", "ByAttribute");
      view->WriteElement(sSection, "Representation", _rpr->sName());
      view->WriteElement(sSection, "Stretch", fStretch);
      if (fStretch)
        view->WriteElement(sSection, "Range", rrStretch);
      break;
    case drmARROW:
      view->WriteElement(sSection, "Method", "Arrow");
      break;
    case drmGRAPH:
      view->WriteElement(sSection, "Method", "Graph");
      view->WriteElement(sSection, "Stretch", fStretch);
      view->WriteElement(sSection, "Range", rrStretch);
      break;
  }    
  switch (drc) {
    case drcLIGHT:
      view->WriteElement(sSection, "Coloring", "Light");
      break;
    case drcNORMAL:
      view->WriteElement(sSection, "Coloring", "Normal");
      break;
    case drcDARK:
      view->WriteElement(sSection, "Coloring", "Dark");
      break;
    case drcGREY:
      view->WriteElement(sSection, "Coloring", "Grey");
      break;
  }

	view->WriteElement(sSection, "UseCustomLegendRange", m_fCustomLegendValueRange);
	view->WriteElement(sSection, "CustomLegendValueRange", m_vrLegend);
}

String BaseMapDrawer::sName()
{
  return bmap->sName(true);
}

bool BaseMapDrawer::fAttributes()
{
  return bmap->fTblAtt();
}

bool BaseMapDrawer::fRepresentation()
{
  if (drm != drmRPR)
    return false;
  if (!_rpr.fValid() || _rpr->fDataReadOnly())
    return false;
  return true;  
}

bool BaseMapDrawer::fProperty()
{
  return true;
}

void BaseMapDrawer::Prop()
{
	Drawer::Prop();
/*
  Domain dmOld = bmap->dm();
  zFrameWin* win = static_cast<zFrameWin*>(mppn->parent());
  PropInfForm(win, bmap->fnObj);
  // in case mapwindow is magically closed when still in property form
  // e.g. CloseAll from main window.
  if (0 == bmap)
    DummyError();
  if (dmOld != bmap->dm())
    Configure();
*/
}



bool BaseMapDrawer::fEditable()
{
	MapPaneView* mpv = mcd->mpvGetView();
	if (0 == mpv)
		return false;
  return !bmap->fDataReadOnly();
}

bool BaseMapDrawer::fEditDomain()
{
  if (bmap->dm()->fDataReadOnly())
    return false;
  if (0 != bmap->dm()->pdsrt())
    return true;
  if (0 != bmap->dm()->pdvi())
    return true;
  if (0 != bmap->dm()->pdvr())
    return true;
  return false;  
}

void BaseMapDrawer::EditDomain()
{
  FileName fn = bmap->dm()->fnObj;
	String sFileName = fn.sFullNameQuoted();
	IlwWinApp()->OpenDocumentAsDomain(sFileName.scVal());
}

void BaseMapDrawer::EditRepresentation()
{
  if (!_rpr.fValid())
    return;
  if (_rpr->prc() || _rpr->prv() || _rpr->prg()) {
    FileName fn = _rpr->fnObj;
		String sFileName = fn.sFullNameQuoted();
		String sOption = bmap->fnObj.sExt;
		sOption[0] = '-';
		String sCmd("show %S %S", sFileName, sOption);
		IlwWinApp()->Execute(sCmd.scVal());
  }
  else
    MessageBox(*mcd->wndGetActiveView(), SDCMsgRprNotEditable.scVal(), SDCMsgEditRpr.scVal(), MB_OK);
}

void BaseMapDrawer::ExecuteAction(const Coord& crd)
{
  if (bmap->fTblAtt()) {
    Table tbl = bmap->tblAtt();
    Column col = tbl["action"];
    if (col.fValid()) {
      long iRaw = bmap->iRaw(crd);
      String sAction = col->sValue(iRaw,0);
      if ("" != sAction) {
        String sDir = tbl->fnObj.sPath();
        ShellExecute(*AfxGetMainWnd(), NULL, sAction.scVal(), NULL, sDir.scVal(), SW_SHOW);
      }
    }
  }
}

Color BaseMapDrawer::clrDraw(Color c) const
{
	return c.clrDraw(drc);
}

Ilwis::Record BaseMapDrawer::rec(const Coord& crd)
{
  long iRec = bmap->iRaw(crd);
  if (iRec > 0) {
    Table tbl = bmap->tblAtt();
    if (tbl.fValid()) 
      return tbl->rec(iRec);
  }  
  return Ilwis::Record();
}

bool BaseMapDrawer::fShowDisplayOptions()
{
	IlwisSettings settings("DefaultSettings");
	String sFlags = settings.sValue("ShowDisplayOptions");
	Array<String> arFlags;
	Split(sFlags, arFlags, ",");
	if (arFlags.iSize() < IlwisWinApp::dosEND)
		return true;
	if ( arFlags[IlwisWinApp::dosALL] == "1" )
		return true;
	IlwisObject::iotIlwisObjectType iotType = IlwisObject::iotObjectType(bmap->fnObj);
	int iBase;
	switch(iotType)
	{
		case IlwisObject::iotRASMAP:
			iBase = IlwisWinApp::dosRAS; 
			if ( arFlags[IlwisWinApp::dosRASIMG] == "1" && _dm->pdi()) // special case, only with raster
				return true;
			break;
		case IlwisObject::iotPOLYGONMAP:
				iBase = IlwisWinApp::dosPOL;
				break;
		case IlwisObject::iotSEGMENTMAP:
				iBase = IlwisWinApp::dosSEG;
				break;
		case IlwisObject::iotPOINTMAP:
				iBase = IlwisWinApp::dosPNT;
				break;
		default:
			return false;
	}
	if ( arFlags[iBase] == "1")
		return true;
	if ( iotType == IlwisObject::iotRASMAP)
		++iBase; // this skips dosRASIMG which has been handled already
	if ( arFlags[iBase + 1] == "1" && _dm->pdv())
		return true;
	if ( arFlags[iBase + 2] == "1" && _dm->pdsrt())
		return true;
	if ( arFlags[iBase + 3] == "1" && _dm->pdbool())
		return true;
	if ( arFlags[iBase + 4] == "1" && 
	   (_dm->pdbit() || _dm->pdgrp() || _dm->pdcol() || _dm->pdUniqueID() || _dm->pdp()))
		return true;

	return false;
}

bool BaseMapDrawer::fLegendUseful() const
{
	return drm != drmSINGLE;
}

void BaseMapDrawer::InitClrRandom()
{ 
  m_clrRandom.clear();
  DomainSort* ds = dm()->pdsrt();
  int iSize = 1000;
  if (ds)
    iSize = 1 + ds->iSize();
  if ( iSize < 0)
	  return;
  m_clrRandom.resize(iSize);
  for (int i = 0; i < iSize; ++i)
  {
    Color clr;
    clr.red() = (byte)random(256);
    clr.green() = (byte)random(256);
    clr.blue() = (byte)random(256);
    m_clrRandom[i] = clr;
  }
}

Color BaseMapDrawer::clrRandom(int iRaw) const
{
  iRaw = abs(iRaw);
  if (iRaw > m_clrRandom.size())
    iRaw %= m_clrRandom.size();
  return m_clrRandom[iRaw];
}

BaseMapDrawerForm::BaseMapDrawerForm(BaseMapDrawer* bmdr, const String& sTitle, bool fShowForm)
  : DrawerForm(bmdr, sTitle), bmd(bmdr),
    fgValue(0), fgClass(0), fgID(0), fgBool(0), fgColor(0)
{
	if (!fShowForm) 
		NotShowAlways();

	bm = bmd->bmap;
	tblAtt = bm->tblAtt();
	FieldGroup* fg = new FieldGroup(root,true);
	String ss = bm->sTypeName();
	replace(ss.begin(), ss.end(), '\"', ' ');
	StaticText *st = new StaticText(fg, ss, false, true); // true: do not use & as accelerator
	st->SetIndependentPos();

	CFont *font = IlwWinApp()->GetFont(IlwisWinApp::sfFORM);
	LOGFONT lfont;
	font->GetLogFont(&lfont);
	lfont.lfHeight *= 1.2;
	lfont.lfWeight = FW_SEMIBOLD;
	CFont *fnt2 = new CFont();
	fnt2->CreateFontIndirect(&lfont);
	st->Font(fnt2);

	String s = bm->sDescription;

	FieldString *fs = new FieldString(fg, "", &s, Domain(), true, ES_AUTOHSCROLL|WS_GROUP|ES_READONLY);
	zDimension dim = fs->Dim(s);
	fs->SetWidth(180);
	fs->SetIndependentPos();

  dmMap = bm->dm();
  if (dmMap->fValidDomain())
		if (0 == dmMap->pdUniqueID() && dmMap->fnObj == bm->fnObj)
      st = new StaticText(fg, SDCRemInternalDomain);
    else  
      st = new StaticText(fg, dmMap->sTypeName());
  else  
    st = new StaticText(fg, SDCInfMapWithInvalidDomain);
  st->SetIndependentPos();
  dmUse = dmMap;
  new CheckBox(fg, SDCUiInfo, &bmd->fSelectable);
    
  if (bmd->_rpr.fValid())
    sRprName = bmd->_rpr->sName(true);
  fAtt = false;  
//  fRprLink = false;
  cbAtt = 0;  
  stStretch = 0;
  fri = 0;
  frr = 0;
  rbRpr = 0;
  rbSingle = 0;
  rbMultiple = 0;
  fldRprValue = 0;
  fldRprClass = 0;

  ValueRange vr = bm->vr();
  if (bm->dm()->pdbool())
    vr = ValueRange();
  bool fImage = 0 != bm->dm()->pdi();
  if (vr.fValid() || fImage) {
    StaticText* st = 0;
    if (!fImage && 0 != vr->vrr()) {
      RangeReal rr = bm->rrMinMax();
      if (rr.rHi() != rUNDEF && rr.rLo() <= rr.rHi()) {
//        DomainValueReal* dvr = bm->dm()->pdvr();
        String s = String(SDCInfMinMax_SS.scVal(),
                   bm->dvrs().sValue(rr.rLo()), bm->dvrs().sValue(rr.rHi()));
        st = new StaticText(root, s);
      }
    }
    else {
      RangeInt ri = bm->riMinMax();
      if (ri.iHi() != iUNDEF && ri.iLo() <= ri.iHi()) {
        String s = String(SDCInfMinMax_ll.scVal(), ri.iLo(), ri.iHi());
        st = new StaticText(root, s);
      }
    }
    if (st)
      st->SetIndependentPos();
  }

	AskScaleLimits();
}

DomainType BaseMapDrawerForm::dmtUse() const
{
  if (dmUse->pdc())
    return dmtCLASS;
  if (dmUse->pdid())  
    return dmtID;
  if (dmUse->pdvi())
    return dmtVALUE;
  if (dmUse->pdvr())
    return dmtVALUE;
  if (dmUse->pdi())
    return dmtIMAGE;
  if (dmUse->pdp())
    return dmtPICTURE;
  if (dmUse->pdbit())
    return dmtBIT;
  if (dmUse->pdbool())
    return dmtBOOL;
  if (dmUse->pdcol())
    return dmtCOLOR;  
  if (dmUse->pds())
    return dmtSTRING;
  throw ErrorObject(SDCRemImpDom, 1000);      
}

int BaseMapDrawerForm::exec()
{
  FormWithDest::exec();
  bmd->fAttTable = fAtt;
  if (bmd->rrStretch.rHi() < bmd->rrStretch.rLo() + 1e-20)
    bmd->rrStretch.rHi() = bmd->rrStretch.rLo() + 1;
  if (bmd->riStretch.iHi() <= bmd->riStretch.iLo())
    bmd->riStretch.iHi() = bmd->riStretch.iLo() + 1;
  if (fAtt) {
		bmd->tblAtt = tblAtt;
    bmd->colAtt = tblAtt->col(sAttCol);
    if (!bmd->colAtt->fCalculated())
      bmd->colAtt->Calc();
//    ColumnVirtual* cv = bmd->colAtt->pcv();
//    if (cv)
//      cv->Freeze();
  }  
	else
		bmd->tblAtt = Table();
  bmd->fValue = 0 != dmUse->pdvi() || 0 != dmUse->pdvr();
  bmd->_dm = dmUse;
  if (!dmUse->pdvr() && dmUse->pdv()) {
    bmd->rrStretch.rLo() = bmd->riStretch.iLo(); 
    bmd->rrStretch.rHi() = bmd->riStretch.iHi(); 
  }
  if (dmUse->pdv() && stStretch) {
    FileName fn = sRprName;
    Representation rpr(fn);
    bmd->fStretch = 0 == rpr->prv();
  }  
  bmd->InitClrRandom();
  return 0;
}

void BaseMapDrawerForm::InitAtt(bool fAllowString)
{
  if (bm->fTblAtt()) { 
    fAtt = bmd->fAttTable;
    tblAtt = bm->tblAtt();
    if (fAtt)
      if (bmd->colAtt.fValid())
        sAttCol = bmd->colAtt->sName();  
      else
        fAtt = false;  
    cbAtt = new CheckBox(root, SDCUiAttribute, &fAtt);
    FieldGroup* fgAtt = new FieldGroup(cbAtt);
    long types = dmCLASS|dmVALUE|dmIMAGE|dmPICT|dmBOOL|dmIDENT|dmCOLOR|dmUNIQUEID;
    if (fAllowString)
      types |= dmSTRING;
    fcAtt = new FieldColumn(fgAtt, "", tblAtt, &sAttCol, types);
    fcAtt->Align(cbAtt,AL_AFTER);
    String sFill('x', 50);
    stAttDom = new StaticText(fgAtt, sFill);
    stAttDom->SetIndependentPos();
    stAttDom->Align(cbAtt,AL_UNDER);
    cbAtt->SetCallBack((NotifyProc)&BaseMapDrawerForm::AttCallBack);
    fcAtt->SetCallBack((NotifyProc)&BaseMapDrawerForm::AttCallBack);
  }  
  else {
    cbAtt = 0;
    stAttDom = 0;
    FormEntry* fe = new FieldBlank(root,0);
    fe->SetCallBack((NotifyProc)&BaseMapDrawerForm::AttCallBack);
/*
    stAttDom = new StaticText(root, "");
    stAttDom->SetCallBack((NotifyProc)&BaseMapDrawerForm::AttCallBack);
*/
  }  
}

int BaseMapDrawerForm::AttCallBack(Event*)
{
  String sOldAtt = sAttCol;
  bool fOldAtt = fAtt;
  if (cbAtt) {
    cbAtt->StoreData();
    fldRprClass->AllowRprAttr(!fAtt);
  }  
  if (fAtt) {
    HCURSOR hCurSave = GetCursor();
    SetCursor(LoadCursor(NULL, IDC_WAIT));
    Column colAtt;
    try {
      Table tblAtt = bm->tblAtt();
      if (tblAtt.fValid())
        colAtt = tblAtt->col(sAttCol);
    }
    catch (ErrorObject& err) {
      err.Show();
    }  
    if (colAtt.fValid()) {
      dmUse = colAtt->dm();
      stAttDom->SetVal(dmUse->sTypeName());
      if ((!fOldAtt || sOldAtt != sAttCol) && 0 != dmUse->pdv()) {
        if (fri) 
          fri->SetVal(colAtt->riMinMax());
        if (frr)  
          frr->SetVal(colAtt->rrMinMax());
        RprValueCallBack(0);
      }
    }
    else {
      fAtt = false;
      sAttCol = "";
      stAttDom->SetVal(SDCRemNoValidCol);
    }
    SetCursor(hCurSave);
  }
  if (!fAtt)  
    dmUse = dmMap;
  if (fldRprValue)
    fldRprValue->SetDomain(dmUse);
  if (fldRprClass)
    fldRprClass->SetDomain(dmUse);
  DomUseChanged();  
  return 0;
}

void BaseMapDrawerForm::InitStretch(FormEntry* feParent)
{
  stStretch = new StaticText(feParent, SDCUiStretch); 
  if (rbMultiple)
    stStretch->Align(rbMultiple, AL_UNDER);
  else if (rbSingle)  
    stStretch->Align(rbSingle, AL_UNDER);
  else  
    stStretch->Align(feParent, AL_UNDER);
//  cbStretch->SetCallBack((NotifyProc)&BaseMapDrawerForm::StretchCallBack);
//  feParent->SetCallBack((NotifyProc)&BaseMapDrawerForm::StretchCallBack);
  if (bmd->riStretch.iHi() <= bmd->riStretch.iLo())
    bmd->riStretch.iHi() = bmd->riStretch.iLo() + 1;
  if (bmd->rrStretch.rHi() <= bmd->rrStretch.rLo())
    bmd->rrStretch.rHi() = bmd->rrStretch.rLo() + 1;
  fri = new FieldRangeInt(feParent, "", &bmd->riStretch);
  fri->Align(stStretch, AL_AFTER);
  frr = new FieldRangeReal(feParent, "", &bmd->rrStretch);
  frr->Align(stStretch, AL_AFTER);
}
 

int BaseMapDrawerForm::StretchCallBack(Event*)
{
  if (0 == frr || 0 == fri)
    return 0;
	if ( !dmUse->pdv() )
		stStretch->Hide();
  if (stStretch->fShow()) {
    if (dmUse->pdvr()) {
      fri->Hide();
      frr->Show();
    }
    else {
      fri->Show();
      frr->Hide();
    }
  }
  else {
    fri->Hide();
    frr->Hide();
  }
  return 0;
}

void BaseMapDrawerForm::InitRprValue(RadioGroup* rgParent)
{
  rbRpr = new RadioButton(rgParent, SDCUiRpr);
  fldRprValue = new FieldRepresentationC(rbRpr, "", &sRprName, dmUse);
  fldRprValue->SetCallBack((NotifyProc)&BaseMapDrawerForm::RprValueCallBack);
  rbRpr->SetCallBack((NotifyProc)&BaseMapDrawerForm::RprValueCallBack);
}


void BaseMapDrawerForm::InitRprClass(RadioGroup* rgParent)
{
  RadioButton* rb = new RadioButton(rgParent, SDCUiRpr);
  fldRprClass = new FieldRepresentationC(rb, "", &sRprName, dmUse);
}

void BaseMapDrawerForm::InitBool()
{
  fgBool = new FieldGroup(root);
  new FieldColor(fgBool, SDCUiColorTrue, &bmd->clr);
  new FieldColor(fgBool, SDCUiColorFalse, &bmd->clr0);
}

int BaseMapDrawerForm::RprValueCallBack(Event*)
{
  if (0 == stStretch)
    return 0;
  fldRprValue->StoreData();
  if (rbRpr && !rbRpr->fVal())
    return 0;
  try {
    FileName fn = sRprName;
    Representation rpr(fn);
		RepresentationValue* rpv = rpr->prv();
    if (rpv) {
      stStretch->Hide();
			bmd->rrStretch.rLo() = rpv->rGetLimitValue(0);
			bmd->rrStretch.rHi() = rpv->rGetLimitValue(rpv->iLimits()-1);
		}
    else 
      stStretch->Show();
    StretchCallBack(0);  
  }
  catch (ErrorObject&) {}
  return 0;
}

void BaseMapDrawerForm::InitSingle(RadioGroup* rgParent)
{
  rbSingle = new RadioButton(rgParent, SDCUiSingleColor);
  FieldColor* fc = new FieldColor(rbSingle, "", &bmd->clr);
  fc->Align(rbSingle, AL_AFTER);
}

void BaseMapDrawerForm::InitMultiple(RadioGroup* rgParent)
{
  rbMultiple = new RadioButton(rgParent, SDCUiMultipleColor);
  RadioGroup* rg = new RadioGroup(rbMultiple, "", &bmd->iMultColors, true);
  rg->Align(rbMultiple, AL_UNDER);
  rg->SetIndependentPos();
  new RadioButton(rg, "&7");
  new RadioButton(rg, "1&5");
  new RadioButton(rg, "&31");
  new RadioButton(rg, "&Random");
}

void BaseMapDrawerForm::InitColoring(RadioGroup* rg)
{
  rg->SetIndependentPos();
  RadioButton *rb = new RadioButton(rg, SDCUiLight);
  new RadioButton(rg, SDCUiNormal);
  new RadioButton(rg, SDCUiDark);
  new RadioButton(rg, SDCUiGrey);
	rb->SetIndependentPos();
}


