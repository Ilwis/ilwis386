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
   january 1999, by Jan Hendrikse
	Last change:  WK    5 Jul 99   12:26 pm
*/

#include "Headers\toolspch.h"
#include "Applications\Table\Tblcrvar.h"
#include "Engine\Table\tblstore.h"
#include "Engine\Table\COLSTORE.H"
//#include <float.h>
#include "Headers\Htp\Ilwisapp.htp"
#include "Headers\Err\Ilwisapp.err"
#include "Headers\Hs\tbl.hs"
#include "Headers\Hs\Table.hs"

IlwisObjectPtr * createTableCrossVarioGram(const FileName& fn, IlwisObjectPtr& ptr, const String& sExpr, vector<void *> parms ) {
	if ( sExpr != "")
		return (IlwisObjectPtr *)TableCrossVarioGram::create(fn, (TablePtr &)ptr, sExpr);
	else
		return (IlwisObjectPtr *)new TableCrossVarioGram(fn, (TablePtr &)ptr);
}

const char* TableCrossVarioGram::sSyntax()
{
  return "TableCrossVarioGram(pntmap, colA, colB, LagSpacing[,plane|sphere])";
}

static void AttrTableNotFound(const FileName& fn) {
  throw ErrorObject(WhatError(String(STBLErrAttrTableNotFound), errTableCrossVar), fn);
}
static void ColumnNotFound(const FileName& fn) {
  throw ErrorObject(WhatError(String(STBLErrAttrColNotFound), errTableCrossVar +1), fn);
}

TableCrossVarioGram* TableCrossVarioGram::create(const FileName& fn, TablePtr& p,
                const String& sExpr)
{
  Array<String> as;
  bool ferror = true;
  double rLagSp = 1;
  long iNrDistCl = 10L;
  
  int iParms = IlwisObjectPtr::iParseParm(sExpr, as);
  if (iParms < 4 || iParms > 5)
    throw ErrorExpression(sExpr, sSyntax()); 
  PointMap pm(as[0], fn.sPath());
  Table tbl = pm->tblAtt(); 
  if (!tbl.fValid())
    AttrTableNotFound(fn);
  Column clA = tbl->col(as[1]);
  if (!clA.fValid())
    ColumnNotFound(tbl->fnObj);
  Column clB = tbl->col(as[2]);
  if (!clB.fValid())
    ColumnNotFound(tbl->fnObj);
  rLagSp = as[3].rVal();
  if (rLagSp <= 0) 
    throw ErrorObject(WhatError(String(STBLErrLagSpacingNotPos), errTableCrossVar +2), fn);
	bool fSphericDist = false; //default
	if (iParms == 5) { 
		if (fCIStrEqual("plane", as[4]))
			fSphericDist = false;
		else if (fCIStrEqual("sphere", as[4]))
			fSphericDist = true;
		else 
			throw ErrorExpression(sExpr, sSyntax());
	}
  Coord crdMin, crdMax; 
  pm->Bounds(crdMin, crdMax);
	CoordSystemLatLon* csllIn = pm->cs()->pcsLatLon();
	if (0 != csllIn) {			//pointmap has latlon coords
		crdMin.x *= 111111.1; //make degrees metric (1 deg ~ 111km or less)
		crdMin.y *= 111111.1;
		crdMax.x *= 111111.1;
		crdMax.y *= 111111.1;
	}
  double rDiffX = crdMax.x - crdMin.x;
  double rDiffY = crdMax.y - crdMin.y;
  double rMaxDiff = sqrt(rDiffX * rDiffX + rDiffY * rDiffY); 
  
  iNrDistCl = 1 + (long)(rMaxDiff / rLagSp);
  iNrDistCl = min (iNrDistCl,100);
    
  return new TableCrossVarioGram(fn, p, pm, clA, clB, rLagSp, iNrDistCl, fSphericDist);
}

TableCrossVarioGram::TableCrossVarioGram(const FileName& fn, TablePtr& p)
: TableVirtual(fn, p)
{
  fNeedFreeze = true;
  ReadElement("TableCrossVarioGram", "PointMap", pmp);
  //ReadElement("TableCrossVarioGram", "PointMapB", pmpB);
  Table tbl= pmp->tblAtt(); 
	tbl->LoadData();
  if (!tbl.fValid())
    AttrTableNotFound(fn);
  String sColA, sColB;
  ReadElement("TableCrossVarioGram", "AttribColA", sColA);
  colA = tbl->col(sColA);
  if (!colA.fValid())
    ColumnNotFound(tbl->fnObj);
  ReadElement("TableCrossVarioGram", "AttribColB", sColB);
  colB = tbl->col(sColB);
  if (!colB.fValid())
    ColumnNotFound(tbl->fnObj);    
  //ReadElement("TableCrossVarioGram", "AttribColA", colA);
  //ReadElement("TableCrossVarioGram", "AttribColB", colB);
	m_distMeth = Distance::distPLANE;	//default
  String sDistanceMethod;
  ReadElement("TableCrossVarioGram", "DistanceMethod",sDistanceMethod);  
  if (fCIStrEqual("sphere", sDistanceMethod)) 
    m_distMeth = Distance::distSPHERE;
	else
		m_distMeth = Distance::distPLANE;
	ReadElement("TableCrossVarioGram", "LagSpacing", rLagSpacing);
  ReadElement("TableCrossVarioGram", "NrDistClasses", iNrDistClasses);
  objdep.Add(pmp.ptr());
  //objdep.Add(pmpB.ptr());
  Init();
}

TableCrossVarioGram::TableCrossVarioGram(const FileName& fn, TablePtr& p, 
                                         const PointMap& pmap,
                                         const Column& clA, const Column& clB,
                                         double rLagSp, long iNrDistCl,
                                         bool fSphericDist)
: TableVirtual(fn, p, true), //FileName(fn, ".TB#", true), Domain("none")),  
  pmp(pmap),
  colA(clA),
  colB(clB),
  rLagSpacing(rLagSp),
  iNrDistClasses(iNrDistCl)
{
  Table tblAttri = pmp->tblAtt();  
  if (!tblAttri.fValid())
    AttrTableNotFound(fn);
  if (!colA.fValid())
    ColumnNotFound(tblAttri->fnObj);
  if (!colB.fValid())
    ColumnNotFound(tblAttri->fnObj);  
  ptr.SetDomain(Domain("none"));
  ptr.iRecNew(iNrDistClasses);   
  if (colA->dm()->pdv() == 0)
    throw ErrorValueDomain(colA->dm()->sName(true, fnObj.sPath()), colA->sTypeName(), 
                          errTableCrossVar +2);
  if (colB->dm()->pdv() == 0)
    throw ErrorValueDomain(colB->dm()->sName(true, fnObj.sPath()), colB->sTypeName(), 
                          errTableCrossVar +3);                     
	if (fSphericDist)
		m_distMeth = Distance::distSPHERE;
	else
		m_distMeth = Distance::distPLANE;
	fNeedFreeze = true;
  objdep.Add(pmp.ptr());
  objdep.Add(colA.ptr());
  objdep.Add(colB.ptr());
  if (!fnObj.fValid())
    objtime = objdep.tmNewest();
  Init();
}

TableCrossVarioGram::~TableCrossVarioGram()
{
}

void TableCrossVarioGram::Store()
{
  TableVirtual::Store();
  WriteElement("TableVirtual", "Type", "TableCrossVarioGram");
  WriteElement("TableCrossVarioGram", "PointMap", pmp);
  WriteElement("TableCrossVarioGram", "AttribColA", colA);
  WriteElement("TableCrossVarioGram", "AttribColB", colB);
  WriteElement("TableCrossVarioGram", "LagSpacing", rLagSpacing);
  WriteElement("TableCrossVarioGram", "NrDistClasses", iNrDistClasses);
	String sDistanceMethod;
	switch (m_distMeth) {
    case Distance::distPLANE: sDistanceMethod = "plane"; break; 
    case Distance::distSPHERE: sDistanceMethod = "sphere"; break; 
  } 
	WriteElement("TableCrossVarioGram", "DistanceMethod", sDistanceMethod);
}

String TableCrossVarioGram::sAddInfo() const
{
  String s = "Average over all data of attribute A\r\n(mean of all sampled observations)";
  s &= String("\r\n  = %.3f (in units of A)", rDataMeanA);
  s &= "\r\nVariance over all data of attribute A\r\n(mean of all squared deviations from the average)";
  s &= String("\r\n  = %.3f (in squared A units)", rDataVarianceA);
  s &= "\r\nAverage over all data of attribute B\r\n(mean of all sampled observations)";
  s &= String("\r\n  = %.3f (in units of B)", rDataMeanB);
  s &= "\r\nVariance over all data of attribute B\r\n(mean of all squared deviations from the average)";
  s &= String("\r\n  = %.3f (in squared B units)", rDataVarianceB);
  s &= "\r\nCrossVariance over all data shared by A and B\r\n(mean of all cross-multiplied deviations from the average)";
  s &= String("\r\n  = %.3f (in mixed 'AxB' units)", rDataCoVarianceAB);
	/* String s = String(STBInfDataMeanA_lf.scVal(), rDataMeanA);
	s &= String(STBInfDataVarianceA_lf.scVal(), rDataVarianceA);
	s &= String(STBInfDataMeanB_lf.scVal(), rDataMeanB);
	s &= String(STBInfDataVarianceB_lf.scVal(), rDataVarianceB);
	s &= String(STBInfDataCoVarianceAB_lf.scVal(), rDataCoVarianceAB); */
  return s;
}

int TableCrossVarioGram::iGroup(double rD)
// look with given distance rD for place in interval table
{
  int il,im,ih;
  il = 1; ih = iNrDistClasses;
  if (rD > rDistLimits[ih])
    return (iNrDistClasses+1);
  if (rD < rDistLimits[il])
    return (il);
  while (ih-il > 1) {
    im = (il+ih) / 2;
    if (rD > rDistLimits[im])
      il = im;
    else
      ih = im;
  }
  return ih;
}


bool TableCrossVarioGram::fComputeSemiAndCrossVar()

{  
  long iNrPoints = pmp->iFeatures(); // predictand variable A
  //                           // co-variable B
  long i;
  RealArray rDistAvg(1);// 
  LongArray iNrObservA(1);   //A, B AB only
  RealArray rSemValueA(1);   //in omnidirect case (isotropy)
  LongArray iNrObservB(1);
  RealArray rSemValueB(1);   //
  LongArray iNrObservAB(1);
  RealArray rCrossValueAB(1);// 

  
  rDistAvg.Resize(iNrDistClasses+1);
  iNrObservA.Resize(iNrDistClasses+1);     // Number of observations
  rSemValueA.Resize(iNrDistClasses+1);     // Semivariogram values
  iNrObservB.Resize(iNrDistClasses+1);
  rSemValueB.Resize(iNrDistClasses+1);
  iNrObservAB.Resize(iNrDistClasses+1);
  rCrossValueAB.Resize(iNrDistClasses+1); 
  

  // Offset parameter removed from Resize functions
  for (i = 1; i <= iNrDistClasses; i++)  {
    rDistAvg[i] = 0.0;
    iNrObservA[i] = 0;
    rSemValueA[i] = 0.0;
    iNrObservB[i] = 0;
    rSemValueB[i] = 0.0;
    iNrObservAB[i] = 0;
    rCrossValueAB[i] = 0.0;
  }  

	Distance dis(pmp->cs(), m_distMeth);
  double rDifA, rDifB, rDifAB, rDis;
	
	iValidPointsA = 0;
  iValidPointsB = 0;
  iValidPointsBothAB = 0;
  double rDeviASquared = 0.0;
  double rDeviBSquared = 0.0;
  double rDeviABCrossed = 0.0;
  Coord cPoint_i, cPoint_j;
  trq.SetTitle(STBLTitleCrossVariogram);
  trq.SetText(STBLCalculate); 
  for (i = 1; i <= iNrPoints; i++) {  // collecting squared differences
    if (trq.fUpdate(i, iNrPoints))    // of variable 
      return false;
    long iRaw = pmp->iRaw(i);
		cPoint_i = pmp->cValue(i);
    for (long j = i+1; j <= iNrPoints; j++) {
			if (trq.fAborted())
        return false;
			cPoint_j = pmp->cValue(j);
			rDis = dis.rDistance(cPoint_i, cPoint_j);
      int iGrpNr = iGroup(rDis);
      if (iGrpNr > iNrDistClasses)
        continue;
      long jRaw = pmp->iRaw(j);  
      if (!fInvalidA[i] && !fInvalidA[j]) {
        rDifA = (colA->rValue(iRaw))-(colA->rValue(jRaw));
        iNrObservA[iGrpNr]++;
        rSemValueA[iGrpNr] += (rDifA * rDifA)/2.0;  
      }
      if (!fInvalidB[i] && !fInvalidB[j]){
        rDifB = (colB->rValue(iRaw))-(colB->rValue(jRaw));
        iNrObservB[iGrpNr]++;
        rSemValueB[iGrpNr] += (rDifB * rDifB)/2.0;  
      }
      if (!fInvalidA[i] && !fInvalidA[j]
           && !fInvalidB[i] && !fInvalidB[j]) {
        rDifAB = rDifA * rDifB;
        rDistAvg[iGrpNr] += rDis;  // for 'weighted' abscissa  
        iNrObservAB[iGrpNr]++;
        rCrossValueAB[iGrpNr] +=  rDifAB/2.0;                
      }
    }
// find variance and covariance over all data:
    if (!fInvalidA[i]) {
      iValidPointsA++;
      rDeviASquared += (colA->rValue(iRaw) - rDataMeanA)*(colA->rValue(iRaw) - rDataMeanA);
    }
    if (!fInvalidB[i]) {
      iValidPointsB++;
      rDeviBSquared += (colB->rValue(iRaw) - rDataMeanB)*(colB->rValue(iRaw) - rDataMeanB);
    }
    if (!fInvalidA[i] && !fInvalidB[i]) {
      iValidPointsBothAB++;
      rDeviABCrossed += (colA->rValue(iRaw) - rDataMeanA)*(colB->rValue(iRaw) - rDataMeanB);
    }
  }
  rDataVarianceA = rUNDEF;
  rDataVarianceB = rUNDEF;
  rDataCoVarianceAB = rUNDEF;
  if (iValidPointsA)
    rDataVarianceA = rDeviASquared / iValidPointsA;
  if (iValidPointsB)
    rDataVarianceB = rDeviBSquared / iValidPointsB;
  if (iValidPointsBothAB)
    rDataCoVarianceAB = rDeviABCrossed / iValidPointsBothAB;
   // find distance average per lag (== per distclass) only for set AB:
  for (int iGrpNr = 1; iGrpNr <= iNrDistClasses; iGrpNr++) {
    if (iNrObservAB[iGrpNr] != 0)
      rDistAvg[iGrpNr] /= (double)iNrObservAB[iGrpNr];
    else 
      rDistAvg[iGrpNr] = rUNDEF;
  }
  // write data in table
  for (int iRec = 1; iRec <= iNrDistClasses; iRec++) {
    colDist->PutVal(iRec, rDistLimits[iRec]- rLagSpacing/2.0);
    colDistAvg->PutVal(iRec, rDistAvg[iRec]);
    colPairsA->PutVal(iRec, iNrObservA[iRec]);
    colPairsB->PutVal(iRec, iNrObservB[iRec]);
    colPairsAB->PutVal(iRec, iNrObservAB[iRec]);
    if (iNrObservA[iRec] > 0)                                     //should set A == set AB ???
      colSemA->PutVal(iRec, rSemValueA[iRec] / iNrObservA[iRec]);
    if (iNrObservB[iRec] > 0)     
      colSemB->PutVal(iRec, rSemValueB[iRec] / iNrObservB[iRec]);
    if (iNrObservAB[iRec] > 0)     
      colCross->PutVal(iRec, rCrossValueAB[iRec] / iNrObservAB[iRec]);
  }
	ptr.SetAdditionalInfoFlag(true);
  ptr.SetAdditionalInfo(sAddInfo());
  return true;
}


bool TableCrossVarioGram::fFreezing()
{
  ptr.DeleteRec(iOffset(),iRecs());
  ptr.iRecNew(iNrDistClasses);
  Init();

  long iNrPoints = pmp->iFeatures();      
  
  if (iNrPoints < 3 )
    throw ErrorObject(WhatError(STBLErrTooFewPoints, errTableCrossVar+4),
                      sTypeName());
  long iPnt;
  double rZ;
  Coord crd;
	
	rDataMeanA = 0.0;
  fInvalidA.Resize(iNrPoints+1);
  long iValidPointsA = 0;
  for (iPnt = 1; iPnt <= iNrPoints; iPnt++) {
    long iRaw = pmp->iRaw(iPnt);
    crd = pmp->cValue(iPnt);
    rZ = colA->rValue(iRaw);
    fInvalidA[iPnt] = crd.x  == rUNDEF || crd.y == rUNDEF || rZ == rUNDEF;
    if (fInvalidA[iPnt])
      continue;
		rDataMeanA += rZ;
    iValidPointsA++;
      // look for domain of X, Y and attribute
  }
  if (iValidPointsA < 3)
    throw ErrorObject(WhatError(STBLErrTooFewValidPoints, errTableCrossVar+5),
                      sTypeName());
	rDataMeanA /= iValidPointsA;

  rDataMeanB = 0.0;
  fInvalidB.Resize(iNrPoints+1);
  long iValidPointsB = 0;
  for (iPnt = 1; iPnt <= iNrPoints; iPnt++) {
    long iRaw = pmp->iRaw(iPnt);
    crd = pmp->cValue(iPnt);
    rZ = colB->rValue(iRaw);
    fInvalidB[iPnt] = crd.x  == rUNDEF || crd.y == rUNDEF || rZ == rUNDEF;
    if (fInvalidB[iPnt])
      continue;
		rDataMeanB += rZ;
    iValidPointsB++;
      // look for domain of X, Y and attribute
  }
  if (iValidPointsB < 3)
    throw ErrorObject(WhatError(STBLErrTooFewValidPoints, errTableCrossVar+6),
                      sTypeName());   
	rDataMeanB /= iValidPointsB; 

  // rDistLimits contains limit values for grouping in classes
  rDistLimits.Resize(iNrDistClasses+2);
  for (long i=0; i <= iNrDistClasses;i++)
      rDistLimits[i+1] = (double) i * rLagSpacing + rLagSpacing/2.0;                    
  //bool fInterrupted;  // call variogram if appropriate and check if stop pressed
  return  fComputeSemiAndCrossVar();
  //if (!fInterrupted)
  //  return(false);
  //return true;
}


void TableCrossVarioGram::Init()
{
  sFreezeTitle = "TableCrossVarioGram";
  htpFreeze = htpTableCrossVariogramT;
  String sDescA = String("Predictand_Predictand");
  String sDescB = String("Covariable_Covariable");
  String sDescAB = String(": Predictand_Covariable");
  if (pts==0)
    return;
  colDist = pts->col("Distance");
  if (!colDist.fValid()) {
    colDist = pts->colNew("Distance", Domain("distance"));
    colDist->sDescription = STBLMsgPointDistance;
  }
  colDist->SetOwnedByTable(true);
  colDist->SetReadOnly(true);

  colDistAvg = pts->col("AvgLag");
  if (!colDistAvg.fValid()) {
    colDistAvg = pts->colNew("AvgLag", Domain("distance"));
    colDistAvg->sDescription = STBLMsgAvgPointDistanceAll & sDescAB;
  }
  colDistAvg->SetOwnedByTable(true);
  colDistAvg->SetReadOnly(true);

  colPairsA = pts->col("NrPairsA");
  if (!colPairsA.fValid()) {
    colPairsA = pts->colNew("NrPairsA", Domain("count"));
    colPairsA->sDescription = String(STBLMsgNumberPointPairs_S.scVal(), sDescA);
  }
  colPairsA->SetOwnedByTable(true);
  colPairsA->SetReadOnly(true);

  colPairsB = pts->col("NrPairsB");
  if (!colPairsB.fValid()) {
    colPairsB = pts->colNew("NrPairsB", Domain("count"));
    colPairsB->sDescription = String(STBLMsgNumberPointPairs_S.scVal(), sDescB);
  }
  colPairsB->SetOwnedByTable(true);
  colPairsB->SetReadOnly(true);

  colPairsAB = pts->col("NrPairsAB");
  if (!colPairsAB.fValid()) {
    colPairsAB = pts->colNew("NrPairsAB", Domain("count"));
    colPairsAB->sDescription = String(STBLMsgNumberPointPairs_S.scVal(), sDescAB);
  }
  colPairsB->SetOwnedByTable(true);
  colPairsB->SetReadOnly(true);

  colSemA = pts->col("SemiVarA");
  if (!colSemA.fValid()) {
    colSemA = pts->colNew("SemiVarA", Domain("value"), ValueRange(0,1e10,0.01));
    colSemA->sDescription = String(STBLMsgSemiVariogram_S.scVal(), sDescA);
  }
  colSemA->SetOwnedByTable(true);
  colSemA->SetReadOnly(true);

  colSemB = pts->col("SemiVarB");
  if (!colSemB.fValid()) {
    colSemB = pts->colNew("SemiVarB", Domain("value"), ValueRange(0,1e10,0.01));
    colSemB->sDescription = String(STBLMsgSemiVariogram_S.scVal(), sDescB);
  }
  colSemB->SetOwnedByTable(true);
  colSemB->SetReadOnly(true);

  colCross = pts->col("CrossVarAB");
  if (!colCross.fValid()) {
    colCross = pts->colNew("CrossVarAB", Domain("value"), ValueRange(-1e10,1e10,0.01));
    colCross->sDescription = String(STBLMsgCrossVariogram_S.scVal(), sDescAB);
  }
  colCross->SetOwnedByTable(true);
  colCross->SetReadOnly(true);
}

void TableCrossVarioGram::UnFreeze()
{
  if (colDist.fValid()) {
    pts->RemoveCol(colDist);
    colDist = Column();
  }
  if (colDistAvg.fValid()) {
    pts->RemoveCol(colDistAvg);
    colDistAvg = Column();
  }
  if (colPairsA.fValid()) {
    pts->RemoveCol(colPairsA);
    colPairsA = Column();
  }  
  if (colSemA.fValid()) {
    pts->RemoveCol(colSemA);
    colSemA = Column();
  }
  if (colPairsB.fValid()) {
    pts->RemoveCol(colPairsB);
    colPairsB = Column();
  }  
  if (colSemB.fValid()) {
    pts->RemoveCol(colSemB);
    colSemB = Column();
  }
  if (colPairsAB.fValid()) {
    pts->RemoveCol(colPairsAB);
    colPairsAB = Column();
  }  
  if (colCross.fValid()) {
    pts->RemoveCol(colCross);
    colCross = Column();  
  }
  TableVirtual::UnFreeze();
}  

String TableCrossVarioGram::sExpression() const
{
	String sDistanceMethod;
	if (m_distMeth == Distance::distSPHERE)
		sDistanceMethod = String("sphere");
	else 
		sDistanceMethod = String("plane");
  return String("TableCrossVarioGram(%S,%S,%S,%.2f,%S)",
        pmp->sName(true, fnObj.sPath()),
        colA->sName(true, fnObj.sPath()),
        colB->sName(true, fnObj.sPath()),
        rLagSpacing,
				sDistanceMethod);  
}
