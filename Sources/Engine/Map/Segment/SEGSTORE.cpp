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
// $Log: /ILWIS 3.0/SegmentMap/SEGSTORE.cpp $
 * 
 * 41    8/24/01 13:03 Willem
 * Removed the SetReadOnly() function. This is now handled by
 * IlwisObjectPtr::SetReadOnly() for all ilwis objects
 * 
 * 40    5-04-01 15:44 Koolhoven
 * changed Pack() (a little more efficient)
 * count of number of deleted segments improved
 * 
 * 39    21-03-01 4:04p Martin
 * tblSegment was used in constructor but it need not to be present, so
 * the table if now first checked for its validity
 * 
 * 38    19-03-01 8:28a Martin
 * seterase function was left empty, release diskspace did not work
 * properly because of that
 * 
 * 37    16-03-01 20:43 Koolhoven
 * added tblSegment->DoNotStore(false) because it was forgotten
 * 
 * 36    16/03/01 14:09 Willem
 * Added validity check for "SegmentValue" column before using it
 * 
 * 35    3/16/01 13:33 Retsios
 * Make columns table-owned for showastbl
 * Only look for the SegmentValue column in segment maps
 * 
 * 34    13-03-01 2:24p Martin
 * loading of csy for the internal columns of a seg/pol map will not
 * generate an error
 * 
 * 33    3/05/01 15:13 Retsios
 * Check on validity of colSegmentValue before using it (problems when =
 * colUNDEF)
 * 
 * 32    1/03/01 14:32 Willem
 * The DomainValueRangeStruct and ValueRange for the segment map now also
 * set the ValueRange of the proper TableSegment Column as well to have
 * the domain of the segmentmap and that of the TableSegment Column
 * synchronized.
 * 
 * 31    23-02-01 11:04a Martin
 * columns of the segstore table are now tableowned
 * 
 * 30    23-02-01 10:57a Martin
 * removed a suspicious line in the constructor of segmentmapstore. It set
 * the fErase on true for 2.0 file but this had the effect that any use of
 * 2.0 files could lead to a possible deletion of the file (e.g
 * setreadonly of a 2.0 file)
 * 
 * 29    13-02-01 8:28a Martin
 * when changing the coordsystem of a map the coordsystem of the internal
 * tables (columns) is also changed
 * 
 * 28    20-12-00 19:08 Koolhoven
 * pack() now has a tranquilizer
 * 
 * 27    1-12-00 15:45 Koolhoven
 * in Export() use ptr.cbGetCoordBounds() instead of (the now removed)
 * ptr.crdBounds()
 * 
 * 26    24-11-00 12:29 Koolhoven
 * Added Pack() option for segments
 * 
 * 25    11/01/00 2:01p Martin
 * removed superflous GetObjectStructure. They are all handled in the
 * relevant ilwisobject
 * 
 * 24    30-10-00 11:00a Martin
 * solved a problem witjh loading of tables. tblSegment was not yet set
 * when trying to set its status on loaded. Only occured during
 * foreignformat.
 * 
 * 23    3-10-00 13:05 Hendrikse
 * Segstore(data) should not try to load itself when there is no
 * meaningfull datafile. (iSeg == 0)
 * 
 * 22    9/19/00 9:08a Martin
 * added protection against superflous stores
 * 
 * 21    9/18/00 9:40a Martin
 * if newwly created the loadstate of the internal table(s) is set to true
 * 
 * 20    12-09-00 1:04p Martin
 * prevented automatic conversion (on disk) to 3.0 as long as no changes
 * are made. Delete old datafiles if converted to 3.0
 * 
 * 19    11-09-00 11:30a Martin
 * added function for objectStructure 
 * 
 * 18    16-06-00 20:49 Koolhoven
 * use Column members of SegmentMapStore instead of enum ColumnTypes with
 * indirections
 * 
 * 17    12-05-00 10:55 Koolhoven
 * iNode(), crdNode() and crdCoord() are now protected against
 * segmentbuffers which have illegal contents (undefs)
 * 
 * 16    16/03/00 16:17 Willem
 * - Export segments to ILWIS 1.4 format now exports the segments using
 * interface functions. Because of format changes copying data files did
 * not yield proper results anymore.
 * 
 * 15    3/03/00 14:24 Willem
 * The data file is now stored in the same directory as the ODF
 * 
 * 14    28-02-00 8:49a Martin
 * Segmentmap can now read foreign formats
 * 
 * 13    15/02/00 15:56 Willem
 * The fPolygonMap now checks the extension to see whether it belongs to a
 * PolygonMap
 * 
 * 12    15/02/00 12:48 Willem
 * - new segmentmaps were deleted immediately after creation; now first
 * existance is checked
 * - the deleted column is now filled with a valid value
 * 
 * 11    8-02-00 18:01 Wind
 * set fUpdateCatalog flag to false after creation of internal table
 * 
 * 10    8-02-00 11:58 Wind
 * bug in newFeature(long iSegNr)
 * 
 * 9     8-02-00 11:11 Wind
 * bug in newFeature(long iSegNr)
 * 
 * 8     7-02-00 17:04 Wind
 * solved problems with readonly maps and their conversion to format 3.0
 * 
 * 7     17-01-00 8:17a Martin
 * changed rowcols to coords
 * 
 * 6     10-12-99 11:48a Martin
 * removed internal rowcols and replaced them by true coords
 * 
 * 5     29-10-99 9:20 Wind
 * thread save stuff
 * 
 * 4     9/24/99 10:37a Wind
 * replaced calls to static funcs ObjectInfo::ReadElement and WriteElement
 * by calls to member functions
 * 
 * 3     9/08/99 12:55p Wind
 * changed constructor calls FileName(fn, sExt, true) to FileName(fn,
 * sExt)
 * or changed FileName(fn, sExt, false) to FileName(fn.sFullNameQuoted(),
 * seExt, false)
 * to ensure that proper constructor is called
 * 
 * 2     9/08/99 12:02p Wind
 * comments
*/
// Revision 1.9  1998/09/16 17:25:20  Wim
// 22beta2
//
// Revision 1.8  1998/03/20 14:45:21  Willem
// Added a check for too many segments when a SegNew is executed. The constant
// is defined in base.h
//
// Revision 1.7  1997/09/25 16:28:39  Wim
// In constructor do never set offset for alSegBuf, just resize 1+iSeg()
//
// Revision 1.6  1997-09-24 19:08:43+02  Wim
// newFeature(nr) was not updating alSegBuf properly
//
// Revision 1.5  1997-09-19 17:52:19+02  Wim
// Added buffering with alSegBuf
//
// Revision 1.4  1997-08-25 09:54:30+02  Wim
// In Store() filSegCode is now also flushed
//
// Revision 1.3  1997-08-08 00:07:00+02  Willem
// Export to 1.4 now truncates the segment code to 15 characters.
//
/* SegmentMapStore
   Copyright Ilwis System Development ITC
   march 1995, by Wim Koolhoven
	Last change:  WN   20 Mar 98   10:31 am
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
#include "Headers\geos\operation\distance\DistanceOp.h"
#include "Engine\Table\Rec.h"
#include "Engine\Map\Point\ilwPoint.h"

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
			Table tblSegment;
			tblSegment.SetPointer(new TablePtr(fn, sSection));
			tblSegment->Load();
			tblSegment->DoNotStore(true);
			Column colMinCoords = tblSegment->col("MinCoords");
			Column colMaxCoords = tblSegment->col("MaxCoords");
			Column colCoords = tblSegment->col("Coords");
			ColumnCoordBuf *colCrdBuf = colCoords->pcbuf();
			Column colDeleted   = tblSegment->col("Deleted");
			Column colSegmentValue = tblSegment->col("SegmentValue");
			for(int i = 0; i < tblSegment->iRecs(); ++i) {
				ILWIS::Segment *seg;
				bool fVals = ptr.dvrs().fRealValues();
				double value;
				if (fVals){
					seg  =  new ILWIS::RSegment();
					value = colSegmentValue->rValue(i+1);
				} else {
					seg = new ILWIS::LSegment();
					if (ptr.dvrs().fRawAvailable())
						value = colSegmentValue->iRaw(i+1);
					else
						value = colSegmentValue->iValue(i+1);
				}
				CoordinateSequence *seq = NULL;
				seq = colCrdBuf->iGetValue(i+1);
				seg->PutCoords(seq);
				seg->PutVal(value);
				geometries->push_back(seg);
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
			CoordBounds cb = seg->crdBounds();
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
		if (s->crdBounds().fNear(crd,rMinDist)) {
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
  bool ret =  fSegExist(crdBufNew, segNew.crdBounds(), trq);
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
    if (!crdBoundsNew.fContains(seg->crdBounds()))
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
