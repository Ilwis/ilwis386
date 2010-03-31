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
// BoxLayoutItem.cpp: implementation of the BoxLayoutItem class.
//
//////////////////////////////////////////////////////////////////////

#include "Client\Headers\formelementspch.h"
#include "Client\Editors\Layout\LayoutItem.h"
#include "Client\Editors\Layout\BoxLayoutItem.h"
#include "Client\Editors\Layout\LayoutDoc.h"
#include "Headers\Hs\Layout.hs"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

BoxLayoutItem::BoxLayoutItem(LayoutDoc* ld)
	: LayoutItem(ld)
	, lnBox(Color(0,0,0),0.5)
{											
	fFill = false;
	clrFill = Color(255,255,255);
}

BoxLayoutItem::~BoxLayoutItem()
{
}

void BoxLayoutItem::ReadElements(ElementContainer& en, const char* sSection)
{
	LayoutItem::ReadElements(en, sSection);
	lnBox.Read(sSection, "Box Line", en);
	ObjectInfo::ReadElement(sSection, "Fill", en, fFill);
	ObjectInfo::ReadElement(sSection, "Fill Color", en, clrFill);
}

void BoxLayoutItem::WriteElements(ElementContainer& en, const char* sSection)
{
	LayoutItem::WriteElements(en, sSection);
	lnBox.Write(sSection, "Box Line", en);
	ObjectInfo::WriteElement(sSection, "Fill", en, fFill);
	ObjectInfo::WriteElement(sSection, "Fill Color", en, clrFill);
}

String BoxLayoutItem::sType() const
{
	return "Box";
}

String BoxLayoutItem::sName() const
{
	return SLONameBox;
}

void BoxLayoutItem::OnDraw(CDC* cdc)
{
	CRect rect = rectPos();
	if (fFill) {
		CRect rct = rect;
		CGdiObject* penOld = cdc->SelectStockObject(NULL_PEN);
		CBrush br(clrFill);
		CBrush* brOld = cdc->SelectObject(&br);
		rct.right += 1;
		rct.bottom += 1;
		cdc->Rectangle(&rct);
		cdc->SelectObject(brOld);
		cdc->SelectObject(penOld);
	}
	lnBox.drawRectangle(cdc, rect);
}

class BoxLayoutItemForm : public FormWithDest
{
public:
  BoxLayoutItemForm(CWnd* wnd, BoxLayoutItem* li) 
		: FormWithDest(wnd, SLOTitleBox)
  {
		new FieldLine(root, &li->lnBox, true);
		CheckBox* cb = new CheckBox(root, SLOUiFillColor, &li->fFill);
		new FieldColor(cb, "", &li->clrFill);
    SetMenHelpTopic(htpCnfAnnBox);
		create();
	}
};

bool BoxLayoutItem::fConfigure()
{
	BoxLayoutItemForm frm(ld->wndGetActiveView(), this);
	return frm.fOkClicked();
}
