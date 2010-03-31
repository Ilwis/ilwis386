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
/* $Log: /ILWIS 3.0/RasterApplication/MAPNRPNT.cpp $
 * 
 * 8     4-03-03 10:31 Willem
 * - Changed: removed mismatch netween syntax in sSyntax() function as
 * comapred with code
 * 
 * 7     15-02-03 16:40 Hendrikse
 * replaced Planar, Spherical by Plane, Sphere
 * 
 * 6     11-12-02 12:17 Hendrikse
 * avoids now to use rawvalues in all 'input is value domain' cases,
 * making use of rBufOut. Otherwise 'small' domains (with few diff values)
 * or wronly coverted to raw values and back 
 * 
 * 5     7-10-02 11:37 Hendrikse
 * implemented use of m_distMeth = Distance::distsphere; etc from new
 * Class Distance in CoordSystems project
 * 
 * 4     18-09-02 18:45 Hendrikse
 * implemented the fSphericDistance option
 * 
 * 3     9/08/99 8:57a Wind
 * changed sName() to sNameQuoted() in sExpression() to support long file
 * names
 * 
 * 2     3/11/99 12:16p Martin
 * Added support for Case insesitive 
// Revision 1.6  1998/09/16 17:24:42  Wim
// 22beta2
//
// Revision 1.4  1997/09/11 17:03:36  Wim
// Init() function is now called in constructor
//
// Revision 1.3  1997-08-25 21:36:55+02  Wim
// iMinP is now initialized in SearchNP()
// Scan points not of stREAL with the raw value (useful for ID's)
// Skip points with undef coords
//
/* MapNearestPoint
   Copyright Ilwis System Development ITC
   december 1995, by Dick Visser
	Last change:  WK   11 Sep 97    6:58 pm
*/
#include "Applications\Raster\MAPNRPNT.H"
#include "Engine\Map\Raster\MAPSTORE.H"
#include "Engine\Table\tblstore.h"
#include "Engine\Table\COLSTORE.H"
#include "Engine\Base\DataObjects\valrange.h"
#include "Engine\Base\Algorithm\Qsort.h"
#include "Headers\Err\Ilwisapp.err"
#include "Headers\Htp\Ilwisapp.htp"
#include "Headers\Hs\map.hs"

IlwisObjectPtr * createMapNearestPoint(const FileName& fn, IlwisObjectPtr& ptr, const String& sExpr, vector<void *> parms ) {
	if ( sExpr != "")
		return (IlwisObjectPtr *)MapNearestPoint::create(fn, (MapPtr &)ptr, sExpr);
	else
		return (IlwisObjectPtr *)new MapNearestPoint(fn, (MapPtr &)ptr);
}

const char* MapNearestPoint::sSyntax() {
  return "MapNearestPoint(pntmap,georef[,plane|sphere])";
}

MapNearestPoint* MapNearestPoint::create(const FileName& fn, MapPtr& p, const String& sExpr)
{
  Array<String> as;
  short iParms = IlwisObjectPtr::iParseParm(sExpr, as);
  //if (!IlwisObjectPtr::fParseParm(sExpr, as))
	if (iParms > 3 || iParms < 1)
    ExpressionError(sExpr, sSyntax());
  PointMap pmp(as[0], fn.sPath());
  GeoRef gr(as[1], fn.sPath());
	bool fSphericDist = false; //default
	if ((iParms == 3) && fCIStrEqual("sphere", as[2]))
		fSphericDist = true;
  return new MapNearestPoint(fn, p, pmp, gr, fSphericDist);
}

MapNearestPoint::MapNearestPoint(const FileName& fn, MapPtr& p)
: MapFromPointMap(fn, p)
{
  Init();
}

MapNearestPoint::MapNearestPoint(const FileName& fn, MapPtr& p, const PointMap& pmp,
																 const GeoRef& gr, const bool fSpheric)
: MapFromPointMap(fn, p, pmp, gr)
{
  if (gr->fGeoRefNone())
    throw ErrorGeoRefNone(fnObj, errMapNearestPoint);
  if (!cs()->fConvertFrom(pmp->cs()))
    IncompatibleCoordSystemsError(cs()->sName(true, fnObj.sPath()), pmp->cs()->sName(true, fnObj.sPath()), sTypeName(), errMapNearestPoint+1);
  if (!fnObj.fValid())
    objtime = objdep.tmNewest();
	if (fSpheric)
		m_distMeth = Distance::distSPHERE;
	else
		m_distMeth = Distance::distPLANE;
  Init();
}

void MapNearestPoint::Store()
{
  MapFromPointMap::Store();
  WriteElement("MapFromPointMap", "Type", "MapNearestPoint");
}

MapNearestPoint::~MapNearestPoint()
{
}

void MapNearestPoint::Init()
{
  fNeedFreeze = true;
  sFreezeTitle = "MapNearestPoint";
  htpFreeze = htpMapNearestPointT;
}


static bool LessFunc(long i1, long i2, void* p)
{
  MapNearestPoint* mp = static_cast<MapNearestPoint*>(p);
  Coord cri1,cri2;
  cri1 = mp->pmp->cValue(mp->iSortList[i1]);
  cri2 = mp->pmp->cValue(mp->iSortList[i2]);
  return(cri1.y < cri2.y);
}

static void SwapFunc(long i1, long i2, void* p)
{
  MapNearestPoint* mp = static_cast<MapNearestPoint*>(p);
  long t = mp->iSortList[i1];
  mp->iSortList[i1] = mp->iSortList[i2];
  mp->iSortList[i2] = t;
}

long MapNearestPoint::SearchNP(long iPlp, long iRc, long iCc)
{
  double rMinSqDist = DBL_MAX; // least found distance squared !!!
  double rMinDif = DBL_MAX;    // least found distance
  long iMinP = 1;              // point with least found distance
  RowCol rcRc = RowCol(iRc, iCc);
  Coord  crdRC = gr()->cConv(rcRc);
	Distance dis = Distance(pmp->cs(), m_distMeth);
  // starting with iPlp look first forward and lateron backward in list
  // for a nearer point
  bool fTransformCoords = cs() != pmp->cs();
  for (long iPc = iPlp; iPc <= iNrPoints; iPc++)  {
    long iSLp = iSortList[iPc];
    Coord coPm = pmp->cValue (iSLp);
    if (coPm.fUndef())
      continue;
    if (fTransformCoords)
      coPm = cs()->cConv(pmp->cs(), coPm);
    if (coPm.fUndef())
      continue;
    // in the list we are moving towards greater Y
    double rdy =  fabs(coPm.y - crdRC.y);
    if (rdy > rMinDif) break;     // terminate if distance in y > rMinDif
    double rdx = fabs(crdRC.x - coPm.x);
    if (rdx > rMinDif) continue;  // go to check next point
    double rSqDist = rdx * rdx + rdy * rdy;
    if (rSqDist < rMinSqDist)  {
       rMinSqDist = rSqDist;
       rMinDif = sqrt(rMinSqDist);
       iMinP = iPc;
    }
  }
  // now look backward in the list
  for (long iPc = iPlp-1; iPc >= 1; iPc--)  {
    long iSLp = iSortList[iPc];
    Coord coPm = pmp->cValue (iSLp);
    // in the list we are moving towards smaller Y
    double rdy = fabs(crdRC.y - coPm.y);
    if (rdy > rMinDif) break;     // terminate loop if distance > rMinDif
    double rdx = fabs(crdRC.x - coPm.x);
    if (rdx > rMinDif) continue;  // go to check next point
    double rSqDist = rdx * rdx + rdy * rdy;
    if (rSqDist < rMinSqDist)  {
       rMinSqDist = rSqDist;
       rMinDif = sqrt(rMinSqDist);
       iMinP = iPc;
    }
  }
	if (m_distMeth == Distance::distSPHERE)
	{
		rMinSqDist = DBL_MAX;
		rMinDif = DBL_MAX; 
		Coord  crdRC = gr()->cConv(rcRc);
		for (long iPc = iPlp; iPc <= iNrPoints; iPc++)  {
	    long iSLp = iSortList[iPc];
	    Coord crdPm = pmp->cValue (iSLp);
	    if (crdPm.fUndef())
	      continue;
	    if (fTransformCoords)
	      crdPm = cs()->cConv(pmp->cs(), crdPm);
	    if (crdPm.fUndef())
	      continue;
			double rSqDist = dis.rDistance2(crdRC, crdPm);
			if (rSqDist < rMinSqDist)  {
			  rMinSqDist = rSqDist;
			  rMinDif = sqrt(rMinSqDist);
			  iMinP = iPc;
			}
		}
		for (long iPc = iPlp-1; iPc >= 1; iPc--)  {
			long iSLp = iSortList[iPc];
			Coord crdPm = pmp->cValue (iSLp);
    // in the list we are moving towards smaller Y
			double rDistan2 = dis.rDistance2(crdRC, crdPm);
			if (rDistan2 < rMinSqDist)  {
			 rMinSqDist = rDistan2;
			 rMinDif = sqrt(rMinSqDist);
			  iMinP = iPc;
			}
		}
	}
  return iMinP;
}

bool MapNearestPoint::fFreezing()
{
  RowCol rcRc;
  Coord crdRC;
  iNrPoints = pmp->iFeatures();
  iSortList.Resize(iNrPoints + 1);

  StoreType st = pmp->st();
  // determine size of map in row's and col's
  RowCol rcMrc = gr()->rcSize();
  long iMRow = rcMrc.Row;
  long iMCol = rcMrc.Col;
  // declaration to declare the names for the compiler
  //    array's are resized as needed
  RealBuf rBufOut(1);
  LongBuf iBufOut(1);
  // create output buffer according to store type
  switch (st) {
    case stREAL   :  {
       rBufOut.Size(iMCol); }
    case stLONG   :
    case stINT    :
    case stBYTE   :
    case stNIBBLE :
    case stDUET   :
    case stBIT    : {
        iBufOut.Size(iMCol); 
				rBufOut.Size(iMCol);//in case of 0 != dm()->pdv() (input has dom value)  
		}
  };

  trq.SetText(SMAPTextCalculating);
  trq.SetTitle(SMAPTextMapNearestPoint);
  // Code changed so that points with undef values are not entered
  // in the list DV
  long iValCnt = 0;
  if (st == stREAL)
    for (long iPc = 0; iPc < iNrPoints; iPc++)  {
      if (pmp->rValue(iPc)!= rUNDEF && !pmp->cValue(iPc).fUndef()) {
        iValCnt++;
        iSortList[iValCnt] = iPc;
      }
    }
  else
    for (long iPc = 0; iPc < iNrPoints; iPc++)  {
      if (pmp->iRaw(iPc)!= iUNDEF && !pmp->cValue(iPc).fUndef()) {
        iValCnt++;
        iSortList[iValCnt] = iPc;
      }
    }
  iNrPoints = iValCnt;
  QuickSort(1, iNrPoints, LessFunc, SwapFunc, this);
//       (IlwisObjectPtrLessProc)&MapNearestPoint::LessFunc,
//       (IlwisObjectPtrSwapProc)&MapNearestPoint::SwapFunc);


  for (long iRc = 0; iRc < iMRow; iRc++)  {
    if (trq.fUpdate(iRc, iMRow))
      return false;

    // start new Row, get coordinate of first point
    // reset pointers and distances
    long iMinP = 1;             // pointer to point with minimal distance
    for (long iCc = 0; iCc < iMCol; iCc++)  {
      iMinP= SearchNP(iMinP, iRc, iCc);
      // Retrieve attribute of nearest neighbour depending of attribute type
      double rel; long iel;
			if (0 != dm()->pdv())
      //if (st == stREAL)  
			{
        rel =  pmp->rValue(iSortList[iMinP]);
        rBufOut[iCc] = rel;
      }
      else  {
        iel =  pmp->iRaw(iSortList[iMinP]);
        iBufOut[iCc] = iel;
      }
    }
    // write out row of map
		if (0 != dm()->pdv())
    //if (st == stREAL) 
      pms->PutLineVal(iRc,rBufOut);
    else
      pms->PutLineRaw(iRc,iBufOut);
  }
  return true;
}

String MapNearestPoint::sExpression() const
{
	String sDistanceMethod;;
	if (m_distMeth == Distance::distSPHERE)
		sDistanceMethod = String("sphere");
	else
		sDistanceMethod = String("plane");
  return String("MapNearestPoint(%S,%S,%S)", pmp->sNameQuoted(true, fnObj.sPath()),
                                          gr()->sNameQuoted(true, fnObj.sPath()),
																					sDistanceMethod);
}




