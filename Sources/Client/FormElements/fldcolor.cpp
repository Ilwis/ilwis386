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
/* FieldColor
   by Wim Koolhoven, june 1994
   (c) Ilwis System Development ITC
	Last change:  WK   21 Oct 98    1:07 pm
*/
#include "Client\Headers\formelementspch.h"
#include "Client\FormElements\fldcolor.h"
#include "Engine\Base\System\mutex.h"
#include "Headers\Hs\COLORS.hs"
#include "afxdlgs.h"

FieldColorSimple::FieldColorSimple(FormEntry* p, Color* color)
  : FieldOneSelect(p,(long*)color)
{
  psn->iMinWidth = 1.5 * FLDNAMEWIDTH;
  SetHelpTopic(htpUiColor);
}

FieldColorSimple::~FieldColorSimple()
{
//  ose->removeNotify(this, Notify(EN_CHANGE));
}

const int iCols = 32;
static bool fColorInitialized = false;
static struct
{
  int r,g,b;
  String* sName;
} ColorStruct[]        = {   0,  0,  0, NULL,
                     84, 84, 84, NULL,
                    128,128,128, NULL,
                    168,168,168, NULL,
                    255,255,255, NULL,
                    255,  0,  0, NULL,
                      0,255,  0, NULL,
                      0,  0,255, NULL,
                      0,255,255, NULL,
                    255,  0,255, NULL,
                    255,255,  0, NULL,
                    255,136,  0, NULL,
                    255, 68,  0, NULL,
                    216,168,  0, NULL,
                    164, 40, 40, NULL,
                    244,164, 96, NULL,
                    140, 68, 20, NULL,
                    255, 20,148, NULL,
                    160, 32,240, NULL,
                    124,252,  0, NULL,
                     48,216, 56, NULL,
                     48,128, 76, NULL,
                    116,222,120, NULL,
                    108,142, 36, NULL,
                      0,176, 20, NULL,
                      0, 86, 44, NULL,
                     84,148,132, NULL,
                     24,204,224, NULL,
                     30,144,255, NULL,
                     64,104,225, NULL,
                    116,160,255, NULL,
                     48, 48,100, NULL,
                      0,  0,  0, NULL };

Color clrStd(int iNr)
{
  iNr %= iCols;
  Color clr;
  clr.red() = ColorStruct[iNr].r;
  clr.green() = ColorStruct[iNr].g;
  clr.blue() = ColorStruct[iNr].b;
  return clr;
};
                     
static CCriticalSection csColorInit;
static void ColorInitialize()
{
  csColorInit.Lock();
//  MutexFileName mut(FileName(), "InitializeColor");
  if (!fColorInitialized) {
    fColorInitialized = true;
    int i = 0;
    ColorStruct[i++].sName = new String(SCLColBlack);
    ColorStruct[i++].sName = new String(SCLColDimGrey);
    ColorStruct[i++].sName = new String(SCLColGrey);
    ColorStruct[i++].sName = new String(SCLColLightGrey);
    ColorStruct[i++].sName = new String(SCLColWhite);
    ColorStruct[i++].sName = new String(SCLColRed);
    ColorStruct[i++].sName = new String(SCLColGreen);
    ColorStruct[i++].sName = new String(SCLColBlue);
    ColorStruct[i++].sName = new String(SCLColCyan);
    ColorStruct[i++].sName = new String(SCLColMagenta);
    ColorStruct[i++].sName = new String(SCLColYellow);
    ColorStruct[i++].sName = new String(SCLColOrange);
    ColorStruct[i++].sName = new String(SCLColOrangeRed);
    ColorStruct[i++].sName = new String(SCLColGold);
    ColorStruct[i++].sName = new String(SCLColBrown);
    ColorStruct[i++].sName = new String(SCLColSandyBrown);
    ColorStruct[i++].sName = new String(SCLColSaddleBrown);
    ColorStruct[i++].sName = new String(SCLColPink);
    ColorStruct[i++].sName = new String(SCLColPurple);
    ColorStruct[i++].sName = new String(SCLColLawnGreen);
    ColorStruct[i++].sName = new String(SCLColYellowGreen);
    ColorStruct[i++].sName = new String(SCLColForestGreen);
    ColorStruct[i++].sName = new String(SCLColPaleGreen);
    ColorStruct[i++].sName = new String(SCLColOliveGreen);
    ColorStruct[i++].sName = new String(SCLColLimeGreen);
    ColorStruct[i++].sName = new String(SCLColDarkGreen);
    ColorStruct[i++].sName = new String(SCLColSeaGreen);
    ColorStruct[i++].sName = new String(SCLColTurquoise);
    ColorStruct[i++].sName = new String(SCLColDodgerBlue);
    ColorStruct[i++].sName = new String(SCLColRoyalBlue);
    ColorStruct[i++].sName = new String(SCLColSkyBlue);
    ColorStruct[i++].sName = new String(SCLColMidnightBlue);
  }
  csColorInit.Unlock();
}                     

void WEP_Color()
{
 // csColorInit.Lock();
  for (int i = 0; i < iCols; ++i)
    delete ColorStruct[i].sName;
  fColorInitialized = false;
 // csColorInit.Unlock();
}

void FieldColorSimple::create()
{
  ColorInitialize();
  FieldOneSelect::create();
  int iCol = -1;
  Color c = *(Color*)val;
  Color cText = GetSysColor(COLOR_WINDOWTEXT);
  if ((long)c == -1)
    c = cText;

  int i = 0;
  for (; i < iCols; ++i) {
    const char* cstr = ColorStruct[i].sName->scVal();
    ose->AddString(cstr);
    Color color(ColorStruct[i].r,ColorStruct[i].g,ColorStruct[i].b);
    ose->SetItemData(i,color);
    if (iCol == -1 && (long)c == (long)color)
      iCol = i;
  }
  if (iCol == -1) {
    if ((long)c == (long)cText) {
      iCol = 0;
      i += 1;
      String s = SCLColDefault;
      ose->InsertString(0, s.sVal());
      ose->SetItemData(0,c);
    }
    else
      iCol = i;
  }
  String s = SCLColCustom;
  idCustom = ose->AddString(s.sVal());
  ose->SetItemData(idCustom,c);
  ose->SetCurSel(iCol);
  ose->setNotify(this, (NotifyProc)&FieldColorSimple::SelChanged, Notify(CBN_SELCHANGE));
  ose->SetFont(frm()->fnt);  // Yes on !
}

int FieldColorSimple::SelChanged(Event* )
{
  int id = ose->GetCurSel();
  CString s;
  ose->GetLBText(id, s);
  if (id == idCustom)
    CreateColor(0);
  return 0;
}

bool FieldColorSimple::fDrawIcon()
{
    return false;
}

bool FieldColorSimple::DrawObject(long value, DRAWITEMSTRUCT* ds)
{
  CPen *pen = new CPen();
  CBrush *brush = new CBrush();
  pen->CreatePen(PS_NULL, 1, (COLORREF)0);
	COLORREF clr = GetSysColor(COLOR_WINDOW);
  brush->CreateSolidBrush(clr);
  if (-2 == value) {
    delete pen;
    Color cPen = GetSysColor(COLOR_WINDOWTEXT);
    if (ds->itemState & ODS_SELECTED)
      cPen = GetSysColor(COLOR_HIGHLIGHTTEXT);
    pen = new CPen();
    pen->CreatePen(PS_SOLID, 1, cPen);
  }
  else {
    delete brush;
    brush = new CBrush();
    brush->CreateSolidBrush(value);
  }
  HBRUSH oldBrush=(HBRUSH)SelectObject(ds->hDC, *brush);
  HPEN oldPen=(HPEN)SelectObject(ds->hDC, *pen);
  Rectangle(ds->hDC, ds->rcItem.left + 2, ds->rcItem.top+2,
                     ds->rcItem.left + 15, ds->rcItem.top+15);
  SelectObject(ds->hDC, oldBrush);
  SelectObject(ds->hDC, oldPen);
  delete brush;
  delete pen;
  return true;
}

void FieldColorSimple::SetVal(Color clr)
{
  for (int id = 0; id < iCols; ++id) 
  {
    Color c = (Color)ose->GetItemData(id);
    if (c == clr) {
      ose->SetCurSel(id);
      return;
    }
  }
  ose->SetItemData(idCustom,clr);
  ose->SetCurSel(idCustom);
}

int FieldColorSimple::CreateColor(void * Evt) 
{
  Color c = ose->GetItemData(idCustom);
  if (long(c) <= 0)
    c = Color(255,64,64);
  CColorDialog frmCS(c, CC_ANYCOLOR|CC_SHOWHELP, _frm->wnd()) ;
  if (frmCS.DoModal()==IDOK) 
  {
    c = frmCS.GetColor();
    ose->SetItemData(idCustom,c);
    ose->SetCurSel(idCustom);
  }

  return 1;
}  


//---------[ FieldColor ]---------------------------------------------------------------------------------------
FieldColor::FieldColor(FormEntry* p, const String& sQuestion,
            Color* color)
   : FieldGroup(p)
{
  if (sQuestion.length() != 0)
    new StaticTextSimple(this, sQuestion);
  fcs = new FieldColorSimple(this, color);
  if (children.iSize() > 1) // also static text
    children[1]->Align(children[0], AL_AFTER);
  fcs->psn->SetBound(0,-1,0,0);
  pbCreate = new OwnButtonSimple(this, "CreateBut",
				 (NotifyProc)&FieldColor::CreateColor, 
				 true, false);
  pbCreate->Align(fcs, AL_AFTER);
  pbCreate->SetIndependentPos();
}

void FieldColor::create() 
{ 
  // glue create button to field before it
  pbCreate->psn->iPosX = fcs->psn->iPosX+fcs->psn->iMinWidth;
  CreateChildren();
}

int FieldColor::CreateColor(void* Evt) 
{
  return fcs->CreateColor(Evt);
}

FieldFillColor::FieldFillColor(FormEntry* parent, const String& sQuestion,
		 Color* color)
  : FieldGroup(parent)
{
  if (sQuestion.length() != 0)
    new StaticTextSimple(this, sQuestion);
  fcs = new FieldFillColorSimple(this, color);
  if (children.iSize() > 1) // also static text
    children[1]->Align(children[0], AL_AFTER);
}

void FieldFillColorSimple::create()
{
  FieldColorSimple::create();
  int iSel = ose->GetCurSel();
  String s = SCLColTransparent;
  ose->InsertString(0, s.sVal());
  ose->SetItemData(0,-2);
  if (*val == -2)
    ose->SetCurSel(0);
  else {
    iSel += 1;
    ose->SetCurSel(iSel);
  }
  idCustom += 1;
}

void FieldFillColorSimple::StoreData()
{
  FieldColorSimple::StoreData();
  if (1 == *val)  // correct that black becomes 1 by ose.
    *val = 0;
}

//-------[ ColorSelector ]------------------------------------------------------------------------
ColorSelector::ColorSelector(FormEntry* parent, Color* col, bool fTr)
: BaseSelector(parent), clr(col), fTrans(fTr)
{
  SetWidth(60);
  style = LBS_HASSTRINGS | WS_VSCROLL;
}

void ColorSelector::create()
{
  ColorInitialize();
  BaseSelector::create();
  int iCol = -1;
  Color c = *clr;
  Color cText = GetSysColor(COLOR_WINDOWTEXT);
  if ((long)c == -1)
    c = cText;
  else if ((long)c == -2)
    iCol = 0;  
  if (fTrans) {  
    String s = SCLColTransparent;
    lb->AddString(s.sVal());
    lb->SetItemData(0,-2);
  }  
  int i = 0;
  for (; i < iCols; ++i) {
    int id = lb->AddString(ColorStruct[i].sName->scVal());
    Color color(ColorStruct[i].r,ColorStruct[i].g,ColorStruct[i].b);
    lb->SetItemData(id,color);
    if (iCol == -1 && (long)c == (long)color)
      iCol = i + fTrans;
  }
  if (iCol == -1) {
    if ((long)c == (long)cText) {
      iCol = 0;
      i += 1;
      String s = SCLColDefault;
      lb->InsertString(0, s.sVal());
      lb->SetItemData(0,c);
    }
    else {
      iCol = i + fTrans;
    }  
  }
  String s = SCLColCustom;
  idCustom = lb->AddString(s.sVal());
  lb->SetItemData(idCustom,c);
  lb->SetCurSel(iCol);
  lb->setNotify(this, (NotifyProc)&ColorSelector::DoubleClick, Notify(LBN_DBLCLK));
  lb->setNotify(this, (NotifyProc)&ColorSelector::DrawItem, WM_DRAWITEM);
}

void ColorSelector::StoreData()
{
  int id = lb->GetCurSel();
  *clr = (Color)lb->GetItemData(id);
}

String ColorSelector::sName(int id)
{
  CString s;
  lb->GetText(id, s);
  return String(s);
}

void ColorSelector::DrawItem(Event * ev)
{
  DrawItemEvent *dev=dynamic_cast<DrawItemEvent *>(ev);
  ISTRUE(fINotEqual, dev, (DrawItemEvent *)NULL);

  DRAWITEMSTRUCT *dis=reinterpret_cast<DRAWITEMSTRUCT *> (dev->lParm);

  int id = dis->itemID;
  CString s;
  CRect rct;
  CRgn rgn;
  lb->GetClientRect(&rct);
  if ( dis->rcItem.bottom > rct.Height() || dis->rcItem.top < 0 ) return;
  lb->GetText(id, s);
  long iColor = (Color)lb->GetItemData(id);
  Color col(iColor);
  bool fSel = lb->GetCurSel() == id;
  lb->idw.DrawItem(dis, 20, String(s), fSel, true, false);
  Color c = GetSysColor(COLOR_WINDOW);
  CPen *pen = new CPen();
  pen->CreatePen(PS_NULL, 1, (COLORREF)0);
  CBrush *brush = new CBrush();
  brush->CreateSolidBrush(c);
  if (-2 == iColor) 
  {
    delete pen;
    Color cPen = GetSysColor(COLOR_WINDOWTEXT);
    pen = new CPen();
    pen->CreatePen(PS_SOLID, 1, cPen);
  }
  else {
    delete brush;
    brush = new CBrush();
    brush->CreateSolidBrush(col);
  }
  HBRUSH oldBrush=(HBRUSH)SelectObject(dis->hDC, *brush);
  HPEN oldPen = (HPEN)SelectObject(dis->hDC, *pen);
  Rectangle(dis->hDC, dis->rcItem.left + 2, dis->rcItem.top+2,
                     dis->rcItem.left + 15, dis->rcItem.top+15);
  SelectObject(dis->hDC, oldBrush);
  SelectObject(dis->hDC, oldPen);
  delete brush;
  delete pen;

}

int ColorSelector::CustomColor(Event *)
{
  Color c = lb->GetItemData(idCustom);
  if (c == Color(0,0,0))
    c = Color(255,64,64);

  CColorDialog frmCS(c, CC_ANYCOLOR|CC_SHOWHELP, _frm->wnd()) ;
  if (frmCS.DoModal()==IDOK) 
  {
    c = frmCS.GetColor();
    lb->SetCurSel(idCustom);
    lb->SetItemData(idCustom,c);
    lb->Invalidate();
  }
  return 0;
}

void ColorSelector::DoubleClick(Event* Evt)
{
  int id = lb->GetCurSel();
  if (id == idCustom) {
    CustomColor(0);
    return ;
  }
  else 
    if (0 == Evt)
      return ;
    else
      frm()->OnOK();
}

void ColorSelector::SetVal(Color clr)
{
  for (int id = 0; id < iCols; ++id) 
  {
    Color c = lb->GetItemData(id);
    if (c == clr) {
      lb->SetCurSel(id);
      return;
    }
  }  
  lb->SetItemData(idCustom,clr);
  lb->SetCurSel(idCustom);
}





