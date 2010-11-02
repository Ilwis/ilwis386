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

#define OPAQUE_VALUE 0
//class structRGB { public: byte r, g, b, transparency; };

enum DrawColors { drcLIGHT, drcNORMAL, drcDARK, drcGREY };

#define colorUNDEF Color(0,0,0,255)

class _export Color
{
public:
	static Color clrPrimary(int iNr);

	Color() { m_red = m_green = m_blue = 0; m_transparency = OPAQUE_VALUE; }
	Color(byte rd, byte gr, byte bl, byte transparency = OPAQUE_VALUE)
	    { m_red = rd; m_green = gr; m_blue = bl; m_transparency = transparency; }
	Color(const Color& c)
	{ iValue = c.iValue; }
	Color& operator=(const Color& c)
	{ iValue = c.iValue; return *this; }
	Color(long _iVal)
	{ iValue = _iVal; }
	void SetHSI(byte hue, byte sat, byte intens);
	byte& red()   { return m_red; }
	byte& green() { return m_green; }
	byte& blue()  { return m_blue; }
	byte& transparency() { return m_transparency; }
	byte red()   const { return m_red; }
	byte green() const { return m_green; }
	byte blue()  const { return m_blue; }
	byte alpha() const { return 255 - m_transparency; }
	byte transparency() const { return m_transparency; }
	double redP()   const { return (double)m_red / 255.0; }
	double greenP() const { return (double)m_green / 255.0; }
	double blueP()  const { return (double)m_blue / 255.0; }
	double transparencyP()  const { return (double)m_transparency / 255.0; }
	double alphaP()  const { return 1.0 - (double)m_transparency / 255.0; }
	byte yellow()   const { return 255-m_blue; }
	byte magenta() const { return 255-m_green; }
	byte cyan()  const { return 255-m_red; }
	bool operator==(const Color& c)
	{ return (m_red==c.red()) && (m_green==c.green()) && (m_blue==c.blue() && m_transparency==c.transparency()); }
	bool operator!=(const Color& c)
		{ return !(*this == c); }
	//operator long() const ; // for COLORREF conversion; so it will ignore the transparency
	//long iVal() const ;
	inline operator long() const
	{ return iValue & 0x00ffffff; } // needed for COLORREF
	inline long iVal() const
	{ return iValue; }
	byte hue() const; 
	byte sat() const; 
	byte intens() const; 
	byte grey() const; 
	Color clrDraw(DrawColors drc) const;
private:
	union {
		struct{byte m_red, m_green, m_blue, m_transparency;};
		long iValue;
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







