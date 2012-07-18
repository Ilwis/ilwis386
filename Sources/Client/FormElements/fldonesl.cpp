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

#define FLDONESELWIDTH    FLDNAMEWIDTH

#include "Client\Headers\formelementspch.h"
#include "Client\ilwis.h"

 
FieldOneSelect::FieldOneSelect(FormEntry* parent, long* value, bool fSrt, bool _edit)
: FormEntry(parent),
  idw(false), ose(NULL)
{
  val = value;
  psn->iMinWidth = (short)FLDONESELWIDTH;
  fSort = fSrt;
  edit = _edit;
}

FieldOneSelect::~FieldOneSelect()
{
   if (ose) delete ose;
}

void FieldOneSelect::show(int sw)
{
	if (ose && ose->GetSafeHwnd() )
  ose->ShowWindow(sw);
}

void FieldOneSelect::create()
{
	zPoint pntFld = zPoint(psn->iPosX, psn->iPosY);
	if (fSort)
		ose = new OneSelectEdit(this,_frm, pntFld, Id(), val, CBS_SORT, psn->iMinWidth, edit);
	else
		ose = new OneSelectEdit(this,_frm, pntFld, Id(), val, 0L, psn->iMinWidth, edit);
	if (_npChanged)
		ose->setNotify(_cb, _npChanged, Notify(CBN_SELCHANGE));
	ose->SetFont(frm()->fnt);

	CreateChildren();
}

String FieldOneSelect::sGetText()
{
    if ( !fShow() ) return "";
    CString s;
    if ( ose) ose->GetWindowText(s);
    return String(s);
}

void FieldOneSelect::StoreData()
{
	if (ose == 0)
		return;

  ose->storeData();
  FormEntry::StoreData();
}

bool FieldOneSelect::fDrawIcon()
{
    return true;
}

void FieldOneSelect::DrawItem(Event* ev)
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
  if ( !fDrawIcon())
  {
    idw.DrawItem(dis, 20, String(s1), fSel, true, false);
		DrawObject(tmp,dis);
  }
  else
    idw.DrawItem(dis, String(s1), fSel, true, false);
		
}

bool FieldOneSelect::DrawObject(long, DRAWITEMSTRUCT* )
{
  return false;
}

void FieldOneSelect::SetFocus()
{
  ose->SetFocus();
}

void FieldOneSelect::SetCallBack(NotifyProc np)
{
	FormEntry::SetCallBack(np);
}

void FieldOneSelect::SetCallBack(NotifyProc np, CallBackHandler* cb)
{
	FormEntry::SetCallBack(np, cb);

	if (_npChanged && ose)
		ose->setNotify(_cb, _npChanged, Notify(CBN_SELCHANGE));
}


BEGIN_MESSAGE_MAP(OneSelectEdit, OwnerDrawCB)
  ON_WM_KILLFOCUS()
  ON_WM_SETFOCUS()
END_MESSAGE_MAP()

OneSelectEdit::OneSelectEdit(FormEntry* fe, FormBase* parent, zPoint pos,
                             int id, long *v, long cbs_sort , int iWidth, bool edit) 
	: OwnerDrawCB(fe, WS_TABSTOP | WS_VSCROLL | WS_GROUP | cbs_sort |
                CBS_AUTOHSCROLL | CBS_OWNERDRAWFIXED | CBS_HASSTRINGS |
				(edit ? CBS_DROPDOWN : CBS_DROPDOWNLIST),
                CRect(pos, zDimension(iWidth, 250)), 
	              parent->wnd(), id)
{
  ilwapp = IlwWinApp()->Context();
  val = v;
  parent->setNotify(this,(NotifyProc)&OneSelectEdit::WindowPosChanging, WM_WINDOWPOSCHANGING);
  parent->setNotify(this,(NotifyProc)&OneSelectEdit::DrawItem, WM_DRAWITEM);
//create();
}

OneSelectEdit::~OneSelectEdit() 
{
}

void OneSelectEdit::SetFont(CFont* pFont, BOOL bRedraw)
{
	OwnerDrawCB::SetFont(pFont, bRedraw); // parent class

    CDC pdc; 
    pdc.CreateCompatibleDC(0);

	CFont *oldFont = pdc.SelectObject(pFont);
	CSize cs = pdc.GetTextExtent(CString("gh"));
	pdc.SelectObject(oldFont);

	SetItemHeight(-1, cs.cy + 3);
}

int OneSelectEdit::DrawItem(Event* ev)
{
  _fe->DrawItem(ev);
  return 1;
}

void OneSelectEdit::MeasureItem( LPMEASUREITEMSTRUCT lpMeasureItemStruct )
{
  zDisplay canvas(this);
  canvas.pushFont(_fe->frm()->fnt);
  zFontInfo info = zFontInfo(&canvas);
  lpMeasureItemStruct->itemHeight = info.iPixHeight();
  canvas.popFont();
}

int OneSelectEdit::WindowPosChanging(Event *)
{
  ShowDropDown(FALSE);
  return 0;
}

int OneSelectEdit::storeData()
{
  int id = GetCurSel();
  if (id < 0) return 0;

  if (val) 
  {
    *val = GetItemData(id);
    if (*val == 0)
      *val = id;
  }  
  return 1;
}

void OneSelectEdit::OnSetFocus(CWnd *old)
{
  OwnerDrawCB::OnSetFocus(old);
  ilwapp->setHelpItem(_fe->htp());
}

void OneSelectEdit::OnKillFocus(CWnd *old)
{
  OwnerDrawCB::OnKillFocus(old);
  ilwapp->setHelpItem(_fe->frm()->htp());
}

// FieldOneSelectStringSimple -------------------------------------------------------------------------
FieldOneSelectStringSimple::FieldOneSelectStringSimple(FormEntry* parent, long* value, const vector<String>& vstr, bool edit)
: FieldOneSelect(parent, value,false,edit)
, vs(vstr), localIndex(-1), txt(0)
{
  SetWidth(75);
}

FieldOneSelectStringSimple::FieldOneSelectStringSimple(FormEntry* parent, String* text, const vector<String>& vstr, bool edit)
: FieldOneSelect(parent, &localIndex,false,edit)
, vs(vstr), txt(text)
{
  SetWidth(75);
}

FieldOneSelectStringSimple::~FieldOneSelectStringSimple()
{
}

void FieldOneSelectStringSimple::create()
{
  FieldOneSelect::create();
  vector<String>::const_iterator iter;
  for (iter = vs.begin(); iter != vs.end(); ++iter) 
    ose->AddString((*iter).c_str());
	ose->SetCurSel(*val);
}

void FieldOneSelectStringSimple::SetFocus() {
	if ( ose) {
		ose->SetFocus();
		ose->ShowDropDown();
	}
}

void FieldOneSelectStringSimple::StoreData() {
	ose->storeData();
	if (txt && localIndex >= 0) {
		*txt = vs[localIndex];
	}
}

// FieldOneSelectString -------------------------------------------------------------------------------
FieldOneSelectString::FieldOneSelectString(FormEntry* parent, const String& sQuestion, long* value, const vector<String>& vs, bool edit)
: FieldGroup(parent)
{
  if (sQuestion.length() != 0)
    new StaticTextSimple(this, sQuestion);
  foss = new FieldOneSelectStringSimple(this, value, vs, edit);
  if (children.iSize() > 1) // also static text
    children[1]->Align(children[0], AL_AFTER);
}

FieldOneSelectString::FieldOneSelectString(FormEntry* parent, const String& sQuestion, String* value, const vector<String>& vs, bool edit)
: FieldGroup(parent)
{
  if (sQuestion.length() != 0)
    new StaticTextSimple(this, sQuestion);
  foss = new FieldOneSelectStringSimple(this, value, vs, edit);
  if (children.iSize() > 1) // also static text
    children[1]->Align(children[0], AL_AFTER);
}

FieldOneSelectString::~FieldOneSelectString()
{
}

void FieldOneSelectString::SetCallBack(NotifyProc np) 
{
  foss->SetCallBack(np); 
  FormEntry::SetCallBack(np);
}

void FieldOneSelectString::SetCallBack(NotifyProc np, CallBackHandler* cb) 
{ 
  foss->SetCallBack(np, cb); 
  FormEntry::SetCallBack(np, cb);
}

void FieldOneSelectString::SetComboWidth(short iWidth)
{
	foss->SetWidth(iWidth);
}

void FieldOneSelectString::SelectVal(const String& sVal) 
{ 
	foss->ose->SelectString(-1,sVal.c_str());
}

void FieldOneSelectString::SetVal(int iVal)
{
	if (foss->ose)
		foss->ose->SetCurSel(iVal);
}

int FieldOneSelectString::iVal()
{
	if (foss->ose)
		return foss->ose->GetCurSel();
	else
		return -1;
}

void FieldOneSelectString::resetContent(const vector<String>& vs) {
	if ( foss && foss->ose) {
		foss->ose->ResetContent();
		for(int  i =0; i < vs.size(); ++i) {
			foss->ose->AddString(vs[i].c_str());
		}
	}
}


