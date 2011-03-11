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
/* $Log: /ILWIS 3.0/FormElements/fldonesl.cpp $
 * 
 * 14    14-11-02 18:43 Retsios
 * Merge from SMCE - Added SetComboWidth function, and Set and Get
 * functions for the combo selection
 * 
 * 16    27-09-02 17:10 Retsios
 * Added two functions to 1) trigger the combo box to select another
 * string, 2) Retrieve the index of the currently selected string
 * 
 * 15    25-09-02 19:33 Retsios
 * Added option to change the width of the combo box (the normal SetWidth
 * changes only the width of the label)
 * 
 * 13    26-10-01 17:38 Koolhoven
 * added class FieldOneSelectString
 * 
 * 12    14/07/00 17:26 Willem
 * Added a SetFont function that will also set the correct size of the
 * owner drawn combobox
 * 
 * 11    14-03-00 2:41p Martin
 * protected show against being not  initialized 
 * 
 * 10    1-09-99 4:19p Martin
 * layouting
 * 
 * 9     20/08/99 16:49 Willem
 * The (new) SetCallBack function now sets the callback to both the edit
 * control and the list box.
 * 
 * 8     1-07-99 15:53 Koolhoven
 * Call backs on FieldOneSelect now work
 * 
 * 7     9-06-99 3:38p Martin
 * Changes for general callbacks
 * 
 * 6     6/01/99 9:47a Martin
 * problems with % sign in strings.
 * 
 * 5     4/01/99 12:12p Martin
 * Standard font come from IlwisApp. This is a const font, caused several
 * problems
 * 
 * 4     15-03-99 17:06 Willem
 * Adjusted the remarks around the $Log: /ILWIS 3.0/FormElements/fldonesl.cpp $ marker
 * 
 * 14    14-11-02 18:43 Retsios
 * Merge from SMCE - Added SetComboWidth function, and Set and Get
 * functions for the combo selection
 * 
 * 16    27-09-02 17:10 Retsios
 * Added two functions to 1) trigger the combo box to select another
 * string, 2) Retrieve the index of the currently selected string
 * 
 * 15    25-09-02 19:33 Retsios
 * Added option to change the width of the combo box (the normal SetWidth
 * changes only the width of the label)
 * 
 * 13    26-10-01 17:38 Koolhoven
 * added class FieldOneSelectString
 * 
 * 12    14/07/00 17:26 Willem
 * Added a SetFont function that will also set the correct size of the
 * owner drawn combobox
 * 
 * 11    14-03-00 2:41p Martin
 * protected show against being not  initialized 
 * 
 * 10    1-09-99 4:19p Martin
 * layouting
 * 
 * 9     20/08/99 16:49 Willem
 * The (new) SetCallBack function now sets the callback to both the edit
 * control and the list box.
 * 
 * 8     1-07-99 15:53 Koolhoven
 * Call backs on FieldOneSelect now work
 * 
 * 7     9-06-99 3:38p Martin
 * Changes for general callbacks
 * 
 * 6     6/01/99 9:47a Martin
 * problems with % sign in strings.
 * 
 * 5     4/01/99 12:12p Martin
 * Standard font come from IlwisApp. This is a const font, caused several
 * problems
 * 
 * 3     15-03-99 16:06 Koolhoven*/
// Revision 1.4  1998/09/16 17:37:53  Wim
// 22beta2
//
// Revision 1.3  1997/09/26 11:09:41  Wim
// In DrawItem() if id < 0 do not draw but skip
//
/* ui/fldonesl.c
// FieldOneSelect
// by Wim Koolhoven, june 1994
// (c) Computer Department ITC
	Last change:  WK    9 Mar 98    5:35 pm
*/

#define FLDONESELWIDTH    FLDNAMEWIDTH

#include "Client\Headers\formelementspch.h"
#include "Client\ilwis.h"

 
FieldOneSelect::FieldOneSelect(FormEntry* parent, long* value, bool fSrt)
: FormEntry(parent),
  idw(false), ose(NULL)
{
  val = value;
  psn->iMinWidth = (short)FLDONESELWIDTH;
  fSort = fSrt;
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
		ose = new OneSelectEdit(this,_frm, pntFld, Id(), val, CBS_SORT, psn->iMinWidth);
	else
		ose = new OneSelectEdit(this,_frm, pntFld, Id(), val, 0L, psn->iMinWidth);
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
                             int id, long *v, long cbs_sort , int iWidth) 
	: OwnerDrawCB(fe, WS_TABSTOP | WS_VSCROLL | WS_GROUP | cbs_sort |
                CBS_AUTOHSCROLL | CBS_OWNERDRAWFIXED | CBS_HASSTRINGS |
                CBS_DROPDOWNLIST,
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
FieldOneSelectStringSimple::FieldOneSelectStringSimple(FormEntry* parent, long* value, const vector<string>& vstr)
: FieldOneSelect(parent, value)
, vs(vstr)
{
  SetWidth(75);
}

FieldOneSelectStringSimple::~FieldOneSelectStringSimple()
{
}

void FieldOneSelectStringSimple::create()
{
  FieldOneSelect::create();
  vector<string>::const_iterator iter;
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

// FieldOneSelectString -------------------------------------------------------------------------------
FieldOneSelectString::FieldOneSelectString(FormEntry* parent, const String& sQuestion, long* value, const vector<string>& vs)
: FieldGroup(parent)
{
  if (sQuestion.length() != 0)
    new StaticTextSimple(this, sQuestion);
  foss = new FieldOneSelectStringSimple(this, value, vs);
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
	foss->ose->SelectString(-1,sVal.scVal());
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
