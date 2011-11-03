///////////////////////////////////////////////////////////
//  PCoordinateSystem.cpp
//  Implementation of the Class PCoordinateSystem
//  Created on:      19-Sep-2011 1:29:09 PM
//  Original author: Martin Schouwenburg
///////////////////////////////////////////////////////////

#include "engine\spatialreference\coordsys.h"
#include "Engine\SpatialReference\Ellips.h"
#include "PCoordinateSystem.h"

using namespace std;

PCoordinateSystem::PCoordinateSystem() {
}

PCoordinateSystem::PCoordinateSystem(const string& name, bool setWorkingDir ){
	FileName fn(name);
	if ( fn.sExt == "")
		fn.sExt = ".csy";
	csy = CoordSystem(fn);
}

PCoordinateSystem::~PCoordinateSystem(){

}

PCRSBoundingBox PCoordinateSystem::convertBB(const PCoordinateSystem& sourceCsy, const PCRSBoundingBox& bb) const{
	if ( !isValid() || !sourceCsy.isValid())
		return PCRSBoundingBox();
	
	CoordBounds cb = csy->cbConv(sourceCsy.getIlwisCsy(),bb.toCoordBounds());

	return PCRSBoundingBox(cb);
}

PCoordinate PCoordinateSystem::convertCrd(const PCoordinateSystem& sourceCsy, const PCoordinate& crd) const{
	if ( !csy.fValid() || !sourceCsy.isValid())
		return PCoordinate();
	Coord crdNew = csy->cConv(sourceCsy.getIlwisCsy(), Coord(crd.getX(), crd.getY()));
	return PCoordinate(crdNew);
}

bool PCoordinateSystem::isProjected() const{
	if ( csy.fValid()) {
		return csy->pcsProjection() != 0;
	}
	return false;
}

bool PCoordinateSystem::isValid() const {
	return csy.fValid();
}

CoordSystem PCoordinateSystem::getIlwisCsy() const{
	return csy;
}

PDatum PCoordinateSystem::getDatum() const{
	if ( getIlwisCsy()->pcsViaLatLon()) {
		PDatum datum(*(csy->pcsProjection()->datum));

		return datum;
	}
	return PDatum();
}

void PCoordinateSystem::setDatum(const PDatum& dat) {
	if ( getIlwisCsy()->pcsViaLatLon()) {
		getIlwisCsy()->pcsViaLatLon()->datum = new MolodenskyDatum(dat.getName(), dat.getArea());
		getIlwisCsy()->pcsViaLatLon()->ell = *(dat.getEllipsoid().getIlwisEllipsoid());

	}
}

PEllipsoid PCoordinateSystem::getEllipsoid() const{
	if ( getIlwisCsy()->pcsViaLatLon() ) {
		PEllipsoid ell(csy->pcsProjection()->ell.sName);

		return ell;
	}
	return PEllipsoid();
}

PProjection PCoordinateSystem::getProjection() const {
	if ( isProjected()) {
		PProjection prj(getIlwisCsy()->pcsProjection()->prj->sName());
		return prj;
	}

	return PProjection();
}

void PCoordinateSystem::setProjection(const PProjection& proj) {
	if ( !isValid() || !isProjected()) {
		//csy = CoordSystem();
		//csy.SetPointer(new CoordSystemProjection(
	}
}

string PCoordinateSystem::getName() const{

	if ( isValid())
		return csy->fnObj.sFile;

	return  "?";
}