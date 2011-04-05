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

*/

#include "Engine\Map\Segment\SEGSTORE.H"
#include "Engine\Map\Segment\SEG14.H"
#include "Engine\Table\Col.h"
#include "Engine\Domain\dmcoord.h"
#include "Engine\Domain\dmcoordbuf.h"
#include "Engine\Domain\dmsort.h"
#include "Engine\Table\ColumnCoordBuf.h"
#include "Engine\Base\DataObjects\ObjectStructure.h"
#include "Headers\Err\ILWISDAT.ERR"
#include "Headers\Hs\segment.hs"
#include "Headers\Hs\CONV.hs"
#include "Engine\Applications\objvirt.h"
#include "Engine\Applications\ModuleMap.h"
#include "Engine\Base\System\Engine.h"
#include "Engine\Table\NewTableStore.h"
#include "Engine\Table\TableStoreIlwis3.h"
#include "geos\operation\distance\DistanceOp.h"
#include "Engine\Table\Rec.h"
#include "Engine\Map\Point\ilwPoint.h"

using namespace ILWIS;

static void TooManySegments()
{
  String sWhat(SSEGErrTooManySegments_i.scVal(), MAX_SEGMENTS);
  throw ErrorObject(WhatError(sWhat, errMAXSEG));
}  

SegmentMapStore::SegmentMapStore(const FileName& fn, SegmentMapPtr& p, bool fCreate, bool fForeign)
: MultiLineString(new vector<Geometry *>(), new GeometryFactory()), ptr(p),fErase(false)
{
  ILWISSingleLock sl(&ptr.csAccess, TRUE, SOURCE_LOCATION);
	String sDummy;

  ILWIS::Version::BinaryVersion fvFormatVersion;
  ptr.ReadElement("SegmentMapStore", "Format", (int &)fvFormatVersion);
  if (fCreate ) 
	{
		if ( fForeign)
			fvFormatVersion = ILWIS::Version::bvFORMATFOREIGN;
		else
		{
			String sFn = ptr.fnObj.sFullPath(true);
			if (!File::fExist(sFn) || (_access(sFn.sVal(), 2) != -1)) // not readonly
				fvFormatVersion = ILWIS::Version::bvFORMAT30;
		}
	}
	getEngine()->getVersion()->fSupportsBinaryVersion(fvFormatVersion);
    ptr.setVersionBinary(fvFormatVersion);

	if (fCreate || fvFormatVersion == ILWIS::Version::bvFORMAT30 )
	{
		ptr.ReadElement("SegmentMapStore", "Status", iStatus);
		ptr.ReadElement("SegmentMapStore", "Segments", ptr._iSeg);
		ptr.ReadElement("SegmentMapStore", "DeletedSegments", ptr._iSegDeleted);
		ptr.ReadElement("SegmentMapStore", "Coordinates", ptr._iCrd);
		ptr._iSeg = max(ptr._iSeg, 0);
		ptr._iSegDeleted = max(ptr._iSegDeleted, 0);
		ptr._iCrd = max( ptr._iCrd, 0);
		bool fPolygonMap = (fCIStrEqual(ptr.fnObj.sExt, ".mpa"));
		String sSection = fPolygonMap && !fForeign ? "top:" : "";
		if ( fCreate )
		{
	/*		if ( ! fForeign )
			{
				String sExt = fPolygonMap ? ".mpat#" : ".mps#";
				Table tblSegment;
				tblSegment.SetPointer(new TablePtr(fn, FileName(fn.sFullPath(), sExt), Domain("none"), sSection));
				tblSegment->Load();
				for(int i = 0; i < tblSegment->iRecs(); ++i) {
					Column colCoord = tbl->
				}
			}*/
		}
		else
		{
			bool *fDoNotShowError = (bool *)(getEngine()->pGetThreadLocalVar(IlwisAppContext::tlvDONOTSHOWFINDERROR));
			bool fOldVal = *fDoNotShowError;
			*fDoNotShowError = true;
			TableStoreIlwis3 tbl;
			tbl.load(fn,sSection);


			int colMinCrd = tbl.index("MinCoords");
			int colMaxCrd = tbl.index("MaxCoords");
			int colCoords = tbl.index("Coords");
			tbl.sharedValue(colCoords,true);
			int colDeleted = tbl.index("Deleted");
			int colValue = tbl.index("SegmentValue");
			Tranquilizer trq("Loading data");

			for(int i = 0; i < tbl.getRowCount(); ++i) {
				ILWIS::Segment *seg;
				bool fVals = ptr.dvrs().fRealValues();
				double value;
				if (fVals){
					seg  =  new ILWIS::RSegment();
				} else {
					seg = new ILWIS::LSegment();
				}
				tbl.get(i,colValue,value);
				CoordinateSequence *seq = NULL;
				tbl.get(i, colCoords,&seq);
				seg->PutCoords(seq);
				seg->PutVal(value);
				geometries->push_back(seg);
				if ( i % 100 == 0) {
					trq.fUpdate(i,tbl.getRowCount()); 
				}
			}
			*fDoNotShowError = fOldVal;
		}
	}
}

void SegmentMapStore::Store()
{
	//remove possible old datafiles

	FileName fnCoordFile, fnValueFile, fnSegmentFile;	
	if ( ptr.ReadElement("SegmentMapStore", "DataCrd", fnCoordFile))
	{
		if ( fnCoordFile.fExist() )
			DeleteFile(fnCoordFile.sFullPath().scVal());
		ptr.WriteElement("SegmentMapStore", "DataCrd", (char*)0);			
	}		
	if ( ptr.ReadElement("SegmentMapStore", "DataSeg", fnSegmentFile))
	{
		if ( fnSegmentFile.fExist() )
			DeleteFile(fnSegmentFile.sFullPath().scVal());
		ptr.WriteElement("SegmentMapStore", "DataSeg", (char*)0);			
	}					

	if ( ptr.ReadElement("SegmentMapStore", "DataSegCode", fnValueFile))
	{
		if ( fnValueFile.fExist() )
			DeleteFile(fnValueFile.sFullPath().scVal());
		ptr.WriteElement("SegmentMapStore", "DataSegCode", (char*)0);					
	}		

	ptr.WriteElement("SegmentMapStore", "Status", (long)iStatus);
	ILWIS::Version::BinaryVersion fvFormatVersion = ptr.getVersionBinary();
	ptr.WriteElement("SegmentMapStore", "Format", (long)fvFormatVersion);
	ptr.WriteElement("SegmentMapStore", "Segments", iSeg());
	if (iSegDeleted() >= 0)
		ptr.WriteElement("SegmentMapStore", "DeletedSegments", iSegDeleted());
	ptr.WriteElement("SegmentMapStore", "Coordinates", iCrd());
	if (!ptr.fReadOnly()) {
		Table tblSegment;
		tblSegment.SetPointer(new TablePtr(ptr.fnObj, FileName(ptr.fnObj.sFullPath(), ".mps#"), Domain("none"), ""));
		Domain dmcrd;
		Domain dmcrdbuf("CoordBuf");
		dmcrd.SetPointer(new DomainCoord(ptr.cs()->fnObj));

		Column colMinCoords = tblSegment->colNew("MinCoords", dmcrd, ValueRange());
		Column colMaxCoords = tblSegment->colNew("MaxCoords", dmcrd, ValueRange());
		colMinCoords->SetOwnedByTable();
		colMaxCoords->SetOwnedByTable();		
		Column colCoords = tblSegment->colNew("Coords", dmcrdbuf);
		ColumnCoordBuf *colCrdBuf = colCoords->pcbuf();
		Column colDeleted = tblSegment->colNew("Deleted", Domain("bool"));
		Column colSegmentValue = tblSegment->colNew("SegmentValue", ptr.dvrs());
		int actual = 0;
		for(int i = 0; i < geometries->size(); ++i) {
			ILWIS::Segment *seg = (ILWIS::Segment *)geometries->at(i);	
			if ( seg->fDeleted()) continue;
			++actual;
			CoordBounds cb = seg->cbBounds();
			tblSegment->recNew();
			const CoordinateSequence *seq = seg->getCoordinates();
			colCrdBuf->PutVal(actual,seq, seq->size());
			colSegmentValue->PutRaw(actual,seg->iValue());
			colMinCoords->PutVal(actual, cb.cMin);
			colMaxCoords->PutVal(actual,cb.cMax);
			colDeleted->PutVal(actual,(long)0);
			delete seq;
		}
		tblSegment->Store();
	}
}

void SegmentMapStore::UnStore(const FileName& fn)
{
	//Table tbl(fn); 
	//tbl->pts->UnStore(fn);
}  

SegmentMapStore::~SegmentMapStore() {            // automatic close the map
	if (ptr.fErase || fErase) {
		FileName fnDat(ptr.fnObj,".mps#");
		_unlink(fnDat.sFullName().scVal());
	}

}

void SegmentMapStore::Updated()
{
  ptr.Updated();
  timStore = ptr.objtime;
}

void SegmentMapStore::Export(const FileName& fn) const
{
 // File filSlg(FileName(fn, ".slg"), facCRT);
 // filSlg.WriteLnAscii("1");  // version
 // filSlg.WriteLnAscii("0");  // nr of pol maps
 // filSlg.WriteLnAscii("*");  // mask
	//File filSegments(FileName(fn, ".seg"), facCRT);
	//File filCoords(FileName(fn, ".crd"), facCRT);
	//filSlg.SetErase(true);
	//filSegments.SetErase(true);
	//filCoords.SetErase(true);

 // Tranquilizer trq(SSEGTitleExport14Segments_S);
 // trq.Start();

	//CoordBounds cb = ptr.cbGetCoordBounds();
	//float rAlfa = (float)(max(cb.width(), cb.height()) / 30000);
	//if (rAlfa < 1e-5)
	//	rAlfa = (float)1e-5;
	//float rBeta1 = (float)(cb.MinX() / 2 + cb.MaxX() / 2);
	//float rBeta2 = (float)(cb.MinY() / 2 + cb.MaxY() / 2);
	//filCoords.Write(sizeof(float), &rAlfa);
	//filCoords.Write(sizeof(float), &rBeta1);
	//filCoords.Write(sizeof(float), &rBeta2);

	//// clear control points area
	//char *p = new char[20 * 8];
	//memset(p, 0, 20 * 8);
	//filCoords.Write(20 * 8, p);
	//delete p;

 // segtype st;
 // String sSegVal;
 // trq.SetText(SSEGTextStoringSegmentNames);
 // DomainSort *pdsrt = ptr.dm()->pdsrt();
 // bool fCodes = (0 != pdsrt) && pdsrt->fCodesAvailable();

	//Buf<crdtype> rcbuf;
	//CoordBuf cbuf;

	//// skip first special segment for now
	//// will be filled in after last segment has been written
 // filSegments.Seek(sizeof(segtype));
	//long iSegCnt = 0;
	//long iNrCoords = 0;
	//long iCur = 0;
	//long iNrSeg = ptr.iSeg();
	//long iCrdIndex = 1;
	//MinMax mmMap;
 // for (ILWIS::Segment seg = ptr.segFirst(); seg.fValid(); ++seg) 
	//{
 //   if (trq.fUpdate(iCur, iNrSeg))
 //     return;
	//	iCur++;

 //   if (0 == pdsrt)
 //     sSegVal = seg.sValue();
 //   else {
 //     long iRaw = seg.iRaw();
 //     if (fCodes)
 //       sSegVal = pdsrt->sCodeByRaw(iRaw);
 //     else
 //       sSegVal = pdsrt->sNameByRaw(iRaw);
 //   }
 //   sSegVal = sSegVal.sLeft(15);     // maximum length of segment names
 //   sSegVal = sSegVal.sTrimSpaces();
	//	memset(&st, 0, sizeof(segtype));
 //   for (unsigned short ii = 0; ii < sSegVal.length(); ii++)
 //     if (sSegVal[ii] == ' ')
 //       sSegVal[ii] = '_';
 //   st.code = sSegVal;

	//	long iCrdCnt;
	//	seg.GetCoords(iCrdCnt, cbuf);

	//	if (iCrdCnt > rcbuf.iSize())
	//		rcbuf.Size(iCrdCnt);

	//	MinMax mmSeg;
	//	// convert to ILWIS 1.x internal coordinates
	//	for (int i = 0; i < iCrdCnt; i++)
	//	{
	//		double rCol, rRow;
	//		Coord c = cbuf[i];
	//		if (c.fUndef())
	//			rCol = rRow = rUNDEF;
	//		else 
	//		{
	//			rCol = (c.x - rBeta1) / rAlfa;
	//			rRow = (c.y - rBeta2) / rAlfa;
	//		}
	//		RowCol rc = RowCol(roundx(rRow), roundx(rCol));
	//		mmSeg += rc;
	//		rcbuf[i] = rc;
	//	}
	//	mmMap += mmSeg;

	//	long iToDo;
	//	long iOff = 0;
	//	while (iCrdCnt > 0) 
	//	{
	//		if (iCrdCnt < 990 + 10)     // '+ 10' to be certain last segment part has a least 10 points
	//			iToDo = iCrdCnt;
	//		else
	//			iToDo = 990;

	//		st.fstp = iCrdIndex;
	//		filCoords.Write((iToDo - 2) * sizeof(crdtype), &rcbuf[iOff + 1]);
	//		iCrdIndex += iToDo - 2 - 1;  // -2: don't count nodes
	//		st.lstp = iCrdIndex;
	//		iCrdIndex++;
	//		st.mm = mmSeg;
	//		st.fst = rcbuf[iOff];
	//		st.lst = rcbuf[iOff + iToDo - 1];
	//		filSegments.Write(sizeof(segtype), &st);
	//		iSegCnt++;
	//		iNrCoords += iToDo;

	//		iOff += iToDo - 1;  // node needs to be done twice
	//		iCrdCnt -= iToDo;
	//		if (iCrdCnt > 0) iCrdCnt++;  // '++' because intermediate node needs to be done twice
	//	}

	//	if (iSegCnt > 32000)
	//	{
	//		// not all segments have been exported, so:
	//		//   extend the calculated boundary of the map to avoid
	//		//   that points are located exactly on the boundary
	//		mmSeg.rcMin.Col -= mmSeg.width() / 20;
	//		mmSeg.rcMin.Row -= mmSeg.height() / 20;
	//		mmSeg.rcMax.Col += mmSeg.width() / 20;
	//		mmSeg.rcMax.Row += mmSeg.height() / 20;
	//		String sWarn = String(SCVWarnTooManySegments_I.scVal(), iSegCnt);
	//	  getEngine()->Message(sWarn.scVal(),
 //                             SSEGTitleExport14Segments_S.scVal(),
 //                             MB_OK | MB_ICONEXCLAMATION);
	//		break;
	//	}
	//	else
	//	{
	//		// all segments have been exported, therefore use the original
	//		// map boundary
	//		mmSeg.rcMin.Col = (long)((cb.cMin.x - rBeta1) / rAlfa);
	//		mmSeg.rcMin.Row = (long)((cb.cMin.y - rBeta2) / rAlfa);
	//		mmSeg.rcMax.Col = (long)((cb.cMax.x - rBeta1) / rAlfa);
	//		mmSeg.rcMax.Row = (long)((cb.cMax.y - rBeta2) / rAlfa);
	//	}
 // }

	//// write ILWIS 1.4 segment map information record
	//memset(&st, 0, sizeof(segtype));
	//st.code = "";
	//st.fst = crdtype(0, 1); // nr of control points, format version == 1
	//st.lst = crdtype(0, iSegCnt);
	//st.mm = mmMap;
	//st.fstp = 0;  // not used
	//st.lstp = iNrCoords; // nr of coords written;

	//filSegments.Seek(0);
	//filSegments.Write(sizeof(segtype), &st);

	//filSlg.SetErase(false);
	//filSegments.SetErase(false);
	//filCoords.SetErase(false);

	//trq.Stop();
}

void SegmentMapStore::SetErase(bool f)
{
	fErase = f;
} 

ILWIS::Segment *SegmentMapStore::seg(long iRec) const
{
  ILWISSingleLock sl(&ptr.csAccess, TRUE, SOURCE_LOCATION);
  if (abs(iRec) >= geometries->size()) 
	  return NULL;
  return ((ILWIS::Segment *) geometries->at(iRec));
}

Geometry *SegmentMapStore::getTransformedFeature(long iRec, const CoordSystem& csy) const
{
  ILWISSingleLock sl(&ptr.csAccess, TRUE, SOURCE_LOCATION);
  if (abs(iRec) >= geometries->size()) 
	  return NULL;
   ILWIS::Segment *s = (ILWIS::Segment *) geometries->at(iRec);
   CoordinateSequence *seq = s->getCoordinates();
   CoordinateArraySequence *aseq = new CoordinateArraySequence();
   for(int i =0; i < seq->size(); ++i) {
	   Coord cold = seq->getAt(i);
	   Coord cnew = ptr.cs()->cConv(csy, cold);
	   aseq->add(cnew);
   }
   delete seq;
   ILWIS::Segment *seg;
   if ( ptr.dvrs().fUseReals()) {
		seg  = new ILWIS::RSegment();
	} else {
		seg =  new ILWIS::LSegment();
	}
   seg->PutCoords(aseq);
   seg->PutVal(s->rValue());
   return seg;
}

ILWIS::Segment *SegmentMapStore::seg(const String& sV) {
  for(int i = 0; i< geometries->size(); ++i) {
	  ILWIS::Segment *seg = (ILWIS::Segment *)geometries->at(i);
	  if ( seg->fValid() ) {
		  if ( seg->sValue(ptr.dvrs()) == sV) 
			  return seg;
	  }
  }
  return NULL;
}

ILWIS::Segment *SegmentMapStore::segFirst(long& index, bool fAcceptDeleted) const
{
  long iRec = iSeg();
  ILWISSingleLock sl(&ptr.csAccess, TRUE, SOURCE_LOCATION);
  for(int i = 0; i< geometries->size(); ++i) {
	  ILWIS::Segment *seg = (ILWIS::Segment *)geometries->at(i);
	  index = i;
	  if (seg->fValid())
		  return seg;
	  else if ( fAcceptDeleted && seg->fDeleted() ) {
    	  return seg;
	  }
  }
  return NULL;
}

ILWIS::Segment *SegmentMapStore::segLast() const
{
  ILWISSingleLock sl(&ptr.csAccess, TRUE, SOURCE_LOCATION);
  for(int i = geometries->size(); i > 0; --i) {
	  ILWIS::Segment *seg = (ILWIS::Segment *)geometries->at(i);
	  if ( seg->fValid())
		  return seg;
  }
  return NULL;
}

Feature* SegmentMapStore::newFeature(geos::geom::Geometry *line)        // create a new segment
{
	ILWISSingleLock sl(&ptr.csAccess, TRUE, SOURCE_LOCATION);
	bool fVals = ptr.dvrs().fValues();
	ILWIS::Segment *seg = NULL;
	if ( ptr.dvrs().fUseReals()) {
		seg  = new ILWIS::RSegment((LineString *)line);
	} else {
		seg =  new ILWIS::LSegment((LineString *)line);
	}
	geometries->push_back(seg);
	ptr._iSeg = geometries->size();
	Updated();
  	
	return seg;
}

long SegmentMapStore::iNode(Coord crd) const
{
	Coord crdRes, crdTmp;
	double rTmp;
	double rDist = HUGE_VAL;
	int  segIndex = 0;
	for (;segIndex < geometries->size(); ++segIndex) {
		ILWIS::Segment *seg = (ILWIS::Segment *)geometries->at(segIndex);
		crdTmp = seg->crdBegin();
		if (crdTmp.fUndef()) // did never happen in version 2, and should never happen
			continue;
		rTmp = rDist2(crdTmp, crd);
		if (rTmp < rDist) {
			rDist = rTmp;
			crdRes = crdTmp;
		}
		crdTmp = seg->crdEnd();
		if (crdTmp.fUndef()) // did never happen in version 2, and should never happen
			continue;
		rTmp = rDist2(crdTmp, crd);
		if (rTmp < rDist) {
			rDist = rTmp;
			crdRes = crdTmp;
		}
		if (rDist == 0) break;
	}
	if ( segIndex < geometries->size()   )
		return segIndex;
	return iUNDEF;
}

Coord SegmentMapStore::crdNode(Coord crd) const
{
	Coord crdRes, crdTmp;
	double rTmp;
	double rDist = HUGE_VAL;
	int  segIndex = 0;
	for (;segIndex < geometries->size(); ++segIndex) {
		ILWIS::Segment *seg = (ILWIS::Segment *)geometries->at(segIndex);
		if ( !seg->fValid())
			continue;
		crdTmp = seg->crdBegin();
		if (crdTmp.fUndef()) // did never happen in version 2, and should never happen
			continue;
		rTmp = rDist2(crdTmp, crd);
		if (rTmp < rDist) {
			rDist = rTmp;
			crdRes = crdTmp;
		}
		crdTmp = seg->crdEnd();
		if (crdTmp.fUndef()) // did never happen in version 2, and should never happen
			continue;
		rTmp = rDist2(crdTmp, crd);
		if (rTmp < rDist) {
			rDist = rTmp;
			crdRes = crdTmp;
		}
		if (rDist == 0) break;
	}
	if ( segIndex <= geometries->size())
		return crdRes;
	return Coord();
}

bool fOk(const SegmentMapStore* sm, bool& fFirst, ILWIS::Segment* s)
  {
    if (s->fValid())
      return true;
    if (!fFirst)
      return false;
	long index;
    s = sm->segFirst(index);
    fFirst = false;
    return s->fValid();
  }

Coord SegmentMapStore::crdCoord(Coord crd, ILWIS::Segment** seg, long& iNr) const // existing coordinate
{
	ILWIS::LPoint pnt(crd, 1);
	double minDist = -1.0;
	ILWIS::Segment *closestSeg = NULL;
	for(int i = 0; i < geometries->size(); ++i) {
		ILWIS::Segment *s = (ILWIS::Segment *)geometries->at(i);
		if ( !s || s->fValid()==false)
			continue;
		geos::operation::distance::DistanceOp dop(s,&pnt);
		double dist = dop.distance();
		if ( dist <= minDist || minDist == -1.0) {
			minDist = dist;
			closestSeg = s;
		}
		if ( dist == 0.0)
			break;
	}
	double minDistInternal = -1.0;
	Coord cSearch;
	if ( closestSeg != NULL) {
		CoordinateSequence *seq = closestSeg->getCoordinates();
		for(int j = 0; j < seq->size(); ++j) {
			ILWIS::LPoint pntLine(seq->getAt(j),1);
			geos::operation::distance::DistanceOp dop(&pntLine,&pnt);
			double dist = dop.distance();
			if ( dist < minDistInternal || minDistInternal == -1.0) {
				minDistInternal = dist;
				iNr = j;
				cSearch = seq->getAt(j);
			}
		}
		if ( iNr < 0)
			throw ErrorObject("Couldnt find closest point in segment");
		if ( seg != NULL)
			*seg = closestSeg;
		delete seq;
		return cSearch;
	}
	return Coord();
}

Coord SegmentMapStore::crdPoint(Coord crd, ILWIS::Segment** seg, long& iAft,
                           bool fAcceptDeleted) const // somewhere on a segment
{
	Coord crdRes = crdCoord(crd,seg,iAft);
	double rMinDist2 = rDist2(crd,crdRes)+1;
	double rMinDist = sqrt(rMinDist2);
	for (int i = 0; i < geometries->size(); ++i) {
		ILWIS::Segment *s = (ILWIS::Segment *)geometries->at(i);
		if ( s == NULL || s->fValid()== false)
			continue;
		if (s->cbBounds().fNear(crd,rMinDist)) {
			CoordinateSequence *buf = s->getCoordinates();
			for (int i = 0; i < buf->size()-1; ++i) {
				CoordBounds cb(buf->getAt(i),buf->getAt(i+1));
				if (!cb.fNear(crd,rMinDist)) continue;

				double dxAB, dyAB, dxAC, dyAC, d2, u, v;
				Coord crdA = buf->getAt(i);
				Coord crdB = buf->getAt(i+1);
				dxAB = crdB.x - crdA.x;
				dyAB = crdB.y - crdA.y;
				dxAC = crd.x  - crdA.x;
				dyAC = crd.y  - crdA.y;
				d2 = dxAB * dxAB + dyAB * dyAB;
				if (d2 < 0.1) continue; // should never happen
				v = (dxAC * dyAB - dyAC * dxAB) / d2;
				if (abs(dxAB) > abs(dyAB))
					u = (dxAC + v * dyAB) / dxAB;
				else
					u = (dyAC - v * dxAB) / dyAB;
				if (u >= 0 && u <= 1) {
					Coord crdTmp;
					crdTmp.x = crdA.x + u * dxAB;
					crdTmp.y = crdA.y + u * dyAB;
					double rD2 = rDist2(crdTmp,crd);
					if (rD2 < rMinDist2) {
						crdRes = crdTmp;
						if ( seg != NULL)
							*seg = s;
						iAft = i;
						rMinDist2 = rD2;
						rMinDist = sqrt(rMinDist2);
					}
				}
			} // for i
			delete buf;
		} // if seg near
	} // for seg
	return crdRes;;


}


void SegmentMapStore::GetDataFiles(Array<FileName>& afnDat, Array<String>* asSection, Array<String>* asEntry) const
{
//  IlwisObjectPtr::GetDataFiles(afnDat, asSection, asEntry);
  if (ptr.getVersionBinary() == ILWIS::Version::bvFORMAT30)
	  ObjectInfo::Add(afnDat, FileName(ptr.fnObj,".mps#"), ptr.fnObj.sPath());
  else if (ptr.getVersionBinary() == ILWIS::Version::bvFORMAT20) {
    FileName fnDat;
    ptr.ReadElement("SegmentMapStore", "DataSeg", fnDat);
    ObjectInfo::Add(afnDat, fnDat, ptr.fnObj.sPath());
    ptr.ReadElement("SegmentMapStore", "DataSegCode", fnDat);
    ObjectInfo::Add(afnDat, fnDat, ptr.fnObj.sPath());
    ptr.ReadElement("SegmentMapStore", "DataCrd", fnDat);
    ObjectInfo::Add(afnDat, fnDat, ptr.fnObj.sPath());
  }
}

bool SegmentMapStore::fSegExist(const ILWIS::Segment& segNew, Tranquilizer* trq) const
{
  CoordinateSequence *crdBufNew = segNew.getCoordinates();
  bool ret =  fSegExist(crdBufNew, segNew.cbBounds(), trq);
  delete crdBufNew;
  return ret;
}

bool SegmentMapStore::fSegExist(const CoordinateSequence *crdBufNew, 
                                const CoordBounds& crdBoundsNew, Tranquilizer* trq) const
{
  for(int i = 0; i < geometries->size(); ++i) {
	  ILWIS::Segment *seg = (ILWIS::Segment *)geometries->at(i);
    if (0 != trq)
      trq->fAborted();
    if (!crdBoundsNew.fContains(seg->cbBounds()))
      continue;
	CoordinateSequence * crdBuf = seg->getCoordinates();
	if (crdBuf->size() != crdBufNew->size())
      continue;
	int j=0;
    for (; j < crdBuf->size(); ++j)
		if (crdBuf->getAt(j) != crdBufNew->getAt(j))
        break;
	if (j == crdBuf->size()){
		delete crdBuf;
      return true;
	}
    for (j=crdBuf->size()-1; j >= 0; --j)
		if (crdBuf->getAt(j) != crdBufNew->getAt(j))
        break;
	if (j < 0){
		delete crdBuf;
      return true;
	}
	delete crdBuf;
  }
  return false;
}

void SegmentMapStore::KeepOpen(bool f)
{
}

bool SegmentMapStore::fConvertTo(const DomainValueRangeStruct& _dvrsTo, const Column& col)
{
 // DomainValueRangeStruct dvrsTo = _dvrsTo;
 // if (col.fValid())
 //   dvrsTo = col->dvrs();
 // Tranquilizer trq;
 // trq.SetTitle(SSEGTitleDomainConversion);
 // trq.SetText(SSEGTextConverting);
 //
 // ILWIS::Segment sg;
	//Column colValue = tblSegment->col("SegmentValue");
	//colValue->fConvertTo(_dvrsTo, col);
	throw ErrorObject("TODO, SegmentMapStore::fConvertTo");
  
  return true;
}

void SegmentMapStore::Flush()
{
}

void SegmentMapStore::Truncate() // remove all contents, make empty
{
  Flush();
  ObjectInfo::WriteElement("SegmentMapStore", (char*)0, ptr.fnObj, (char*)0);
  ptr._iSeg = 0;
  ptr._iSegDeleted = 0;
  ptr._iCrd = 0;
  ptr.setVersionBinary(ILWIS::Version::bvFORMAT30);
  iStatus = 0;
  geometries->resize(0);
  Store();
}

void SegmentMapStore::DoNotUpdate()
{
}

void SegmentMapStore::SetCoordSystem(const CoordSystem& cs)
{
}

void SegmentMapStore::Pack()
{
  ILWISSingleLock sl(&ptr.csAccess, TRUE, SOURCE_LOCATION);
  Tranquilizer trq;
  trq.SetTitle(SSEGTitlePackSegments);
  trq.SetText(SSEGTextPacking);
  trq.Start();
  int i =0;
  while(i < ptr.iFeatures()) {
	  ILWIS::Segment *s = seg(i);
	  if ( s->fDeleted())
		  geometries->erase(geometries->begin() + i, geometries->begin() + +i + 1);
	  else
		  ++i;
  }
  ptr._iSeg = geometries->size();
  ptr._iSegDeleted = 0;
  ptr.Updated();
}


void SegmentMapStore::removeFeature(const String& id, const vector<int>& selectedCoords) {
	for(vector<Geometry *>::iterator cur = geometries->begin(); cur != geometries->end(); ++cur) {
		ILWIS::Segment *seg = CSEGMENT(*cur);
		if ( seg->getGuid() == id  ) {
			if ( selectedCoords.size() == 0 || selectedCoords.size() == geometries->size()) {
				delete seg;
				geometries->erase(cur);
			} else {
				CoordBuf crdBuf;
				CoordinateSequence *seq = seg->getCoordinates();
				vector<bool> status(seq->size(), true);
				for(int i = 0 ; i < selectedCoords.size(); ++i) {
					status[selectedCoords.at(i)] = false;

				}
				int reducedSize = seq->size() - selectedCoords.size();
				crdBuf.Size(reducedSize);
				int count = 0;
				for( int j = 0; j < seq->size(); ++j) {
					if ( !status[j] )
						continue;
					crdBuf[count++] = seq->getAt(j);
				}
				seg->PutCoords(count, crdBuf);
			}
		} 
	}
}