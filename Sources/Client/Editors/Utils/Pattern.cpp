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
/* $Log: /ILWIS 3.0/UserInterfaceObjects/Pattern.cpp $
 * 
 * 7     3/15/01 11:54 Retsios
 * Solves a terrible GDI memory leak when drawing on a metafile DC
 * 
 * 6     3/13/01 13:24 Retsios
 * (perhaps) solved a memory leak
 * 
 * 5     1-11-00 18:13 Koolhoven
 * added DrawColors paramer to constructor from Representation
 * 
 * 4     1-11-00 15:04 Koolhoven
 * using the transparent (-2) color now will effectively use the
 * NULL_BRUSH
 * 
 * 3     4-06-99 17:38 Koolhoven
 * Removed zDisplay use from Pattern::drawPolygon()
 * this prevents assertion errors during popBrush()
 * 
 * 2     5/27/99 3:39p Martin
 * 
 * 1     5/27/99 8:25a Martin
// Revision 1.2  1998/09/16 17:33:58  Wim
// 22beta2
//
/* 
   Pattern
   by Wim Koolhoven, may 1996
   (c) ILWIS System Development, ITC
	Last change:  WK    8 Jun 98    6:16 pm
*/

#include "Headers\toolspch.h"
#include "Client\Base\ZappToMFC.h"
#include "Engine\Domain\dm.h"
#include "Client\Editors\Utils\Pattern.h"

Pattern::Pattern()
: br(0), fTransparent(true), clrFG(RGB(0,0,0)), clrBG(RGB(255,255,255))
{}

Pattern::Pattern(Color color)
: br(0), fTransparent(true), clrFG(color), clrBG(RGB(255,255,255))
{
  br = new CBrush(color);
}

Pattern::Pattern(const short aPat[8], Color col1, Color col2)
: br(0), clrFG(col1), clrBG(col2), fTransparent(true)
{
	CBitmap bm;
  bm.CreateBitmap(8,8,1,1,aPat);
  br = new CBrush(&bm);
}

Pattern::Pattern(PolDspType typ, Color color)
: br(0), fTransparent(true), clrFG(color), clrBG(RGB(255,255,255))
{
  switch (typ) {
    case pFull:
      br = new CBrush(color);
      break;
    case pDownHatch:
      br = new CBrush(HS_BDIAGONAL, color);
      break;
    case pUpHatch:
      br = new CBrush(HS_FDIAGONAL, color);
      break;
    case pCrossHatch:
      br = new CBrush(HS_CROSS, color);
      break;
    case pDiagCrossHatch:
      br = new CBrush(HS_DIAGCROSS, color);
      break;
    case pHorzHatch:
      br = new CBrush(HS_HORIZONTAL, color);
      break;
    case pVertHatch:
      br = new CBrush(HS_VERTICAL, color);
      break;
  }
}

Pattern::Pattern(const Representation& rpr, long iRaw, DrawColors drc)
: br(0), fTransparent(true), clrFG(0,0,0), clrBG(RGB(255,255,255))
{
  int iType = rpr->iPattern(iRaw);
  Color clr = rpr->clrRaw(iRaw);
  Color clr2 = rpr->clrSecondRaw(iRaw);
  if (0 == iType) {
		if ((long)clr == -2) // transparent
			return;
    br = new CBrush(clr.clrDraw(drc));
    return;
  }
	clr = clr.clrDraw(drc);
	clr2 = clr2.clrDraw(drc);
  if (9999 == iType) {
    clrFG = clr;
    clrBG = clr2;
    short aPat[8];
    rpr->GetPattern(iRaw, aPat);
		CBitmap bm;
    bm.CreateBitmap(8,8,1,1,aPat);
    br = new CBrush(&bm);
    return;
  }
  if (0x10 & iType) {
    fTransparent = false;
    iType -= 0x10;
    clrBG = clr2;
  }
  else if (0x20 & iType) {
    fTransparent = false;
    iType -= 0x20;
    clrBG = clr;
    clr = clr2;
  }
  PolDspType typ = (PolDspType)iType;
  switch (typ) {
    case pDownHatch:
      br = new CBrush(HS_BDIAGONAL, clr);
      break;
    case pUpHatch:
      br = new CBrush(HS_FDIAGONAL, clr);
      break;
    case pCrossHatch:
      br = new CBrush(HS_CROSS, clr);
      break;
    case pDiagCrossHatch:
      br = new CBrush(HS_DIAGCROSS, clr);
      break;
    case pHorzHatch:
      br = new CBrush(HS_HORIZONTAL, clr);
      break;
    case pVertHatch:
      br = new CBrush(HS_VERTICAL, clr);
      break;
  }
}

Pattern::~Pattern()
{
  if (br)
	{
		br->DeleteObject();
    delete br;
	}
}

void Pattern::drawPolygon(CDC *dc, vector<Array<zPoint>>& p)
{
	LOGBRUSH lb;
	br->GetLogBrush(&lb);
  Color clrText = dc->SetTextColor(clrFG);
  Color clrBack = dc->SetBkColor(clrBG);
  if (fTransparent)
		dc->SetBkMode(TRANSPARENT);
  else
		dc->SetBkMode(OPAQUE);
	CGdiObject* brushOld;
  if (0 == br)
		brushOld = dc->SelectStockObject(NULL_BRUSH);
	else
		brushOld = dc->SelectObject(br);

	dc->Polygon(p[0].buf(), p[0].size());
	// dc->SelectObject(brushOld);
	// (after lots of research) it was found that above line wasn't working well when
	// the dc is a CMetafileDC. The line below seems to work well in all cases (our
	// goal is to "release" our brush from the dc so that we can delete it.
	// More info: SDK, Q81497, "Pitfalls to Avoid with Metafiles"
	for(int j=1; j < p.size(); ++j) {
		dc->SelectStockObject(WHITE_BRUSH);
		dc->Polygon(p[j].buf(), p[j].size());
	}
	dc->SelectStockObject(NULL_BRUSH);
	dc->SetTextColor(clrText);
	dc->SetBkColor(clrBack);
}

void Pattern::drawRectangle(HDC hDC, CRect* rc)
{
  COLORREF clrText = SetTextColor(hDC, clrFG);
  COLORREF clrBack = SetBkColor(hDC, clrBG);
  if (fTransparent)
    SetBkMode(hDC, TRANSPARENT);
  else
    SetBkMode(hDC, OPAQUE);
	HANDLE hBrush;
  if (0 == br)
		hBrush = SelectObject(hDC, GetStockObject(NULL_BRUSH));
	else
		hBrush = SelectObject(hDC, *br);
  Rectangle(hDC, rc->left, rc->top, rc->right, rc->bottom);
  SelectObject(hDC, hBrush);
  SetTextColor(hDC, clrText);
  SetBkColor(hDC, clrBack);
}


