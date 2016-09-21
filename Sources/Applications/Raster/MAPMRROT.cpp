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
/* MapMirrorRotate
   Copyright Ilwis System Development ITC
   july 1995, by Wim Koolhoven
	Last change:  JEL  14 Jul 97    1:05 pm
*/
#include "Engine\Base\DataObjects\valrange.h"
#include "Engine\Base\DataObjects\WPSMetaData.h"
#include "Applications\Raster\MAPMRROT.H"
#include "Engine\Map\Raster\Map.h"
#include "Engine\SpatialReference\Gr.h"
#include "Engine\SpatialReference\Grsmpl.h"
#include "Engine\SpatialReference\Grcornrs.h"
#include "Headers\Htp\Ilwisapp.htp"
#include "Headers\Err\Ilwisapp.err"
#include "Headers\Hs\map.hs"

IlwisObjectPtr * createMapMirrorRotate(const FileName& fn, IlwisObjectPtr& ptr, const String& sExpr, vector<void *> parms ) {
	if ( sExpr != "")
		return (IlwisObjectPtr *)MapMirrorRotate::create(fn, (MapPtr &)ptr, sExpr);
	else
		return (IlwisObjectPtr *)new MapMirrorRotate(fn, (MapPtr &)ptr);
}

String wpsmetadataMapMirrorRotate() {
	WPSMetaData metadata("MapMirrorRotate");
	return metadata.toString();
}

ApplicationMetadata metadataMapMirrorRotate(ApplicationQueryData *query) {
	ApplicationMetadata md;
	if ( query->queryType == "WPSMETADATA" || query->queryType == "") {
		md.wpsxml = wpsmetadataMapMirrorRotate();
	}
	if ( query->queryType == "OUTPUTTYPE" || query->queryType == "")
		md.returnType = IlwisObject::iotRASMAP;
	if ( query->queryType == "EXPERSSION" || query->queryType == "")
		md.skeletonExpression =  MapMirrorRotate::sSyntax();

	return md;
}

const char * MapMirrorRotate::sSyntax()
 { return "MapMirrorRotate(map,rotatetype)"; }

static const char * sMethods[] = { "Normal", "MirrHor", "MirrVert", "Transpose",
                                   "MirrDiag", "Rotate90", "Rotate180", "Rotate270", 0 };

static int iFind(const String& s, const char* sArr[])
{
  int i = 0; 
  while (sArr[i]) {
    if (_strnicmp(sArr[i], s.c_str(), s.length()) == 0)
      return i;
    i++;
  }
  return shUNDEF;
}

class DATEXPORT ErrorRotateMethod: public ErrorObject
{
public:
  ErrorRotateMethod(const String& sMethod, const WhereError& where)
  : ErrorObject(WhatError(String(TR("Invalid rotate method: '%S'").c_str(), sMethod), errMapMirrorRotate), where) {}
};

void RotateMethodError(const String& sMethod, const FileName& fn)
{
  throw ErrorRotateMethod(sMethod, fn);
}

MapMirrorRotate* MapMirrorRotate::create(const FileName& fn, MapPtr& p, const String& sExpr)
{
  Array<String> as(2);
  if (!IlwisObjectPtr::fParseParm(sExpr, as))
    ExpressionError(sExpr, sSyntax());
  Map mp(as[0], fn.sPath());
  int iMethod = iFind(as[1], sMethods);
  if (iMethod == shUNDEF)
    RotateMethodError(as[1], fn);  
  return new MapMirrorRotate(fn, p, mp, iMethod);
}

MapMirrorRotate::MapMirrorRotate(const FileName& fn, MapPtr& p)
: MapFromMap(fn, p)
{
  String sMirrRot;
  ReadElement("MapMirrorRotate", "Type", sMirrRot);
  String sMeth;
  ReadElement("MapMirrorRotate", "Method", sMeth);
  iMethod = iFind(sMeth, sMethods);
  if (iMethod == shUNDEF)
    RotateMethodError(sMeth, fnObj);
  ReadGeoRefFromFileAndSet();
  Init(iMethod);
}

MapMirrorRotate::MapMirrorRotate(const FileName& fn, MapPtr& p, const Map& mp, int iMeth)
: MapFromMap(fn, p, mp), iMethod(iMeth)
{
	Init(iMethod);
	bool fGRNone = fCIStrEqual(mp->gr()->fnObj.sFile , "none");
	if (!fGRNone)
	{
		GeoRef grNew;
		grNew.SetPointer(new GeoRefMirrorRotate(FileName(fn, ".grf"), mp->gr(), (MirrorRotateType)iMethod));
		ptr.SetGeoRef(grNew);
		ptr.SetSize(grNew->rcSize());
		ptr.gr()->SetDescription(String(TR("Created from %S").c_str(), sExpression()));
		ptr.gr()->Store();
	}
	else
		if (fRotate)
			ptr.SetSize(RowCol(iCols(), iLines()));

	if (dvrs().fRealValues())
	{
		ptr.SetMinMax(mp->rrMinMax());
		ptr.SetPerc1(mp->rrPerc1());
	}
	else
	{
		ptr.SetMinMax(mp->riMinMax());
		ptr.SetPerc1(mp->riPerc1());
	}

	objdep.Add(ptr.gr());
	// TODO: copy histogram to be implemented
	if (!fnObj.fValid())
		objtime = objdep.tmNewest();
}

void MapMirrorRotate::Store()
{
  MapFromMap::Store();
  WriteElement("MapFromMap", "Type", "MapMirrorRotate");
  WriteElement("MapMirrorRotate", "Method", sMethods[iMethod]);
}

MapMirrorRotate::~MapMirrorRotate()
{
}

void MapMirrorRotate::SetType(int iMethod)
{ 
  fSwapCols = iMethod==mrVERT  || iMethod==mrDIAG || iMethod==mrR270 || iMethod==mrR180 ;
  fSwapRows = iMethod==mrHOR   || iMethod==mrDIAG || iMethod==mrR180 || iMethod==mrR90 ;
  fRotate = iMethod==mrTRANS || iMethod==mrDIAG || iMethod==mrR270 || iMethod==mrR90 ;
  fNeedFreeze = fRotate;
}

void MapMirrorRotate::Init(int iMethod)
{
  sFreezeTitle = "MapMirrorRotate";
  htpFreeze = "ilwisapp\\mirror_rotate_algorithm.htm";
  SetType(iMethod);
  iInpLines = mp->iLines();
  iInpCols = mp->iCols();
}

long MapMirrorRotate::iComputePixelRaw(RowCol rc) const
{
  if (fSwapRows)
    rc.Row = iInpLines - rc.Row - 1;
  if (fSwapCols)
    rc.Col = iInpCols - rc.Col - 1;
  return mp->iRaw(rc);
}

long MapMirrorRotate::iComputePixelVal(RowCol rc) const
{
  if (fSwapRows)
    rc.Row = iInpLines - rc.Row - 1;
  if (fSwapCols)
    rc.Col = iInpCols - rc.Col - 1;
  return mp->iValue(rc);
}

double MapMirrorRotate::rComputePixelVal(RowCol rc) const
{
  if (fSwapRows)
    rc.Row = iInpLines - rc.Row - 1;
  if (fSwapCols)
    rc.Col = iInpCols - rc.Col - 1;
  return mp->rValue(rc);
}

void MapMirrorRotate::ComputeLineRaw(long iLine, ByteBuf& buf, long iFrom, long iNum) const
{
  if (fSwapRows)
    iLine = iInpLines - iLine - 1;
  mp->GetLineRaw(iLine, buf, iFrom, iNum);
  if (fSwapCols) { // swap pixels in line
    byte bRaw;
    for (int i=0; i < iNum / 2; i++) {
      bRaw = buf[i];
      buf[i] = buf[iNum-i-1];
      buf[iNum-i-1] = bRaw;
    }
  }
}

void MapMirrorRotate::ComputeLineRaw(long iLine, IntBuf& buf, long iFrom, long iNum) const
{
  if (fSwapRows)
    iLine = iInpLines - iLine - 1;
  mp->GetLineRaw(iLine, buf, iFrom, iNum);
  if (fSwapCols) { // swap pixels in line
    int iRaw;
    for (int i=0; i < iNum / 2; i++) {
      iRaw = buf[i];
      buf[i] = buf[iNum-i-1];
      buf[iNum-i-1] = iRaw;
    }
  }
}

void MapMirrorRotate::ComputeLineRaw(long iLine, LongBuf& buf, long iFrom, long iNum) const
{
  if (fSwapRows)
    iLine = iInpLines - iLine - 1;
  mp->GetLineRaw(iLine, buf, iFrom, iNum);
  if (fSwapCols) { // swap pixels in line
    long iRaw;
    for (int i=0; i < iNum / 2; i++) {
      iRaw = buf[i];
      buf[i] = buf[iNum-i-1];
      buf[iNum-i-1] = iRaw;
    }
  }
}

void MapMirrorRotate::ComputeLineVal(long iLine, LongBuf& buf, long iFrom, long iNum) const
{
  if (fSwapRows)
    iLine = iInpLines - iLine - 1;
  mp->GetLineVal(iLine, buf, iFrom, iNum);
  if (fSwapCols) { // swap pixels in line
    long iVal;
    for (int i=0; i < iNum / 2; i++) {
      iVal = buf[i];
      buf[i] = buf[iNum-i-1];
      buf[iNum-i-1] = iVal;
    }
  }
}

void MapMirrorRotate::ComputeLineVal(long iLine, RealBuf& buf, long iFrom, long iNum) const
{
  if (fSwapRows)
    iLine = iInpLines - iLine - 1;
  mp->GetLineVal(iLine, buf, iFrom, iNum);
  if (fSwapCols) { // swap pixels in line
    double rVal;
    for (int i=0; i < iNum / 2; i++) {
      rVal = buf[i];
      buf[i] = buf[iNum-i-1];
      buf[iNum-i-1] = rVal;
    }
  }
}

ByteBuf undef(const ByteBuf&)
{
  return ByteBuf();
}

IntBuf undef(const IntBuf&)
{
  return IntBuf();
}

LongBuf undef(const LongBuf&)
{
  return LongBuf();
}

RealBuf undef(const RealBuf&)
{
  return RealBuf();
}

bool MapMirrorRotate::fFreezing()
{
	bool ok;
	if (!fRotate)
		ok =  MapVirtual::fFreezing();
	else
		ok = fCompute(mp.ptr(), &ptr, MirrorRotateType(iMethod), &trq);

	unsigned int rot90s = mrR90 | mrR270 | mrTRANS;
	if ( iMethod != mrR90 && iMethod != mrR270 && iMethod != mrTRANS) {
		ptr.SetGeoRef(mp->gr());
	} else {
		if ( mp->gr()->pgc()) {
			GeoRefCorners *smp = mp->gr()->pgc();
			RowCol rc(mp->rcSize().Col, mp->rcSize().Row);
			GeoRef grf(smp->cs(), rc, smp->a11, smp->a12, smp->a21, smp->a22, smp->b1, smp->b2);
			ptr.SetGeoRef(grf);
		}
	}
	return ok;

}

String MapMirrorRotate::sExpression() const
{
  return String("MapMirrorRotate(%S,%s)", mp->sNameQuoted(true, fnObj.sPath()), sMethods[iMethod]);
}

bool MapMirrorRotate::fCompute(MapPtr* ptr, MapPtr* ptrRot, MirrorRotateType mrt, Tranquilizer* trq)
{
  assert(mrt==mrTRANS || mrt==mrDIAG || mrt==mrR270 || mrt==mrR90);
  bool fSwapCols = mrt==mrVERT  || mrt==mrDIAG || mrt==mrR270 || mrt==mrR180 ;
  bool fSwapRows = mrt==mrHOR   || mrt==mrDIAG || mrt==mrR180 || mrt==mrR90 ;
  long iCols = ptr->iCols();
  long iLines = ptr->iLines();

  trq->SetText(TR("Rotating"));
  switch (ptr->st()) {
    case stREAL:
	case stFLOAT: {
      int iNrBufCol = (32767>>2) / iLines > iCols ? iCols : 32767/iLines;
      RealBuf bufIn(iNrBufCol);
      Array<RealBuf> aBufOut(iNrBufCol);
      for (int kk = 0; kk < iNrBufCol; kk++)
        aBufOut[kk].Size(iLines);
      int iNrPass = (iCols - 1) / iNrBufCol + 1;
      int iNrColOver = iCols - (iNrPass - 1) * iNrBufCol;
      for (int i=0; i < iNrPass; i++) {
        int iOpt = i==iNrPass-1 ? iNrColOver : iNrBufCol;
        int iFrom = !fSwapCols ? i * iNrBufCol : (i==iNrPass-1 ? 0 : iCols - (i+1)*iNrBufCol);
//        int iFrom = !fSwapCols ? i * iNrBufCol : (i == iNrPass-1 ? 0 : iCols - i*iNrBufCol);
        for (int j=0; j<iLines; j++) {
          int iL = fSwapRows ? iLines - j - 1 : j;
          ptr->GetLineVal(iL, bufIn, iFrom, iOpt);
          if (fSwapCols)
            for (int k = 0; k < iOpt; k++)
              aBufOut[k][j] = bufIn[iOpt-k-1];
          else
            for (int k=0; k < iOpt; k++)
              aBufOut[k][j] = bufIn[k];
        }
        for (int j=0; j<iOpt; j++)
          ptrRot->PutLineVal(i * iNrBufCol + j, aBufOut[j]);
        if (trq->fUpdate(i * iNrBufCol + iOpt, iCols))
          return false;
      }
    }
    break;
    case stLONG: {
      int iNrBufCol = (32767>>1) / iLines > iCols ? iCols : 32767/iLines;
      LongBuf bufIn(iNrBufCol);
      Array<LongBuf> aBufOut(iNrBufCol);
      for (int kk = 0; kk < iNrBufCol; kk++)
        aBufOut[kk].Size(iLines);
      int iNrPass = (iCols - 1) / iNrBufCol + 1;
      int iNrColOver = iCols - (iNrPass - 1) * iNrBufCol;
      for (int i=0; i < iNrPass; i++) {
        int iOpt = i==iNrPass-1 ? iNrColOver : iNrBufCol;
        int iFrom = !fSwapCols ? i * iNrBufCol : (i==iNrPass-1 ? 0 : iCols - (i+1)*iNrBufCol);
//        int iFrom = !fSwapCols ? i * iNrBufCol : (i==iNrPass-1 ? 0 : iCols - i*iNrBufCol);
        for (int j=0; j<iLines; j++) {
          int iL = fSwapRows ? iLines - j - 1 : j;
          ptr->GetLineRaw(iL, bufIn, iFrom, iOpt);
          if (fSwapCols)
            for (int k = 0; k < iOpt; k++)
              aBufOut[k][j] = bufIn[iOpt-k-1];
          else
            for (int k=0; k < iOpt; k++)
              aBufOut[k][j] = bufIn[k];
        }
        for (int j=0; j<iOpt; j++)
          ptrRot->PutLineRaw(i * iNrBufCol + j, aBufOut[j]);
        if (trq->fUpdate(i * iNrBufCol + iOpt, iCols))
          return false;
      }
    }
    break;
    case stINT: {
      int iNrBufCol = 32767 / iLines > iCols ? iCols : 32767/iLines;
      IntBuf bufIn(iNrBufCol);
      Array<IntBuf> aBufOut(iNrBufCol);
      for (int kk = 0; kk < iNrBufCol; kk++)
        aBufOut[kk].Size(iLines);
      int iNrPass = (iCols - 1) / iNrBufCol + 1;
      int iNrColOver = iCols - (iNrPass - 1) * iNrBufCol;
      for (int i=0; i < iNrPass; i++) {
        int iOpt = i==iNrPass-1 ? iNrColOver : iNrBufCol;
        int iFrom = !fSwapCols ? i * iNrBufCol : (i==iNrPass-1 ? 0 : iCols - (i+1)*iNrBufCol);
//        int iFrom = !fSwapCols ? i * iNrBufCol : (i==iNrPass-1 ? 0 : iCols - i*iNrBufCol);
        for (int j=0; j<iLines; j++) {
          int iL = fSwapRows ? iLines - j - 1 : j;
          ptr->GetLineRaw(iL, bufIn, iFrom, iOpt);
          if (fSwapCols)
            for (int k = 0; k < iOpt; k++)
              aBufOut[k][j] = bufIn[iOpt-k-1];
          else
            for (int k=0; k < iOpt; k++)
              aBufOut[k][j] = bufIn[k];
        }
        for (int j=0; j<iOpt; j++)
          ptrRot->PutLineRaw(i * iNrBufCol + j, aBufOut[j]);
        if (trq->fUpdate(i * iNrBufCol + iOpt, iCols))
          return false;
      }
    }
    break;
    case stBYTE:
    case stNIBBLE:
    case stDUET:
    case stBIT: {
      int iNrBufCol = 32767 / iLines > iCols ? iCols : 32767/iLines;
      ByteBuf bufIn(iNrBufCol);
      Array<ByteBuf> aBufOut(iNrBufCol);
      for (int kk = 0; kk < iNrBufCol; kk++)
        aBufOut[kk].Size(iLines);
      int iNrPass = (iCols - 1) / iNrBufCol + 1;
      int iNrColOver = iCols - (iNrPass - 1) * iNrBufCol;
      for (int i=0; i < iNrPass; i++) {
        int iOpt = i==iNrPass-1 ? iNrColOver : iNrBufCol;
        int iFrom = !fSwapCols ? i * iNrBufCol : (i==iNrPass-1 ? 0 : iCols - (i+1)*iNrBufCol);
        for (int j=0; j<iLines; j++) {
          int iL = fSwapRows ? iLines - j - 1 : j;
          ptr->GetLineRaw(iL, bufIn, iFrom, iOpt);
          if (fSwapCols)
            for (int k = 0; k < iOpt; k++)
              aBufOut[k][j] = bufIn[iOpt-k-1];
          else
            for (int k=0; k < iOpt; k++)
              aBufOut[k][j] = bufIn[k];
        }
        for (int k=0; k<iOpt; k++)
          ptrRot->PutLineRaw(i * iNrBufCol + k, aBufOut[k]);
        if (trq->fUpdate(i * iNrBufCol + iOpt, iCols))
          return false;
      }
    }
  }

  return true;
}




