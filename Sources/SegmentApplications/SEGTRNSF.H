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
/* SegmentMapTransform
   Copyright Ilwis System Development ITC
   may 1996, by Jelle Wind
	Last change:  JEL   6 May 97    6:01 pm
*/

#ifndef ILWSEGMAPTRANSF_H
#define ILWSEGMAPTRANSF_H
#include "Engine\Applications\SEGVIRT.H"
#include "Engine\SpatialReference\Coordsys.h"

IlwisObjectPtr * createSegmentMapTransform(const FileName& fn, IlwisObjectPtr& ptr, const String& sExpr, vector<void *> parms=vector<void*>() );

class _export SegmentMapTransform: public SegmentMapVirtual
{
  friend class SegmentMapVirtual;
public:
	static double rDefaultDensifyDistance(const SegmentMap& segmap);
  SegmentMapTransform(const FileName&, SegmentMapPtr& p);
  SegmentMapTransform(const FileName& fn, SegmentMapPtr& p, const SegmentMap& sm,
											const CoordSystem& cs, const double rD);
  ~SegmentMapTransform();
  static const char* sSyntax();
  virtual String sExpression() const;
  virtual void Store();
  virtual bool fFreezing();
  static SegmentMapTransform* create(const FileName&, SegmentMapPtr& p, const String& sExpression);
  void Init();
private:
  SegmentMap smp;
  CoordSystem csy;
	double rDistance;
};

#endif




