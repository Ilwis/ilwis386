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
/*
// $Log: /ILWIS 3.0/Table/Pattanal.cpp $
 * 
 * 10    18-09-00 19:50 Hendrikse
 * adde -1 in
 * String TablePattAnal::sRNN() const
 * to avoid non-computed record in case of less then 6 points in the map
 * 
 * 9     18-09-00 16:22 Hendrikse
 * In revision 8 the change of the offset into 1 was not OK, I put it back
 * to 0.
 * (compare to tblhist.cpp,  revisions 15 and 18 (resp 28--2-2000 and
 * 15-08-2000) dealing also with  Column::PutBufVal(....)
 * 
 * 8     28-02-00 12:25 Wind
 * adapted offset in call to PutBufVal
 * 
 * 7     20-01-00 4:39p Martin
 * MAXLIMIT is now respected when deleting records ( was ignored and
 * asumed array would handle it correctly)
 * 
 * 6     10-12-99 13:10 Wind
 * made internal arrays zero based
 * 
 * 5     7-12-99 15:51 Hendrikse
 * Added and used long iActualNrOfNeighbours = min(iNrPoints,
 * iNEIGHBOURS); at several places in order to avoid creation of
 * superfluous and even wrong columns
 * 
 * 4     9/24/99 10:39a Wind
 * replaced calls to static funcs ObjectInfo::ReadElement and WriteElement
 * by calls to member functions
 * 
 * 3     9/08/99 11:55a Wind
 * comments
 * 
 * 2     9/08/99 10:27a Wind
 * adpated to use of quoted file names in sExpression()
*/
// Revision 1.5  1998/09/16 17:25:32  Wim
// 22beta2
//
// Revision 1.4  1998/02/24 14:27:30  martin
// linefeed/ carriage return error in determining additional info.
//
// Revision 1.3  1997/08/27 15:56:19  Dick
// Removed one line less from table so first line is probabilty 0
// tables Prob1Pnt and so on shifted one line up (caused by start counting with 0
// or 1 in diffrent columns
// still to be done remove prob6pnt and so on if only 3,4,5,6,7 valid points
//
// Revision 1.2  1997/08/15 14:09:25  Dick
// *** empty log message ***
//
/*TablePattAnal
   Copyright Ilwis System Development ITC
   april 1995, by Jelle Wind
	Last change:  WK    1 Jul 98   10:01 am
*/
#define PATTANAL_C
#include "Applications\Table\PATTANAL.H"
#include "Engine\Table\tblstore.h"
#include "Engine\Table\COLSTORE.H"
#include "Engine\Base\Algorithm\Grouplim.h"
#include "Headers\Htp\Ilwisapp.htp"
#include "Headers\Err\Ilwisapp.err"
#include "Headers\Hs\tbl.hs"

/***************************************************************************/
/* A list is made which holds for each (valid) point a list of             */
/*  distances to the 6 (iNEIGHBOURS) nearest neighbours and their        */
/*  pointnumbers                                                           */
/* A so called distance related Hit list is made eg. for every distance    */
/*  an interval is determined and is counted in the Hit list               */
/* After this has been done for all points, hit lists are composed for     */
/*  the nearest second nearest and so on neighbours                        */
/***************************************************************************/


#define MAXLIMIT 150L     // Number of different distance classes
//#define iNEIGHBOURS 6   // Number of Neighbours of which distances
//                          // and point number are registrated for each point
// note if iNEIGHBOURS is changed number of elements in tabels rProbCsr and
// rGamma too must be changed

static double rProbCsr[] = { 0, 0.6215, 0.3291, 0.2431, 0.2015, 0.1760, 0.1582 };
// Table 4.1, page 70, Point Pattern Analysis, Boots & Getis 1988
static double rGamma[] = { 0, 0.5, 0.75, 0.9375, 1.0937, 1.2305, 1.3535 };
// Table 3.1, page 40, Point Pattern Analysis, Boots & Getis 1988

const char* TablePattAnal::sSyntax()
{
  return "TablePattAnal(pntmap)";
}

IlwisObjectPtr * createTablePattAnal(const FileName& fn, IlwisObjectPtr& ptr, const String& sExpr, vector<void *> parms ) {
	if ( sExpr != "")
		return (IlwisObjectPtr *)TablePattAnal::create(fn, (TablePtr &)ptr, sExpr);
	else
		return (IlwisObjectPtr *)new TablePattAnal(fn, (TablePtr &)ptr);
}

TablePattAnal* TablePattAnal::create(const FileName& fn, TablePtr& p, const String& sExpr)
{
  Array<String> as(1);
  if (!IlwisObjectPtr::fParseParm(sExpr, as))
    throw ErrorExpression(sExpr, sSyntax());
  PointMap pmp(as[0], fn.sPath());
  return new TablePattAnal(fn, p, pmp);
}

TablePattAnal::TablePattAnal(const FileName& fn, TablePtr& p)
: TableVirtual(fn, p)
{
  fNeedFreeze = true;
  ReadElement("TablePattAnal", "PointMap", pmp);
  objdep.Add(pmp.ptr());
  ptr.ReadElement("TablePattAnal", "ReflexiveArray", rReflexive);
  ptr.ReadElement("TablePattAnal", "ReflexiveCSRArray", rReflexiveCsr);
  ptr.ReadElement("TablePattAnal", "AvgDistArray", rAvgDist);
  ptr.ReadElement("TablePattAnal", "AvgDistCSRArray", rAvgDistCsr);
  Init();
}

TablePattAnal::TablePattAnal(const FileName& fn, TablePtr& p, const PointMap& pmap)
: TableVirtual(fn, p, true), //FileName(fn, ".TB#", true), Domain("none")),
  pmp(pmap)
{
  ptr.SetDomain("none");
  ptr.iRecNew(MAXLIMIT);
  fNeedFreeze = true;
  objdep.Add(pmp.ptr());
  if (!fnObj.fValid())
    objtime = objdep.tmNewest();
}

TablePattAnal::~TablePattAnal()
{
}

void TablePattAnal::Store()
{
  TableVirtual::Store();
  WriteElement("TableVirtual", "Type", "TablePattAnal");
  WriteElement("TablePattAnal", "PointMap", pmp);
  ptr.WriteElement("TablePattAnal", "ReflexiveArray", rReflexive);
  ptr.WriteElement("TablePattAnal", "ReflexiveCSRArray", rReflexiveCsr);
  ptr.WriteElement("TablePattAnal", "AvgDistArray", rAvgDist);
  ptr.WriteElement("TablePattAnal", "AvgDistCSRArray", rAvgDistCsr);
}


void TablePattAnal::DistPrep(long iNrPnt)
{
/***************************************************************************/
/* make the list which holds for each (valid) point a list of              */
/*  distances to the 6 (iNEIGHBOURS) nearest neighbours and their        */
/*  pointnumbers                                                           */
/* if no valid values point number is zero and distance is FLT_MAX         */
/***************************************************************************/
  int iLc;   // counter in List
  int iNc;   // "Neighbour " counter

  nn.Resize(iNrPnt);
  for (iLc = 0; iLc < iNrPnt; iLc++)
  {
    for (iNc = 0; iNc < iNEIGHBOURS; iNc++)
    {
      nn[iLc].iPntNr[iNc] = iUNDEF;
      nn[iLc].rDist[iNc] = FLT_MAX;
    }
  }
}

void TablePattAnal::DistInsert(long iPntNr, long iPntNrN, float rDist)
{
/***************************************************************************/
/* insert the distance and pointnumber in list                             */
/* iPntNr is actual point number, iPntNrN is point number "neighbour"      */
/***************************************************************************/

  int iNc;   // "Neighbour " counter
  // look if distance is greater than maximum distance already stored
  if (nn[iPntNr-1].rDist[iNEIGHBOURS-1]<rDist)
    return;
  // look for insertion place for this point
  for (iNc = iNEIGHBOURS - 1; iNc >= 0; iNc--)
    {
      if(nn[iPntNr-1].rDist[iNc] < rDist) break;
    }
  iNc++;
  // shift other neighbours one place to the right
  for (int j = iNEIGHBOURS-2; j >= iNc; j--)
    {
      nn[iPntNr-1].rDist[j+1] = nn[iPntNr-1].rDist[j];
      nn[iPntNr-1].iPntNr[j+1] = nn[iPntNr-1].iPntNr[j];
    }
  nn[iPntNr-1].rDist[iNc] = rDist;
  nn[iPntNr-1].iPntNr[iNc] = iPntNrN;
}


double TablePattAnal::rDistNeighb(long iPntNr,long iPntNrN)
/***************************************************************************/
/* Retrieve from the list for a given point (iPnrNr) the distance to       */
/*   the iPntNrN nearest neighbour                                         */
/* Return rUNDEF if that neighbour does not exist                          */
/***************************************************************************/
{
  float rt = (nn[iPntNr-1].rDist[iPntNrN-1]);
  if (rt == FLT_MAX)
    return rUNDEF;
  return rt;
}

bool TablePattAnal::fFreezing()
{
  Init();
  ptr.DeleteRec(iOffset(),iRecs());
  ptr.iRecNew(MAXLIMIT);
  long iNrPoints = pmp->iFeatures();
  long iPntNmbr, jPntNmbr;
  Coord crd, crdI, crdJ;
  int iNc, iLc;           // counter in List and neighbour counter
  // iHit contains the hits for the distance classes
//LongArray iHit(MAXLIMIT, 1);
  LongArray iHit(MAXLIMIT+1); // zero based now
  for (int iHitc = 1;iHitc <= MAXLIMIT; iHitc++)
    iHit[iHitc] = 0;

// build array indicating invalid points
//ArrayLarge<bool> fInvalid(iNrPoints, 1);
  ArrayLarge<bool> fInvalid(iNrPoints+1); // zero based now
  long iValidPoints = 0;
  for (iPntNmbr = 1; iPntNmbr <= iNrPoints; iPntNmbr++) {
    crd =pmp->cValue(iPntNmbr);
    bool f = crd.x == rUNDEF || crd.y == rUNDEF;
    fInvalid[iPntNmbr] = f;
    if (!f)
      iValidPoints++;
  }
  if (iValidPoints < 3)
    throw ErrorObject(WhatError(TR("More than two points needed"), errTablePattAnal), sTypeName());

  DistPrep(iNrPoints);   // create neighbours distance list
  // calculate bounding rectangle area
  Coord crdMin, crdMax;
  pmp->Bounds(crdMin, crdMax);
  double rDiffX=crdMax.x - crdMin.x;
  double rDiffY=crdMax.y - crdMin.y;
  double rArea = (crdMax.x - crdMin.x) * (crdMax.y - crdMin.y);
  double rMaxDiff = sqrt(rDiffX * rDiffX + rDiffY * rDiffY);
  GroupingLimits GrpLim (rMaxDiff, MAXLIMIT, 24);   //make list of distance limits
  // main computation loop
  trq.SetText(TR("Calculate"));
  for (iPntNmbr = 1; iPntNmbr <= iNrPoints; iPntNmbr++)
  {
    if (fInvalid[iPntNmbr])
      continue;
    if (trq.fUpdate(iPntNmbr, iValidPoints)) {
      nn.Resize(0);
      return false;
    }
    crdI = pmp->cValue(iPntNmbr);
    for (jPntNmbr = iPntNmbr + 1; jPntNmbr <= iNrPoints; jPntNmbr++)
    {
      if (fInvalid[jPntNmbr])
        continue;
      crdJ = pmp->cValue(jPntNmbr);
      double rDX = crdI.x - crdJ.x;
      double rDY = crdI.y - crdJ.y;
      float rDist = (float) sqrt(rDX * rDX + rDY * rDY);
      DistInsert(iPntNmbr, jPntNmbr, rDist);
      DistInsert(jPntNmbr, iPntNmbr, rDist);
      int ip = GrpLim.iGroup((double)rDist);
      if (ip <= MAXLIMIT) iHit[ip]++;
    }
  }

//Finalize
// iMaxLim is highist value for which a registration is found
  int iMaxLim = MAXLIMIT;
  while (iHit[iMaxLim] == 0)
    iMaxLim--;
  for( int iHitc = 2; iHitc <= iMaxLim; iHitc++)
    iHit[iHitc] += iHit[iHitc-1];

  // make list for distances between all neighbours
  double rcvalreg = 0.5 * (double) iValidPoints * ((double) iValidPoints - 1.0);
  for (int iHitc = 1; iHitc <= iMaxLim; iHitc++) {
    colDist->PutVal(iHitc, GrpLim.rValue(iHitc));
    colProbAllPnt->PutVal(iHitc, (double)iHit[iHitc] / rcvalreg);
  }  
// make distance hit list for first, second and so on neighbours
  RealBuf abufProbPnt[iNEIGHBOURS+1];
  for (iNc = 1; iNc <= iNEIGHBOURS ; iNc++)
    abufProbPnt[iNc].Size(iMaxLim+1);

  for (iLc = 1; iLc <= iMaxLim; iLc++)
    for (iNc = 1; iNc <= iNEIGHBOURS; iNc++)
      abufProbPnt[iNc][iLc] = 0.0;

  for (iPntNmbr = 1; iPntNmbr <= iNrPoints; iPntNmbr++)
  {
    if (fInvalid[iPntNmbr])
      continue;
    for (iNc = 1; iNc <= iNEIGHBOURS; iNc++)
    {
      long ip = GrpLim.iGroup(rDistNeighb(iPntNmbr, iNc));
      if (ip != iUNDEF && iNc < iNrPoints) // add one
        abufProbPnt[iNc][ip] += 1;
    }
  }

  for (iLc = 2; iLc <= iMaxLim; iLc++)
    for (iNc = 1;iNc <= iNEIGHBOURS; iNc++)
      abufProbPnt[iNc][iLc] += abufProbPnt[iNc][iLc-1];

  for (iLc = 1; iLc <= iMaxLim; iLc++)
    for (iNc = 1; iNc <= iNEIGHBOURS; iNc++)
      abufProbPnt[iNc][iLc] /= iValidPoints;

/*rReflexive.Resize(iNEIGHBOURS, 1);
  rReflexiveCsr.Resize(iNEIGHBOURS, 1);
  rAvgDist.Resize(iNEIGHBOURS, 1);
  rAvgDistCsr.Resize(iNEIGHBOURS, 1);*/
  rReflexive.Resize(iNEIGHBOURS+1);   // zero based
  rReflexiveCsr.Resize(iNEIGHBOURS+1);
  rAvgDist.Resize(iNEIGHBOURS+1);
  rAvgDistCsr.Resize(iNEIGHBOURS+1);

  for (iNc = 1; iNc <= iNEIGHBOURS; iNc++) {
    rReflexive[iNc] = 0.0;
    rReflexiveCsr[iNc] = 0.0;
    rAvgDist[iNc] = 0.0;
    rAvgDistCsr[iNc] = 0.0;
  }

// Reflexive
  for (iNc = 1; iNc <= iNEIGHBOURS; iNc++) {
    for (iPntNmbr = 1; iPntNmbr <= iNrPoints; iPntNmbr++) {
      if (fInvalid[iPntNmbr])
        continue;
      long k = iDistPntNr(iPntNmbr, iNc);
      if (iDistPntNr(k, iNc) == iPntNmbr)
          rReflexive[iNc] += 1.0;
    }
  }

// Average distance
  for (iNc = 1; iNc <= iNEIGHBOURS; iNc++) {
    for (iPntNmbr = 1; iPntNmbr <= iNrPoints; iPntNmbr++) {
      if (fInvalid[iPntNmbr])
        continue;
      rAvgDist[iNc] += rDistNeighb(iPntNmbr, iNc);
    }
    rAvgDist[iNc] /= iValidPoints;
  }

// compute reflexive and average distance compared to random

  for (iNc = 1; iNc <= iNEIGHBOURS; iNc++) {
    rReflexiveCsr[iNc] = rProbCsr[iNc] * iNrPoints;
    rAvgDistCsr[iNc] = rGamma[iNc] * sqrt(rArea / iNrPoints);
  }
  long iActualNrOfNeighbours = min(iNrPoints, iNEIGHBOURS);
  for (iNc = 1; iNc <= iActualNrOfNeighbours; iNc++)
    colProbPnt[iNc]->PutBufVal(abufProbPnt[iNc], 0);   //revision 1.3 put to 0; // because this columns start with 0 others with 1
    // In 3.0 (jelle): Put it back to 1, because implementation takes care of offset now
                              

  // delete non significant records
  int iDelRec = iMaxLim + 1 < MAXLIMIT ? MAXLIMIT - iMaxLim : 0;
  ptr.DeleteRec(iMaxLim + 1,  iDelRec );
  if (iMaxLim > 10)  {
    for (iLc = 1; iLc < (iMaxLim - 10); iLc++)
      if (colProbAllPnt->rValue(iLc+1) != 0.0) break;    // revision 1.3 put to +1
                                       // to keep one line with zero in the table
    ptr.DeleteRec(1, iLc - 1);
  }

//  _iOffset = ptr->iOffset();
//  _iRecs = ptr->iRecs();

  nn.Resize(0);
//  fChanged = true;
//  Store();
  ptr.SetAdditionalInfoFlag(true);
  ptr.SetAdditionalInfo(sRNN());
  return true;
}

void TablePattAnal::Init()
{
	long iNrPoints = pmp->iFeatures();
  sFreezeTitle = "TablePattAnal";
  htpFreeze = "ilwisapp\\pattern_analysis_algorithm.htm";
  if (pts==0)
    return;
//  Time tim = objtime;
  colDist = pts->col("Distance");
  if (!colDist.fValid()) {
    colDist = pts->colNew("Distance", Domain("distance"));
    colDist->sDescription = "Distance between points";
  }
  colDist->SetOwnedByTable(true);
  colDist->SetReadOnly(true);
  colProbAllPnt = pts->col("ProbAllPnt");
  if (!colProbAllPnt.fValid()) {
    colProbAllPnt = pts->colNew("ProbAllPnt", DomainValueRangeStruct(0.0, 1.0, 0.0001));
    colProbAllPnt->sDescription = "Total probability";
  }
  colProbAllPnt->SetOwnedByTable(true);
  colProbAllPnt->SetReadOnly(true);
	long iActualNrOfNeighbours = min(iNrPoints, iNEIGHBOURS);
//  colProbPnt.Resize(iActualNrOfNeighbours, 1);  // offset = 1
  colProbPnt.Resize(iActualNrOfNeighbours+1);  // zero based now
  for (int i=1; i <= iActualNrOfNeighbours; i++) {
    String sColName = String("Prob%iPnt", i);
    colProbPnt[i] = pts->col(sColName);
    if (!colProbPnt[i].fValid()) {
      colProbPnt[i] = pts->colNew(sColName, DomainValueRangeStruct(0.0, 1.0, 0.0001));
      colProbPnt[i]->sDescription = String("Probability for %i neighbours", i);
    }
    colProbPnt[i]->SetOwnedByTable(true);
    colProbPnt[i]->SetReadOnly(true);
  }
}

void TablePattAnal::UnFreeze()
{
  if (colDist.fValid()) {
    pts->RemoveCol(colDist);
    colDist = Column();
  }
  if (colProbAllPnt.fValid()) {
    pts->RemoveCol(colProbAllPnt);
    colProbAllPnt = Column();
  }
	long iActualNrOfNeighbours = min(pmp->iFeatures(), iNEIGHBOURS);
  for (int i=1; i <= iActualNrOfNeighbours; i++)
	{
		if (colProbPnt.iSize() > 0 )
		{
			if (colProbPnt[i].fValid()) {
				pts->RemoveCol(colProbPnt[i]);
				colProbPnt[i] = Column();
			}
    }
	}
//colProbPnt.Resize(0,1);
  colProbPnt.Reset(); // zero based now
  TableVirtual::UnFreeze();

}

String TablePattAnal::sExpression() const
{
  return String("TablePattAnal(%S)",
                pmp->sNameQuoted(true, fnObj.sPath()));
}

String TablePattAnal::sRNN() const
{ 
	long iActualNrOfNeighbours = min(pmp->iFeatures() - 1, iNEIGHBOURS);
  String s("Reflexive Nearest Neighbours\r\n\r\n");
  s &= String("Order  Observed values    Assumed with CSR\r\n");
  s &= String("==========================================\r\n");
  for (int i = 1; i <= iActualNrOfNeighbours; ++i ) 
    s &= String("%4i        %6.2f          %6.2f\r\n", i, rReflexive[i], rReflexiveCsr[i]);
  s &= String("\r\nDistance to Nearest Neighbours\r\n\r\n");
  s &= String("Order  Observed values    Assumed with CSR\r\n");
  s &= String("==========================================\r\n");
  for (int i = 1; i <= iActualNrOfNeighbours; ++i ) 
    s &= String("%4i    %10.2f      %10.2f\r\n", i, rAvgDist[i], rAvgDistCsr[i]);
  return s;
}




