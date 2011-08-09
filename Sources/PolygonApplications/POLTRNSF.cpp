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
// $Log: /ILWIS 3.0/PolygonMap/POLTRNSF.cpp $
 * 
 * 7     8-02-01 17:12 Hendrikse
 * implem errormessage TR("Use of attribute maps is not possible: '%S'") 
 * 
 * 6     17-10-00 14:34 Hendrikse
 * implementeded members and functions for densifying polygon coords
 * 
 * 5     16-10-00 17:02 Hendrikse
 * removed the 1000 coords limit in fFreezing ( CoordBuf crdBuf;//(1000);)
 * 
 * 4     10-12-99 3:14p Martin
 * removed internal coordinates and used true coords
 * 
 * 3     9/08/99 11:57a Wind
 * comments
 * 
 * 2     9/08/99 10:23a Wind
 * adpated to use of quoted file names
 */
// Revision 1.4  1998/09/16 17:25:54  Wim
// 22beta2
//
// Revision 1.3  1997/08/13 15:05:39  Wim
// Do not add superflous extra new polygons
//
// Revision 1.2  1997-08-05 12:49:37+02  Wim
// sSyntax() corrected
//
/* PolygonMapTransform
   Copyright Ilwis System Development ITC
   may 1996, by Jelle Wind
	Last change:  WK   13 Aug 97    5:03 pm
*/                                                                      

#include "PolygonApplications\POLTRNSF.H"
#include "Engine\Map\Polygon\POLSTORE.H"
#include "Engine\Base\DataObjects\valrange.h"
#include "Headers\Err\Ilwisapp.err"
#include "Headers\Htp\Ilwisapp.htp"
#include "Headers\Hs\polygon.hs"
#include "Engine\Base\Round.h"

#define EPS10 1.0e-10

IlwisObjectPtr * createPolygonMapTransform(const FileName& fn, IlwisObjectPtr& ptr, const String& sExpr, vector<void *> parms ) {
	if ( sExpr != "")
		return (IlwisObjectPtr *)PolygonMapTransform::create(fn, (PolygonMapPtr &)ptr, sExpr);
	else
		return (IlwisObjectPtr *)new PolygonMapTransform(fn, (PolygonMapPtr &)ptr);
}

const char* PolygonMapTransform::sSyntax() {
  return "PolygonMapTransform(polmap,coordsys[,dist])";
}

double PolygonMapTransform::rDefaultDensifyDistance(const PolygonMap& polmap)
{
  double w = polmap->cb().width();
  double h = polmap->cb().height();
  double m = min(w,h);
  long iNrPol = polmap->iFeatures();
  if (iNrPol == 0 || !polmap->cs().fValid())
    return rUNDEF;
  else
	if (polmap->cs()->pcsLatLon()) 
		return rRoundDMS(m / 1000); // default distance in DMS
	else								// between denser coords
		return rRound (m / 1000); //default distance in meters
}

PolygonMapTransform* PolygonMapTransform::create(const FileName& fn, PolygonMapPtr& p, const String& sExpr)
{
  Array<String> as;
  short iParms = IlwisObjectPtr::iParseParm(sExpr, as);
  if (iParms > 3 || iParms < 2)
        ExpressionError(sExpr, sSyntax());
	String sInputPolMapName = as[0];
	char *pCh = sInputPolMapName.strrchrQuoted('.');
  if ((pCh != 0) && (0 != _strcmpi(pCh, ".mps")))  // attrib map
		throw ErrorObject(WhatError(String(TR("Use of attribute maps is not possible: '%S'").c_str(), as[0]),
																 errPolygonMapTransform), fn);
  PolygonMap pmp(as[0], fn.sPath());
  CoordSystem csy(as[1], fn.sPath());
	double rD = 0; // default no densifying
  if (iParms > 2)
    rD = max(as[2].rVal(), 0);  // default distance over-ruled by user
  return new PolygonMapTransform(fn, p, pmp, csy, rD);
}

PolygonMapTransform::PolygonMapTransform(const FileName& fn, PolygonMapPtr& p)
: PolygonMapVirtual(fn, p)
{
  fNeedFreeze = true;
  String sColName;
  try {
    ReadElement("PolygonMapTransform", "PolygonMap", pmp);
    ReadElement("PolygonMapTransform", "CoordSys", csy);
		ReadElement("PolygonMapTransform", "Distance", rDistance);
  }
  catch (const ErrorObject& err) {  // catch to prevent invalid object
    err.Show();
    return;
  }
  Init();
  objdep.Add(pmp.ptr());
  objdep.Add(csy.ptr());
}

PolygonMapTransform::PolygonMapTransform(const FileName& fn, PolygonMapPtr& p, const PolygonMap& pm, 
                                   const CoordSystem& cs, const double rD)
: PolygonMapVirtual(fn, p, cs,cs->cb,pm->dvrs()), pmp(pm), csy(cs), rDistance(rD)
{
  if (!cs->fConvertFrom(pmp->cs()))
    IncompatibleCoordSystemsError(cs->sName(true, fnObj.sPath()), pmp->cs()->sName(true, fnObj.sPath()), sTypeName(), errPolygonMapTransform+1);
  fNeedFreeze = true;
  Init();
  objdep.Add(pmp.ptr());
  objdep.Add(csy.ptr());
  if (!fnObj.fValid()) // 'inline' object
    objtime = objdep.tmNewest();
  if (pmp->fTblAttSelf())
    SetAttributeTable(pmp->tblAtt());
  SetCB(csy->cbConv(pmp->cs(), pmp->cb()));
}

void PolygonMapTransform::Store()
{
  PolygonMapVirtual::Store();
  WriteElement("PolygonMapVirtual", "Type", "PolygonMapTransform");
  WriteElement("PolygonMapTransform", "PolygonMap", pmp);
  WriteElement("PolygonMapTransform", "CoordSys", csy);
	WriteElement("PolygonMapTransform", "Distance", rDistance);
}

PolygonMapTransform::~PolygonMapTransform()
{
}

String PolygonMapTransform::sExpression() const
{
  return String("PolygonMapTransform(%S,%S)", pmp->sNameQuoted(false, fnObj.sPath()), 
                                              csy->sNameQuoted(false, fnObj.sPath()),
																							rDistance);
}


void PolygonMapTransform::Init()
{
  htpFreeze = "ilwisapp\transform_polygon_map_functionality_algorithm.htm";
  sFreezeTitle = "PolygonMapTransform";
}

class CoordinateTransformer : public geos::geom::CoordinateFilter {
public:
	CoordinateTransformer(const CoordSystem& _csOld, const CoordSystem& _csNew) {
		csOld = _csOld;
		csNew = _csNew;
	}
	void filter_rw(Coordinate *crdOld) const {
		Coord newCoord = csNew->cConv(csOld, Coord(*crdOld));
		crdOld->x = newCoord.x;
		crdOld->y = newCoord.y;
	}
private:
	CoordSystem csOld;
	CoordSystem csNew;
};

bool PolygonMapTransform::fFreezing()
{
	trq.SetText(TR("Copying polygons"));
	long iPol = pmp->iFeatures();
	CoordBuf crdBufIn, crdBufOut;
	bool fDensify = false;
	if (rDistance > EPS10)
		fDensify = true;
	trq.SetText(String(TR("Transforming segments for '%S'").c_str(), sName(true, fnObj.sPath())));
	Coord crd;
	CoordSystem csOld = pmp->cs();
	for (long i = 0; i < iPol; ++i) {
		if (trq.fUpdate(i, iPol))
			return false;
		ILWIS::Polygon *pol = (ILWIS::Polygon *)pmp->getFeature(i);
		if (!pol->fValid()) {
			continue;
		}  
		ILWIS::Polygon *newPol;

		if (fDensify) 
			newPol = DensifyPolygon(rDistance, pol);
		else 
			newPol = (ILWIS::Polygon *)pol->clone();

		CoordinateTransformer filter(csOld,csy);
		newPol->apply_rw(&filter);
		pms->addPolygon(newPol);
	}

	trq.fUpdate(iPol, iPol);
	return true;
}

  

ILWIS::Polygon *PolygonMapTransform::DensifyPolygon(const double rDistance, ILWIS::Polygon *pol) {
	
	ILWIS::Polygon *polNew = CPOLYGON(pms->newFeature());
	CoordBuf cbOut;
	Densify(rDistance,pol->getExteriorRing(), cbOut);
	polNew->addBoundary(new LinearRing(cbOut.clone(), new GeometryFactory()));
	for(int i=0; i < pol->getNumInteriorRing(); ++i) {
		CoordBuf cbHole;
		Densify(rDistance, pol->getInteriorRingN(i), cbHole);
		polNew->addHole(new LinearRing(cbHole.clone(), new GeometryFactory()));
	}
	if (dvrs().fValues()) {
		polNew->PutVal(pol->rValue());
	}else {
		polNew->PutVal(pol->iValue());
	}
	return polNew;
	
}
void PolygonMapTransform::Densify(const double rDistance, const LineString *line, CoordBuf& crdBufOut) const
{
	long iCrdIn, iNrInCrds;                
	double rNewCoordsDistance = abs(rDistance);
	double dx, dy, rLegLength, rDx, rDy;
	const CoordinateSequence *seq = line->getCoordinates();
	CoordBuf crdBufIn(seq), crdBufTemp;
	iNrInCrds = crdBufIn.size();
	long iCrdOut = 0; // initialize counter of coords in new segment
	long iInsertablePoints;// per leg of 'old' segment
	long iInsertedPoints;  // counts for each 'old' leg again from 0	
	crdBufTemp.Size((int)(line->getLength()/rNewCoordsDistance) + iNrInCrds);
	for (iCrdIn = 0; iCrdIn < iNrInCrds - 1; iCrdIn++) { 
		crdBufTemp[iCrdOut] = crdBufIn[iCrdIn]; // copy the input-vertex coord to crdBufOut
		iCrdOut++;
		dx = crdBufIn[iCrdIn + 1].x - crdBufIn[iCrdIn].x;   // determine for each old leg
		dy = crdBufIn[iCrdIn + 1].y - crdBufIn[iCrdIn].y;   // its x and y components
		rLegLength = sqrt(dx*dx + dy*dy) ;        // the length of this leg 
		if ( rLegLength < EPS10 ) continue;       // if its length ~ 0 it is skipped,
		rDx = rNewCoordsDistance * dx / rLegLength; // compute the new leg vector
		rDy = rNewCoordsDistance * dy / rLegLength; // components along that leg
		iInsertedPoints = 0;
		iInsertablePoints = (long)floor(rLegLength / rNewCoordsDistance);
		if (iInsertablePoints > 0 ) {                   // densification of current leg required
			while (iInsertedPoints < iInsertablePoints) {       
				crdBufTemp[iCrdOut].x = crdBufTemp[iCrdOut - 1].x + rDx;
				crdBufTemp[iCrdOut].y = crdBufTemp[iCrdOut - 1].y + rDy;
				iCrdOut++;
				iInsertedPoints++;	
			}
		}
	}
	crdBufTemp[iCrdOut] = crdBufIn[iNrInCrds - 1]; //copy last vertex (end-node)
	iCrdOut++;
	crdBufOut.Size(iCrdOut);
	for (int i = 0; i < iCrdOut; i++) 
		crdBufOut[i] = crdBufTemp[i];
	delete seq;
}




