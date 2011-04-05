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
/* PolygonMapFromRas
   Copyright Ilwis System Development ITC
   nov. 1996, by Jelle Wind
	Last change:  JEL   7 May 97    9:41 am
*/                                                                      
#define ILWPOLFRMRAS_C

#include "geos/operation/polygonize/Polygonizer.h"
#include "PolygonApplications\POLFRMRS.H"
#include "Engine\Base\DataObjects\valrange.h"
#include "Engine\Domain\dmsort.h"
#include "Engine\Applications\MAPVIRT.H"
#include "Headers\Err\Ilwisapp.err"
#include "Headers\Htp\Ilwisapp.htp"
#include "Headers\Hs\polygon.hs"

IlwisObjectPtr * createPolygonMapFromRas(const FileName& fn, IlwisObjectPtr& ptr, const String& sExpr, vector<void *> parms ) {
	if ( sExpr != "")
		return (IlwisObjectPtr *)PolygonMapFromRas::create(fn, (PolygonMapPtr &)ptr, sExpr);
	else
		return (IlwisObjectPtr *)new PolygonMapFromRas(fn, (PolygonMapPtr &)ptr);
}

static void TooManySegments(const FileName& fnObj)
{
  throw ErrorObject(WhatError(SPOLErrTooManySegments, errPolygonMapFromRas), fnObj);
}

const char* PolygonMapFromRas::sSyntax() {
  return "PolygonMapFromRas(rasmap,8|4,smooth|nosmooth)";
}

PolygonMapFromRas* PolygonMapFromRas::create(const FileName& fn, PolygonMapPtr& p, const String& sExpr)
{
  Array<String> as;
  int iParms = IlwisObjectPtr::iParseParm(sExpr, as);
  if ((iParms < 1) || (iParms > 3))
    ExpressionError(sExpr, sSyntax());
  Map map(as[0], fn.sPath());
  bool f8Con = true;
  bool fSmooth = true;
  if (iParms > 1) {
    String s8Con = as[1];
    if ((s8Con != "4") && (s8Con != "8"))
       ExpressionError(sExpr, sSyntax());
    f8Con = s8Con == "8";
  }
  if (iParms == 3) {
    String sSmooth = as[2];
    if (!fCIStrEqual(sSmooth, "smooth") && !fCIStrEqual(sSmooth, "nosmooth"))
      ExpressionError(sExpr, sSyntax());
    fSmooth = fCIStrEqual(sSmooth,"smooth");
  }  
  return new PolygonMapFromRas(fn, p, map, f8Con, fSmooth);
}

PolygonMapFromRas::PolygonMapFromRas(const FileName& fn, PolygonMapPtr& p)
: PolygonMapVirtual(fn, p)
{
  fNeedFreeze = true;
  String sColName;
  try {
    ReadElement("PolygonMapFromRas", "Map", map);
  }
  catch (const ErrorObject& err) {  // catch to prevent invalid object
    err.Show();
    return;
  }
  ReadElement("PolygonMapFromRas", "EightCon", fEightCon);
  ReadElement("PolygonMapFromRas", "Smoothing", fSmooth);
  Init();
  objdep.Add(map.ptr());
}

PolygonMapFromRas::PolygonMapFromRas(const FileName& fn, PolygonMapPtr& p, const Map& mp, 
                                bool fEightCn, bool fSmth)
: PolygonMapVirtual(fn, p, mp->cs(),mp->cb(),mp->dvrs()), map(mp), fEightCon(fEightCn), fSmooth(fSmth)
{
  fNeedFreeze = true;
  if (map->gr()->fGeoRefNone())
    throw ErrorGeoRefNone(map->gr()->fnObj, errPolygonMapFromRas+1);
  Init();
  objdep.Add(map.ptr());
  if (!fnObj.fValid()) // 'inline' object
    objtime = objdep.tmNewest();
  if (mp->fTblAttSelf()) {
    try {
      SetAttributeTable(mp->tblAtt());
    }
    catch (const ErrorObject& err) {
      err.Show();
    }  
  }  
}

void PolygonMapFromRas::Store()
{
  PolygonMapVirtual::Store();
  WriteElement("PolygonMapVirtual", "Type", "PolygonMapFromRas");
  WriteElement("PolygonMapFromRas", "Map", map);
  WriteElement("PolygonMapFromRas", "EightCon", fEightCon);
  WriteElement("PolygonMapFromRas", "Smoothing", fSmooth);
}

PolygonMapFromRas::~PolygonMapFromRas()
{
}

String PolygonMapFromRas::sExpression() const
{
  String s8Con, sSmooth;
  if (fEightCon)
    s8Con = "8";
  else
    s8Con = "4";
  if (fSmooth)
    sSmooth = "Smooth";
  else
    sSmooth = "NoSmooth";
  return String("PolygonMapFromRas(%S,%S,%S)", map->sNameQuoted(false, fnObj.sPath()), s8Con, sSmooth);
}

bool PolygonMapFromRas::fDomainChangeable() const
{
  return false;
}

void PolygonMapFromRas::Init()
{
  htpFreeze = htpPolygonMapFromRasT;
  sFreezeTitle = "PolygonMapFromRas";
}

bool PolygonMapFromRas::fSetPolygonLabels(const Map& mpAreas)
{
	trq.SetText(SPOLTextInitPolygons);
	Table tblArnAtt = mpAreas->tblAtt();
	assert(tblArnAtt.fValid());
	tblArnAtt->fErase = true;
	String sColMap = map->fnObj.sFile;
	if (sColMap.length() == 0)  // attribute input map
	{
		// sName() returns the expression: map.col, but ColNew() has renamed
		// this to map_col, so do the same here, before accessing the Column
		sColMap = map->sName();
		replace(sColMap.begin(), sColMap.end(), '.', '_');
	}
	Column colAtt(tblArnAtt, sColMap);
	assert(colAtt.fValid());
	DomainSort * pdsrtArn = mpAreas->dm()->pdsrt();
	assert(pdsrtArn);
	iNrPol = pdsrtArn->iSize();
	for (long i = 0; i < iNrPol; i++)
	{
		if (trq.fUpdate(i, iNrPol))
			return false;

		ILWIS::Polygon *pol = pms->pol(i);
		if (!pol || pol->fValid() == false)
			continue;
		Coord c =  pol->crdFindPointInPol();
		long raw = mpAreas->iRaw(c);
		long iKey = mpAreas->dm()->pdsrt()->iKey(mpAreas->dm()->pdsrt()->iOrd(raw));
		String sV = mpAreas->dm()->sValueByRaw(raw);
		String sV2 = colAtt->sValue(iKey);
		double r = pol->rArea();
		if (dvrs().fRealValues()) {
			double rV = colAtt->rValue(iKey);
			if ( rV != rUNDEF)
				pol->PutVal(rV);
		}
		else if (dvrs().fValues()) {
			long iV = colAtt->iValue(iKey);
			if ( iV != iUNDEF)
				pol->PutVal(iV);
		}
		else{
			long v = colAtt->iRaw(iKey);
			pol->PutVal( v == iUNDEF + 1 ? iUNDEF : v); // area numbering makes undefs, undefs + 1 (logic of that applic)
		}
	}
	for (long i = 0; i < iNrPol; i++) {
		ILWIS::Polygon *pol = pms->pol(i);
		if (!pol || pol->fValid() == false)
			continue;
		if ( pol->rValue() == rUNDEF) {
			pol->Delete(true);
		}
	}
	return true;
}

bool PolygonMapFromRas::fFindBoundaries(const Map& mpAreas)
{
	long iLines = map->iLines();
	long iCols = map->iCols();
	iBufInp.Size(iCols, 1, 1);  // rasvec var: line
	iBufInpPrev.Size(iCols, 1, 1); // rasvec var: last_line
	dbBufPrev.Size(iCols, 1, 1); // rasvec var: prev
	dbBufCurr.Size(iCols, 1, 1); // rasvec var: cur
	dbBufNext.Size(iCols, 1, 1); // rasvec var: next
	sbHoriz.Size(iCols, 1, 1); // rasvec var: segline[RIGHT]
	sbVert.Size(iCols, 1, 1); // rasvec var: segline[DOWN]

	iNrSeg = 0;

	long i, j;
	for (j=-1; j <= iCols; ++j)
	{
		iBufInpPrev[j] = iUNDEF;
		dbBufPrev[j] = dbNONE;
		sbHoriz[j] = 0;
		sbVert[j] = 0;
	}
	for (i=0; i < iLines + 1; ++i)
	{
		if (trq.fUpdate(i, iLines))
			return false;
		mpAreas->GetLineRaw(i, iBufInp);
		iBufInp[-1] = iUNDEF;
		iBufInp[iCols] = iUNDEF;
		// calc dbBufCurr with horizontal directions by comparing vertical differences of pixel rows
		// if upper pixel > lower pixel then direction is RIGHT
		if ( i == iLines + 1 )
			for (j=-1; j <= iCols; ++j)
				iBufInp[j] = iUNDEF;   //  fill the extra bottom line with Undefs
			for (j=-1; j <= iCols; ++j)
			{
				if (iBufInpPrev[j]==iBufInp[j])
					dbBufCurr[j] = dbNONE;
				else if (iBufInpPrev[j]>iBufInp[j])
					dbBufCurr[j] = dbRIGHT;
				else
					dbBufCurr[j] = dbLEFT;
			}
			// calc dbBufNext with vertical directions by comparing horizontal differences of pixel columns
			// if left pixel > right pixel then direction is UP
			dbBufNext[-1] = dbNONE;
			for (j=0; j <= iCols; ++j)
			{
				if (iBufInp[j-1]==iBufInp[j])
					dbBufNext[j] = dbNONE;
				else if (iBufInp[j-1]>iBufInp[j])
					dbBufNext[j] = dbUP;
				else
					dbBufNext[j] = dbDOWN;
			}
			
			for (j=0; j <= iCols; ++j)
			{
				byte b = 0;
				// b indicates for 4 pixel boundaries where pixels are different
				//
				//          2
				//     pix  |  pix
				//          |
				//     4 ------- 1
				//          |
				//     pix  |  pix
				//          8
				//
				if (dbBufCurr[j] != dbNONE)
					b |= 1;
				if (dbBufPrev[j] != dbNONE)
					b |= 2;
				if (dbBufCurr[j-1] != dbNONE)
					b |= 4;
				if (dbBufNext[j] != dbNONE)
					b |= 8;
				switch (b)
				{
					case 0:
						{
							sbHoriz[j] = 0;
							sbVert[j] = 0;
						}
						break;
					case 7: case 11: case 13: case 14 :
						{
							NewNode(i, j, b);
						}
						break;
					case 15 :
						{
							if (!fEightCon)
								NewNode(i, j, b);
							else
							{
								bool f1 = iBufInpPrev[j]==iBufInp[j-1];
								bool f2 = iBufInpPrev[j-1]==iBufInp[j];
								if (f1 && f2)
								{
									if (iBufInp[j-1] > iBufInp[j])
										f2 = false;
									else
										f1 = false;
								}
								if (f1)
								{
									AppendLeftUp(i, j); // b = 6
									//b = 9
									sbVert[j] = sbHoriz[j] = sbNewInBetween(j);
									
								}
								else if (f2)
								{
									AppendUp(i, j, 1/*3*/);
									SegBound* h = sbHoriz[j];
									AppendLeft(i, j, 0/*12*/);
									sbHoriz[j] = h;
								}
								else
									NewNode(i, j, b);
							}
							break;
						}
					case 3: case 10 : // append to upper segment
						{
							AppendUp(i,j,b);
						}
						break;
					case 5: case 12 : // append to left segment
						{
							AppendLeft(i,j,b);
						}
						break;
					case 6 : //  append to left and up (become same segment)
						{
							AppendLeftUp(i,j);
						}
						break;
					case 9 :
						{
							sbVert[j] = sbHoriz[j] = sbNewInBetween(j);
						}
						break;
					case 1: case 2: case 4: case 8 : // does not occur
						break;
				}
			}
			// swap buffers to be able to use them in the next line
			Swap(iBufInp, iBufInpPrev);
			Swap(dbBufNext, dbBufPrev);
	}
	return true;
}


bool PolygonMapFromRas::fFreezing()
{
	int iConn = fEightCon ? 8 : 4;
	FileName fnArn = FileName(fnObj, ".mpr");
	Map mapArn(FileName::fnUnique(fnArn), String("MapAreaNumbering(%S,%i,0)", map->sNameQuoted(), iConn));
	mapArn->fErase = true;
	mapArn->dm()->fErase = true;
	mapArn->Calc();
	if (!mapArn->fCalculated())
		return false;

	if (!fFindBoundaries(mapArn)) 
		return false;

	vector<geos::geom::Polygon *> *polygons = polygonizer.getPolygons();
	// fouten afhandleing, maar eerst testen;
	for(long i = 0; i < polygons->size(); ++i) {
		ILWIS::Polygon *pol = CPOLYGON(pms->newFeature());
		if (!pol)
			continue;
		geos::geom::Polygon *gpol = polygons->at(i);
		pol->addBoundary(new LinearRing(gpol->getExteriorRing()->getCoordinates(), new GeometryFactory()));
		for(int j = 0; j < gpol->getNumInteriorRing(); ++j) {
			pol->addHole(new LinearRing(gpol->getInteriorRingN(j)->getCoordinates(), new GeometryFactory()));
		}
//		pol->PutVal(dm()->pdsrt()->iKey(i+1));
	}

	if (!fSetPolygonLabels(mapArn))  // Give all polygons a label (which is a number)
		return false;    // tranquilizer: cancel is clicked in the function

	//trq.SetTitle(sFreezeTitle);
	//trq.SetText(SPOLTextExtractTopologies);

	//return fFindBoundaries(mapArn); // if false then tranquilizer:cancel is clicked in the function
	return true;

}

void PolygonMapFromRas::DetLink(DirBound db1, DirBound db2, DirBound db3, 
                        const Array<bool>& fSegExist, const Array<bool>& fBeginSeg,
                        const Array<SegBound*>& sbSeg)
{
  long iSegNr;
  if (fSegExist[db1]) {
    if (fSegExist[db2]) {
      iSegNr = sbSeg[db2]->iSegNr;
      if (!fBeginSeg[db2])
        iSegNr = -iSegNr;
    }
    else {
      iSegNr = sbSeg[db3]->iSegNr;
      if (!fBeginSeg[db3])
        iSegNr = -iSegNr;
    }
    if (fBeginSeg[db1])
      aiBwl[sbSeg[db1]->iSegNr] = iSegNr;
    else
      aiFwl[sbSeg[db1]->iSegNr] = iSegNr;
  }
}

void PolygonMapFromRas::NewNode(long iLine, long iCol, byte b)
{
  Array<bool> fSegExist(dbRIGHT+1);
  Array<bool> fBeginSeg(dbRIGHT+1);
  Array<SegBound*> sbSeg(dbRIGHT+1);
  fSegExist[dbRIGHT] = b & 1;
  fSegExist[dbUP   ] = ( b & 2 ) != 0;
  fSegExist[dbLEFT ] = ( b & 4 ) != 0 ;
  fSegExist[dbDOWN ] = ( b & 8 ) != 0 ;
  if (fSegExist[dbRIGHT]) {// new segment to the right 
    sbSeg[dbRIGHT] = sbNewWithOneEnd(iLine, iCol, true, (bool &)fBeginSeg[dbRIGHT]);
    sbHoriz[iCol] = sbSeg[dbRIGHT];
  }
  else
    sbHoriz[iCol] = 0;
  if (fSegExist[dbUP]) { // end of segment up 
    sbSeg[dbUP] = sbVert[iCol];
    EndOfSegment(iLine, iCol, *sbSeg[dbUP], true, (bool &)fBeginSeg[dbUP]);
  }
  if (fSegExist[dbLEFT]) { //end of segment to the left 
    sbSeg[dbLEFT] = sbHoriz[iCol - 1];
    EndOfSegment(iLine, iCol, *sbSeg[dbLEFT], false, (bool &)fBeginSeg[dbLEFT]);
  }
  if (fSegExist[dbDOWN]) { // new segment down }
    sbSeg[dbDOWN] = sbNewWithOneEnd(iLine, iCol, false, (bool &)fBeginSeg[dbDOWN]);
    sbVert[iCol] = sbSeg[dbDOWN];
  }
  else
    sbVert[iCol] = 0;

  DetLink(dbRIGHT, dbDOWN, dbLEFT, fSegExist, fBeginSeg, sbSeg);
  DetLink(dbUP, dbRIGHT, dbDOWN, fSegExist, fBeginSeg, sbSeg);
  DetLink(dbLEFT, dbUP, dbRIGHT, fSegExist, fBeginSeg, sbSeg);
  DetLink(dbDOWN, dbLEFT, dbUP, fSegExist, fBeginSeg, sbSeg);

  if (fSegExist[dbUP]) // end of segment up 
    StoreSegm(*sbSeg[dbUP]);
  if (fSegExist[dbLEFT]) // end of segment to the left 
    StoreSegm(*sbSeg[dbLEFT]);
}


void PolygonMapFromRas::AppendLeftUp(long iLine, long iCol)
/* Appends codes of point (iLine, iCol) to segment to the left of point. 
   The segment above the point is chained to the left segment.   
   Is called if type of point = 6 (line from left to up or line  
   from up to left.                                              */
{
  enum WhatDel { DELNONE, DELLEFT, DELUP } del;
  SegBound* sbUp = sbVert[iCol];
  SegBound* sbLeft = sbHoriz[iCol - 1];
  if (sbLeft != sbUp) {
    del = DELNONE;
    if (dbBufCurr[iCol - 1]==dbLEFT) { //line comes from above, goes left }
      if (sbUp->fBeginSeg) {
        sbUp->dlChain.append(sbLeft->dlChain);
//        for (DLIter<ChainRec> iter(&sbLeft->dlChain); iter.fValid(); iter++) 
//          sbUp->dlChain.append(iter());
        del = DELLEFT;
        sbUp->fEndSeg = sbLeft->fEndSeg;
        if (sbUp->fEndSeg) {
          aiFwl[sbUp->iSegNr] = aiFwl[sbLeft->iSegNr];
          for (unsigned long i=1; i<=aiBwl.iSize(); ++i) {
            if (aiBwl[i] == -sbLeft->iSegNr)
              aiBwl[i] = -sbUp->iSegNr;
            else if (aiBwl[i] == sbLeft->iSegNr)
              aiBwl[i] = sbUp->iSegNr;
            else if (aiFwl[i] == -sbLeft->iSegNr)
              aiFwl[i] = -sbUp->iSegNr;
            else if (aiFwl[i] == sbLeft->iSegNr)
              aiFwl[i] = sbUp->iSegNr;
          }
        }
        sbUp->crdTo = sbLeft->crdTo;
        StoreSegm(*sbUp);
      }
      else { // sbUp->fBeginSeg == false
        sbLeft->dlChain.insert(sbUp->dlChain);
//        DLIter<ChainRec> iter(&sbUp->dlChain);
//        for (iter.last(); iter.fValid(); iter--) 
//          sbLeft->dlChain.insert(iter());
        del = DELUP;
      }
    }
    else { // line comes from left, goes to above 
        if (sbUp->fEndSeg) {
          sbUp->dlChain.insert(sbLeft->dlChain);
//          DLIter<ChainRec> iter(&sbLeft->dlChain);
//          for (iter.last(); iter.fValid(); iter--)
//            sbUp->dlChain.insert(iter());
          del = DELLEFT;
          sbUp->fBeginSeg = sbLeft->fBeginSeg;
          if (sbUp->fBeginSeg) {
            aiBwl[sbUp->iSegNr] = aiBwl[sbLeft->iSegNr];
            for (unsigned long i=1; i<=aiFwl.iSize(); ++i) {
              if (aiFwl[i] == sbLeft->iSegNr)
                aiFwl[i] = sbUp->iSegNr;
              else if (aiFwl[i] == -sbLeft->iSegNr)
                aiFwl[i] = -sbUp->iSegNr;
              else if (aiBwl[i] == sbLeft->iSegNr)
                aiBwl[i] = sbUp->iSegNr;
              else if (aiBwl[i] == -sbLeft->iSegNr)
                aiBwl[i] = -sbUp->iSegNr;
            }
          }
          sbUp->crdFrom = sbLeft->crdFrom;
          StoreSegm(*sbUp);
        }
        else {
          sbLeft->dlChain.append(sbUp->dlChain);
  //        for (DLIter<ChainRec> iter(&sbUp->dlChain); iter.fValid(); iter++) 
//            sbLeft->dlChain.append(iter());
          del = DELUP;
        }
    }
    // adjust sbHoriz and sbVert by replacing the deleted SegBound with the other one
    if (del == DELLEFT) {
      for (long i = sbHoriz.iLower(); i <= sbHoriz.iUpper(); i++) {
        if (sbHoriz[i] == sbLeft)
          sbHoriz[i] =  sbUp;
        if (sbVert[i] == sbLeft)
          sbVert[i] = sbUp;
      }
      aiSegNr &= sbLeft->iSegNr;
      aiFwl[sbLeft->iSegNr] = 0;
      aiBwl[sbLeft->iSegNr] = 0;
      delete sbLeft;
    }
    else if (del == DELUP) {
      for (long i = sbHoriz.iLower(); i <= sbHoriz.iUpper(); i++) {
        if (sbHoriz[i] == sbUp)
          sbHoriz[i] =  sbLeft;
        if (sbVert[i] == sbUp)
          sbVert[i] = sbLeft;
      }
      aiSegNr &= sbUp->iSegNr;
      aiFwl[sbUp->iSegNr] = 0;
      aiBwl[sbUp->iSegNr] = 0;
      delete sbUp;
    }
    else {
      StoreSegm(*sbLeft);
      StoreSegm(*sbUp);
    }
  }
  else { // segment is contour of island 
    // create new node; start and end of segment 
    // modify segment 
    sbUp->fBeginSeg = true;
    sbUp->crdFrom.x = iCol; sbUp->crdFrom.y = iLine;
    sbUp->fEndSeg = true;
    sbUp->crdTo.x = iCol; sbUp->crdTo.y = iLine;
    aiFwl[sbUp->iSegNr] = sbUp->iSegNr;
    aiBwl[sbUp->iSegNr] = -sbUp->iSegNr;
    StoreSegm(*sbUp);
  }
  sbHoriz[iCol] = 0;
  sbVert[iCol] = 0;
/*  String s("Left up ");
  for (long i=1; i<aiBwl.iSize(); ++i) {
    s &= String("(%li ", aiFwl[i]);
    s &= String("%li)", aiBwl[i]);
  }
  trq.Message(s);*/
}

void PolygonMapFromRas::AppendUp(long iLine, long iCol, byte b)
// Append chain codes of (iLine,iRow) to segment above point
{
  SegBound* sb = sbVert[iCol];
  DirBound db;
  if ((dbBufCurr[iCol]==dbLEFT) || ((b != 1)) && (dbBufNext[iCol]==dbUP)) {
    // line goes up 
    if (b & 1) 
      db = dbLEFT;
    else
      db = dbUP;
    if (sb->crFirst().dbCode == db)
      sb->crFirst().iLength++;
    else 
      sb->Insert(db, 1); // insert at begin
  }  
  else { // line goes down 
    if (b & 1) 
      db = dbRIGHT;
    else
      db = dbDOWN;
    if (sb->dlChain.last().dbCode == db)
      sb->crLast().iLength++;
    else 
      sb->Append(db, 1); // append at end
  }  
  if (b == 10) {
    sbHoriz[iCol] = 0;
    sbVert[iCol] = sb;
 }
 else {
    sbVert[iCol] = 0;
    sbHoriz[iCol] = sb;
 }
 StoreSegm(*sb);
}

void PolygonMapFromRas::AppendLeft(long iLine, long iCol, byte b)
// Append chain code from (iLine, iCol) to segment to the left.
{
  SegBound* sb = sbHoriz[iCol - 1];
  DirBound db;
  if (((dbBufCurr[iCol]==dbLEFT) && (b != 0)) || (dbBufNext[iCol]==dbUP)) {
    // line goes left 
    if (b & 1) 
      db = dbLEFT;
    else
      db = dbUP;
    if (sb->crFirst().dbCode == db)
      sb->crFirst().iLength++;
    else 
      sb->Insert(db, 1); // insert at begin
  }  
  else { // line goes right 
    if (b & 1) 
      db = dbRIGHT;
    else
      db = dbDOWN;
    if (sb->dlChain.last().dbCode == db)
      sb->crLast().iLength++;
    else 
      sb->Append(db, 1); // append at end
  }  
  if (b == 5) {
    sbVert[iCol] = 0;
    sbHoriz[iCol] = sb;
 }
 else {
    sbHoriz[iCol] = 0;
    sbVert[iCol] = sb;
 }
 StoreSegm(*sb);
}

SegBound* PolygonMapFromRas::sbNewInBetween(long iCol)
// Creates a new segment without nodes but with two chain codes. 
// Is called if point has code 9 ((left,down) or (up,right)).  
{
  SegBound* sb = new SegBound;
//  sb->iForw = sb->iBackw = 0;
  sb->iSegNr = iNewSegNr();
  if (sb->iSegNr >= (int)aiFwl.iSize()) {
    aiFwl.Resize(sb->iSegNr+1);
    aiBwl.Resize(sb->iSegNr+1);
  } 
  aiFwl[sb->iSegNr] = aiBwl[sb->iSegNr] = 0;
  ChainRec crBeg, crEnd;
  if (dbBufCurr[iCol] == dbLEFT) {
    // left, down 
    crBeg.dbCode = dbLEFT;
    crEnd.dbCode = dbDOWN;
    sb->iLeftRaw = iBufInp[iCol];
    sb->iRightRaw = iBufInpPrev[iCol];
  }
  else { //up, right 
    crBeg.dbCode = dbUP;
    crEnd.dbCode = dbRIGHT;
    sb->iLeftRaw = iBufInpPrev[iCol];
    sb->iRightRaw = iBufInp[iCol];
  }
  sb->dlChain.insert(crBeg); // insert at begin
  sb->dlChain.append(crEnd); // append to end
  sb->fBeginSeg = sb->fEndSeg = false; 
  sb->crdFrom.x = sb->crdFrom.y = sb->crdTo.x = sb->crdTo.y = 0;
  return sb;
}

SegBound* PolygonMapFromRas::sbNewWithOneEnd(long iLine, long iCol, bool fRight, bool& fBegin)
// Creates new segment with a node at one end.                 
// If fRightSeg==true : it has to be a segment to the right of node,
// else a segment under the node.                                 
{
  DirBound db;
  if (fRight) 
    db = (DirBound)dbBufCurr[iCol];
  else
    db = (DirBound)dbBufNext[iCol];
  SegBound* sb = new SegBound;
//  sb->iForw = sb->iBackw = 0;
  sb->iSegNr = iNewSegNr();
  if (sb->iSegNr >= (int)aiFwl.iSize()) {
    aiFwl.Resize(sb->iSegNr+1);
    aiBwl.Resize(sb->iSegNr+1);
  }  
  aiFwl[sb->iSegNr] = aiBwl[sb->iSegNr] = 0;
  if ((db == dbRIGHT) || (db==dbDOWN)) { // start of segment 
    sb->Insert(db, 1); // at start
    sb->fBeginSeg = true;
    fBegin = true;
    sb->fEndSeg = false;
    sb->crdFrom.x = iCol; sb->crdFrom.y = iLine;
    sb->crdTo.x = sb->crdTo.y = 0;
  }
  else { // end of segment 
    sb->Append(db, 1); // at end
    sb->fBeginSeg = false;
    fBegin = false;
    sb->fEndSeg = true;
    sb->crdFrom.x = sb->crdFrom.y = 0;
    sb->crdTo.x = iCol; sb->crdTo.y = iLine;
  }  
  switch (db)  {
    case dbRIGHT : {
//            top.PolL := last_line^[x]; top.PolR := line^[x];
           sb->iLeftRaw = iBufInpPrev[iCol];
           sb->iRightRaw = iBufInp[iCol];
         }
         break;
    case dbUP : {
//              top.PolL := line^[x - 1]; top.PolR := line^[x];
           sb->iLeftRaw = iBufInp[iCol-1];
           sb->iRightRaw = iBufInp[iCol];
         }
         break;
    case dbLEFT : {
//              top.PolL := line^[x]; top.PolR := last_line^[x];
           sb->iLeftRaw = iBufInp[iCol];
           sb->iRightRaw = iBufInpPrev[iCol];
         }
         break;
    case dbDOWN : {
//                top.PolL := line^[x]; top.PolR := line^[x - 1];
           sb->iLeftRaw = iBufInp[iCol];
           sb->iRightRaw = iBufInp[iCol-1];
         }
         break;
    default : assert(0==1);
      break;
  }
  return sb;
}

void PolygonMapFromRas::EndOfSegment(long iLine, long iCol, SegBound& sb, bool fUp, bool& fBegin)
{
  if ((fUp && (dbBufPrev[iCol] == dbUP)) ||
      (!fUp && (dbBufCurr[iCol-1] == dbLEFT))) { // begin of segment
    fBegin = true;
    sb.fBeginSeg = true;
    sb.crdFrom.x = iCol; sb.crdFrom.y = iLine;
  }
  else { // end of segment 
    fBegin = false;
    sb.fEndSeg = true;
    sb.crdTo.x = iCol; sb.crdTo.y = iLine;
  }  
}

/// StoreSegm function  without smoothing: 
/*

void PolygonMapFromRas::StoreSegm(const SegBound& sb)
{
  if (!sb.fBeginSeg || !sb.fEndSeg)
    return;
  CoordBuf cBuf(1000);
  RowColBuf rcBuf(1000);
  long iCrd;
  // calculate coordinates
  RowCol crdFrom = sb.crdFrom;
  map->gr()->RowCol2Coord(crdFrom.y, crdFrom.x, cBuf[0]);
  iCrd = 1;
  for (DLIter<ChainRec> iter(&const_cast<SegBound&>(sb).dlChain); iter.fValid(); iter++) {
    DirBound db = iter().dbCode;
    switch (db)  {
      case dbRIGHT : {
             crdFrom.x += iter().iLength;
           }
           break;
      case dbUP : {
             crdFrom.y -= iter().iLength;
           }
           break;
      case dbLEFT : {
             crdFrom.x -= iter().iLength;
           }
           break;
      case dbDOWN : {
             crdFrom.y += iter().iLength;
           }
           break;
      default : assert(0==1);
        break;
    }
    map->gr()->RowCol2Coord(crdFrom.y, crdFrom.x, cBuf[iCrd++]);
  }
  
  Segment seg = ptr->newFeature();
  seg.WorldCoors2InternalCoors(cBuf, rcBuf, iCrd);
  seg.PutCoors(iCrd, rcBuf);
  DomainSort* pdsrt = dm()->pdsrt();
  String sVal("%S | %S", map->dm()->sValueByRaw(sb.iLeftRaw,0), map->dm()->sValueByRaw(sb.iRightRaw,0));
  long iRaw = pdsrt->iAdd(sVal);
  seg.PutVal(iRaw);
}
*/ 

/// Sore Segm function with smoothing:


void PolygonMapFromRas::StoreSegm(SegBound& sb)
{
  if (!sb.fBeginSeg || !sb.fEndSeg)
    return;
  CoordBuf cBuf;
  long iCrd = 0;
  bool fIsland = false; // sb.crdFrom == sb.crdTo;
  if (fSmooth) {

    // calculate coordinates
    Coord crdFrom = sb.crdFrom;
    Coord crdTo = sb.crdTo;
    ArrayLarge<ChainRec> acr;
    for (DLIter<ChainRec> iter(&const_cast<SegBound&>(sb).dlChain); iter.fValid(); iter++) 
      acr &= iter();
    cBuf.Size(acr.iSize()*2); // should be sufficient
    double x, y, dx, dy;
    x = crdFrom.x;
    y = crdFrom.y;
    long iPrevStep, iCurrStep, iNextStep;
    if ( fIsland ) {
      ChainRec crLast = acr[acr.iSize()-1];
      iPrevStep = crLast.iLength;
      if ((crLast.dbCode == dbUP) || (crLast.dbCode == dbLEFT))
        iPrevStep = -iPrevStep;
    }
    else {
      iPrevStep = 0;
      cBuf[iCrd++] = crdFrom;
    }
    iCurrStep = acr[0].iLength;
    if ((acr[0].dbCode == dbUP) || (acr[0].dbCode == dbLEFT))
      iCurrStep = -iCurrStep;
    Coord cFirst = Coord(crdFrom.x, crdFrom.y);
    for (unsigned long i=0; i < acr.iSize(); ++i) {
      if ( i != acr.iSize() - 1 ) {
        iNextStep = acr[i+1].iLength;
        if ((acr[i+1].dbCode == dbUP) || (acr[i+1].dbCode == dbLEFT))
          iNextStep = -iNextStep;
      }
      else {
        if ( fIsland ) {
          iNextStep = acr[0].iLength;
          if ((acr[0].dbCode == dbUP) || (acr[0].dbCode == dbLEFT))
            iNextStep = -iNextStep;
        }
        else
          iNextStep = 0;
      }
      switch ( acr[i].dbCode )
      {
        case dbRIGHT: case dbLEFT : {
            dx = iCurrStep; dy = 0;
          }
          break;
        case dbUP: case dbDOWN :  {
            dx = 0; dy = iCurrStep;
          }
          break;

        default :
          assert(0==1);
          break;
      }
      long iNewCrd=0;
      if (abs(iCurrStep) == 1) {
        if ( iPrevStep != iNextStep ) {
          cBuf[iCrd++] = Coord(x + dx / 2, y + dy / 2);
          iNewCrd++;
        }
        if ( !fIsland && (iNextStep == 0 ) ) {
          cBuf[iCrd++] = Coord(x + dx, y + dy);
          iNewCrd++;
        }
      }
      else {
        if ( (abs(iPrevStep) == 1) || (iNextStep == 1) ) {
          cBuf[iCrd++] = Coord(x + dx / 2, y + dy / 2);
          iNewCrd++;
        }
        if ( abs(iNextStep) != 1 ) {
          cBuf[iCrd++] = Coord(x + dx, y + dy);
          iNewCrd++;
        }
      }
      if (fIsland && (iCrd == iNewCrd))
        cFirst = cBuf[0];
      x += dx;
      y += dy;
      iPrevStep = iCurrStep;
      iCurrStep = iNextStep;
    }
    if (fIsland) {
      if (cBuf[0] != cBuf[iCrd-1])
        cBuf[iCrd++] = cBuf[0];
    }  
	StoreSegm(sb, cBuf, iCrd, fIsland);
  //  aiSegNr &= sb.iSegNr;
  }
  else {
    // count chains
    int iChains= 0;
    for (DLIter<ChainRec> iter1(&const_cast<SegBound&>(sb).dlChain); iter1.fValid(); iter1++) 
      iChains++;
    cBuf.Size(iChains+1);
    // calculate coordinates
    Coord crdCurr = sb.crdFrom;
    cBuf[iCrd++] = crdCurr;
    for (DLIter<ChainRec> iter(&const_cast<SegBound&>(sb).dlChain); iter.fValid(); iter++) {
      DirBound db = iter().dbCode;
      switch (db)  {
        case dbRIGHT : {
               crdCurr.x += iter().iLength;
             }
             break;
        case dbUP : {
               crdCurr.y -= iter().iLength;
             }
             break;
        case dbLEFT : {
               crdCurr.x -= iter().iLength;
             }
             break;
        case dbDOWN : {
               crdCurr.y += iter().iLength;
             }
             break;
        default : assert(0==1);
          break;
      }
     cBuf[iCrd++] = crdCurr;
    }
  }
	CoordBuf cBuf2(iCrd);
	for(int ic=0; ic < iCrd; ++ic)
		cBuf2[ic] = cBuf[ic];
	
  StoreSegm(sb, cBuf, iCrd, fIsland);
}

void PolygonMapFromRas::StoreSegm(const SegBound& sb, CoordBuf& cBuf, long& iCrd, bool fIsland)
{
  if (iCrd == 0)
    return;

  trq.fAborted();
  CoordBuf cBufTemp(iCrd);
  for ( long j=0; j < iCrd; ++j)
    map->gr()->RowCol2Coord(cBuf[j].y, cBuf[j].x, cBufTemp[j]);

  //geos::operation::polygonize::Polygonizer polygonizer;
 // ILWIS::Polygon *pol;
 // if ( sb.iSegNr > pms->iPol())
	//pol = pms->newFeature();
 // else
	//pol = pms->pol(sb.iSegNr);
 // if ( !pol)
	//  throw ErrorObject(String("Couldn't find polygon nr %d",sb.iSegNr));
 // if(fIsland) {
	//  pol->addHole(new LinearRing(cBufTemp.clone(), new GeometryFactory()));
 // }
 // else{
	// pol->addBoundary(new LinearRing(cBufTemp.clone(), new GeometryFactory()));
	// DomainSort* pdsrt = dm()->pdsrt();
	// String sVal("%S | %S", map->dm()->sValueByRaw(sb.iLeftRaw,0), map->dm()->sValueByRaw(sb.iRightRaw,0));
	// long iRaw = pdsrt->iAdd(sVal);
	// pol->PutVal(iRaw);
 // }
  GeometryFactory *fact = new GeometryFactory();
  polygonizer.add( (Geometry *)(fact->createLineString(cBufTemp.clone())));
  for ( long j=0; j < iCrd; ++j)
	  cBuf[j] = cBufTemp[j];
  iCrd = 0;
}

long PolygonMapFromRas::iNewSegNr()
{
  long iInd = aiSegNr.iSize()-1;
  if (iInd >= 0) {
    long iSegNr = aiSegNr[iInd];
    aiSegNr.Remove(iInd, 1);
    return iSegNr;
  }  
  return ++iNrSeg;
}





