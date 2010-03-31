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
#ifndef ZAPPTOOLS_H
#define ZAPPTOOLS_H

#include "Headers\stdafx.h"

#define FLD_ALPHA			    0x00000001L
#define FLD_NUMERIC			  0x00000002L
#define FLD_SIGNEDNUMERIC	0x00000004L
#define FLD_HEXNUMERIC		0x00000008L
#define FLD_ALPHANUMERIC	0X00000010L
#define FLD_CHARSETMASK		0x000000ffL

#define FLD_NOTREQUIRED		0x00000100L
#define FLD_STRIPLIT		  0x00040000L
#define FLD_COMPLETE		  0x00080000L

#define FLD_NOAUTOSELECT	0x00100000L
#define FLD_TOUPPER			  0x00004000L
#define FLD_TOLOWER			  0x00008000L
#define FLD_PROPER			  0x00000020L
#define FLD_SHOWGUIDES		0x00200000L

typedef int zCoord;

class zPoint: public CPoint {
public:
  zPoint() {}
  zPoint(zCoord u):CPoint(u, u) {}
  zPoint(zCoord u,zCoord v):CPoint(u,v) {}
  zPoint(const CPoint& p):CPoint((const CPoint&)p) {}

  zPoint operator -() const { return zPoint(-x,-y); }
  zPoint operator +(const zPoint p) const { return zPoint(x+p.x,y+p.y);}
  zPoint operator -(const zPoint p) const { return zPoint(x-p.x,y-p.y); }
  zPoint operator *(const zCoord i) const { return zPoint(x*i,y*i);}
  friend zPoint operator *(const zCoord i,const zPoint p) { return zPoint(i*p.x, i* p.y); }
  zPoint operator /(zCoord i) const { return zPoint(x/i,y/i);}
  zPoint operator %(zCoord i) const { return zPoint(x%i,y%i);}
  zPoint operator +=(const zPoint p) { return zPoint(x+=p.x,y+=p.y);}
  zPoint operator -=(const zPoint p) { return zPoint(x-=p.x,y-=p.y);}
  zPoint operator *=(const zCoord i) { return zPoint(x *= i, y *= i); }
  int operator ==(const zPoint p) const { return x==p.x && y==p.y;}
  int operator !=(const zPoint p) const { return x!=p.x || y!=p.y;}
  int operator >=(const zPoint p) const { return x>=p.x && y>=p.y;}
  int operator <=(const zPoint p) const { return x<=p.x && y<=p.y;}
  int operator >(const zPoint p) const { return x>p.x && y>p.y;}
  int operator <(const zPoint p) const { return x<p.x && y<p.y;}
  zPoint operator =(const zCoord c) { x = y = c; return zPoint(c); }
  zPoint Min(const zPoint p) const { return zPoint(min(x, p.x), min(y, p.y));}
  zPoint Max(const zPoint p) const { return zPoint(max(x, p.x), max(y, p.y));}
  /*zCoord& x() { return x; }
  zCoord& y() { return y; }
  zCoord x() const { return x; }
  zCoord y() const { return y; }*/

  void setNull() { x = y = 0; }
  //operator POINT() { return (POINT)*this; }
};

class zDimension : public CSize
{
    public:
        zDimension() : CSize(0,0) {}
        zDimension(int x, int y) : CSize(x,y) {}
        zDimension(CPoint pnt) : CSize(pnt) {}
        zDimension(int iSz) : CSize(iSz) {}
        zDimension(CSize sz) : CSize(sz) {}

        int width() const { return abs(cx); }
        int height() const { return abs(cy); }
        long& width()  { return cx; }
        long& height() { return cy; }
    
        zDimension operator -(const zDimension p) const { return zDimension(cx-p.cx,cy-p.cy); }
        zDimension operator *(const int i) const { return zDimension(cx*i,cy*i);}
        zDimension operator /(int i) const { return zDimension(cx/i,cy/i);}
        zDimension operator %(int i) const { return zDimension(cx%i,cy%i);}
        zDimension operator *=(const int i) { return zDimension(cx *= i, cy *= i); }
        int operator >=(const zDimension p) const { return cx>=p.cx && cy>=p.cy;}
        int operator <=(const zDimension p) const { return cx<=p.cx && cy<=p.cy;}
        int operator >(const zDimension p) const { return cx>p.cx && cy>p.cy;}
        int operator <(const zDimension p) const { return cx<p.cx && cy<p.cy;}
        zDimension operator =(const int c) { cx = cy = c; return zDimension(c); }
        zDimension Min(const zDimension p) const { return zDimension(min(cx, p.cx), min(cy, p.cy));}
        zDimension Max(const zDimension p) const { return zDimension(max(cx, p.cx), max(cy, p.cy));}
};
    
class zRect: public CRect
{
public:
  zRect() {}
  zRect(const CRect& rect): CRect(rect) {}
	zRect(POINT tl, POINT br): CRect(tl, br) {}
	zRect(int l, int t, int r, int b): CRect(l,t,r,b) {}
	zRect(POINT tl, SIZE sz): CRect(tl, sz) {}

  long& left() { return TopLeft().x; }
  long& top() { return TopLeft().y; }
  long& right() { return BottomRight().x; }
  long& bottom() { return BottomRight().y; }
	zPoint& topLeft() { return (zPoint&)TopLeft(); }
	zPoint& bottomRight() { return (zPoint&)BottomRight(); }
  int width() const { return Width(); }
  int height() const { return Height(); }
	zDimension size() 
	{ return zDimension(right()-left(), bottom()-top()); }
};

class zRectRegion: public CRgn
{
public:
	zRectRegion(LPCRECT rect) 
		{ CreateRectRgnIndirect(rect); }
	zRectRegion(int x1, int y1, int x2, int y2)
		{ CreateRectRgn(x1,y1,x2,y2); }
};


#endif ZAPPTOOLS_H
