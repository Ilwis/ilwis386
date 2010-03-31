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
/* $Log: /ILWIS 3.0/RasterApplication/POLRAS.H $
 * 
 * 4     12-11-99 9:23 Wind
 * bug 1132 (actived use of doubles)
 * 
 * 3     9-06-99 4:13p Martin
 * // -> /*
 * 
 * 2     6/09/99 11:34a Martin
 * Changed Array<int> to ArrayInt to solve compile problems (short, int)
// Revision 1.5  1998/09/16 17:24:41  Wim
// 22beta2
//
// Revision 1.4  1997/09/11 09:11:05  Wim
// Removed mpPatchTmp from the class, added it instead as parameter to fInitFill()
//
// Revision 1.3  1997-08-14 11:53:44+02  Wim
// Excluded PrepareLine for RealPatches, because excluded anyway
//
/* MapRasterizePolygon
   Copyright Ilwis System Development ITC
   june 1995, by Jelle Wind
   Last change:  WK   11 Sep 97   11:06 am
*/

#ifndef ILWMAPPOLRAS_H
#define ILWMAPPOLRAS_H
#include "Engine\Applications\MAPFPOL.H"
#include "Engine\Base\DataObjects\ARRAY.H"

IlwisObjectPtr * createMapRasterizePolygon(const FileName& fn, IlwisObjectPtr& ptr, const String& sExpr, vector<void *> parms=vector<void*>() );

inline MinMax undef(MinMax) {
  return MinMax(RowCol(iUNDEF, iUNDEF), RowCol(iUNDEF, iUNDEF));
}

class DATEXPORT MapRasterizePolygon: public MapFromPolygonMap
{
  friend class DATEXPORT MapVirtual;
  friend class DATEXPORT MapFromPolygonMap;
public:
  static const char* sSyntax();
  virtual void Store();
  static MapRasterizePolygon* create(const FileName&, MapPtr& p, const String& sExpression);
  MapRasterizePolygon(const FileName&, MapPtr&);
  bool fFreezing();
protected:
  _export MapRasterizePolygon(const FileName&, MapPtr&, const PolygonMap& pm, 
                const GeoRef& gr);
  ~MapRasterizePolygon();
  virtual String sExpression() const;
 
private:
	long FindValue(long row, long col) const;
};

#endif




