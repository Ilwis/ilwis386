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
#include "Engine\Base\Algorithm\CachedRelation.h"

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

	Tranquilizer trq("Loading data");
	GeometryFactory * fact = new GeometryFactory();

	for(int iPolygon=0; iPolygon < iPol; ++iPolygon)
	{
		poltype pol = polbuf[iPolygon];
		std::vector<geos::geom::CoordinateSequence*> coords;
		if (getRings(pol.iSegStart, topbuf, polbuf, sgbuffer, crdbuffer, coords, fact)) {
			if ( coords.size() == 0)
				continue;
			autocorrectCoords(coords);
			std::vector<std::pair<geos::geom::LinearRing *, std::vector<geos::geom::Geometry *> *>> polys = makePolys(coords, fact);
			for (std::vector<std::pair<geos::geom::LinearRing *, vector<geos::geom::Geometry *> *>>::iterator poly = polys.begin(); poly != polys.end(); ++poly) {
				geos::geom::LinearRing * ring = poly->first;
				std::vector<geos::geom::Geometry *> * holes = poly->second;
				geos::geom::Polygon * gpol(fact->createPolygon(ring, holes)); // takes ownership of both ring and holes pointers
				ILWIS::Polygon *polygon;
				void *val = &(valuebuffer[iPolygon * iValueSize]);
				if ( ptr.dvrs().fUseReals()) {
					polygon = new ILWIS::RPolygon(spatialIndex,gpol);
					double rVal = *((double *) val );
					polygon->PutVal(rVal);
				} else{
					polygon = new ILWIS::LPolygon(spatialIndex,gpol);
					long iVal = *((long *) val);
					polygon->PutVal(iVal);
				}
				geometries->push_back(polygon);
			}
		}
		if ( iPolygon % 100 == 0) {
			trq.fUpdate(iPolygon, iPol); 
		}
	}
	trq.fUpdate(iPol, iPol);

	CalcBounds();

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

bool PolygonMapStoreFormat20::getRings(long startIndex, const toptype* topbuf, const poltype* polbuf, const segtype *sgbuffer, const crdtype *crdbuffer, vector<CoordinateSequence*>& coords, GeometryFactory * fact){
	long index = startIndex;

	CoordinateSequence *seq = new CoordinateArraySequence();
	do{
		CoordinateSequence *line = GetCoordinateSequence(abs(index)-1,sgbuffer,crdbuffer);
		if (line->size() > 0) {
			if( seq->size() == 0 || seq->back() == ((index > 0) ? line->front() : line->back())){
				seq->add(line,false,index > 0);
				delete line;
			} else if ( seq->front() == ((index > 0) ? line->back() : line->front())) {
				if (index < 0)
					CoordinateSequence::reverse(line);
				line->add(seq,false,true);
				delete seq;
				seq = line;
			} else {
				coords.push_back(seq);
				seq = new CoordinateArraySequence();
				seq->add(line,false,index > 0);
				delete line;
			}
			if (seq->front() == seq->back()) {
				if (seq->size() > 3)
					coords.push_back(seq);
				else
					delete seq;
				seq = new CoordinateArraySequence();
			}
		} else
			delete line;
		int oldIndex = index;
		toptype top = topbuf[abs(index)-1];
		index = (index > 0) ? top.iFwd : top.iBwd;
		if ( oldIndex == index && index != startIndex) // this would indicate infintite loop. corrupt data
			return false;
	} while(abs(index) != abs(startIndex) && index != iUNDEF);
	if (seq->size() > 0)
		coords.push_back(seq);
	else
		delete seq;

	return true;
}

CoordinateSequence * PolygonMapStoreFormat20::GetCoordinateSequence(short iSegIndex,const segtype *sgbuffer,const crdtype *crdbuffer) {
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
	CoordinateSequence *seq = new CoordinateArraySequence(iCrdInSeg + 2);
	seq->setAt(ToCoord(st.fst), 0);	
	int iCrd = 0;
	for( ; iCrd < iCrdInSeg; iCrd += iCount)
		seq->setAt(ToCoord(crdbuffer[iStart + iCrd - 1]), iCrd + 1);
	seq->setAt(ToCoord(st.lst), iCrd + 1);
	if ( iSegIndex < 0)
		CoordinateSequence::reverse(seq);
	return seq;
}

Coord PolygonMapStoreFormat20::ToCoord(const crdtype& crd)
{
	return Coord( crd.x * rAlfa1 + rBeta1, crd.y * rAlfa1 + rBeta2 );
}

bool PolygonMapStoreFormat20::appendCoords(geos::geom::CoordinateSequence* & coordsA, geos::geom::CoordinateSequence & coordsB, bool fForward) const
{
	if (fForward ? (coordsA->back() == coordsB.front()) : (coordsA->back() == coordsB.back())) {
		coordsA->add(&coordsB, false, fForward);
		return true;
	} else
		return false;
}

void PolygonMapStoreFormat20::autocorrectCoords(std::vector<geos::geom::CoordinateSequence*> & coords) const
{
	bool fChanged;
	std::vector<long> openCoords;
	for (long i = 0; i < coords.size(); ++i) { // mark all open coordinatesequences (back != front)
		if (coords[i]->back() != coords[i]->front())
			openCoords.push_back(i);
	}
	do { // link all possible backs to fronts, creating closed coordinatesequences that can become polygons
		fChanged = false;
		for (long i = 0; i < openCoords.size(); ++i) {
			for (long j = i + 1; j < openCoords.size(); ++j) {
				if (appendCoords(coords[openCoords[i]], *coords[openCoords[j]], true)) {
					delete coords[openCoords[j]];
					coords.erase(coords.begin() + openCoords[j]);
					openCoords.erase(openCoords.begin() + j);
					for (long k = j; k < openCoords.size(); ++k) // shift all indexes
						openCoords[k] = openCoords[k] - 1;
					j = j - 1;
					fChanged = true;
				}
			}
			if (coords[openCoords[i]]->back() == coords[openCoords[i]]->front()) {
				openCoords.erase(openCoords.begin() + i);
				i = i - 1;
			}
		}
	} while (fChanged);
	for (long i = 0; i < openCoords.size(); ++i) { // delete all remaining open coordinatesequences
		delete coords[openCoords[i]];
		coords.erase(coords.begin() + openCoords[i]);
		for (long k = i; k < openCoords.size(); ++k) // shift all indexes
			openCoords[k] = openCoords[k] - 1;
	}
}

std::vector<std::pair<geos::geom::LinearRing *, vector<geos::geom::Geometry *> *>> PolygonMapStoreFormat20::makePolys(std::vector<geos::geom::CoordinateSequence*> & coords, GeometryFactory * fact) const
{
	std::vector<std::pair<geos::geom::LinearRing *, vector<geos::geom::Geometry *> *>> result;
	CachedRelation relation;

	// CoordSequence to Polygons
	std::vector<geos::geom::Polygon*> rings;
	for (std::vector<geos::geom::CoordinateSequence*>::iterator coordsN = coords.begin(); coordsN != coords.end(); ++coordsN)
		rings.push_back(fact->createPolygon(new geos::geom::LinearRing(*coordsN, fact), 0));

	std::multimap<long, geos::geom::Polygon*> levels;
	long iMaxDepth = 0;
	
	// find the winding level of each ring
	for (long i = 0; i < rings.size(); ++i) {
		geos::geom::Polygon * ringI = rings[i];
		long iDepth = 0;
		for (long j = 0; j < rings.size(); ++j) {
			if (j == i) // test against every other ring, except itself
				continue;
			geos::geom::Polygon * ringJ = rings[j];
			if (relation.within(ringI, ringJ))
				++iDepth;
		}
		levels.insert(std::pair<long, geos::geom::Polygon*>(iDepth, ringI));
		iMaxDepth = max(iMaxDepth, iDepth);
	}

	// go through all exterior rings and collect their holes; EVEN depths become exterior, ODD depths become holes, each hole is added only to its closest exterior ring
	for (long iDepth = 0; iDepth <= iMaxDepth; iDepth += 2) {
		pair<multimap<long, geos::geom::Polygon *>::iterator, multimap<long, geos::geom::Polygon *>::iterator> exteriors = levels.equal_range(iDepth);
		pair<multimap<long, geos::geom::Polygon *>::iterator, multimap<long, geos::geom::Polygon *>::iterator> holes = levels.equal_range(iDepth + 1);
		for (multimap<long, geos::geom::Polygon *>::iterator exteriorIt = exteriors.first; exteriorIt != exteriors.second; ++exteriorIt) {
			std::vector<geos::geom::Geometry*> * ringHoles = new std::vector<geos::geom::Geometry*>();
			for (multimap<long, geos::geom::Polygon *>::iterator holeIt = holes.first; holeIt != holes.second; ++holeIt) {
				if (relation.within(holeIt->second, exteriorIt->second))
					ringHoles->push_back((Geometry*)holeIt->second->getExteriorRing());
			}
			result.push_back(std::pair<geos::geom::LinearRing *, vector<geos::geom::Geometry *> *>((LinearRing*)exteriorIt->second->getExteriorRing(), ringHoles));
		}
	}

	return result;
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
