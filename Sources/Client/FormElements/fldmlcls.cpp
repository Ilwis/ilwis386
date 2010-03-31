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
/* FieldClassList
   by Wim Koolhoven, october 1994
   (c) Ilwis System Development ITC
	Last change:  WK   24 Aug 98    6:28 pm
*/
#include "Client\Headers\formelementspch.h"
#include "Client\FormElements\fldmlcls.h"
#include "Engine\Domain\dmclass.h"
#include "Headers\Htp\Ilwismen.htp"

/*
extern "C" {
// from Undocumented Windows
DWORD FAR PASCAL DragObject(HWND,HWND,WORD,WORD,NPSTR,HANDLE);	// USER.464
BOOL FAR PASCAL DragDetect(HWND,LPPOINT);  // USER.465
}*/

GeneralClassSelector::GeneralClassSelector(FormEntry* par)
: BaseSelector(par)
{
  psn->iMinWidth = (short)(psn->iMinWidth * 1.5);
}

void GeneralClassSelector::create()
{
  BaseSelector::create();
  lb->setNotify(this, (NotifyProc)&GeneralClassSelector::DrawItem, WM_DRAWITEM); 
}

String GeneralClassSelector::sName(int id)
{
  if (lb) {
    CString sTemp;
    lb->GetText(id,sTemp);
    return String(sTemp);
  }
  else
    return "";  
}

void GeneralClassSelector::DrawItem(Event* ev)
{
  DrawItemEvent *dev=dynamic_cast<DrawItemEvent *>(ev);
  ISTRUE(fINotEqual, dev, (DrawItemEvent *)NULL);

  DRAWITEMSTRUCT *dis=reinterpret_cast<DRAWITEMSTRUCT *> (dev->lParm);

  int id = dis->itemID;
  if (id < 0)
    return ;
  String str = sName(id);
  BOOL fSel = lb->GetSel(id);
  BOOL fUpNgbSel = FALSE;
  if (fSel && id > 0)
    fUpNgbSel = lb->GetSel(id-1) > 0;
  idw.DrawItem(dis,str,fSel,true,fUpNgbSel);
  return ;
}

void GeneralClassSelector::SetSel(int id, bool fSel)
{
  lb->SetSel(id, fSel);
}

void GeneralClassSelector::SetSel(const String& str, bool fSel)
{
  int id = lb->FindStringExact(-1, str.scVal());
  lb->SetSel(id, fSel);
}

bool GeneralClassSelector::fSel(int id)
{
  return lb->GetSel(id) > 0;
}

int GeneralClassSelector::iGetSelected(IntBuf& buf)
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

MultiClassSelector::MultiClassSelector(FormEntry* par, DomainClass* dom)
: GeneralClassSelector(par), dc(dom)
{
  style = LBS_EXTENDEDSEL | LBS_MULTIPLESEL | WS_VSCROLL | LBS_HASSTRINGS;
}

void MultiClassSelector::create()
{
  GeneralClassSelector::create();
  lb->ResetContent();
  int iSize = dc->iNettoSize();
  for (int i = 1; i <= iSize; ++i)
    lb->AddString(dc->sValue(i,0).scVal());
}

ClassSequenceSelector::ClassSequenceSelector(FormEntry* par, DomainClass* dom, Array<int>& array)
: GeneralClassSelector(par),
  curArrow(Arrow), dc(dom), arr(array)
{
  style = LBS_EXTENDEDSEL | LBS_MULTIPLESEL | WS_VSCROLL;
  fDrag = false;
  cur = 0;
}

ClassSequenceSelector::~ClassSequenceSelector()
{
  lb->removeNotify(this,WM_LBUTTONDOWN);
  lb->removeNotify(this,WM_LBUTTONUP);
  lb->removeNotify(this,WM_MOUSEMOVE);
}

void ClassSequenceSelector::create()
{
  GeneralClassSelector::create();
  lb->ResetContent();
//  int iLow = arr.iLower();
  int iLow = 0;
  for (unsigned int i = 0; i < arr.iSize(); ++i) {
    int id = Add(arr[iLow+i]);
    SetSel(id, false);
  }
  lb->setNotify(this,(NotifyProc)&ClassSequenceSelector::mouseButtonDown, WM_LBUTTONDOWN);
  lb->setNotify(this,(NotifyProc)&ClassSequenceSelector::mouseButtonUp, WM_LBUTTONUP);
  lb->setNotify(this,(NotifyProc)&ClassSequenceSelector::mouseMove, WM_MOUSEMOVE);
  lb->SetCaretIndex(0);
}

String ClassSequenceSelector::sName(int id)
{
  if (0 == lb)
    return "";
  long iRaw = (long)lb->GetItemData(id);
  return dc->sValueByRaw(iRaw,0);
}

FormEntry* ClassSequenceSelector::CheckData()
{
  if (0 == iClasses())
    return this;
  return 0;
}

void ClassSequenceSelector::StoreData()
{
  int iNr = iClasses();
  arr.Resize(iNr);
//  int iLow = arr.iLower();
  int iLow = 0;
  for (int i = 0; i < iNr; ++i)
    arr[iLow+i] = (long)lb->GetItemData(i);
}

int ClassSequenceSelector::mouseButtonDown(Event* Evt)
{
  MouseLBDownEvent *ev=dynamic_cast<MouseLBDownEvent *>(Evt);
	if (0 == ev)
		return 0;

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
	    cur = new zCursor("Lgbx16Cur");
	    lb->SetCapture();
      CRect rect;
	    lb->GetWindowRect(rect);
	    rect.top -= 20;
	    rect.bottom += 20;
	    ClipCursor(&rect);
	    ::SetCursor(*cur);
    }
  }
  return 0;
}

int ClassSequenceSelector::mouseButtonUp(Event* Evt)
{
  MouseLBUpEvent *ev=reinterpret_cast<MouseLBUpEvent *>(Evt);
  if (ev) 
  {
    if (!fDrag)
      return 0;
    fDrag = false;
    ::ReleaseCapture();
    ClipCursor(0);
    ::SetCursor(curArrow);
    if (cur) 
    {
      delete cur;
      cur = 0;
    }
    int iTop = lb->GetTopIndex();
    int iHeight = lb->GetItemHeight(0);
		CPoint pt = ev->pos();
    int id = iTop + pt.y / iHeight;
    if (id != idDrag) 
    {
      long iRaw = (long)lb->GetItemData(idDrag);
      String s = sName(idDrag);
      lb->DeleteString(idDrag); 
      if (id < 0)
	      id = 0;
      if (id > lb->GetCount())
        id = lb->AddString(s.scVal());
      else
        id = lb->InsertString(id, s.scVal()); 
      lb->SetCaretIndex(id); 
      lb->SetItemData(id, iRaw);
    }
    lb->SetSel(id);
  }
  return 0;
}

int ClassSequenceSelector::mouseMove(Event *)
{
  if (fDrag)
    return 1;
  else
    return 0;
}

int ClassSequenceSelector::idActive()
{
  if (lb->GetCount() == 0)
    return -1;
  return lb->GetCaretIndex();
}

int ClassSequenceSelector::Add(const String& s)
{
  long iRaw = dc->iRaw(s);
  return Add(iRaw);
}

int ClassSequenceSelector::Add(long iRaw)
{
  int iNr = lb->GetCount();
  int id;
  for (id = 0; id < iNr; ++id) {
    long iRawId = (long)lb->GetItemData(id);
    if (iRaw == iRawId) {
      lb->SetSel(id);
      return id;
    }
  }
  id = lb->AddString(sName(id).scVal());
  lb->SetSel(id);
  lb->SetCaretIndex(id);
  lb->SetItemData(id, iRaw);
  return id;
}

void ClassSequenceSelector::Del(int id)
{
  if (id >= 0) {
    lb->DeleteString(id);
  }
}

int ClassSequenceSelector::iClasses() const
{
  return lb->GetCount();
}

String ClassSequenceSelector::sGetText()
{
  String s;
  int iNr = lb->GetCount();
  for (int i = 0; i < iNr; ++i) {
    s &= sName(i);
    s &= "\r\n";
  }
  return s;
}

FieldMultiClass::FieldMultiClass(FormEntry* par, const String& sQuestion, DomainClass* dom, Array<int>& arr)
: FieldGroup(par, true)
{
  StaticTextSimple* sts = 0;
  if ("" != sQuestion) {
    sts = new StaticTextSimple(this, sQuestion);
    sts->SetIndependentPos();
  }
  mcs = new MultiClassSelector(this, dom);
  if (sts)
    mcs->Align(sts, AL_UNDER);
  FieldBlank* fb = new FieldBlank(this, 2);
  fb->Align(mcs, AL_AFTER);
  PushButton* pbAdd = new PushButton(this, " > ", (NotifyProc)&FieldMultiClass::Add, true);
  PushButton* pbDel = new PushButton(this, " < ", (NotifyProc)&FieldMultiClass::Del, true);
  pbAdd->SetWidth(30);
  pbDel->SetWidth(30);
  cacs = new ClassSequenceSelector(this, dom, arr);
  cacs->Align(fb, AL_AFTER);
}

void FieldMultiClass::create()
{
  FieldGroup::create();
}

int FieldMultiClass::Add(Event*)
{
  cacs->SetSel(-1,false);
  IntBuf buf;
  int iNr = mcs->iGetSelected(buf);
  for (int i = 0; i < iNr; ++i) {
    String s = mcs->sName(buf[i]);
    mcs->SetSel(s, false);
    cacs->Add(s);
  }
  return 0;
}

int FieldMultiClass::Del(Event*)
{
  int iNr = cacs->iClasses();
  for (int i = iNr - 1; i >= 0; --i) {
    if (cacs->fSel(i)) {
      String s = cacs->sName(i);
      cacs->Del(i);
      mcs->SetSel(s, true);
    }
  }
  return 0;
}

String FieldMultiClass::sGetText()
{
  return cacs->sGetText();
}





