#include "Headers\toolspch.h"
#include "Engine\Base\system\logger.h"
#include "Engine\Base\system\engine.h"
#include "Engine\Domain\dmcoord.h"
#include "Engine\Table\ColumnCoordBuf.h"
#include "Engine\Map\Polygon\PolygonStore30.h"

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
	for(int i = 1; i <= tblPolygon->iRecs(); ++i) {
		if ( colArea->rValue(i) <  0)
			continue;
		vector<LinearRing*> rings;
		if (getRings(colTopStart->iValue(i), tblTopology, tblPolygon, rings)) {
			ILWIS::Polygon *polygon;
			if ( ptr.dvrs().fValues()) {
				polygon = new ILWIS::RPolygon();	
			} else{
				polygon = new ILWIS::LPolygon();
			}
			if ( rings.size() == 0)
				continue;
			polygon->addBoundary(rings.at(0));
			CoordinateSequence *seq = rings.at(0)->getCoordinates();
			CoordBounds cb = ptr.cb();
			polygon->PutVal(colValue->iRaw(i));
				for(int j = 1; j < rings.size(); ++j) {
				polygon->addHole(rings[j]);
			}
			geometries->push_back(polygon);
		}

	}
	CalcBounds();
}

bool PolygonMapStoreFormat30::getRings(long startIndex, const Table& tblTopology, const Table& tblPolygon, vector<LinearRing*>& rings ){
	Column colFwl = tblTopology->col("ForwardLink");
	Column colBwl = tblTopology->col("BackwardLink");
	Column colCoords = tblTopology->col("Coords");
	ColumnCoordBuf *colCrdBuf = colCoords->pcbuf();
	long index = startIndex;

	CoordinateSequence *seq = new CoordinateArraySequence();
	vector<String> debug;
	bool forward = isForwardStartDirection(colFwl, colBwl, colCrdBuf,index);
	do{
		CoordinateSequence *line = colCrdBuf->iGetValue(abs(index));
		//long iFwl = colFwl->iRaw(abs(index));
		//long iBwl = colBwl->iRaw(abs(index));
		//debug.push_back(String("(%d 2.10%f,%2.10f) (%2.10f,%2.10f)  [%d/%d]",abs(index),line->front().x,line->front().y,line->back().x,line->back().y,iFwl,iBwl));
		if( seq->size() == 0 || seq->back() == line->front()){
			seq->add(line,false,true);
			delete line;
		} else if ( seq->back() == line->back()) {
			seq->add(line, false, false);
			delete line;
		} else if ( seq->front() == line->front()) {
			CoordinateSequence::reverse(line);
			line->add(seq,false,true);
			delete seq;
			seq = line;
		} else if ( seq->front() == line->back()) {
			line->add(seq,false,true);
			delete seq;
			seq = line;
		}
	
		if ( seq->front() == seq->back() && seq->size() > 3) {
			LinearRing *ring = new LinearRing(seq, new GeometryFactory());
			rings.push_back(ring);			
			seq = new CoordinateArraySequence();
		}
		int oldIndex = index;
		index = forward ?  colFwl->iRaw(abs(index)): colBwl->iRaw(abs(index));
		if ( oldIndex == index && index != startIndex) // this would indicate infintite loop. corrupt data
			return false;
		forward = index > 0;
	} while(abs(index) != abs(startIndex));
	delete seq;

	return true;
}

bool PolygonMapStoreFormat30::isForwardStartDirection(const Column& colFwl,const Column& colBwl, ColumnCoordBuf *colCrdBuf,long index) {
	long iFwl = colFwl->iRaw(abs(index));
	long iBwl = colBwl->iRaw(abs(index));
	if ( abs(iFwl) == abs(iBwl)	)
		return true;
	if ( index < 0)
		return false;
	CoordinateSequence *lineStart = colCrdBuf->iGetValue(abs(index));
	CoordinateSequence *lineFWL = colCrdBuf->iGetValue(abs(iFwl));
	bool forward = false;
	if ( iFwl > 0)
		forward =  lineStart->back() == lineFWL->front();
	else 
		forward = lineStart->back() == lineFWL->back();
	delete lineFWL;
	delete lineStart;

	return forward;

}

