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
/* Symbol
// by Wim Koolhoven
// (c) Ilwis, ITC
	Last change:  WK   26 Mar 98    5:33 pm
*/
#include "Client\Headers\formelementspch.h"
#include "Client\Editors\Utils\SYMBOL.H"
#include <math.h>
#include "Engine\Representation\Rpr.h"
#include "Headers\Hs\Drwforms.hs"

#undef Polygon

Symbol::Symbol()
{
  iSize = 3;
  iWidth = 1;
  col = -1;
  fillCol = -2;
  smb = smbCircle;
  hIcon = 0;
}

Symbol::Symbol(const Representation& rpr, long iRaw)
{
  iSize = rpr->iSymbolSize(iRaw);
  iWidth = rpr->iSymbolWidth(iRaw);
  col = rpr->clrSymbol(iRaw);
  fillCol = rpr->clrSymbolFill(iRaw);
  smb = (SymbolType)rpr->iSymbolType(iRaw);
  hIcon = 0;
}  

Symbol::~Symbol()
{
  if (hIcon) DestroyIcon(hIcon);
}

void Symbol::drawSmb(CDC* cdc, HPALETTE hPal, zPoint pnt) const
{
  if (0 == iSize)
    return;
  if (abs(pnt.x) > 30000 || abs(pnt.y) > 30000) return;
	const double rPixPerPnt = 1.0; //254.0 / 72;
  int iPixSize = iSize * rPixPerPnt;
  Color c, fc;
  if ((long)col == -1)
    c = ::GetSysColor(COLOR_WINDOWTEXT);
  else 
	{
    if (hPal == 0)
      c = col;
    else 
		{  
      int nCol = GetNearestPaletteIndex(hPal,col);
      c = PALETTEINDEX(nCol);
    }  
  }
	CPen pen(PS_SOLID, iWidth, c);
	CPen *penOld = cdc->SelectObject(&pen);
	CBrush br;
	CBrush* brOld;
  if ((long)fillCol == -2)
	{
		LOGBRUSH lb;
		lb.lbStyle=BS_NULL;
		br.CreateBrushIndirect(&lb);
	}
  else 
	{
    if (hPal == 0)
      fc = fillCol;
    else 
		{  
      int nCol = GetNearestPaletteIndex(hPal,fillCol);
      fc = PALETTEINDEX(nCol);
    }  
		br.CreateSolidBrush(fc);
  }
	brOld = cdc->SelectObject(&br);
  if (iPixSize == 1) 
	{
    cdc->MoveTo(pnt);
    pnt.x += 1;
    cdc->LineTo(pnt);
  }
  else switch (smb) 
	{
    case smbCircle:
      {
        int iSiz2 = iPixSize / 2;
				zRect rect;
				rect.left() = pnt.x - iSiz2;
				rect.top() = pnt.y - iSiz2;
				rect.right() = rect.left() + iPixSize + 1;
				rect.bottom() = rect.top() + iPixSize + 1;
 				cdc->Ellipse(rect);
      }
      break;
    case smbSquare:
      {
				zRect rect;
				rect.left() = pnt.x - iPixSize/2;
				rect.top() = pnt.y - iPixSize/2;
				rect.right() = rect.left() + iPixSize;
				rect.bottom() = rect.top() + iPixSize;
 				cdc->Rectangle(rect);
      }
      break;
    case smbDiamond:
      {
				int iSiz2 = iPixSize / 2;
				CPoint p[4];
				p[0].x = pnt.x;
				p[0].y = pnt.y - iSiz2;
				p[1].x = pnt.x - iSiz2;
				p[1].y = pnt.y;
				p[2].x = p[0].x;
				p[2].y = p[0].y + 2 * iSiz2;
				p[3].x = p[1].x + 2 * iSiz2;
				p[3].y = p[1].y;
 				cdc->Polygon(p,4);
      }
      break;
    case smbDeltaUp:
      {
				int iSiz2 = iPixSize / 2;
				zPoint p[3];
				p[0] = pnt;
				p[0].y -= iSiz2;
				p[1].x = p[0].x - iSiz2;
				p[1].y = p[0].y + 2 * iSiz2;
				p[2].x = p[1].x + 2 * iSiz2;
				p[2].y = p[1].y;
 				cdc->Polygon(p,3);
      }
      break;
    case smbDeltaDown:
      {
				int iSiz2 = iPixSize / 2;
				zPoint p[3];
				p[0] = pnt;
				p[0].y += iSiz2;
				p[1].x = p[0].x - iSiz2;
				p[1].y = p[0].y - 2 * iSiz2;
				p[2].x = p[1].x + 2 * iSiz2;
				p[2].y = p[1].y;
 				cdc->Polygon(p,3);
      }
      break;
    case smbPlus:
      {
        int iSiz2 = iPixSize / 2;
				zPoint p = pnt;
				p.y -= iSiz2;
				cdc->MoveTo(p);
				p.y += 2 * iSiz2 + 1;
  			cdc->LineTo(p);
      }
      // fall through
    case smbMinus:
      {
        int iSiz2 = iPixSize / 2;
				zPoint p = pnt;
				p.x -= iSiz2;
				cdc->MoveTo(p);
				p.x += 2 * iSiz2 + 1;
				cdc->LineTo(p);
      }
      break;
    case smbCross:
      {
        int iSiz2 = iPixSize / 2;
				zPoint p = pnt;
  			p.x -= iSiz2;
				p.y -= iSiz2;
				cdc->MoveTo(p);
				p.x += 2 * iSiz2 + 1;
				p.y += 2 * iSiz2 + 1;
				cdc->LineTo(p);
				p = pnt;
  			p.x -= iSiz2;
				p.y += iSiz2;
				cdc->MoveTo(p);
				p.x += 2 * iSiz2 + 1;
				p.y -= 2 * iSiz2 + 1;
  			cdc->LineTo(p);
      } 
			break;
    case smbIcon:
      if (hIcon != 0) 
			{
				int iX, iY;
				iX = GetSystemMetrics(SM_CXICON);
				iY = GetSystemMetrics(SM_CYICON);
				pnt.x -= iX/2;
				pnt.y -= iY/2;
				cdc->DrawIcon(pnt.x,pnt.y,hIcon);
      }
      break;
  }
	cdc->SelectObject(penOld);
	cdc->SelectObject(brOld);
}

zPoint Symbol::pntText(CDC* cdc, zPoint pnt) const
{
	const double rPixPerPnt = 1.0; // 254.0 / 72;
  int iPixSize = iSize * rPixPerPnt;
  switch (smb) {
    case smbCircle:
      pnt.x += M_SQRT2 * iPixSize / 4;
      pnt.y += M_SQRT2 * iPixSize / 4;
      break;
    case smbSquare:
    case smbDeltaUp:
      pnt.x += iPixSize/2;
      pnt.y += iPixSize/2;
      break;
    case smbDiamond:
      pnt.y += iPixSize/4 + 1;
      // fall through
    case smbDeltaDown:
      pnt.x += iPixSize/4 + 1;
      break;
    case smbPlus:
      pnt.y += 2;
      // fall through
    case smbMinus:
      pnt.x += 2;
      break;
    case smbCross:
      pnt.x += iPixSize / 4;
      pnt.y -= iPixSize / 4;
      break;
    case smbIcon:
      if (hIcon != 0) 
			{
				int iX = GetSystemMetrics(SM_CXICON);
				int iY = GetSystemMetrics(SM_CYICON);
				pnt.x += iX/2;
				pnt.y += iY/2;
      }
      break;
  }
  return pnt;
}

FieldSymbolSimple::FieldSymbolSimple(FormEntry* p,
	     long* Symbol, HICON* hIcon)
  : FieldOneSelect(p,Symbol)
{
  hicon = hIcon;
}

FieldSymbolSimple::~FieldSymbolSimple()
{
//  ose->removeNotifySelChange(this, (NotifyProc)&FieldSymbolSimple::SelChanged);
}

void FieldSymbolSimple::create()
{
  FieldOneSelect::create();
  int i = 0;
  ose->AddString(TR("Circle").c_str());
  SendMessage(*ose,CB_SETITEMDATA,i++,(long)smbCircle);
  ose->AddString(TR("Square").c_str());
  SendMessage(*ose,CB_SETITEMDATA,i++,(long)smbSquare);
  ose->AddString(TR("Diamond").c_str());
  SendMessage(*ose,CB_SETITEMDATA,i++,(long)smbDiamond);
  ose->AddString(TR("Delta Up").c_str());
  SendMessage(*ose,CB_SETITEMDATA,i++,(long)smbDeltaUp);
  ose->AddString(TR("Delta Down").c_str());
  SendMessage(*ose,CB_SETITEMDATA,i++,(long)smbDeltaDown);
  ose->AddString(TR("Plus").c_str());
  SendMessage(*ose,CB_SETITEMDATA,i++,(long)smbPlus);
  ose->AddString(TR("Minus").c_str());
  SendMessage(*ose,CB_SETITEMDATA,i++,(long)smbMinus);
  ose->AddString(TR("Cross").c_str());
  ose->SendMessage(CB_SETITEMDATA,i++,(long)smbCross);
//  ose->add(TR("Icon"));
//  SendMessage(*ose,CB_SETITEMDATA,i++,(long)smbIcon);
  ose->SetCurSel((int)*val);
  ose->setNotify(this, (NotifyProc)&FieldSymbolSimple::SelChanged, Notify(CB_SETCURSEL));
}

bool FieldSymbolSimple::fDrawIcon()
{
    return false;
}

bool FieldSymbolSimple::DrawObject(long value, DRAWITEMSTRUCT* dis)
{
  bool fRet = true;
	CDC dc;
	dc.Attach(dis->hDC);

  Color c = ::GetSysColor(COLOR_WINDOWTEXT);
  Color fc = ::GetSysColor(COLOR_ACTIVECAPTION);
  if (dis->itemState & ODS_SELECTED)
    c = ::GetSysColor(COLOR_HIGHLIGHTTEXT);
  CPen pen(PS_SOLID, 1, c);
  CBrush brush(fc);
  CBrush *oldBrush=dc.SelectObject(&brush);
  CPen *oldPen=dc.SelectObject(&pen);
  int left=dis->rcItem.left;
  int top=dis->rcItem.top;
  switch (SymbolType(value)) {
    case smbCircle:
      dc.Ellipse(left+2, top+2, left+14 , top+14);
      break;
    case smbSquare:
      dc.Rectangle(left+2, top+2, left+14 , top+14);
      break;
    case smbDiamond:
      {
				POINT p[4] = { { left + 7, top + 2} , { left + 2, top + 7} ,
                       { left + 7,top + 12}, { left + 12, top + 7} };
				dc.Polygon(p,4);
      }
      break;
    case smbDeltaUp:
      {
				POINT p[3] = { {left + 8, top + 2}, { left + 2, top + 13}, { left+ 13, top + 13} };
				dc.Polygon(p, 3);
      }
      break;
    case smbDeltaDown:
      {
				POINT p[3] = { {left + 8, top + 13}, { left + 2, top + 2}, { left + 13, top + 2} };
				dc.Polygon(p, 3);
      }
      break;
    case smbPlus:
				dc.MoveTo(left + 8, top + 3);
				dc.LineTo(left + 8, top + 13);
      // fall through
    case smbMinus:
				dc.MoveTo(left + 3, top + 8);
				dc.LineTo(left + 13, top + 8);
      break;
    case smbCross:
				dc.MoveTo(left + 3, top + 3);
				dc.LineTo(left + 14, top + 14);
				dc.MoveTo(left + 3, top + 13);
				dc.LineTo(left + 14, top + 2);
      break;
    default:
      fRet = false;
  }
  dc.SelectObject(oldBrush);
  dc.SelectObject(oldPen);
	dc.Detach();
  return fRet;
}

int FieldSymbolSimple::SelChanged(Event*)
{
  int id = ose->GetCurSel();
  CString s;
  ose->GetLBText(id, s);
  if (String(s) == TR("Icon")) {
    String sTypes("%S (*.ico)", TR("Icons"));
		CFileDialog frmIcon(TRUE, "*.ico", 0, 0, sTypes.c_str(), _frm->wnd());  
    //zFileOpenForm frmIcon((zWindow*)_frm,TR("Select Icon"),"*.ico",types);
    if (frmIcon.DoModal() == IDOK) 
		{
      if (*hicon) ::DestroyIcon(*hicon);
      *hicon = ::ExtractIcon(AfxGetInstanceHandle() ,frmIcon.GetPathName(),0);
      if (*hicon == (HICON)1) *hicon = (HICON)0;
    }
  }
  return 0;
}

FieldSymbol::FieldSymbol(FormEntry* parent, const String& sQuestion,
	      long* symbol, HICON* hIcon)
  : FieldGroup(parent)
{
  FormEntry* st = NULL;
  if (sQuestion.length())
    st = new StaticTextSimple(this,sQuestion);
  fss = new FieldSymbolSimple(this, symbol, hIcon);
  if (st)
    fss->Align(st, AL_AFTER);
}

