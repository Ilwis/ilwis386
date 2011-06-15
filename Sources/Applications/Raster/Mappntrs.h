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
/* MapRasterizePoint
   Copyright Ilwis System Development ITC
   june 1995, by Jelle Wind
   Last change:  WK    5 Jun 98   11:15 am
*/

#ifndef ILWMAPPNTRAS_H
#define ILWMAPPNTRAS_H
#include "Engine\Map\Raster\Map.h"
#include "Engine\Applications\MAPFPNT.H"

IlwisObjectPtr * createMapRasterizePoint(const FileName& fn, IlwisObjectPtr& ptr, const String& sExpr, vector<void *> parms=vector<void*>() );
ApplicationMetadata metadataMapRasterizePoint(ApplicationQueryData *query);

class DATEXPORT MapRasterizePoint: public MapFromPointMap
{
  enum ptrsType {ptrsNORM, ptrsCOUNT, ptrsSUM};
  friend DATEXPORT MapVirtual;
  friend DATEXPORT MapFromPointMap;
public:
  MapRasterizePoint(const FileName& fn, MapPtr&);
  MapRasterizePoint(const FileName& fn, MapPtr&, const PointMap& pmp, const GeoRef& gr,
              int iPointSize, ptrsType type);
  ~MapRasterizePoint();
  static const char* sSyntax();
  static const char* sSyntaxCount();
  static const char* sSyntaxSum();
  virtual String sExpression() const;
  virtual bool fFreezing();
  virtual void Store();
  static MapRasterizePoint* create(const FileName& fn, MapPtr&, const String& sExpression);
//  virtual void Replace(const String& sExpr);
private:  
  ptrsType type;
  int iPointSize, iHalfSize;
  void Init();
  bool fInitFill();
};

#endif // ILWMAPPNTRAS_H




