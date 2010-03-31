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
/* TableSpatCorr
   Copyright Ilwis System Development ITC
   august 1995, by Jelle Wind, Dick Visser
	Last change:  WK    4 Aug 98    6:03 pm
*/

#ifndef ILWMAPPNTSPATCOR_H
#define ILWMAPPNTSPATCOR_H
#include "Engine\Applications\TBLVIRT.H"
#include "Engine\Map\Point\PNT.H"
#include "Engine\SpatialReference\Distance.h"

IlwisObjectPtr * createTableSpatCorr(const FileName& fn, IlwisObjectPtr& ptr, const String& sExpr, vector<void *> parms );


class DATEXPORT TableSpatCorr: public TableVirtual
{
  friend class TableVirtual;
  enum SPT { eLogT, eOmniDirT, eBiDirecT, eBiDirecBWT};
public:  
  static const char* sSyntax();
  TableSpatCorr(const FileName& fn, TablePtr&);
  static TableSpatCorr* create(const FileName& fn, TablePtr&, const String& sExpr);
protected:
  //TableSpatCorr(const FileName& fn, TablePtr&, const PointMap& pmp);
  TableSpatCorr(const FileName& fn, TablePtr& p,const PointMap& pmap,
                SPT esp, double rLagLength, long iNrDistClasses,
                double rDirection, double rTolerance, double rBandWidth,
								bool fSphericDist);

  ~TableSpatCorr();
  virtual void Store();
	String sAddInfo() const;
  virtual void UnFreeze();
  virtual bool fFreezing();
  virtual String sExpression() const;
	Distance::DistanceMethod m_distMeth;
private:
  void Init();
  void DetLimits(double RMaxDiff);
  int iGroup(double rDistance);
  bool TableSpatCorr::StatSemiVar();

  //String c_sSPType;
  double c_rLagLength;
  long c_iNrDistClasses;  // stored value in ODF is not used anymore !!!!
  double c_rDirection;
  double c_rTolerance;
  double c_rBandWidth;
	double rDataMean;  // over-all average of point-attribute values
  double rDataVariance; // over-all mean of squared diffs of point-attribute values

  SPT eSPType;
  PointMap pmp;
  long iValidPoints;
	
  ArrayLarge<bool> fInvalid;

  Column colDistAvg1;
  Column colDistAvg2;
  Column colDist;
  Column colCorr;
  Column colVar;
  Column colPairs;
  Column colPairs1;
  Column colSem1;
  Column colPairs2;
  Column colSem2;
  RealArray rDistLimits;
	LongArray iNrObserv;
	Distance dis;
};

#endif




