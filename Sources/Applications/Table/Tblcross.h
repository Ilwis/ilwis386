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
/* TableCross
   Copyright Ilwis System Development ITC
   april 1996, by Jelle Wind, Dick Visser
	Last change:  WK   10 Mar 98   12:52 pm
*/

#ifndef ILWTBLCROSS_H
#define ILWTBLCROSS_H
#include "Engine\Applications\TBLVIRT.H"
#include "Engine\Map\Raster\Map.h"

IlwisObjectPtr * createTableCross(const FileName& fn, IlwisObjectPtr& ptr, const String& sExpr, vector<void *> parms );

class CrossCombLongLong { 
public:
  CrossCombLongLong()
    { i1 = iUNDEF; i2 = iUNDEF; iIndex = iUNDEF; iCount = 0; }  
  CrossCombLongLong(long _i1, long _i2)
    { i1 = _i1; i2 = _i2; iIndex = iUNDEF; iCount = 1; }  
  bool operator ==(const CrossCombLongLong& cc)
  { return cc.i1 == i1 && cc.i2 == i2; }  
  long i1, i2, iIndex, iCount; 
}; 

class CrossCombLongReal { 
public:
  CrossCombLongReal()
    { i1 = iUNDEF; r2 = rUNDEF; iIndex = iUNDEF; iCount = 0; }  
  CrossCombLongReal(long _i1, double _r2)
    { i1 = _i1; r2 = _r2; iIndex = iUNDEF; iCount = 1; }  
  bool operator ==(const CrossCombLongReal& cc)
  { return cc.i1 == i1 && cc.r2 == r2; }  
  long i1; 
  double r2;
  long iIndex, iCount; 
}; 

class CrossCombRealLong { 
public:
  CrossCombRealLong()
    { r1 = rUNDEF; i2 = iUNDEF; iIndex = iUNDEF; iCount = 0; }  
  CrossCombRealLong(double _r1, long _i2)
    { r1 = _r1; i2 = _i2; iIndex = iUNDEF; iCount = 1; }  
  bool operator ==(const CrossCombRealLong& cc)
  { return cc.r1 == r1 && cc.i2 == i2; }  
  double r1; 
  long i2, iIndex, iCount; 
}; 

class CrossCombRealReal { 
public:
  CrossCombRealReal()
    { r1 = rUNDEF; r2 = rUNDEF; iIndex = iUNDEF; iCount = 0; }  
  CrossCombRealReal(double _r1, double _r2)
    { r1 = _r1; r2 = _r2; iIndex = iUNDEF; iCount = 1; }  
  bool operator ==(const CrossCombRealReal& cc)
  { return cc.r1 == r1 && cc.r2 == r2; }  
  double r1, r2;
  long iIndex, iCount; 
}; 

class DATEXPORT TableCross: public TableVirtual
{
    friend class TableVirtual;

    public:
        static const char*      sSyntax();
        Column                  colMap1;
        Column                  colMap2;
        static TableCross*              create(const FileName& fn, TablePtr& p, const String& sExpression);
        TableCross(const FileName& fn, TablePtr& p);

    protected:
        TableCross(const FileName& fn, TablePtr& p, const Map& mp1, const Map& mp2,
                   const String& sMapCross, bool fIgnore1, bool fIgnore2);
        ~TableCross();

        String                          sCrossValue(const Map& map, long iRaw);
        virtual void                    Store();
        virtual void                    UnFreeze();
        virtual bool                    fFreezing();
        virtual String                  sExpression() const;
        Table                           tblAgg(const FileName& fn) const;
        Column                          colAgg(const Table& tbl, const String& sColName, const String& sAgg) const;

    private:
        void                            Init();
        Map                             map1;
        Map                             map2;
        String                          sMapCross; //, sTblAggr;

        Column                          colNPix;
        Column                          colArea;
        bool                            fIgnoreUndef1;
        bool                            fIgnoreUndef2;
        HashTable<CrossCombLongLong>    htll;
        HashTable<CrossCombLongReal>    htlr;
        HashTable<CrossCombRealLong>    htrl;
        HashTable<CrossCombRealReal>    htrr;

};

#endif




