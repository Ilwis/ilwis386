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
// LegendBlockDrawer.cpp: implementation of the LegendBlockDrawer class.
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
#include "Engine\Map\Point\PNT.H"
#include "Client\Editors\Utils\SYMBOL.H"
#include "Client\Editors\Utils\Smbext.h"
#include "Client\Editors\Utils\MULTICOL.H"
#include "Client\Editors\Layout\LegendDrawer.h"
#include "Client\Mapwindow\Drawers\BaseDrawer.h"
#include "Client\Mapwindow\Drawers\Drawer.h"
#include "Client\Mapwindow\Drawers\BaseMapDrawer.h"
#include "Client\Mapwindow\Drawers\PointMapDrawer.h"
#include "Client\Editors\Layout\LegendBlockDrawer.h"

  class ConfForm: public FormWithDest {
  public:
    ConfForm(CWnd* w, const String& sTitle,	LegendBlockDrawer* lbd,
             int* iNrVal)
    : FormWithDest(w, sTitle)
    {
      RadioGroup* rgVals = new RadioGroup(root, SDCUiNrValues, iNrVal, true);
      rgVals->SetIndependentPos();
      new RadioButton(rgVals, "&2");
      new RadioButton(rgVals, "&3");
      new RadioButton(rgVals, "&4");
      new RadioButton(rgVals, "&5");
      new RadioButton(rgVals, "&6");
      new RadioButton(rgVals, "&7");
			lbd->lli.flfAskLogFont(root);
      SetMenHelpTopic(htpCnfAnnLegendPointBlock);
      create();
    }
  };


bool LegendBlockDrawer::fConfigure()
{
  if (!fValid())
    return false;
  int iVals = iNrValues - 2;
  ConfForm frm(wndGetActiveView(), sTitle(), this, &iVals);
  if (frm.fOkClicked()) {
    iNrValues = iVals + 2;
    Setup();
    return true;
  }
  return false;
}

int LegendBlockDrawer::iMaxWidth() 
{
	CWindowDC cdc(CWnd::GetDesktopWindow());
	TextLayoutItem::InitFont init(&lli, &cdc);

  int iMaxW = 0;
  for (int i = 0; i < iNrValues; ++i) 
	{
    double rVal = rValue(i);
    String sText = sValue(rVal);
		CString str = sText.scVal();
		CSize sz = cdc.GetTextExtent(str);
		if (sz.cx > iMaxW)
			iMaxW = sz.cx;
  }
  return iMaxW;
}


bool LegendPointCircleDrawer::fValid()
{
  if (!LegendBlockDrawer::fValid())
    return false;
  PointMapDrawer* pdr = dynamic_cast<PointMapDrawer*>(bmd());
  if (pdr)
    return drmGRAPH == pdr->drmMethod() &&
           PointMapDrawer::stPIECHART == pdr->eST &&
           pdr->fStretch &&
           pdr->colVal.fValid();
  else
    return false;
}

void LegendPointCircleDrawer::Setup()
{
  if (!fValid()) 
		return;

  PointMapDrawer* pdr = dynamic_cast<PointMapDrawer*>(bmd());
	CWindowDC cdc(CWnd::GetDesktopWindow());
  int iMaxSize = 3 * pdr->iSize;
	int iFontSize = lli.iFontSize();

  int iTotWidth = 2 * iMaxSize + iMaxWidth() + iFontSize / 2;
  int iTotHeight = 2 * iMaxSize + iFontSize;
	MinMax mm = lli.mmPosition();
	mm.MaxCol() = mm.MinCol() + iTotWidth;
	mm.MaxRow() = mm.MinRow() + iTotHeight;
	lli.SetPosition(mm);
}

String LegendPointCircleDrawer::sValue(double rVal)
{
  PointMapDrawer* pdr = dynamic_cast<PointMapDrawer*>(bmd());
  return pdr->colVal->dvrs().sValue(rVal,0);
}

void LegendPointCircleDrawer::OnDraw(CDC* cdc)
{
  if (!fValid()) 
		return;

  PointMapDrawer* pdr = dynamic_cast<PointMapDrawer*>(bmd());
  int iMaxSize = 3 * pdr->iSize;
	int iFontSize = lli.iFontSize();

	CRect rct = lli.rectPos();
	CPoint pt = rct.TopLeft();
  pt.x += iMaxSize;
  pt.y += iFontSize / 2 + 2 * iMaxSize;
  const CPoint pStart = pt;

	RangeReal rr = pdr->rrStretchRange();
  double rMin = rr.rLo();
  double rMax = rr.rHi();

	CGdiObject* brOld = cdc->SelectStockObject(HOLLOW_BRUSH);
  for (int i = 0; i < iNrValues; ++i)
  {
    double rVal = rValue(i);
    int iSiz = (int)(3.0 * pdr->rStretchSize(rVal, rMin, rMax));
    CPoint p = pStart;
		CRect rect;
		rect.bottom = pStart.y;
		rect.top = pStart.y - 2 * iSiz;
		rect.left = pStart.x - iSiz; 
		rect.right = pStart.x + iSiz; 
		cdc->Ellipse(&rect);
		p.y = rect.top;
    cdc->MoveTo(p);
    p.x += iMaxSize;
    cdc->LineTo(p);
    p.x += iFontSize / 2;
    p.y -= iFontSize / 2;
    String sText = sValue(rVal);
		CString str = sText.scVal();
		cdc->TextOut(p.x, p.y, str);
  }
	cdc->SelectObject(brOld);
}


bool LegendPointBarDrawer::fValid()
{
  if (!LegendBlockDrawer::fValid())
    return false;
  PointMapDrawer* pdr = dynamic_cast<PointMapDrawer*>(bmd());
  if (pdr)
    return drmGRAPH == pdr->drmMethod() &&
           (PointMapDrawer::stBARGRAPH == pdr->eST ||
            PointMapDrawer::stCOMPBAR == pdr->eST);
  else
    return false;
}

String LegendPointBarDrawer::sValue(double rVal) {
  PointMapDrawer* pdr = dynamic_cast<PointMapDrawer*>(bmd());
  return pdr->cacarr[0].col->dvrs().sValue(rVal,0);
}

void LegendPointBarDrawer::Setup()
{
  if (!fValid()) 
		return;
  PointMapDrawer* pdr = dynamic_cast<PointMapDrawer*>(bmd());
	CWindowDC cdc(CWnd::GetDesktopWindow());
  int iMaxSize = 3 * pdr->iSize;
	int iFontSize = lli.iFontSize();

  int iPixWidth = 3 * pdr->iWidth;
  int iTotWidth = iPixWidth + iMaxWidth() + iFontSize / 2;
  int iTotHeight = iMaxSize + iFontSize;
	MinMax mm = lli.mmPosition();
	mm.MaxCol() = mm.MinCol() + iTotWidth;
	mm.MaxRow() = mm.MinRow() + iTotHeight;
	lli.SetPosition(mm);
}

void LegendPointBarDrawer::OnDraw(CDC* cdc)
{
  if (!fValid()) 
		return;
  PointMapDrawer* pdr = dynamic_cast<PointMapDrawer*>(bmd());
  int iMaxSize = 3 * pdr->iSize;
	int iFontSize = lli.iFontSize();

	CRect rct = lli.rectPos();
	CPoint pt = rct.TopLeft();
  pt.y += iFontSize / 2 + iMaxSize;
  const CPoint pStart = pt;

  int iPixWidth = 3 * pdr->iWidth;
  double rMin = pdr->rrStretch.rLo();
  double rMax = pdr->rrStretch.rHi();
  if (rMax <= rMin)
    rMax = rMin + 1;
	CGdiObject* brOld = cdc->SelectStockObject(HOLLOW_BRUSH);
  for (int i = 0; i < iNrValues; ++i)
  {
    double rVal = rValue(i);
    double rSiz = rVal;
    rSiz -= rMin;
    rSiz /= (rMax - rMin);
    if (rSiz < 0)
      rSiz = 0;
    else if (rSiz > 1)
      rSiz = 1;
    rSiz *= iMaxSize;
    int iSize = rounding(rSiz) + 1;
    zPoint p1 = pStart;
    p1.y -= iSize;
    zPoint p2 = pStart;
    p2.x += iPixWidth;
    cdc->Rectangle(p1.x, p1.y, p2.x, p2.y);
    p1.x += iPixWidth + iFontSize / 2;
    p1.y -= iFontSize / 2;
    String sText = sValue(rVal);
		CString str = sText.scVal();
		cdc->TextOut(p1.x, p1.y, str);
  }
	cdc->SelectObject(brOld);
  return;
}


