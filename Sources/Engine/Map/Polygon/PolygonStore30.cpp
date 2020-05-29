#include "Headers\toolspch.h"
#include "Engine\Base\system\logger.h"
#include "Engine\Base\system\engine.h"
#include "Engine\Domain\dmcoord.h"
#include "Engine\Table\ColumnCoordBuf.h"
#include "Engine\Map\Polygon\PolygonStore30.h"
#include "Engine\Base\Algorithm\CachedRelation.h"

PolygonMapStoreFormat30::PolygonMapStoreFormat30(const FileName& fn, PolygonMapPtr& p) :
	PolygonMapStore(fn, p, true)
{
	Load();
}

void PolygonMapStoreFormat30::Load()
{
	Table tblPolygon;
	Table tblTopology;
	tblPolygon.SetPointer(new TablePtr(ptr.fnObj, ""));
	tblTopology.SetPointer(new TablePtr(ptr.fnObj,"top:"));
	tblPolygon->fUpdateCatalog = false; // was set by constructor
	tblPolygon->fErase = false;

	//SetColumns();
	tblPolygon->Load();
	tblTopology->Load();
	Column colValue = tblPolygon->col("PolygonValue");
	Column colTopStart = tblPolygon->col("TopStart");
	Column colArea = tblPolygon->col("Area");
	Column colDeleted = tblPolygon->col("Deleted");
	Tranquilizer trq("Loading data");
	GeometryFactory * fact = new GeometryFactory();
	long iPol = tblPolygon->iRecs();
	for(int i = 1; i <= iPol; ++i) {
		if ( colArea->rValue(i) <  0)
			continue;
		std::vector<geos::geom::CoordinateSequence*> coords;
		if (getRings(colTopStart->iValue(i), tblTopology, tblPolygon, coords, fact)) {
			if ( coords.size() == 0)
				continue;
			autocorrectCoords(coords);
			std::vector<std::pair<geos::geom::LinearRing *, std::vector<geos::geom::Geometry *> *>> polys = makePolys(coords, fact);
			for (std::vector<std::pair<geos::geom::LinearRing *, vector<geos::geom::Geometry *> *>>::iterator poly = polys.begin(); poly != polys.end(); ++poly) {
				geos::geom::LinearRing * ring = poly->first;
				std::vector<geos::geom::Geometry *> * holes = poly->second;
				geos::geom::Polygon * gpol(fact->createPolygon(ring, holes)); // takes ownership of both ring and holes pointers
				ILWIS::Polygon *polygon;
				if ( ptr.dvrs().fUseReals()) {
					polygon = new ILWIS::RPolygon(spatialIndex,gpol);
				} else{
					polygon = new ILWIS::LPolygon(spatialIndex,gpol);
				}
				polygon->PutVal(colValue->iRaw(i));
				if (colDeleted.fValid())
					polygon->Delete(colDeleted->iValue(i) != 0);
				geometries->push_back(polygon);
			}
		}
		if ( i % 100 == 0) {
			trq.fUpdate(i, iPol); 
		}
	}
	trq.fUpdate(iPol, iPol);

	CalcBounds();
}

bool PolygonMapStoreFormat30::getRings(long startIndex, const Table& tblTopology, const Table& tblPolygon, vector<CoordinateSequence*>& coords, GeometryFactory * fact){
	Column colFwl = tblTopology->col("ForwardLink");
	Column colBwl = tblTopology->col("BackwardLink");
	Column colCoords = tblTopology->col("Coords");
	ColumnCoordBuf *colCrdBuf = colCoords->pcbuf();
	long index = startIndex;

	CoordinateSequence *seq = new CoordinateArraySequence();
	do{
		CoordinateSequence *line = colCrdBuf->iGetValue(abs(index));
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
		index = (index > 0) ? colFwl->iRaw(index) : colBwl->iRaw(-index);
		if ( oldIndex == index && index != startIndex) // this would indicate infintite loop. corrupt data
			return false;
	} while(abs(index) != abs(startIndex) && index != iUNDEF);
	if (seq->size() > 0)
		coords.push_back(seq);
	else
		delete seq;

	return true;
}

bool PolygonMapStoreFormat30::appendCoords(geos::geom::CoordinateSequence* & coordsA, geos::geom::CoordinateSequence & coordsB, bool fForward) const
{
	if (fForward ? (coordsA->back() == coordsB.front()) : (coordsA->back() == coordsB.back())) {
		coordsA->add(&coordsB, false, fForward);
		return true;
	} else
		return false;
}

void PolygonMapStoreFormat30::autocorrectCoords(std::vector<geos::geom::CoordinateSequence*> & coords) const
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

std::vector<std::pair<geos::geom::LinearRing *, vector<geos::geom::Geometry *> *>> PolygonMapStoreFormat30::makePolys(std::vector<geos::geom::CoordinateSequence*> & coords, GeometryFactory * fact) const
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
