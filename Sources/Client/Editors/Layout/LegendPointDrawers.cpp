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
#include "Client\FormElements\fldmlcls.h"
#include "Client\Editors\Layout\LayoutItem.h"
#include "Client\Editors\Layout\TextLayoutItem.h"
#include "Client\Editors\Layout\LegendLayoutItem.h"
#include "Client\Editors\Layout\MapLayoutItem.h"
#include "Engine\Domain\dmclass.h"
#include "Headers\constant.h"
#include "Client\Base\IlwisDocument.h"
#include "Client\Editors\Layout\LayoutDoc.h"
#include "Headers\Hs\Drwforms.hs"
#include "Headers\Hs\Editor.hs"
#include "Client\FormElements\fldcolor.h"
#include "Client\FormElements\fldfontn.h"
#include "Engine\Map\Point\PNT.H"
#include "Client\Editors\Utils\SYMBOL.H"
#include "Client\Editors\Utils\Smbext.h"
#include "Client\Editors\Utils\Pattern.h"
#include "Client\Editors\Utils\MULTICOL.H"
#include "Client\Editors\Layout\LegendDrawer.h"
#include "Client\Mapwindow\Drawers\BaseDrawer.h"
#include "Client\Mapwindow\Drawers\Drawer.h"
#include "Client\Mapwindow\Drawers\BaseMapDrawer.h"
#include "Client\Mapwindow\Drawers\PointMapDrawer.h"
#include "Client\Editors\Layout\LegendClassDrawer.h"
#include "Client\Editors\Layout\LegendValueDrawer.h"
#include "Client\MainWindow\Catalog\CatalogDocument.h"
#include "Engine\Map\Mapview.h"
#include "Client\Mapwindow\MapCompositionDoc.h"
#include "Client\Editors\Layout\LegendPointDrawers.h"
#include "Headers\Hs\Layout.hs"


LegendPointArrowDrawer::LegendPointArrowDrawer(LegendLayoutItem& item)
  : LegendClassDrawer(item)
{
// no Class map but Class Column
	PointMapDrawer* pmd = dynamic_cast<PointMapDrawer*>(bmd());
	Column col = pmd->colClr;
  if (!col.fValid())
    return;
  dc = col->dm()->pdc();
  if (0 == dc)
    return;
  int iSize = dc->iNettoSize();
  aiKeys.Resize(iSize);
  for (int i = 0; i < iSize; ++i)
    aiKeys[i] = dc->iKey(1+i);
}

LegendPointArrowDrawer::~LegendPointArrowDrawer()
{}

bool LegendPointArrowDrawer::fValid() const
{
  if (!LegendItemDrawer::fValid()) // not class, because other dc
    return false;
  PointMapDrawer* pdr = dynamic_cast<PointMapDrawer*>(bmd());
  if (0 == pdr)
    return false;
  if (0 == dc) {
    if (0 == pdr)
      return false;
    Column col = pdr->colClr;
    if (!col.fValid())
      return false;
    const_cast<LegendPointArrowDrawer*>(this)->dc = col->dm()->pdc();
  }
  return drmARROW == pdr->drmMethod() &&
          pdr->colClr.fValid() && 0 != dc;
}

    // copied from LegendClassDrawer.cpp
    // only htp number is different
  class LegendPointArrowDrawerConfForm: public FormWithDest {
  public:
    LegendPointArrowDrawerConfForm(CWnd* w, const String& sTitle, LegendClassDrawer* lcd,
             int* iCols)
    : FormWithDest(w, sTitle)
    {
      RadioGroup* rgCols = new RadioGroup(root, SDCUiLegendCols, iCols, true);
      rgCols->SetIndependentPos();
      new RadioButton(rgCols, "&1");
      new RadioButton(rgCols, "&2");
      new RadioButton(rgCols, "&3");
      new RadioButton(rgCols, "&4");

			FieldGroup* fgText = new FieldGroup(root);
			CheckBox* cb1 = new CheckBox(fgText, SDCUiCode, &lcd->fCode);
			CheckBox* cb2 = new CheckBox(fgText, SDCUiName, &lcd->fName);
			cb2->Align(cb1, AL_UNDER);
			CheckBox* cb3 = new CheckBox(fgText, SDCUiDescription, &lcd->fDesc);
			cb3->Align(cb2, AL_UNDER);

      FormEntry* fe = new FieldMultiClass(root, "", lcd->dc, lcd->aiKeys);
			fe->Align(fgText, AL_UNDER);

			FieldBlank* fb = new FieldBlank(root, 0);
			fb->Align(fe, AL_UNDER);

			lcd->lli.flfAskLogFont(root);
			new FieldReal(root, SLOUiBoxWidth, &lcd->rWidthFact, ValueRange(0.1,9.99,0.01));

      SetMenHelpTopic(htpCnfAnnLegendPointArrow);
      create();
    }
  };

bool LegendPointArrowDrawer::fConfigure() 
{
  if (!fValid())
    return false;
  int iCols = iColumns - 1;
  LegendPointArrowDrawerConfForm frm(wndGetActiveView(), sTitle(), this, &iCols);
  if (frm.fOkClicked()) {
    iColumns = iCols + 1;
    Setup();
    return true;
  }
  return false;
}

void LegendPointArrowDrawer::WriteElements(ElementContainer& ec, const char* sSection)
{
  LegendClassDrawer::WriteElements(ec, sSection);
  ObjectInfo::WriteElement(sSection, "LegendType", ec, "PointArrow");
}

void LegendPointArrowDrawer::DrawItem(CDC* cdc, CPoint p, int i)
{ // copy from code in pntdrwr.c
  long iRaw = aiKeys[i];
  Color clr = dc->rpr()->clrRaw(iRaw);
//    double rLength = 10;
  double rPixLength = iRowHeight * 3 / 2; // just like lines of segments
    // rLength * dsp->pixPerInchY()/rMmPerInch; // mm
  double rTop = 15; // 5 pt
	CPen pen(PS_SOLID, 1, clr);
  CPen *oldPen = cdc->SelectObject(&pen);;
  p.y += iRowHeight / 2;
  cdc->MoveTo(p);
  double rRotRad = M_PI_2;
  p.x += rounding(rPixLength * sin(rRotRad));
  p.y -= rounding(rPixLength * cos(rRotRad));
  cdc->LineTo(p);
  zPoint pTop = p;

  double rAngle = 45;
  double rAngleRad = rAngle * M_PI / 180.0;
  p.x -= rounding(rTop * sin(rRotRad+rAngleRad));
  p.y += rounding(rTop * cos(rRotRad+rAngleRad));
  cdc->LineTo(p);
  p = pTop;
  cdc->MoveTo(p);
  p.x -= rounding(rTop * sin(rRotRad-rAngleRad));
  p.y += rounding(rTop * cos(rRotRad-rAngleRad));
  cdc->LineTo(p);
	cdc->SelectObject(oldPen);
}


bool LegendPointGraphDrawer::fConfigure() 
{
  class ConfForm: public FormWithDest {
  public:
		ConfForm(CWnd* w, const String& sTitle, 
						 int* iCols, int* iText, LegendLayoutItem *li)
		: FormWithDest(w, sTitle)
    {
      RadioGroup* rgCols = new RadioGroup(root, SDCUiLegendCols, iCols, true);
      rgCols->SetIndependentPos();
      new RadioButton(rgCols, "&1");
      new RadioButton(rgCols, "&2");
      new RadioButton(rgCols, "&3");
      new RadioButton(rgCols, "&4");
      RadioGroup* rgText = new RadioGroup(root, SDCUiText, iText);
      new RadioButton(rgText, SDCUiName);
      new RadioButton(rgText, SDCUiDescription);
			li->flfAskLogFont(root);
      SetMenHelpTopic(htpCnfAnnLegendPointGraph);
      create();
    }
  };
  if (!fValid())
    return false;
  int iCols = iColumns - 1;
  int iText = (int)eText;
  ConfForm frm(wndGetActiveView(), sTitle(), &iCols, &iText, &lli);
  if (frm.fOkClicked()) {
    iColumns = iCols + 1;
    eText = (eTextType) iText;
    Setup();
    return true;
  }
  return false;
}

bool LegendPointValueDrawer::fConfigure() 
{
  class ConfForm: public FormWithDest {
  public:
		ConfForm(CWnd* w, const String& sTitle, 
             bool* fUpward, int* iNrSymbols, bool *fUnits, LegendLayoutItem *li)
		: FormWithDest(w, sTitle)
    {
      CheckBox* cb = new CheckBox(root, SDCUiUpward, fUpward);
      RadioGroup* rgSyms = new RadioGroup(root, SDCUiNrSymbols, iNrSymbols, true);
      rgSyms->Align(cb, AL_UNDER);
      rgSyms->SetIndependentPos();
      new RadioButton(rgSyms, "&2");
      new RadioButton(rgSyms, "&3");
      new RadioButton(rgSyms, "&4");
      new RadioButton(rgSyms, "&5");
      new RadioButton(rgSyms, "&6");
      new RadioButton(rgSyms, "&7");
			li->flfAskLogFont(root)	;	
			new CheckBox(root, SLOUiUseUnits, fUnits);
      SetMenHelpTopic(htpCnfAnnLegendPointValue);
      create();
    }
  };
  if (!fValid())
    return false;
  int iSym = iNrSymbols - 2;
  ConfForm frm(wndGetActiveView(), sTitle(), &fUpward, &iSym, &fUnit, &lli);
  if (frm.fOkClicked()) {
    iNrSymbols = iSym + 2;
    Setup();
    return true;
  }
  return false;
}

