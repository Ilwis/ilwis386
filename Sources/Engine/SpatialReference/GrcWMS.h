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
/* GeoRefCornersWMS
   Copyright Ilwis System Development ITC
   march 1995, by Wim Koolhoven
	Last change:  WK   11 Jun 98    2:02 pm
*/

#ifndef ILWGRCORNERSWMS_H
#define ILWGRCORNERSWMS_H
#include "Engine\SpatialReference\Grsmpl.h"

typedef void (ForeignFormat::*RetrieveImageProc)();

class _export GeoRefCornersWMS: public GeoRefCorners
{
  friend class GeoRef;
  friend class GeoRefPtr;
  static GeoRefCornersWMS* create(const FileName& fn, const String& sExpression);
public:
  GeoRefCornersWMS(const FileName& fn, const CoordSystem& cs, 
                MinMax mx, bool fCoc,
                const Coord& cMin, const Coord& cMax)
    : GeoRefCorners(fn,cs,mx.rcSize(), fCoc, cMin, cMax), fInitial(true),crdMinInit(cMin), crdMaxInit(cMax)
    { }
  GeoRefCornersWMS(const FileName& fn, MinMax mx, bool fCoc,
                const Coord& cMin, const Coord& cMax)
    : GeoRefCorners(fn, mx.rcSize(), fCoc, cMin, cMax), fInitial(true), crdMinInit(cMin), crdMaxInit(cMax)
    { }
  static const char* sSyntax();
  virtual String sType() const;
  virtual void Store();
  String sName(bool fExt, const String& sDirRelative) const;
  void AdjustBounds(MinMax mx, MinMax zoom, MinMax oldBounds, bool recalcCoords);
  void setMinMax(MinMax mx);
  bool initial();
  void reset();
  bool hasChanged();
  void resetChanged();

  CoordBounds getInitialCoordBounds();
  CoordBounds calcNewCB(MinMax mx, MinMax zoom, MinMax oldBounds);

  void Lock();
  void Unlock();
  CoordBounds cbWMSRequest() const;
  RowCol rcWMSRequest() const;
  void SetRCWMSRequest(RowCol rc);
  void SetCBWMSRequest(const CoordBounds & cb);
  void SetWMSHandler(ForeignFormat* context, RetrieveImageProc proc);
  void retrieveImage();
 protected:
  GeoRefCornersWMS(const FileName&);
  bool fInitial;
  Coord crdMinInit;
  Coord crdMaxInit;
  CoordBounds m_cbWMSRequest;
  RowCol m_rcWMSRequest;
  ForeignFormat* wmsFormat;
  RetrieveImageProc retrieveImageProc;
  CCriticalSection csHandleRequest;
};


#endif // ILWGRCORNERS_H




