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
#ifndef SEG14_H
#define SEG14_H

/* $Log: /ILWIS 3.0/SegmentMap/SEG14.H $
 * 
 * 4     16/03/00 16:13 Willem
 * - Structures are now properly aligned by using #pragma pack()
 * - lstring15() constructors now clear the data area
 * 
 * 3     17-01-00 8:45a Martin
 * comment
 * 
 * 2     17-01-00 8:17a Martin
 * changed rowcols to coords
// Revision 1.3  1998/09/16 17:25:20  Wim
// 22beta2
//
// Revision 1.2  1997/09/19 15:51:39  Wim
// made some functions const
//
/* SegmentMap structures as in version 1.4
   Copyright Ilwis System Development ITC
   march 1995, by Wim Koolhoven
	Last change:  WK   19 Sep 97   11:20 am
*/

#pragma pack(1)
struct DATEXPORT lstring15  // compatible with ILWIS 1.4 LString(15)
{
  unsigned char data[16];
  unsigned char& len() { return data[0]; }
  lstring15() { memset(data, 0, 16); }
  lstring15(const String& s) {
		memset(data, 0, 16);
    len() = s.length();
    if (len() > 15) len() = 15;
    memcpy(&data[1],(const void *)(s.c_str()),len());
  }
  lstring15(const char* s) {
		memset(data, 0, 16); 
    len() = strlen(s);
    if (len() > 15) len() = 15;
    memcpy(&data[1],s,len());
  }
  operator String() {
    memcpy(sTmp,&data[1],len());
    sTmp[len()] = 0;
    return sTmp;
  }
private:
  static char sTmp[16];
};
//char lstring15::sTmp[16];

struct crdtype        // compatible with ILWIS 1.4 record
{
  short x, y;
  crdtype() {}
  crdtype(short X, short Y) { x = X; y = Y; }
  crdtype(const RowCol& rc) { x = (short)rc.Col; y = (short)rc.Row; }
  operator RowCol() const
    { return RowCol(long(y),long(x)); }
  friend double rDist2(crdtype a, crdtype b)
    { double dx = (a.x - (double)b.x); double dy = (a.y - (double)b.y);
      return dx*dx + dy*dy; }
};



struct minmax14  // compatible with ILWIS 1.4 record
{
  short MinX, MinY, MaxX, MaxY;
  minmax14() {
    MinX =  SHRT_MAX;
    MinY =  SHRT_MAX;
    MaxX = -SHRT_MAX;
    MaxY = -SHRT_MAX;
  }
  minmax14(const MinMax& mm) {
    crdtype min = mm.rcMin;
    MinX = min.x; MinY = min.y;
    crdtype max = mm.rcMax;
    MaxX = max.x; MaxY = max.y;
  }
  operator MinMax() const {
    MinMax mm;
    mm.rcMin = RowCol(crdtype(MinX,MinY));
    mm.rcMax = RowCol(crdtype(MaxX,MaxY));
    return mm;
  }
};

struct segtype        // compatible with ILWIS 1.4 record
{
  lstring15 code;
  crdtype fst, lst;
  minmax14 mm;
  long fstp, lstp;
};

#pragma pack()


#endif
