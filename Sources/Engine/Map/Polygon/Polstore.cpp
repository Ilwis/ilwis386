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
// $Log: /ILWIS 3.0/PolygonMap/Polstore.cpp $
 * 
 * 29    23-05-05 17:28 Willem
 * tblPolygon.fErase is now correctly uses the function parameter
 * 
 * 28    1-06-04 15:03 Koolhoven
 * Both polFirst() and polLast() now when colDeleted is not valid they
 * still perform the expected behaviour
 * 
 * 27    22-04-04 10:20 Willem
 * [Bug=6471]
 * The polFirst and polLast functions now add protection when asking for
 * the deleted column. The "deleted" column is not always there.This
 * occurs in case of cyclic dependency between an .mpr and .mpa for
 * instance
 * 
 * 26    11/12/01 2:19p Martin
 * The section that is removed from the ODF when saving an old 2.0 map to
 * 3.0 was wrongly identified. It remained thus
 * 
 * 25    8/23/01 17:07 Willem
 * Removed the SetReadOnly() function. This is now handled by
 * IlwisObjectPtr::SetReadOnly() for all ilwis objects
 * 
 * 24    19-03-01 8:33a Martin
 * SetErase function was not implmented properly. The tblPolygon was not
 * deleted
 * 
 * 23    3/16/01 13:30 Retsios
 * Make columns table-owned for showastbl
 * 
 * 22    13-03-01 2:24p Martin
 * loading of csy for the internal columns of a seg/pol map will not
 * generate an error
 * 
 * 21    1/03/01 14:31 Willem
 * PolygonMap and PolygonMapStore now set DomainValueRangeStruct or
 * ValueRange of the proper TablePolygon Column as well to have the domain
 * of the polygonmap and that of the TablePolygon Column synchronized..
 * 
 * 20    13-02-01 8:28a Martin
 * when changing the coordsystem of a map the coordsystem of the internal
 * tables (columns) is also changed
 * 
 * 19    20-12-00 8:39a Martin
 * destructor protected against the non-existence of a VoronoiPoint map
 * (unloaded object)
 * 
 * 18    27-11-00 9:56a Martin
 * odd typo. The fErase flag was set to true when the file was read-only.
 * Probably a msitake when selecting the correct function from the
 * dropdownlist
 * 
 * 17    17/11/00 16:59 Willem
 * Polstore now set the readonly flag of all files belonging to the
 * polygonmap
 * 
 * 16    24-10-00 12:27p Martin
 * added and changed the getobjectstructure function
 * 
 * 15    9/18/00 9:40a Martin
 * if newwly created the loadstate of the internal table(s) is set to true
 * 
 * 14    12-09-00 2:30p Martin
 * automatic conversion to 3.0 only if data has changed. Old data files
 * are removed
 * 
 * 13    11-09-00 10:53a Martin
 * added function for getobjectstructure and DoNOtUpdate
 * 
 * 12    9-03-00 8:40a Martin
 * tblptr to tbl use, better use of the tbltop in LayerInfo
 * 
 * 11    3-03-00 4:32p Martin
 * changes for Foreign format polygon
 * 
 * 10    1-03-00 12:43p Martin
 * handling of polygon maps as foreign format (phase 1)
 * 
 * 9     25-02-00 10:46 Koolhoven
 * Solved ambiguity in TablePtr constructor
 * 
 * 8     8-02-00 18:01 Wind
 * set fUpdateCatalog flag to false after creation of internal table
 * 
 * 7     7-02-00 17:04 Wind
 * solved problems with read only maps and there conversion to format 3.0
 * 
 * 6     17-01-00 8:17a Martin
 * changed rowcols to coords
 * 
 * 5     13-12-99 12:38 Wind
 * changed aiPol and aiTop in fCompatc() to zero based
 * 
 * 4     29-10-99 9:19 Wind
 * thread save stuff
 * 
 * 3     9/24/99 10:37a Wind
 * replaced calls to static funcs ObjectInfo::ReadElement and WriteElement
 * by calls to member functions
 * 
 * 2     9/08/99 11:57a Wind
 * comments
*/
// Revision 1.7  1998/09/16 17:25:54  Wim
// 22beta2
//
// Revision 1.6  1997/09/26 16:39:40  Wim
// Safety Flush() added in destructor
//
// Revision 1.5  1997-09-26 15:47:33+02  Wim
// In destructor make sure sm->fErase has same value as ptr.fErase
// It could happen that (after UndoAll()) that sm->fErase was eroneously true.
//
// Revision 1.4  1997-08-20 22:01:58+02  Wim
// Corrected names for inclusions when exporting to 1.4
//
// Revision 1.3  1997-08-08 00:06:17+02  Willem
// Export to 1.4 now truncatesthe pol code to 15 characters
//
// Revision 1.2  1997/08/06 20:38:10  Willem
// Export to Ilwis 1.4 replaces spaces in domain items with underscores.
//
/* PolygonMapStore
   Copyright Ilwis System Development ITC
   april 1995, by Wim Koolhoven
	Last change:  WK    7 Sep 98   10:34 am
*/
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
#include "Headers\geos\algorithm\locate\SimplePointInAreaLocator.h"
#include "Headers\Hs\polygon.hs"

PolygonMapStore::PolygonMapStore(PolygonMapPtr& p, const FileName& fn) :
	MultiPolygon(  new vector<Geometry *>(), new GeometryFactory()),
	ptr(p),
	fnObj(fn),
	iStatusFlags(0)
{
	ptr.ReadElement("PolygonMapStore", "Polygons", ptr._iPol);
	ptr.ReadElement("PolygonMapStore", "DeletedPolygons", ptr._iPolDeleted);
}

PolygonMapStore::PolygonMapStore(const FileName& fn, PolygonMapPtr& p, bool fCreate)
: MultiPolygon(  new vector<Geometry *>(), new GeometryFactory()), fnObj(p.fnObj), ptr(p), iStatusFlags(0)
{
	//ILWIS::Version::BinaryVersion fvBinaryVersion;

	//ptr.ReadElement("PolygonMapStore", "Format", (int &)fvBinaryVersion);
	//if ( fvBinaryVersion == shUNDEF && fCreate) fvBinaryVersion = ILWIS::Version::bvPOLYGONFORMAT37;
	//if ( fvBinaryVersion == shUNDEF && !fCreate) fvBinaryVersion = ILWIS::Version::bvFORMAT30;
	//getEngine()->getVersion()->fSupportsBinaryVersion(fvBinaryVersion);
	//ptr.setVersionBinary(fvBinaryVersion);
	//if ( fCreate && fvBinaryVersion == ILWIS::Version::bvPOLYGONFORMAT37 )
	//{
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
	for(int i = 0; i < geometries->size(); ++i) {
		ILWIS::Polygon *pol = (ILWIS::Polygon *)geometries->at(i);
		if ( pol->fValid()) {
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
		}
	}

	FileName	fnPolygon;
	ptr.WriteElement("PolygonMapStore","Format",ptr.getVersionBinary());
	ptr.WriteElement("PolygonMapStore", "DataPol", fnData);
	/*if ( ptr.ReadElement("PolygonMapStore", "DataPol", fnPolygon))
	{
		if ( fnPolygon.fExist() )
			DeleteFile(fnPolygon.sFullPath().scVal());
		ptr.WriteElement("PolygonMapStore", "DataPol", (char*)0);			
	}*/		
	ptr.WriteElement("PolygonMap", "Type", "PolygonMapStore");

	ptr.WriteElement("PolygonMapStore", "Polygons", iPol());
	if (iPolDeleted() != iUNDEF)
		ptr.WriteElement("PolygonMapStore", "DeletedPolygons", iPolDeleted());
}

void PolygonMapStore::UnStore(const FileName& fn)
{
  FileName fnData;
  if (ObjectInfo::ReadElement("VoronoiPointMapStore", "DataSeg", fn, fnData))
    _unlink(fnData.sFullName(true).scVal()); // delete data file if it's still there
  if (ObjectInfo::ReadElement("VoronoiPointMapStore", "DataCrd", fn, fnData))
    _unlink(fnData.sFullName(true).scVal()); // delete data file if it's still there
  if (ObjectInfo::ReadElement("VoronoiPointMapStore", "DataSegCode", fn, fnData))
    _unlink(fnData.sFullName(true).scVal()); // delete data file if it's still there
  if (ObjectInfo::ReadElement("PolygonMapStore", "DataPol", fn, fnData))
    _unlink(fnData.sFullName(true).scVal()); // delete data file if it's still there
  if (ObjectInfo::ReadElement("PolygonMapStore", "DataTop", fn, fnData))
    _unlink(fnData.sFullName(true).scVal()); // delete data file if it's still there
  if (ObjectInfo::ReadElement("PolygonMapStore", "DataPolCode", fn, fnData))
    _unlink(fnData.sFullName(true).scVal()); // delete data file if it's still there
  ObjectInfo::WriteElement("PolygonMapStore", (char*)0, fn, (char*)0);
}  

PolygonMapStore::~PolygonMapStore()
{
}

void PolygonMapStore::Updated()
{
	ptr.Updated();
	timStore = ptr.objtime;
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
  Tranquilizer trq(SPOLTitleExport14Polygons);
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
  trq.SetText(SPOLTextStoringPolygonNames);
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
		pol  = new ILWIS::RPolygon(dynamic_cast<geos::geom::Polygon *>(p));
	} else {
		pol =  new ILWIS::LPolygon(dynamic_cast<geos::geom::Polygon *>(p));
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
		pol  = new ILWIS::RPolygon();
	} else {
		pol =  new ILWIS::LPolygon();
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
		throw ErrorObject(SPOLErrPolDomainError);
	ptr._iPol = geometries->size();
}

//bool PolygonMapStore::fVersionCheck(IlwisObjectPtr::BinaryVersion vers) const {
//	return ( vers == IlwisObjectPtr::fvFORMAT30 ||
//		vers == IlwisObjectPtr::fvFORMATFOREIGN ||
//		vers == IlwisObjectPtr::fvFORMAT37);
//}







