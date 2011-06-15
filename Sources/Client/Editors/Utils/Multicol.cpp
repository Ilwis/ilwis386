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
/* $Log: /ILWIS 3.0/UserInterfaceObjects/Multicol.cpp $
 * 
 * 2     25-05-00 12:25p Martin
 * complex pointmap display works again
 * 
 * 1     24-05-00 3:05p Martin
// Revision 1.1  1998/09/16 17:33:58  Wim
// 22beta2
//
/* MultiColumnSelector, FieldMultiColumn
   by Wim Koolhoven, feb. 1998
  (c) Ilwis System Development ITC
	Last change:  WK   11 Aug 98   12:25 pm
*/
#define MULTICOL_C
#include "Client\Headers\formelementspch.h"
#include "Client\FormElements\selector.h"
#include "Client\Editors\Utils\MULTICOL.H"
#include "Client\FormElements\objlist.h"
#include "Client\FormElements\fldcolor.h"
#include "Headers\Hs\Drwforms.hs"
#include "Headers\Hs\Represen.hs"

//extern "C" {
//// from Undocumented Windows
//DWORD FAR PASCAL DragObject(HWND,HWND,WORD,WORD,NPSTR,HANDLE);	// USER.464
//BOOL FAR PASCAL DragDetect(HWND,LPPOINT);  // USER.465
//}

GeneralColumnSelector::GeneralColumnSelector(FormEntry* par, TablePtr* tblptr)
: BaseSelector(par), tbl(tblptr)
{
  psn->iMinWidth *= 1.5;
}

String GeneralColumnSelector::sName(int id)
{
  if (lb) 
	{
		CString sc;
		lb->GetText(id, sc);
    String s(sc);
    return s;
  }
  else
    return "";  
}

void GeneralColumnSelector::DrawItem(Event* ev)
{
  DrawItemEvent *dev=reinterpret_cast<DrawItemEvent *>(ev);
  DRAWITEMSTRUCT *dis=reinterpret_cast<DRAWITEMSTRUCT *> (dev->lParm);
  int id = dis->itemID;
  if (id < 0)
    return ;
  String str("%S.clm", sName(id));
  BOOL fSel = lb->GetCurSel() > 0;
  BOOL fUpNgbSel = FALSE;
  if (fSel && id > 0)
    fUpNgbSel = lb->GetSel(id - 1) > 0;
  idw.DrawItem(dis,str,fSel,true,fUpNgbSel);
  return ;
}

void GeneralColumnSelector::StoreData()
{
  // empty implementation!
}

void GeneralColumnSelector::SetSel(int id, bool fSel)
{
	lb->SetSel(id, (BOOL) fSel);
}

void GeneralColumnSelector::SetSel(const String& str, bool fSel)
{
	String s = str ;
  int id = lb->FindStringExact(-1, s.scVal());
	lb->SetSel(id);
}

bool GeneralColumnSelector::fSel(int id)
{
  return lb->GetSel(id) > 0;
}

int GeneralColumnSelector::iGetSelected(IntBuf& buf)
{
  int iCnt = lb->GetSelCount();
  int* pi = new int[iCnt];
  iCnt = lb->GetSelItems(iCnt, pi);
  buf.Size(iCnt);
  for (int i = 0; i < iCnt; ++i)
    buf[i] = pi[i];
  delete [] pi;
  return iCnt;
}

MultiColumnSelector::MultiColumnSelector(FormEntry* par, TablePtr* tblptr, long types)
: GeneralColumnSelector(par, tblptr), dmTypes(types)
{
  style = LBS_EXTENDEDSEL | LBS_MULTIPLESEL | WS_VSCROLL | LBS_HASSTRINGS;
}

void MultiColumnSelector::create()
{
  GeneralColumnSelector::create();
  lb->ResetContent();
  for (int i = 0; i < tbl->iCols(); ++i) {
    Column col = tbl->col(i);
    DomainType dmt = col->dm()->dmt();
    if (dmtype(dmt) & dmTypes) {
      String s = col->sName();
      lb ->AddString(s.scVal());
    }
  }
}

ColumnAndColorSelector::ColumnAndColorSelector(FormEntry* par, TablePtr* tbl, ColumnAndColorArray& array, bool fcolor)
: GeneralColumnSelector(par, tbl),
  curArrow(Arrow), fColor(fcolor), arr(array)
{
  style = LBS_EXTENDEDSEL | LBS_MULTIPLESEL | WS_VSCROLL;
  fDrag = false;
  cur = 0;
  iDfltCol = 4;
}

ColumnAndColorSelector::~ColumnAndColorSelector()
{
}

void ColumnAndColorSelector::CleanUp()
{
  for (int i = lb->GetCount() - 1; i >= 0; --i)
    Del(i);
}

void ColumnAndColorSelector::create()
{
  GeneralColumnSelector::create();
  lb->ResetContent();
  for (int i = 0; i < arr.iSize(); ++i) {
    int id = Add(arr[i].col);
    SetColor(id, arr[i].clr);
    SetSel(id, false);
  }
  lb->setNotify(this,(NotifyProc)&ColumnAndColorSelector::mouseButtonDown, WM_LBUTTONDOWN);
  lb->setNotify(this,(NotifyProc)&ColumnAndColorSelector::mouseButtonUp, WM_LBUTTONUP);
	lb->setNotify(this,(NotifyProc)&ColumnAndColorSelector::mouseMove, WM_MOUSEMOVE);
	lb->setNotify(this,(NotifyProc)&ColumnAndColorSelector::ChangeColor, WM_LBUTTONDBLCLK);
	lb->setNotify(this, (NotifyProc)&ColumnAndColorSelector::DrawItem, WM_DRAWITEM); 
 	lb->SetCaretIndex(0);
}

FormEntry* ColumnAndColorSelector::CheckData()
{
  if (0 == iCols())
    return this;
  return 0;
}

void ColumnAndColorSelector::StoreData()
{
  int iNr = iCols();
  arr.Resize(iNr);
  for (int i = 0; i < iNr; ++i) {
    arr[i].col = col(i);
    arr[i].clr = clr(i);
  }
}

void ColumnAndColorSelector::ShowColor(bool fCol)
{
  fColor = fCol;
  lb->Invalidate();
}

int ColumnAndColorSelector::mouseButtonDown(Event* Evt)
{
  MouseLBDownEvent *ev=dynamic_cast<MouseLBDownEvent *>(Evt);
  if (ev && !ev->fShiftPressed() && !ev->fCtrlPressed()) 
	{
    if (lb->GetCount() == 0)
      return 0;
    int iTop = lb->GetTopIndex();
    int iHeight = lb->GetItemHeight(0); 
    CPoint p = ev->pos();
    idDrag = iTop + p.y / iHeight;
    if (idDrag < lb->GetCount()) 
		{
      if (DragDetect(lb->m_hWnd,p))
			{
				if (cur)
					delete cur;
				fDrag = true;
				cur = new zCursor("Clm16Cur");
				lb->SetCapture();
				CRect rect;
				lb->GetWindowRect(&rect);
				rect.top -= 20;
				rect.bottom += 20;
				ClipCursor(&rect);
				::SetCursor(*cur);
				return 1; // list box should not get mouse button down event
			}
			else
				PostMessage(lb->m_hWnd, WM_LBUTTONUP, 0, Evt->lParm);
		}
  }
  return 0;
}

int ColumnAndColorSelector::mouseButtonUp(Event* Evt)
{
  MouseLBDownEvent *ev=dynamic_cast<MouseLBDownEvent *>(Evt);
  if (ev) 
	{
    if (!fDrag)
      return 0;
    fDrag = false;
    ::ReleaseCapture();
    ClipCursor(0);
    ::SetCursor(curArrow);
    if (cur) {
      delete cur;
      cur = 0;
    }
    int iTop = lb->GetTopIndex();
		int iHeight = lb->GetItemHeight(0);
    int id = iTop + ev->pos().y / iHeight;
    lb->SetSel(id); 
    if (id != idDrag) 
		{
      ColumnAndColor* cac = (ColumnAndColor*)lb->GetItemData(idDrag);
      char* str = cac->col->sName().sVal();
      lb->DeleteString(idDrag); 
      if (id < 0)
				id = 0;
      if (id > lb->GetCount())
				lb->AddString(str);
      else
				lb->InsertString(id, str);
			lb->SetItemData(id, (LPARAM)cac);
    }
  }
  return 0;
}

int ColumnAndColorSelector::mouseMove(Event *)
{
  if (fDrag)
    return 1;
  else
    return 0;
}

void ColumnAndColorSelector::DrawItem(Event* ev)
{
  if (!fColor)
    GeneralColumnSelector::DrawItem(ev);

  DrawItemEvent *dev=reinterpret_cast<DrawItemEvent *>(ev);
  DRAWITEMSTRUCT *dis=reinterpret_cast<DRAWITEMSTRUCT *> (dev->lParm);
	CRect rct;
  lb->GetClientRect(&rct);
  if ( dis->rcItem.bottom > rct.Height() || dis->rcItem.top < 0 ) return ;
  int id = dis->itemID;
  if (id < 0)
    return ;
  String str = sName(id);
  long iColor = clr(id);
  Color col = iColor;
  bool fSel = lb->GetCurSel() == id;
  idw.DrawItem(dis, 20, str, fSel, true, false);
  Color c = GetSysColor(COLOR_WINDOW);
  CPen *pen = new CPen(PS_NULL,1,(COLORREF )0);
  CBrush *brush = new CBrush();
  brush->CreateSolidBrush(c);
  if (-2 == iColor) 
	{
    delete pen;
    Color cPen = GetSysColor(COLOR_WINDOWTEXT);
		pen->CreatePen(PS_NULL, 1, (COLORREF)0);
  }
  else 
	{
    delete brush;
    brush = new CBrush();
    brush->CreateSolidBrush(col);
  }
	CDC dc;
	dc.Attach(dis->hDC);
  CBrush *oldBrush = dc.SelectObject(brush);
  CPen *oldPen = dc.SelectObject(pen);
  dc. Rectangle( dis->rcItem.left + 2, dis->rcItem.top+2,
                     dis->rcItem.left + 15, dis->rcItem.top+15);
  dc.SelectObject(oldBrush);
  dc.SelectObject(oldPen);
  delete brush;
  delete pen;
	dc.Detach();
  return ;

}

int ColumnAndColorSelector::idActive()
{
  if (lb->GetCount() == 0)
    return -1;
  return lb->GetCaretIndex();
}

String ColumnAndColorSelector::sName(int id)
{
  ColumnAndColor* cac = (ColumnAndColor*)lb->GetItemData(id);
  return cac->col->sName();
}

int ColumnAndColorSelector::Add(const Column& col)
{
  String s = col->sName();
  int iNr = lb->GetCount();
  if (iNr == 0) // when adding to empty box, always use same colors.
    iDfltCol = 4;
  int id;
  for (id = 0; id < iNr; ++id)
    if (s == sName(id)) {
      lb->SetCurSel(id);
      return id;
    }
  id = lb->AddString(s.scVal());
  ColumnAndColor* cac = new ColumnAndColor();
  cac->col = col;
  cac->clr = clrStd(++iDfltCol);
  lb->SetItemData(id, (LPARAM)cac);
  lb->SetCurSel(id);
  lb->SetCaretIndex(id);
  return id;
}

void ColumnAndColorSelector::Del(int id)
{
  if (id >= 0) 
	{
    ColumnAndColor* cac = (ColumnAndColor*)lb->GetItemData(id);
    delete cac;
    lb->DeleteString(id);
  }
}

int ColumnAndColorSelector::Del(const Column& col)
{
  String sName = col->sName();
  char* s = sName.sVal();
  int id = lb->FindStringExact( -1, s);
  if (id >= 0)
    lb->DeleteString(id);
  return id;
}

int ColumnAndColorSelector::iCols() const
{
  return lb->GetCount();
}

Column ColumnAndColorSelector::col(int id) const
{
  String str = (const_cast<ColumnAndColorSelector *>(this))->sName(id);
  Column col = tbl->col(str);
  return col;
}

Color ColumnAndColorSelector::clr(int id) const
{
  ColumnAndColor* cac = (ColumnAndColor*)lb->GetItemData(id);
  return cac->clr;
}

void ColumnAndColorSelector::SetColor(int id, Color clr)
{
  ColumnAndColor* cac = (ColumnAndColor*)lb->GetItemData(id);
  cac->clr = clr;
  lb->Invalidate();
}

int ColumnAndColorSelector::ChangeColor(Event*)
{
  class _export ColorForm: public FormWithDest
  {
  public:
    ColorForm(CWnd* wPar, const String& sCol, Color* clr)
    : FormWithDest(wPar, SDCTitleColor)
    {
      String sText(SDCRemSelColorFor_S.scVal(), sCol);
      StaticText* st = new StaticText(root, sText);
      st->SetIndependentPos();
      cs = new ColorSelector(root, (Color*)clr);
      PushButton* pb = new PushButton(root, SRPUiCustomColor,
        (NotifyProc)&ColorForm::CustomColor);
      pb->Align(cs, AL_UNDER);
  //    SetMenHelpTopic("ilwismen\\representation_class_editor_edit_item_raster.htm");
      create();
    }
  private:
    int CustomColor(Event*)
    {
      cs->CustomColor(0);
      return 0;
    }
    ColorSelector* cs;
  };
  if (!fColor)
    return 0;
  int id = idActive();
  if (id < 0) {
    MessageBeep(MB_ICONASTERISK);
    return 0;
  }
  Color color = clr(id);
  String sCol = col(id)->sName();
  ColorForm form(frm()->wnd(), sCol, &color);
  if (form.fOkClicked()) {
    SetColor(id, color);
  }
  return 0;
}

FieldMultiColumn::FieldMultiColumn(FormEntry* par, const String& sQuestion, TablePtr* ptr, ColumnAndColorArray& arr, long types, bool fCol)
: FieldGroup(par, true), tbl(ptr), fColor(fCol)
{
  StaticTextSimple* sts = 0;
  if ("" != sQuestion)
    sts = new StaticTextSimple(this, sQuestion);
  mcs = new MultiColumnSelector(this, tbl, types);
  if (sts)
    mcs->Align(sts, AL_UNDER);
  FieldBlank* fb = new FieldBlank(this, 2);
  fb->Align(mcs, AL_AFTER);
  PushButton* pbAdd = new PushButton(this, " > ", (NotifyProc)&FieldMultiColumn::Add, true);
  PushButton* pbDel = new PushButton(this, " < ", (NotifyProc)&FieldMultiColumn::Del, true);
  pbAdd->SetWidth(30);
  pbDel->SetWidth(30);
  cacs = new ColumnAndColorSelector(this, tbl, arr, fColor);
  cacs->Align(fb, AL_AFTER);
  pbColor = new PushButton(this, SDCUiButtonColor, (NotifyProc)&FieldMultiColumn::ChangeColor, true);
  pbColor->Align(cacs, AL_UNDER);
}

void FieldMultiColumn::CleanUp()
{
	cacs->CleanUp();
}

void FieldMultiColumn::create()
{
  FieldGroup::create();
  if (!fColor)
    pbColor->show(SW_HIDE);
}

void FieldMultiColumn::ShowColor(bool fCol)
{
  fColor = fCol;
  cacs->ShowColor(fColor);
  if (fColor)
    pbColor->show(SW_NORMAL);
  else
    pbColor->show(SW_HIDE);
}

int FieldMultiColumn::Add(Event*)
{
  cacs->SetSel(-1,false);
  IntBuf buf;
  int iNr = mcs->iGetSelected(buf);
  for (int i = 0; i < iNr; ++i) {
    String s = mcs->sName(buf[i]);
		int iPos = s.rfind('.');
		if ( iPos != -1 )
      s = s.substr(0, iPos);
    Column col = tbl->col(s);
    mcs->SetSel(s, false);
    if (col.fValid())
      cacs->Add(col);
  }
  return 0;
}

int FieldMultiColumn::Del(Event*)
{
  int iNr = cacs->iCols();
  for (int i = iNr - 1; i >= 0; --i) {
    if (cacs->fSel(i)) {
      Column col = cacs->col(i);
      if (!col.fValid())
        return 0;
      String s = col->sName();
      cacs->Del(i);
      mcs->SetSel(s, true);
    }
  }
  return 0;
}

int FieldMultiColumn::ChangeColor(Event*)
{
  return cacs->ChangeColor(0);
}


