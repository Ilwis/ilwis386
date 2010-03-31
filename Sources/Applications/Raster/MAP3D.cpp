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
// $Log: /ILWIS 3.0/RasterApplication/MAP3D.cpp $
 * 
 * 5     9-03-01 12:47 Hendrikse
 * corrected " if (trq.fUpdate(iRun, iRuns)) " to make the tranquilizer
 * run forward
 * 
 * 4     2-03-01 9:14a Martin
 * array bounds were overwritten by some points. Made the array somewhat
 * larger
 * 
 * 3     9/08/99 11:51a Wind
 * comment problem
 * 
 * 2     9/08/99 8:51a Wind
 * changed sName( to sNameQuoted( in sExpression()
*/
// Revision 1.6  1998/09/16 17:24:31  Wim
// 22beta2
//
// Revision 1.5  1997/09/08 15:16:22  janh
// *** empty log message ***
//
// Revision 1.4  1997/09/05 14:41:09  janh
// renamed Coord2RowCol into Crd2RC in MakePoints( )
//
// Revision 1.3  1997/09/04 09:37:55  janh
// try catch removed from constructor MapApply3D(fn,p)
//
// Revision 1.2  1997/09/03 10:18:30  Wim
// Check if georef is really a GeoRef3D
//
/* MapApply3D
   Copyright Ilwis System Development ITC
	Last change:  JHE   8 Sep 97    4:16 pm
*/

#include "Applications\Raster\MAP3D.H"
#include "Engine\SpatialReference\GR3D.H"
#include "Headers\Htp\Ilwisapp.htp"
#include "Headers\Err\Ilwisapp.err"
#include "Headers\Hs\map.hs"

IlwisObjectPtr * createMapApply3D(const FileName& fn, IlwisObjectPtr& ptr, const String& sExpr, vector<void *> parms ) {
	if ( sExpr != "")
		return (IlwisObjectPtr *)MapApply3D::create(fn, (MapPtr &)ptr, sExpr);
	else
		return (IlwisObjectPtr *)new MapApply3D(fn, (MapPtr &)ptr);
}


const char * MapApply3D::sSyntax()
 { return "MapApply3D(map,georef3d)"; }

static void GeoRefNoneError(const FileName& fn, IlwisError err)
{
  throw ErrorGeoRefNone(fn, err);
}

static void NoGeoRef3DError(const FileName& fn)
{
  throw ErrorObject(WhatError(SMAPErrGeoRef3DRequired, errMapApply3D+2), fn);
}

MapApply3D* MapApply3D::create(const FileName& fn, MapPtr& p, const String& sExpr)
{
  Array<String> as(2);
  if (!IlwisObjectPtr::fParseParm(sExpr, as))
    ExpressionError(sExpr, sSyntax());
  Map mp(as[0], fn.sPath());
  GeoRef gr = GeoRef(as[1], fn.sPath());

  return new MapApply3D(fn, p, mp, gr);
}

MapApply3D::MapApply3D(const FileName& fn, MapPtr& p)
: MapFromMap(fn, p)
{
  String s;
  fNeedFreeze = true;
  ReadGeoRefFromFileAndSet();
  if (mp->gr()->fGeoRefNone())
    GeoRefNoneError(mp->fnObj, errMapApply3D);
  if (!cs()->fConvertFrom(mp->gr()->cs()))
    IncompatibleCoordSystemsError(cs()->sName(true, fnObj.sPath()), mp->gr()->cs()->sName(true, fnObj.sPath()), sTypeName(), errMapApply3D+1);
  Init();
  objdep.Add(gr().ptr());
}

MapApply3D::MapApply3D(const FileName& fn, MapPtr& p, const Map& mp, const GeoRef& gr)
: MapFromMap(fn, p, mp)
{
  if (mp->gr()->fGeoRefNone())
    GeoRefNoneError(mp->fnObj, errMapApply3D);
  if (!gr->cs()->fConvertFrom(mp->gr()->cs()))
    IncompatibleCoordSystemsError(gr->cs()->sName(true, fnObj.sPath()), mp->gr()->cs()->sName(true, fnObj.sPath()), sTypeName(), errMapApply3D+1);
  ptr.SetSize(gr->rcSize());
  ptr.SetGeoRef(gr);
  fNeedFreeze = true;
  Init();
  objdep.Add(gr.ptr());
  if (!fnObj.fValid())
    objtime = objdep.tmNewest();
}

void MapApply3D::Init()
{
  sFreezeTitle = "MapApply3D";
  htpFreeze = htpMapApply3DT;
  if (0 == gr()->pg3d())
    NoGeoRef3DError(gr()->fnObj);
}

void MapApply3D::Store()
{
  MapFromMap::Store();
  WriteElement("MapFromMap", "Type", "MapApply3D");
}

MapApply3D::~MapApply3D()
{
}

String MapApply3D::sExpression() const
{
  return String("MapApply3D(%S,%S)",
                 mp->sNameQuoted(true, fnObj.sPath()), gr()->sNameQuoted(true, fnObj.sPath()));
}
#define TopLeft 0
#define TopRight 1
#define BottomLeft 2
#define BottomRight 3

bool MapApply3D::fFreezing()
{
  trq.SetText(SMAPTextInitMap);
  pms->FillWithUndef();

  grf3d = gr()->pg3d();
  if (0 == grf3d)
    NoGeoRef3DError(gr()->fnObj);
  fSameGrf = grf3d->mapDTM->gr() == mp->gr();

  // determine direction of drawing: top <-> bottom ; left <-> right
  RowCol rc[4], rcCorners[4];
  rc[TopLeft]=RowCol(0L, 0L);
  rc[TopRight]=RowCol(0L, mp->iCols()-1L);
  rc[BottomRight]=RowCol(mp->iLines()-1L, mp->iCols()-1L);
  rc[BottomLeft]=RowCol(mp->iLines()-1L, 0L);
  rcCorners[TopLeft]=grf3d->rcConv(mp->gr()->cConv(rc[TopLeft]));
  rcCorners[TopRight]=grf3d->rcConv(mp->gr()->cConv(rc[TopRight]));
  rcCorners[BottomRight]=grf3d->rcConv(mp->gr()->cConv(rc[BottomRight]));
  rcCorners[BottomLeft]=grf3d->rcConv(mp->gr()->cConv(rc[BottomLeft]));

  bInpBuf.Size(mp->iCols()+1);
  shInpBuf.Size(mp->iCols()+1);
  iInpBuf.Size(mp->iCols()+1);
  rInpBuf.Size(mp->iCols()+1);
  bOutBuf.Size(iCols());
  shOutBuf.Size(iCols());
  iOutBuf.Size(iCols());
  rOutBuf.Size(iCols());
  rcTopBuf.Size(mp->iCols()+1);
  rcBottomBuf.Size(mp->iCols()+1);

  bool fTop2Bottom, fLeft2Right;
  int iCrn = 0;
  for (int ic=1; ic <=3; ++ic) {
    if (rcCorners[ic].Row < rcCorners[iCrn].Row)
      iCrn = ic;
  }
  switch ( iCrn) {
    case TopLeft:
      fTop2Bottom = true;
      fLeft2Right = true;
      break;
    case TopRight:
      fTop2Bottom = true;
      fLeft2Right = false;
      break;
    case BottomRight:
      fTop2Bottom = false;
      fLeft2Right = false;
      break;
    case BottomLeft:
      fTop2Bottom = false;
      fLeft2Right = true;
      break;
  }
  
  long iRowDelta, iColDelta;
  long iStartRow, iStartCol, iEndRow, iEndCol;
  if (fTop2Bottom) {
    iRowDelta = 1;
    iStartRow = 0;
    iEndRow = mp->iLines()-1;
  }
  else {
    iRowDelta = -1;
    iStartRow = mp->iLines()-1;
    iEndRow = 0;
  }
  if (fLeft2Right) {
    iColDelta = 1;
    iStartCol = 0;
    iEndCol = mp->iCols()-1;
  }
  else {
    iColDelta = -1;
    iStartCol = mp->iCols()-1;
    iEndCol = 0;
  }

  // start drawing
  long iRuns = mp->iLines()+1;
  long iRow = iStartRow;
  short iTopN=0, iBottomN=0, n=0;
  trq.SetText(String(SMAPTextCreate3D_S.scVal(), mp->sName(true, mp->fnObj.sPath())));
  aiCol1.Resize(iLines() + 2);
  aiCol2.Resize(iLines() + 2);
  for (unsigned long i=0; i < aiCol1.iSize(); ++i) {
    aiCol1[i] = aiCol2[i] = iUNDEF;
  }
  for (long iRun = 0; iRun < iRuns; ++iRun) {
    RowCol rc1, rc2;
    if (trq.fUpdate(iRun, iRuns))
      return false;

    MakePoints(iRow, iStartCol, iEndCol, iBottomN, iColDelta);

    if (iRun > 0) {
      n=min(iTopN, iBottomN);
      DrawPolygons(n);
    }
    Swap(rcTopBuf, rcBottomBuf);
    iTopN=iBottomN;
    iRow += iRowDelta;
    if (iRowDelta < 0)
      iRow = max(0, iRow);
    else
      iRow = min(iEndRow, iRow);
  }
  return true;
}

void MapApply3D::MakePoints(long iRow, long iStartCol, long iEndCol, short& n, long iColDelta)
{
  long iCols = mp->iCols()+1;
  n = (short)iCols;
  double rRow, rCol;
  RealBuf rHeightBuf;
  int iBufSize = iCols;
  rHeightBuf.Size(iBufSize);
  if (fSameGrf) {
    grf3d->mapDTM->GetLineVal(iRow, rHeightBuf, min(iStartCol, iEndCol), iBufSize);
    rHeightBuf[iBufSize-1] = rHeightBuf[iBufSize-2];
  }
  switch ( st() ) {
    case stBYTE: case stNIBBLE: case stDUET: case stBIT :
      mp->GetLineRaw(iRow, bInpBuf, min(iStartCol, iEndCol), iBufSize);
      bInpBuf[iBufSize-1] = bInpBuf[iBufSize-2];
      break;
    case stINT:
      mp->GetLineRaw(iRow, shInpBuf, min(iStartCol, iEndCol), iBufSize);
      shInpBuf[iBufSize-1] = shInpBuf[iBufSize-2];
      break;
    case stLONG:
      mp->GetLineRaw(iRow, iInpBuf, min(iStartCol, iEndCol), iBufSize);
      iInpBuf[iBufSize-1] = iInpBuf[iBufSize-2];
      break;
    case stREAL:
      mp->GetLineVal(iRow, rInpBuf, min(iStartCol, iEndCol), iBufSize);
      rInpBuf[iBufSize-1] = rInpBuf[iBufSize-2];
      break;
  }
  RowCol rc(iRow, iStartCol);
  for (long j=0; j < iCols; ++j) {
    Coord c;
    mp->gr()->RowCol2Coord(rc.Row, rc.Col, c);
    if (fSameGrf)
      grf3d->Crd2RC(c, rRow, rCol, rHeightBuf[rc.Col]);
    else
      grf3d->Coord2RowCol(c, rRow, rCol);
    rcBottomBuf[j] = RowCol(rRow, rCol);
/*    switch ( st() ) {
      case stBYTE: case stNIBBLE: case stDUET: case stBIT :
        bInpBuf[j] = bInpBuf[rc.Col];
        break;
      case stINT:
        shInpBuf[j] = shInpBuf[rc.Col];
        break;
      case stLONG:
        iInpBuf[j] = iInpBuf[rc.Col];
        break;
      case stREAL:
        rInpBuf[j] = rInpBuf[rc.Col];
        break;
    }*/
    rc.Col += iColDelta;
    if (iColDelta < 0)
      rc.Col = max(0L, rc.Col);
    else
      rc.Col = min((long)iBufSize-1, rc.Col);
  }
  if (iColDelta < 0) {
    int j;
    switch ( st() ) {
      case stBYTE: case stNIBBLE: case stDUET: case stBIT :
        for (j=0; j < iBufSize/2; j++) {
          byte b = bInpBuf[j];
          bInpBuf[j] = bInpBuf[iBufSize-j-1];
          bInpBuf[iBufSize-j-1] = b;
        }
        break;
      case stINT:
        for (j=0; j < iBufSize/2; j++) {
          short sh = shInpBuf[j];
          shInpBuf[j] = shInpBuf[iBufSize-j-1];
          shInpBuf[iBufSize-j-1] = sh;
        }
        break;
      case stLONG:
        for (j=0; j < iBufSize/2; j++) {
          long i = iInpBuf[j];
          iInpBuf[j] = iInpBuf[iBufSize-j-1];
          iInpBuf[iBufSize-j-1] = i;
        }
        break;
      case stREAL:
        for (j=0; j < iBufSize/2; j++) {
          double r = rInpBuf[j];
          rInpBuf[j] = rInpBuf[iBufSize-j-1];
          rInpBuf[iBufSize-j-1] = r;
        }
       break;
    }
  }

}

void MapApply3D::DrawPolygons(long n)
{
  MinMax mm(RowCol(-100L, -100L), RowCol(iLines()+100, iCols()+100));
  RowColBuf rcCorners(5);
  int j;
  long iRaw;
  short shRaw;
  byte bRaw;
  double rVal;
  for (int i=0; i<n-1 ;++i ) {
    rcCorners[0] = rcTopBuf[i];
    rcCorners[1] = rcTopBuf[i+1];
    rcCorners[2] = rcBottomBuf[i+1];
    rcCorners[3] = rcBottomBuf[i];
    rcCorners[4] = rcCorners[0];
    if ((rcCorners[0].Row >= 32) && (rcCorners[0].Col >= 396))
      rcCorners[0] = rcCorners[4];

    // determine min and max row and col number
    long iColMax = iUNDEF;
    long iRowMax = iUNDEF;
    for (j=0; j < 4 ; j++) {
      iColMax = max(iColMax, rcCorners[j].Col);
      iRowMax = max(iRowMax, rcCorners[j].Row);
    }
    long iColMin = LONG_MAX;
    long iRowMin = LONG_MAX;
    for (j=0; j < 4 ; j++) {
      iColMin = min(iColMin, rcCorners[j].Col);
      iRowMin = min(iRowMin, rcCorners[j].Row);
    }
    if ((iRowMax < 0) || (iRowMin >= iLines()))
      continue;
    if ((iColMax < 0) || (iColMin >= iCols()))
      continue;
    RowCol rc;
    switch ( st() ) {
      case stBYTE: case stNIBBLE: case stDUET: case stBIT :
        bRaw = bInpBuf[i];
        iRaw = bRaw;
        break;
      case stINT:
        shRaw = shInpBuf[i];
        iRaw = longConv(shRaw);
        break;
      case stLONG:
        iRaw = iInpBuf[i];
        break;
      case stREAL:
        rVal = rInpBuf[i];
        break;
    }

    // first check the three simple cases
    if (iColMax == iColMin) { // vertical line
      if ((iColMax < 0) || (iColMin >= iCols()))
        continue;
      rc.Col = iColMin;
      rc.Row = iRowMin;
      iRowMax = min(iLines()-1, iRowMax);
      iRowMin = max(0, iRowMin);
      if (fUseReals())
        for (j=iRowMin; j <= iRowMax; ++j, ++rc.Row)
          pms->PutVal(rc, rVal);
      else
        for (j=iRowMin; j <= iRowMax; ++j, ++rc.Row)
          pms->PutRaw(rc, iRaw);
      continue;
    }
    if (iRowMax == iRowMin) { // horizontal line
      if ((iRowMax < 0) || (iRowMin >= iLines()))
        continue;
      iColMax = min(iCols()-1, iColMax);
      iColMin = max(0, iColMin);
      rc.Col = iColMin;
      rc.Row = iRowMin;
      if (fUseReals())
        for (j=iColMin; j <= iColMax; ++j, ++rc.Col)
          pms->PutVal(rc, rVal);
      else
        for (j=iColMin; j <= iColMax; ++j, ++rc.Col)
          pms->PutRaw(rc, iRaw);
      continue;
    }

    if ((iRowMax == iRowMin+1) && (iColMax == iColMin+1)) {
       // 2, 3, or 4 pixels in 2x2 square
      if (fUseReals()) {
        pms->PutVal(rcCorners[0], rVal);
        for (j=1; j <= 3 ; j++) {
          bool fAlreadyDone = rcCorners[0] == rcCorners[j];
          if (!fAlreadyDone)
            for (int k=1; k < j; k++)
              if (rcCorners[j] == rcCorners[k]) {
                fAlreadyDone = true;
                break;
              }
          if (!fAlreadyDone)
            pms->PutVal(rcCorners[j], rVal);
        }
      }
      else {
        pms->PutRaw(rcCorners[0], iRaw);
        for (j=1; j <= 3 ; j++) {
          bool fAlreadyDone = rcCorners[0] == rcCorners[j];
          if (!fAlreadyDone)
            for (int k=1; k < j; k++)
              if (rcCorners[j] == rcCorners[k]) {
                fAlreadyDone = true;
                break;
              }
          if (!fAlreadyDone)
            pms->PutRaw(rcCorners[j], iRaw);
        }
      }
      continue;
    }
    // more complex case
    bool f = true;
    for (j=0; j < 4 ; j++)
      f = f && mm.fContains(rcCorners[j]);
    if (!f)
      continue; // skip polygon
    SetLineArrays(rcCorners[0], rcCorners[1]);
    SetLineArrays(rcCorners[1], rcCorners[2]);
    SetLineArrays(rcCorners[0], rcCorners[3]);
    SetLineArrays(rcCorners[3], rcCorners[2]);
    switch ( st() ) {
      case stBYTE: case stNIBBLE: case stDUET: case stBIT :
        for (j=0;  j < iColMax-iColMin+1; ++j)
          bOutBuf[j] = bRaw;
        DrawPolygonFromArrays(iRowMin, iRowMax, bRaw);
        break;
      case stINT:
        for (j=0;  j < iColMax-iColMin+1; ++j)
          shOutBuf[j] = shRaw;
        DrawPolygonFromArrays(iRowMin, iRowMax, shRaw);
        break;
      case stLONG:
        for (j=0;  j < iColMax-iColMin+1; ++j)
          iOutBuf[j] = iRaw;
        DrawPolygonFromArrays(iRowMin, iRowMax, iRaw);
        break;
      case stREAL:
        for (j=0;  j < iColMax-iColMin+1; ++j)
          rOutBuf[j] = rVal;
        DrawPolygonFromArrays(iRowMin, iRowMax, rVal);
        break;
    }
  }
}

void MapApply3D::SetLineArrays(const RowCol& rcFrom, const RowCol& rcTo)
{
  long i;
  long iFrom, iTo;
  if (rcTo.Row == rcFrom.Row) { // horizontal
    iFrom = min(rcFrom.Col, rcTo.Col);
    iTo   = max(rcFrom.Col, rcTo.Col);
      Add(rcFrom.Row, iFrom);
      Add(rcFrom.Row, iTo);
  }
  else {
    if (rcTo.Col == rcFrom.Col) { // vertical
      iFrom = min(rcFrom.Row, rcTo.Row);
      iTo   = max(rcFrom.Row, rcTo.Row);
      for (i=iFrom; i<=iTo; i++)
        Add(i, rcTo.Col);
    }
    else {
      int iDx = (rcTo.Col - rcFrom.Col);
      int iDy = (rcTo.Row - rcFrom.Row);
      int iDirX = iDx<0 ? -1: 1;
      int iDirY = iDy<0 ? -1: 1;
      int iX, iY;
      iDx = abs(iDx);
      iDy = abs(iDy);
      if (iDx >= iDy) { // horizontal line parts
        int iInc1 = 2 * iDy, iInc2 = 2 * (iDy - iDx);
        int iDelta=iInc1 - iDx;
        int iNum=1;
        iX = rcFrom.Col; iY = rcFrom.Row;
        do {
          if (iDelta < 0) iDelta += iInc1;
          else {
            iDelta += iInc2;
            iFrom=(iDirX==1)?iX-iNum+1:iX;
            iTo  =(iDirX==1)?iX:iX+iNum-1;
            Add(iY, iFrom);
            Add(iY, iTo);
            iY += iDirY;
            iNum = 0;
          }
          iX += iDirX;
          iNum++;
        } while (iX != rcTo.Col);
        iFrom=(iDirX==1)?iX-iNum+1:iX;
        iTo  =(iDirX==1)?iX:iX+iNum-1;
        Add(iY, iFrom);
        Add(iY, iTo);
      }
      else { // vertical line parts
        int iInc1 = 2 * iDx, iInc2 = 2 * (iDx - iDy);
        int iDelta=iInc1 - iDy;
        int iNum=1;
        iX = rcFrom.Col; iY = rcFrom.Row;
        do {
          if (iDelta < 0) iDelta += iInc1;
          else {
            iDelta += iInc2;
            iFrom=(iDirY==1)?iY-iNum+1:iY;
            iTo  =(iDirY==1)?iY:iY+iNum-1;
            for (i=iFrom; i<=iTo; i++)
              Add(i, iX);
            iX += iDirX;
            iNum = 0;
          }
          iY += iDirY;
          iNum++;
        } while (iY != rcTo.Row);
        iFrom=(iDirY==1)?iY-iNum+1:iY;
        iTo  =(iDirY==1)?iY:iY+iNum-1;
        for (i=iFrom; i<=iTo; i++)
          Add(i, iX);
      }
    }
  }
}

void MapApply3D::Add(long iRow, long iCol)
{
  if ((iRow < 0) || (iRow > (long)aiCol1.iSize()))
    return;
  if (iCol < 0)
    iCol = 0;
  if (iCol >= iCols())
    iCol = iCols()-1;
  if (aiCol1[iRow] == iUNDEF) {
    aiCol1[iRow] = iCol;
    aiCol2[iRow] = iCol;
    return;
  }
  aiCol2[iRow] = max(aiCol2[iRow], iCol);
  aiCol1[iRow] = min(aiCol1[iRow], iCol);
}

void MapApply3D::DrawPolygonFromArrays(long iRowMin, long iRowMax, byte bRaw)
{
  for (long i=max(0,iRowMin); i <= min(iLines()-1, iRowMax); ++i) {
    if (aiCol2[i] != iUNDEF)
      pms->PutLineRaw(i, bOutBuf, aiCol1[i], aiCol2[i]-aiCol1[i]+1);
    else
      pms->PutRaw(RowCol(i,aiCol1[i]), bRaw);
    aiCol1[i] = aiCol2[i] = iUNDEF;
  }
}

void MapApply3D::DrawPolygonFromArrays(long iRowMin, long iRowMax, short shRaw)
{
  for (long i=max(0,iRowMin); i <= min(iLines()-1, iRowMax); ++i) {
    if (aiCol2[i] != iUNDEF)
      pms->PutLineRaw(i, shOutBuf, aiCol1[i], aiCol2[i]-aiCol1[i]+1);
    else
      pms->PutRaw(RowCol(i,aiCol1[i]), shRaw);
    aiCol1[i] = aiCol2[i] = iUNDEF;
  }
}

void MapApply3D::DrawPolygonFromArrays(long iRowMin, long iRowMax, long iRaw)
{
  for (long i=max(0,iRowMin); i <= min(iLines()-1, iRowMax); ++i) {
    if (aiCol2[i] != iUNDEF)
      pms->PutLineRaw(i, iOutBuf, aiCol1[i], aiCol2[i]-aiCol1[i]+1);
    else
      pms->PutRaw(RowCol(i,aiCol1[i]), iRaw);
    aiCol1[i] = aiCol2[i] = iUNDEF;
  }
}

void MapApply3D::DrawPolygonFromArrays(long iRowMin, long iRowMax, double rVal)
{
  for (long i=max(0,iRowMin); i <= min(iLines()-1, iRowMax); ++i) {
    if (aiCol2[i] != iUNDEF)
      pms->PutLineVal(i, rOutBuf, aiCol1[i], aiCol2[i]-aiCol1[i]+1);
    else
      pms->PutVal(RowCol(i,aiCol1[i]), rVal);
    aiCol1[i] = aiCol2[i] = iUNDEF;
  }
}




