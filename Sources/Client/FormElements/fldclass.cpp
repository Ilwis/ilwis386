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
/* FieldClass
   by Wim Koolhoven, june 1994
   (c) Ilwis System Development ITC
	Last change:  MS   23 Sep 97    3:01 pm
*/
#include "Client\Headers\formelementspch.h"
#include "Client\FormElements\fldclass.h"

FieldClassSimple::FieldClassSimple(FormEntry* p, long* iClass, DomainSort* dsrt)
  : FieldOneSelect(p, iClass)
{
  ds = dsrt;
  int iWidth = ds->iWidth();
  if (iWidth > 8) {
    if (iWidth > 40)
      iWidth = 40;
    iWidth += 4;
    iWidth *= 3.5;
    if (iWidth > 60)
      SetWidth(iWidth);
  }
  rpr = ds->rpr();  
//  setHelpItem(htpUiClass);
}

FieldClassSimple::~FieldClassSimple()
{
}


void FieldClassSimple::create()
{
  FieldOneSelect::create();
  ose->ResetContent();
  for (int i = 1; i <= ds->iNettoSize(); ++i) {
    long iRaw = ds->iKey(i);
    ose->AddString(ds->sValueByRaw(iRaw,0).sVal());
    ose->SetItemData(i-1,iRaw);
  }
  ose->SetFont(frm()->fnt);  // Yes on !
  SetVal(*val);
}

void FieldClassSimple::StoreData()
{
  int id = ose->GetCurSel();
	if (id < 0) {
		*val = iUNDEF;
		return;
	}
  CString s;
  ose->GetLBText(id, s);
  *val = ds->iRaw(String(s));
}

void FieldClassSimple::SetVal(long iClass)
{
  String sClass = ds->sValueByRaw(iClass, 0);
  ose->SelectString(-1, sClass.sVal());
}


FieldClass::FieldClass(FormEntry* p, const String& sQuestion,
             long* iClass, DomainSort* ds)
   : FieldGroup(p)
{
  if (sQuestion.length() != 0)
    new StaticTextSimple(this, sQuestion);
  fcs = new FieldClassSimple(this, iClass, ds);
  if (children.iSize() > 1) // also static text
    children[1]->Align(children[0], AL_AFTER);
}

bool FieldClassSimple::fDrawIcon()
{
    return false;
}

bool FieldClassSimple::DrawObject(long value, DRAWITEMSTRUCT* dis)
{
  if (!rpr.fValid())
    return false;
  if (0 == rpr->prc())
    return false;
  if (value == iUNDEF)  
    return false;
  Color clr = rpr->clrRaw(value);
  
  CPen *pen, *penDummy = new CPen();
  CBrush *brush, *brushDummy = new CBrush();

  pen = penDummy->FromHandle((HPEN)NULL_PEN);
  brush = brushDummy->FromHandle((HBRUSH)NULL_BRUSH);

  if (-2 == value) 
  {
    pen = new CPen();
    DWORD cPen = GetSysColor(COLOR_WINDOWTEXT);
    pen->CreatePen(BS_SOLID, 1, cPen);
  }
  else {
    brush = new CBrush();
    brush->CreateSolidBrush(clr);
  }
  HBRUSH oldBrush=(HBRUSH)SelectObject(dis->hDC, *brush);
  HPEN oldPen=(HPEN)SelectObject(dis->hDC, *pen);
  Rectangle(dis->hDC, dis->rcItem.left + 2, dis->rcItem.top+2,
                     dis->rcItem.left + 15, dis->rcItem.top+15);
  SelectObject(dis->hDC, oldBrush);
  SelectObject(dis->hDC, oldPen);
//  DeleteObject(*brush);/
//  DeleteObject(*pen);
  delete penDummy;
  delete brushDummy;
  if (value == -2) // not all pens may be deleted, some are system;
    delete pen ;
  else
    delete brush;
  return true;
}

void FieldClassSimple::DrawItem(Event* ev)
{
  DrawItemEvent *dev= dynamic_cast<DrawItemEvent *>(ev);
  ISTRUE(fINotEqual, dev, (DrawItemEvent *)NULL);
  DRAWITEMSTRUCT *dis=reinterpret_cast<DRAWITEMSTRUCT *> (dev->lParm);

  int id = dis->itemID;
  CString s;
  if (id < 0) return;
    
  ose->GetLBText(id, s);
  bool fSel = ose->GetCurSel() == id;
  DWORD tmp = ose->GetItemData(id);
	string s1 = s;
  if (rpr.fValid())
  {
    idw.DrawItem(dis, 20, String(s1), fSel, true, false);
		DrawObject(tmp,dis);
  }
  else
    idw.DrawItem(dis, 3, String(s1), fSel, true, false);
}





