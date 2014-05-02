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

 Created on: 2014-05-02
*/                                                                      
#include "Engine\Table\tbl.h"
#include "Engine\Base\DataObjects\ObjectCollection.h"
#include "Engine\Applications\ObjectCollectionVirtual.h"
#include "Engine\Base\DataObjects\WPSMetaData.h"
#include "ProbabilityDensityFromPoints.h"
#include "Engine\Map\Raster\Map.h"
#include "Client\ilwis.h"

IlwisObjectPtr * createProbabilityDensityFromPoints(const FileName& fn, IlwisObjectPtr& ptr, const String& sExpr, vector<void *> parms ) {
	if ( sExpr != "")
		return (IlwisObjectPtr *)new ProbabilityDensityFromPoints(fn, (ObjectCollectionPtr &)ptr, sExpr);
	else
		return (IlwisObjectPtr *)new ProbabilityDensityFromPoints(fn, (ObjectCollectionPtr &)ptr);
}

namespace {
	const char* sSyntax() 
	{
		return "ProbabilityDensityFromPoints(input_pointmap, distance_attr, sigma_distance_attr, direction_attr, sigma_direction_attr)";
	}
}

const char* ProbabilityDensityFromPoints::sSyntax() {
  return "ProbabilityDensityFromPoints(input_pointmap, distance_attr, sigma_distance_attr, direction_attr, sigma_direction_attr)";
}

String wpsmetadataProbabilityDensityFromPoints() {
	WPSMetaData metadata("ProbabilityDensityFromPoints");
	return metadata.toString();
}

ApplicationMetadata metadataProbabilityDensityFromPoints(ApplicationQueryData *query) {
	ApplicationMetadata md;
	if ( query->queryType == "WPSMETADATA" || query->queryType == "")
		md.wpsxml = wpsmetadataProbabilityDensityFromPoints();
	if ( query->queryType == "OUTPUTTYPE" || query->queryType == "")
		md.returnType = IlwisObject::iotOBJECTCOLLECTION;
	if ( query->queryType == "EXPERSSION" || query->queryType == "")
		md.skeletonExpression =  sSyntax();

	return md;
}

ProbabilityDensityFromPoints::ProbabilityDensityFromPoints(const FileName& fn, ObjectCollectionPtr& p)
: ObjectCollectionVirtual(fn, p, false)
{
  fNeedFreeze = true;
  try {
	FileName fn;
    ReadElement("ProbabilityDensityFromPoints", "ReferencePoints", fn);
	pmReferencePoints = PointMap(fn);
    ReadElement("ProbabilityDensityFromPoints", "DistanceAttr", sDistanceAttr);
    ReadElement("ProbabilityDensityFromPoints", "SigmaDistanceAttr", sSigmaDistanceAttr);
    ReadElement("ProbabilityDensityFromPoints", "DirectionAttr", sDirectionAttr);
    ReadElement("ProbabilityDensityFromPoints", "SigmaDirectionAttr", sSigmaDirectionAttr);
  }
  catch (const ErrorObject& err) {  // catch to prevent invalid object
    err.Show();
    return;
  }
  Init();
}

ProbabilityDensityFromPoints::ProbabilityDensityFromPoints(const FileName& fn, ObjectCollectionPtr& p, const String& sExpr)
: ObjectCollectionVirtual(fn, p, true)
{
  Array<String> as(5);
  if (!IlwisObjectPtr::fParseParm(sExpr, as))
    ExpressionError(sExpr, sSyntax());
  String sInputMapName = as[0];
  FileName fnInput(sInputMapName, ".mpp");
  if (!fnInput.fExist())
    throw ErrorObject(TR("Input pointmap doesnt exist"));
  pmReferencePoints = PointMap(fnInput);
  sDistanceAttr = as[1];
  sSigmaDistanceAttr = as[2];
  sDirectionAttr = as[3];
  sSigmaDirectionAttr = as[4];
  if (sDistanceAttr == "" || sSigmaDistanceAttr == "" || sDirectionAttr == "" || sSigmaDirectionAttr == "")
    throw ErrorObject(TR("attributes cannot be empty"));
  fNeedFreeze = true;
  Init();
  objdep.Add(pmReferencePoints);
}


void ProbabilityDensityFromPoints::Store()
{
  ObjectCollectionVirtual::Store();
  FileName fn;
  WriteElement("ObjectCollectionVirtual", "Type", "ProbabilityDensityFromPoints");
  WriteElement("ProbabilityDensityFromPoints", "ReferencePoints", pmReferencePoints->fnObj);
  WriteElement("ProbabilityDensityFromPoints", "DistanceAttr", sDistanceAttr);
  WriteElement("ProbabilityDensityFromPoints", "SigmaDistanceAttr", sSigmaDistanceAttr);
  WriteElement("ProbabilityDensityFromPoints", "DirectionAttr", sDirectionAttr);
  WriteElement("ProbabilityDensityFromPoints", "SigmaDirectionAttr", sSigmaDirectionAttr);
}

ProbabilityDensityFromPoints::~ProbabilityDensityFromPoints()
{
}

String ProbabilityDensityFromPoints::sExpression() const
{
	return String("ProbabilityDensityFromPoints(%S)", pmReferencePoints->sNameQuoted(true, fnObj.sPath()));
}

bool ProbabilityDensityFromPoints::fDomainChangeable() const
{
  return false;
}

void ProbabilityDensityFromPoints::Init()
{
  sFreezeTitle = "ProbabilityDensityFromPoints";
}

bool ProbabilityDensityFromPoints::fFreezing()
{
	bool *fNoUpdate = (bool *)(IlwWinApp()->Context()->pGetThreadLocalVar(IlwisAppContext::tlvNOUPDATECATALOG));
	bool fPrevNoUpdate = *fNoUpdate;
	*fNoUpdate = true;
	FileName fnOcMaps (String("%S_images.ioc", fnObj.sFile));
	ParmList pm;
	ObjectCollection ocMaps (fnOcMaps, "ObjectCollection", pm);
	// add ocMaps to the same collections as "this"
	ocMaps->Store(); // otherwise ObjectInfo::WriteAdditionOfFileToCollection (triggered by oc->Add(fnOcMaps)) has no effect
	if (ObjectInfo::fInCollection(fnObj)) {
		int iNr;
		ObjectInfo::ReadElement("Collection", "NrOfItems", fnObj, iNr);
		for(unsigned int iC = 0; iC < (unsigned int)iNr; ++iC) {
			FileName fnCol;
			ObjectInfo::ReadElement("Collection", String("Item%d", iC).c_str(), fnObj, fnCol);
			// only for real object colections, not for maplists etc.
			if (".ioc" != fnCol.sExt)
				continue;
			ObjectCollection oc(fnCol);
			if (oc.fValid())
				oc->Add(fnOcMaps);
		}
	}
	Table tbl = pmReferencePoints->tblAtt();
	Column colDistance = tbl->col(sDistanceAttr);
	Column colSigmaDistance = tbl->col(sSigmaDistanceAttr);
	Column colDirection = tbl->col(sDirectionAttr);
	Column colSigmaDirection = tbl->col(sSigmaDirectionAttr);
	long iPoints = pmReferencePoints->iFeatures();
	try {
		for (long i = 0; i < iPoints; ++i) {
			Coord crdwgs = pmReferencePoints->cValue(i);
			long iRaw = pmReferencePoints->iRaw(i);
			double distance = colDistance->rValue(iRaw);
			double sigma_distance = colSigmaDistance->rValue(iRaw);
			double direction = colDirection->rValue(iRaw);
			double sigma_direction = colSigmaDirection->rValue(iRaw);
			String s("MapProbabilityDensity(%f,%f,%f,%f,%f,%f)", crdwgs.x, crdwgs.y, distance, sigma_distance, direction, sigma_direction);
			FileName fn (String("%S_%d.mpr", fnObj.sFile, i));
			Map mp (fn, s);
			mp->Calc();
			ptr.Add(mp);
			ocMaps->Add(mp);
			ptr.Add(mp->gr());
			if (trq.fUpdate(i, iPoints)) {
				*fNoUpdate = fPrevNoUpdate;
				return false;
			}
		}
	} catch (const ErrorObject& err) {  // catch to prevent invalid object
		err.Show();
		ptr.Add(ocMaps); // Trick to make it stop trying to compute it over and over
		*fNoUpdate = fPrevNoUpdate;
		return false;
	}
	trq.fUpdate(iPoints, iPoints);
	*fNoUpdate = fPrevNoUpdate;
	return true;
}

void ProbabilityDensityFromPoints::UnFreeze()
{
	bool *fNoUpdate = (bool *)(IlwWinApp()->Context()->pGetThreadLocalVar(IlwisAppContext::tlvNOUPDATECATALOG));
	bool fPrevNoUpdate = *fNoUpdate;
	*fNoUpdate = true;

	FileName fnOcMaps (String("%S_images.ioc", fnObj.sFile));
	ObjectCollection ocMaps (fnOcMaps);
	if (ocMaps.fValid())
		ocMaps->fErase = true;
	ObjectCollectionVirtual::UnFreeze();

	*fNoUpdate = fPrevNoUpdate;
}




