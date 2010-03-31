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
// tls/color
// Color interface for ILWIS 2.0 (compatible with zColor)
// july 1994, Jelle Wind

#ifndef ILW_COLORH
#define ILW_COLORH
#include "Engine\Base\DataObjects\Buf.h"


class structRGB { public: byte r, g, b, mode; };

enum DrawColors { drcLIGHT, drcNORMAL, drcDARK, drcGREY };

class _export Color
{
public:
	static Color clrPrimary(int iNr);

	Color() { iVal = 0;}
	Color(byte rd, byte gr, byte bl, int palRel=0)
	    { rgb.r = rd; rgb.g = gr; rgb.b = bl; rgb.mode = palRel; }
	Color(const Color& c)
		{ iVal = c.iVal; }
	Color& operator=(const Color& c)
		{ iVal = c.iVal; return *this; }
	Color(long _iVal)
		{ iVal = _iVal; }
	void SetHSI(byte hue, byte sat, byte intens);
	byte& red()   { return rgb.r; }
	byte& green() { return rgb.g; }
	byte& blue()  { return rgb.b; }
	byte red()   const { return rgb.r; }
	byte green() const { return rgb.g; }
	byte blue()  const { return rgb.b; }
	byte yellow()   const { return 255-rgb.b; }
	byte magenta() const { return 255-rgb.g; }
	byte cyan()  const { return 255-rgb.r; }
	bool operator==(const Color& c)
		{ return (rgb.r==c.red()) && (rgb.g==c.green()) && (rgb.b==c.blue()); }
	bool operator!=(const Color& c)
		{ return !(*this == c); }
	operator long() const { return iVal; }
	byte hue() const; 
	byte sat() const; 
	byte intens() const; 
	byte grey() const; 
	Color clrDraw(DrawColors drc) const;
private:
	union {
		structRGB rgb;
		long iVal;
	};
};

#ifdef TLSTMPL_C
#define COLOR_SPEC __export
#else
#define COLOR_SPEC __import
#endif

template class COLOR_SPEC Buf<Color>;
template class COLOR_SPEC BufExt<Color>;

typedef Buf<Color>   ColorBuf;
typedef BufExt<Color>   ColorBufExt;

#endif







