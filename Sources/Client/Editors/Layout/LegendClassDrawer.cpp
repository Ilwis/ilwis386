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
// LegendClassDrawer.cpp: implementation of the LegendClassDrawer class.
//
//////////////////////////////////////////////////////////////////////

#include "Client\Headers\formelementspch.h"
#include "Client\Editors\Layout\LayoutItem.h"
#include "Client\Editors\Layout\TextLayoutItem.h"
#include "Client\Editors\Layout\LegendLayoutItem.h"
#include "Client\Editors\Layout\MapLayoutItem.h"
#include "Headers\constant.h"
#include "Client\Base\IlwisDocument.h"
#include "Client\Editors\Layout\LayoutDoc.h"
#include "Headers\Hs\Drwforms.hs"
#include "Headers\Hs\Editor.hs"
#include "Headers\Hs\Layout.hs"
#include "Client\FormElements\fldcolor.h"
#include "Client\FormElements\fldfontn.h"
#include "Engine\Map\Segment\Seg.h"
#include "Engine\Map\Polygon\POL.H"
#include "Engine\Map\Point\PNT.H"
#include "Client\Editors\Utils\SYMBOL.H"
#include "Client\Editors\Utils\Smbext.h"
#include "Client\Editors\Utils\MULTICOL.H"
#include "Client\Editors\Layout\LegendDrawer.h"
#include "Client\Mapwindow\Drawers\BaseDrawer.h"
#include "Client\Mapwindow\Drawers\Drawer.h"
#include "Client\Mapwindow\Drawers\BaseMapDrawer.h"
#include "Client\Editors\Layout\LegendClassDrawer.h"
#include "Engine\Domain\dmclass.h"
#include "Client\FormElements\fldmlcls.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

LegendClassDrawer::LegendClassDrawer(LegendLayoutItem& item)
: LegendItemDrawer(item)
, dc(0), fName(true), fCode(true), fDesc(false), fTransparent(true)
{
  if (0 == bmd())
    return;
  dc = bmd()->dm()->pdc();
  if (0 == dc)
    return;
  // by default fill legend with whole domain in default sequence
  int iSize = dc->iNettoSize();
  aiKeys.Resize(iSize);
  for (int i = 0; i < iSize; ++i)
    aiKeys[i] = dc->iKey(1+i);
}

LegendClassDrawer::~LegendClassDrawer()
{
}

void LegendClassDrawer::ReadElements(ElementContainer& ec, const char* sSection)
{
	LegendItemDrawer::ReadElements(ec, sSection);
  String sText;
	fCode = fName = fDesc = false;
 	ObjectInfo::ReadElement(sSection, "Text", ec, sText);
  if ("Code" == sText) 
    fCode = true; 
  else if ("Name" == sText)
    fName = true;
  else if ("Both" == sText)
    fCode = fName = true;
  else if ("Description" == sText)
    fDesc = true;
	if (!fDesc)
	 	ObjectInfo::ReadElement(sSection, "Descr", ec, fDesc);

	long iNum = 0;
 	ObjectInfo::ReadElement(sSection, "NumberOfEntries", ec, iNum);
	ObjectInfo::ReadElement(sSection, "Transparent", ec, fTransparent);	
  if (iNum < 0)
    iNum = 0;
  aiKeys.Resize(iNum);
  for (int i = 0; i < iNum; ++i)
  {
    String sHead("LegendEntry%d", 1+i);
    long iK = iUNDEF;
		ObjectInfo::ReadElement(sSection, sHead.scVal(), ec, iK);
    aiKeys[i] = iK;
  }
}

void LegendClassDrawer::WriteElements(ElementContainer& ec, const char* sSection)
{
	LegendItemDrawer::WriteElements(ec, sSection);
	String sText;
	if (fCode) 
		if (fName)
			sText = "Both";
		else
			sText = "Code";
	else if (fName)
		sText = "Name";
	else if (fDesc)
		sText = "Description"; 
	ObjectInfo::WriteElement(sSection, "Text", ec, sText);
	ObjectInfo::WriteElement(sSection, "Descr", ec, fDesc);
	int iNum = aiKeys.iSize();
	ObjectInfo::WriteElement(sSection, "NumberOfEntries", ec, iNum);
	ObjectInfo::WriteElement(sSection, "Transparent", ec, fTransparent);
	for(unsigned int i = 0; i < aiKeys.iSize(); ++i)
	{
		long iK = aiKeys[i];
		String sHead("LegendEntry%d", 1+i);
		ObjectInfo::WriteElement(sSection, sHead.scVal(), ec, iK);
	}
	ObjectInfo::WriteElement(sSection, "LegendType", ec, "Class");
}

bool LegendClassDrawer::fValid() const
{
  if (!LegendItemDrawer::fValid())
    return false;
  if (0 == dc) {
    const_cast<DomainClass*>(dc) = bmd()->dm()->pdc();
    if (0 == dc)
      return false;
  }
  return bmd()->dm()->pdc() == dc;
}

int LegendClassDrawer::iItems()
{
  return aiKeys.iSize();
}

String LegendClassDrawer::sItem(int i)
{
  long iRaw = aiKeys[i];
	String sRet;
	if (fName)
		if (fCode)
			sRet = dc->sValueByRaw(iRaw,0);
		else
			sRet = dc->sNameByRaw(iRaw,0);
	else if (fCode)
		sRet = dc->sCodeByRaw(iRaw,0);
	else if (fDesc)
		dc->sDescriptionByRaw(iRaw);
	else
		return sUNDEF;
	if (fDesc)
		return String("%S %S", sRet, dc->sDescriptionByRaw(iRaw));
	else
		return sRet;
}

  class LegendClassDrawerConfForm: public FormWithDest {
  public:
    LegendClassDrawerConfForm(CWnd* w, const String& sTitle, LegendClassDrawer* lcdrw,
             int* iCols)
    : FormWithDest(w, sTitle)
		, lcd(lcdrw)
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
			if (lcd->fAskRowHeight) {
				fBoxHeight = !lcd->fAutoRowHeight;
				CheckBox* cb = new CheckBox(root, SLOUiBoxHeight, &fBoxHeight);
				iBoxHeight = lcd->iRowHeight;
				new FieldInt(cb, "", &iBoxHeight, ValueRange(3, 999), true);
				FieldBlank* fb = new FieldBlank(root, 0);
				fb->Align(cb, AL_UNDER);
			}
			new FieldReal(root, SLOUiBoxWidth, &lcd->rWidthFact, ValueRange(0.1,9.99,0.01));
			new FieldInt(root, SLOUiExtraHeight, &lcd->iRowExtra, ValueRange(0,99), true);
			new CheckBox(root, SLOUiTransparent, &lcd->fTransparent);

      SetMenHelpTopic(htpCnfAnnLegendClass);
      create();
    }
	private:
		int _export exec() 
		{
			FormWithDest::exec();
			if (lcd->fAskRowHeight) {
				lcd->fAutoRowHeight = !fBoxHeight;
				lcd->iRowHeight = iBoxHeight;
			}			
			return 0;
		}
		LegendClassDrawer* lcd;
		bool fBoxHeight;
		int iBoxHeight;
  };

bool LegendClassDrawer::fConfigure()
{
  if (!fValid())
    return false;
  int iCols = iColumns - 1;
  LegendClassDrawerConfForm frm(wndGetActiveView(), sTitle(), this, &iCols);
  if (frm.fOkClicked()) {
    iColumns = iCols + 1;
    Setup();
    return true;
  }
  return false;
}

void LegendClassDrawer::OnDraw(CDC* cdc)
{
	if ( !fTransparent)
	{
		CRect rct = lli.rectPos();
		long iC = GetSysColor(COLOR_WINDOW);
		CBrush brush(iC);
		CBrush *oldBrush = cdc->SelectObject(&brush);
		cdc->Rectangle(&rct);
		cdc->SelectObject(oldBrush);
	}		
	LegendItemDrawer::OnDraw(cdc);		
}
