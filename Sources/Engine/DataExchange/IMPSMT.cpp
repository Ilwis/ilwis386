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
/* impsmt.c
   Import Ilwis Segment List
   by Willem Nieuwenhuis, march 11, 1996
   ILWIS Department ITC
   Last change:  WN   29 Sep 97   10:52 am
*/
#include "Headers\toolspch.h"
#include "Engine\DataExchange\Convloc.h"
#include "Engine\Map\Segment\SEGSTORE.H"
#include "Engine\Map\Polygon\POLSTORE.H"
#include "Engine\Domain\dmsort.h"
#include "Engine\SpatialReference\csbonly.h"

enum kind { KNDClass, KNDID, KNDValue, KNDNone };

#define errSMTBase         1090
#define errSMTFileNotFound errSMTBase + 0
#define errSMTIllegalSMT   errSMTBase + 1
#define errSMTFormatError  errSMTBase + 2

class SMTList {
public:
	SMTList(const FileName& fnFile);
	bool fSMTSegment(CoordBuf&, long&);  // return true if EOF
	bool fIsValueDomain()    { return fValueDomain; }
	void GetBounds(const FileName& fnObject, CoordSystem&, CoordBounds&);
	void ScanMap(Tranquilizer&, CoordBuf&);
	void SetupSegMap(const FileName&);
	String sSegmentCode() { return _sSegCode; }
	long iLinesRead()     { return _iLins; }
	long iSegmentsRead()  { return _iNrSeg; }
	~SMTList();
	String sReason;
private:
	bool fSMTCoord(Coord&);        // read world coordinate from file
	kind kndSMTCode(String&);      // check if eligible for Domain ID
	void GetNextLine();            // get line from SMT list
	bool fVectorType();            // SMT: polygons or segments?
	bool fPolygons;                // store result of fVectorType()
	bool fInSegment;               // flag if SMT segment has more than 1000 coords
	bool fValueDomain;
	double rMin, rMax;
	File* fileSMT;
	String sLine, _sSegCode;
	long _iLins, _iNrSeg;
};

// implementation

SMTList::SMTList(const FileName& fnFile) {
	fileSMT = new File(fnFile);
	_iLins = 0;
	_iNrSeg = 0;
	fInSegment = false;
	fValueDomain = true;
	rMin = 0;
	rMax = -1;
}

SMTList::~SMTList() {
	if (fileSMT) delete fileSMT;
}

void SMTList::GetNextLine() 
{
	do
    fileSMT->ReadLnAscii(sLine);
	while (sLine.length() == 0);
	_iLins++;
}

bool SMTList::fSMTCoord(Coord& cFile) 
{
	short iRes = sscanf(sLine.scVal(), "%lf %lf", &cFile.x, &cFile.y);
	return (2 == iRes);
}

kind SMTList::kndSMTCode(String& sC) {
	double r;
	short iRes = sscanf(sLine.scVal(), "%lf", &r);
	if ((0 == iRes) || (1 == iRes)) {
		Array<String> as;
		Split(sLine, as, " \t");
		sC = as[0];
		if (0 == iRes)
			return KNDClass;
		else
		{
			rMin = min(rMin, r);
			rMax = max(rMax, r);
			return KNDValue;
		}
	}
	else
		return KNDNone;    // should not come here
}

bool SMTList::fVectorType() {
	sReason = SCVErrFileNotFound;
	if (!fileSMT)
		throw ErrorObject(sReason, errSMTFileNotFound);
	sReason = SCVErrIllegalSMT;
	GetNextLine();
	sLine.toLower();
	fPolygons = sLine.find("polygons") != String::npos;
	if (sLine.find("segments") == String::npos)
		throw ErrorObject(sReason, errSMTIllegalSMT);
	return fPolygons;
}

void SMTList::GetBounds(const FileName& fnObject, CoordSystem& csVect, CoordBounds& cbVect) {
	sReason = SCVErrWrongBoundary;
	Coord cMin, cMax;
	GetNextLine();
	if (2 != sscanf(sLine.scVal(), "%lf %lf", &cMin.x, &cMin.y) ) {
		sReason &= String(SCVTextInLine_l.scVal(), _iLins);
		throw ErrorObject(sReason, errSMTFormatError);
	}
	GetNextLine();
	if (2 != sscanf(sLine.scVal(), "%lf %lf", &cMax.x, &cMax.y) ) {
		sReason &= String(SCVTextInLine_l.scVal(), _iLins);
		throw ErrorObject(sReason, errSMTFormatError);
	}
	cbVect = CoordBounds(cMin, cMax);
}

bool SMTList::fSMTSegment(CoordBuf& cbSeg, long& iNrCoord)
{
	if (fileSMT->fEof()) return false;
	iNrCoord = 0;
	kind knd = kndSMTCode(_sSegCode);
	if (KNDNone != knd) 
	{
		fValueDomain = fValueDomain && (knd == KNDValue);
		GetNextLine();
		Coord crd;
		
		if (iNrCoord >= cbSeg.iSize())
			cbSeg.Size(cbSeg.iSize() + 1000);
		while ( fSMTCoord(cbSeg[iNrCoord])) 
		{
			iNrCoord++;
			if (iNrCoord >= cbSeg.iSize())
				cbSeg.Size(cbSeg.iSize() + 1000);
			if (fileSMT->fEof())
				break;    // allow last segment to be converted
			GetNextLine();
		}  // end while: sLine now holds a segment code or EOF is reached
		_iNrSeg++;
	}
	else
		return false;  // should not come here
	return true;
}

void SMTList::ScanMap(Tranquilizer& trq, CoordBuf& cbSegment)
{
	long iNrCoord;
	long iSize = fileSMT->iSize();
	
	GetNextLine();  // skip SMT type (segments or polygons
	GetNextLine();  // skip boundary info
	GetNextLine();  // skip boundary info
	GetNextLine();  // position now at first segment
	
	trq.SetText(SCVTextScanSegment_);
	while (fSMTSegment(cbSegment, iNrCoord)) 
	{
		if (trq.fUpdate(fileSMT->iLoc(), iSize))
			return;
	}
}

void SMTList::SetupSegMap(const FileName& fnObject) {
	fileSMT->Seek(0);  // go back to the beginning
	fPolygons = fVectorType();
	CoordSystem csSMT;
	CoordBounds cbSMT;
	GetBounds(fnObject, csSMT, cbSMT);
	
	Domain dm;
	FileName fnDomain = FileName::fnUnique(FileName(fnObject, ".dom"));
	if (fValueDomain)
	{
		double rDelta = (rMax - rMin) / 10;  // add some margin in the valuerange
		dm = Domain("value", rMin - rDelta, rMax + rDelta, 0.01);
	}
	else
	{
		dm = Domain(fnDomain, 0, dmtCLASS);
		DomainSort* dms = dm->pdsrt();
		dms->dsType = DomainSort::dsMANUAL;
	}
	DomainValueRangeStruct dvs(dm);
	SegmentMap sm(fnObject, csSMT, cbSMT, dvs);
	sm->sDescription = String("%S %S", SCVTextSegmentMap, fnObject.sFile);
	sm->Store();
	GetNextLine();     // Set file pointer to first line of segment ( Segment code }
}

void ImpExp::ImportSMT(const FileName& fnFile, const FileName& fnObject ) {
	try {
		trq.SetTitle(SCVTitleImportSmtList);
		trq.fUpdate(0);
		SMTList smt(fnFile);
		
		CoordBuf cbSegment;
		long iNrCoord;
		smt.ScanMap(trq, cbSegment);
		
		smt.SetupSegMap(fnObject);            // create the segment map
		SegmentMap sm = SegmentMap(fnObject); // reuse the segment map, forcing a mapstore
		sm->fErase = true;
		smt.sReason = SCVErrCreateVector;
		if (sm->fDependent()) {
			smt.sReason &= String(SCVTextInLine_l.scVal(), smt.iLinesRead());
			throw ErrorObject(smt.sReason, errSMTFormatError);
		}
		
		smt.sReason = SCVErrIllegalSMT;
		sm->Updated();
		trq.fUpdate(0);
		
		String s;
		DomainSort *pds = sm->dm()->pdsrt();
		while (smt.fSMTSegment(cbSegment, iNrCoord)) 
		{
			s = SCVTextProcSegment_;
			s &= smt.sSegmentCode();
			trq.SetText(s);
			if (trq.fUpdate(smt.iSegmentsRead()))
				return;
			if (iNrCoord >= 2) 
			{
				if (pds != NULL && pds->iRaw(smt.sSegmentCode()) == iUNDEF)
					pds->iAdd(smt.sSegmentCode());
				ILWIS::Segment *segCur = CSEGMENT(sm->newFeature());
				segCur->PutCoords(iNrCoord, cbSegment);
				segCur->PutVal(sm->dvrs(),smt.sSegmentCode());
			}
		}
		trq.fUpdate(0);
		sm->fErase = false;
	}
	catch (ErrorObject& err) {
		err.Show();
	}
}


