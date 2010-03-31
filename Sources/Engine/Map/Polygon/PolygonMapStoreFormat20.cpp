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
#include "Engine\Domain\dmcoord.h"
#include "Engine\Map\Polygon\PolygonMapStoreFormat20.h"

PolygonMapStoreFormat20::PolygonMapStoreFormat20(const FileName& fn, PolygonMapPtr& p) :
	PolygonMapStore(fn, p, true)
{
	Load();
}

void PolygonMapStoreFormat20::Load()
{
	File *filPol=NULL, *filPolCode=NULL, *filTop=NULL;
	FileName CoordFile, ValueFile, SegmentFile;
	File *SGFile=NULL, *CDFile=NULL;

	FileName fnPolygon, fnTop, fnCode;

	if ( ptr.ReadElement("PolygonMapStore", "DataPol", fnPolygon) && fnPolygon.fExist())
		filPol = new File(fnPolygon);
	if ( ptr.ReadElement("PolygonMapStore", "DataTop", fnTop) && fnTop.fExist())
		filTop = new File(fnTop);
	if ( ptr.ReadElement("PolygonMapStore", "DataPolCode", fnCode) && fnCode.fExist())
		filPolCode = new File(fnCode);
 	if (ptr.ReadElement("SegmentMapStore", "DataCrd", CoordFile) && CoordFile.fExist())
		CDFile = new File(CoordFile);
	if (ptr.ReadElement("SegmentMapStore", "DataSeg", SegmentFile) && SegmentFile.fExist())
		SGFile = new File(SegmentFile);
	

	rAlfa1 = ptr.rReadElement("SegmentMap", "Alfa");
	rBeta1 = ptr.rReadElement("SegmentMap", "Beta1");
	rBeta2 = ptr.rReadElement("SegmentMap", "Beta2");
	long iPol = ptr.iReadElement("PolygonMapStore","Polygons");

	int iNrSegments, iNrCoords, iNrSegDeleted;
	ptr.ReadElement("SegmentMapStore", "Segments", iNrSegments);
	ptr.ReadElement("SegmentMapStore", "DeletedSegments", iNrSegDeleted);
	ptr.ReadElement("SegmentMapStore", "Coordinates", iNrCoords);

	int iValueSize = st() != stREAL ? 4 : 8;
	segtype *sgbuffer = new segtype[iNrSegments];
	crdtype *crdbuffer = new crdtype[iNrCoords];

	SGFile->Seek(sizeof(segtype));
	SGFile->Read(sizeof(segtype) * iNrSegments, (void *)sgbuffer);
	CDFile->Seek(172);
	CDFile->Read(sizeof(crdtype) * iNrCoords, (void *)crdbuffer);

	poltype *polbuf = new poltype[iPol];
	toptype *topbuf = new toptype[iNrSegments];
	char *valuebuffer = new char[ iValueSize * iPol]; 

	filPol->Read(sizeof(poltype) * iPol, (void *)polbuf);
	filTop->Seek(sizeof(toptype));
	filTop->Read(sizeof(toptype) * iNrSegments, (void *)topbuf);
	filPolCode->Seek(0); // ?? check
	filPolCode->Read(iValueSize * iPol, (void *)valuebuffer);

	vector<CoordBuf> coords;
	
	
	DomainSort *pds = dm()->pdsrt();

	for(int iPolygon=0; iPolygon < iPol; ++iPolygon)
	{
		long iVal = 0;
		double rVal = 0;
		poltype pol = polbuf[iPolygon];
		int iStart = abs(pol.iSegStart)-1;
		int iCurrent = iStart;
		toptype top = topbuf[abs(iStart)];
		Coord crd;
		vector< vector<Coordinate> *> rings;
		vector<Coordinate> *v = new vector<Coordinate>();
		bool fEnd = false;
		do{
			CoordBuf crdBuf;
			GetCoordBuf(iCurrent,crdBuf,sgbuffer,crdbuffer);
			if ( !crd.fUndef() && v->size() > 0 && v->at(0) != crdBuf[crdBuf.size() - 1]) {
				rings.push_back(v);
				v = new vector<Coordinate>();
			}

			for(int j=0; j < crdBuf.size(); ++j) {
				Coord c = crdBuf[j];
				if (!c.fUndef() ) {
					if ( v->size() > 0)
						if ( Coordinate(c) == v->at(v->size()))
							continue;
					v->push_back(c);
				};
			}
			toptype top = topbuf[abs(iCurrent)];
			fEnd = ( abs(top.iFwd) - 1 == iStart || abs(top.iFwd) - 1 == iCurrent) ? true : false;
			iStart = iCurrent;
			iCurrent = abs(top.iFwd)-1;
			crd = crdBuf[crdBuf.size() - 1];
			if ( fEnd) {
				rings.push_back(v);
			}

		} while(!fEnd);
		ILWIS::Polygon *polygon;
		void *val = &(valuebuffer[iPolygon * iValueSize]);
		if ( ptr.st() == stREAL )
		{
			polygon = new ILWIS::RPolygon();
			rVal = *((double *) val );
			polygon->PutVal(rVal);
		}
		else
		{
			polygon = new ILWIS::LPolygon();
			iVal = *((long *) val);
			polygon->PutVal(iVal);
		}
		polygon->addBoundary(new LinearRing(new CoordinateArraySequence(rings.at(0)),new GeometryFactory()));
		for(int i=1; i < rings.size(); ++i) {
			polygon->addHole(new LinearRing(new CoordinateArraySequence(rings.at(i)),new GeometryFactory()));
		}
		addPolygon(polygon);
	}
	

	delete [] sgbuffer;
	delete [] crdbuffer;
	delete SGFile;
	delete CDFile;
	delete filTop;
	delete filPol;
	delete filPolCode;
	//	fvFormatVersion = fvFORMAT30; // this changes the to the 3.0 format
	//	Updated();
}

void PolygonMapStoreFormat20::GetCoordBuf(short iSegIndex, CoordBuf& crdBuf,segtype *sgbuffer,crdtype *crdbuffer) {
	segtype st = sgbuffer[abs(iSegIndex)];
	int iStart = st.fstp;
	int iEnd = st.lstp;
	

	if ( iStart < 0 ) iStart += LONG_MAX;
	int iCount = iStart <= iEnd ? 1 : -1;
		int iCrdInSeg = 0;
	if ( iEnd == 0 && iStart == 0) // hmpff
		iCrdInSeg = 0;
	else
		iCrdInSeg = iEnd - iStart + 1;
	crdBuf.Size(iCrdInSeg + 2);
	crdBuf[0] = ToCoord(st.fst);
	
	int iCrd = 0;
	for( ; iCrd < iCrdInSeg; iCrd += iCount)
		crdBuf[ iCrd + 1 ] = ToCoord(crdbuffer[iStart + iCrd - 1]);
	crdBuf[ iCrd + 1] = ToCoord(st.lst);
	if ( iSegIndex < 0)
		CoordinateSequence::reverse(&crdBuf);
}



Coord PolygonMapStoreFormat20::ToCoord(const crdtype& crd)
{
	return Coord( crd.x * rAlfa1 + rBeta1, crd.y * rAlfa1 + rBeta2 );
}

void PolygonMapStoreFormat20::SetErase(bool f)
{
	File *filPol=NULL, *filPolCode=NULL, *filTop=NULL;
	FileName fnPolygon, fnTop, fnCode;

	if ( ptr.ReadElement("PolygonMapStore", "DataPol", fnPolygon))
		filPol = new File(fnPolygon);
	if ( ptr.ReadElement("PolygonMapStore", "DataTop", fnTop))
		filTop = new File(fnTop);
	if ( ptr.ReadElement("PolygonMapStore", "DataPolCode", fnCode))
		filPolCode = new File(fnCode);

	if (0 != filPol)
		filPol->SetErase(f);

	if (0 != filTop)
		filTop->SetErase(f);

	if (0 != filPolCode)
		filPolCode->SetErase(f);
} 
