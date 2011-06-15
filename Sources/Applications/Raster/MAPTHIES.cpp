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
/* MapThiessen
   Copyright Ilwis System Development ITC
   august 1996, by Jelle Wind
	Last change:  JEL   7 May 97    9:30 am
*/

#include "Engine\Base\DataObjects\valrange.h"
#include "Engine\Base\DataObjects\WPSMetaData.h"
#include "Applications\Raster\MAPDist.H"
#include "Applications\Raster\MAPThies.H"
#include "Headers\Htp\Ilwisapp.htp"
#include "Headers\Err\Ilwisapp.err"

IlwisObjectPtr * createMapThiessen(const FileName& fn, IlwisObjectPtr& ptr, const String& sExpr, vector<void *> parms ) {
	if ( sExpr != "")
		return (IlwisObjectPtr *)MapThiessen::create(fn, (MapPtr &)ptr, sExpr);
	else
		return (IlwisObjectPtr *)new MapThiessen(fn, (MapPtr &)ptr);
}

#define HIVAL (LONG_MAX >> 1)

String wpsmetadataMapThiessen() {
	WPSMetaData metadata("MapThiessen");
	return metadata.toString();
}

ApplicationMetadata metadataMapThiessen(ApplicationQueryData *query) {
	ApplicationMetadata md;
	if ( query->queryType == "WPSMETADATA" || query->queryType == "") {
		md.wpsxml = wpsmetadataMapThiessen();
	}
	if ( query->queryType == "OUTPUTTYPE" || query->queryType == "")
		md.returnType = IlwisObject::iotRASMAP;
	if ( query->queryType == "EXPERSSION" || query->queryType == "")
		md.skeletonExpression =  MapThiessen::sSyntax();

	return md;
}

const char * MapThiessen::sSyntax()
 { return "MapThiessen(sourcemap,distancemap)\nMapThiessen(sourcemap,weightmap,distancemap)"; }


MapThiessen* MapThiessen::create(const FileName& fn, MapPtr& p, const String& sExpr)
{
  Array<String> as;
  int iParms = IlwisObjectPtr::iParseParm(sExpr, as);
  if ((iParms <2) || (iParms > 3))
    ExpressionError(sExpr, sSyntax());
  Map mpSrc(as[0], fn.sPath());
  Map mpWgth;
  if ((iParms == 3) && (as[1].length() != 0) && (as[1] != "1"))
    mpWgth = Map(as[1], fn.sPath());
  return new MapThiessen(fn, p, mpSrc, mpWgth, as[iParms-1]);
}

MapThiessen::MapThiessen(const FileName& fn, MapPtr& p)
: MapFromMap(fn, p)
{
  String sWeightMap;
  ReadElement("MapThiessen", "WeightMap", sWeightMap);
  if (sWeightMap != "1")
    ReadElement("MapThiessen", "WeightMap", mpWeight);
  if (mpWeight.fValid())
    objdep.Add(mpWeight.ptr());
  ReadElement("MapThiessen", "DistanceMap", sDistMap);
  fNeedFreeze = true;
  Init();
}

MapThiessen::MapThiessen(const FileName& fn, MapPtr& p, const Map& mapSrc, const Map& mapWght, const String& sDist)
: MapFromMap(fn, p, mapSrc), mpWeight(mapWght), sDistMap(sDist)
{
  if (mpWeight.fValid()) {
    if (!mpWeight->dvrs().fValues())
      ValueDomainError(mpWeight->dm()->sName(true, fnObj.sPath()), mpWeight->sTypeName(), errMapThiessen);
    objdep.Add(mpWeight.ptr());
  }  
  fNeedFreeze = true;
  Init();
  if (!fnObj.fValid())
    objtime = objdep.tmNewest();
}

void MapThiessen::Init()
{
  sFreezeTitle = "MapThiessen";
  htpFreeze = "ilwisapp\\distance_calculation_algorithm.htm";
}

void MapThiessen::Store()
{
  MapFromMap::Store();
  WriteElement("MapFromMap", "Type", "MapThiessen");
  WriteElement("MapThiessen", "WeightMap", mpWeight);
  WriteElement("MapThiessen", "DistanceMap", sDistMap);
}

MapThiessen::~MapThiessen()
{
}

bool MapThiessen::fFreezing()
{
  FileName fnDistMap(sDistMap, fnObj);
  fnDistMap.sExt = ".mpr";
  Map mpDist;
  // check existence of distance map
  if (File::fExist(fnDistMap)) {
    try {
      mpDist = Map(fnDistMap);
    }
    catch (const ErrorObject&) {
    }  
  }
  if (mpDist.fValid()) {
    if (0 == dynamic_cast<MapDistance*>(mpDist.ptr()))
      mpDist = Map();
  }    
  if (!mpDist.fValid()) {
    String sWeight = "1";
    if (mpWeight.fValid())
      sWeight = mpWeight->sNameQuoted(true, fnObj.sPath());
    mpDist = Map(fnDistMap, String("MapDistance(%S,%S,%S)", mp->sNameQuoted(true, fnObj.sPath()), sWeight, 
                   ptr.sNameQuoted(true, fnObj.sPath())));
  }                 
  if (!mpDist->fDependent())
    return false;
  mpDist->DeleteCalc();
  mpDist->Calc();
  return mpDist->fCalculated();
}

String MapThiessen::sExpression() const
{
  if (!mpWeight.fValid())
    return String("MapThiessen(%S,%S)", mp->sNameQuoted(true, fnObj.sPath()), sDistMap);
  return String("MapThiessen(%S,%S,%S)", mp->sNameQuoted(true, fnObj.sPath()), 
                                         mpWeight->sNameQuoted(true, fnObj.sPath()), sDistMap);
}




