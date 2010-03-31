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
/* zApp extensions
   by Wim Koolhoven, january 1996
  (c) Ilwis System Development ITC
	Last change:  WK   28 Aug 98    4:35 pm
*/

#ifndef ZAPPTOOLS_H
#error include ZappInterface/ZappTools.h before including Zappext.h
#endif

#ifndef ZAPPEXT_H
#define ZAPPEXT_H

#undef IMPEXP
#define IMPEXP __import

class IMPEXP ExtRect: public zRect
{
public:
  ExtRect() {}
  ExtRect(const zRect& rect): zRect(rect) {}
  int center() { return (left() + right()) / 2; }
  int middle() { return (top() + bottom()) / 2; }
  zPoint topCenter()    { return zPoint(center(), top()); }
  zPoint bottomCenter() { return zPoint(center(), bottom()); }
  zPoint middleLeft()   { return zPoint(left(), middle()); }
  zPoint middleRight()  { return zPoint(right(), middle()); }
  void Check()
  { if (top() > bottom()) 
      { int tmp = top(); top() = bottom(); bottom() = tmp; }
    if (left() > right())  
      { int tmp = left(); left() = right(); right() = tmp; }
  }    
};

inline int dist(zPoint p, zPoint q)
{
  return abs(p.x - q.x) + abs(p.y - q.y);
}

#endif // ZAPPEXT_H







