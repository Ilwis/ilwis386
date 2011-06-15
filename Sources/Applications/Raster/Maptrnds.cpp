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

#include "Engine\Base\DataObjects\valrange.h"
#include "Engine\Base\DataObjects\WPSMetaData.h"
#include "Applications\Raster\MAPTRNDS.H"
#include "Engine\Table\tblstore.h"
#include "Engine\Table\COLSTORE.H"
#include "Engine\Base\Algorithm\Realmat.h"
#include "Engine\Base\Algorithm\Lstsqrs.h"
#include "Headers\Err\Ilwisapp.err"
#include "Headers\Htp\Ilwisapp.htp"
#include "Headers\Hs\map.hs"

IlwisObjectPtr * createMapTrendSurface(const FileName& fn, IlwisObjectPtr& ptr, const String& sExpr, vector<void *> parms ) {
	if ( sExpr != "")
		return (IlwisObjectPtr *)MapTrendSurface::create(fn, (MapPtr &)ptr, sExpr);
	else
		return (IlwisObjectPtr *)new MapTrendSurface(fn, (MapPtr &)ptr);
}

static const char * sSurfaceType[] = { "Plane", "Linear2", "Parabolic2", "2", "3", "4", "5", "6", 0 };

String wpsmetadataMapTrendSurface() {
	WPSMetaData metadata("MapMovingSurface");
	return metadata.toString();
}

ApplicationMetadata metadataMapTrendSurface(ApplicationQueryData *query) {
	ApplicationMetadata md;
	if ( query->queryType == "WPSMETADATA" || query->queryType == "") {
		md.wpsxml = wpsmetadataMapTrendSurface();
	}
	if ( query->queryType == "OUTPUTTYPE" || query->queryType == "")
		md.returnType = IlwisObject::iotRASMAP;
	if ( query->queryType == "EXPERSSION" || query->queryType == "")
		md.skeletonExpression = MapTrendSurface::sSyntax();

	return md;
}

const char* MapTrendSurface::sSyntax() {
  return "MapTrendSurface(pntmap,georef,surfacetype)";
}

static int iFind(const String& s, const char* sArr[])
{
  int i = 0;
  while (sArr[i]) {
    if (_strnicmp(sArr[i], s.scVal(), s.length()) == 0)
      return i;
    i++;
  }
  return shUNDEF;
}


class DATEXPORT ErrorSurface: public ErrorObject
{
public:
  ErrorSurface(const String& sSurfaceType, const WhereError& where)
  : ErrorObject(WhatError(String(SMAPErrInvalidSurfaceType_S.scVal(), sSurfaceType), errMapTrendSurface), where) {}
};

MapTrendSurface* MapTrendSurface::create(const FileName& fn, MapPtr& p, const String& sExpr)
{
  Array<String> as(3);
  if (!IlwisObjectPtr::fParseParm(sExpr, as))
    ExpressionError(sExpr, sSyntax());
  PointMap pmp(as[0], fn.sPath());
  GeoRef gr(as[1], fn.sPath());
  SurfaceType sft = (SurfaceType)iFind(as[2], sSurfaceType);
  if (sft == shUNDEF)
    throw ErrorSurface(as[2], fn);
  return new MapTrendSurface(fn, p, pmp, gr, sft);
}

MapTrendSurface::MapTrendSurface(const FileName& fn, MapPtr& p)
: MapFromPointMap(fn, p)
{
  String sSurf;
  ReadElement("MapTrendSurface", "SurfaceType", sSurf);
  sft = (SurfaceType)iFind(sSurf, sSurfaceType);
  if (sft == shUNDEF) {
    ErrorSurface(sSurf, sTypeName()).Show();
    sft = sfPLANE;
  }
  objdep.Add(gr().ptr());
  Init();
}

MapTrendSurface::MapTrendSurface(const FileName& fn, MapPtr& p, const PointMap& pmp, const GeoRef& gr,
                                   SurfaceType sftp)
: MapFromPointMap(fn, p, pmp, gr), sft(sftp)
{
  if (!dvrs().fValues())
    ValueDomainError(dm()->sName(true, fnObj.sPath()), sTypeName(), errMapTrendSurface+1);
  if (gr->fGeoRefNone())
    throw ErrorGeoRefNone(gr->fnObj, errMapTrendSurface+2);
  if (!cs()->fConvertFrom(pmp->cs()))
    IncompatibleCoordSystemsError(cs()->sName(true, fnObj.sPath()), pmp->cs()->sName(true, fnObj.sPath()), sTypeName(), errMapTrendSurface+4);
  objdep.Add(gr.ptr());
  if (!fnObj.fValid())
    objtime = objdep.tmNewest();
  Init();
}

MapTrendSurface::~MapTrendSurface()
{
}

void MapTrendSurface::Store()
{
  MapFromPointMap::Store();
  WriteElement("MapFromPointMap", "Type", "MapTrendSurface");
  WriteElement("MapTrendSurface", "SurfaceType", sSurfaceType[sft]);
}

void MapTrendSurface::Init()
{
  fNeedFreeze = true;
  sFreezeTitle = "MapTrendSurface";
  htpFreeze = "ilwisapp\\trend_surface_algorithm.htm";
}

static String sFormula(const CVector& cv)
{
  String s = String("Equation of Trend Surface:\r\n");
  s &= String("Z = ");
  int iRows = cv.iRows();
  switch (iRows) {
    case 3:
      s &= String("%.9g + %.9g*X + %.9g*Y", cv(0), cv(1), cv(2));
      return s;
    case 4:
      s &= String("%.9g + %.9g*X + %.9g*Y + %.9g*X*Y", cv(0), cv(1), cv(2), cv(3));
      return s;
    case 5:
      s &= String("%.9g + %.9g*X+%.9g*Y + %.9g*X^2 + %.9g*Y^2", cv(0), cv(1), cv(2), cv(3), cv(4));
      return s;
    default:
      s &= String("%.9g + %.9g*X + %.9g*Y +  %.9g*X^2 + %.9g*X*Y + %.9g*Y^2", cv(0), cv(1), cv(2), cv(3), cv(4), cv(5));
      if (6 == iRows)
        return s;
      s &= String("+ %.9g*X^3 + %.9g*X^2*Y + %.9g*X*Y^2 + %.9g*Y^3", cv(6), cv(7), cv(8), cv(9));
      if (10 == iRows)
        return s;
      s &= String("+ %.9g*X^4 + %.9g*X^3*Y + %.9g*X^2*Y^2 +  %.9g*X*Y^3 + %.9g*Y^4", cv(10), cv(11), cv(12), cv(13), cv(14));
      if (15 == iRows)
        return s;
      s &= String("+ %.9g*X^5 + %.9g*X^4*Y + %.9g*X^3*Y^2 + %.9g*X^2*Y^3 + %.9g*X*Y^4 + %.9g*Y^5", cv(15), cv(16), cv(17), cv(18), cv(19));
      if (20 == iRows)
        return s;
      s &= String("+ %.9g*X^6 + %.9g*X^5*Y + %.9g*X^4*Y^2 + %.9g*X^3*Y^3 + %.9g*X^2*Y^4 + %.9g*X*Y^5 + %.9g*Y^6", cv(20), cv(21), cv(22), cv(23), cv(24));
      return s;
  }
}

bool MapTrendSurface::fFreezing()
{
enum esft{Plane, Lin2, Parab2, o2, o3, o4, o5, o6 };

  long iNrPoints = pmp ->iFeatures();
//  bool fUseReals = pmp->dvrs().fUseReals();
  // determine size of map in row's and col's
  RowCol rcMrc = gr()->rcSize();
  long iMRow = rcMrc.Row;
  long iMCol = rcMrc.Col;
  // declaration to declare the names for the compiler
  //    array's are resized as needed
  RealBuf rBufOut(iMCol);
//  LongBuf iBufOut(1);
  // create output buffer according to store type
//  if (fUseReals)
//    rBufOut.Size(iMCol);
//  else
//    iBufOut.Size(iMCol);
  bool fTransformCoords = cs() != pmp->cs();
  
  trq.SetText(SMAPTextCalculating);
  trq.SetTitle(SMAPTextMapTrendSurface);
  int iDim=0;    // dimension of matrices
  // determine dimension of matrices depending of surface type
  switch (sft) {
    case Plane:  iDim =  3; break;
    case Lin2:   iDim =  4; break;
    case Parab2: iDim =  5; break;
    case o2:     iDim =  6; break;
    case o3:     iDim = 10; break;
    case o4:     iDim = 15; break;
    case o5:     iDim = 21; break;
    case o6:     iDim = 28; break;
  };
    
  CVector rT(iDim);
  CVector rR(iDim);
	Coord cLocalCentroid = Coord(0,0);
  long iVPc = 0;   // valid point counter
  // at first count number of valid points
  for (long iPc = 1; iPc <= iNrPoints; iPc++)  {
    Coord cri=pmp->cValue(iPc);
    if (fTransformCoords)
      cri = cs()->cConv(pmp->cs(), cri);
    double rAttr=pmp->rValue(iPc);
    if((cri.x==rUNDEF)||(cri.y==rUNDEF)||(rAttr==rUNDEF)) continue;
    iVPc++;
		cLocalCentroid.x += cri.x;
		cLocalCentroid.y += cri.y;
  }
	if (iVPc > 0) {
		cLocalCentroid.x /= iVPc;
		cLocalCentroid.y /= iVPc;
	}

  if (iDim > iVPc) { 
    String s(SMAPErrNotEnoughPoints_ii.scVal(), iVPc, iDim);
    throw ErrorObject(WhatError(s, errMapTrendSurface+3), fnObj);
  }  

  RealMatrix rM(iVPc,iDim);
  CVector rA(iVPc);


  iVPc = -1;
  for ( long iPc = 1; iPc <= iNrPoints; iPc++)  {
    Coord cri=pmp->cValue(iPc);
    if (fTransformCoords)
      cri = cs()->cConv(pmp->cs(), cri);
    double rAttr=pmp->rValue(iPc);
    if((cri.x==rUNDEF)||(cri.y==rUNDEF)||(rAttr==rUNDEF)) continue;

		cri.x -= cLocalCentroid.x; // reduce (cConv-transformed) crds to local system for
		cri.y -= cLocalCentroid.y; // better numerical accuracy of trend surf coefficients

    iVPc++;

    rA(iVPc) = rAttr;

    rM(iVPc,0) = 1.0;                   // constant factor
    rM(iVPc,1) = cri.x;                 // X
    rM(iVPc,2) = cri.y;                 // Y
    if(sft == Plane)
      continue;
    if(sft == Lin2)  {
      rM(iVPc,3) = cri.x * cri.y;       // X * Y
      continue;
    }
    if(sft == Parab2)  {
      rM(iVPc,3) = cri.x * cri.x;       // X^2
      rM(iVPc,4) = cri.y * cri.y;       // Y^2
      continue;
    }
    rM(iVPc,3) = cri.x * cri.x;         // X^2
    rM(iVPc,4) = cri.x * cri.y;         // X * Y
    rM(iVPc,5) = cri.y * cri.y;         // Y^2
    if(sft == o2)
      continue;
    rM(iVPc,6) = rM(iVPc,3) * cri.x;    // X^3
    rM(iVPc,7) = rM(iVPc,4) * cri.x;    // X^2 * Y
    rM(iVPc,8) = rM(iVPc,4) * cri.y;    // X   * Y^2
    rM(iVPc,9) = rM(iVPc,5) * cri.y;    // X^3
    if(sft == o3)
      continue;
    rM(iVPc,10) = rM(iVPc,6) * cri.x;    // X^4
    rM(iVPc,11) = rM(iVPc,7) * cri.x;    // X^3 * Y
    rM(iVPc,12) = rM(iVPc,8) * cri.x;    // X^2 * Y^2
    rM(iVPc,13) = rM(iVPc,8) * cri.y;    // X   * Y^3
    rM(iVPc,14) = rM(iVPc,9) * cri.y;    // Y^4
    if(sft == o4)
      continue;
    rM(iVPc,15) = rM(iVPc,10) * cri.x;    // X^5
    rM(iVPc,16) = rM(iVPc,11) * cri.x;    // X^4 * Y
    rM(iVPc,17) = rM(iVPc,12) * cri.x;    // X^3 * Y^2
    rM(iVPc,18) = rM(iVPc,12) * cri.y;    // X^2 * Y^3
    rM(iVPc,19) = rM(iVPc,13) * cri.y;    // X^1 * Y^4
    rM(iVPc,20) = rM(iVPc,14) * cri.y;    // Y^5
    if(sft == o5)
      continue;
    rM(iVPc,21) = rM(iVPc,15) * cri.x;    // X^6
    rM(iVPc,22) = rM(iVPc,16) * cri.x;    // X^5 * Y
    rM(iVPc,23) = rM(iVPc,17) * cri.x;    // X^4 * Y^2
    rM(iVPc,24) = rM(iVPc,18) * cri.x;    // X^3 * Y^3
    rM(iVPc,25) = rM(iVPc,18) * cri.y;    // X^2 * Y^4
    rM(iVPc,26) = rM(iVPc,19) * cri.y;    // X^1 * Y^5
    rM(iVPc,27) = rM(iVPc,20) * cri.y;    // Y^6
  }
  
  rR = LeastSquares(rM, rA);
  ptr.SetAdditionalInfo(sFormula(rR));
  ptr.SetAdditionalInfoFlag(ptr.sAdditionalInfo().length() > 0);
  for (long iRc = 0; iRc < iMRow; iRc++)  {
    if (trq.fUpdate(iRc, iMRow))
      return false;

    // start new Row, get coordinate of first point
    for (long iCc = 0; iCc < iMCol; iCc++)  {
      Coord  coCo = gr()->cConv(RowCol(iRc, iCc));
			coCo.x -= cLocalCentroid.x; // reduce again (cConv-transformed) crds to local syst
			coCo.y -= cLocalCentroid.y; // for use with computed polynomial that refers to this syst

      rT(0) = 1.0;                        // constant
      rT(1) = coCo.x;                     // X
      rT(2) = coCo.y;                     // Y
      if(sft == Plane) goto label1;
      if(sft == Lin2)  {
        rT(3) = coCo.x * coCo.y;            // X * Y
        goto label1;
      }
      if(sft == Parab2)  {
        rT(3) = coCo.x * coCo.x;            // X^2
        rT(4) = coCo.y * coCo.y;            // Y^2
        goto label1;
      }

      rT(3) = coCo.x * coCo.x;            // X^2
      rT(4) = coCo.x * coCo.y;            // X * Y
      rT(5) = coCo.y * coCo.y;            // Y^2
      if(sft == o2)
        goto label1;
      rT(6) = rT(3) * coCo.x;             // X^3
      rT(7) = rT(4) * coCo.x;             // X^2 * Y
      rT(8) = rT(4) * coCo.y;             // X * Y^2
      rT(9) = rT(5) * coCo.y;             // Y^3
      if(sft == o3)
        goto label1;
      rT(10) = rT(6) * coCo.x;            // X^4
      rT(11) = rT(7) * coCo.x;            // X^3 * Y
      rT(12) = rT(8) * coCo.x;            // X^2 * Y^2
      rT(13) = rT(8) * coCo.y;            // X * Y^3
      rT(14) = rT(9) * coCo.y;            // Y^4
      if(sft == o4)
        goto label1;

      rT(15) = rT(10) * coCo.x;           // X^5
      rT(16) = rT(11) * coCo.x;           // X^4 * Y
      rT(17) = rT(12) * coCo.x;           // X^3 * Y^2
      rT(18) = rT(12) * coCo.y;           // X^2 * Y^3
      rT(19) = rT(13) * coCo.y;           // X * Y^4
      rT(20) = rT(14) * coCo.y;           // Y^5
      if(sft == o5)
        goto label1;

      rT(21) = rT(15) * coCo.x;           // X^6
      rT(22) = rT(16) * coCo.x;           // X^5 * Y
      rT(23) = rT(17) * coCo.x;           // X^4 * Y^2
      rT(24) = rT(18) * coCo.x;           // X^3 * Y^3
      rT(25) = rT(18) * coCo.y;           // X^2 * Y^4
      rT(26) = rT(19) * coCo.y;           // X * Y^5
      rT(27) = rT(20) * coCo.y;           // Y^6

 label1:;
      double rVal = 0.0;
      for (int i = 0; i < iDim; i++)
        rVal += rT(i ) * rR(i);
//      if (fUseReals)
        rBufOut[iCc] = rVal;
//      else
//        iBufOut[iCc] = roundx (rVal);

    }
    // write out row of map
//    if (fUseReals)
      pms->PutLineVal(iRc,rBufOut);
//    else
//      pms->PutLineVal(iRc,iBufOut);
  }
  return true;
}

String MapTrendSurface::sExpression() const
{
  return String("MapTrendSurface(%S,%S,%s)", pmp->sNameQuoted(true, fnObj.sPath()),
                 gr()->sNameQuoted(true, fnObj.sPath()), sSurfaceType[sft]);
}

bool MapTrendSurface::fDomainChangeable() const
{
  return true;
}

bool MapTrendSurface::fValueRangeChangeable() const
{
  return true;
}




