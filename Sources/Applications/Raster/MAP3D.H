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
/* MapApply3D
   Copyright Ilwis System Development ITC
   Last change:  JEL  20 May 97   11:37 am
*/

#ifndef ILWMAP3D_H
#define ILWMAP3D_H
#include "Engine\Applications\MAPFMAP.H"

IlwisObjectPtr * createMapApply3D(const FileName& fn, IlwisObjectPtr& ptr, const String& sExpr, vector<void *> parms=vector<void*>() );

class DATEXPORT GeoRef3D;

class DATEXPORT MapApply3D: public MapFromMap
{
  friend MapFromMap;
public:  
  static const char* sSyntax();  
  virtual bool fFreezing();
  virtual String sExpression() const;
  static MapApply3D* create(const FileName& fn, MapPtr& p, const String& sExpr);
  MapApply3D(const FileName& fn, MapPtr& p);
protected:
  virtual void Store();
  MapApply3D(const FileName& fn, MapPtr& p, const Map& map, const GeoRef& gr);
  ~MapApply3D();
  void   Init();
  struct RowColValueStruct {
    public:
      RowCol rc;
      union {
        long iRaw;
        short shRaw;
        byte bRaw;
        double rVal;
      };
  };
  void MakePoints(long iRow, long iStartCol, long iEndCol, short& n,
                  long iColDelta);
  void DrawPolygons(long n);
  void SetLineArrays(const RowCol& rcFrom, const RowCol& rcTo);
  void DrawPolygonFromArrays(long iRowMin, long iRowMax, byte bRaw);
  void DrawPolygonFromArrays(long iRowMin, long iRowMax, short shRaw);
  void DrawPolygonFromArrays(long iRowMin, long iRowMax, long iRaw);
  void DrawPolygonFromArrays(long iRowMin, long iRowMax, double rVal);
  void Add(long iRow, long iCol);
  bool fSameGrf;
  GeoRef3D* grf3d;
  LongArray aiCol1, aiCol2;
  ByteBuf bInpBuf, bOutBuf;
  IntBuf shInpBuf, shOutBuf;
  LongBuf iInpBuf, iOutBuf;
  RealBuf rInpBuf, rOutBuf;
  RowColBuf rcTopBuf, rcBottomBuf;
};

#endif




