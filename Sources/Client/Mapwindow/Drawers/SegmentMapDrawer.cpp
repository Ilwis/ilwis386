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
// SegmentMapDrawer.cpp: implementation of the SegmentMapDrawer class.
//
//////////////////////////////////////////////////////////////////////

#include "Client\Headers\formelementspch.h"
#include "Engine\Base\System\RegistrySettings.h"
#include "Engine\Map\Segment\Seg.h"
#include "Engine\Map\Polygon\POL.H"
#include "Client\Mapwindow\Positioner.h"
#include "Client\Mapwindow\Drawers\BaseDrawer.h"
#include "Client\Mapwindow\Drawers\Drawer.h"
#include "Headers\Hs\Drwforms.hs"
#include "Client\FormElements\fldcol.h"
#include "Client\FormElements\fldrpr.h"
#include "Client\FormElements\fldcolor.h"
#include "Client\Mapwindow\Drawers\BaseMapDrawer.h"
#include "Client\Mapwindow\Drawers\SegmentMapDrawer.h"
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
#include "Client\Editors\Utils\SYMBOL.H"
#include "Client\Editors\Utils\Smbext.h"
#include "Client\Editors\Utils\line.h"
#include "Client\Editors\Representation\RprEditForms.h"
#include "Engine\Representation\Rprclass.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

BEGIN_MESSAGE_MAP(SegmentMapDrawer, BaseMapDrawer)
	//{{AFX_MSG_MAP(SegmentMapDrawer)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


SegmentMapDrawer::SegmentMapDrawer(MapCompositionDoc* mapcd, const SegmentMap& segmap)
  : BaseMapDrawer(mapcd), fMask(false), mask(segmap->dm())
{
  sm = segmap;
  _dm = sm->dm();
  bmap = sm.ptr();
  _rpr = segmap->dm()->rpr();
  iWidth = RepresentationClass::iSIZE_FACTOR;
//  clr = -1; take default from BaseMapDrawer instead
  fSelectable = false;
  if (_rpr.fValid())
    drm = drmRPR;
  else
    drm = drmSINGLE;
  if (_dm->pdvi() || _dm->pdvr()) {
    riStretch = sm->riMinMax(true);
    rrStretch = sm->rrMinMax(true);
		if (rrStretch.rLo() >= rrStretch.rHi())
			rrStretch = sm->vr()->rrMinMax();
    if (riStretch.iLo() >= riStretch.iHi())
      riStretch = sm->vr()->riMinMax();
  }  
}

SegmentMapDrawer::SegmentMapDrawer(MapCompositionDoc* mapcd, const MapView& view, const char* sSection)
  : BaseMapDrawer(mapcd, view, sSection, "SegmentMap"), fMask(false)
{
  FileName fn;
  view->ReadElement(sSection, "SegmentMap", fn);
  sm = SegmentMap(fn);
  mask.SetDomain(sm->dm());
//  dm = sm->dm();
  bmap = sm.ptr();
  iWidth = view->iReadElement(sSection, "Width");
  fSelectable = false;
  String sMask;
  view->ReadElement(sSection, "Mask", sMask);
  if (sMask.length()) {
    fMask = true;
    mask.SetMask(sMask);
  }    
}

void SegmentMapDrawer::WriteLayer(MapView& view, const char* sSection)
{
  view->WriteElement(sSection, "Type", "SegmentMapDrawer");
  FileName fn = sm->sName(true);
  view->WriteElement(sSection, "SegmentMap", fn);
  BaseMapDrawer::WriteLayer(view, sSection);
  view->WriteElement(sSection, "Width", (long)iWidth);
  if (fMask)
    view->WriteElement(sSection, "Mask", mask.sMask());
  else  
    view->WriteElement(sSection, "Mask", (char*)0);
}

SegmentMapDrawer::~SegmentMapDrawer()
{
  ILWISSingleLock csl(&cs, TRUE, SOURCE_LOCATION);
}

bool SegmentMapDrawer::fSegInMask(const ILWIS::Segment* seg) const
{
  if (fMask)
	  return seg->fInMask(sm->dvrs(), mask);
  return true;
}

int SegmentMapDrawer::draw(CDC* cdc, zRect rect, Positioner* psn, volatile bool* fDrawStop)
{
	ILWISSingleLock csl(&cs, TRUE, SOURCE_LOCATION);
	if (!fAct) return 0;
	sm->KeepOpen(true);
	bool fSameCsy = sm->cs() == mcd->georef->cs();
	Color c;
	CPen pen, *penOld;
	//  if (drm == drmSINGLE) {
	if ((long)clr == -1)
		c = SysColor(COLOR_WINDOWTEXT);
	else
		c = clr;
	pen.CreatePen(PS_SOLID, iWidth, c);
	penOld = cdc->SelectObject(&pen);
	//  }

	////  zPoint *p = new zPoint[1000];
	Array<zPoint> p;
	long iTot;
	CoordBounds cbIntern;
	CoordBounds cb = cbRect(psn);
	bool fBoundCheck = !cb.fUndef();
	if (fBoundCheck) {
		double rW10 = cb.width() / 10;
		double rH10 = cb.height() /10;
		cb.MinX() -= rW10;
		cb.MaxX() += rW10;
		cb.MinY() -= rH10;
		cb.MaxY() += rH10;
		cbIntern += cb.cMin;
		cbIntern += cb.cMax;
		if (!fSameCsy) {
			double dX = cb.width() / 10;
			double dY = cb.height() / 10;
			Coord c;
			int i, j;
			for (i = 0, c.x = cb.MinX(); i < 10; c.x += dX, ++i)
				for (j = 0, c.y = cb.MinY(); j < 10; c.y += dY, ++j) {
					Coord crd = sm->cs()->cConv(mcd->georef->cs(), c);
					if (!crd.fUndef())
						cbIntern += crd;
				}  
		}      
	}  
	riTranquilizer = RangeInt(1,sm->iFeatures());
	iTranquilizer = 0;
	for(int i = 0; i < sm->iFeatures(); ++i) {
		ILWIS::Segment *s = (ILWIS::Segment *)sm->getFeature(i);
		if ( s == NULL || s->fValid() == false)
			continue;

		++iTranquilizer;
		if (*fDrawStop)
			break;
		if (!fBoundCheck || cbIntern.fContains(s->cbBounds())) {
			if (fMask) {
				if (!fSegInMask(s))
					continue;
			}
			double rVal;
			long iRaw;
			if (sm->dm()->pdvi() || sm->dm()->pdvr()) {
				rVal = s->rValue();
				iRaw = longConv(rVal); // for multiple colors
				if (rVal == rUNDEF) continue;
			}
			else {
				iRaw = s->iValue();
				if (iRaw == iUNDEF) continue;
				if (fAttTable) {
					if (colAtt->dm()->pdvi() || colAtt->dm()->pdvr()) {
						rVal = colAtt->rValue(iRaw);
						iRaw = longConv(rVal); // for multiple colors
						if (rVal == rUNDEF) continue;
					}
					else {
						iRaw = colAtt->iRaw(iRaw);
						if (iRaw == iUNDEF) continue;
					}
				}
			}
			CPen* penO = 0;
			if (drm != drmSINGLE) {
				p.Resize(0);
				Color c;
				int iW = iWidth;
				switch (drm) {
		  case drmRPR:
			  if (_dm->pdv()) {
				  if (fStretch)
					  c = _rpr->clr(rVal,rrStretch);
				  else  
					  c = _rpr->clr(rVal);
			  }   
			  else {
				  iTot = psn->iSegPos(s,p,sm->cs());
				  if (iTot > 1) {
					  Line line(_rpr, iRaw);
					  line.clrLine() = clrDraw(line.clrLine());
					  line.clrLineFill() = clrDraw(line.clrLineFill());
					  line.drawLine(cdc, p.buf(), iTot);
				  }
				  continue;
				  //              c = (zColor)_rpr->clrRaw(iRaw);
				  //              iW = _rpr->iWidth(iRaw);
			  }  
			  break;
		  case drmBOOL:
			  if (0 == iRaw)
				  continue;
			  // fall through
		  case drmCOLOR:
			  // fall through
		  case drmMULTIPLE: 
			  c = clrRaw(iRaw);
			  break;
				}
				if ((long)c == iUNDEF || iW < 0)
					continue;
				cdc->SelectObject(penOld);
				pen.DeleteObject();
				pen.CreatePen(PS_SOLID, iW, c);
				penO = cdc->SelectObject(&pen);
			}
			iTot = psn->iSegPos(s,p,sm->cs());      
			if (iTot > 1)
				cdc->Polyline(p.buf(), iTot/*+1*/);
			if (drm != drmSINGLE) 
				cdc->SelectObject(penO);
		}
	}
	//  delete p;
	//  if (drm == drmSINGLE) 
	cdc->SelectObject(penOld);
	sm->KeepOpen(false);
	return 0;
}
/*
bool SegmentMapDrawer::fProperty()
{
  return 0 != sm->psms();
}
*/
/*
void SegmentMapDrawer::Prop()
{
  if (!fProperty())
    return;
  zFrameWin* win = static_cast<zFrameWin*>(mppn->parent());
  PropInfForm(win, sm);
}
*/


class SegmentMapDrawerForm: public BaseMapDrawerForm
{
public:
  SegmentMapDrawerForm(SegmentMapDrawer* mdr, bool fShowForm=true);
  void DomUseChanged();
  int exec();
private:
  int iValueOption;
  int iClassOption;
  int iIdentOption;
  SegmentMapDrawer* mdw;
  String sMask;
};

SegmentMapDrawerForm::SegmentMapDrawerForm(SegmentMapDrawer* mdr, bool fShowForm)
: BaseMapDrawerForm(mdr, SDCTitleSegMap, fShowForm), mdw(mdr)
{
	iImg = IlwWinApp()->iImage(".mps");

  CheckBox* cbMask = new CheckBox(root, SDCUiMask, &mdw->fMask);
  sMask = mdw->mask.sMask();
  FieldString* fs = new FieldString(cbMask, "", &sMask);
	fs->SetWidth(80);
  FieldBlank* fb = new FieldBlank(root, 0);
  fb->Align(cbMask, AL_UNDER);
  bool fTbl = bm->fTblAtt();
//  if (fTbl)
    InitAtt();
  if (fTbl || bm->dm()->pdvi() || bm->dm()->pdvr()) {  
    iValueOption = (int) mdw->drm;
    FieldGroup* fg = new FieldGroup(root);
    if (fTbl)
      fg->Align(stAttDom, AL_UNDER);
    RadioGroup* rgValue = new RadioGroup(fg, "", &iValueOption);
    InitRprValue(rgValue);
    InitSingle(rgValue);
    InitMultiple(rgValue);
    InitStretch(rbRpr);
    FieldInt* fi = new FieldInt(fg, SDCUiLineWidth, &mdw->iWidth, ValueRangeInt(1L,100L));
    fi->Align(stStretch, AL_UNDER);
    RadioGroup* rg = new RadioGroup(fg, "", (int*)&mdw->drc, true);
    InitColoring(rg);
    fgValue = fg;
  }
  if (fTbl || bm->dm()->pdc()) {  
    if (mdw->drm == drmSINGLE)
      iClassOption = 1;
    else
      iClassOption = 0;
//    iClassOption = (int) mdw->drm;
    FieldGroup* fg = new FieldGroup(root);
    RadioGroup* rgClass = new RadioGroup(fg, "", &iClassOption);
    if (fTbl)
      rgClass->Align(stAttDom, AL_UNDER);
//    new RadioButton(rgClass, SDUiRpr);
    InitRprClass(rgClass);
    RadioButton* rb = new RadioButton(rgClass, SDCUiSingleColor);
    FieldGroup* fgSingle = new FieldGroup(rb);
    FieldColor* fc = new FieldColor(fgSingle, "", &mdw->clr);
    fc->Align(rb, AL_AFTER);
    FieldInt* fi = new FieldInt(fgSingle, SDCUiLineWidth, &mdw->iWidth, ValueRangeInt(1L,100L));
    fi->Align(rb, AL_UNDER);
    RadioGroup* rg = new RadioGroup(fg, "", (int*)&mdw->drc, true);
    InitColoring(rg);
    fgClass = fg;
  }  
  if (fTbl || bm->dm()->pdid()) {
    if (mdw->drm == drmMULTIPLE)
      iIdentOption = 1;
    else
      iIdentOption = 0;
//    iIdentOption = (int) mdw->drm - 1;
    FieldGroup* fgIdent = new FieldGroup(root);
    if (fTbl)
      fgIdent->Align(stAttDom, AL_UNDER);
    RadioGroup* rg = new RadioGroup(fgIdent, "", &iIdentOption);
    InitSingle(rg);
    InitMultiple(rg);
    new FieldInt(fgIdent, SDCUiLineWidth, &mdw->iWidth, ValueRangeInt(1L,100L));
    RadioGroup* rgCol = new RadioGroup(fgIdent, "", (int*)&mdw->drc, true);
    InitColoring(rgCol);
    fgID = fgIdent;
  }  
  if (fTbl || bm->dm()->pdbool()) {
    InitBool();
    if (fTbl)
      fgBool->Align(stAttDom, AL_UNDER);
    RadioGroup* rg = new RadioGroup(fgBool, "", (int*)&mdw->drc, true);
    InitColoring(rg);
  }
	if (fTbl) {
    fgColor = new FieldGroup(root);
    fgColor->Align(stAttDom, AL_UNDER);
    RadioGroup* rg = new RadioGroup(fgColor, "", (int*)&mdw->drc, true);
    InitColoring(rg);
	}
  SetMenHelpTopic(htpCnfSegmentMapDrawer);
  create();
}

void SegmentMapDrawerForm::DomUseChanged()
{
  if (fgValue) fgValue->Hide();
  if (fgClass) fgClass->Hide();
  if (fgID) fgID->Hide();
  if (fgBool) fgBool->Hide();
  if (fgColor) fgColor->Hide();
  switch (dmtUse()) {
    case dmtCLASS: {
      Representation rpr;
      if (mdw->_dm == dmUse)
        rpr = mdw->_rpr;
      if (!rpr.fValid())
        rpr = dmUse->rpr();
      if (rpr.fValid()) {
        sRprName = rpr->sName(true);
        fldRprClass->SetVal(sRprName);
      }  
      fgClass->Show();
    } break;
    case dmtID:
      fgID->Show();
      break;
    case dmtVALUE:  
      { Representation rpr;
        if (mdw->_dm == dmUse)
          rpr = mdw->_rpr;
        if (!rpr.fValid())
          rpr = dmUse->rpr();
        if (rpr.fValid())
          sRprName = rpr->sName(true);
        else
          sRprName = "gray.rpr";
      }    
      fldRprValue->SetVal(sRprName);
      //RprCallBack(0);    
      StretchCallBack(0);
      fgValue->Show();
      break;
    case dmtBOOL:
      fgBool->Show();
      break;
    case dmtCOLOR:
      fgColor->Show();
      break;
  }
}

int SegmentMapDrawerForm::exec()
{
  BaseMapDrawerForm::exec();
  if (mdw->fMask)
    mdw->mask.SetMask(sMask);
  mdw->_rpr = Representation();
  switch (dmtUse()) {
    case dmtCLASS:
      mdw->drm = (DrawMethod) iClassOption;
      if (mdw->drm == drmRPR) {
        mdw->_rpr = Representation(FileName(sRprName));
        dmUse->SetRepresentation(mdw->_rpr);
      }  
//      if (mdw->_rpr->pra())
//        mdw->_rpr = Representation(mdw->_rpr, mdw->sm->tblAtt());
      break;
    case dmtID:
      if (iIdentOption) 
        mdw->drm = drmMULTIPLE;
      else  
        mdw->drm = drmSINGLE;
      break;
    case dmtVALUE:
      mdw->drm = (DrawMethod) iValueOption;
      if (mdw->drm == drmRPR) {
        mdw->_rpr = Representation(FileName(sRprName));
        dmUse->SetRepresentation(mdw->_rpr);
      }  
      break;
    case dmtBOOL:
      mdw->drm = drmBOOL;
      break;
    case dmtCOLOR:
      mdw->drm = drmCOLOR;
      break;
  }
  if (mdw->drm == drmRPR) {
    if (!mdw->_rpr.fValid())
      mdw->drm = drmSINGLE;
  }    
  return 0;
}
  


int SegmentMapDrawer::Configure(bool fShowForm)
{
  bool fShow = fAct;
  bool fOk;
  if (fNew) {
    fAct = false;  // to prevent "in between" drawing
    fNew = false;
  }
  {
		iWidth /= RepresentationClass::iSIZE_FACTOR;
    SegmentMapDrawerForm frm(this, fShowForm);
		iWidth *= RepresentationClass::iSIZE_FACTOR;
    fOk = frm.fOkClicked();
    if (fOk)
      fShow = true;
  }  	     
  if (fOk) {
    fShow = true;
    Setup();
  }
  fAct = fShow;
  return fOk;
}

int SegmentMapDrawer::Setup()
{
  return 0;
}

String SegmentMapDrawer::sInfo(const Coord& crd)
{
  ILWIS::Segment *seg = sm->seg(crd);
  if ( seg == NULL)
	  return "";
  if (fMask)
      if (!fSegInMask(seg))
        return "";
	String s = seg->sValue(sm->dvrs());
		if (s == "?")
			return "";
    if (fAttTable) {
      long iRaw = seg->iValue();
      if (iRaw != iUNDEF) {
        String sAtt = colAtt->sValue(iRaw);
        for (int iLen = sAtt.length(); iLen && sAtt[iLen-1] == ' '; --iLen)
          sAtt[iLen-1] = 0;
        s &= ": ";
        s &= sAtt;
      }  
    }
//  }
  return s;
}

/*
void SegmentMapDrawer::Edit()
{
  mppn->SegmentEditStr(sm->sName());
}

Record SegmentMapDrawer::rec(const Coord& crd)
{
  return recFromMap(sm, crd);
}
*/


FormBaseDialog* SegmentMapDrawer::wEditRpr(CWnd* wPar, int iRaw)
{
  if (iRaw > 0 && _rpr.fValid()) {
    RepresentationClass* rpc = _rpr->prc();
    if (rpc) 
      return new SegmentMapRprEditForm(wPar, rpc, iRaw, true);
  }
  return 0;
}

FormBaseDialog* SegmentMapDrawer::wEditRpr(CWnd* wPar, const Coord& crd)
{
  long iRaw = sm->iRaw(crd);
	return wEditRpr(wPar, iRaw);
}


IlwisObject SegmentMapDrawer::obj() const
{
  return sm;
}

zIcon SegmentMapDrawer::icon() const
{
	return zIcon("SegIcon");
}

void SegmentMapDrawer::DrawLegendRect(CDC* cdc, CRect rect, int iRaw) const
{
	CPoint p1, p2;
	p1.x = rect.left;
	p1.y = (rect.top + rect.bottom) / 2;
	p2.x = rect.right;
	p2.y = p1.y;
	Line* line;
	if (drmRPR == drm) 
		line = new Line(_rpr, iRaw);
	else {
		Color c = clrRaw(iRaw);
		line = new Line(c, iWidth);
	}
	line->Resize(1.0/RepresentationClass::iSIZE_FACTOR); // To make the sizes in the MapWindow conform to the Layout
	line->drawLine(cdc, p1, p2);
	delete line;
}

void SegmentMapDrawer::DrawValueLegendRect(CDC* cdc, CRect rect, double rVal) const
{
	if (!_rpr.fValid())
		return;
	CPoint p1, p2;
	p1.x = rect.left;
	p1.y = (rect.top + rect.bottom) / 2;
	p2.x = rect.right;
	p2.y = p1.y;
	Line* line;
	Color c;
  if (fStretch)
    c = _rpr->clr(rVal,rrStretch);
  else  
    c = _rpr->clr(rVal);
  if ((long)c == -2)
		return;
	line = new Line(c, 0.3);
	line->drawLine(cdc, p1, p2);
	delete line;
}
