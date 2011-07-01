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
*/                                                                      
#include "Engine\Table\tbl.h"
#include "Engine\Base\DataObjects\ObjectCollection.h"
#include "Engine\Applications\ObjectCollectionVirtual.h"
#include "Engine\Applications\ModuleMap.h"
#include "Engine\Map\Polygon\POL.H"
#include "Tracking.H"
#include "Engine\Map\basemap.h"
#include "Engine\Base\DataObjects\valrange.h"
#include "Engine\Base\System\LOGGER.H"
#include "Engine\Domain\dmident.h"
#include "Engine\Base\System\Engine.h"
#include "Headers\Err\Ilwisapp.err"
#include "Headers\Htp\Ilwisapp.htp"


IlwisObjectPtr * createPolygonMapListTracking(const FileName& fn, IlwisObjectPtr& ptr, const String& sExpr, vector<void *> parms ) {
	if ( sExpr != "")
		return (IlwisObjectPtr *)PolygonMapListTracking::create(fn, (ObjectCollectionPtr &)ptr, sExpr);
	else
		return (IlwisObjectPtr *)new PolygonMapListTracking(fn, (ObjectCollectionPtr &)ptr);
}

InfoVector* getApplicationInfo() {	  

	InfoVector *infos = new InfoVector();   
	infos->push_back(CommandMap::newCommandInfo(createPolygonMapListTracking,"PolygonMapListTracking"));

	return infos;
}
const char* PolygonMapListTracking::sSyntax() {
  return "PolygonMapListTracking(inputcollection)";
}

PolygonMapListTracking* PolygonMapListTracking::create(const FileName& fn, ObjectCollectionPtr& p, const String& sExpr)
{
	Array<String> as(1);
	if (!IlwisObjectPtr::fParseParm(sExpr, as))
		ExpressionError(sExpr, sSyntax());
	String sInputMapName = as[0];
	FileName fnInput(sInputMapName);
	if ( fnInput.fExist() == false)
		throw ErrorObject(TR("Input collection doesnt exist"));
  //BaseMap basemp(FileName(ssInputMapName));
  return new PolygonMapListTracking(fn, p, ObjectCollection(fnInput));
}

PolygonMapListTracking::PolygonMapListTracking(const FileName& fn, ObjectCollectionPtr& p)
: ObjectCollectionVirtual(fn,p)
{
  fNeedFreeze = true;
  String sColName;
  try {
	  FileName fn;
    ReadElement("PolygonMapListTracking", "InputCollection", fn);
	collection = ObjectCollection(fn);
  }
  catch (const ErrorObject& err) {  // catch to prevent invalid object
    err.Show();
    return;
  }
  Init();
  objdep.Add(collection.ptr());
}

PolygonMapListTracking::PolygonMapListTracking(const FileName& fn, ObjectCollectionPtr& p, const ObjectCollection& col)
: ObjectCollectionVirtual(fn, p), collection(col)
{
  fNeedFreeze = true;
  Init();
  objdep.Add(col.ptr());
  if (!fnObj.fValid()) // 'inline' object
    objtime = objdep.tmNewest();
}


void PolygonMapListTracking::Store()
{
  ObjectCollectionVirtual::Store();
  FileName fn;
  WriteElement("ObjectCollectionVirtual", "Type", "PolygonMapListTracking");
  WriteElement("PolygonMapListTracking", "InputCollection", collection->fnObj);
}

PolygonMapListTracking::~PolygonMapListTracking()
{
}

String PolygonMapListTracking::sExpression() const
{
	return String("PolygonMapListTracking(%S)", collection->sNameQuoted(true, fnObj.sPath()));
}

bool PolygonMapListTracking::fDomainChangeable() const
{
  return false;
}

void PolygonMapListTracking::Init()
{
  sFreezeTitle = "PolygonMapListTracking";
}

bool PolygonMapListTracking::fFreezing()
{
	OverlapLayers layers;
	vector<PolygonMap> maps; 
	setup(layers, maps);
	Tranquilizer trq3, trq2;

	for(int i=1; i < maps.size(); ++i) {
		trq.fUpdate(i,maps.size());
		PolygonMap polmap = maps[i];
		layers[i].resize(layers[i-1].size());
		trq2.SetTitle(polmap->fnObj.sFile);
		int debug=0;
		for(int k=0; k < polmap->iFeatures(); ++k) {
			trq2.fUpdate(k, polmap->iFeatures());
			ILWIS::Polygon *pol1 = CPOLYGON(polmap->getFeature(k));
			String nm = pol1->sValue(polmap->dvrs());
			if ( isAcceptable(pol1)) {
				int bestIndex = -1;
				double bestOverlap = 0;
				for(int j=0; j < layers[i-1].size(); ++j) {
					ILWIS::Polygon *pol2 = layers[i-1][j].candidate;
					if ( isAcceptable(pol2)) {
						trq3.fUpdate(j,layers[i-1].size()); 
						double overlap = calcOverlap(pol2, pol1);
						if ( overlap > 0 ) {
							if ( overlap < layers[i][j].overlap)
								continue;
							if ( bestIndex == -1 || bestOverlap < overlap){
								bestIndex = j;
								bestOverlap = overlap;
							}
							
						} 
					}
				}
				if ( bestIndex == -1){
					layers[i].push_back(OverlapStruct(pol1, pol1->sValue(polmap->dvrs())));
				} else {
					layers[i][bestIndex] = OverlapStruct(pol1, pol1->sValue(polmap->dvrs()), bestOverlap);
				}
			}
		}
	}
	trq2.fUpdate(0,0);
	int sz = layers[layers.size() - 1].size();
	DomainIdentifier *dmid = new DomainIdentifier(FileName(ptr.fnObj,".dom"),sz,"object");
	Domain dm;
	dm.SetPointer(dmid);
	trq.SetTitle(TR("Creating layers"));
	for(int i = 0; i < layers.size(); ++i) {
		FileName fnLayer(String("%S_%d.mpa",ptr.fnObj.sFile, i));
		Table attribTable(FileName(fnLayer,".tbt"),dm);
		Column col(attribTable,"Area",Domain("Value"));
		PolygonMap layer(fnLayer,maps[0]->cs(),maps[0]->cb(), dm);
		layer->SetAttributeTable(attribTable);
		ptr.Add(layer);
		trq.fUpdate(i, layers.size());
		for(int j =0; j < layers[i].size(); ++j) {
			ILWIS::Polygon *pol = layers[i][j].candidate;
			if ( pol) {
				ILWIS::Polygon *newPol = CPOLYGON(layer->newFeature(pol));
				newPol->PutVal((long)j+1);
				col->PutVal(j+1,newPol->rArea());
			}
		}
		layer->Store();
	}

	//trq.fUpdate(iFeatures, iFeatures);
	return true;
}

bool PolygonMapListTracking::isAcceptable(ILWIS::Polygon *pol) {
	return !(pol == 0 || !pol->fValid() || pol->iValue() == iUNDEF);
}

double PolygonMapListTracking::calcOverlap(ILWIS::Polygon* pol1, ILWIS::Polygon* pol2) {
	double overlap = 0;
	try {
		CoordBounds cb1 = pol1->cbBounds();
		CoordBounds cb2 = pol2->cbBounds();
			double area1q = pol1->getArea();
			double area2q = pol2->getArea();
		if ( !(cb1.fContains(cb2) || cb2.fContains(cb1)))
			return 0;

		CoordinateSequence *seq1 = pol1->getCoordinates();
		CoordinateSequence *seq2 = pol2->getCoordinates();
		Geometry *geom = pol1->intersection(pol2);
		if ( geom && geom->isValid() && geom->getArea() > 0) {
			double area1 = pol1->getArea();
			double area2 = pol2->getArea();
			double areaO = geom->getArea();
			overlap = areaO / sqrt(area1 * area2);

		}
	} catch ( geos::util::TopologyException& ex) {
		return 0;
	}
	return overlap;
}

void PolygonMapListTracking::setup(OverlapLayers& layers,vector<PolygonMap>& maps) {
	vector<int> polmapIndex;
	for(int i = 0; i < collection->iNrObjects(); ++i) {
		if (IOTYPE(collection->fnObject(i)) != IlwisObject::iotPOLYGONMAP) 
			continue;
		polmapIndex.push_back(i);
	}
	for(int i=0; i < polmapIndex.size(); ++i) {
		PolygonMap polmap(collection->fnObject(polmapIndex[i]));
		maps.push_back(polmap);
	}
	layers.resize(maps.size());
	PolygonMap polmap(maps.front());
	for(int j=0; j < polmap->iFeatures(); ++j) {
		ILWIS::Polygon *pol = CPOLYGON(polmap->getFeature(j));
		if ( isAcceptable(pol)) {
			layers[0].push_back(OverlapStruct(pol));
		}
	}


}







