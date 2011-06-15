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
// $Log: MPSEGDNS.H $
// Revision 1.3  1998/09/16 17:24:40  Wim
// 22beta2
//
// Revision 1.2  1997/08/26 15:39:20  Wim
// Removed not implemented functions
//
/* MapSegmentDensity
   Copyright Ilwis System Development ITC
   august 1996, by Jelle Wind
   Last change:  WK   26 Aug 97    5:35 pm
*/

#ifndef ILWMAPSEGDNS_H
#define ILWMAPSEGDNS_H
#include "Engine\Applications\MAPFSEG.H"
#include "Engine\Base\DataObjects\Patch.h"
#include "Engine\Base\mask.h"

IlwisObjectPtr * createMapSegmentDensity(const FileName& fn, IlwisObjectPtr& ptr, const String& sExpr, vector<void *> parms=vector<void*>() );
ApplicationMetadata metadataMapSegmentDensity(ApplicationQueryData *query);

class DATEXPORT MapSegmentDensity: public MapFromSegmentMap
{
  friend class DATEXPORT MapVirtual;
  friend class DATEXPORT MapFromSegmentMap;
  friend class DATEXPORT MapInterpolContour;
public:
  static const char* sSyntax();
  virtual void Store();
  static MapSegmentDensity* create(const FileName&, MapPtr& p, const String& sExpression);
  MapSegmentDensity(const FileName&, MapPtr& p);
protected:

  MapSegmentDensity(const FileName&, MapPtr& p, const SegmentMap& sm, const String& sMask,
                const GeoRef& gr);
  ~MapSegmentDensity();
  virtual String sExpression() const;
  virtual bool fFreezing();
private:
  bool fInitFill(Map&);
  void CalcPatchBounds(long, long);
  bool fInPatch(const ILWIS::Segment*);
  bool fLineInPatch(const Coord&, const Coord&);
  bool fLineInPatch(const RowCol&, const RowCol&);
  bool fInPatch(const RowCol&);
  bool intersection(Coord co0, Coord co1, Coord cob, Coord coe,
                         Coord& cr, double& rbeta, double& rmu,
                         Coord& crPlusDelta);
  void CalcLineLength(RealPatch&, const Coord&, const Coord&, const RowCol&, const RowCol&, const RowCol&);
  Coord cTopLeft, cBotRight, cTopRight, cBotLeft;
  MinMax mm;
  Mask mask;
  Map mapTmp;
};

#endif



