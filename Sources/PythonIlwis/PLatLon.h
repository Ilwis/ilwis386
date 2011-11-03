///////////////////////////////////////////////////////////
//  PLatLon.h
//  Implementation of the Class PLatLon
//  Created on:      19-Sep-2011 1:29:09 PM
//  Original author: Martin Schouwenburg
///////////////////////////////////////////////////////////

#pragma once

#include "PCoordinate.h"

class PLatLon : public PCoordinate
{

public:
	//python interface
	PLatLon();
	virtual ~PLatLon();

	std::string getLat(long precision=0) const;
	std::string getLon(long precision=0) const;
	void setLat(const std::string& lat);
	void setLon(const std::string& lon);
	std::string toString(long precision=0) const;
	virtual bool isValid() const;
	double getPhi() const;
	double getLambda() const;
	void setPhi(double phi);
	void setLambda(double lambda);

	//other
	PLatLon(const LatLon& crd);
	LatLon getLatLon() const;

private:
	double parseLatLon(const String& ll);



};