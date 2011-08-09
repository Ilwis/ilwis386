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

#include "Headers\toolspch.h"
#include "Applications\Table\TblBursaWolf.h"
#include "Engine\Table\tblstore.h"
#include "Engine\Domain\dmcoord.h"
#include "Engine\Domain\dm.h"
#include "Engine\Table\COLSTORE.H"
//#include <float.h>
#include "Headers\Htp\Ilwisapp.htp"
#include "Headers\Err\Ilwisapp.err"
#include "Headers\Hs\tbl.hs"
#include "Headers\Hs\Table.hs"
//#include "Client\Forms\DatumWizard.h"
#include "Engine\SpatialReference\csviall.h"
#include "Engine\SpatialReference\Csproj.h"

const char* TableBursaWolf::sSyntax()
{
  return "TableBursaWolf(Table, CoordColumn, HeightsColumn, "
					"dx(m), dy(m), dz(m), "
					"Rx(arcsec), Ry(arcsec), Rz(arcsec), "
					"Scale (ppm), targetCoordSys, "
					"X0, Y0, Z0, UserDefinedPivot(0|1))";
}

IlwisObjectPtr * createTableBursaWolf(const FileName& fn, IlwisObjectPtr& ptr, const String& sExpr, vector<void *> parms ) {
	if ( sExpr != "")
		return (IlwisObjectPtr *)TableBursaWolf::create(fn, (TablePtr &)ptr, sExpr);
	else
		return (IlwisObjectPtr *)new TableBursaWolf(fn, (TablePtr &)ptr);
}

TableBursaWolf* TableBursaWolf::create(const FileName& fn, TablePtr& p,
                const String& sExpr)
{
  Array<String> as;
  bool ferror = true;
  int iParms = IlwisObjectPtr::iParseParm(sExpr, as);
  if (iParms != 15 && iParms != 11)
    throw ErrorExpression(sExpr, sSyntax()); 
  FileName fnTbl = as[0];
  fnTbl.sExt = ".tbt";
  Table tbl(fnTbl);
  if (!tbl.fValid())
    NotFoundError(fnTbl);
  Column colCrdsIn = tbl->col(as[1]);
  DomainCoord* dmCrd = colCrdsIn->dm()->pdcrd();
//  if (!colCrdsIn.fValid() || !dmCrd)
//    CoordColumnNotFound(tbl->fnObj);
  Column colHghtsIn = tbl->col(as[2]);
  DomainValue* dmv = colHghtsIn->dm()->pdv();
//  if (!colHghtsIn.fValid() || !dmv)
//   HeightColumnNotFound(tbl->fnObj);
  double dx = as[3].rVal();
  double dy = as[4].rVal();
  double dz = as[5].rVal();
//  if (abs(dx) + abs(dy) + abs(dz) > 3000) 
//	  throw ErrorObject(WhatError(String(STBLErrShifttooLarge), errTableBursaWolf +2), fn);
  double dXrot = as[6].rVal();
  double dYrot = as[7].rVal();
  double dZrot = as[8].rVal();
//  if (abs(dXrot) + abs(dYrot) + abs(dZrot) > 300) 

//	  throw ErrorObject(WhatError(String(STBLErrRotationtooLarge), errTableBursaWolf +3), fn);
double dSc = as[9].rVal();
    //if (abs(dSc) > 100) 
  //  throw ErrorObject(WhatError(String(STBLErrScaleDifftooLarge), errTableBursaWolf +4), fn);
	FileName fnCsy = as[10];
	String sFnCsy = String(fnCsy.sFile);
	CoordSystem cs = CoordSystem(fnCsy);
	fnCsy.sExt = ".csy";
	if (!cs.fValid())
		NotFoundError(fnCsy);
	double x0 = 0;
  double y0 = 0;
  double z0 = 0;
	int iUserPiv = 0;
	bool fUserPiv = false;
	if (iParms == 15)
	{
		x0 = as[11].rVal();
		y0 = as[12].rVal();
		z0 = as[13].rVal();
		iUserPiv = as[14].iVal();
		fUserPiv = (iUserPiv==1)?true:false;
	}
  return new TableBursaWolf(fn, p, tbl, colCrdsIn, colHghtsIn,
	   dx,  dy,  dz,  dXrot, dYrot, dZrot, dSc, sFnCsy, x0, y0, z0, fUserPiv);
}

TableBursaWolf::TableBursaWolf(const FileName& fn, TablePtr& p)
: TableVirtual(fn, p)
{
  fNeedFreeze = true;
  ReadElement("TableBursaWolf", "InputTable", m_TblIn);
  //ReadElement("TableBursaWolf", "PointMapB", pmpB);
  Table tbl= Table(m_TblIn); 
	tbl->LoadData();
  //if (!m_TblIn.fValid())
  //  AttrTableNotFound(fn);
  String sColCrd, sColHgt;
  ReadElement("TableBursaWolf", "InputCrdColumn", sColCrd);
  colCoordsIn = tbl->col(sColCrd);
  //if (!colCoordsIn.fValid())
  //  ColumnNotFound(tbl->fnObj);
  ReadElement("TableBursaWolf", "InputHgtColumn", sColHgt);
  colHeightsIn = tbl->col(sColHgt);
  //if (!colHeightsIn.fValid())
  //  ColumnNotFound(tbl->fnObj); 
	ReadElement("TableBursaWolf", "DX", m_rX); 
  ReadElement("TableBursaWolf", "DY", m_rY); 
  ReadElement("TableBursaWolf", "DZ", m_rZ); 
  ReadElement("TableBursaWolf", "DXrot", m_rXrot); 
  ReadElement("TableBursaWolf", "DYrot", m_rYrot); 
  ReadElement("TableBursaWolf", "DZrot", m_rZrot);			
  ReadElement("TableBursaWolf", "ScaleDiff", m_rScaleDiff); 
   	ReadElement("TableBursaWolf", "X0", m_rX0); 
  ReadElement("TableBursaWolf", "Y0", m_rY0); 
  ReadElement("TableBursaWolf", "Z0", m_rZ0); 
  ReadElement("TableBursaWolf", "UserPivot", m_fUserPivot); 
	ReadElement("TableBursaWolf", "TargetCoordSystem", m_sCsyOut);
	m_CsyOut = CoordSystem(m_sCsyOut);
  objdep.Add(m_TblIn.ptr());
  objdep.Add(m_CsyOut.ptr());
}

TableBursaWolf::TableBursaWolf(const FileName& fn, TablePtr& p, const Table& tbl, 
                    const Column& cCoordsIn, const Column& cHeightsIn,
					const double rX, const double rY, const double rZ, 
					const double rXrot, const double rYrot, const double rZrot,
					const double rScaleDiff, const String sCsy,
					const double rX0, const double rY0, const double rZ0,
					const bool fUserPiv)
: TableVirtual(fn, p, true), //FileName(fn, ".TB#", true), Domain("none")),  
  m_TblIn(tbl),
  colCoordsIn(cCoordsIn),
  colHeightsIn(cHeightsIn),
  m_rX(rX), m_rY(rY), m_rZ(rZ), 
  m_rXrot(rXrot), m_rYrot(rYrot), m_rZrot(rZrot),			
  m_rScaleDiff(rScaleDiff), m_sCsyOut(sCsy),
  m_rX0(rX0), m_rY0(rY0), m_rZ0(rZ0), m_fUserPivot(fUserPiv)
{
  //if (!colCoordsIn.fValid())
  //  ColumnNotFound(tblAttri->fnObj);
  //if (!colHeightsIn.fValid())
  //  ColumnNotFound(tblAttri->fnObj);  
  ptr.SetDomain(Domain("none"));
  //if (colCoordsIn->dm()->pdv() == 0)
  //  throw ErrorValueDomain(colCoordsIn->dm()->sName(true, fnObj.sPath()), colA->sTypeName(), 
  //                        errTableBursaWolf +2);
  //if (colHeightsIn->dm()->pdv() == 0)
  //  throw ErrorValueDomain(colHeightsIn->dm()->sName(true, fnObj.sPath()), colB->sTypeName(), 
   //                       errTableBursaWolf +3);                     
fNeedFreeze = true;
  objdep.Add(m_TblIn.ptr());
  objdep.Add(colCoordsIn.ptr());
  objdep.Add(colHeightsIn.ptr());
  if (!fnObj.fValid())
    objtime = objdep.tmNewest();
}

TableBursaWolf::~TableBursaWolf()
{
}

void TableBursaWolf::Store()
{
  TableVirtual::Store();
  WriteElement("TableVirtual", "Type", "TableBursaWolf");
  WriteElement("TableBursaWolf", "InputTable", m_TblIn);
  WriteElement("TableBursaWolf", "InputCrdColumn", colCoordsIn);
  WriteElement("TableBursaWolf", "InputHgtColumn", colHeightsIn);
  WriteElement("TableBursaWolf", "DX", m_rX); 
  WriteElement("TableBursaWolf", "DY", m_rY); 
  WriteElement("TableBursaWolf", "DZ", m_rZ); 
  WriteElement("TableBursaWolf", "DXrot", m_rXrot); 
  WriteElement("TableBursaWolf", "DYrot", m_rYrot); 
  WriteElement("TableBursaWolf", "DZrot", m_rZrot);			
  WriteElement("TableBursaWolf", "ScaleDiff", m_rScaleDiff); 
  WriteElement("TableBursaWolf", "TargetCoordSystem", m_sCsyOut);
  WriteElement("TableBursaWolf", "X0", m_rX0); 
  WriteElement("TableBursaWolf", "Y0", m_rY0); 
  WriteElement("TableBursaWolf", "Z0", m_rZ0);
  WriteElement("TableBursaWolf", "UserPivot", m_fUserPivot); 
}

String TableBursaWolf::sAddInfo() const
{  
	return "";// s;
}


long TableBursaWolf::iCollectValidInputLatLonHeights()
{  
	// Zie tblForms.cpp Find7ParametersForm::exec()

	long iRec = m_TblIn->iRecs();
	DomainCoord*  pdomcrd = colCoordsIn->dvrs().dm()->pdcrd();
	CoordSystem cs1 = pdomcrd->cs();
	csvll1 = cs1->pcsViaLatLon();
	csvll2 = m_CsyOut->pcsViaLatLon();
	cspr1 = cs1->pcsProjection();
	cspr2 = m_CsyOut->pcsProjection();
	fProj1 = (0 != cspr1);
	fProj2 = (0 != cspr2);
	ell1 = csvll1->ell;
	ell2 = csvll2->ell;

	lalo1.Resize(iRec);
	rHeight1.Resize(iRec);
	
  long iNrValidPts = 0;//m_TblIn->iRecs(); 
  double rH1Tmp;
	Coord cTmp;
	LatLon llTmp;
	for (long i = 0; i < iRec; i++) {
		cTmp = colCoordsIn->cValue(i+1);
		if (cTmp == crdUNDEF) continue;
		
		if (fProj1)
			llTmp = cspr1->llConv(cTmp);
		else
			llTmp = LatLon(cTmp.y, cTmp.x);	
		if (llTmp.fUndef()) continue;
		
		rH1Tmp = colHeightsIn->rValue(i+1);
		if (rH1Tmp == rUNDEF) continue;

		lalo1[iNrValidPts] = llTmp;
		rHeight1[iNrValidPts] = rH1Tmp;
		iNrValidPts++;
	}
	if (iNrValidPts == 0) { 
		String sMsg = String("\r\nNo complete and valid control points found");
		//MessageBox(sMsg.c_str(), "Datum Transformation with 7 Parameters", MB_OK);
		return 0; // no valid active ctrl points
	}
	return iNrValidPts;
}

CoordCTS TableBursaWolf::ctsFindLocalCentroid(Array<LatLon> lalo,
										Array<double> rHeight, 
										CoordSystemViaLatLon& csvll, long iNrValidPts)
{
	Ellipsoid ell1 = csvll.ell;
	CoordCTS cts; // cartesian Geocentric coords
	LatLonHeight llh;
	CoordCTS ctsCentroid1 = CoordCTS(0,0,0);
  for ( long j = 0; j < iNrValidPts; j++) {
		llh = LatLonHeight(lalo[j], rHeight[j]);
		cts = ell1.ctsConv(llh);
		ctsCentroid1.x += cts.x;
		ctsCentroid1.y += cts.y;
		ctsCentroid1.z += cts.z;
	}
	ctsCentroid1.x /= iNrValidPts;
	ctsCentroid1.y /= iNrValidPts;
	ctsCentroid1.z /= iNrValidPts;
	return ctsCentroid1;
}


bool TableBursaWolf::fFreezing()
{
	long iValidPoints;
	iValidPoints = iCollectValidInputLatLonHeights();
	if (iValidPoints == 0)
		return false;
	else
		ctsCentroid1 = ctsFindLocalCentroid( lalo1, rHeight1,
											*csvll1, iValidPoints);
	
  ptr.iRecNew(iValidPoints);
	Init();  // initialize output columns

	CoordCTS ctsIn, ctsOut, ctsPivot;
		if (m_fUserPivot)
	{
		ctsPivot.x = m_rX0;
		ctsPivot.y = m_rY0;
		ctsPivot.z = m_rZ0;
	}
	else
	{
		ctsPivot.x = ctsCentroid1.x;
		ctsPivot.y = ctsCentroid1.y;
		ctsPivot.z = ctsCentroid1.z;
	}
	Coord crdInOut, crdOut;
	double rRadperArcSec = 4.8481368110953599358991410235795e-6;
	// user enters rotations in Arcsecs, 1 ArcSec ~ 4.848 microradians
	double xRot = m_rXrot * rRadperArcSec;
	double yRot = m_rYrot * rRadperArcSec; 
	double zRot = m_rZrot * rRadperArcSec; 
	// user enters scale diff in parts per million (1 mm error in length per 1 km)
	double rScD = m_rScaleDiff / 1000000; 
	for (long i = 0; i < iValidPoints; i++) 
	{
		LatLonHeight llhDatumShifted;
		LatLonHeight llhIn = LatLonHeight(lalo1[i], rHeight1[i]);
		ctsIn = ell1.ctsConv(llhIn);
		if (fProj1)
			crdInOut = cspr1->cConv(lalo1[i]);//(projected) 
		else
			crdInOut = Coord(lalo1[i].Lon, lalo1[i].Lat);//(latlons)
			//colCoordsInAndOut->PutVal(i+1, lalo1[i]);
		colCoordsInAndOut->PutVal(i+1, crdInOut);//put input crds in outp table 
    colHeightsInAndOut->PutVal(i+1, rHeight1[i]);

		colctsXCoordIn->PutVal(i+1, ctsIn.x);
		colctsYCoordIn->PutVal(i+1, ctsIn.y);
		colctsZCoordIn->PutVal(i+1, ctsIn.z);
		ctsOut = ell2.ctsConv(ctsIn, ctsPivot, m_rX, m_rY, m_rZ,
												xRot, yRot, zRot, rScD);  
		colctsXCoordOut->PutVal(i+1, ctsOut.x);
		colctsYCoordOut->PutVal(i+1, ctsOut.y);
		colctsZCoordOut->PutVal(i+1, ctsOut.z);
		
		llhDatumShifted = ell2.llhConv(ctsOut);
		crdOut = Coord(llhDatumShifted.Lon, llhDatumShifted.Lat);
		if (fProj2)
			crdOut = cspr2->cConv(llhDatumShifted);
		colCoordsOut->PutVal(i+1, crdOut);
    colHeightsOut->PutVal(i+1, llhDatumShifted.rHeight);
	}
  return true; 
}


void TableBursaWolf::Init()
{
  if (pts==0)
    return;
  //sFreezeTitle = "TableBursaWolf";
  //htpFreeze = htpTableBursaWolfT;

  // Copy of input coord column (only valid points)
	colCoordsInAndOut = pts->col(colCoordsIn->sName());
  if (!colCoordsInAndOut.fValid()) {
		Domain dmcrdIn;
		DomainCoord *dmc = colCoordsIn->dm()->pdcrd();
		if (!dmc)
			throw ErrorObject("Fatal: DomainCoord expected");
 		dmcrdIn.SetPointer(new DomainCoord(dmc->csyCoordSys()->fnObj));
    colCoordsInAndOut = pts->colNew(colCoordsIn->sName(), dmcrdIn);
  }
  colCoordsInAndOut->SetOwnedByTable(true);
  colCoordsInAndOut->SetReadOnly(true);

  // Copy of input height column (only valid points)
	String sInH = String("Height in ") & colCoordsIn->sName();
	colHeightsInAndOut = pts->col(sInH);
  if (!colHeightsInAndOut.fValid())
		colHeightsInAndOut = pts->colNew(sInH, Domain("value"), ValueRange(-1e5,1e5,0.001));
  colHeightsInAndOut->SetOwnedByTable(true);
  colHeightsInAndOut->SetReadOnly(true);
	
// First column
	colCoordsOut = pts->col(m_sCsyOut); 
  if (!colCoordsOut.fValid()) {
		Domain dmcrdOut;
 		dmcrdOut.SetPointer(new DomainCoord(m_CsyOut->fnObj));
    colCoordsOut = pts->colNew(m_sCsyOut, dmcrdOut);
  }
  colCoordsOut->SetOwnedByTable(true);
  colCoordsOut->SetReadOnly(true);

  // Second column
	String sOutH = String("Height in ") & m_sCsyOut;
	colHeightsOut = pts->col(sOutH);
  if (!colHeightsOut.fValid())
		colHeightsOut = pts->colNew(sOutH, Domain("value"), ValueRange(-1e5,1e5,0.001));
  colHeightsOut->SetOwnedByTable(true);
  colHeightsOut->SetReadOnly(true);

  // Third column
	 colctsXCoordIn = pts->col("ctsX1");
  if (!colctsXCoordIn.fValid())
		colctsXCoordIn = pts->colNew("ctsX1", Domain("value"), ValueRange(-1e7,1e7,0.001));
  colctsXCoordIn->SetOwnedByTable(true);
  colctsXCoordIn->SetReadOnly(true);

  // Fourth column
	colctsYCoordIn = pts->col("ctsY1");
  if (!colctsYCoordIn.fValid())
		colctsYCoordIn = pts->colNew("ctsY1", Domain("value"), ValueRange(-1e7,1e7,0.001));
  colctsYCoordIn->SetOwnedByTable(true);
  colctsYCoordIn->SetReadOnly(true);

  // Fifth column
	colctsZCoordIn = pts->col("ctsZ1");
  if (!colctsZCoordIn.fValid())
		colctsZCoordIn = pts->colNew("ctsZ1", Domain("value"), ValueRange(-1e7,1e7,0.001));
  colctsZCoordIn->SetOwnedByTable(true);
  colctsZCoordIn->SetReadOnly(true);

  // Sixth column
	colctsXCoordOut = pts->col("ctsX2");
  if (!colctsXCoordOut.fValid())
		colctsXCoordOut = pts->colNew("ctsX2", Domain("value"), ValueRange(-1e7,1e7,0.001));
  colctsXCoordOut->SetOwnedByTable(true);
  colctsXCoordOut->SetReadOnly(true);

  // Seventh column
	colctsYCoordOut = pts->col("ctsY2");
  if (!colctsYCoordOut.fValid())
		colctsYCoordOut = pts->colNew("ctsY2", Domain("value"), ValueRange(-1e7,1e7,0.001));
  colctsYCoordOut->SetOwnedByTable(true);
  colctsYCoordOut->SetReadOnly(true);

  // Last column
	colctsZCoordOut = pts->col("ctsZ2");
  if (!colctsZCoordOut.fValid())
		colctsZCoordOut = pts->colNew("ctsZ2", Domain("value"), ValueRange(-1e7,1e7,0.001));
  colctsZCoordOut->SetOwnedByTable(true);
  colctsZCoordOut->SetReadOnly(true);
}

void TableBursaWolf::UnFreeze()
{
  if (colCoordsOut.fValid()) {
    pts->RemoveCol(colCoordsOut);
    colCoordsOut = Column();
  }
  if (colHeightsOut.fValid()) {
    pts->RemoveCol(colHeightsOut);
    colHeightsOut = Column();
  }
  TableVirtual::UnFreeze();
}  

String TableBursaWolf::sExpression() const
{
	//FileName fnCS(m_sCsyOut);
	String sExpr("TableBursaWolf(%S,%S,%S,%.6f,%.6f,%.6f,%.8f,%.8f,%.8f,%.8f,%S,%.6f,%.6f,%.6f,%i)",
        m_TblIn->sName(true, fnObj.sPath()),
        colCoordsIn->sName(true, fnObj.sPath()),
        colHeightsIn->sName(true, fnObj.sPath()),
				m_rX, m_rY, m_rZ, 
				m_rXrot, m_rYrot, m_rZrot,			
				m_rScaleDiff, m_sCsyOut,
				m_rX0, m_rY0, m_rZ0,m_fUserPivot);  
	return sExpr;
}
