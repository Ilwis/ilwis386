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
/* $Log:
*/

#include "Headers\toolspch.h"
#include "Engine\Base\DataObjects\Color.h"

void Color::SetHSI(byte hue, byte sat, byte intens)
{
  double X, Y;
  if (hue == 0) {
    X = sat; Y = 0;
  }
  else if (hue == 60) {
    X = 0; Y = sat;
  }
  else if (hue == 120) {
    X = -sat; Y = 0;
  }
  else if (hue == 180) {
    X = 0; Y = -sat;
  }
  else if ((hue < 60) || (hue > 180)) {
    double F = tan((int)hue/120.0*M_PI);
    X = sat / sqrt(1 + F*F); Y = F * X;
  }
  else {
    double F = tan((int)hue/120.0*M_PI);
    X = -sat / sqrt(1 + F*F); Y = F * X;
  }  
  int iBlue = (int)(((int)intens - X/3 - Y * sqrt(3.0)/3)*255.0/240);
	m_blue = iBlue < 0 ? 0 : iBlue > 255 ? 255 : iBlue;
  int iGreen = (int)((2 * ((int)intens - X/3) - (int)m_blue)*255/240.0);
	m_green = iGreen < 0 ? 0 : iGreen > 255 ? 255 : iGreen;
  int iRed = (int)((3.0 * (int)intens - (int)m_blue - (int)m_green)*255/240.0);
	m_red = iRed < 0 ? 0 : iRed > 255 ? 255 : iRed;
}

byte Color::hue() const 
{ 
	double y = sqrt(3.0)/2 * (green()-(long)blue());
  double x = red()-(green()+(long)blue())/2; 
  if ((x == 0) && (y==0))
    return 0;
  double r = (120 / M_PI) * atan2(y, x); 
  if (r < 0) 
    r += 240;
  return byteConv(r);
} 
 
byte Color::sat() const 
{ 
	return byteConv(sqrt((double)(red()*(long)red()+green()*(long)green()+blue()*(long)blue()-red()*(long)green()
                       -red()*(long)blue()-green()*(long)blue())*240/255.0)); 
}

byte Color::intens() const 
{ 
	return byteConv(((int)red()+(int)green()+(int)blue()) / 255.0 * 80.0); 
}

byte Color::grey() const 
{ 
	return byteConv(0.3 * red() + 0.59 * green() + 0.11 * blue()); 
}

Color Color::clrDraw(DrawColors drc) const
{
	Color c = *this;
  switch (drc) {
    case drcLIGHT:
      c.red()   /= 2;
      c.green() /= 2;
      c.blue()  /= 2;
      c.red()   += 128;
      c.green() += 128;
      c.blue()  += 128;
      break;
    case drcDARK:
      c.red()   /= 2;
      c.green() /= 2;
      c.blue()  /= 2;
      break;
    case drcGREY: {
      byte b = (byte)(0.3 * c.red() + 0.59 * c.green() + 0.11 * c.blue());
      c.red()   = b;
      c.green() = b;
      c.blue()  = b;
      break;
    }  
  }
  return c;
}

inline Color::operator long() const 
{ 
 long color = 0; 
 color= (red() << 16) | (green() << 8) | blue(); 
 return color;
}

inline long Color::iVal() const 
{
	long c = m_red + 256 * m_green + 256 * 256 * m_blue + 256 * 256 * 256 * m_transparency;
	return c;
}

inline void Color::setVal(long _iVal) {
	red() =  _iVal & 0xff;
	green() = _iVal >> 8;
	blue() =  _iVal  >> 16; 
	transparency() = _iVal >> 24;
}

Color Color::clrPrimary(int iNr)
{
	switch (iNr%32)
	{
		case  0: return Color(  0,  0,  0);
		case  1: return Color(255,  0,  0);
		case  2: return Color(255,255,  0);
		case  3: return Color(  0,  0,255);
		case  4: return Color(255,  0,255);
		case  5: return Color(  0,255,255);
		case  6: return Color(  0,255,  0);
		case  7: return Color(128,128,128);
		case  8: return Color(224,224,224);  // was white 255,255,255
		case  9: return Color(128,  0,  0);
		case 10: return Color(128,128,  0);
		case 11: return Color(  0,  0,128);
		case 12: return Color(128,  0,128);
		case 13: return Color(  0,128,128);
		case 14: return Color(  0,128,  0);
		case 15: return Color(255,128,  0);
		case 16: return Color(191,  0,  0);
		case 17: return Color(191,191,  0);
		case 18: return Color(  0,  0,191);
		case 19: return Color(191,  0,191);
		case 20: return Color(  0,191,191);
		case 21: return Color(  0,191,  0);
		case 22: return Color(191,191,191);
		case 23: return Color(192,220,192);
		case 24: return Color( 63,  0,  0);
		case 25: return Color( 63, 63,  0);
		case 26: return Color(  0,  0, 63);
		case 27: return Color( 63,  0, 63);
		case 28: return Color(  0, 63, 63);
		case 29: return Color(  0, 63,  0);
		case 30: return Color( 63, 63, 63);
		case 31: return Color(127, 63,  0);
	}  
	return Color();
}
