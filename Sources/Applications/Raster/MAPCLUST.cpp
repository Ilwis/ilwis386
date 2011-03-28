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
// $Log: /ILWIS 3.0/RasterApplication/MAPCLUST.cpp $
 * 
 * 12    9-01-06 13:04 Willem
 * Allow value maps for clustering
 * Increase the maximum number of classes ro 255
 * Debugged: erroneous shift in case of 3 band clustering
 * 
 * 11    5-12-02 18:18 Hendrikse
 * made proper use of fnObj.sRelativeQuoted() to debug for output table
 * names with spaces etc
 * 
 * 10    4/09/01 11:31 Willem
 * Restored to calling the original constructor for the MapList creation.
 * 
 * 9     5/04/01 16:03 Willem
 * Added extra check on validity of MapList
 * 
 * 8     23-11-00 17:54 Koolhoven
 * added \n to sSyntax()
 * 
 * 7     28-10-00 11:37 Hendrikse
 * added for the output columns:
 * col->SetReadOnly();
 * col->SetOwnedByTable(true);
 * added	tblCross.SetPointer(0); to remove intermediate results
 * 
 * 6     8/15/00 5:15p Wind
 * added statistics table
 * 
 * 5     22-02-00 16:56 Wind
 * bug with array boundaries that caused wrong results
 * 
 * 4     9/08/99 12:58p Wind
 * changed constructor calls FileName(fn, sExt, true) to FileName(fn,
 * sExt)
 * or changed FileName(fn, sExt, false) to FileName(fn.sFullNameQuoted(),
 * seExt, false)
 * to ensure that proper constructor is called
 * 
 * 3     9/08/99 11:51a Wind
 * comment problem
 * 
 * 2     9/08/99 8:54a Wind
 * changed sName() to sNameQuoted() in sExpression() tot suport quoted
 * file names
*/
// Revision 1.5  1998/09/16 17:24:39  Wim
// 22beta2
//
// Revision 1.4  1998/02/05 16:41:37  Wim
// Create internal domain only after all the checks which can cause error throwing,
// to prevent the creation of an invalid .mpr file
//
// Revision 1.3  1997-09-05 15:52:59+01  Wim
// CreateMapStore() after calling SetDomainValueRangeStruct() in fFreezing()
//
// Revision 1.2  1997-07-29 16:29:22+02  Wim
// Internal domain uses now prefix "Cluster"
//
/* MapCluster
   Copyright Ilwis System Development ITC
   july 1995, by Wim Koolhoven
*/
#define MAPCLUST_C
#include "Applications\Raster\MAPCLUST.H"
#include "Engine\Base\Algorithm\Qsort.h"
#include "Headers\Htp\Ilwisapp.htp"
#include "Headers\Err\Ilwisapp.err"
#include "Headers\Hs\map.hs"

IlwisObjectPtr * createMapCluster(const FileName& fn, IlwisObjectPtr& ptr, const String& sExpr, vector<void *> parms ) {
	if ( sExpr != "")
		return (IlwisObjectPtr *)MapCluster::create(fn, (MapPtr &)ptr, sExpr);
	else
		return (IlwisObjectPtr *)new MapCluster(fn, (MapPtr &)ptr);
}

#define MAXCLASSES 255

const char* MapCluster::sSyntax() 
{ return "MapCluster(maplist,classes)\n"
"MapCluster(maplist,classes,stattable)"; }

MapCluster* MapCluster::create(const FileName& fn, MapPtr& p, const String& sExpr)
{
	Array<String> as;
	int iParms = IlwisObjectPtr::iParseParm(sExpr, as);
	if ((iParms < 2) || (iParms > 4))
		ExpressionError(sExpr, sSyntax());

	MapList mpl(as[0], fn.sPath());

	if (mpl.fValid())
	{
		int iClasses = as[1].iVal();
		if (iClasses == shUNDEF)
			throw ErrorObject(WhatError(String(SMAPErrInvalidNrClasses_S.scVal(), as[1]), errMapCluster+4), fn);
		String sStatTable;
		if (iParms == 3)
			sStatTable = as[2];
		return new MapCluster(fn, p, mpl, iClasses, sStatTable);
	}
	return 0;
}

MapCluster::MapCluster(const FileName& fn, MapPtr& p)
: MapFromMapList(fn, p)
{
  ReadElement("MapCluster", "Classes", iClasses);
  ReadElement("MapCluster", "StatTable", sStatTable);
  fNeedFreeze = true;
  sFreezeTitle = "MapCluster";
  htpFreeze = htpMapClusterT;
  iBands = mpl->iSize();
}

MapCluster::MapCluster(const FileName& fn, MapPtr& p, const MapList& mpl, int iClass, const String& sStatTbl)
: MapFromMapList(fn, p, mpl, Domain("image")/* just temp, see below*/), iClasses(iClass), sStatTable(sStatTbl)
{
  if (iClasses < 2)
    throw ErrorObject(WhatError(String(SMAPErrTooFewClasses_i.scVal(), iClasses), errMapCluster+1), sTypeName());
  if (iClasses > MAXCLASSES)
    throw ErrorObject(WhatError(String(SMAPErrTooManyClasses_ii.scVal(), MAXCLASSES, iClasses), errMapCluster+2), sTypeName());
  iBands = mpl->iSize();
  if ((iBands == 0) || (iBands > MAXBANDS))
    throw ErrorObject(WhatError(String(SMAPErrTooManyBands_i.scVal(), iBands), errMapCluster+3), sTypeName());
/*  // check on image domain
  Map mp = mpl->map(mpl->iLower());
  if (0 == mp->dm()->pdi())
    throw ErrorImageDomain(mp->dm()->sName(true, fnObj.sPath()), mp->fnObj, errMapCluster);
  for (int i=1; i< iBands ; ++i) {
    mp = mpl->map(mpl->iLower()+i);
    if (0 == mp->dm()->pdi())
      throw ErrorImageDomain(mp->dm()->sName(true, fnObj.sPath()), mp->fnObj, errMapCluster);
  }  */

  // reset the output domain to class with iClasses classes
  SetDomainValueRangeStruct(DomainValueRangeStruct(Domain(fnObj, iClass, dmtCLASS)));
  fNeedFreeze = true;
  sFreezeTitle = "MapCluster";
  htpFreeze = htpMapClusterT;
  if (!fnObj.fValid())
    objtime = objdep.tmNewest();
}

void MapCluster::Store()
{
  MapFromMapList::Store();
  WriteElement("MapFromMapList", "Type", "MapCluster");
  WriteElement("MapCluster", "Classes", (long)iClasses);
	if (sStatTable.length() > 0)
    WriteElement("MapCluster", "StatTable", sStatTable);
}

MapCluster::~MapCluster()
{
}

void MapCluster::initFeatureSpaceHistogram(int iBands) {
    int i, j, k, l;
  switch (iBands) {
    case 1: {
      bMax = 255; iShift = 8; indexShift = 0;
      for (i=0; i < 256; i++)
        for (j=0; j < 256; j++) {
          unsigned int x = j + (i << 8);
          HistBands[x].iCount = 0;
          if (i == 0)
            HistBands[x].iComb = j;
          else 
            HistBands[x].iComb = 0;
        }
      break;
    }
    case 2: {
      bMax = 255; iShift = 8; indexShift = 0;
      for (i=0; i < 256; i++)
        for (j=0; j < 256; j++) {
          unsigned int x = j + (i << 8);
          HistBands[x].iCount = 0;
          HistBands[x].iComb = x;
        }
      break;
    }
    case 3: {
      bMax = 31; iShift = 5; indexShift = 5;
      for (i=0; i < 32; i++)
        for (j=0; j < 32; j++)
          for (k=0; k < 32; k++) {
            unsigned int x = k + (j << 5) + (unsigned)(i << 10);
            HistBands[x].iCount = 0;
            HistBands[x].iComb = x;
          }
      break;
    }  
    case 4: {
      bMax = 15; iShift = 4; indexShift = 4;
      for (i=0; i < 16; i++)
        for (j=0; j < 16; j++)
          for (k=0; k < 16; k++)
            for (l=0; l < 16; l++) {
              unsigned int x = l + (k << 4) + (unsigned)(j << 8) + (unsigned)(i << 12);
              HistBands[x].iCount = 0;
              HistBands[x].iComb = x;
            }
      break;
    }      
  }
}

void MapCluster::initBandLineBuffers() {
  RowCol rc = rcSize();
  long iCols = rc.Col;
  long iLines = rc.Row;
    for (int j = 0; j < iBands; j++) 
        bLine[j].Size(iCols);
    for (int j = iBands; j < MAXBANDS; j++) {
        bLine[j].Size(iCols);
        for (int k = 0; k < iCols; ++k)
            bLine[j][k] = 0;
    }
}
void MapCluster::initStretchLookup() {
    aarIndex2Value.Resize(iBands);
    int i, j;

    for (j = 0; j < iBands; j++) {
        // Setup lookup table from an index to value boundaries for the feature space axis
        RangeReal rng = mpl[j + mpl->iLower()]->rrPerc1(true); // force calculation of histogram
        abrLookup[j].bottom = rng.rLo();
        abrLookup[j].top = rng.rHi();

        aarIndex2Value[j].Resize(bMax + 1); 
        double low = rng.rLo();
        double high = rng.rHi();
        for (i = 0; i <= bMax; ++i) {
            aarIndex2Value[j][i] = low + ((high - low) * i) / (double) bMax;
        }
    }
}

void MapCluster::writeStatisticsTable(String& sStatTable) {
    try {
      FileName fnStatTbl(fnObj);
			fnStatTbl = FileName(sStatTable, ".tbt");
			Table tblStat(fnStatTbl, dm());
			tblStat->Store();
			SetAttributeTable(tblStat);
			for (int i=0; i < iBands; i++) {
				// create avg, prd, min and max columns per cluster for each band
				// first make (temp) cross table
        trq.SetText(String(SMAPTextCrossClusterMap_S.scVal(), mpl[i+mpl->iLower()]->sName(true, fnObj.sPath())));
			  String sTblCross("TableCross(%S, %S,IgnoreUndefs)", fnObj.sRelativeQuoted(false), mpl[i+mpl->iLower()]->sNameQuoted(true, fnObj.sPath()));
				Table tblCross(FileName::fnUnique(fnStatTbl), sTblCross);
				tblCross->Calc();
				tblCross->fErase = true;
				tblCross->dm()->fErase = true;
				// do the aggregation and store it in stat table
				String sBand = mpl[i+mpl->iLower()]->fnObj.sRelativeQuoted(false);
				String sTbl = tblCross->sNameQuoted(true, fnObj.sPath());
				
				String sColName("%S_Avg", sBand);
        trq.SetText(String(SMAPTextCreateStatTableCol_S.scVal(), sColName));
				String sColDef("ColumnAggregateAvg(%S.%S,%S.%S,%S.NPix)", sTbl, sBand, sTbl, fnObj.sRelativeQuoted(false), sTbl);
				Column col(tblStat,sColName, sColDef);
				col->BreakDependency();
				col->SetReadOnly();
				col->SetOwnedByTable(true);
				
				sColName = String("%S_Prd", sBand);
        trq.SetText(String(SMAPTextCreateStatTableCol_S.scVal(), sColName));
				sColDef = String("ColumnAggregatePrd(%S.%S,%S.%S,%S.NPix)", sTbl, sBand, sTbl, fnObj.sRelativeQuoted(false), sTbl);
				col = Column(tblStat,sColName, sColDef);
				col->BreakDependency();
				col->SetReadOnly();
				col->SetOwnedByTable(true);
				
				sColName = String("%S_Min", sBand);
        trq.SetText(String(SMAPTextCreateStatTableCol_S.scVal(), sColName));
				sColDef = String("ColumnAggregateMin(%S.%S,%S.%S)", sTbl, sBand, sTbl, fnObj.sRelativeQuoted(false));
				col = Column(tblStat,sColName, sColDef);
				col->BreakDependency();
				col->SetReadOnly();
				col->SetOwnedByTable(true);
				
				sColName = String("%S_Max", sBand);
        trq.SetText(String(SMAPTextCreateStatTableCol_S.scVal(), sColName));
				sColDef = String("ColumnAggregateMax(%S.%S,%S.%S)", sTbl, sBand, sTbl, fnObj.sRelativeQuoted(false));
				col = Column(tblStat,sColName, sColDef);
				col->BreakDependency();
				col->SetReadOnly();
				col->SetOwnedByTable(true);
				
				tblCross.SetPointer(0);
			}
		}
		catch (const ErrorObject& err) {
			err.Show();
		}
}
// calculate the feature space histogram index from the input pixel vector
// the index is at most 16 bit:
//    8 bit for one band
//   16 bit for two bands (2 x 8)
//   15 bit for three bands (3 x 5)
//   16 bit for four bands (4 x 4)
long MapCluster::getFSIndex(long col) {
    long index = 0;
    for (int bandNr = 0; bandNr < iBands; bandNr++) {
        BoundRange& br = abrLookup[bandNr];
        double val = bLine[bandNr][col];
        // taking the 1% range needs check for values in the 1% range
        if (val < br.bottom) val = br.bottom;
        if (val > br.top) val = br.top;

        double ratio = (val - br.bottom) / (br.top - br.bottom);
        int subindex = (long) (ratio * bMax);
        index += (subindex) << (indexShift * bandNr);
		if ( index < 0) {
			int kk = 0;
			++kk;
		}
    }
    return index;
}
/* For the input bands a combined histogram is calculated. Of each pixel
   (byte) the least significant 4 bits are ignored. The histogram entries are
   defined by the struct HistBandsRec.
*/
bool MapCluster::fFreezing()
{
    RowCol rc = rcSize();
    long iCols = rc.Col;
    long iLines = rc.Row;
    int i, j, l;          // counters
    long k;
    ByteArrayLarge pClassTab(65536L);
    ArrayLarge<ClusterRec> pNewCluster(MAXCLASSES);
    HistBands.Resize(65536L);
    
    /* A temporary map is created for all combination of input band pixels.*/
    File filTmp(FileName::fnUnique(FileName(fnObj, ".tmp")), facCRT);
    filTmp.SetErase(true);
    filTmp.KeepOpen(true);
    SetDomainValueRangeStruct(Domain("image")); // remove internal domain from memory
    SetDomainValueRangeStruct(DomainValueRangeStruct(Domain(fnObj, iClasses, dmtCLASS, "Cluster")));
    CreateMapStore();
    
    initFeatureSpaceHistogram(iBands);
    
    initBandLineBuffers();
    
    // fill stretch arrays for the input maps,
    // output range 0..31 (5 bits, 3 bands)
    //    or        0..15 (4 bits, 4 bands)
    initStretchLookup();
    
    // calculate combined histogram of 3 input maps
    Buf<unsigned short> iTmpLine(iCols);
    ByteBuf bTmpLine(iCols);
    unsigned int iIndex;
    
    trq.SetText(SMAPTextCalcCompHist);
    trq.SetTitle(sFreezeTitle);
    trq.setHelpItem(htpFreeze);

    // Calculate the feature space histogram
    // and assign the featurespace index to each
    // pixel vector (in a temporary map)
    for (l = 0; l < iLines; l++) {
        if (l % 10 == 0) {
            if (trq.fUpdate(l, iLines)) 
                return false;
        }
        else if (trq.fAborted()) 
            return false;
        
        // read input bands in bLine
        for (j = 0; j < iBands; j++) 
            mpl[j + mpl->iLower()]->GetLineVal(l, bLine[j]);
        
        // calculate for each pixel combined value (after stretching)
        for (k = 0; k < iCols; k++) {
            iIndex = getFSIndex(k);
            HistBands[iIndex].iCount++;
            iTmpLine[k] = iIndex;
        }
		vector<unsigned short> dummy;
		for(int zz = 0; zz < iTmpLine.iSize(); ++zz)
			dummy.push_back(iTmpLine[zz]);
        filTmp.Write(2 * iCols, iTmpLine.buf());
    }
    trq.fUpdate(iLines, iLines);  // mark the tranquilizer to 100%
    
    /* check actual number of combinations in combined histogram
       and fill histogram array from the bottom up. */
    long iClusterComb = 0;
    long iFilled = 65535L; // 16 bit (2 bands x 8 or 4 bands x 4)
    if (iBands == 3)
        iFilled = 32767;   // 15 bit (3 bands x 5)
    for (long wi = 0; wi <= iFilled; wi++) {
        if (HistBands[wi].iCount != 0) { // existing entry
            HistBands[iClusterComb].iComb  =  HistBands[wi].iComb;
            HistBands[iClusterComb].iCount =  HistBands[wi].iCount;
            iClusterComb++;
        }
    }

    // calculate classes
    int iNewClusters = iClusterCalc(pNewCluster, iClusterComb, iLines * (long) iCols, iClasses);
    if (iNewClusters == 0)
        return 0;   // empty feature space (!), so we are done
    
    // pClassTab is reclassify table for mpTmp to mpOut
    for (long wi = 0; wi <= 65535L; wi++)
        pClassTab[(unsigned int)wi] = 0;
    
    // fill pClassTab
    for (i = 0; i < iNewClusters; i++)
        for (k = pNewCluster[i].iFirst; k < pNewCluster[i].iFirst + pNewCluster[i].iNumRec; k++)
            pClassTab[HistBands[k].iComb] = i + 1;
        
    // create output map from temp map using classify table pClassTab
    trq.SetText(SMAPTextWriteOutMap);
    filTmp.Seek(0);
    for (l=0; l<iLines; l++ ) {
        if (l % 10 == 0) {
            if (trq.fUpdate(l, iLines)) 
                return false;
        }
        else if (trq.fAborted()) 
            return false;

	    filTmp.Read(2 * iCols, iTmpLine.buf());
	
        for (k = 0; k < iCols; k++)
            bTmpLine[k] = pClassTab[(unsigned int)iTmpLine[k]];
        pms->PutLineRaw(l, bTmpLine);
    };
    trq.fUpdate(iLines, iLines);
    
    if (sStatTable.length() > 0) {
        writeStatisticsTable(sStatTable);
        
    }
    return true;
}

String MapCluster::sExpression() const
{
    if (sStatTable.length() == 0)
        return String("MapCluster(%S,%i)", mpl->sNameQuoted(true, fnObj.sPath()), iClasses);
    return String("MapCluster(%S,%i,%S)", mpl->sNameQuoted(true, fnObj.sPath()), iClasses, sStatTable);
}

static bool fLess(long i1, long i2, void* p)
{
    MapCluster* mp = static_cast<MapCluster*>(p);
    HistBandsRec h1, h2;
    h1 = mp->HistBands[i1];
    h2 = mp->HistBands[i2];
    byte bMax = mp->bMax;
    int iShift = mp->iShift;
    switch (mp->iBands) {
    case 1: 
        return h1.iComb  < h2.iComb;
    case 2: {
        const word MaskBands[4] = { bMax, bMax << iShift, 0, 0 };
        return (h1.iComb & MaskBands[mp->iSortBands]) < (h2.iComb & MaskBands[mp->iSortBands]);
            }  
    case 3: {
        const word MaskBands[4] = { bMax, bMax << iShift, bMax << (2 * iShift), 0 };
        return (h1.iComb & MaskBands[mp->iSortBands]) < (h2.iComb & MaskBands[mp->iSortBands]);
            }  
    case 4: {
        const word MaskBands[4] = { bMax, bMax << iShift, bMax << (2 * iShift), bMax << (3 * iShift) };
        return (h1.iComb & MaskBands[mp->iSortBands]) < (h2.iComb & MaskBands[mp->iSortBands]);
            }  
    }
    return false;
}

static void Swap(long i1, long i2, void* p)
{
    MapCluster* mp = static_cast<MapCluster*>(p);
    HistBandsRec h;
    h = mp->HistBands[i1];
    mp->HistBands[i1] = mp->HistBands[i2];
    mp->HistBands[i2] = h;
}


// pNewCluster : resulting list of ClusterCubeRecs
//   iClusterCmb : number of entries in HistBands
//   iTotal : total number of pixels (sum of iCount in each HistBands record)
//   iNumClusters : nr of desired classes
//   return: number of allocated classes (can be less than requested
//           if the entries can't be split) */
int MapCluster::iClusterCalc(ArrayLarge<ClusterRec>& pNewCluster, long iClusterComb, long iTotal, int iNumClusters)
{
    int i, j;     // counters
    long k;
    int iCubeIndex;  // cube index in pNewCluster
    int iNewClusters;
    word iMinCluster, iMaxCluster;
    
    for (i = 0; i < MAXCLASSES; i++) {
        pNewCluster[i].iFirst = -1;
        pNewCluster[i].iNumRec = 0;
        pNewCluster[i].iCount = 0;
        for (j = 0; j < iBands; j++) {
            pNewCluster[i].rBandsMin[j] = aarIndex2Value[j][0];
            pNewCluster[i].rBandsMax[j] = aarIndex2Value[j][bMax];
        };
    };
    
    pNewCluster[0].iFirst = 0;
    pNewCluster[0].iNumRec = iClusterComb;
    pNewCluster[0].iCount = iTotal;
    
    iNewClusters = 1;  // count for new nr. of cubes
    trq.SetText("Determine classes");
    while (iNewClusters < iNumClusters) {
        long iSum, iCnt;

        if (trq.fUpdate(iNewClusters)) 
            return 0;
        
        long iMaxSize = -1;
        /*  Selection by largest nr. of pxels in cube */
        for (i=0; i<iNewClusters; i++) {
            if ((pNewCluster[i].iCount > iMaxSize) &&
                (pNewCluster[i].iNumRec > 1) /* if == 1 it can't be split */ ) {
                iMaxSize = pNewCluster[i].iCount;
                iCubeIndex = i;
            }
        }
        if (iMaxSize == -1) break;

        // iCubeIndex contains now cube nr. to be split
        // determine largest cube side
        double rMaxWidth = -1;
        for (j = 0; j < iBands; j++) {
            double clusterSize = pNewCluster[iCubeIndex].rBandsMax[j] - pNewCluster[iCubeIndex].rBandsMin[j];
            if (clusterSize > rMaxWidth) {
                rMaxWidth = clusterSize;
                iSortBands = j;
            }
        }
        // iSortBands contains now cube side to be split

        // sort histogram records contained in cube iCubeIndex
        QuickSort(pNewCluster[iCubeIndex].iFirst,
            pNewCluster[iCubeIndex].iFirst + pNewCluster[iCubeIndex].iNumRec - 1,
            fLess, Swap, this);

        // and split
        // find pos, so that in both new cubes are approx. same nr. of pixels
        iSum = pNewCluster[iCubeIndex].iCount / 2;
        k = pNewCluster[iCubeIndex].iFirst;
        iCnt = HistBands[k].iCount;
        k++;
        for (;k <= pNewCluster[iCubeIndex].iFirst+pNewCluster[iCubeIndex].iNumRec-2; k++) {
            iCnt += HistBands[k].iCount;
            if (iCnt > iSum) {
                iCnt -= HistBands[k].iCount;
                break;
            }
        }
        // k now contains first hist. rec index of 'new' cube
        // iCnt now contains nr. of pixels in 'old' cube
        // adjust Bands values to 5 bits:
        iMaxCluster = HistBands[k - 1].iComb;
        iMinCluster = HistBands[k].iComb;

        iMinCluster = (iMinCluster >> (iSortBands * iShift)) & bMax;
        iMaxCluster = (iMaxCluster >> (iSortBands * iShift)) & bMax;

        // adjust current and old cube
        pNewCluster[iNewClusters].iFirst = k;
        pNewCluster[iNewClusters].iCount = pNewCluster[iCubeIndex].iCount - iCnt;
        pNewCluster[iNewClusters].iNumRec = pNewCluster[iCubeIndex].iNumRec -
            (k - pNewCluster[iCubeIndex].iFirst);

        pNewCluster[iCubeIndex].iCount = iCnt;
        pNewCluster[iCubeIndex].iNumRec -=  pNewCluster[iNewClusters].iNumRec;
        for (j=0; j<iBands; j++) {
            pNewCluster[iNewClusters].rBandsMin[j] = pNewCluster[iCubeIndex].rBandsMin[j];
            pNewCluster[iNewClusters].rBandsMax[j] = pNewCluster[iCubeIndex].rBandsMax[j];
        }
        pNewCluster[iNewClusters].rBandsMin[iSortBands] = aarIndex2Value[iSortBands][iMinCluster];//+1;
        pNewCluster[iCubeIndex].rBandsMax[iSortBands] = aarIndex2Value[iSortBands][iMaxCluster];
        iNewClusters++;
    }
    
    trq.fUpdate(iNewClusters);
    
    return iNewClusters;
}
