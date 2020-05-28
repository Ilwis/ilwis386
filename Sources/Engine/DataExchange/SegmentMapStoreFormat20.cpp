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
#include "Engine\DataExchange\SegmentMapStoreFormat20.h"
#include "Engine\Table\ColumnCoordBuf.h"
#include "Engine\Domain\dmsort.h"
#include "Engine\Base\System\Engine.h"
#include "Headers\Hs\segment.hs"


SegmentMapStoreFormat20::SegmentMapStoreFormat20(const FileName& fn, SegmentMapPtr& p) :
	SegmentMapStore(fn, p, true)
{
	Load();
}

void SegmentMapStoreFormat20::Load()
{
	FileName CoordFile, ValueFile, SegmentFile;
	File *SGFile=NULL, *CDFile=NULL, *SCFile =NULL;

	if (ptr.ReadElement("SegmentMapStore", "DataCrd", CoordFile) && CoordFile.fExist())
		CDFile = new File(CoordFile);
	if (ptr.ReadElement("SegmentMapStore", "DataSeg", SegmentFile) && SegmentFile.fExist())
		SGFile = new File(SegmentFile);
	if (ptr.ReadElement("SegmentMapStore", "DataSegCode", ValueFile) && ValueFile.fExist())
		SCFile = new File(ValueFile);

	if ( CDFile == 0 || SGFile == 0 )
		throw ErrorObject(SSEGErrMissing20DataFiles);

	bool *fDoNotShowError = (bool *)(getEngine()->pGetThreadLocalVar(IlwisAppContext::tlvDONOTSHOWFINDERROR));
	bool fOldVal = *fDoNotShowError;
	*fDoNotShowError = true;

	rAlfa1 = ptr.rReadElement("SegmentMap", "Alfa");
	rBeta1 = ptr.rReadElement("SegmentMap", "Beta1");
	rBeta2 = ptr.rReadElement("SegmentMap", "Beta2");

	int iNrSegments, iNrCoords, iNrSegDeleted;
	ptr.ReadElement("SegmentMapStore", "Segments", iNrSegments);
	ptr.ReadElement("SegmentMapStore", "DeletedSegments", iNrSegDeleted);
	ptr.ReadElement("SegmentMapStore", "Coordinates", iNrCoords);
	ptr.SetNumberOfSegments(iNrSegments);
	ptr.SetNumberOfDeletedSegments(iNrSegDeleted);
	ptr.SetNumberOfCoords(iNrCoords);

	int iValueSize = st() != stREAL ? 4 : 8;
	segtype *sgbuffer = new segtype[iNrSegments];
	crdtype *crdbuffer = new crdtype[iNrCoords];

	char *valuebuffer;

	if ( SCFile )
		valuebuffer = new char[ iValueSize * iNrSegments]; 

	SGFile->Seek(sizeof(segtype));
	SGFile->Read(sizeof(segtype) * iNrSegments, (void *)sgbuffer);
	CDFile->Seek(172);
	CDFile->Read(sizeof(crdtype) * iNrCoords, (void *)crdbuffer);

	if ( SCFile )
	{
		SCFile->Seek(iValueSize);
		SCFile->Read(iValueSize * iNrSegments, (char *)valuebuffer);
	}

	Tranquilizer trq("Loading data");
	DomainSort *pds = dm()->pdsrt();
	for(long iSeg = 0; iSeg < iNrSegments; ++iSeg)
	{
		segtype st = sgbuffer[iSeg];
		ILWIS::Segment *seg;
		if ( SCFile )
		{
			void *val = &(valuebuffer[iSeg * iValueSize]);
			if (ptr.st() == stREAL)
			{
				seg = new ILWIS::RSegment(spatialIndex);
				double rVal = *((double *) val );
				seg->PutVal(rVal);
			}
			else
			{
				seg = new ILWIS::LSegment(spatialIndex);
				long iVal = *((long *) val);
				seg->PutVal(iVal);
			}
		}
		else
		{
			// .SC# file is missing so try to get
			// the segment code from the .SEG file, which is always available
			seg = new ILWIS::LSegment(spatialIndex);
			String sCode = st.code;
			if (pds)
				seg->PutVal(pds->iRaw(sCode));
		}

		int iStart = st.fstp;
		int iEnd = st.lstp;

		if ( iStart < 0 ) iStart += LONG_MAX;

		int iCount = iStart <= iEnd ? 1 : -1;

		int iCrdInSeg = 0;

		if ( iEnd == 0 && iStart == 0) // hmpff
			iCrdInSeg = 0;
		else
			iCrdInSeg = iEnd - iStart + 1;

		CoordinateSequence *seq = new CoordinateArraySequence(iCrdInSeg + 2);
		seq->setAt(RowCol2Coord(st.fst), 0);

		int iCrd = 0;
		for( ; iCrd < iCrdInSeg; iCrd += iCount)
			seq->setAt(RowCol2Coord(crdbuffer[iStart + iCrd - 1]), iCrd + 1);

		seq->setAt(RowCol2Coord(st.lst), iCrd + 1);

		Coord crdMin = RowCol2Coord(crdtype(st.mm.MinX, st.mm.MinY));
		Coord crdMax = RowCol2Coord(crdtype(st.mm.MaxX, st.mm.MaxY));

		seg->PutCoords(seq);
		seg->Delete(st.fstp < 0);
		geometries->push_back(seg);
		
		if ( iSeg % 100 == 0) {
			trq.fUpdate(iSeg, iNrSegments);
		}

		const Envelope *env =  seg->getEnvelopeInternal();
	}
	trq.fUpdate(iNrSegments, iNrSegments);

	delete [] sgbuffer;
	delete [] crdbuffer;
	if (SCFile) delete [] valuebuffer;
	delete SGFile;
	delete CDFile;
	delete SCFile;
	*fDoNotShowError = fOldVal;
	//tblSegment->Loaded(true);
//	fvFormatVersion = fvFORMAT30; // this changes the to the 3.0 format
//	Updated();
}

Coord SegmentMapStoreFormat20::RowCol2Coord(const crdtype& crd)
{
	return Coord( crd.x * rAlfa1 + rBeta1, crd.y * rAlfa1 + rBeta2 );
}

crdtype SegmentMapStoreFormat20::Coord2RowCol(const Coord& crd, double rAlfa1, double rBeta1, double rBeta2)
{
	return crdtype( (short)((crd.x - rBeta1)) / rAlfa1, (short)(( crd.y - rBeta2 ) / rAlfa1));
}

void SegmentMapStoreFormat20::SaveAsFormat20(const SegmentMap& mp)
{
	int iNrSegments = mp->iFeatures();
	segtype segt;
	String sType;
	bool fPolygonMap = mp->ReadElement("PolygonMap", "Type", sType) != 0 ? true : false;
	File *SGFile = NULL;

	File *CDFile = new File(FileName(mp->fnObj.sFile, fPolygonMap ? ".pd#": ".cd#"));
	File *SCFile = new File(FileName(mp->fnObj.sFile, fPolygonMap ? ".pc#" : ".sc#"));
	if ( !fPolygonMap ) SGFile = new File(FileName(mp->fnObj.sFile, ".sg#"));

	char dummy[172];
	memset(dummy, 0, 172);
	memset(&segt, 0, sizeof(segt));

	long iDummy=0;
	
	SCFile->Write(sizeof(long), &iDummy);
	CDFile->Write(172, &dummy);
	if ( !fPolygonMap) SGFile->Write(sizeof(segtype), &segt); 

	int iCDPos = 0;
	const int iMaxPnt = 1000;
	CoordBounds cb = mp->cb();
	double  rAlfa = max(cb.width(), cb.height()) / 30000;
	if (rAlfa < 1e-5)  rAlfa = 1e-5;      // Store() member function stores only 6 decimals
	double rBeta1 = cb.MinX()/2 + cb.MaxX()/2;
	double  rBeta2 = cb.MinY()/2 + cb.MaxY()/2;

	int iTotalSeg = 0;

	for (ILWIS::Segment* seg = mp->segFirst(); seg->fValid(); ++seg) 
	{
		CoordinateSequence *cbuf = seg->getCoordinates();
		long iSz = cbuf->size();
		int iNr20Segments = iSz / iMaxPnt + 1;
		int iPointsLastSeg = iSz % iMaxPnt;

		for ( int i = 1; i <= iNr20Segments; ++i)
		{
			int iNrPoints = (i == iNr20Segments) ? iSz % iMaxPnt : iMaxPnt;
			int iStartPoint =  (i - 1) * iMaxPnt;

			for ( int j = 1; j < iNrPoints - 1; ++j)
			{
				RowCol rc1 = Coord2RowCol(cbuf->getAt(iStartPoint + j), rAlfa, rBeta1, rBeta2);
				CDFile->Write(sizeof(crdtype), &rc1);
			}

			if ( !fPolygonMap )
			{
				segt.fst = Coord2RowCol(cbuf->getAt( iStartPoint), rAlfa, rBeta1, rBeta2);
				segt.lst = Coord2RowCol(cbuf->getAt( iStartPoint + iNrPoints - 1), rAlfa, rBeta1, rBeta2);
				CoordBounds cb = seg->cbBounds();
				Coord cMin = cb.cMin;
				Coord cMax = cb.cMax;
				segt.mm = MinMax(Coord2RowCol(cMin, rAlfa, rBeta1, rBeta2), Coord2RowCol(cMax, rAlfa, rBeta1, rBeta2));
				segt.fstp = iCDPos + 4;
				iCDPos += ( iNrPoints - 1) * 4;
				segt.lstp = iCDPos;
				SGFile->Write(sizeof(segtype), &seg);
			}
			if ( mp->fUseReals() )
			{
				double rVal = seg->rValue();
				SCFile->Write(sizeof(double), &rVal);
			}
			else
			{
				long iVal = seg->iValue();
				SCFile->Write(sizeof(long), &iVal);
			}
			++iTotalSeg;
		}
	}	
	mp->WriteElement("SegmentMap", "Alfa",  rAlfa );
	mp->WriteElement("SegmentMap", "Beta1", rBeta1);
	mp->WriteElement("SegmentMap", "Beta2", rBeta2);
	mp->WriteElement("SegmentMapStore", "DataSeg",  SGFile->sName() );
	mp->WriteElement("SegmentMapStore", "DataSegCode",  SCFile->sName() );
	mp->WriteElement("SegmentMapStore", "DataSegCrd",  CDFile->sName() );
	mp->WriteElement("SegmentMapStore", "Format", ILWIS::Version::bvFORMAT20);
	mp->WriteElement("SegmentMapStore", "Segments", iTotalSeg);

	delete SCFile;
	delete CDFile;
	delete SGFile;
}

void SegmentMapStoreFormat20::SetErase(bool f)
{
	FileName CoordFile, ValueFile, SegmentFile;
	File *SGFile=NULL, *CDFile=NULL, *SCFile =NULL;

	if ( ptr.ReadElement("SegmentMapStore", "DataCrd", CoordFile) && CoordFile.fExist())
		CDFile = new File(CoordFile);
	if ( ptr.ReadElement("SegmentMapStore", "DataSeg", SegmentFile) && SegmentFile.fExist())
		SGFile = new File(SegmentFile);
	if ( ptr.ReadElement("SegmentMapStore", "DataSegCode", ValueFile) && ValueFile.fExist())
		SCFile = new File(ValueFile);

	if (CDFile)
		CDFile->SetErase(f);

	if (SGFile)
		SGFile->SetErase(f);

	if (CDFile)
		CDFile->SetErase(f);
} 
