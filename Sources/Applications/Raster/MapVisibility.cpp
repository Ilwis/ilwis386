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
// $Log: /ILWIS 3.0/RasterApplication/MapVisibility.cpp $
 * 
 * 2     11/16/01 11:47 Hendrikse
 * now sets default Height-scaling at 1
 * made larger tolerance: 5 * PixelSize
 * 
 * 1     8/10/01 20:35 Hendrikse
 * MapVisibility makes from a rasterDTM (heightvalues) a boolean rastermap
 * with the same georef where pixels are true if they are visible from a
 * given observer point (X,Y,Z) coordinates 
 * 
 
/* MapVisibility
   Copyright Ilwis System Development ITC
   august 2001, by Jan Hendrikse
*/
#include "Applications\Raster\MapVisibility.h"
#include "Engine\Base\DataObjects\valrange.h"
#include "Headers\Htp\Ilwisapp.htp"
#include "Headers\Err\Ilwisapp.err"
#include "Headers\Hs\map.hs"
#include "Headers\Hs\Georef.hs"
#include "Engine\SpatialReference\GR3D.H"

IlwisObjectPtr * createMapVisibility(const FileName& fn, IlwisObjectPtr& ptr, const String& sExpr, vector<void *> parms ) {
	if ( sExpr != "")
		return (IlwisObjectPtr *)MapVisibility::create(fn, (MapPtr &)ptr, sExpr);
	else
		return (IlwisObjectPtr *)new MapVisibility(fn, (MapPtr &)ptr);
}

const char* MapVisibility::sSyntax() {
  return "MapVisibility(map,observerXcrd,observerYcrd,observerHeight)";
}

MapVisibility* MapVisibility::create(const FileName& fn, MapPtr& p, const String& sExpr)
{
  Array<String> as;
  String sFunc = IlwisObjectPtr::sParseFunc(sExpr);
  int iParms = IlwisObjectPtr::iParseParm(sExpr, as);
  if (iParms != 4) 
    ExpressionError(sExpr, sSyntax());
  Map mp(as[0], fn.sPath());
  if (fCIStrEqual(sFunc, "MapVisibility")) {
    
    Coord crd;
		double rHeight;
    crd.x = as[1].rVal();
    crd.y = as[2].rVal();
    rHeight= as[3].rVal();
    if ((crd.x == rUNDEF) || (crd.y == rUNDEF) || (rHeight == rUNDEF))
      ExpressionError(sExpr, sSyntax());
    return new MapVisibility(fn, p, mp, crd, rHeight);
  }
  return 0;
}

MapVisibility::MapVisibility(const FileName& fn, MapPtr& p)
: MapFromMap(fn, p)
{
  ReadElement("MapVisibility", "Observer Coordinate", crdObserver);
  ReadElement("MapVisibility", "Observer Height", rHeightObserver);
  Init();
}

MapVisibility::MapVisibility(const FileName& fn, MapPtr& p,
           const Map& mp, const Coord& crd, const double& rH)
: MapFromMap(fn, p, mp), crdObserver(crd), rHeightObserver(rH)
{
  
  if (gr()->fGeoRefNone()) 
      throw ErrorGeoRefNone(mp->fnObj, errMapSubMap+6);
	ptr.SetDomainValueRangeStruct(Domain("bool"));
  Init();
  if (!fnObj.fValid())
    objtime = objdep.tmNewest();
  fNeedFreeze = false;
}

void MapVisibility::Store()
{
  MapFromMap::Store();
  WriteElement("MapFromMap", "Type", "MapVisibility");
  WriteElement("MapVisibility", "Observer Coordinate", crdObserver);
  WriteElement("MapVisibility", "Observer Height", rHeightObserver);
}

MapVisibility::~MapVisibility()
{
}

void MapVisibility::Init()
{
  fNeedFreeze = false;
  sFreezeTitle = "MapVisibility";
//  htpFreeze = htpMapVisibilityT;
}

bool MapVisibility::fFreezing()
{ 
  GeoRef3D* grf3dSW = new GeoRef3D(FileName(),rcSize(), mp);
	GeoRef3D* grf3dNE = new GeoRef3D(FileName(),rcSize(), mp);
		// 2 georef3D's in order to view complete landscape
	CoordBounds cb = mp->cb();
	Coord cFocal = cb.cMin; // define 2 focal points to reach whole area
	cFocal += Coord(1,1);		// and avoid the border of the DTM
	grf3dSW->SetScaleHeight(1);
	grf3dSW->SetFocalPoint(cFocal);
	grf3dSW->SetViewPoint(crdObserver);
	grf3dSW->SetViewHeight(rHeightObserver);
	grf3dSW->SetViewAngle(180);
	//grf3dSW->SetScaleHeight(1);
	cFocal = cb.cMax;
	cFocal -= Coord(1,1);
	grf3dNE->SetScaleHeight(1);
	grf3dNE->SetFocalPoint(cFocal); 
	grf3dNE->SetViewPoint(crdObserver);
	grf3dNE->SetViewHeight(rHeightObserver);
	grf3dNE->SetViewAngle(180);
	//grf3dNE->SetScaleHeight(1);
	long iNrRows = mp->iLines();
  long iNrCols = mp->iCols();
  LongBuf iBufOut(iNrCols);
	double rTolerance = gr()->rPixSize() * 5;
	Coord  cDTM;
	RowCol rcTerrain;
	Coord cReturnedSW, cReturnedNE;

	double rRow, rCol;
	for (long iRow = 0; iRow < iNrRows; ++iRow)  
	{  
    if (trq.fUpdate(iRow, iNrRows))
      return false;
		rcTerrain.Row = iRow;
    for (long iCol = 0; iCol < iNrCols; ++iCol)  
		{  
      if (trq.fAborted())
        return false;
			rcTerrain.Col = iCol;
			cDTM = gr()->cConv(rcTerrain);  // take terrain-coord of DTM RowCol
			grf3dSW->Coord2RowCol(cDTM,rRow,rCol);
			grf3dSW->RowCol2Coord(rRow,rCol,cReturnedSW);
			grf3dNE->Coord2RowCol(cDTM,rRow,rCol);
			grf3dNE->RowCol2Coord(rRow,rCol,cReturnedNE);
			if (cReturnedSW.fNear(cDTM,rTolerance) || cReturnedNE.fNear(cDTM,rTolerance))
        iBufOut[iCol] = true; // if one of the 2 results matches well, it's OK
      else
        iBufOut[iCol] = false;
    }
    pms->PutLineVal(iRow,iBufOut);
  }
	return true;
}

String MapVisibility::sExpression() const
{
    return String("MapVisibility(%S, %2f,%2f,%2f)", mp->sNameQuoted(true, fnObj.sPath()), 
                  crdObserver.x, crdObserver.y, rHeightObserver);
}
