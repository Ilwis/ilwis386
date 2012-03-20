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
/* Selector
   by Wim Koolhoven, june 1994
   (c) Ilwis System Development ITC
	Last change:  WK   11 Aug 98   12:14 pm
*/
#include "Client\Headers\formelementspch.h"
#include "Client\FormElements\selector.h"
#include "Client\Base\ZappToMFC.h"

//-----------------------------------------------------------------------------------
BaseSelector::BaseSelector(FormEntry* par)
  : FormEntry(par,0,true),
    lb(0),
    idw(false)
{
  psn->iMinWidth = psn->iWidth = FLDNAMEWIDTH;
  psn->iMinHeight = psn->iHeight = 150;
  style = LBS_MULTIPLESEL | WS_VSCROLL;
}

void BaseSelector::create()
{
  zPoint pntFld = zPoint(psn->iPosX,psn->iPosY);
  zDimension dimFld = zDimension(psn->iWidth,psn->iMinHeight);
  lb = new OwnerDrawListBox(this, LBS_OWNERDRAWFIXED |  LBS_NOTIFY | style |
		                        LBS_DISABLENOSCROLL | WS_VSCROLL | WS_BORDER | WS_TABSTOP,
                            CRect(pntFld, dimFld), frm()->wnd() , Id());
  lb->SetFont(frm()->fnt);
  CreateChildren();
}

BaseSelector::~BaseSelector()
{
  delete lb;
}

void BaseSelector::DrawItem(Event* ev)
{
  DrawItemEvent *dev=dynamic_cast<DrawItemEvent *>(ev);
  ISTRUE(fINotEqual, dev, (DrawItemEvent *)NULL);

  DRAWITEMSTRUCT *dis=reinterpret_cast<DRAWITEMSTRUCT *> (dev->lParm);

  int id = dis->itemID;
	if (id < 0)
		return;

	String s = sName(id);
	CDC dc;
	dc.Attach(dis->hDC);
  int iV = dis->rcItem.top / 2 + dis->rcItem.bottom / 2 + 5;
  int iLeft = dis->rcItem.left + 20;

	bool fSel = ((dis->itemAction | ODA_SELECT) && (dis->itemState & ODS_SELECTED));

  idw.DrawItem(dis, s, fSel, true, false);
//  dc.TextOut( iLeft, iV, s.c_str(), s.length());

	dc.Detach();
}

void BaseSelector::show(int sw)
{
  if (lb)
    lb->ShowWindow(sw);
}

//----[ StringArrayLister ]----------------------------------------------------------------------------------
StringArrayLister::StringArrayLister(FormEntry* fe, const Array<String>& arr)
: BaseSelector(fe) 
{
  style = WS_VSCROLL | LBS_HASSTRINGS;
  as.resize(arr.size());
  copy(arr.begin(), arr.end(), as.begin());
}

void StringArrayLister::StoreData()
{}

void StringArrayLister::create()
{
  BaseSelector::create();
  lb->ResetContent();
  for (int i = 0; i < as.iSize(); ++i)
    lb->AddString(as[i].c_str());

	lb->setNotify(this,(NotifyProc)&StringArrayLister::OnKeyUp, WM_KEYDOWN);		
}

String StringArrayLister::sName(int id)
{
  if ( id == -1)
	return sUNDEF;
  if (lb) 
  {
    CString s;
    lb->GetText(id, s);
    return String(s);
  }
  else
    return "";  
}

String _export StringArrayLister::sGetSelectedString(){
	return sName(iGetSingleSelection());
}


void StringArrayLister::Remove(int index) {
	if (lb){
		lb->DeleteString(index);
	}
}
String StringArrayLister::sGetText()
{
  if (!fShow() || !lb) return "";
  int iCnt = lb->GetCount();
  String sText;
  CString sLbText;
  for (int i = 0; i < iCnt; ++i) 
  {
    lb->GetText(i, sLbText);
    sText &= String(sLbText);
    sText &= "\r\n";
  }
  return sText;
}

void StringArrayLister:: resetContent(const Array<String>& arr){
  lb->ResetContent();
  as.resize(arr.size());
  copy(arr.begin(), arr.end(), as.begin());
  for(int i=0; i < arr.size(); ++i) 
  {
	  lb->AddString(arr[i].c_str());
  }
}

void StringArrayLister::AddStrings(const Array<String>& as)
{
	Clear();
	for (int i = 0; i < as.iSize(); ++i)
		lb->AddString(as[i].c_str());
}

void StringArrayLister::AddString(const String& s, int iWhere)
{
	if ( iWhere != iUNDEF)
		lb->InsertString(iWhere, s.c_str());
	else
		lb->AddString(s.c_str());
}

void StringArrayLister::Clear() {
	lb->ResetContent();
}

void StringArrayLister::SetSel(int index, bool select) {
	if (lb)
		lb->SetCurSel(index);
}

int StringArrayLister::OnKeyUp( Event* ev) 
{
	int nKey = ev->wParm;
  if (nKey == VK_INSERT || nKey == 'C') 
	{
		short iReturn = HIWORD(GetKeyState(VK_CONTROL));
		if ( iReturn != 0)
		{
			zClipboard cb(lb);
			cb.clear();
			int id = lb->GetCurSel();
			if ( id != -1 )
			{
				CString cstr;
				lb->GetText(id, cstr);
				String ss(cstr);
				cb.add(ss.sVal());				
			}				
			return -2;
		}			
  }
  return -1;
}

void StringArrayLister::DrawItem(Event* ev)
{
	DrawItemEvent *dev = dynamic_cast<DrawItemEvent *>(ev);
	ISTRUE(fINotEqual, dev, (DrawItemEvent *)NULL);
	DRAWITEMSTRUCT *dis = reinterpret_cast<DRAWITEMSTRUCT *> (dev->lParm);

	int id = dis->itemID;
	CString s;
	if (id < 0) 
		return;

	lb->GetText(id, s);
	bool fSel = lb->GetCurSel() == id;
	DWORD tmp = lb->GetItemData(id);
	string s1 = s;
	idw.DrawItem(dis, 0, String(s1), fSel, true, false);
	//DrawObject(tmp,dis);
}

int StringArrayLister::iGetSingleSelection() {
	return lb->GetCurSel();
}






