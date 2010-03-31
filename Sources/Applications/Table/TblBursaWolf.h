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
/* TableBursaWolf
   Copyright Ilwis System Development ITC
   february 2003, by Jan Hendrikse
	Last change:  JH   15 Feb 03    9:44 am
*/

#ifndef TBLBURSAWOLF_H
#define TBLBURSAWOLF_H
#include "Engine\Applications\TBLVIRT.H"
#include "Engine\Map\Point\PNT.H"
#include "Engine\Domain\dm.h"
#include "Engine\SpatialReference\Ellips.h"

IlwisObjectPtr * createTableBursaWolf(const FileName& fn, IlwisObjectPtr& ptr, const String& sExpr, vector<void *> parms );

class DATEXPORT TableBursaWolf: public TableVirtual
{
  friend class TableVirtual;
public:  
  static const char* sSyntax();
  static CoordCTS _export ctsFindLocalCentroid(Array<LatLon> lalo,
	Array<double> rHeight, CoordSystemViaLatLon& csvll, long iNrValidPts);
  TableBursaWolf(const FileName& fn, TablePtr&);
    static TableBursaWolf* create(const FileName& fn, TablePtr&, const String& sExpr);
protected:
  TableBursaWolf(const FileName& fn, TablePtr& p, const Table& tbl, 
                const Column& cCoordsIn, const Column& rHeightsIn,
				const double rX, const double rY, const double rZ, 
				const double rXrot, const double rYrot, const double rZrot,
				const double rScaleDiff, const String sCsy,
				const double x0, const double y0, const double z0,
				const bool fUserPiv);

  ~TableBursaWolf();
  virtual void Store();
	String sAddInfo() const;
  virtual void UnFreeze();
  virtual bool fFreezing();
  virtual String sExpression() const;
private:
  void Init();
  long iCollectValidInputLatLonHeights();
  Table m_TblIn;
  Column colCoordsIn;
  Column colHeightsIn;
	Column colCoordsInAndOut;
	Column colHeightsInAndOut;
	//LatLon* lalo1;
	//double* rHeight1;
	Array<LatLon> lalo1;
	Array<double> rHeight1;
  double m_rX; 
  double m_rY; 
  double m_rZ; 
  double m_rXrot; 
  double m_rYrot; 
  double m_rZrot;			
  double m_rScaleDiff;
  double m_rX0; 
  double m_rY0; 
  double m_rZ0;
  bool m_fUserPivot;
  String m_sCsyOut;
	CoordSystem m_CsyOut;
  String m_sCsyIn;
	CoordSystem m_CsyIn;
  ArrayLarge<bool> fInvalidCrdsIn;
  ArrayLarge<bool> fInvalidHgtsIn;
  long iValidPoints;
	long iValidCoords;
  long iValidHeights; 
  Column colCoordsOut;
  Column colHeightsOut;
	Column colctsXCoordIn;
  Column colctsYCoordIn;
  Column colctsZCoordIn;
  Column colctsXCoordOut;
  Column colctsYCoordOut;
  Column colctsZCoordOut;
	CoordCTS ctsCentroid1;
	CoordCTS ctsCentroid2;
	CoordSystemViaLatLon* csvll1;
	CoordSystemViaLatLon* csvll2;
	CoordSystemProjection* cspr1;
	CoordSystemProjection* cspr2;
	bool fProj1;
	bool fProj2;
	Ellipsoid ell1;
	Ellipsoid ell2;
};

#endif
