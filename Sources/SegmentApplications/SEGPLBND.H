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
/*// $Log: /ILWIS 3.0/SegmentMap/SEGPLBND.H $
 * 
 * 4     27-11-00 11:19 Koolhoven
 * removed domain as parameter
 * 
 * 3     15-11-99 13:14 Wind
 * comment
 * 
 * 2     15-11-99 12:47 Wind
 * solved bug 1280 (no internal domain option)
*/
// Revision 1.3  1998/09/16 17:25:20  Wim
// 22beta2
//
// Revision 1.2  1997/07/29 16:22:31  Wim
// fSingleName to distinguish between "single" or "composite" naming
// of the segments.
//
/* SegmentMapPolBoundaries
   Copyright Ilwis System Development ITC
   march 1995, by Wim Koolhoven
	Last change:  WK   29 Jul 97    6:04 pm
*/

#ifndef ILWSEGPOLBND_H
#define ILWSEGPOLBND_H
#include "Engine\Applications\SEGVIRT.H"
#include "Engine\Base\mask.h"
#include "Engine\Map\Polygon\POL.H"

IlwisObjectPtr * createSegmentMapPolBoundaries(const FileName& fn, IlwisObjectPtr& ptr, const String& sExpr, vector<void *> parms=vector<void*>() );

class DATEXPORT SegmentMapPolBoundaries: public SegmentMapVirtual
{
  friend class SegmentMapVirtual;
public:
  SegmentMapPolBoundaries(const FileName&, SegmentMapPtr& p);
  SegmentMapPolBoundaries(const FileName& fn, SegmentMapPtr& p, const PolygonMap& sm, const String& sMask, bool fSingle);
  ~SegmentMapPolBoundaries();
  static const char* sSyntax();
  virtual String sExpression() const;
  virtual void Store();
  virtual bool fFreezing();
  virtual bool fDomainChangeable() const;
  static SegmentMapPolBoundaries* create(const FileName&, SegmentMapPtr& p, const String& sExpression);
  //void AddSegment(Topology& top, const Polygon& polLeft, const Polygon& polRight,DomainSort* pdsrtInp, DomainSort* pdsrt);
  //bool fSelectSegment(const Polygon& polLeft, const Polygon& polRight, Mask& mask);
  void Init();
private:
  PolygonMap pmp;
  Mask mask;
  bool fSingleName;
};

#endif




