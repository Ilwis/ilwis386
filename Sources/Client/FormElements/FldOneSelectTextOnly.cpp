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
#include "Client\FormElements\FldOneSelectTextOnly.h"

FieldOneSelectTextOnly::FieldOneSelectTextOnly(FormEntry* parent, String* sV, bool fSort) :
  FieldOneSelect(parent, &iDummy),
  sVal(sV)
{
		*val=1;
}

void FieldOneSelectTextOnly::AddString(const String& str)
{
	if (ose)
		ose->AddString(str.c_str());
}

void FieldOneSelectTextOnly::SelectItem(const String& str)
{
	if (ose)
  {
		int iIndex = ose->FindString(-1, str.c_str());
		if (iIndex >= 0)
			ose->SetCurSel(iIndex);
  }
}

void FieldOneSelectTextOnly::SetFocus() {
	if ( ose) {
		ose->SetFocus();
		ose->ShowDropDown();
	}

}

void FieldOneSelectTextOnly::DrawItem(Event* ev)
{
	DrawItemEvent *dev = dynamic_cast<DrawItemEvent *>(ev);
	ISTRUE(fINotEqual, dev, (DrawItemEvent *)NULL);
	DRAWITEMSTRUCT *dis = reinterpret_cast<DRAWITEMSTRUCT *> (dev->lParm);

	int id = dis->itemID;
	CString s;
	if (id < 0) 
		return;

	ose->GetLBText(id, s);
	bool fSel = ose->GetCurSel() == id;
	DWORD tmp = ose->GetItemData(id);
	string s1 = s;
	idw.DrawItem(dis, 3, String(s1), fSel, true, false);
	DrawObject(tmp,dis);
}

void FieldOneSelectTextOnly::StoreData()
{
	if (ose)
  {
		int iIndex = ose->GetCurSel();
		if (iIndex >= 0 )
    {
			CString str;
			ose->GetLBText(iIndex, str);
      *sVal = String("%s", str);
		}
		else
			*sVal = sUNDEF;
	}
}
