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
/* FieldMirrotRotateType editor
   by Willem Nieuwenhuis, 23 August 1999
   Copyright Ilwis System Development ITC
*/

#include "Client\Headers\formelementspch.h"
#include "Headers\Hs\Appforms.hs"
#include "Client\FormElements\FldMirRotType.h"

String sRemoveAmpersand(const String& sItem)
{
	return sItem.sHead("&") + sItem.sTail("&");
}


FieldMirrorRotateType::FieldMirrorRotateType(FormEntry* par, long* iType)
	: FieldOneSelect(par, iType), m_iType(iType)
{
	zDimension dim;
	long iMax = 0;
	int i = 0;
	m_sTypes[i++] = sRemoveAmpersand(SAFUiMirRotOriginal);   dim = Dim(SAFUiMirRotOriginal);   iMax = max(iMax, dim.cx);
	m_sTypes[i++] = sRemoveAmpersand(SAFUiMirrorHorizontal); dim = Dim(SAFUiMirrorHorizontal); iMax = max(iMax, dim.cx);
	m_sTypes[i++] = sRemoveAmpersand(SAFUiMirrorVertical);   dim = Dim(SAFUiMirrorVertical);   iMax = max(iMax, dim.cx);
	m_sTypes[i++] = sRemoveAmpersand(SAFUiTranspose);        dim = Dim(SAFUiTranspose);        iMax = max(iMax, dim.cx);
	m_sTypes[i++] = sRemoveAmpersand(SAFUiMirrorDiagonal);   dim = Dim(SAFUiMirrorDiagonal);   iMax = max(iMax, dim.cx);
	m_sTypes[i++] = sRemoveAmpersand(SAFUiRotate90);         dim = Dim(SAFUiRotate90);         iMax = max(iMax, dim.cx);
	m_sTypes[i++] = sRemoveAmpersand(SAFUiRotate180);        dim = Dim(SAFUiRotate180);        iMax = max(iMax, dim.cx);
	m_sTypes[i++] = sRemoveAmpersand(SAFUiRotate270);        dim = Dim(SAFUiRotate270);        iMax = max(iMax, dim.cx);
	SetFieldWidth(iMax + 20);  // 20 for the arrow button;
}

void FieldMirrorRotateType::create()
{
	FieldOneSelect::create();
	for (int i = 0; i < 8; ++i)
		ose->AddString(m_sTypes[i].scVal());
	ose->SetCurSel(*m_iType);
}

bool FieldMirrorRotateType::fDrawIcon()
{
	return false;
}

void FieldMirrorRotateType::DrawItem(Event* ev)
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
	if ( !fDrawIcon())
	{
		idw.DrawItem(dis, 2, String(s1), fSel, true, false);
		DrawObject(tmp,dis);
	}
	else
		idw.DrawItem(dis, String(s1), fSel, true, false);
}
