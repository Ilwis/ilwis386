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
#ifndef PAIR_H
#define PAIR_H

#include "Headers\compatib.h"

typedef int zCoord;

class _export zPair 
{
protected:
  zCoord  _x;
  zCoord  _y;
public:
  zPair(const zPair&p) { _x=p._x; _y=p._y; }
  zPair() { _x=0; _y=0; }
  zPair(zCoord A,zCoord B ):_x(A),_y(B) {}
  zPair(unsigned long d):_x((zCoord)(d&0x0000ffff)), _y((zCoord)((d>>16)&0x0000ffff)){}
  zCoord &x() { return _x; }
  zCoord &y() { return _y; }
  zCoord x() const { return _x; }
  zCoord y() const { return _y; }
};

#define Z_MIN(a,b) (((a)<(b))?(a):(b))
#define Z_MAX(a,b) (((a)>(b))?(a):(b))

class _export zRange:public zPair {
public:
  zRange() {}
  zRange(const zRange&r):zPair((const zPair&)r) {}
  zRange(zCoord a, zCoord b) :zPair(Z_MIN(a,b),Z_MAX(a,b)) {}
  zCoord& lo() { return _x; }
  zCoord& hi() { return _y; }
  zRange operator +(const zCoord a) {
    return zRange(Z_MIN(_x, a), Z_MAX(_y,a));
  }
  zRange operator +(const zRange& a) const {
    return zRange(Z_MIN(_x, a._x), Z_MAX(_y, a._y));
  }
  zRange& operator +=(const zRange& a) {
    if (a._x < _x)
      _x = a._x;
    if (a._y > _y)
      _y = a._y;
    return *this;
  }
  int isNull() { return _x >= _y; }
  void makeNull() { _x = 0x7fff; _y = 0; }
  operator int() const { return _x>=_y? 0:_y-_x; }
  bool in(const zCoord i) const { return (i<=_y && i>=_x) ? TRUE:FALSE; }
};
/*
class zPoint:public CPoint {
public:
  zPoint() {}
  zPoint(zCoord u):CPoint(u, u) {}
  zPoint(zCoord u,zCoord v):CPoint(u,v) {}
  zPoint(const zPoint&p):CPoint((const CPoint&)p) {}

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
  zPoint Min(const zPoint p) const { return zPoint(Z_MIN(x, p.x), Z_MIN(y, p.y));}
  zPoint Max(const zPoint p) const { return zPoint(Z_MAX(x, p.x), Z_MAX(y, p.y));}
  /*zCoord& x() { return x; }
  zCoord& y() { return y; }
  zCoord x() const { return x; }
  zCoord y() const { return y; }

  //void setNull() { x = y = 0; }
  //operator POINT() { return (POINT)*this; }
};

/*class _export zDimension:public zPair {
public:
  zDimension() {}
  zDimension(zCoord w,zCoord h):zPair(w,h) {}
  zDimension(const zDimension& d):zPair(d._x,d._y) {}
  zDimension(unsigned long d):zPair(d) {}
  zCoord& width() { return _x; }
  zCoord& height() { return _y; }
  zCoord width() const { return _x; }
  zCoord height() const { return _y; }
  zCoord widthVal() const { return _x; }
  zCoord heightVal() const { return _y; }
  int operator ==(const zDimension d) const { return _x==d._x && _y==d._y;}
  int operator !=(const zDimension d) const { return _x!=d._x || _y!=d._y;}
};


class _export zRect {
  zPoint tl;
  zPoint br;
public:
  zRect() {}
  zRect(zCoord lft,zCoord tp,zCoord rgt,zCoord btm):tl(lft,tp),br(rgt,btm) {}
  zRect(const zPoint& p,const zDimension& d)
    :tl(p),br(p.xVal()+d.widthVal(),p.yVal()+d.heightVal()) {}
  zRect(const zPoint& p,const zPoint& p2) : tl(p), br(p2) {}
  zRect(const zRect&r):tl(r.tl),br(r.br) {}

  zPoint& topLeft()  { return tl; }
  zPoint& bottomRight() { return br; }
  zPoint topRight() const { return zPoint(br.x(), tl.y()); }
  zPoint bottomLeft() const { return zPoint(tl.x(), br.y()); }
  zCoord& left()  { return tl.x();}
  zCoord& right() { return br.x();}
  zCoord& top()   { return tl.y();}
  zCoord& bottom(){ return br.y();}

  zPoint topLeft() const { return tl; }
  zPoint bottomRight() const { return br; }
  zCoord left() const { return tl.x(); }
  zCoord right() const { return br.x(); }
  zCoord top() const { return tl.y(); }
  zCoord bottom() const { return br.y(); }

  void operator+=(const zPoint &p) { tl +=p; br +=p; }
  void operator-=(const zPoint &p) { tl -= p; br -= p; }
  int operator==(const zRect &r) const { return (tl==r.tl && br==r.br); }
  int operator!=(const zRect &r) const { return (tl!=r.tl || br!=r.br); }
  zDimension size() { return zDimension(br.x()-tl.x(),br.y()-tl.y()); }
  int width() const { return br.xVal() - tl.xVal(); }
  int height() const { return br.yVal() - tl.yVal(); }
  bool isNull() const {
    return tl.xVal()==tl.yVal()==br.xVal()==br.yVal()?1:0;
  }
  zRect& makeNull() { tl = 0; br=0; return *this; }
  zRect& operator=(const RECT& rr) {
    tl.x() =rr.left; br.x() =rr.right; tl.y() =rr.top; br.y()=rr.bottom;
    return *this;
  }
  bool contains(const zPoint p) const { return (p >= tl) && (p <= br); }
  bool contains(const zRect& r) const { return (contains(r.tl) && contains(r.br)); }
  bool intersects(const zRect& r) const { return (tl.Max(r.tl)<br.Min(r.br)) ? 1:0; }
}; */
#endif







