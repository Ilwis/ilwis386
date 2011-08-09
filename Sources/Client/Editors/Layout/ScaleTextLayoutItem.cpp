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
// ScaleTextLayoutItem.cpp: implementation of the ScaleTextLayoutItem class.
//
//////////////////////////////////////////////////////////////////////

#include "Client\Headers\formelementspch.h"
#include "Client\Editors\Layout\LayoutItem.h"
#include "Client\Editors\Layout\TextLayoutItem.h"
#include "Client\Editors\Layout\ScaleTextLayoutItem.h"
#include "Client\Editors\Layout\MapLayoutItem.h"
#include "Headers\constant.h"
#include "Client\Base\IlwisDocument.h"
#include "Client\Editors\Layout\LayoutDoc.h"
#include "Headers\Hs\Drwforms.hs"
#include "Headers\Hs\Editor.hs"
#include "Client\FormElements\fldcolor.h"
#include "Client\FormElements\fldfontn.h"
#include "Headers\Hs\Layout.hs"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

BEGIN_MESSAGE_MAP(ScaleTextLayoutItem, TextLayoutItem)
	//{{AFX_MSG_MAP(ScaleTextLayoutItem)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


ScaleTextLayoutItem::ScaleTextLayoutItem(LayoutDoc* ld, MapLayoutItem* mapli)
	: TextLayoutItem(ld)
	, mli(mapli)
{
	MinMax mm = mli->mmPosition();
	mm.MaxRow() = mm.MinRow();
	mm.MinRow() -= 100; // 1 cm above map.
	mm.MaxCol() = mm.MinCol() + 300;
	SetPosition(mm,-1);
}

ScaleTextLayoutItem::ScaleTextLayoutItem(LayoutDoc* ld)
	: TextLayoutItem(ld), mli(0)
{
}

ScaleTextLayoutItem::~ScaleTextLayoutItem()
{
}

void ScaleTextLayoutItem::OnDraw(CDC* cdc)
{
	double rScale = mli->rScale();
	sText = String("1 : %.f", rScale);
	DrawSingleLine(cdc);
}

bool ScaleTextLayoutItem::fOnChangedItemSize(LayoutItem* li)
{
	return li == this || li== mli;
}

class ScaleTextLayoutItemForm : public FormWithDest
{
public:
  ScaleTextLayoutItemForm(CWnd* wnd, ScaleTextLayoutItem* tli) 
		: FormWithDest(wnd, TR("Edit Scale Text"))
  {
      FieldGroup *fg = new FieldGroup(root);
      FieldColor *fc = new FieldColor(fg, TR("&Color"), &tli->clr);
	    new FieldLogFont(fg, &tli->lf);
      new CheckBox(fg, TR("&Transparent"), &tli->fTransparent);

      SetMenHelpTopic("ilwismen\\layout_editor_insert_edit_scale_text.htm");
      create();
  }
};



bool ScaleTextLayoutItem::fConfigure()
{
	ScaleTextLayoutItemForm frm(ld->wndGetActiveView(), this);
	if (sText == " ")
		return false;
	return frm.fOkClicked();
}

String ScaleTextLayoutItem::sType() const
{
	return "ScaleText";
}

void ScaleTextLayoutItem::ReadElements(ElementContainer& en, const char* sSection)
{
	TextLayoutItem::ReadElements(en, sSection);
	int iNr;
	ObjectInfo::ReadElement(sSection, "Map View", en, iNr);
	mli = dynamic_cast<MapLayoutItem*>(ld->liFindID(iNr));
}

void ScaleTextLayoutItem::WriteElements(ElementContainer& en, const char* sSection)
{
	TextLayoutItem::WriteElements(en, sSection);
	ObjectInfo::WriteElement(sSection, "Map View", en, mli->iID());
}

String ScaleTextLayoutItem::sName() const
{
	if (0 == mli)
		return TR("Scale Text");
	else
		return String(TR("Scale Text of %S").c_str(), mli->sName());
}

bool ScaleTextLayoutItem::fDependsOn(LayoutItem* li)
{
	return li == mli;
}


double ScaleTextLayoutItem::rScale() const   // 1:....
{
	return mli->rScale();
}

void ScaleTextLayoutItem::SetScale(double rScale)
{
	mli->SetScale(rScale);
}
