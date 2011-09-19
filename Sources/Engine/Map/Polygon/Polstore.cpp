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

#define POLSTORE_C
#include "Engine\Map\Polygon\POLSTORE.H"
#include "Engine\Map\Polygon\POL14.H"
#include "Engine\Map\Feature.h"
#include "Engine\Map\Segment\SEGSTORE.H"
#include "Engine\Table\Col.h"
#include "Engine\Domain\dmsort.h"
#include "Engine\Domain\dmcoord.h"
#include "Engine\Table\ColumnCoordBuf.h"
#include "Engine\Applications\objvirt.h"
#include "Engine\Applications\ModuleMap.h"
#include "Engine\Base\System\Engine.h"
#include "geos\algorithm\locate\SimplePointInAreaLocator.h"
#include <geos/index/quadtree/Quadtree.h>
#include <geos/geom/Envelope.h>
#include "Headers\Hs\polygon.hs"

PolygonMapStore::PolygonMapStore(PolygonMapPtr& p, const FileName& fn) :
	MultiPolygon(  new vector<Geometry *>(), new GeometryFactory()),
	ptr(p),
	fnObj(fn),
	iStatusFlags(0)
{
	ptr.ReadElement("PolygonMapStore", "Polygons", ptr._iPol);
    spatialIndex = new geos::index::quadtree::Quadtree();
}

PolygonMapStore::PolygonMapStore(const FileName& fn, PolygonMapPtr& p, bool fCreate)
: MultiPolygon(  new vector<Geometry *>(), new GeometryFactory()), fnObj(p.fnObj), ptr(p), iStatusFlags(0)
{
	spatialIndex = new geos::index::quadtree::Quadtree();
	ptr._iPol = 0;
	ptr._iPolDeleted = 0;
	//}

	
	
}

void PolygonMapStore::CalcBounds() {
   	ptr._iPol = geometries->size();
	ptr._iPolDeleted = 0;
	if (ptr.cbOuter.fUndef()) 
	{
		Coord cMin,cMax;
		Geometry *g = getEnvelope();
		if ( g->isEmpty() == false) {
			CoordinateSequence *seq = g->getCoordinates();
			if (dynamic_cast<geos::geom::Polygon *>(g)) {
				cMin = Coord(seq->getAt(0));
				cMax = Coord(seq->getAt(2));
			} else if( dynamic_cast<geos::geom::Point *>(g)) {
			  cMax = cMin = Coord(seq->getAt(0));
			}else if( dynamic_cast<geos::geom::LineString *>(g)) {
				cMin = Coord(seq->getAt(0));
				cMax = Coord(seq->getAt(g->getNumPoints() - 1));
			}
			delete seq;
			ptr.cbOuter = CoordBounds(cMin,cMax);
			ptr.fChanged = true;
		}
	}
}

ILWIS::Polygon *PolygonMapStore::operator [](long i) {
	if ( i < geometries->size())
		return (ILWIS::Polygon *)geometries->at(i);
	return NULL;
}

void PolygonMapStore::StoreRing(File& binaryFile, const LineString *ls) {
	CoordinateSequence *seq = ls->getCoordinates();
	const vector<Coordinate> *v = seq->toVector();
	long sz = v->size();
	const void *p = &sz;
	byte *vc = (byte *)&(*v)[0];
	binaryFile.Write(4L, p);
	binaryFile.Write(v->size() * 3 * 8, &(*v)[0]);
	delete seq;
}

void PolygonMapStore::Store()
{
	FileName fnData(ptr.fnObj,".mpz#");
	File  binaryFile(fnData,facCRT);
	long correct = 0;
	for(int i = 0; i < geometries->size(); ++i) {
		ILWIS::Polygon *pol = (ILWIS::Polygon *)geometries->at(i);
		if ( pol->fValid() && !pol->fDeleted()) {
			const LineString *ring = pol->getExteriorRing();
			StoreRing(binaryFile, ring);
			double value = pol->rValue();
			binaryFile.Write(8,&value);
			long numberOfRings = pol->getNumInteriorRing();
			binaryFile.Write(4, &numberOfRings); 
			for(int j = 0; j < pol->getNumInteriorRing();++j) {
				ring = pol->getInteriorRingN(j);
				StoreRing(binaryFile,ring);
			}
		} else
			++correct;
	}

	FileName	fnPolygon;
	ptr.WriteElement("PolygonMapStore","Format",ptr.getVersionBinary());
	ptr.WriteElement("PolygonMapStore", "DataPol", fnData);
	ptr.WriteElement("PolygonMap", "Type", "PolygonMapStore");

	ptr.WriteElement("PolygonMapStore", "Polygons", iPol() - correct);
}

void PolygonMapStore::UnStore(const FileName& fn)
{
  FileName fnData;
  if (ObjectInfo::ReadElement("VoronoiPointMapStore", "DataSeg", fn, fnData))
    _unlink(fnData.sFullName(true).c_str()); // delete data file if it's still there
  if (ObjectInfo::ReadElement("VoronoiPointMapStore", "DataCrd", fn, fnData))
    _unlink(fnData.sFullName(true).c_str()); // delete data file if it's still there
  if (ObjectInfo::ReadElement("VoronoiPointMapStore", "DataSegCode", fn, fnData))
    _unlink(fnData.sFullName(true).c_str()); // delete data file if it's still there
  if (ObjectInfo::ReadElement("PolygonMapStore", "DataPol", fn, fnData))
    _unlink(fnData.sFullName(true).c_str()); // delete data file if it's still there
  if (ObjectInfo::ReadElement("PolygonMapStore", "DataTop", fn, fnData))
    _unlink(fnData.sFullName(true).c_str()); // delete data file if it's still there
  if (ObjectInfo::ReadElement("PolygonMapStore", "DataPolCode", fn, fnData))
    _unlink(fnData.sFullName(true).c_str()); // delete data file if it's still there
  ObjectInfo::WriteElement("PolygonMapStore", (char*)0, fn, (char*)0);
}  

PolygonMapStore::~PolygonMapStore()
{
	delete spatialIndex;
}

void PolygonMapStore::Updated()
{
	ptr.Updated();
	timStore = ptr.objtime;
}

Geometry *PolygonMapStore::getFeatureById(const String& id) const {
	return BaseMapPtr::getFeatureById(geometries, id);
}

void PolygonMapStore::Export(const FileName& fn) const
{
 /* File filPlg(FileName(fn, ".PLG"), facCRT);
  filPlg.SetErase(true);
  long iStatus;
  ptr.ReadElement("VoronoiPointMapStore", "Status", iStatus);
  filPlg.WriteLnAscii(String("%li", iStatus));  // status
  filPlg.WriteLnAscii(fn.sFile); // VoronoiPoint map
  filPlg.WriteLnAscii("*");  // mask
  File filSlg(FileName(fn, ".SLG"), facCRT);
  filSlg.SetErase(true);
  filSlg.WriteLnAscii("1");  // version
  filSlg.WriteLnAscii("1");  // nr of pol maps
  filSlg.WriteLnAscii("*");  // mask
  filSlg.WriteLnAscii("===Pol===");
  filSlg.WriteLnAscii(fn.sFile);  // polygon map name
  filSlg.WriteLnAscii("*");  // mask

//File::Copy(FileName(fnObj, ".PS#"), FileName(fn, ".SEG"));
//File::Copy(FileName(fnObj, ".PD#"), FileName(fn, ".CRD"));
  Tranquilizer trq(TR("Export polygon map to ILWIS 1.4"));
  trq.Start();
  if (!File::fCopy(FileName(sm->pms->filSeg->sName()), FileName(fn, ".SEG"), trq))
    return;
  if (!File::fCopy(FileName(sm->pms->filCrd->sName()), FileName(fn, ".CRD"), trq))
    return;
  if (!File::fCopy(FileName(filPol->sName()), FileName(fn, ".POL"), trq))
    return;
  if (!File::fCopy(FileName(filTop->sName()), FileName(fn, ".TOP"), trq))
    return;
  File filSeg(FileName(fn, ".SEG", true), facRW);
  trq.SetText(SPOLTextSetVoronoiPointCodes);
  segtype st;
  lstring15 code(String("a00"));
  for (long i=0; i <= sm->iSeg(); ++i) {
    if (trq.fUpdate(i, sm->iSeg()))
      return;
    filSeg.Seek(i*sizeof(segtype));
    filSeg.Read(sizeof(segtype), &st);
    st.code = code;
    filSeg.Seek(i*sizeof(segtype));
    filSeg.Write(sizeof(segtype), &st);
  }
  DomainSort *pdsrt = ptr.dm()->pdsrt();
  bool fCodes;
  if (0 != pdsrt)
    fCodes = pdsrt->fCodesAvailable();
  String s;
  File filPol(FileName(fn, ".POL", true), facRW);
  filPol.Seek(sizeof(poltype));
  poltype pt;
  trq.SetText(TR("Storing polygon names"));
  for (i=0; i < iPol(); ++i) {
    if (trq.fUpdate(i, iPol()))
      return;
    filPol.Seek(i*sizeof(poltype));
    filPol.Read(sizeof(poltype), &pt);
    if (pt.rArea < 0)
      s = "000000000000000";
    else if (0 == pdsrt)
      s = pol(i+1).sValue();
    else {
      long iRaw = pol(i+1).iRaw();
      if (fCodes)
        s = pdsrt->sCodeByRaw(iRaw);
      else
        s = pdsrt->sNameByRaw(iRaw);
    }
    s = s.sLeft(15);      // maximum length in Ilwis 1.4
    s = s.sTrimSpaces();
    for (short ii = 0; ii < s.length(); ii++)
      if (s[ii] == ' ')
        s[ii] = '_';
    pt.sName = s;
    filPol.Seek(i*sizeof(poltype));
    filPol.Write(sizeof(poltype), &pt);
  }
  filPlg.SetErase(false);
  filSlg.SetErase(false);*/
}

void PolygonMapStore::SetErase(bool f)
{
} 

ILWIS::Polygon *PolygonMapStore::polFirst() const
{
	for(int i=0; i < geometries->size(); ++i) {
		ILWIS::Polygon *pol = (ILWIS::Polygon *)geometries->at(i);
		if ( pol->fValid())
			return pol;
	}

	return NULL;
}

ILWIS::Polygon *PolygonMapStore::pol(long index) const{
	if ( index >= geometries->size())
		return NULL;
	return (ILWIS::Polygon *)geometries->at(index);
}

ILWIS::Polygon *PolygonMapStore::polLast() const
{
	for(int i=geometries->size() -1 ; i >= 0; --i) {
		ILWIS::Polygon *pol = (ILWIS::Polygon *)geometries->at(i);
		if ( pol->fValid())
			return pol;
	}

	return NULL;	
}

vector<ILWIS::Polygon *> PolygonMapStore::pol(const Coord& crd) const // Point in polygon
{
	vector<ILWIS::Polygon *> polygons;
	for(int i = 0; i < geometries->size(); ++i) {
		ILWIS::Polygon *pol = (ILWIS::Polygon *)geometries->at(i);
		if ( pol->fValid()) {
			if ( geos::algorithm::locate::SimplePointInAreaLocator::containsPointInPolygon(crd, pol))
				polygons.push_back(pol);

		}
	}
	return polygons;
}

//void PolygonMapStore::AddLastPol(long id)
//{
//  ILWISSingleLock sl(&ptr.csAccess, TRUE, SOURCE_LOCATION);
//  if (id > 0) {
//    int iNr;
//    for (iNr = 0; iNr < 4; ++iNr)
//      if (iLastPol[iNr] == id)
//        break;
//    for (; iNr > 0; --iNr)
//      iLastPol[iNr] = iLastPol[iNr-1];
//    iLastPol[0] = id;
//  }
//}
//
//void PolygonMapStore::RemoveLastPol(long id)
//{
//  ILWISSingleLock sl(&ptr.csAccess, TRUE, SOURCE_LOCATION);
//  if (id > 0) {
//    int iNr;
//    for (iNr = 0; iNr < 4; ++iNr)
//      if (iLastPol[iNr] == id)
//        break;
//    for (; iNr < 4; ++iNr)
//      iLastPol[iNr] = iLastPol[iNr+1];
//    iLastPol[4] = iUNDEF;
//  }
//}

bool fContainsComplete(const CoordBounds& cbOuter, const CoordBounds& cbInner)
{
  if (cbOuter.MinY() > cbInner.MinY()) return 0;
  if (cbOuter.MaxY() < cbInner.MaxY()) return 0;
  if (cbOuter.MinX() > cbInner.MinX()) return 0;
  if (cbOuter.MaxX() < cbInner.MaxX()) return 0;
  return 1;
}

void PolygonMapStore::GetDataFiles(Array<FileName>& afnDat, Array<String>* asSection, Array<String>* asEntry) const
{
    FileName fnDat;
    ptr.ReadElement("PolygonMapStore", "DataPol", fnDat);
    ObjectInfo::Add(afnDat, fnDat, fnObj.sPath());
	FileName fnTriangle(ptr.fnObj,".tria#");
	if ( fnTriangle.fExist()) {
		ObjectInfo::Add(afnDat, fnTriangle, fnObj.sPath());
	}
}

void PolygonMapStore::KeepOpen(bool f)
{
}

Feature* PolygonMapStore::newFeature(geos::geom::Geometry *p)        // create a new pol
{
	ILWISSingleLock sl(&ptr.csAccess, TRUE, SOURCE_LOCATION);
	ILWIS::Polygon *pol = NULL;
	if ( ptr.dvrs().fUseReals()) {
		pol  = new ILWIS::RPolygon(spatialIndex,dynamic_cast<geos::geom::Polygon *>(p));
	} else {
		pol =  new ILWIS::LPolygon(spatialIndex,dynamic_cast<geos::geom::Polygon *>(p));
	}
	geometries->push_back(pol);
	ptr._iPol = geometries->size();
	Updated();
  	
	return pol;
}

Geometry *PolygonMapStore::getTransformedFeature(long iRec, const CoordSystem& csy) const
{
  ILWISSingleLock sl(&ptr.csAccess, TRUE, SOURCE_LOCATION);
  if (abs(iRec) >= geometries->size()) 
	  return NULL;
   ILWIS::Polygon *pol;
   if ( ptr.dvrs().fUseReals()) {
		pol  = new ILWIS::RPolygon(spatialIndex);
	} else {
		pol =  new ILWIS::LPolygon(spatialIndex);
	}
   ILWIS::Polygon *p = (ILWIS::Polygon *) geometries->at(iRec);

   CoordinateSequence *seq = p->getExteriorRing()->getCoordinates();
   CoordinateArraySequence *aseq = new CoordinateArraySequence();
   for(int i =0; i < seq->size(); ++i) {
	   Coord cold = seq->getAt(i);
	   Coord cnew = ptr.cs()->cConv(csy, cold);
	   aseq->add(cnew);
   }
   pol->addBoundary(new LinearRing(aseq,new GeometryFactory()));
   for(int j = 0; j < p->getNumInteriorRing(); ++j) {
	   seq = p->getInteriorRingN(j)->getCoordinates();
	   aseq = new CoordinateArraySequence();
	   for(int i =0; i < seq->size(); ++i) {
		   Coord cold = seq->getAt(i);
		   Coord cnew = ptr.cs()->cConv(csy, cold);
		   aseq->add(cnew);
	   }
	   delete seq;
	   pol->addHole(new LinearRing(aseq,new GeometryFactory()));
   }
   delete seq;
   pol->PutVal(p->rValue());
   return pol;
}

void PolygonMapStore::addPolygon(ILWIS::Polygon *pol) {
	bool fVals = ptr.dvrs().fUseReals();
	ILWIS::RPolygon *rpol = dynamic_cast<ILWIS::RPolygon *>(pol);
	if ( fVals && rpol)
		geometries->push_back(pol);
	else if (!fVals && !rpol )
		geometries->push_back(pol);
	else
		throw ErrorObject(TR("Trying Polgyon of wrong data type to map"));
	ptr._iPol = geometries->size();
}

bool PolygonMapStore::removeFeature(const String& id, const vector<int>& selectedCoords) {
	for(vector<Geometry *>::iterator cur = geometries->begin(); cur != geometries->end(); ++cur) {
		ILWIS::Polygon *pol = CPOLYGON(*cur);
		if ( pol->getGuid() == id  ) {
			if ( selectedCoords.size() == 0 || selectedCoords.size() == geometries->size()) {
				delete pol;
				geometries->erase(cur);
				return true;
			} else {
				CoordBuf crdBuf;
				CoordinateSequence *seq = pol->getCoordinates();
				vector<bool> status(seq->size(), true);
				for(int i = 0 ; i < selectedCoords.size(); ++i) {
					status[selectedCoords.at(i)] = false;

				}
				int reducedSize = seq->size() - selectedCoords.size();
				crdBuf.Size(reducedSize);
				int count = 0;
				for(int j = 0; j < seq->size(); ++j) {
					if ( !status[j] )
						continue;
					crdBuf[count++] = seq->getAt(j);
				}
				//pol->PutCoords(count, crdBuf);
			}
		} 
	}
	return false;
}







