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
// MapListMatrixMultiply.cpp: implementation of the MapListMatrixMultiply class.
//
//////////////////////////////////////////////////////////////////////

#include "Headers\toolspch.h"
#include "Engine\Base\System\Engine.h"
#include "Engine\Base\DataObjects\ilwisobj.h"
#include "Engine\Applications\ModuleMap.h"
#include "Engine\Base\DataObjects\WPSMetaData.h"
#include "Applications\MapList\MapListMatrixMultiply.h"
#include "Headers\Hs\maplist.hs"

namespace {
  const char* sSyntax() 
  {
    return "MapListMatrixMultiply(maplist,matrix)";
  }
}

IlwisObjectPtr * createMapListMatrixMultiply(const FileName& fn, IlwisObjectPtr& ptr, const String& sExpr, vector<void *> parms ) {
	if ( sExpr != "")
		return (IlwisObjectPtr *)new MapListMatrixMultiply(fn, (MapListPtr &)ptr, sExpr);
	else
		return (IlwisObjectPtr *)new MapListMatrixMultiply(fn, (MapListPtr &)ptr);
}

String wpsmetadataMapListMatrixMultiply() {
	WPSMetaData metadata("MapListMatrixMultiply");
	metadata.AddTitle("MapListMatrixMultiply");
	metadata.AddAbstract("A maplist is created which contains the transformed raster maps when the matrix is applied to the map(s). This is done with Principal components and Factor Analysis ");
	metadata.AddKeyword("spatial");
	metadata.AddKeyword("raster");
	metadata.AddKeyword("Principal Components");
	metadata.AddKeyword("Factor Analysis");
	WPSParameter parm1("inputmap","string");
	parm1.AddTitle("Filename Inputmap");
	WPSParameter parm2("matrix object","string");
	parm2.AddTitle("Matrix object to be used");
	WPSParameter parm3("NrOutput maps","integer");
	parm3.AddTitle("Number of output maps in resulting maplist");
	metadata.AddParameter(parm1);
	metadata.AddParameter(parm2);
	metadata.AddParameter(parm3);
	WPSParameter parmout("outputmaplist","string",false);
	parmout.AddTitle("Filename Output maplist");
	metadata.AddParameter(parmout);

	return metadata.toString();
}

ApplicationMetadata metadataMatrixMultiply(ApplicationQueryData *query) {
	ApplicationMetadata md;
	if ( query->queryType == "WPSMETADATA") {
		md.wpsxml = wpsmetadataMapListMatrixMultiply();
	}
	if ( query->queryType == "OUTPUTTYPE")
		md.returnType = IlwisObject::iotMAPLIST;

	return md;
}

MapListMatrixMultiply::MapListMatrixMultiply(const FileName& fn, MapListPtr& ptr)
: MapListVirtual(fn, ptr, false)
{
  ReadElement("MapListMatrixMultiply", "MapList", ml);
  FileName fnMat;
  ReadElement("MapListMatrixMultiply", "Matrix", fnMat);
  mat = MatrixObject(fnMat);
  ReadElement("MapListMatrixMultiply", "NrOutMaps", iOutBands);
  Init();
}
	
MapListMatrixMultiply::MapListMatrixMultiply(const FileName& fn, MapListPtr& ptr, const String& sExpr)
: MapListVirtual(fn, ptr, true)
, iOutBands(0)
{
  Array<String> as;
  short iParms = IlwisObjectPtr::iParseParm(sExpr, as);
  if (iParms < 2 || iParms > 3)
    ExpressionError(sExpr, sSyntax());  
  ml = MapList(FileName(as[0]));
  if (!ml.fValid())
    throw ErrorObject(WhatError("MapList expected",0), WhereError(as[0]));
  mat = MatrixObject(as[1]);
  if (!mat.fValid())
    throw ErrorObject(WhatError("Matrix expected",0), WhereError(as[1]));
  if (3 == iParms)
    iOutBands = as[2].iVal();
  if (iOutBands < 0)
    iOutBands = 0;
  objdep.Add(ml);
  objdep.Add(mat);
  Init();
}

MapListMatrixMultiply::~MapListMatrixMultiply()
{
}

void MapListMatrixMultiply::Store()
{
  MapListVirtual::Store();
  WriteElement("MapListVirtual", "Type", "MapListMatrixMultiply");
  WriteElement("MapListMatrixMultiply", "MapList", ml);
  WriteElement("MapListMatrixMultiply", "Matrix", mat->fnObj);
  if (iOutBands > 0)
    WriteElement("MapListMatrixMultiply", "NrOutMaps", iOutBands);
  else
    WriteElement("MapListMatrixMultiply", "NrOutMaps", (char*)0);
}

void MapListMatrixMultiply::Init()
{
  sFreezeTitle = "MapListMatrixMultiply";
}

bool MapListMatrixMultiply::fFreezing()
{
  GeoRef grf = ml->gr();
  ptr.SetGeoRef(grf);

  trq.SetText(SMPLTextConstructing);
  int iMaps = mat->iRows();
  if (iOutBands > 0 && iOutBands < iMaps)
    iMaps = iOutBands;
    
  SetSize(iMaps);
  if (0 == iMaps)
    return false;

  Domain dmValue("value.dom");

  for (int i = 0; i < iMaps; ++i)
  {
    if (trq.fUpdate(i, iMaps)) 
    {
      SetSize(0);
      return false;
    }
    String sMap("%S_%i", fnObj.sFile, i+1);
    FileName fnMap(sMap);
    map(i) = Map(fnMap, grf, grf->rcSize(), dmValue);
  }

  trq.SetText(SMPLTextCalculating);
  RowCol rcSize = grf->rcSize();
  int iLines = rcSize.Row;
  int iCols = rcSize.Col;
  int iMapsIn = min(ml->iSize(), mat->iCols());
  RealBuf* bufIn = new RealBuf[iMapsIn];
  for (int i = 0; i < iMapsIn; ++i)
    bufIn[i].Size(iCols);
  RealBuf* bufMat =  new RealBuf[iMaps];
  for (int i = 0; i < iMaps; ++i) {
    bufMat[i].Size(iMapsIn);
    for (int j = 0; j < iMapsIn; ++j)
      bufMat[i][j] = mat->mat(i,j);
  }
  RealBuf bufOut(iCols);
  for (int iLine = 0; iLine < iLines; ++iLine)
  {
    if (trq.fUpdate(iLine, iLines))
      return false;
    for (int i = 0; i < iMapsIn; ++i)
      ml[i]->GetLineVal(iLine, bufIn[i]);
    for (int i = 0; i < iMaps; ++i)
    {
      for (int c = 0; c < iCols; ++c)
      {
        double rRes = 0;
        for (int j = 0; j < iMapsIn; ++j)
          rRes += bufMat[i][j] * bufIn[j][c];
        bufOut[c] = rRes;
      }
      map(i)->PutLineVal(iLine, bufOut);
    }
  }
  delete [] bufIn;
  delete [] bufMat;
  return true;
}

String MapListMatrixMultiply::sExpression() const
{
  if (iOutBands > 0)
    return String("MapListMatrixMultiply(%S,%S,%i)", ml->sName(), mat->sName(), iOutBands);
  else    
    return String("MapListMatrixMultiply(%S,%S)", ml->sName(), mat->sName());
}
