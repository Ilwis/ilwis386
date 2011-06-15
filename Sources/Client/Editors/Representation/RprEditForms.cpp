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
#include "Engine\Map\Segment\Seg.h"
#include "engine\map\polygon\POL.H"
#include "Engine\Base\System\RegistrySettings.h"
#include "Client\FormElements\fldcol.h"
#include "Client\FormElements\fldrpr.h"
#include "Client\Mapwindow\Positioner.h"
#include "Client\Mapwindow\MapPaneView.h"
#include "Engine\Domain\dmsort.h"
#include "Engine\Domain\dmpict.h"
#include "Client\Editors\Utils\SYMBOL.H"
#include "Client\FormElements\fldcolor.h"
#include "Client\Editors\Utils\Smbext.h"
#include "Client\FormElements\fldcolor.h"
#include "Engine\Domain\dmcolor.h"
#include "Engine\Domain\Dmvalue.h"
#include "Engine\Representation\RPRVALUE.H"
#include "Engine\Representation\Rprgrad.h"
#include "Engine\Representation\Rprclass.h"
#include "Headers\Hs\Represen.hs"
#include "Client\Editors\Utils\line.h"
#include "Client\Editors\Utils\Pattern.h"
#include "Client\Editors\Representation\RprEditForms.h"

MapRprEditForm::MapRprEditForm(CWnd* wPar, 
  RepresentationClass* rc, long raw, bool fModal)
: FormBaseDialog(wPar, SRPTitleEditRprItem, true, fModal, false),
  rcl(rc), iRaw(raw)
{
  String sText;
  if (rcl->dm()->pdp())
    sText = String("%i", raw);
  else  
    sText = rcl->dm()->sValueByRaw(raw,0);
  col = rcl->clrRaw(iRaw);
  StaticText* st = new StaticText(root, sText);
  st->SetIndependentPos();
  cs = new ColorSelector(root, &col);
  PushButton* pb = new PushButton(root, SRPUiCustomColor, 
    (NotifyProc)&MapRprEditForm::CustomColor);
  pb->Align(cs, AL_UNDER);  
  SetMenHelpTopic("ilwismen\\representation_class_editor_edit_item_raster.htm");
  create();
}

int MapRprEditForm::CustomColor(Event *)
{
  cs->CustomColor(0);
  return 0;
}

int MapRprEditForm::exec() {
  root->StoreData();
  Color clr = col;
  rcl->PutColor(iRaw,clr);
  return 0;
}

SegmentMapRprEditForm::SegmentMapRprEditForm(CWnd* wPar, 
                          RepresentationClass* rc, long raw, bool fModal)
: FormBaseDialog(wPar, SRPTitleEditRprItem, true, fModal, false),
  rcl(rc), iRaw(raw), line(rc, raw)
{
  String sText;
  if (rcl->dm()->pdp())
    sText = String("%i", raw);
  else  
    sText = rcl->dm()->sValueByRaw(raw,0);
  col = rcl->clrRaw(iRaw);

  StaticText* st = new StaticText(root, sText);
  st->SetIndependentPos();
  cs = new ColorSelector(root, &col);
  cs->SetIndependentPos();
  PushButton* pb = new PushButton(root, SRPUiCustomColor, 
    (NotifyProc)&SegmentMapRprEditForm::CustomColor);
  pb->Align(cs, AL_UNDER);  
  pb->SetIndependentPos();

  new FieldBlank(root, 0.3);
	line.ResizeSymbol(1.0/RepresentationClass::iSIZE_FACTOR);
  new FieldLine(root, &line);

  SetMenHelpTopic("ilwismen\\representation_class_editor_edit_item_segment.htm");
  create();
}

int SegmentMapRprEditForm::CustomColor(Event*)
{
  cs->CustomColor(0);
  return 0;
}

int SegmentMapRprEditForm::exec() {
  root->StoreData();
	line.ResizeSymbol(RepresentationClass::iSIZE_FACTOR);
  line.clrLine() = col;
  line.Store(rcl, iRaw);
  return 0;
}

class _export FieldPolHatchDsp: public FieldGroup
{
public:
  FieldPolHatchDsp(FormEntry* parent, const String& sQuestion, long*);
};

class _export FieldPolPattern: public FieldGroup
{
public:
  FieldPolPattern(FormEntry* parent, const String& sQuestion, long*, short pPat[8]);
};

PolygonMapRprEditForm::PolygonMapRprEditForm(CWnd* wPar, 
                        RepresentationClass* rc, long raw, bool fModal)
: FormBaseDialog(wPar, SRPTitleEditRprItem, true, fModal, false),
  rcl(rc), iRaw(raw)
{
  String sText;
	pPattern = 0;
  if (rcl->dm()->pdp())
    sText = String("%i", raw);
  else  
    sText = rcl->dm()->sValueByRaw(raw,0);
  col = rcl->clrRaw(iRaw);
  col2 = rcl->clrSecondRaw(iRaw);
  colHBG = col2;
  int iPattern = rcl->iPattern(iRaw);
  short pPat[8];
  fInverse = false;
  pHatch = 0;
  if (0 == iPattern)
    iType = 0;
  else if (9999 == iPattern) {
    iType = 2;
    rcl->GetPattern(iRaw,pPat);
  }
  else {
    iType = 1;
    switch (iPattern & 0x30) {
      case 0x00:
        fInverse = false;
        colHBG = -2;
        break;
      case 0x10:
        fInverse = false;
        break;
      case 0x20:
        fInverse = true;
        break;
    }
    iPattern &= 0x000F;
    pHatch = iPattern - 1;
  }

  StaticText* st = new StaticText(root, sText);
  st->SetIndependentPos();
  cs = new ColorSelector(root, &col, true);
  cs->SetIndependentPos();
  PushButton* pb = new PushButton(root, SRPUiCustomColor, 
    (NotifyProc)&PolygonMapRprEditForm::CustomColor);
  pb->SetIndependentPos();
  pb->Align(cs, AL_UNDER);
  new FieldBlank(root, 0.3);

  rgType = new RadioGroup(root, "", &iType);
  new RadioButton(rgType, SRPUiSolid);
  RadioButton* rbHatch = new RadioButton(rgType, SRPUiHatching);
  RadioButton* rbPatt = new RadioButton(rgType, SRPUiPattern);

  FieldPolHatchDsp* fphd = new FieldPolHatchDsp(rbHatch, "", &pHatch);
  fphd->Align(rbHatch, AL_AFTER);

  FieldPolPattern* fpp = new FieldPolPattern(rbPatt, "", &pPattern, pPat);
  fpp->Align(rbPatt, AL_AFTER);
  rgType->SetCallBack((NotifyProc)&PolygonMapRprEditForm::OptionChanged);

  feHatching = new FieldColor(root, SRPUiHatchColor, &col2);
  feHatching->Align(rgType, AL_UNDER);
  feHatchBG  = new FieldFillColor(root, SRPUiBackColor, &colHBG);
  feHatchBG->Align(rgType, AL_UNDER);
  fePatFG = new FieldColor(root, SRPUiForeColor, &col2);
  fePatFG->Align(rgType, AL_UNDER);
	if ( col2 == Color(-2))
		col2 = Color(255,255,255);
  fePatBG = new FieldColor(root, SRPUiBackColor, &col2);
  fePatBG->Align(rgType, AL_UNDER);

  pbInverse = new PushButton(root, SRPUiInverseColors,
    (NotifyProc)&PolygonMapRprEditForm::InverseColors);
  SetMenHelpTopic("ilwismen\\representation_class_editor_edit_item_polygon.htm");
  create();
}

int PolygonMapRprEditForm::CustomColor(Event*)
{
  cs->CustomColor(0);
  return 0;
}

int PolygonMapRprEditForm::OptionChanged(Event*)
{
  rgType->StoreData();
  fePatFG->Hide();
  fePatBG->Hide();
  feHatching->Hide();
  feHatchBG->Hide();
  switch (iType) {
    case 1: // hatching
      if (fInverse)
        feHatching->Show();
      else
        feHatchBG->Show();
      pbInverse->Show();
      break;
    case 2: // pattern
      if (fInverse)
        fePatFG->Show();
      else
        fePatBG->Show();
      pbInverse->Show();
      break;
    default:
      pbInverse->Hide();
  }
  return 0;
}

int PolygonMapRprEditForm::InverseColors(Event*)
{
  cs->StoreData();
  Color clr;
  switch (iType) {
    case 1:
      if (fInverse) {
        feHatching->StoreData();
        clr = col2;
        feHatchBG->SetVal(col);
      }
      else {
        feHatchBG->StoreData();
        clr = colHBG;
        feHatching->SetVal(col);
      }
      break;
    case 2:
      if (fInverse) {
        fePatFG->StoreData();
        clr = col2;
        fePatBG->SetVal(col);
      }
      else {
        fePatBG->StoreData();
        clr = col2;
        fePatFG->SetVal(col);
      }
      break;
    default:
      return 0;
  }
  cs->SetVal(clr);
  fInverse = !fInverse;
  return OptionChanged(0);
}

int PolygonMapRprEditForm::exec() {
  root->StoreData();
  Color clr = col;
  rcl->PutColor(iRaw,clr);
  switch (iType) {
    case 0: // solid
      rcl->PutPattern(iRaw, 0);
      break;
    case 1: { // hatch
      int iHatch = pHatch;
      if (fInverse) {
        iHatch |= 0x20;
        Color clr2 = col2;
        rcl->PutSecondColor(iRaw,clr2);
      }
      else if ((long)colHBG != -2) {
        iHatch |= 0x10;
        Color clr2 = colHBG;
        rcl->PutSecondColor(iRaw,clr2);
      }
      rcl->PutPattern(iRaw, iHatch);
    } break;
    case 2: // pattern
      if ((short*)-1 != (short*)pPattern)
        rcl->PutPatternData(iRaw,(short*)pPattern);
      Color clr2 = col2;
      rcl->PutSecondColor(iRaw,clr2);
      break;
  }
  return 0;
}

PointMapRprEditForm::PointMapRprEditForm(CWnd* wPar, 
                        RepresentationClass* rc, long raw, bool fModal)
: FormBaseDialog(wPar, SRPTitleEditRprItem, true, fModal, false),
  rcl(rc), iRaw(raw), smb(rc,raw)
{
  String sText;
  if (rcl->dm()->pdp())
    sText = String("%i", raw);
  else  
    sText = rcl->dm()->sValueByRaw(raw,0);
  StaticText* st = new StaticText(root, sText);
  st->SetIndependentPos();
	smb.Resize(1.0/RepresentationClass::iSIZE_FACTOR);
  fs = new FieldExtendedSymbol(root, smb);
  SetMenHelpTopic("ilwismen\\representation_class_editor_edit_item_point.htm");
  create();
}

int PointMapRprEditForm::exec() {
  root->StoreData();
	smb.Resize(RepresentationClass::iSIZE_FACTOR);
  if (smb.smType == ExtendedSymbol::smSIMPLE) {
    rcl->PutSymbolType(iRaw,(short)smb.smb);
  }
  else {
    rcl->PutSymbolType(iRaw,-(short)smb.cSmb);
    rcl->PutSymbolFont(iRaw,smb.sFaceName);
  }
  rcl->PutSymbolSize(iRaw,smb.iSize);
  rcl->PutSymbolWidth(iRaw,smb.iWidth);
  Color clr = smb.col;
  rcl->PutSymbolColor(iRaw,clr);
  clr = smb.fillCol;
  rcl->PutSymbolFill(iRaw,clr);
  rcl->PutSymbolRotation(iRaw,smb.rRotation);
  return 0;
}

class FieldPolHatchDspSimple: public FieldOneSelect
{
public:
  FieldPolHatchDspSimple(FormEntry* parent, long* polDsp)
  : FieldOneSelect(parent,polDsp)
  {
    psn->iMinWidth = (short)(1.5 * FLDNAMEWIDTH);
  }
  ~FieldPolHatchDspSimple();
private:
  void create();
  void StoreData();     
  bool DrawObject(long value, DRAWITEMSTRUCT* dis);
  bool fDrawIcon();
};

FieldPolHatchDspSimple::~FieldPolHatchDspSimple()
{
}


FieldPolHatchDsp::FieldPolHatchDsp(FormEntry* p, const String& sQuestion,
	     long* polDsp)
  : FieldGroup(p)
{
  FormEntry* st = 0;
  if (sQuestion[0])
    st = new StaticTextSimple(this, sQuestion);
  FormEntry* pd = new FieldPolHatchDspSimple(this, polDsp);
  if (st)
    pd->Align(st, AL_AFTER);
}

void FieldPolHatchDspSimple::StoreData()
{
  FieldOneSelect::StoreData();
}

bool FieldPolHatchDspSimple::fDrawIcon()
{
    return false;
}

void FieldPolHatchDspSimple::create()
{
  FieldOneSelect::create();
  int i = 0;
  int iSel = 0;
  ose->AddString(SRPPatUpwards.scVal());
	ose->SetItemData(i++, (long)pDownHatch);
  ose->AddString(SRPPatDownwards.scVal());
	ose->SetItemData(i++, (long)pUpHatch);
  ose->AddString(SRPPatCrossing.scVal());
	ose->SetItemData(i++, (long)pCrossHatch);
  ose->AddString(SRPPatDiagonal.scVal());
  ose->SetItemData(i++, (long)pDiagCrossHatch);
  ose->AddString(SRPPatHorizontal.scVal());
	ose->SetItemData(i++, (long)pHorzHatch);
  ose->AddString(SRPPatVertical.scVal());
	ose->SetItemData(i++, (long)pVertHatch);

  if (*val <= HS_VERTICAL)
    iSel = *val;
  ose->SetCurSel(iSel);
  ose->SetFont(frm()->fnt);  // Yes on !
}

bool FieldPolHatchDspSimple::DrawObject(long value, DRAWITEMSTRUCT* dis)
{
  CPen pen(PS_NULL,1, RGB(0,0,0));
  Pattern *pat=NULL;
  Color color = ::GetSysColor(COLOR_WINDOWTEXT);
  if (dis->itemState & ODS_SELECTED)
    color = ::GetSysColor(COLOR_HIGHLIGHTTEXT);
  pat = new Pattern((PolDspType)value, color);
  HPEN oldPen=(HPEN)SelectObject(dis->hDC, pen);
  CRect rect(dis->rcItem.left+2, dis->rcItem.top+2,
             dis->rcItem.left+15, dis->rcItem.top+15);
  pat->drawRectangle(dis->hDC, &rect);
  SelectObject(dis->hDC, oldPen);
  delete pat;
  return true;
}

class FieldPolPatternSimple: public FieldOneSelect
{
public:
  FieldPolPatternSimple(FormEntry* parent, long* polDsp, short pat[8])
  : FieldOneSelect(parent,polDsp,true), ppi(0)
  {
    memcpy(pPat, pat, 16);
    psn->iMinWidth = (short)(1.5 * FLDNAMEWIDTH);
  }
  ~FieldPolPatternSimple();
private:
  void create();
  void StoreData();     
  bool DrawObject(long value, DRAWITEMSTRUCT* dis);
  bool fDrawIcon();
  short** ppi;
  int iPpiSize;
  short pPat[8];
};

FieldPolPatternSimple::~FieldPolPatternSimple()
{
  if (ppi) {
    for (int i = 0; i < iPpiSize; ++i)
      if (ppi[i]) delete [] ppi[i];
    delete [] ppi; 
  }  
}   


FieldPolPattern::FieldPolPattern(FormEntry* p, const String& sQuestion,
	     long* polDsp, short pPat[8])
  : FieldGroup(p)
{
  FormEntry* st = 0;
  if (sQuestion[0])
    st = new StaticTextSimple(this, sQuestion);
  FormEntry* pd = new FieldPolPatternSimple(this, polDsp, pPat);
  if (st)
    pd->Align(st, AL_AFTER);
}

void FieldPolPatternSimple::StoreData()
{
  FieldOneSelect::StoreData();
  short* pi = (short*) *val;
  if (0 == pi || (short*)-1 == pi)
    return;
  memcpy(pPat, pi, 16);
  *val = (long)pPat;
}

bool FieldPolPatternSimple::fDrawIcon()
{
    return false;
}

void FieldPolPatternSimple::create()
{
  FieldOneSelect::create();
  int i = 0;
  int iSel = 0;

  Array<String> as;
  RepresentationPtr::GetPatternNames(as);
  int iSize = as.iSize();
  iPpiSize = iSize;
  typedef short* pShort;
  ppi = new pShort[iPpiSize];
  for (int j = 0; j < iSize; ++j) {
		const char *s = as[j].scVal();
    int id = ose->AddString(s);
    ppi[j] = new short[8];
    short* pi = RepresentationPtr::pPattern(as[j]);
		
    memcpy(ppi[j], pi, 16);
    if (0 == memcmp(pPat,ppi[j],16)) 
		{
      iSel = id;
    }
    ose->SetItemData(id,(long)ppi[j]);
  }
  ose->SetCurSel(iSel);	
  ose->SetFont(frm()->fnt);  // Yes on !
}

bool FieldPolPatternSimple::DrawObject(long value, DRAWITEMSTRUCT* dis)
{
  CPen pen(PS_NULL, 1, RGB(0,0,0));
  Pattern *pat=NULL;
  Color col1 = ::GetSysColor(COLOR_WINDOW);
  Color col2 = ::GetSysColor(COLOR_WINDOWTEXT);
  short* pi = (short*) value;
  pat = new Pattern(pi, col1, col2);
  HPEN oldPen=(HPEN)SelectObject(dis->hDC, pen);
  CRect rect(dis->rcItem.left+2, dis->rcItem.top+2,
             dis->rcItem.left+15, dis->rcItem.top+15);
  pat->drawRectangle(dis->hDC, &rect);
  SelectObject(dis->hDC, oldPen);
  delete pat;
  return true;
}

