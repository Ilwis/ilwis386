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
/* Form for Geodetic Datum Computation
   Copyright Ilwis System Development ITC
   september 2003, by Jan Hendrikse
*/

#ifndef FINDDATUMPARMS_H
#define FINDDATUMPARMS_H

#include "Client\FormElements\formbase.h"
#include "Client\FormElements\fldcol.h"
#include "Engine\SpatialReference\Ellips.h"

class PtMapDomCsy
{
public:
//	PtMapDomCsy() {}
//	PtMapDomCsy(const PtMapDomCsy& p) { pm = p.pm; cs = p.cs; dm = p.dm; sCol = p.sCol; }
	bool fValid() { return pm.fValid(); }
	String sCol;
	Domain dm;
	CoordSystem cs;
	PointMap pm;
};

class _export FormFindDatumParms: public FormBaseDialog
{
public:
  FormFindDatumParms(CWnd* mw, const char* sPar);
private:  
  int exec();
  int MapACallBack(Event*);
	int MapBCallBack(Event*);
	int MethodCallBack(Event*);

	PtMapDomCsy pmdcA, pmdcB;

	PtMapDomCsy BuildAndCheckStruct(const String& sMap);
	int CheckDomainEquality(PtMapDomCsy pmdcA, PtMapDomCsy pmdcB);
	long iFindValidCoordsAndHeights();
	CoordCTS ctsFindCentroid1(long& iActivePnt);
	CoordCTS ctsFindCentroid2(long& iActivePnt);
	FieldGroup *fgA, *fgB;
  FieldPointMap* fldMapA; 
	FieldPointMap*	fldMapB;
	CheckBox* cbUseHeightsA;
  FieldColumn *fldColA; 
	FieldColumn	*fldColB;
	FieldString* fsNrPntsA; 
	FieldString* fsNrPntsB;
	FieldString* fsCsyNameA; 
	FieldString* fsCsyNameB;
	RadioGroup* rgMethod;
	int iMethod;
  String sMapA, sMapB, sColA, sColB;
	int iNrPntsA, iNrPntsB, iNrPntsCommon;
	CoordSystem csA, csB;
	String sCoordSysA, sCoordSysB;
	Domain dmMapA, dmMapB, dmTblA, dmTblB;
	Table tblAttribA, tblAttribB;
	bool fUseHeightsA, fUseHeightsB;
	bool fInverseMolo, fShiftGeoCenter, fBursaWolf, fMolBadekas;
	StaticText* stRemark;
	StaticText* stRemark2;
	LatLon* lalo1;
	LatLon* lalo2;
	double* rHeight1;
	double* rHeight2;
	Array<double> rX1; //array of used geocentric coords
	Array<double> rY1;
	Array<double> rZ1;
	Array<double> rX2;
	Array<double> rY2;
	Array<double> rZ2;
	CoordSystem cs1, cs2;
  CoordSystemViaLatLon* csvll1;
  CoordSystemViaLatLon* csvll2;
	CoordSystemProjection* cspr1;
	CoordSystemProjection* cspr2;
	bool fProj1;
	bool fProj2;
	Ellipsoid ell1;
	Ellipsoid ell2; 
};

#endif // TBLFINDDATUMPARMS_H
