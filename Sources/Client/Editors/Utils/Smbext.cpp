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
/* $Log: /ILWIS 3.0/UserInterfaceObjects/Smbext.cpp $
 * 
 * 14    19-02-03 13:51 Retsios
 * Added a Resize function - for the time being this is only needed for
 * the legend and representation of classes, where the original effect is
 * too "thick" compared to the layout.
 * 
 * 13    20-02-02 10:29 Willem
 * The SelectObject function calls are now properly paired (not mixing
 * handles and pointers anymore)
 * 
 * 12    7-03-01 17:12 Koolhoven
 * changed internal size calculations
 * now just size 1 is 1 pixel, instead of taking care of sizes of inches
 * etc.
 * 
 * 11    5-03-01 18:34 Koolhoven
 * removed calls to GetDeviceCaps() always calculate with 0.1 mm units
 * 
 * 10    10/20/00 3:41p Martin
 * char is not enough to store 255 + x values for different fonts items/
 * simple symbols
 * 
 * 9     23-06-00 14:07 Koolhoven
 * Do not use palette in draw when current DC does not use a palette
 * 
 * 8     4-04-00 17:46 Koolhoven
 * Added Read() and Write() functions to load/store in ODF
 * 
 * 7     30-03-00 14:37 Koolhoven
 * Corrected for use with metafiles (needed for Layout, Copying to
 * ClipBoard etc.)
 * 
 * 6     20-12-99 18:16 Koolhoven
 * Reading of a MapView object (.mpv file) is now possible
 * 
 * 5     19-07-99 12:19 Koolhoven
 * Do not delete objects created on the stack
 * 
 * 4     7-06-99 16:31 Koolhoven
 * Removed use of zDisplay in drawSmb() and thus preventing assertion
 * errors
 * 
 * 3     7-06-99 15:51 Koolhoven
 * pntText() now uses a CDC* instead a zDisplay* as parameter
 * 
 * 2     5/26/99 12:56p Martin
 * 
 * 1     5/26/99 10:03a Martin
// Revision 1.6  1998/09/18 15:12:18  Wim
// In rPixelPerPoint() hdcLast and rPixPerPnt are now static variables
// and no longer members of the class.
//
// Revision 1.5  1998-09-16 18:33:58+01  Wim
// 22beta2
//
// Revision 1.4  1997/09/23 13:50:09  martin
// Resource leake closed.
//
// Revision 1.3  1997/09/11 14:39:55  martin
// Changed the way items are drawn.
//
// Revision 1.2  1997/08/27 13:52:23  Wim
// Minimum size of symbols is 1
//
/* smbext.c
   Extended Symbol
// by Wim Koolhoven
// (c) Ilwis, ITC
	Last change:  WK   18 Sep 98    4:10 pm
*/
#include "Client\Headers\formelementspch.h"
#include "Client\Editors\Utils\SYMBOL.H"
#include "Client\FormElements\fldcolor.h"
#include "Client\Editors\Utils\Smbext.h"
#include "Engine\Representation\Rpr.h"
#include "Headers\Hs\Symbol.hs"
#include "Engine\Map\Mapview.h"
#include "Engine\Representation\Rprclass.h"

#undef Polygon

ExtendedSymbol::ExtendedSymbol()
: smType(smSIMPLE), cSmb('0')
{
  iSize = 10;
  iWidth = 1;
  col = -1;
  fillCol = -2;
  smb = smbCircle;
  rRotation = 0;
}

ExtendedSymbol::ExtendedSymbol(const MapView& view, const char* sSection)
: smType(smSIMPLE), cSmb('0')
{
  iSize = view->iReadElement(sSection, "Size");
  view->ReadElement(sSection, "Color", (Color&)col);
  String sType;
  view->ReadElement(sSection, "SymbolType", sType);
  if (sType == "Font") {
    smType = smFONT;
    view->ReadElement(sSection, "SymbolFont", sFaceName);
    cSmb = view->iReadElement(sSection, "SymbolChar");
  }
  else {
    smType = smSIMPLE;
    smb = (SymbolType)view->iReadElement(sSection, "Symbol");
    view->ReadElement(sSection, "FillColor", (Color&)fillCol);
    iWidth = view->iReadElement(sSection, "Width");
  }
  rRotation = 0;
  view->ReadElement(sSection, "Rotation", rRotation);
}

ExtendedSymbol::ExtendedSymbol(const Representation& rpr, long iRaw)
: smType(smSIMPLE), cSmb('0'), smb(smbCircle)
{
  iSize = rpr->iSymbolSize(iRaw);
  iWidth = rpr->iSymbolWidth(iRaw);
  col = rpr->clrSymbol(iRaw);
  fillCol = rpr->clrSymbolFill(iRaw);
  int iSmb = rpr->iSymbolType(iRaw);
  if (iSmb >= 0)
    smb = (SymbolType) iSmb;
  else {
    smType = smFONT;
    cSmb = -iSmb;
    sFaceName = rpr->sSymbolFont(iRaw);
  }
  rRotation = rpr->rSymbolRotation(iRaw);
}

ExtendedSymbol::ExtendedSymbol(const RepresentationPtr* rpr, long iRaw)
: smType(smSIMPLE), cSmb('0'), smb(smbCircle)
{
  iSize = rpr->iSymbolSize(iRaw);
  iWidth = rpr->iSymbolWidth(iRaw);
  col = rpr->clrSymbol(iRaw);
  fillCol = rpr->clrSymbolFill(iRaw);
  int iSmb = rpr->iSymbolType(iRaw);
  if (iSmb >= 0)
    smb = (SymbolType) iSmb;
  else {
    smType = smFONT;
    cSmb = -iSmb;
    sFaceName = rpr->sSymbolFont(iRaw);
  }
  rRotation = rpr->rSymbolRotation(iRaw);
}  

ExtendedSymbol::~ExtendedSymbol()
{
}

void ExtendedSymbol::Store(RepresentationClass* rcl, long iRaw)
{
  if (smType == smSIMPLE) 
	{
    //rcl->PutSymbolType(iRaw,(short)smb);
  }
  else 
	{
    //rcl->PutSymbolType(iRaw,-(short)cSmb);
    rcl->PutSymbolFont(iRaw,sFaceName);
  }
  rcl->PutSymbolSize(iRaw,iSize);
  rcl->PutSymbolWidth(iRaw,iWidth);
  rcl->PutSymbolColor(iRaw,col);
  rcl->PutSymbolFill(iRaw,fillCol);
  rcl->PutSymbolRotation(iRaw,rRotation);
}

void ExtendedSymbol::Read(const char* sSection, const char* sPrefix, const FileName& filename)
{
	String sSize("%s Size", sPrefix);
	String sColor("%s Color", sPrefix);
	String sSymbolType("%s SymbolType", sPrefix);
	String sSymbol("%s Symbol", sPrefix);
	String sWidth("%s Width", sPrefix);
	String sFillColor("%s FillColor", sPrefix);
	String sSymbolFont("%s SymbolFont", sPrefix);
	String sSymbolChar("%s SymbolChar", sPrefix);
	String sRotation("%s Rotation", sPrefix);

  ObjectInfo::ReadElement(sSection, sSize.c_str(), filename, iSize);
  ObjectInfo::ReadElement(sSection, sColor.c_str(), filename, col);
	String sType;
  ObjectInfo::ReadElement(sSection, sSymbolType.c_str(), filename, sType);
	if ("Simple" == sType) {
		smType = smSIMPLE;
		int iSmb;
		ObjectInfo::ReadElement(sSection, sSymbol.c_str(), filename, iSmb);
    smb = (SymbolType)iSmb;
    ObjectInfo::ReadElement(sSection, sFillColor.c_str(), filename, fillCol);
    ObjectInfo::ReadElement(sSection, sWidth.c_str(), filename, iWidth);
	}
	else {
		smType = smFONT;
    ObjectInfo::ReadElement(sSection, sSymbolFont.c_str(), filename, sFaceName);
		int iSmb;
		ObjectInfo::ReadElement(sSection, sSymbolChar.c_str(), filename, iSmb);
    cSmb = iSmb;
	}
}

void ExtendedSymbol::Write(const char* sSection, const char* sPrefix, const FileName& filename)
{
	String sSize("%s Size", sPrefix);
	String sColor("%s Color", sPrefix);
	String sSymbolType("%s SymbolType", sPrefix);
	String sSymbol("%s Symbol", sPrefix);
	String sWidth("%s Width", sPrefix);
	String sFillColor("%s FillColor", sPrefix);
	String sSymbolFont("%s SymbolFont", sPrefix);
	String sSymbolChar("%s SymbolChar", sPrefix);
	String sRotation("%s Rotation", sPrefix);

  ObjectInfo::WriteElement(sSection, sSize.c_str(), filename, (long)iSize);
  ObjectInfo::WriteElement(sSection, sColor.c_str(), filename, col);
  switch (smType) 
	{
    case smSIMPLE:
      ObjectInfo::WriteElement(sSection, sSymbolType.c_str(), filename, "Simple");
      ObjectInfo::WriteElement(sSection, sSymbol.c_str(), filename, (long)smb);
      ObjectInfo::WriteElement(sSection, sWidth.c_str(), filename, (long)iWidth);
      ObjectInfo::WriteElement(sSection, sFillColor.c_str(), filename, fillCol);
      break;
    case smFONT:
      ObjectInfo::WriteElement(sSection, sSymbolType.c_str(), filename, "Font");
      ObjectInfo::WriteElement(sSection, sSymbolFont.c_str(), filename, sFaceName);
      ObjectInfo::WriteElement(sSection, sSymbolChar.c_str(), filename, (long)cSmb);
      break;  
  }
  ObjectInfo::WriteElement(sSection, sRotation.c_str(), filename, rRotation);
}

void ExtendedSymbol::WriteLayer(MapView& view, const char* sSection) const
{
  view->WriteElement(sSection, "Size", (long)iSize);
  view->WriteElement(sSection, "Color", col);
  switch (smType) 
	{
    case smSIMPLE:
      view->WriteElement(sSection, "SymbolType", "Simple");
      view->WriteElement(sSection, "Symbol", (long)smb);
      view->WriteElement(sSection, "Width", (long)iWidth);
      view->WriteElement(sSection, "FillColor", fillCol);
      break;
    case smFONT:
      view->WriteElement(sSection, "SymbolType", "Font");
      view->WriteElement(sSection, "SymbolFont", sFaceName);
      view->WriteElement(sSection, "SymbolChar", (long)cSmb);
      break;  
  }
  view->WriteElement(sSection, "Rotation", rRotation);
}

double ExtendedSymbol::rPixelPerPoint(HDC hDC)
{
	// 25.4 mm/inch, 0.1 mm units, 72 points/inch
	//	double rPixPerPnt = 254.0 / 72; is too large (factor 2.7 acoording to Boudewijn)
	double rPixPerPnt = 1.0; // very arbitrary number (3.5 / 2.7 > 1.0)
  return rPixPerPnt;
}

void ExtendedSymbol::drawSmb(CDC* cdc, HPALETTE hPal, zPoint pnt)
{
  if (0 == iSize)
    return;
  if (abs(pnt.x) > 30000 || abs(pnt.y) > 30000) return;
  int iPixSize = round(iSize * rPixelPerPoint(cdc->GetSafeHdc()));
	bool fPalette = cdc->GetDeviceCaps(RASTERCAPS) & RC_PALETTE ? true : false;
	if (!fPalette)
		hPal = 0;
  switch (smType) 
  { 
    case smSIMPLE: 
    {
      Color c, fc;
      if ((long)col == -1)
        c = ::GetSysColor(COLOR_WINDOWTEXT);
      else {
        if (hPal == 0)
          c = col;
        else {  
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
      else {
        if (hPal == 0)
          fc = fillCol;
        else {  
          int nCol = GetNearestPaletteIndex(hPal,fillCol);
          fc = PALETTEINDEX(nCol);
        }  
				br.CreateSolidBrush(fc);
      }
			brOld = cdc->SelectObject(&br);
      if (iPixSize == 1) {
        cdc->MoveTo(pnt);
        pnt.x += 1;
        cdc->LineTo(pnt);
      }
      else switch (smb) {
        case smbCircle:
        {
          int iSiz2 = iPixSize / 2;
					zRect rect;
					rect.left() = pnt.x - iSiz2;
					rect.top() = pnt.y - iSiz2;
					rect.right() = rect.left() + iPixSize + 1;
					rect.bottom() = rect.top() + iPixSize + 1;
  				cdc->Ellipse(rect);
        } break;
        case smbSquare:
        {
					zRect rect;
					rect.left() = pnt.x - iPixSize/2;
					rect.top() = pnt.y - iPixSize/2;
					rect.right() = rect.left() + iPixSize;
					rect.bottom() = rect.top() + iPixSize;
  				cdc->Rectangle(rect);
        } break;
        case smbDiamond:
        {
          int iSiz2 = iPixSize / 2;
					zPoint p[4];
  				p[0].x = pnt.x;
					p[0].y = pnt.y - iSiz2;
					p[1].x = pnt.x - iSiz2;
					p[1].y = pnt.y;
					p[2].x = p[0].x;
					p[2].y = p[0].y + 2 * iSiz2;
					p[3].x = p[1].x + 2 * iSiz2;
					p[3].y = p[1].y;
  				cdc->Polygon(p,4);
        } break;
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
        } break;
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
        } break;
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
        } break;
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
        } break;
      }
			cdc->SelectObject(penOld);
			cdc->SelectObject(brOld);
    } break;
    case smFONT:
    {
      int iOrientation;
      double rRotRad;
      if (rRotation == rUNDEF || abs(rRotation) > 360) {
        iOrientation = 0;
        rRotRad = 0;
      }
      else {
        iOrientation = -10 * rRotation;
        rRotRad = - M_PI / 180 * rRotation;
      }
			CFont fnt;
			fnt.CreateFont(iPixSize, 0, iOrientation, iOrientation, FW_NORMAL, 0, 0, 0,  
                 SYMBOL_CHARSET, OUT_TT_PRECIS , CLIP_DEFAULT_PRECIS,
								 DEFAULT_QUALITY, FF_DONTCARE, sFaceName.c_str());   

//      zFont fnt(sFaceName, zDimension(0,iPixSize), FW_NORMAL,
//                ffDontCare, (zFntPitch)0x04, 0, iOrientation, iOrientation,
//                SymbolCharSet);
      Color clrOld = cdc->SetTextColor(col);
			CFont* fntOld = cdc->SelectObject(&fnt);
      cdc->SetBkMode(TRANSPARENT);
      UINT taOld = cdc->SetTextAlign(TA_CENTER|TA_TOP);

      pnt.x -= round(sin(rRotRad) * iPixSize / 2);
      pnt.y -= round(cos(rRotRad) * iPixSize / 2);

      char s[2];
      s[0] = cSmb;
      s[1] = 0;
      cdc->TextOut(pnt.x, pnt.y, s);
			cdc->SelectObject(fntOld);
			cdc->SetTextColor(clrOld);
      cdc->SetTextAlign(taOld);
    } break;
  }    
}

zPoint ExtendedSymbol::pntText(CDC* cdc, zPoint pnt) 
{
  int iPixSize= iSize * rPixelPerPoint(cdc->m_hDC);
  switch (smType) 
  { 
    case smSIMPLE: 
    {
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
          pnt.x += iPixSize / 2;
          pnt.y += iPixSize / 2;
          break;
      }
    } break;
    case smFONT:
    {
      pnt.x += iPixSize/2;
      pnt.y += iPixSize/2;
    } break;
  }    
  return pnt;
}

void ExtendedSymbol::Resize(double rFactor)
{
	iSize *= rFactor;
}

FieldSymbolTypeSimple::FieldSymbolTypeSimple(FormEntry* parent, 
  long* type, const String& sType)
  : FieldOneSelect(parent,type,true), sInitFont(sType)
{
  psn->iMinWidth = 1.5 * FLDNAMEWIDTH;
}

int CALLBACK AddFont(const LOGFONT* lplf, const TEXTMETRIC* lptm, 
                     unsigned long nFontType, LPARAM lp)
{
  if (0 == (TRUETYPE_FONTTYPE & nFontType))
    return 1;
  LOGFONT* lf = (LOGFONT*)lplf;
  if (SYMBOL_CHARSET != lf->lfCharSet)
    return 1;
  OneSelectEdit* ose = (OneSelectEdit*) lp;
  String str("%s.ttfont", lf->lfFaceName);
  ose->AddString(str.c_str());
  return 1;
}                               

void FieldSymbolTypeSimple::create()
{
  FieldOneSelect::create();
  HDC hdc = ::GetDC(NULL);
  ::EnumFontFamilies(hdc, NULL, &AddFont, (LPARAM)ose);
  ::ReleaseDC(NULL, hdc);
  idSimple = ose->AddString(TR("Simple").c_str());
  ose->SelectString( -1, sInitFont.c_str());
}

FieldSimpleSymbolSimple::FieldSimpleSymbolSimple(FormEntry* parent, long* smb)
  : FieldOneSelect(parent,smb)
{
}

void FieldSimpleSymbolSimple::create()
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
  SendMessage(*ose,CB_SETITEMDATA,i++,(long)smbCross);
  ose->SetCurSel((int)*val);
}

bool FieldSimpleSymbolSimple::fDrawIcon()
{
    return false;
}

bool FieldSimpleSymbolSimple::DrawObject(long value, DRAWITEMSTRUCT* dis)
{
	bool fRet = true;
	Color c = ::GetSysColor(COLOR_WINDOWTEXT);
	Color fc = ::GetSysColor(COLOR_ACTIVECAPTION);
	if (dis->itemState & ODS_SELECTED)
		c = ::GetSysColor(COLOR_HIGHLIGHTTEXT);
	CPen *pen = new CPen(PS_SOLID, 1, c);
	CBrush *brush = new CBrush(fc);
	CDC dc;
	dc.Attach(dis->hDC);
	CBrush *oldBrush=dc.SelectObject( brush);
	CPen *oldPen = dc.SelectObject(pen);
	int left=dis->rcItem.left;
	int top=dis->rcItem.top;
	switch (SymbolType(value))
	{
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
	DeleteObject(*brush);
	DeleteObject(*pen);
	delete brush;
	delete pen;
	dc.Detach();
	return fRet;
}

FieldFontSymbolSimple::FieldFontSymbolSimple(FormEntry* parent, short* c)
: FormEntry(parent), fnt(0), cSmb(c)
{
  psn->iMinWidth = 450;
  psn->iMinHeight = psn->iHeight = 160;
}

FieldFontSymbolSimple::~FieldFontSymbolSimple()
{
  if (fnt)
    delete fnt;
}

void FieldFontSymbolSimple::show(int sw)
{
  lb->ShowWindow(sw);
}

void FieldFontSymbolSimple::StoreData()
{
  int id = lb->GetCurSel();
  CString sChar;
	lb->GetText(id, sChar);
  *cSmb = sChar[0];
	if ( *cSmb < 0 )
		*cSmb += 256;
}

void FieldFontSymbolSimple::SetFont(CFont* font)
{
  lb->ResetContent();
  lb->SetFont(font); //SendMessage(WM_SETFONT, (unsigned int)(HFONT)*font, 0); 
  if (fnt)
    delete fnt;
  fnt = font;  
  
  // reset size!
  CRect rect(psn->iPosX, psn->iPosY, 
             psn->iPosX+psn->iWidth, psn->iPosY+psn->iHeight);
  lb->MoveWindow(rect);
             
  CDC *dc = lb->GetWindowDC();
  TEXTMETRIC tm;
  dc->GetTextMetrics(&tm);
  lb->SetColumnWidth(tm.tmMaxCharWidth+4) ; //SendMessage(LB_SETCOLUMNWIDTH, tm.tmMaxCharWidth+4, 0);                  
  char s[2];
  s[1] = '\0';
  for (int b = tm.tmFirstChar; b <= tm.tmLastChar; ++b) {
    s[0] = (BYTE)b;
    lb->AddString(s);
  }  
  int iSel = *cSmb - tm.tmFirstChar;
  lb->SetCurSel(iSel);
}

void FieldFontSymbolSimple::create()
{
  CPoint pntFld(psn->iPosX, psn->iPosY);
  zDimension dimFld = zDimension(psn->iWidth,psn->iHeight);
  lb = new CListBox;
	lb->Create(  WS_TABSTOP|WS_BORDER|WS_HSCROLL|WS_GROUP| LBS_HASSTRINGS|LBS_MULTICOLUMN,
		           CRect(pntFld, dimFld), _frm->wnd(), Id());
  lb->SetColumnWidth(18) ; //SendMessage(*lb, LB_SETCOLUMNWIDTH, 18, 0);                  
}


FieldExtendedSymbol::FieldExtendedSymbol(FormEntry* parent, 
        ExtendedSymbol& symbol, bool fAskSize, bool fAskColor)
  : FieldGroup(parent), smb(symbol), ffc(0)
{
  FormEntry *feFirst, *st, *fe;
  feFirst = st = new StaticTextSimple(this,TR("Symbol &Type"));
  feFirst->psn->SetBound(0,10,0,10);
  iType = 0;
  String sType;
  if (smb.smType == ExtendedSymbol::smSIMPLE)
    sType = TR("Simple");
  else  
    sType = smb.sFaceName;
  fsts = new FieldSymbolTypeSimple(this, &iType, sType);
  fsts->SetCallBack((NotifyProc)&FieldExtendedSymbol::TypeChanged, this); 
  fsts->Align(st, AL_AFTER);
    
  fgSimple = new FieldGroup(this);
  fgSimple->Align(feFirst, AL_UNDER);
  st = new StaticTextSimple(fgSimple, TR("&Symbol"));
  st->psn->SetBound(0,10,0,10);
  fsss = new FieldSimpleSymbolSimple(fgSimple, (long*)&smb.smb);
  fsss->SetCallBack((NotifyProc)&FieldExtendedSymbol::FieldSymbolCallBack, this);
  fsss->Align(st, AL_AFTER);
  if (fAskSize) {
    fe = new FieldInt(fgSimple, TR("&Size"), &smb.iSize, ValueRangeInt(1L,250L));
    fe->Align(st, AL_UNDER);
    ffc = new FieldFillColor(fgSimple, TR("Fill Color"), &smb.fillCol);
  }
  else {
    ffc = new FieldFillColor(fgSimple, TR("Fill Color"), &smb.fillCol);
    ffc->Align(st, AL_UNDER);
  }
  if (smb.iWidth < 1)
    smb.iWidth = 1;
  new FieldInt(fgSimple, TR("&Line Width"), &smb.iWidth, ValueRangeInt(1L,100L));
  if (fAskColor)
    new FieldColor(fgSimple, TR("&Color"), &smb.col);
  
  fgFont = new FieldGroup(this);    
  fgFont->Align(feFirst, AL_UNDER);
//  fgFont->SetIndependentPos();
  ffss = new FieldFontSymbolSimple(fgFont, &smb.cSmb);
  ffss->SetIndependentPos();
  ffss->psn->SetBound(0,5,0,10);
  if (fAskSize) {
    fe = new FieldInt(fgFont, TR("&Size"), &smb.iSize, ValueRangeInt(1L,250L));
    fe->Align(ffss, AL_UNDER);
    if (fAskColor)
      new FieldColor(fgFont, TR("&Color"), &smb.col);
  }
  else if (fAskColor)
  {
    fe = new FieldColor(fgFont, TR("&Color"), &smb.col);
    fe->Align(ffss, AL_UNDER);
  }
  new FieldReal(fgFont, TR("&Rotation"), &smb.rRotation, ValueRange(-360, 360, 0.1));
}

int FieldExtendedSymbol::TypeChanged(void *)
{
  fsts->StoreData();
  if (iType == fsts->idSimple) {
    fgSimple->Show();
    fgFont->Hide();
    FieldSymbolCallBack(0);
  }
  else {
    fgSimple->Hide();
    int id = fsts->ose->GetCurSel();
		char sFont[100];
    fsts->ose->GetLBText(id, sFont);
    char* s = strrchr(sFont, '.');
    *s = 0;
		CFont* fnt = new CFont();
		fnt->CreateFont(16, 0, 0, 0, FW_NORMAL, 0, 0, 0,  
                 SYMBOL_CHARSET, OUT_TT_PRECIS , CLIP_DEFAULT_PRECIS,
								 DEFAULT_QUALITY, FF_DONTCARE, sFont); 
		
    ffss->SetFont(fnt);
    fgFont->Show();
  }
  return 0;  
}

int FieldExtendedSymbol::FieldSymbolCallBack(void *)
{
  fsss->StoreData();
  switch (SymbolType(smb.smb)) {
    case smbCircle:
    case smbSquare:
    case smbDiamond:
    case smbDeltaUp:
    case smbDeltaDown:
      ffc->Show();
      break;
    case smbPlus:
    case smbMinus:
    case smbCross:
      ffc->Hide();
      break;
  }

	return 1;
}

void FieldExtendedSymbol::StoreData()
{
  FieldGroup::StoreData();
  if (iType == fsts->idSimple) 
    smb.smType = ExtendedSymbol::smSIMPLE;
  else {
    smb.smType = ExtendedSymbol::smFONT;
    int id = fsts->ose->GetCurSel();
		char sFont[100];
    fsts->ose->GetLBText(id, sFont);
    char* s = strrchr(sFont, '.');
    *s = 0;
    smb.sFaceName = sFont;
  }  
}

