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
// LegendValueDrawer.cpp: implementation of the LegendValueDrawer class.
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
#include "Client\Editors\Layout\LegendValueDrawer.h"
#include "Engine\Representation\RPRVALUE.H"
#include "Headers\Hs\Layout.hs"
#include "Engine\Domain\Dmvalue.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

LegendValueDrawer::LegendValueDrawer(LegendLayoutItem& item)
: LegendBaseDrawer(item)
{
  fUpward = true;
  rRatio = 5;
	fSetMinMax = false;
  dv = bmd()->dm()->pdv();
}

LegendValueDrawer::~LegendValueDrawer()
{
}

void LegendValueDrawer::ReadElements(ElementContainer& ec, const char* sSection)
{
	LegendBaseDrawer::WriteElements(ec, sSection);
  ObjectInfo::ReadElement(sSection, "Upward", ec, fUpward);
  ObjectInfo::ReadElement(sSection, "Ratio", ec, rRatio);
  if (rRatio < 1)
    rRatio = 5;
  ObjectInfo::ReadElement(sSection, "SetMinMax", ec, fSetMinMax);
  ObjectInfo::ReadElement(sSection, "MinMax", ec, rrMinMax);
}

void LegendValueDrawer::WriteElements(ElementContainer& ec, const char* sSection)
{
	LegendBaseDrawer::WriteElements(ec, sSection);
	ObjectInfo::WriteElement(sSection, "LegendType", ec, "Value");
  ObjectInfo::WriteElement(sSection, "Upward", ec, fUpward);
  ObjectInfo::WriteElement(sSection, "Ratio", ec, rRatio);
  ObjectInfo::WriteElement(sSection, "SetMinMax", ec, fSetMinMax);
  ObjectInfo::WriteElement(sSection, "MinMax", ec, rrMinMax);
}

void LegendValueDrawer::OnDraw(CDC* cdc)
{
	if (!fValid())
		return;
	Drawer* dr = bmd();

	int iFontSize = lli.iFontSize();
	MinMax mm = lli.mmPosition();
	int iHeight = mm.height();
  iHeight -= iFontSize;
	int iWidth = (int)(iHeight / rRatio);
  CPoint pStart(mm.MinCol(), mm.MinRow());
  pStart.y += iFontSize / 2;

  int y = 0;
  int iNr = iLimits();
  double r0, r1 = rValue(0);
  for (int i = 1; i < iNr; ++i) {
    r0 = r1;
    r1 = rValue(i);
    double rd = r1 - r0;
    int iSteps = i * iHeight / (iNr-1) - y;
    for (int j = 0; j < iSteps; ++j, ++y) {
      double r = r0 + j * rd / iSteps;
      Color clr = dr->clrVal(r);
			CPen pen(PS_SOLID, 1, clr);
			CBrush br(clr);
			CPen* penOld = cdc->SelectObject(&pen);
			CBrush* brOld = cdc->SelectObject(&br);
      CPoint p = pStart;
      p.y += y;
			CRect rect(p, p);
			rect.right += iWidth;
			rect.bottom += 1;
			cdc->Rectangle(&rect);
			cdc->SelectObject(penOld);
			cdc->SelectObject(brOld);
    }
  }

  zRect rect;
  rect.topLeft() = pStart;
  rect.right() = rect.left() + iWidth;
  rect.bottom() = rect.top() + iHeight;
	CGdiObject* brOld = cdc->SelectStockObject(HOLLOW_BRUSH);
  cdc->Rectangle(&rect);
	cdc->SelectObject(brOld);

  for (int i = 0; i < iNr; ++i) {
    y = i * iHeight / (iNr - 1);
    double r = rValue(i);
    String sText = sValue(r);
    zPoint p = pStart;
    p.x += iWidth - 1;
    p.y += y;
    cdc->MoveTo(p);
    p.x += iFontSize / 4;
    cdc->LineTo(p);
    p = pStart;
    p.x += iWidth + iFontSize / 2;
    p.y += y - iFontSize / 2;
		CString str = sText.scVal();
    cdc->TextOut(p.x, p.y, str);
  }
}

bool LegendValueDrawer::fValid() const
{
  if (!LegendBaseDrawer::fValid())
    return false;
  return 0 != bmd()->dm()->pdv();
}

  class ConfForm: public FormWithDest {
  public:
    ConfForm(CWnd* w, const String& sTitle, LegendValueDrawer* lvd)
    : FormWithDest(w, sTitle)
    {
			CheckBox* cbMinMax = new CheckBox(root, SLOUiMinMax, &lvd->fSetMinMax);
			DomainValueRangeStruct dvrs = lvd->lli.bmd()->dvrs();
			double rStep = dvrs.rStep();
			RangeReal rr = dvrs.rrMinMax();
			double rW = rr.rWidth();
			rr.rLo() -= rW;
			rr.rHi() += rW;
			new FieldRangeReal(cbMinMax, "", &lvd->rrMinMax, ValueRange(rr,rStep));

      CheckBox* cb = new CheckBox(root, SDCUiUpward, &lvd->fUpward);
			cb->Align(cbMinMax, AL_UNDER);
      FieldReal* fr = new FieldReal(root, SDCUiRatio, &lvd->rRatio, ValueRange(1,20,0.1));
      fr->Align(cb, AL_UNDER);

			lvd->lli.flfAskLogFont(root);

      SetMenHelpTopic(htpCnfAnnLegendValue);
      create();
    }
  };

bool LegendValueDrawer::fConfigure()
{
  if (!fValid())
    return false;
	if (!fSetMinMax) {
/*
	  RepresentationValue* rpv = bmd()->rpr()->prv();
		if (rpv) {
			rrMinMax.rLo() = rpv->rValue(0);
			rrMinMax.rHi() = rpv->rValue(rpv->iLimits()-1);
		}
		else 
*/
			rrMinMax = bmd()->rrStretchRange();
  }
  ConfForm frm(wndGetActiveView(), sTitle(), this);
  if (frm.fOkClicked()) {
    Setup();
    return true;
  }
  return false;
}

void LegendValueDrawer::Setup()
{
	if (!fValid())
		return;

	CWindowDC cdc(CWnd::GetDesktopWindow());
	TextLayoutItem::InitFont init(&lli, &cdc);

  int iNr = iLimits();
  int iMaxWidth = 0;
  for (int i = 0; i < iNr; ++i) {
    double rVal = rValue(i);
    String sText = sValue(rVal);
		CString str = sText.scVal();
		CSize sz = cdc.GetTextExtent(str);
    int iW = sz.cx;
    if (iW > iMaxWidth)
      iMaxWidth = iW;
  }
	int iFontSize = lli.iFontSize();
  int iTotHeight = (int)((iFontSize + 1) * 2 * rRatio);
  int iHeight = iTotHeight - iFontSize;
  int iWidth = (int)(iHeight / rRatio);
  int iTotWidth = iWidth + iMaxWidth + iFontSize / 2;

	MinMax mm = lli.mmPosition();
	mm.MaxCol() = mm.MinCol() + iTotWidth;
	mm.MaxRow() = mm.MinRow() + iTotHeight;
	lli.SetPosition(mm);
}


int LegendValueDrawer::iLimits() const
{
  if (!fValid())
    return 2;
  int iLim = 2;
	if (fSetMinMax) 
	{
		double rStep = bmd()->dvrs().rStep();
		if (rStep > 1e-6) {
			int iSteps = (int)(1.0 + rounding(rrMinMax.rWidth() / rStep));
			if (iSteps >= 2 && iSteps < 6)
				return iSteps;
		}
	}
	else {
		RepresentationValue* rpv = bmd()->rpr()->prv();
		if (rpv)
			iLim = rpv->iLimits();
	}

  int iMult = (int)(rRatio / (iLim - 1));
  if (iMult == 0)
    iMult = 1;
  iLim -= 1;
  iLim *= iMult;
  iLim += 1;
  return iLim;
}

double LegendValueDrawer::rValue(int iLimit) const   // 0 <= iLimit < iLimits
{
  if (fUpward)
    iLimit = iLimits() - (1 + iLimit);
  int iRawLim = 2;
  RepresentationValue* rpv = bmd()->rpr()->prv();
  if (rpv && !fSetMinMax) 
    iRawLim = rpv->iLimits();
  int iMult = (int)(rRatio / (iRawLim - 1));
  if (iMult == 0)
    iMult = 1;
  int iLim = iLimit / iMult;
  if (iLim >= iRawLim - 1)
    iLim = iRawLim - 2;
  double rFrac = double(iLimit) / iMult - iLim;
  if (rFrac < 0)
    rFrac = 0;
  if (rFrac > 1)
    rFrac = 1;
  double rLo, rHi;
	if (fSetMinMax) {
		rLo = rrMinMax.rLo();
		rHi = rrMinMax.rHi();
		double rStep = bmd()->dvrs().rStep();
		if (rStep > 1e-6) {
			int iSteps = (int)(1 + rounding(rrMinMax.rWidth() / rStep));
			if (iSteps < 6)
				rFrac = double(iLimit) / (iLimits()-1);
		}
	}
  else if (rpv) {
    rLo = rpv->rValue(iLim);
    rHi = rpv->rValue(1+iLim);
  }
  else {
    rLo = bmd()->rrStretchRange().rLo();
    rHi = bmd()->rrStretchRange().rHi();
  }
  double rVal = rLo + rFrac * (rHi - rLo);
  return rVal;
}

String LegendValueDrawer::sValue(double rValue) const
{
  String sVal = bmd()->dvrs().sValue(rValue,0);
  if (dv->fUnit())
    sVal = String("%S %S", sVal, dv->sUnit());
  return sVal;    
}

