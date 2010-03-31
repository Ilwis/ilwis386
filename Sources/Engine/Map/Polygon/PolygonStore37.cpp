#include "Headers\toolspch.h"
#include "Engine\Domain\dmcoord.h"
#include "Engine\Map\Polygon\PolygonStore37.h"

PolygonMapStoreFormat37::PolygonMapStoreFormat37(const FileName& fn, PolygonMapPtr& p, bool fCreate) :
	PolygonMapStore(fn, p,fCreate)
{
	if ( !fCreate)
		Load();
}

void PolygonMapStoreFormat37::Load()
{
	long iPols;
	ptr.ReadElement("PolygonMapStore", "Polygons", iPols);
	FileName fnData(ptr.fnObj,".mpz#");
	File  binaryFile(fnData,facRO);
	for(int j=0; j < iPols; ++j) {
		ILWIS::Polygon *pol = CPOLYGON(newFeature());
		pol->addBoundary(ReadRing(binaryFile));
		double value;
		binaryFile.Read(8,&value);
		pol->PutVal(value);
		long numberOfHoles;
		binaryFile.Read(4,&numberOfHoles);
		for(int i=0; i< numberOfHoles;++i)
			pol->addHole(ReadRing(binaryFile));
	}
	CalcBounds();
}

LinearRing *PolygonMapStoreFormat37::ReadRing(File&  binaryFile) {
	long numberOfCoords;
	if (binaryFile.Read(4,&numberOfCoords) <= 0)
		throw ErrorObject(String("Can't read from %S",ptr.fnObj.sRelative()));
	Coordinate *p = new Coordinate[numberOfCoords];
	binaryFile.Read(numberOfCoords*3*8,(void *)p);
	vector<Coordinate> *z = new vector<Coordinate>();
	z->assign(p, p+numberOfCoords);
	CoordinateSequence *seq = new CoordinateArraySequence(z);
	LinearRing *ring = new LinearRing(seq,new GeometryFactory());
	delete [] p;
	return ring;
}