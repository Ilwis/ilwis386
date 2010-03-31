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
/*// $Log: /ILWIS 3.0/RasterApplication/MAPHECK.cpp $
 * 
 * 5     5/01/00 14:43 Willem
 * Increased the size of the pClassTab and HistRGB arrays to the really
 * necessary value 
 * 
 * 4     30-11-99 12:21 Wind
 * added local copy of Representation to prevent 'inline deletion' of
 * representation object
 * 
 * 3     9/08/99 11:51a Wind
 * comment problem
 * 
 * 2     9/08/99 8:57a Wind
 * changed sName() to sNameQuoted() in sExpression() to support long file
 * names
*/
// Revision 1.4  1998/09/16 17:24:39  Wim
// 22beta2
//
// Revision 1.3  1997/08/15 18:43:18  Wim
// Only 2 parameters allowed for MapHeckbert()
//
// Revision 1.2  1997-08-05 17:47:15+02  Wim
// sSyntax() corrected
//
/* MapHeckbert
   Copyright Ilwis System Development ITC
   july 1995, by Wim Koolhoven
	Last change:  WK   15 Aug 97    8:43 pm
*/
#define MAPHECK_C
#include "Applications\Raster\MAPHECK.H"
#include "Engine\Representation\Rprclass.h"
#include "Engine\Base\Algorithm\Qsort.h"
#include "Headers\Htp\Ilwisapp.htp"
#include "Headers\Err\Ilwisapp.err"
#include "Headers\Hs\map.hs"

IlwisObjectPtr * createMapHeckbert(const FileName& fn, IlwisObjectPtr& ptr, const String& sExpr, vector<void *> parms ) {
	if ( sExpr != "")
		return (IlwisObjectPtr *)MapHeckbert::create(fn, (MapPtr &)ptr, sExpr);
	else
		return (IlwisObjectPtr *)new MapHeckbert(fn, (MapPtr &)ptr);
}

const char* MapHeckbert::sSyntax() { return "MapHeckbert(maplist,colors)"; }

MapHeckbert* MapHeckbert::create(const FileName& fn, MapPtr& p, const String& sExpr)
{
  Array<String> as;
  int iParms = IlwisObjectPtr::iParseParm(sExpr, as);
  if (iParms != 2)
    ExpressionError(sExpr, sSyntax());
  MapList mpl(as[0], fn.sPath());
  int iColors = as[1].iVal();
  String sDom = fn.sFile;
  return new MapHeckbert(fn, p, mpl, iColors);
}

MapHeckbert::MapHeckbert(const FileName& fn, MapPtr& p)
: MapFromMapList(fn, p)
{
  ReadElement("MapHeckbert", "Colors", iNumColors);
  fNeedFreeze = true;
  sFreezeTitle = "MapHeckbert";
  htpFreeze = htpMapHeckbertT;
}

MapHeckbert::MapHeckbert(const FileName& fn, MapPtr& p, const MapList& mpl, int iColors)
: MapFromMapList(fn, p, mpl, Domain("image")/* just temp, see below*/), iNumColors(iColors)
{
  if (iColors < 2)
    throw ErrorObject(WhatError(String(SMAPErrTooFewColors_i.scVal(), iNumColors), errMapHeckbert+1), sTypeName());
  if (iColors > 255)
    throw ErrorObject(WhatError(String(SMAPErrTooManyColors_i.scVal(), iNumColors), errMapHeckbert+2), sTypeName());
  // check on image domain
  Map mp = mpl->map(mpl->iLower());
  if (0 == mp->dm()->pdi())
    throw ErrorImageDomain(mp->dm()->sName(true, fnObj.sPath()), mp->fnObj, errMapHeckbert+3);
  mp = mpl->map(mpl->iLower()+1);
  if (0 == mp->dm()->pdi())
    throw ErrorImageDomain(mp->dm()->sName(true, fnObj.sPath()), mp->fnObj, errMapHeckbert+3);
  mp = mpl->map(mpl->iLower()+2);
  if (0 == mp->dm()->pdi())
    throw ErrorImageDomain(mp->dm()->sName(true, fnObj.sPath()), mp->fnObj, errMapHeckbert+3);
//  SetDomainValueRangeStruct(DomainValueRangeStruct(Domain(FileName(sDom, ".dom", true), iColors+1, dmtPICTURE)));
  SetDomainValueRangeStruct(DomainValueRangeStruct(Domain(fnObj, iColors+1, dmtPICTURE)));
  fNeedFreeze = true;
  sFreezeTitle = "MapHeckbert";
  htpFreeze = htpMapHeckbertT;
  if (!fnObj.fValid())
    objtime = objdep.tmNewest();
//Store();
}

void MapHeckbert::Store()
{
  MapFromMapList::Store();
  WriteElement("MapFromMapList", "Type", "MapHeckbert");
  WriteElement("MapHeckbert", "Colors", (long)iNumColors);
}

MapHeckbert::~MapHeckbert()
{
}

#define NRENT  32767
#define COLORS 256
#define NRBUF 8
#define MAXBUF 4096
// Note : always NRBUF * MAXBUF = 32768

void MapHeckbert::InitName(const FileName& fn)
{
  SetDomainValueRangeStruct(DomainValueRangeStruct(Domain(fn, iNumColors+1, dmtPICTURE)));
}

/* For the three input bands a combined histogram is calculated. Of each pixel
   (byte) the least significant 3 bits are ignored. The histogram entries are
   defined by the struct HistRGBRec.
   The complete histogram contains 32768 entries of 4 bytes and they are split
   in 8 subarrays of 4096 records : see HistRGB.
*/
bool MapHeckbert::fFreezing()
{
  RowCol rc = rcSize();
  long iCols = rc.Col;
  long iLines = rc.Row;
  int i, j, k, l;          // counters
  ByteArray pClassTab(NRBUF * MAXBUF);
  ArrayLarge<ColorCubeRec> pNewColor(COLORS);
  HistRGB.Resize(NRBUF * MAXBUF);

  /* A temporary map is created for all combination of input band pixels.
     Values are 3*5 bits, so we need an integer map.
     Name for this temp map is derived from process id. */
  Map mpTmp(FileName::fnUnique(fnObj), gr(), rcSize(), DomainValueRangeStruct(Domain("value"), ValueRange(-32766,32767)));
  mpTmp->fErase = true;

  for (i=0; i < 32; i++)
    for (j=0; j < 32; j++)
      for (k=0; k < 32; k++) {
        unsigned int x = k + (j << 5) + (unsigned)(i << 10);
        HistRGB[x].iCount = 0;
        HistRGB[x].iRGB = x;
      }

  ByteBuf bLine[3];        // place holder for lines for each input band
  for (j=0; j<3; j++) 
    bLine[j].Size(iCols);

  ByteBuf bStrArr[3];   // place holder for stretch array for each input band
  // fill stretch arrays for 3 input maps, output range 0..31 (5 bits)
  for (j=0; j<3; j++) {
    bStrArr[j].Size(256);
    RangeInt rng = mpl[j+mpl->iLower()]->riPerc1(true); // force calculation
    k = rng.iLo();
    l = rng.iHi();
    // k and l contain 1% range of histogram
    for (i = 0; i < k; i++)  // fill with zero until min. 1% ( is k)
      bStrArr[j][i] = 0;
    if (l != k)
      for (i = k; i <= l; i++) {  // interpolate in between
        float rl = (((float)(i-k)) / (l-k)) * 255;
        bStrArr[j][i] = ((int)rl) >> 3;  // from 0..255 to 0..31
      }
    else
      bStrArr[j][l] = 31;
    for (i = l+1; i <= 255; i++) // fill with 31 starting from max. 1% ( is l)
      bStrArr[j][i] = 31;
  }
  // calculate combined histogram of 3 input maps
  IntBuf iTmpLine(iCols);
  ByteBuf bTmpLine(iCols);
  unsigned int iIndex;
  trq.SetText(SMAPTextCalcCompHist);
  trq.SetTitle(sFreezeTitle);
  trq.SetHelpTopic(htpFreeze);
  for (l=0; l<iLines; l++) {
    if (l % 10 == 0) {
      if (trq.fUpdate(l, iLines)) 
        return false;
    }
    else if (trq.fAborted()) 
      return false;
    // read input bands in bLine
    for (j=0; j<3; j++) 
      mpl[j+mpl->iLower()]->GetLineRaw(l, bLine[j]);
    // calculate for each pixel combined value (after stretching)
    for (k=0; k<iCols; k++) {
      iIndex = (unsigned int)bStrArr[0][bLine[0][k]] +
               ((unsigned int)bStrArr[1][bLine[1][k]]<<5) +
               ((unsigned int)bStrArr[2][bLine[2][k]]<<10);
      // iIndex contains now 15 bit value: 0rrrrrgggggbbbbb
      // adjust histogram
      HistRGB[iIndex].iCount++;
      // fill pixel in line for temporary map
      iTmpLine[k] = iIndex;
    }
    // save line in temp map
    mpTmp->PutLineRaw(l, iTmpLine);
  }
  if (trq.fUpdate(iLines, iLines))
    return false;
  /* check actual number of combinations in combined histogram
     and fill histogram array from the bottom up. */
  long iColorComb = 0;
  for (long wi=0; wi<=NRENT; wi++)
    if (HistRGB[wi].iCount != 0) { // existing entry
      HistRGB[iColorComb].iRGB   =  HistRGB[wi].iRGB;
      HistRGB[iColorComb].iCount =  HistRGB[wi].iCount;
      iColorComb++;
    }
  // calculate colors
  short iNewColors = iHeckCalc(pNewColor, iColorComb,
                               iLines * (long) iCols, iNumColors);
  if (!iNewColors) return 0;
  // pClassTab is reclassify table for mpTmp to mpOut
  for (long wi=0; wi<=NRENT; wi++)
    pClassTab[(unsigned int)wi] = 0;

  // fill pClassTab
  for (i=0; i<iNewColors; i++)
    for (k=pNewColor[i].iFirst; k<pNewColor[i].iFirst+pNewColor[i].iNumRec; k++)
      pClassTab[HistRGB[k].iRGB] = i + 1;

  // fill representation
  Representation rpr = dm()->rpr(); // needed because next statement would call destructor of Representation
//  RepresentationClass* rprc = dm()->rpr()->prc();
  RepresentationClass* rprc = rpr->prc();
  if (0 != rprc)
    for (i=0; i<iNewColors; i++) { // for each color cube
      // color index 0 is black:
      double r = 0, g = 0, b = 0;
      HistRGBRec ha;  // temporary place holder
      // calculate weighted mean of all records in cube:
      for (k = pNewColor[i].iFirst;
           k < pNewColor[i].iFirst + pNewColor[i].iNumRec; k++)
      {
        ha = HistRGB[k];
        r += ha.iCount * (long)(ha.iRGB & 31);
        g += ha.iCount * (long)((ha.iRGB >> 5) & 31);
        b += ha.iCount * (long)((ha.iRGB >> 10) & 31);
      }
      r /= pNewColor[i].iCount;
      r *= 255.0 / 31;
      g /= pNewColor[i].iCount;
      g *= 255.0 / 31;
      b /= pNewColor[i].iCount;
      b *= 255.0 / 31;
      rprc->PutColor(i+1, Color(byteConv(r),byteConv(g), byteConv(b)));
    }
  // create output map from temp map using classify table pClassTab
  trq.SetText(SMAPTextWriteOutMap);
  for (l=0; l<iLines; l++ ) {
    if (l % 10 == 0) {
      if (trq.fUpdate(l, iLines)) 
        return false;
    }
    else if (trq.fAborted()) 
      return false;
    mpTmp->GetLineRaw(l, iTmpLine);
    for (k=0; k<iCols; k++)
      bTmpLine[k] = pClassTab[iTmpLine[k]];
    pms->PutLineRaw(l, bTmpLine);
  };
  trq.fUpdate(iLines, iLines);

  return true;
}

String MapHeckbert::sExpression() const
{
  String sMpl;
  if (fnObj.fValid())
    sMpl = mpl->sNameQuoted(true, fnObj.sPath());
  else
    sMpl = mpl->sNameQuoted(true, String());
  return String("MapHeckbert(%S,%i)", sMpl, iNumColors);
}

static bool fLess(long i1, long i2, void* p)
{
  MapHeckbert* mp = static_cast<MapHeckbert*>(p);
  int x1 = mp->HistRGB[i1].iRGB;
  int x2 = mp->HistRGB[i2].iRGB;
  switch (mp->iSortRGB) {
    case 0: 
      break;
    case 1: 
      x1 >>= 5;
      x2 >>= 5;
      break;
    case 2: 
      x1 >>= 10;
      x2 >>= 10;
      break;
  }
  x1 &= 31;
  x2 &= 31;
  return x1<x2;
}

static void Swap(long i1, long i2, void* p)
{
  MapHeckbert* mp = static_cast<MapHeckbert*>(p);
  HistRGBRec h;
  h = mp->HistRGB[i1];
  mp->HistRGB[i1] = mp->HistRGB[i2];
  mp->HistRGB[i2] = h;
}


int MapHeckbert::iHeckCalc(ArrayLarge<ColorCubeRec>& pNewColor, int iColorComb, long iTotal, int iNumColors)
/* pNewColor : resulting list of ColorCubeRecs
   iColorCmb : number of entries in HistRGB
   iTotal : total number of pixels (sum of iCount in each HistRGB record)
   iNumColors : nr of desired colors
   routine returns number of allocated colors (can be less than desired colors
   if the entries can't be split) */

{ int i, j, k;     // counters
  int iCubeIndex;  // cube index in pNewColor
  int iNewColors;
  for (i=0; i<COLORS; i++) {
    pNewColor[i].iFirst = -1;
    pNewColor[i].iNumRec = 0;
    pNewColor[i].iCount = 0;
    for (j=0; j<3; j++) {
      pNewColor[i].iRGBMin[j] = 0;
      pNewColor[i].iRGBMax[j] = 31;
    };
  };

  pNewColor[0].iFirst = 0;
  pNewColor[0].iNumRec = iColorComb;
  pNewColor[0].iCount = iTotal;

  iNewColors = 1;  // count for new nr. of cubes
  trq.SetText(SMAPTextDetermineColors);
  while (iNewColors < iNumColors) {
    long iSum, iCnt;
//    if ((iNewColors < 10) || !(iNewColors % 10))
    if (trq.fUpdate(iNewColors)) 
      return 0;

    long iMaxSize = -1;
    /*  Selection by largest nr. of pxels in cube */
    for (i=0; i<iNewColors; i++)
      if ((pNewColor[i].iCount > iMaxSize) &&
          (pNewColor[i].iNumRec > 1) /* if == 1 it can't be split */ ) {
        iMaxSize = pNewColor[i].iCount;
        iCubeIndex = i;
      }
    if (iMaxSize == -1) break;
    // iCubeIndex contains now cube nr. to be split
    // determine largest cube side
    iMaxSize = -1;
    for (j=0; j<3; j++)  // for red, green and blue
      if (pNewColor[iCubeIndex].iRGBMax[j]-pNewColor[iCubeIndex].iRGBMin[j] > iMaxSize) {
        iMaxSize = pNewColor[iCubeIndex].iRGBMax[j] - pNewColor[iCubeIndex].iRGBMin[j];
        iSortRGB = j;
      }
    // iSortRGB contains now cube side to be split
  /* alternative: Selection by longest box side
    for (i=0; i<iNewColors; i++)
      for (j=0; j<3; j++)
        if ((pNewColor[i].iRGBMax[j]-pNewColor[i].iRGBMin[j] > iMaxSize) &&
                                        (pNewColor[i].iNumRec > 1) ) {
          iMaxSize = pNewColor[i].iRGBMax[j] - pNewColor[i].iRGBMin[j];
          iCubeIndex = i;
          iSortRGB = j;
        }
    if (iMaxSize==-1) break;
  */
    // sort histgram records contained in cube iCubeIndex
    QuickSort(pNewColor[iCubeIndex].iFirst,
              pNewColor[iCubeIndex].iFirst+pNewColor[iCubeIndex].iNumRec-1,
              fLess, Swap, this);
//              (IlwisObjectPtrLessProc)&MapHeckbert::fLess, 
//              (IlwisObjectPtrSwapProc)&MapHeckbert::Swap);
    // and split
    // find pos, so that in both new cubes are approx. same nr. of pixels
    iSum = pNewColor[iCubeIndex].iCount / 2;
    k = pNewColor[iCubeIndex].iFirst;
    iCnt = HistRGB[k].iCount;
    k++;
    for (;k <= pNewColor[iCubeIndex].iFirst+pNewColor[iCubeIndex].iNumRec-2;k++)
    { iCnt += HistRGB[k].iCount;
      if (iCnt > iSum) {
        iCnt -= HistRGB[k].iCount;
        break;
      }
    }
    // k now contains first hist. rec index of 'new' cube
    // iCnt now contains nr. of pixels in 'old' cube
    // adjust RGB values to 5 bits:
    int iMaxColor = HistRGB[k-1].iRGB;
    int iMinColor = HistRGB[k].iRGB;
    switch (iSortRGB) {
      case 0 : iMinColor = iMinColor & 31;
               iMaxColor = iMaxColor & 31;
               break;
      case 1 : iMinColor = (iMinColor>>5) & 31;
               iMaxColor = (iMaxColor>>5) & 31;
               break;
      case 2 : iMinColor = (iMinColor>>10) & 31;
               iMaxColor = (iMaxColor>>10) & 31;
               break;
    }
    // adjust current and old cube
    pNewColor[iNewColors].iFirst = k;
    pNewColor[iNewColors].iCount = pNewColor[iCubeIndex].iCount - iCnt;
//    assert(pNewColor[iNewColors].iCount > 0);
    pNewColor[iNewColors].iNumRec = pNewColor[iCubeIndex].iNumRec -
                                         (k - pNewColor[iCubeIndex].iFirst);
    pNewColor[iCubeIndex].iCount = iCnt;
    pNewColor[iCubeIndex].iNumRec -=  pNewColor[iNewColors].iNumRec;
    for (j=0; j<3; j++) {
      pNewColor[iNewColors].iRGBMin[j] = pNewColor[iCubeIndex].iRGBMin[j];
      pNewColor[iNewColors].iRGBMax[j] = pNewColor[iCubeIndex].iRGBMax[j];
    }
    pNewColor[iNewColors].iRGBMin[iSortRGB] = iMinColor;
    pNewColor[iCubeIndex].iRGBMax[iSortRGB] = iMaxColor;
    iNewColors++;
  }

  trq.fUpdate(iNewColors);

  return iNewColors;
}




