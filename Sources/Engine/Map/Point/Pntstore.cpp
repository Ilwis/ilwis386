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
#include "Headers\toolspch.h"
#include "Engine\Base\DataObjects\valrange.h"
#include "Engine\Map\Point\ilwPoint.h"
#include "Engine\Map\Point\PNTSTORE.H"
#include "Engine\Table\tblstore.h"
#include "Engine\Table\COLSTORE.H"
#include "Engine\Table\NewTableStore.h"
#include "Engine\Table\TableStoreIlwis3.h"
#include "Engine\Base\DataObjects\valrange.h"
#include "Engine\Table\Rec.h"
#include "Engine\Base\DataObjects\Tranq.h"
#include "Engine\Domain\dmcoord.h"
#include "Engine\Base\System\Engine.h"
#include "Engine\Base\DataObjects\ObjectStructure.h"
#include "Engine\Table\tblinfo.h"
#include <geos/index/quadtree/Quadtree.h>
#include <geos/geom/Envelope.h>
#include "Headers\Hs\point.hs"

using namespace ILWIS;

#define EPS10 1.e-10

PointMapStore::PointMapStore(const FileName& fn, PointMapPtr& p, bool fDoNotLoad)
: MultiPoint(NULL, new GeometryFactory()),
fnObj(p.fnObj), ptr(p), geomfactory(0)
{
	long sz;
	ptr.ReadElement("PointMap", "Points", sz);
	int bucketSize = max(25L, (long)(sqrt((double)sz) / 3));
	spatialIndex = new QuadTree(p.cb(), bucketSize);
	if ( fDoNotLoad ) // loading and constructing will be done elsewhere (foreignformat)
		return;

	TableStoreIlwis3 tbl;
	tbl.load(fn,"");

	ILWIS::Version::BinaryVersion fvFormatVersion;
	ptr.ReadElement("PointMapStore", "Format", (int &)fvFormatVersion);
	if ( fvFormatVersion == shUNDEF) {
		String name;
		ptr.ReadElement("Col:Coordinate","StoreType",name);
		if ( name != "")
			fvFormatVersion = ILWIS::Version::bvFORMAT30;
		else
			fvFormatVersion = ILWIS::Version::bvFORMAT20;
	}
	/*getEngine()->getVersion()->fSupportsBinaryVersion(fvFormatVersion);
    ptr.setVersionBinary(fvFormatVersion);*/
	geometries = new vector<Geometry *>();

	// determine the format of the pointmap by means of the Coordinate column
	// This Column did not exist in version 2.2 and lower
	bool fFormat20 = fvFormatVersion == ILWIS::Version::bvFORMAT20;
	long iNr = tbl.getRowCount();

	if (fFormat20)
	{
		int xCol = tbl.index("X");
		int yCol = tbl.index("Y");
		int vCol = tbl.index("Name");
		for (long i = 1; i <= iNr; ++i) 
		{
			double x,y,v;
			tbl.get(i-1, xCol, x);
			tbl.get(i-1, yCol, y);
			tbl.get(i-1, vCol, v);
			SetPoint(Coord(x,y), v, tbl.fUsesReals(vCol));
		}
	}
	else {

		Coord crd;
		int crdCol = tbl.index("Coordinate");
		int vCol = tbl.index("Name");
		Tranquilizer trq;
		trq.SetTitle("Loading point map");
		bool usereal = tbl.fUsesReals(vCol);

		geometries->resize(iNr);
		for (long i = 0; i < iNr; ++i) 
		{
			if ( i % 1000 == 0) {
				if (trq.fUpdate(i, iNr))
					return;
			}
			double v;
			tbl.get(i,crdCol,crd);
			tbl.get(i, vCol, v);
			SetPoint(crd, v, usereal,i);
	
		}
	}

}

PointMapStore::PointMapStore(const FileName& fn, PointMapPtr& p, long iPnts)
: MultiPoint(NULL, new GeometryFactory()), fnObj(p.fnObj), ptr(p)
{
	spatialIndex = new QuadTree(p.cb());
	geometries = new vector<Geometry *>();
	for(int i=0; i<iPnts; ++i) 
		pntNew();

}

Feature *PointMapStore::pntNew(geos::geom::Geometry *pnt) {
	ILWIS::Point *p;
	if ( ptr.dvrs().fRealValues()) {
		p = new ILWIS::RPoint(spatialIndex, (geos::geom::Point*)(pnt));
	} else {
		p = new ILWIS::LPoint(spatialIndex, (geos::geom::Point*)(pnt));
	}
	geometries->push_back(p);
	return p;
}

void PointMapStore::SetPoint(const Coord& crd, double v, bool usesReal,long index) {
	if ( geomfactory == 0)
		geomfactory = new GeometryFactory((PrecisionModel *)0);

	if ( usesReal) {
		ILWIS::RPoint *p = new ILWIS::RPoint(spatialIndex,crd, v, geomfactory);
		if ( index < geometries->size() || index < 0)
			geometries->at(index) = p;
		else
			geometries->push_back(p);
	} else {
		ILWIS::LPoint *p = new ILWIS::LPoint(spatialIndex,crd, v, geomfactory);
		if ( index < geometries->size() || index < 0) {
			geometries->at(index) = p;
		}
		else
			geometries->push_back(p);
	} 
}

PointMapStore::~PointMapStore()
{
	delete spatialIndex;
}

void PointMapStore::Store()
{
	FileName fnData(ptr.fnObj,".pt#");
	Table tbl;
	tbl.SetPointer(new TablePtr(ptr.fnObj, fnData, Domain("none"), Table::sSectionPrefix(".mpp")));
	tbl->fUpdateCatalog = false; // was set by constructor
	Domain dmcrd;
	dmcrd.SetPointer(new DomainCoord(ptr.cs()->fnObj));
	dmcrd->pdcrd()->set3D(ptr.use3DCoordinates());
	Column colCoord = tbl->colNew("Coordinate", dmcrd, ValueRange());
	
	DomainValueRangeStruct dvrs = ptr.dvrs();
	Column colVal;
	colVal = tbl->colNew("Name", dvrs);
	colVal->SetOwnedByTable(true);
	colVal->SetDescription("Point name/value");
	
	tbl->iRecNew(geometries->size());

	for(int i = 0 ; i < geometries->size(); ++i) {
		ILWIS::Point *point = CPOINT(geometries->at(i));
		if (!point)
			continue;
     	const Coordinate *c =  point->getCoordinate();
		Coord crd(*c);
		colCoord->PutVal(i+1,crd);
		if (ptr.dvrs().fRawAvailable() ) {
			long v = point->iValue();
			colVal->PutRaw(i + 1,v);
		} else if (ptr.dvrs().fRealValues() ){
			double v = point->rValue();
			colVal->PutVal(i + 1,v);
		}
	}
	tbl->Store();
	//  ObjectInfo::WriteElement("PointMap", "Type", fnObj, "PointMapStore");
	ptr.WriteElement("PointMapStore", "StoreTime", (long)timStore);
	ILWIS::Version::BinaryVersion fvFormatVersion = ptr.getVersionBinary();
	ptr.WriteElement("PointMapStore", "Format", (long)fvFormatVersion);
}

void PointMapStore::UnStore(const FileName& fn)
{
	FileName fnData;
	if (ObjectInfo::ReadElement("TableStore", "Data", fn, fnData))
		_unlink(fnData.sFullName(true).c_str()); // delete data file if it's still there
	// not in all cases next unlink works, so also delete the following two sections
	ObjectInfo::WriteElement("Table", (char*)0, fn, (char*)0);
	ObjectInfo::WriteElement("TableStore", (char*)0, fn, (char*)0);
	ObjectInfo::WriteElement("PointMapStore", (char*)0, fn, (char*)0);
}

void PointMapStore::Export(const FileName& fn) const
{
	File filPnt(FileName(fn, ".pnt"), facCRT);
	filPnt.SetErase(true);
	if (ptr.dvrs().fValues())
		filPnt.WriteLnAscii("X! Y! Value&");
	else
		filPnt.WriteLnAscii("X! Y! Name$");
	Coord crd;
	String s, sName;
	bool fVals = ptr.dvrs().fRealValues();
	for (long i=0; i < iPnt(); i++) {
		Geometry *pnt = geometries->at(i);
		s = String("%li %li ", longConv(pnt->getCoordinate()->x), longConv(pnt->getCoordinate()->y));
		String sName;
		sName = ((ILWIS::Point *)pnt)->sValue(ptr.dvrs()); 

		if (!fVals)
		{
			// replace spaces by underscore
			sName = sName.sLeft(20);     // maximum size of Ilwis 1.4 strings
			replace(sName.begin(), sName.end(), ' ', '_');
		}
		s &= sName;  
		filPnt.WriteLnAscii(s);
	}
	filPnt.SetErase(false);
}

void PointMapStore::SaveAsFormat20()
{
	//TODO


	//FileName fnData;
	//ptr.ReadElement("TableStore", "Data", fnData);
	//Domain dmReal("value");
	//ValueRange vr(-1e8,1e8,0.001);
	//Column colX = tbl->colNew("X", dmReal,vr);
	//colX->SetOwnedByTable(true);
	//colX->SetDescription("X-coord");
	//Column colY = tbl->colNew("Y", dmReal,vr);
	//colY->SetOwnedByTable(true);
	//colY->SetDescription("Y-coord");
	//fnData.sExt = ".pn#";
	//tbl->SetDataFile( fnData);
	//
	//for (long i = 1; i <= iPnt(); ++i) 
	//{
	//	Coord crd = colCoord->cValue(i);
	//	colX->PutVal(i, crd.x);
	//	colY->PutVal(i, crd.y);
	//}
	//tbl->RemoveCol(colCoord);
	//Updated();
}

long PointMapStore::iPnt() const
{
	return geometries->size();
}

void PointMapStore::Updated()
{
	ptr.Updated();
	timStore = ptr.objtime;
}

void PointMapStore::SetErase(bool f)
{
	fErase = f;
} 

long PointMapStore::iRaw(long iRec) const
{
	if ( iRec == iUNDEF)
		return iUNDEF;
	Geometry *pnt = geometries->at(iRec);
	long raw = ((ILWIS::Point *)pnt)->iValue();
	return raw;
}

long PointMapStore::iValue(long iRec) const
{
	if ( iRec == iUNDEF)
		return iUNDEF;

	long value = iUNDEF;
	if ( iRec < geometries->size() && geometries->at(iRec) != NULL)	{
		ILWIS::LPoint *pnt = (ILWIS::LPoint *)geometries->at(iRec - 1);
		value  = pnt->iValue();
		value = ptr.dvrs().iValue(value);
	}

	return value;
}

double PointMapStore::rValue(long iRec) const
{
	double value = rUNDEF;
	if ( iRec < geometries->size() && geometries->at(iRec) != NULL)	{
		if ( ptr.dvrs().fUseReals()) {
			value  = ((ILWIS::RPoint *)(geometries->at(iRec)))->rValue();
		} else {
			value = ((ILWIS::LPoint *)(geometries->at(iRec)))->iValue();
			value = ptr.dvrs().rValue(value);
		}
	}
	return value;
}

String PointMapStore::sValue(long iRec, short iWidth, short iDec) const
{
	if ( iRec < geometries->size() && geometries->at(iRec) != NULL)	{
		if (ptr.dvrs().fUseReals()) {
			double val = rValue(iRec);
			return ptr.dvrs().sValue(val, iWidth, iDec);
		}
		long raw = iRaw(iRec);
		return ptr.dvrs().sValueByRaw(raw, iWidth, iDec);
	}
	 return sUNDEF;
}

Coord PointMapStore::cValue(long iRec) const
{
	if ( iRec  < geometries->size() && geometries->at(iRec) != NULL)	{
		ILWIS::Point *pnt = (ILWIS::Point *)geometries->at(iRec);
		Coord c = *(pnt->getCoordinate());
		return c;
	}
	return Coord();
}

long PointMapStore::iRec(const String& s) const
{
	for (long i = 0; i < geometries->size(); ++i) {
		ILWIS::Point *pnt = (ILWIS::Point *)geometries->at(i);
		if ( fCIStrEqual(s, pnt->sValue(ptr.dvrs(),0)))
		  return i;
	}
  return iUNDEF;
}

long PointMapStore::iRec(long iR) const
{
	for (long i = 0; i < geometries->size(); ++i)
		if (iR == iRaw(i))
			return i;

	return iUNDEF;
}

long PointMapStore::iRec(const Coord& c, double rPrx) const
{
	long iRes = iUNDEF;
	double r2Res = rPrx == rUNDEF ? ptr.rProximity() : rPrx;
	r2Res *= r2Res;
	double r2;
	Coord crd;
	for (long i = 0; i < geometries->size(); ++i) {
		crd = cValue(i);
		if (crd.fUndef())
			continue;
		r2 = rDist2(c, crd);
		if (r2 <= r2Res + EPS10) {
			if (r2 == 0)
				return i;
			r2Res = r2;
			iRes = i;
		}
	}
	if ( iRes != iUNDEF)
		return iRes;
	return iUNDEF;
}

void PointMapStore::PutRaw(long iRec, long iRaw)
{
	if ( iRec >= geometries->size() )
		return;
	((ILWIS::Point *)geometries->at(iRec))->PutVal(iRaw);
	Updated();
}

void PointMapStore::PutVal(long iRec, double rVal)
{
	if ( iRec >= geometries->size() )
		return;
	if(!ptr.dvrs().fValues())
		return;
	if ( ptr.dvrs().fUseReals()) {
		((ILWIS::Point *)geometries->at(iRec))->PutVal(rVal);	
	} else {
		long iRaw = ptr.dvrs().iRaw(rVal);
		((ILWIS::Point *)geometries->at(iRec))->PutVal(iRaw);
	}
	Updated();
}

void PointMapStore::PutVal(long iRec, const String& sVal)
{
	if ( iRec >= geometries->size() )
		return;
	if(!ptr.dvrs().fValues())
		return;
	if ( ptr.dvrs().fUseReals()) {
		((ILWIS::Point *)geometries->at(iRec - 1))->PutVal(sVal.rVal());	
	} else {
		long iRaw = ptr.dvrs().iRaw(sVal);
		((ILWIS::Point *)geometries->at(iRec - 1))->PutVal(iRaw);
	}
	Updated();
}

void PointMapStore::PutVal(long iRec, const Coord& c)
{
	if ( iRec >= geometries->size() )
		return;
	ILWIS::Point *p = (ILWIS::Point *)((*geometries)[iRec]);
	p->setCoord(c);
	Updated(); 
}

void PointMapStore::Delete(long iRec)
{
	geometries->erase(geometries->begin() + iRec);
	Updated();
}

long PointMapStore::iAdd(long iRecs)
{
	for(int i=0; i < iRecs; ++i){
		pntNew();
	}

	return geometries->size();
}

long PointMapStore::iAddRaw(const Coord& c, long iRaw)
{
	ILWIS::LPoint *p = new ILWIS::LPoint(spatialIndex, c, iRaw,0);
	geometries->push_back(p);
	Updated();
	return geometries->size();
}

long PointMapStore::iAddVal(const Coord& c, const String& s)
{
	//if(!ptr.dvrs().fValues())
	//	return iUNDEF;
	ILWIS::Point *p;
	if ( ptr.dvrs().fUseReals()) {
		p = new ILWIS::RPoint(spatialIndex,c, s.rVal(),0);
	} else {
		long iRaw = ptr.dvrs().iRaw(s);
		p = new ILWIS::LPoint(spatialIndex,c, iRaw,0);
	}
	geometries->push_back(p);
	Updated();
	return geometries->size();
}

long PointMapStore::iAddVal(const Coord& c, double rValue)
{
	if(!ptr.dvrs().fValues())
		return iUNDEF;
	ILWIS::Point *p;
	if ( ptr.dvrs().fUseReals()) {
		p = new ILWIS::RPoint(spatialIndex,c, rValue,0);
	} else {
		long iRaw = ptr.dvrs().iRaw(rValue);
		p = new ILWIS::LPoint(spatialIndex,c, iRaw,0);
	}
	geometries->push_back(p);
	return geometries->size();
}

void PointMapStore::PutBufRaw(const CoordBuf& cb, const LongBuf& lb)
{
	for(int i = 0; i < cb.iSize(); ++i) {
		iAddRaw(cb[i], lb[i]);
	}
}

void PointMapStore::GetDataFiles(Array<FileName>& afnDat, Array<String>* asSection, Array<String>* asEntry) const
{
	FileName fnDat(fnObj, ".pt#", true);
	ObjectInfo::Add(afnDat, fnDat, fnObj.sPath());
	if (asSection != 0) {
		(*asSection) &= "TableStore";
		(*asEntry) &= "Data";
	}
}

void PointMapStore::KeepOpen(bool f, bool force)
{
	// points are stored in table in memory
}

bool PointMapStore::fConvertTo(const DomainValueRangeStruct& _dvrsTo, const Column& col)
{
	DomainValueRangeStruct dvrsTo = _dvrsTo;
	if (col.fValid())
		dvrsTo = col->dvrs();
	Tranquilizer trq;
	trq.SetTitle(TR("Domain Conversion"));
	trq.SetText(TR("Converting"));
	//  ColumnStore* pcsNewVal = colNewVal->pcs();
	if (ptr.dvrs().fValues()) {
		trq.Start();
		if (dvrsTo.fUseReals()) {
			double r = rUNDEF;
			for (long i=0; i < iPnt(); i++) {
				if (trq.fUpdate(i, iPnt()))
					return false;
				if (col.fValid())
					r = col->rValue(iValue(i));
				else
					r = rValue(i);
				PutVal(i,r);
			}   
			trq.fUpdate(iPnt(), iPnt());
		}
		else if (dvrsTo.fValues()) {
			long iRaw = iUNDEF;
			double r;
			for (long i=1; i <= iPnt(); i++) {
				if (trq.fUpdate(i, iPnt()))
					return false;
				if (col.fValid())
					iRaw = col->iRaw(iValue(i));
				else {
					r = rValue(i);
					iRaw = dvrsTo.iRaw(r);
				}
				PutRaw(i,iRaw);
			}   
			trq.fUpdate(iPnt(), iPnt());
		}
	}
	else if (col.fValid()) {
		trq.Start();
		// read long raws and use them as record nr. in column col
		String s;
		for (long i=1; i <= iPnt(); i++) {
			if (trq.fUpdate(i, iPnt()))
				return false;
			s = col->sValue(iRaw(i));
			PutVal(i, s);
		}   
		trq.fUpdate(iPnt(), iPnt());
	}
	else  {
		trq.Start();
		String s;
		for (long i=0; i < iPnt(); i++) {
			if (trq.fUpdate(i, iPnt()))
				return false;
			s = sValue(i);
			PutVal(i, s);
		}   
		trq.fUpdate(iPnt(), iPnt());
	}
	ObjectInfo::WriteElement("Col:Name", (char*)0, fnObj, (char*)0);
	return true;
}

void PointMapStore::Flush()
{
//	tbl->Store();
}
								 
void PointMapStore::DoNotUpdate()
{
}

void PointMapStore::SetCoordSystem(const CoordSystem& cs)
{
}

Geometry * PointMapStore::getFeature(long i) const {
	if ( geometries && i < geometries->size())
		return geometries->at(i);
	return NULL;
}

vector<Geometry *> PointMapStore::getFeatures(Coord crd, double rPrx) {
	vector<Geometry *> points;
	if ( crd.fUndef())
		return points;

	long iRes = iUNDEF;
	double r2Res = rPrx == rUNDEF ? ptr.rProximity() : rPrx;
	r2Res *= r2Res;
	double r2;
	for (long i = 0; i < geometries->size(); ++i) {
		Geometry *g = geometries->at(i);
		Coord c =  *(g->getCoordinate());
		if (c.fUndef())
			continue;
		r2 = rDist2(c, crd);
		if (r2 <= r2Res + EPS10)
			points.push_back(g);
	}
	return points;

}

bool PointMapStore::removeFeature(FeatureID id, const vector<int>& selectedCoords) {
	for(vector<Geometry *>::iterator cur = geometries->begin(); cur != geometries->end(); ++cur) {
		ILWIS::Point *pnt = CPOINT(*cur);
		if ( pnt->getGuid() == id) {
			spatialIndex->remove(pnt);
			delete pnt;
			geometries->erase(cur);
			return true;
		}
	}
	return false;
}

Geometry *PointMapStore::getFeatureById(FeatureID id) const {
	return BaseMapPtr::getFeatureById(geometries, id);
}

vector<Feature *> PointMapStore::getFeatures(const CoordBounds& cb, bool complete) const {
	vector<Geometry *> v;
	vector<Feature *> features;
	spatialIndex->query(cb,v);
	for(int i=0; i < v.size(); ++i) {
		Geometry *g = (Geometry *)v[i];
		CoordinateSequence *seq = g->getCoordinates();
		bool isIn = true;
		for( int  j =0; j < seq->size() && isIn; ++j) {
			if ( !cb.fContains(seq->getAt(j))) {
				isIn = false;
			}
		}
		delete seq;
		if ( isIn) {
			features.push_back(CFEATURE(g));
		}

	}

	return features;

}