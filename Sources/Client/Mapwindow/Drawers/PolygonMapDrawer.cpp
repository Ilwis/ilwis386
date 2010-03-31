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
// PolygonMapDrawer.cpp: implementation of the PolygonMapDrawer class.
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
#include "Client\Mapwindow\Drawers\PolygonMapDrawer.h"
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
#include "Client\Editors\Utils\Pattern.h"
#include "Client\Editors\Representation\RprEditForms.h"
#include "Engine\Representation\Rprclass.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

BEGIN_MESSAGE_MAP(PolygonMapDrawer, BaseMapDrawer)
	//{{AFX_MSG_MAP(PolygonMapDrawer)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

PolygonMapDrawer::PolygonMapDrawer(MapCompositionDoc* mapcd, const PolygonMap& polmap)
  : BaseMapDrawer(mapcd), fMask(false), mask(polmap->dm())
{
  pm = polmap;
  _dm = pm->dm();
  bmap = pm.ptr();
  _rpr = polmap->dm()->rpr();
//  clr = Color(128,128,128); //-2;
  fBoundariesOnly = false;
  fBoundaries = true;
  iBoundaryWidth = RepresentationClass::iSIZE_FACTOR;
  fHideSuperfluousBoundaries = false;
  clrBoundary = -1;
  fSelectable = true;
  if (_rpr.fValid())
    drm = drmRPR;
  else
    drm = drmMULTIPLE;
  if (_dm->pdvi() || _dm->pdvr()) {
    riStretch = pm->riMinMax(true);
    rrStretch = pm->rrMinMax(true);
		if (rrStretch.rLo() >= rrStretch.rHi()) {
			rrStretch = pm->vr()->rrMinMax();
      riStretch.iLo() = round(rrStretch.rLo());
      riStretch.iHi() = round(rrStretch.rHi());
		}
    if (drm == drmMULTIPLE)
      drm = drmSINGLE;
  }  
}

PolygonMapDrawer::PolygonMapDrawer(MapCompositionDoc* mapcd, const MapView& view, const char* sSection)
: BaseMapDrawer(mapcd, view, sSection, "PolygonMap"), fMask(false)
{
  FileName fn;
  view->ReadElement(sSection, "PolygonMap", fn);
//  fn.sExt = ".pol";
  pm = PolygonMap(fn);
  mask.SetDomain(pm->dm());
  //  dm = pm->dm();
  bmap = pm.ptr();
  view->ReadElement(sSection, "BoundariesOnly", fBoundariesOnly);
  view->ReadElement(sSection, "Boundaries", fBoundaries);
  iBoundaryWidth = view->iReadElement(sSection, "BoundaryWidth");
  view->ReadElement(sSection, "BoundaryColor", (Color&)clrBoundary);
  String sMask;
  view->ReadElement(sSection, "Mask", sMask);
  view->ReadElement(sSection, "HideSuperfluousBoundaries", fHideSuperfluousBoundaries);
  if (sMask.length()) {
    fMask = true;
    mask.SetMask(sMask);
  }    
}

void PolygonMapDrawer::WriteLayer(MapView& view, const char* sSection)
{
  view->WriteElement(sSection, "Type", "PolygonMapDrawer");
  FileName fn = pm->sName(true);
  view->WriteElement(sSection, "PolygonMap", fn);
  BaseMapDrawer::WriteLayer(view, sSection);
  view->WriteElement(sSection, "BoundariesOnly", fBoundariesOnly);
  view->WriteElement(sSection, "Boundaries", fBoundaries);
  view->WriteElement(sSection, "BoundaryWidth", (long)iBoundaryWidth);
  view->WriteElement(sSection, "BoundaryColor", clrBoundary);
  view->WriteElement(sSection, "HideSuperfluousBoundaries", fHideSuperfluousBoundaries);
  if (fMask)
    view->WriteElement(sSection, "Mask", mask.sMask());
  else  
    view->WriteElement(sSection, "Mask", (char*)0);
}

PolygonMapDrawer::~PolygonMapDrawer()
{
  ILWISSingleLock csl(&cs, TRUE, SOURCE_LOCATION);
}

/*
bool PolygonMapDrawer::fProperty()
{
  return 0 != pm->ppms();
}
*/
/*
void PolygonMapDrawer::Prop()
{
  if (!fProperty())
    return;
  zFrameWin* win = static_cast<zFrameWin*>(mppn->parent());
  PropInfForm(win, pm);
}
*/

/*
void PolygonMapDrawer::pushBrush(PolDspType typ, Color color)
{
  int nCol = GetNearestPaletteIndex(mppn->pal->hPal,color);
  if (typ == pFull) {
    int iColors = GetDeviceCaps(*mppn->canvas(), SIZEPALETTE);
    if (iColors < 255)
      mppn->canvas()->pushBrush(new zBrush(color));
    else  
      mppn->canvas()->pushBrush(new zBrush(PALETTEINDEX(nCol)));
  }    
  else {
    switch (typ) {
      case pDownHatch:
        mppn->canvas()->pushBrush(new zBrush(PALETTEINDEX(nCol),DownHatch));
        break;
      case pUpHatch:
        mppn->canvas()->pushBrush(new zBrush(PALETTEINDEX(nCol),UpHatch));
        break;
      case pCrossHatch:
        mppn->canvas()->pushBrush(new zBrush(PALETTEINDEX(nCol),CrossHatch));
        break;
      case pDiagCrossHatch:
        mppn->canvas()->pushBrush(new zBrush(PALETTEINDEX(nCol),DiagCrossHatch));
        break;
      case pHorzHatch:
        mppn->canvas()->pushBrush(new zBrush(PALETTEINDEX(nCol),HorzHatch));
        break;
      case pVertHatch:
        mppn->canvas()->pushBrush(new zBrush(PALETTEINDEX(nCol),VertHatch));
        break;
      default:  
        mppn->canvas()->pushBrush(new zBrush(color));
    }    
  }
}
*/

bool PolygonMapDrawer::fPolInMask(const ILWIS::Polygon *pol) const
{
  if (fMask) {
	  return pol->fInMask(pm->dvrs(),mask);
  }
  return true;
}

int PolygonMapDrawer::draw(CDC* cdc, zRect rect, Positioner* psn, volatile bool* fDrawStop)
{
	ILWISSingleLock csl(&cs, TRUE, SOURCE_LOCATION);
	if (!fAct) return 0;
	pm->KeepOpen(true);
	bool fSameCsy = pm->cs() == mcd->georef->cs();
	cdc->SetBkMode(TRANSPARENT);
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
					Coord crd = pm->cs()->cConv(mcd->georef->cs(), c);
					if (!crd.fUndef())
						cbIntern += crd;
				}  
		}      
	}  
	CoordBuf crdBuf(1000);
	Color c;
	bool fDrawPol = !fBoundariesOnly;
	
	CPen pen, *penOld = 0;

	if (drm == drmSINGLE) {
		if ((long)clr == -2)
			fDrawPol = false;
		else if ((long)clr == -1)
			c = SysColor(COLOR_WINDOWTEXT); 
		else
			c = clr;
	}
	int iTrqMax = 0;
	if (fDrawPol)
		iTrqMax += pm->iFeatures();
	if ((fBoundaries && iBoundaryWidth) || fBoundariesOnly) 
		iTrqMax += pm->iFeatures();
	riTranquilizer = RangeInt(1,iTrqMax);
	iTranquilizer = 0;
	vector<ILWIS::Polygon *> polygons;
	vector<ILWIS::Polygon *> polswithoutholes;
	multimap<double, ILWIS::Polygon *> polswithholes;
	for(int k=0; k < pm->iFeatures(); ++k) {
		ILWIS::Polygon *pol = (ILWIS::Polygon *)pm->getFeature(k);
		if ( pol == NULL || !pol->fValid())
			continue;
		if ( pol->getNumInteriorRing() != 0) {
			polswithholes.insert(pair<double, ILWIS::Polygon *>(pol->rArea(),pol));
		}
		else
			polswithoutholes.push_back(pol);
		
	}
	for(multimap<double, ILWIS::Polygon *>::reverse_iterator cur=polswithholes.rbegin(); cur!= polswithholes.rend(); ++cur) {
		polygons.push_back((*cur).second);
	}
	for(int i = 0; i < polswithoutholes.size(); ++i) {
		polygons.push_back(polswithoutholes[i]);
	}
	if (fDrawPol) {
		for(int i = 0; i < polygons.size(); ++i) {
			drawSinglePolygon(cdc,rect,psn,fDrawStop,polygons[i],cb,cbIntern,c);

		}
	}    
	//drawSeg:
	if ((fBoundaries && iBoundaryWidth) || fBoundariesOnly) {
		Color c;
		if ((long)clrBoundary == -1)
			c = SysColor(COLOR_WINDOWTEXT);
		else
			c = clrDraw(clrBoundary);
		pen.CreatePen(PS_SOLID, iBoundaryWidth, c);
		penOld = cdc->SelectObject(&pen);
		for(int i = 0; i < pm->iFeatures(); ++i) {
			ILWIS::Polygon *pol = (ILWIS::Polygon *)pm->getFeature(i);
			if ( pol == NULL || !pol->fValid())
				continue;
			++iTranquilizer;
			if (*fDrawStop)
				break;
			vector<Array<zPoint>> p;
			p.resize(1 + pol->getNumInteriorRing());
			psn->iPolPos(pol,p, pm->cs());
			if (p.size() > 0 && p[0].size() > 2){
				for(int j = 0; j < p.size(); ++j)
					cdc->Polyline(p[j].buf(),p[j].size());
			}
		}
		cdc->SelectObject(penOld);
		pen.DeleteObject();
	}
	pm->KeepOpen(false);
	return 0;
}

bool PolygonMapDrawer::drawSinglePolygon(CDC* cdc, zRect, Positioner* psn, volatile bool* fDrawStop, ILWIS::Polygon *pol, const CoordBounds& cb, const CoordBounds& cbIntern, Color c) {
	CPen pen, *penOld = 0;
	CBrush brush, *brushOld, brushBG;

	if (drm == drmSINGLE) {
		if (!fBoundariesOnly) {

			brush.CreateSolidBrush(c);
			brushBG.CreateSolidBrush(SysColor(COLOR_WINDOW));
			brushOld = cdc->SelectObject(&brush);
			if ((long)clr == -1)
				c = SysColor(COLOR_WINDOWTEXT); 
			else
				c = clr;
		}    
	}
	pen.CreatePen(PS_NULL,0,Color(0));
	penOld = cdc->SelectObject(&pen);
	if ( pol == NULL || !pol->fValid())
		return true;
	++iTranquilizer;
	if (*fDrawStop)
		return false;
	//      if (!fSameCsy || mmIntern.fContains(pol.mmBounds())) {
	vector<Array<zPoint>> p;
	p.resize(1 + pol->getNumInteriorRing());
	if ( cb.fUndef() || cbIntern.fContains(pol->cbBounds())) {
		if (fMask) {
			if (!fPolInMask(pol))
				return true;
		}
		if (drm == drmSINGLE) {
			psn->iPolPos(pol,p,pm->cs());
			//cdc->Polygon(p[0].buf(),p[0].size());
			CBrush *brOld = NULL;
			if (p.size() > 0 && p[0].size() > 2){
				for(int j = 0; j < p.size(); ++j) {
					if ( j > 0) {
						brOld = cdc->SelectObject(&brushBG);
					}
					cdc->Polygon(p[j].buf(),p[j].size());
				}
				if ( brOld != NULL)
					cdc->SelectObject(brOld);
			}
		}
		else {
			Pattern* pat = 0;
			Color c;
			long iRaw;
			double rVal;
			if (pm->dm()->pdvi() || pm->dm()->pdvr()) {
				rVal = pol->rValue();
				if (rVal == rUNDEF) return true;
			}
			else { 
				iRaw = pol->iValue();
				if (iRaw == iUNDEF) return true;
				if (fAttTable) {
					if (colAtt->dm()->pdvi() || colAtt->dm()->pdvr()) 
						rVal = colAtt->rValue(iRaw);
					else  
						iRaw = colAtt->iRaw(iRaw);
				}
			}  
			switch (drm) {
					case drmRPR:
						if (_dm->pdvi() || _dm->pdvr()) {
							if (fStretch)
								c = _rpr->clr(rVal,rrStretch);
							else  
								c = _rpr->clr(rVal);
							if ((long)c == -2)
								return true;
						}
						else {
							pat = new Pattern(_rpr, iRaw, drc);
						}
						break;
					case drmBOOL:
						if (0 == iRaw)
							return true;
						// fall through
					case drmCOLOR:
						// fall through
					case drmMULTIPLE: 
						c = clrRaw(iRaw);
						break;
			}
			if (0 == pat && (long)c == iUNDEF)
				return true;
			if (0 == pat)
				pat = new Pattern(c);
			psn->iPolPos(pol,p, pm->cs());
			pat->drawPolygon(cdc, p);

			delete pat;
		}
	}
	cdc->SelectObject(penOld);
	pen.DeleteObject();
	if (drm == drmSINGLE) {
		cdc->SelectObject(brushOld);
		brush.DeleteObject();
		brushBG.DeleteObject();
	}

	return true;
}

class PolygonMapDrawerForm: public BaseMapDrawerForm
{
public:
  PolygonMapDrawerForm(PolygonMapDrawer* mdr, bool fShowForm=true);
  void DomUseChanged();
  int exec();
private:
  int iValueOption;
  int iClassOption;
  int iIdentOption;
  int BndOnlyCallBack(Event*);
  CheckBox* cbBndOnly;
  PolygonMapDrawer* mdw;
  FieldColor* fldClrBnd;
  FieldInt* fldBndWidth;
  String sMask;
};

PolygonMapDrawerForm::PolygonMapDrawerForm(PolygonMapDrawer* mdr, bool fShowForm)
: BaseMapDrawerForm(mdr, SDCTitlePolMap, fShowForm), mdw(mdr)
{
	iImg = IlwWinApp()->iImage(".mpa");

  CheckBox* cbMask = new CheckBox(root, SDCUiMask, &mdw->fMask);
  sMask = mdw->mask.sMask();
  FieldString* fs = new FieldString(cbMask, "", &sMask);
	fs->SetWidth(80);
  FieldBlank* fb = new FieldBlank(root, 0);
  fb->Align(cbMask, AL_UNDER);
  cbBndOnly = new CheckBox(root, SDCUiBoundariesOnly, &mdw->fBoundariesOnly);
  cbBndOnly->SetCallBack((NotifyProc)&PolygonMapDrawerForm::BndOnlyCallBack);
  cbBndOnly->SetIndependentPos();
  CheckBox* cb = new CheckBox(root, SDCUiHideSuperfluousBoundaries, &mdw->fHideSuperfluousBoundaries);
  cb->SetIndependentPos();
//  FieldGroup* fgBndOnly = new FieldGroup(cbBndOnly);
//  fgBndOnly->Align(cbBndOnly, AL_UNDER);
  fldClrBnd = new FieldColor(root, SDCUiBoundaryColor, &mdw->clrBoundary);
  fldBndWidth = new FieldInt(root, SDCUiBoundaryWidth, &mdw->iBoundaryWidth);

  bool fTbl = bm->fTblAtt();
//  if (fTbl) {
    InitAtt(true);
//    if (cbAtt) cbAtt->Align(cbBndOnly, AL_UNDER);
//  }
  fb = new FieldBlank(root, 0);
  if (fTbl ||  bm->dm()->pdvi() || bm->dm()->pdvr()) {
    iValueOption = (int) mdw->drm;
    if (drmMULTIPLE == mdw->drm)
      iValueOption -= 1;
    fgValue = new FieldGroup(root);
    fgValue->Align(fb, AL_UNDER);
    RadioGroup* rgValue = new RadioGroup(fgValue, "", &iValueOption);
    InitRprValue(rgValue);
    rbSingle = new RadioButton(rgValue, SDCUiSingleColor);
    FieldColor* fc = new FieldColor(rbSingle, "", &mdw->clr);
    fc->Align(rbSingle, AL_AFTER);
//    InitMultiple(rgValue);
    InitStretch(rbRpr);
    RadioGroup* rg = new RadioGroup(fgValue, "", (int*)&mdw->drc, true);
    rg->Align(stStretch, AL_UNDER);
    InitColoring(rg);
  }
  if (fTbl || bm->dm()->pdc()) {  
    iClassOption = (int) mdw->drm;
    if (iClassOption > 1)
      iClassOption = 1;
    fgClass = new FieldGroup(root);
    fgClass->Align(fb, AL_UNDER);
    RadioGroup* rgClass = new RadioGroup(fgClass, "", &iClassOption);
//    if (cbAtt)
//    else  
//      rgClass->Align(cbBndOnly, AL_UNDER);
//    new RadioButton(rgClass, SDUiRpr);
    InitRprClass(rgClass);
    RadioButton* rb = new RadioButton(rgClass, SDCUiSingleColor);
    FieldColor* fc = new FieldColor(rb, "", &mdw->clr);
    RadioGroup* rg = new RadioGroup(fgClass, "", (int*)&mdw->drc, true);
    rg->Align(rb, AL_UNDER);
    InitColoring(rg);
  }  
  if (fTbl || bm->dm()->pdid()) {  
    iIdentOption = (int) mdw->drm - 1;
		if (iIdentOption != 0)
			iIdentOption = 1;
    FieldGroup* fgIdent = new FieldGroup(root);
//    if (cbAtt)
      fgIdent->Align(fb, AL_UNDER);
//    else  
//      fgIdent->Align(cbBndOnly, AL_UNDER);
    RadioGroup* rg = new RadioGroup(fgIdent, "", &iIdentOption);
    rbSingle = new RadioButton(rg, SDCUiSingleColor);
    FieldColor* fc = new FieldColor(rbSingle, "", &mdw->clr);
    fc->Align(rbSingle, AL_AFTER);
//    InitSingle(rg);
    InitMultiple(rg);
    RadioGroup* rgCol = new RadioGroup(fgIdent, "", (int*)&mdw->drc, true);
    rgCol->Align(rg, AL_UNDER);
    InitColoring(rgCol);
    fgID = fgIdent;
  }  
  if (fTbl || bm->dm()->pdbool()) {
    InitBool();
    if (fTbl)
      fgBool->Align(fb, AL_UNDER);
    RadioGroup* rg = new RadioGroup(fgBool, "", (int*)&mdw->drc, true);
    InitColoring(rg);
  }
	if (fTbl) {
    fgColor = new FieldGroup(root);
    fgColor->Align(fb, AL_UNDER);
    RadioGroup* rg = new RadioGroup(fgColor, "", (int*)&mdw->drc, true);
    InitColoring(rg);
	}
  SetMenHelpTopic(htpCnfPolygonMapDrawer);
  create();
}

int PolygonMapDrawerForm::BndOnlyCallBack(Event*)
{
  if (cbBndOnly->fVal()) {
    if (cbAtt) cbAtt->Hide();
    if (fgValue) fgValue->Hide();
    if (fgClass) fgClass->Hide();
    if (fgID) fgID->Hide();
    if (fgBool) fgBool->Hide();
	  if (fgColor) fgColor->Hide();
  }
  else {
    if (cbAtt) cbAtt->Show();
    DomUseChanged();
  }
  return 0;
}

void PolygonMapDrawerForm::DomUseChanged()
{
  if (fgValue) fgValue->Hide();
  if (fgClass) fgClass->Hide();
  if (fgID) fgID->Hide();
  if (fgBool) fgBool->Hide();
  if (fgColor) fgColor->Hide();
  if (cbBndOnly->fVal()) 
    return;
  switch (dmtUse()) {
	case dmtCLASS: 
    { 
      Representation rpr;
      if (mdw->_dm == dmUse)
        rpr = mdw->_rpr;
      if (!rpr.fValid())
        rpr = dmUse->rpr();
      if (rpr.fValid()) {
        fldBndWidth->SetVal(rpr->iWidthBoundary());
        fldClrBnd->SetVal((Color)rpr->clrBoundary());
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
          rpr = mdw->rpr();
        if (!rpr.fValid())
          rpr = dmUse->rpr();
        if (rpr.fValid()) {
          sRprName = rpr->sName(true);
          fldBndWidth->SetVal(rpr->iWidthBoundary());
          fldClrBnd->SetVal((Color)rpr->clrBoundary());
        }  
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

int PolygonMapDrawerForm::exec()
{
  BaseMapDrawerForm::exec();
  if (mdw->fMask)
    mdw->mask.SetMask(sMask);
  if (!mdw->fBoundariesOnly) {
    switch (dmtUse()) {
      case dmtCLASS:
        mdw->drm = (DrawMethod) iClassOption;
        mdw->_rpr = Representation(FileName(sRprName));
        dmUse->SetRepresentation(mdw->_rpr);
//        if (mdw->_rpr->pra())
//          mdw->_rpr = Representation(mdw->_rpr, mdw->pm->tblAtt());
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
      else {  
        mdw->_rpr->SetClrBoundary(mdw->clrBoundary);
        mdw->_rpr->SetWidthBoundary(mdw->iBoundaryWidth);
      }  
    }  
    else {
      mdw->_rpr = Representation();
    }  
  }  
  return 0;
}
  
int PolygonMapDrawer::Configure(bool fShowForm)
{
  bool fShow = fAct;
  bool fOk;
  if (fNew) {
    fAct = false;  // to prevent "in between" drawing
    fNew = false;
  }
  {
		iBoundaryWidth /= RepresentationClass::iSIZE_FACTOR;
    PolygonMapDrawerForm frm(this, fShowForm);
		iBoundaryWidth *= RepresentationClass::iSIZE_FACTOR;
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

int PolygonMapDrawer::Setup()
{
  return 0;
}

String PolygonMapDrawer::sInfo(const Coord& crd)
{
	vector<Geometry*> pols = pm->getFeatures(crd);
  String s;
  for(int i = 0; i < pols.size(); ++i) {
	  if ( i > 0)
		  s &= '/';
	  ILWIS::Polygon *pol = (ILWIS::Polygon *)pols.at(i);
	  if ( pol == NULL || pol->fValid() == false)
		  continue;
	  if (!pol->fValid()) return "";
	  if (!fPolInMask(pol)) return "";
	  String part = pol->sValue(pm->dvrs());
	  part = part.sTrimSpaces();
	  s += part;
	  if (fAttTable) {
		long iRaw = pol->iValue();
		if (iRaw != iUNDEF) {
		  String sAtt = colAtt->sValue(iRaw);
		  for (int iLen = sAtt.length(); iLen && sAtt[iLen-1] == ' '; --iLen)
			sAtt[iLen-1] = 0;
		  s &= ": ";
		  s &= sAtt;
		}  
	  }
  }
  return s;
}

/*
void PolygonMapDrawer::Edit()
{
  mppn->PolygonEditStr(pm->sName());
}

Record PolygonMapDrawer::rec(const Coord& crd)
{
  return recFromMap(pm, crd);
}
*/

FormBaseDialog* PolygonMapDrawer::wEditRpr(CWnd* wPar, int iRaw)
{
  if (iRaw > 0 && _rpr.fValid()) {
    RepresentationClass* rpc = _rpr->prc();
    if (rpc) 
      return new PolygonMapRprEditForm(wPar, rpc, iRaw, true);
  }
  return 0;
}

FormBaseDialog* PolygonMapDrawer::wEditRpr(CWnd* wPar, const Coord& crd)
{
  long iRaw = pm->iRaw(crd);
	return wEditRpr(wPar, iRaw);
}

IlwisObject PolygonMapDrawer::obj() const
{
  return pm;
}

zIcon PolygonMapDrawer::icon() const
{
	return zIcon("PolIcon");
}

void PolygonMapDrawer::DrawLegendRect(CDC* cdc, CRect rect, int iRaw) const
{
	Pattern* pat;
	if (drmRPR == drm) 
	  pat = new Pattern(_rpr, iRaw, drc);
	else {
		Color c = clrRaw(iRaw);
    pat = new Pattern(c);
	}
	pat->drawRectangle(cdc->GetSafeHdc(), &rect);
	delete pat;
}

void PolygonMapDrawer::DrawValueLegendRect(CDC* cdc, CRect rect, double rVal) const
{
	if (!_rpr.fValid())
		return;
	Pattern* pat;
	Color c;
  if (fStretch)
    c = _rpr->clr(rVal,rrStretch);
  else  
    c = _rpr->clr(rVal);
  if ((long)c == -2)
		return;
  pat = new Pattern(c);
	pat->drawRectangle(cdc->GetSafeHdc(), &rect);
	delete pat;
}

bool PolygonMapDrawer::fLegendUseful() const
{
	if (!BaseMapDrawer::fLegendUseful())
		return false;
	return !fBoundariesOnly;
}

