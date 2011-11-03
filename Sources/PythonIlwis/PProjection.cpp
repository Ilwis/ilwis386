///////////////////////////////////////////////////////////
//  PProjection.cpp
//  Implementation of the Class PProjection
//  Created on:      19-Sep-2011 1:29:11 PM
//  Original author: Martin Schouwenburg
///////////////////////////////////////////////////////////

#include "PProjection.h"


PProjection::PProjection(){
}

PProjection::PProjection(const string& name) {
	FileName fn(name);
	if ( fn.fExist()) {
		prj = Projection(fn);
	} else {
		prj = Projection(name);
	}
}

PProjection::PProjection(const std::string& name, const std::string ellipsoid){
	FileName fn(name);
	if ( fn.fExist()) {
		prj = Projection(fn, Ellipsoid(ellipsoid));
	} else {
		prj = Projection(name, Ellipsoid(ellipsoid));
	}
}

PProjection::~PProjection(){

}

double PProjection::getProjectionParameter(ProjectionParameter id) const {

	if ( isValid()) {
		ProjectionParamValue pv = mapProjectionParameterToIlwis(id);
		if ( pv ==  pvNORTH || pv ==  pvNORIENTED || pv == pvZONE || pv == pvTILTED)
			return prj->iParam(pv);
		else
			return prj->rParam(pv);
	}

	return rUNDEF;
}

void PProjection::setProjecttionParameter(ProjectionParameter id, double v){
	if ( isValid()) {
		ProjectionParamValue pv = mapProjectionParameterToIlwis(id);
		prj->Param(pv, v);
	}
}

int PProjection::getEPSG() const{
	if ( isValid()) {
		String id = prj->getIdentification();
		if ( id != "")
			return id.sTail(":").iVal();
	}
	return iUNDEF;
}

string PProjection::getName() const{
	if ( isValid())
		return prj->sName();
	return "?";
}

bool PProjection::isEqual(const PProjection& proj) const{
	if ( isValid() && proj.isValid()) {
		return prj.fEqual(proj.getIlwisProjection());
	}
	return false;
}

bool PProjection::isValid() const{
	return prj.fValid();
}

PEllipsoid PProjection::getEllipsoid() const {
	if ( isValid()) {
		PEllipsoid ell(prj->ell.sName);
		return ell;
	}
	return PEllipsoid();
}

PProjection::ProjectionParameter PProjection::mapProjectionParamValueToPythonEnum(ProjectionParamValue pv) {
 switch (pv) {
    case pvX0:        return ppFalseEasting;
    case pvY0:        return ppFalsNorthing;
    case pvLON0:      return ppCentralMeridian;
    case pvLATTS:     return ppLatitudeOfTrueScale;
    case pvLAT0:      return ppCentralParallel;
    case pvLAT1:      return ppStandardParallel_1;
    case pvLAT2:      return ppStandardParallel_2;
    case pvK0:        return ppScale;
    case pvNORTH:     return ppNorthernHemisphere;
    case pvZONE:      return ppZone;
    case pvHEIGHT:    return ppHeight;
    case pvTILTED:    return ppTiltedProjPlane;
    case pvTILT:      return ppTiltOfPlane;
    case pvAZIMYAXIS: return ppAzimuthYAxis;
    case pvAZIMCLINE: return ppAzimuthCentralLine;
    case pvPOLE:      return ppPoleObliqueCylinder;
    case pvNORIENTED: return ppNorthOriented;
    default:          return ppUnknown;
  } 
}

ProjectionParamValue  PProjection::mapProjectionParameterToIlwis(PProjection::ProjectionParameter pp) const{
	switch (pp) {
		 case ppFalseEasting:			return pvX0;       
		 case ppFalsNorthing:			return pvY0;       
		 case ppCentralMeridian:		return pvLON0;     
		 case ppLatitudeOfTrueScale:    return pvLATTS;    
		 case ppCentralParallel:		return pvLAT0;     
		 case ppStandardParallel_1:		return pvLAT1;     
		 case ppStandardParallel_2:		return pvLAT2;     
		 case ppScale:					return pvK0;       
		 case ppNorthernHemisphere:		return pvNORTH;    
		 case ppZone:					return pvZONE;     
		 case ppHeight:					return pvHEIGHT;   
		 case ppTiltedProjPlane:		return pvTILTED;   
		 case ppTiltOfPlane:			return pvTILT;     
		 case ppAzimuthYAxis:			return pvAZIMYAXIS;
		 case ppAzimuthCentralLine:		return pvAZIMCLINE;
		 case ppPoleObliqueCylinder:    return pvPOLE;     
		 case ppNorthOriented:			return pvNORIENTED;
		 default:						return pvNONE;         
  } 
}

Projection PProjection::getIlwisProjection() const{
	return prj;
}
