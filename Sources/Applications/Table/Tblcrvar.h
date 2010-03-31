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
/* TableCrossVarioGram
   Copyright Ilwis System Development ITC
   august 1999, by Jan Hendrikse
	Last change:  JH   15 Feb 99    9:44 am
*/

#ifndef TBLCRVAR_H
#define TBLCRVAR_H
#include "Engine\Applications\TBLVIRT.H"
#include "Engine\Map\Point\PNT.H"
#include "Engine\SpatialReference\Distance.h"

IlwisObjectPtr * createTableCrossVarioGram(const FileName& fn, IlwisObjectPtr& ptr, const String& sExpr, vector<void *> parms );

class DATEXPORT TableCrossVarioGram: public TableVirtual
{
  friend class TableVirtual;
public:  
  static const char* sSyntax();
  TableCrossVarioGram(const FileName& fn, TablePtr&);
  static TableCrossVarioGram* create(const FileName& fn, TablePtr&, const String& sExpr);
protected:
  TableCrossVarioGram(const FileName& fn, TablePtr& p,
                      const PointMap& pmap,
                      const Column& colA,const Column& colB,
                      double rLagSpacing, long iNrDistClasses,
											bool fSphericDist);

  ~TableCrossVarioGram();
  virtual void Store();
	String sAddInfo() const;
  virtual void UnFreeze();
  virtual bool fFreezing();
  virtual String sExpression() const;
private:
  void Init();
  int iGroup(double rDistance);
  bool fComputeSemiAndCrossVar();

  double rLagSpacing;
  long   iNrDistClasses;
  
  PointMap pmp;
  //Table tblAttri;
  Column colA;
  Column colB;
  ArrayLarge<bool> fInvalidA;
  ArrayLarge<bool> fInvalidB;
	long iValidPointsA;
  long iValidPointsB;
  long iValidPointsBothAB;
  double rDataMeanA;
  double rDataMeanB;
  double rDataVarianceA;  //  Mean of (rAi - rAAvg)*(rAi - rAAvg) 
  double rDataVarianceB;  //  Mean of (rBi - rBAvg)*(rBi - rBAvg)
  double rDataCoVarianceAB;  //  Mean of (rAi - rAAvg)*(rBi - rBAvg )
  Column colDist;
  Column colDistAvg;
  Column colPairsA;
  Column colPairsB;
  Column colPairsAB;
  Column colSemA;
  Column colSemB;
  Column colCross;
  RealArray rDistLimits;
	Distance::DistanceMethod m_distMeth;
	bool fCsyLatLonIn, fCsyProjectionIn;
};

#endif
