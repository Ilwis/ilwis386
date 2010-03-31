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
/* PointMapSegDist
   Copyright Ilwis System Development ITC
   march 1995, by Wim Koolhoven
	Last change:  JEL   6 May 97    5:56 pm
*/

#ifndef ILWPOINTMAPSEGDIST_H
#define ILWPOINTMAPSEGDIST_H
#include "Engine\Applications\PNTVIRT.H"
#include "Engine\Map\Segment\Seg.h"

IlwisObjectPtr * createPointMapSeg(const FileName& fn, IlwisObjectPtr& ptr, const String& sExpr, vector<void *> parms=vector<void*>() );

class DATEXPORT PointMapSegDist: public PointMapVirtual
{
  friend class PointMapVirtual;
public:
  PointMapSegDist(const FileName&, PointMapPtr& p);
  PointMapSegDist(const FileName& fn, PointMapPtr& p, const SegmentMap& sm, double rInterval);
  static PointMapSegDist* create(const FileName&, PointMapPtr& p, const String& sExpression);
  ~PointMapSegDist();
  static const char* sSyntaxSegDist();
  static const char* sSyntaxSegCoords();
  static const char* sSyntaxSegNodes();
	static const char* sSyntaxSegCoordsOrNodes(); 
  virtual String sExpression() const;
  virtual void Store();
  virtual bool fFreezing();
protected:
  void Init();
  SegmentMap smp;
  double rInterval;
};

#endif




