///////////////////////////////////////////////////////////
//  PDatum.cpp
//  Implementation of the Class PDatum
//  Created on:      19-Sep-2011 1:29:10 PM
//  Original author: Martin Schouwenburg
///////////////////////////////////////////////////////////

#include "PDatum.h"
#include "PEllipsoid.h"


PDatum::PDatum() : datum(0) {

}

PDatum::PDatum(const string& name, const string& area) {
	datum = new MolodenskyDatum(name, area);
}

PDatum::PDatum(const string& ellipsoid, double dx, double dy, double dz){
	PEllipsoid ell(ellipsoid);
	datum = new MolodenskyDatum(*ell.getIlwisEllipsoid(),dx, dy, dz);

}

PDatum::PDatum(const string& ellipsoid, double dx, double dy, double dz, double rotX, double rotY, double rotZ, double dS){
	PEllipsoid ell(ellipsoid);
	datum = new BursaWolfDatum(*ell.getIlwisEllipsoid(),dx,dy,dz,rotX, rotY, rotZ, dS);
}

PDatum::PDatum(const string& ellipsoid, double dx, double dy, double dz, double rotX, double rotY, double rotZ, double dS,double X0, double Y0, double Z0){
	PEllipsoid ell(ellipsoid);
	datum = new BadekasDatum(*ell.getIlwisEllipsoid(),dx,dy,dz,rotX, rotY, rotZ, dS, X0, Y0, Z0);
}

PDatum::PDatum( Datum& dat) {
	string n = dat.sName();
	datum = new MolodenskyDatum(n, dat.ell.sName);
}


PDatum::~PDatum(){
	delete datum;
}

PLatLon PDatum::fromWGS84(const PLatLon& ll){
	if ( isValid()) {
		PLatLon latlon(datum->llFromWGS84(ll.getLatLon()));
		return latlon;
	}
	return PLatLon();
}

PLatLon PDatum::toWGS84(const PLatLon& ll){
	if ( isValid()) {
		PLatLon latlon(datum->llToWGS84(ll.getLatLon()));
		return latlon;
	}
	return PLatLon();
}

bool PDatum::isValid() const {
	return datum != 0;
}

bool PDatum::isEqual(const PDatum& dat) const{
	if ( isValid())
		return datum->fEqual(dat.getDatum());
	return false;
}

Datum *PDatum::getDatum() const{
	return datum;
}

int PDatum::getEPSG() const {
	if ( isValid()) {
		String id = datum->getIdentification();
		if ( id != "")
			return id.sTail(":").iVal();
	}
	return iUNDEF;
}

string PDatum::getName() const{
	if ( isValid())
		return datum->sName();
	return "?";
}

string PDatum::getArea() const{
	if ( isValid())
		return datum->sArea;
	return "?";
}

PEllipsoid PDatum::getEllipsoid() const {
	if ( isValid()) {
		return PEllipsoid(datum->ell.sName);
	}
	return PEllipsoid();
}

