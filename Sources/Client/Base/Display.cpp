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
#include "Headers\toolspch.h"
#include "Engine\Base\AssertD.h"
#include "Client\Base\ZappToMFC.h"

int _export XDIALOG(unsigned int x, CDC *cdc)
{
  int tmp;
	if (cdc==0) 
  {
		int tmp=(int)(x*LOWORD(GetDialogBaseUnits()))/4;
		return tmp;
	}

	LPSTR aveChString="ABCDEFGHIJLKMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
	DWORD tExtents = cdc->GetTextExtent(aveChString,lstrlen(aveChString)).cx;
	long tmpL = ((DWORD)LOWORD(tExtents)/52l);
	if (((DWORD)LOWORD(tExtents)%52l) >= 26l)
		 tmpL++;
	tmpL *= (DWORD)x;
	tmp = tmpL / 4l;
	if ((tmpL % 4l) >= 2l) tmp++;
	return tmp;
}

int _export YDIALOG(unsigned int y, CDC *cdc)
{
  int tmp;
	if (cdc==0) 
		return (y*HIWORD(GetDialogBaseUnits())/16);

	TEXTMETRIC tm;
	GetTextMetrics(cdc->m_hDC,&tm);
	long tmpL = (DWORD)(tm.tmHeight+tm.tmExternalLeading)*(DWORD)y;
	tmp= tmpL/8l;
	if ((tmpL%8l) >= 4l) tmp++;
	return tmp;
}    
//----------------------------------------------------------------------------------------------------------
zDisplay::zDisplay() :
    _owner(NULL),
    _tempDC(NULL),
    _iLockCount(0),
    _oldFont(NULL),
    _oldBrush(NULL),
    _oldPen(NULL)
{
}

zDisplay::zDisplay(CWnd *owner) :
	_owner(owner),
		_tempDC(owner->GetDC()),
    _iLockCount(0),
    _oldFont(NULL),
    _oldBrush(NULL),
    _oldPen(NULL)
{
}

zDisplay::zDisplay(CDC *owner) :
	_owner(NULL),
	_tempDC(owner),
  _iLockCount(0),
  _oldFont(NULL),
  _oldBrush(NULL),
  _oldPen(NULL)
{
}

zDisplay::~zDisplay()
{
	if (!_pens.empty() || !_brushes.empty() || _fonts.size()!=0) // no excpetions from within a destructor
		_owner->MessageBox("XXObject stack in display not emptied.", "Error", MB_OK | MB_ICONSTOP );
    if(_oldFont)  cdc()->SelectObject(_oldFont);
    if(_oldBrush) cdc()->SelectObject(_oldBrush);
    if(_oldPen)   cdc()->SelectObject(_oldPen);
}

CDC *zDisplay::cdc()
{
	if(_tempDC) return _tempDC;
	CDC *dc=_owner->GetDC();
	if (!dc) throw ErrorObject("XXDevice context is not initialized");
	return dc;
}

int zDisplay::lock()
{
	if (_owner && _owner->GetSafeHwnd())
		_tempDC = _owner->BeginPaint(&_paintStruct);
	if ( !_tempDC) throw ErrorObject("XXCould not aquire device context for drawing.");
	_iLockCount++;

	return _iLockCount;
}

int zDisplay::unlock()
{
	if (_owner && _owner->GetSafeHwnd())
		_owner->EndPaint(&_paintStruct);
	if (_iLockCount==0) throw ErrorObject("XXLock count already zero");
	_iLockCount--;
	_tempDC=NULL;
	return _iLockCount;
}

int zDisplay::pushFont(CFont *font)
{
	if (!font) throw ErrorObject("XXInvalid font added to display.");
  _fonts.push(font);
  if (_oldFont)
      cdc()->SelectObject(_fonts.back());
  else
      _oldFont=cdc()->SelectObject(_fonts.back());
  return _fonts.size();
}

int zDisplay::pushPen(CPen *pen)
{
	if (!pen) throw ErrorObject("XXInvalid pen added to display.");
	_pens.push(pen);
    if ( _oldPen) // there is already a pen;
        cdc()->SelectObject(_pens.back());
    else
        _oldPen=cdc()->SelectObject(_pens.back());
	return _pens.size();
}

int zDisplay::pushBrush(CBrush *brush)
{
	if (!brush) throw ErrorObject("XXInvalid brush added to display.");
	_brushes.push(brush);
    if(_oldBrush) // there is already a brush;
        cdc()->SelectObject(_brushes.back());
    else
        _oldBrush=cdc()->SelectObject(_brushes.back());
	return _brushes.size();
}

CFont* zDisplay::popFont()
{
	if (_fonts.empty())
    throw ErrorObject("XXFont stack empty, no pop possible.");
  CFont *font=_fonts.front();
	_fonts.pop();
  if (!_fonts.empty())
    cdc()->SelectObject(_fonts.back());
	else
	{
		cdc()->SelectObject(_oldFont);
		_oldFont = 0;
	}
  return font;
}

CPen* zDisplay::popPen()
{
	if (_pens.empty()) throw ErrorObject("XXPen stack empty, no pop possible.");
	CPen *pen=_pens.front();
	_pens.pop();
	if ( !_pens.empty())
		cdc()->SelectObject(_pens.back());
	else
	{
		cdc()->SelectObject(_oldPen);
		_oldPen = 0;
	}
	return pen;
}

CBrush* zDisplay::popBrush()
{
	if (_brushes.empty()) throw ErrorObject("XXBrush stack empty, no pop possible.");
	CBrush *brush=_brushes.back();
	_brushes.pop();
	if ( !_brushes.empty())
		cdc()->SelectObject(_brushes.back());
	else
	{
		cdc()->SelectObject(_oldBrush);
		_oldBrush = 0;
	}
	return brush;
}

CFont *zDisplay::font()
{
	if ( _fonts.empty()) 
  {
    CFont dummyFont;
		return dummyFont.FromHandle((HFONT)GetStockObject(SYSTEM_FONT)); // base font
  }
	return _fonts.back();
}

zDimension zDisplay::getTextDim(const char *s, const int c) 
{
	ASSERT_VALID(_tempDC);
	return (zDimension)(_tempDC->GetTextExtent( s, c==0?strlen(s):c));
}

int zDisplay::pixPerInchY()
{
	ISTRUE(fINotEqual, _tempDC, (CDC *)NULL);
	return _tempDC->GetDeviceCaps(LOGPIXELSY);
}	

int zDisplay::pixPerInchX()
{
	ISTRUE(fINotEqual, _tempDC, (CDC *)NULL);
	return  _tempDC->GetDeviceCaps(LOGPIXELSX);
}

BOOL zDisplay::rectangle(CRect& rct)
{
	ISTRUE(fINotEqual, _tempDC, (CDC *)NULL);
	return _tempDC->Rectangle(rct);
}

BOOL zDisplay::ellipse(CRect& rct)
{
	ISTRUE(fINotEqual, _tempDC, (CDC *)NULL);
	return _tempDC->Ellipse(rct);
}

BOOL zDisplay::lineTo(const int iX, const int iY)
{
	ISTRUE(fINotEqual, _tempDC, (CDC *)NULL);
	return _tempDC->LineTo(iX, iY);
}

BOOL zDisplay::lineTo(const CPoint& pnt )
{
	return lineTo(pnt.x, pnt.y);
}

zPoint zDisplay::moveTo(const CPoint& pnt)
{
	return moveTo(pnt.x, pnt.y);
}

zPoint zDisplay::moveTo(const int iX, const int iY)
{
	ISTRUE(fINotEqual, _tempDC, (CDC *)NULL);
	return zPoint(_tempDC->MoveTo(iX, iY));
}

BOOL zDisplay::polygon(CPoint *pnts, int iCount)
{
	ISTRUE(fINotEqual, _tempDC, (CDC *)NULL);
	return _tempDC->Polygon(pnts, iCount);
}

int zDisplay::setTextBackMode(int tm)
{
	ISTRUE(fINotEqual, _tempDC, (CDC *)NULL);
	return _tempDC->SetBkMode(tm);
}

BOOL zDisplay::text(const CPoint& pnt, String s)
{
	ISTRUE(fINotEqual, _tempDC, (CDC *)NULL);
	return _tempDC->TextOut(pnt.x, pnt.y, s.scVal());
}

COLORREF zDisplay::textColor(Color clr)
{
	ISTRUE(fINotEqual, _tempDC, (CDC *)NULL);
	return _tempDC->SetTextColor(clr);
}

Color zDisplay::backColor(const Color c) 
{
	ISTRUE(fINotEqual, _tempDC, (CDC *)NULL);
	return  _tempDC->SetBkColor(c);
}
