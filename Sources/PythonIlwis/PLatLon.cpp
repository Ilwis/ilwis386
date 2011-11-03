///////////////////////////////////////////////////////////
//  PCoordinate.cpp
//  Implementation of the Class PLatLon
//  Created on:      19-Sep-2011 1:29:09 PM
//  Original author: Martin Schouwenburg
///////////////////////////////////////////////////////////

#include "PLatLon.h"

PLatLon::PLatLon(){
}

PLatLon::~PLatLon(){
}


string PLatLon::getLat(long precision) const{
	LatLon ll(getY(), getX());
	return ll.sLat(precision);

}

string PLatLon::getLon(long precision) const{
	LatLon ll(getY(), getX());
	return ll.sLon(precision);
}

string PLatLon::toString(long precision) const{
	LatLon ll(getY(), getX());
	return ll.sValue(precision);
}

void PLatLon::setLat(const string& lat){
	setY(parseLatLon(lat));
}

void PLatLon::setLon(const string& lon){
	setX(parseLatLon(lon));
}

bool PLatLon::isValid() const {
	return getX() <= 180 && getX() >= -180 && getY() >= -90 && getY() <= 90;
}

//other
PLatLon::PLatLon(const LatLon& crd){
}
double PLatLon::getPhi() const    { 
	return M_PI / 180 * getY(); 
}

double PLatLon::getLambda() const { 
	return M_PI / 180 * getX(); 
}

void PLatLon::setPhi(double phi) { 
	setY( phi * 180 / M_PI ); 
}

void PLatLon::setLambda(double lambda) { 
	setX( lambda * 180 / M_PI ); 
}

double PLatLon::parseLatLon(const String& ll){
	double d,m,s;
	d = rUNDEF;
	m = s = 0;
	String deg = ll.sHead("°");
	bool version1 = deg != "";
	if ( version1) {
		d = deg.rVal();
		String rest = ll.sTail("°");
		String min = rest.sHead("\'");
		if ( min != "")
			m = min.rVal();
		rest = rest.sTail("\'");
		if ( rest != "")
			s = rest.rVal();
	} else {
		Array<String> parts;
		Split(ll, parts," ");
		if ( parts.size() >= 1)
			d = parts[0].rVal();
		if ( parts.size() >= 2)
			m = parts[1].rVal();
		if ( parts.size() >= 3)
			s = parts[3].rVal();
	}
	if ( d == rUNDEF || m == rUNDEF || s == rUNDEF)
		return rUNDEF;

	return d + m / 60.0 + s / 3600.0;
}

LatLon PLatLon::getLatLon() const{
	LatLon ll(getY(), getX());
	return ll;
}