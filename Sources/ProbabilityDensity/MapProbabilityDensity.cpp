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

 Created on: 2014-05-02
 ***************************************************************/

#include "ProbabilityDensity\MapProbabilityDensity.H"
#include "Engine\SpatialReference\Grcornrs.h"
#include "Engine\SpatialReference\Csproj.h"

IlwisObjectPtr * createMapProbabilityDensity(const FileName& fn, IlwisObjectPtr& ptr, const String& sExpr, vector<void *> parms ) {
	if ( sExpr != "")
		return (IlwisObjectPtr *)MapProbabilityDensity::create(fn, (MapPtr &)ptr, sExpr);
	else
		return (IlwisObjectPtr *)new MapProbabilityDensity(fn, (MapPtr &)ptr);
}

const char* MapProbabilityDensity::sSyntax() {
  return "MapProbabilityDensity(crdx, crdy, distance, sigma_distance, direction, sigma_direction)";
}

const double MapProbabilityDensity::scalewgs = 1.0/(60.0*8.0); // 1/8 arcminute 

MapProbabilityDensity* MapProbabilityDensity::create(const FileName& fn, MapPtr& p, const String& sExpr)
{
  Array<String> as;
  int iParms = IlwisObjectPtr::iParseParm(sExpr, as);
  if (iParms != 6)
    ExpressionError(sExpr, sSyntax());
  Coord crdwgs(as[0].rVal(), as[1].rVal());
  double distance(as[2].rVal());
  double sigma_distance(as[3].rVal());
  double direction(as[4].rVal());
  double sigma_direction(as[5].rVal());
  GeoRef gr = CreateGeoRef(fn, crdwgs, distance, sigma_distance, direction, sigma_direction);
  return new MapProbabilityDensity(fn, p, gr, crdwgs, distance, sigma_distance, direction, sigma_direction);
}

MapProbabilityDensity::MapProbabilityDensity(const FileName& fn, MapPtr& p)
: MapVirtual(fn, p)
{
  ReadElement("MapProbabilityDensity", "Coordinate", crdwgs);
  ReadElement("MapProbabilityDensity", "Distance", distance);
  ReadElement("MapProbabilityDensity", "Sigma_Distance", sigma_distance);
  ReadElement("MapProbabilityDensity", "Direction", direction);
  ReadElement("MapProbabilityDensity", "Sigma_Direction", sigma_direction);

  fNeedFreeze = true;
  sFreezeTitle = "MapProbabilityDensity";
}

MapProbabilityDensity::MapProbabilityDensity(const FileName& fn, MapPtr& p, const GeoRef& gr, Coord & _crdwgs, double _distance, double _sigma_distance, double _direction, double _sigma_direction)
: MapVirtual(fn, p, gr, gr->rcSize(), ValueRange(ILWIS::TimeInterval(ILWIS::Time((double)0.0), ILWIS::Time((double)10000.0), 0)))
, crdwgs(_crdwgs)
, distance(_distance)
, sigma_distance(_sigma_distance)
, direction(_direction)
, sigma_direction(_sigma_direction)
{
  if (distance<=0.0 || sigma_distance<=0.0 || direction<0.0 || direction>M_PI * 2.0 || sigma_direction<0.0)
    throw ErrorObject(TR("Given distance, direction or their deviation values are not acceptable."));

  fNeedFreeze = true;
  if (!fnObj.fValid())
    objtime = objdep.tmNewest();
  sFreezeTitle = "MapProbabilityDensity";
  ptr.Store(); // store domain and att table info
  Store();
}

GeoRef MapProbabilityDensity::CreateGeoRef(const FileName & fnObj, const Coord & crdwgs, double distance, double sigma_distance, double direction, double sigma_direction)
{
  CoordSystem csyUtm = pay_utmzone_from_point(crdwgs);
  CoordSystem csyWgs ("LatLonWGS84");
  const double z = 3.4807; // This z value is such that for cell values F(-z,z)<0.00025 i.e. one fourth of error margin 0.1%. Here, F(-3.4807 3.4807)= 0.00024999
  Coord pointutm  = csyUtm->cConv(csyWgs, crdwgs);
  CoordBounds boxutm;
  double r1 = distance + sigma_distance*z;
  double azi1 = direction - abs(z * sigma_direction);
  double azi2 = direction + abs(z * sigma_direction);

  if (distance > sigma_distance * z) {
	double r2 = distance - sigma_distance*z;
	boxutm += Coord(pointutm.x + r1 * cos(M_PI_2 - azi1), pointutm.y + r1 * sin(M_PI_2 - azi1)); // add the 4 extremes of the doughnut-slice
	boxutm += Coord(pointutm.x + r1 * cos(M_PI_2 - azi2), pointutm.y + r1 * sin(M_PI_2 - azi2));
	boxutm += Coord(pointutm.x + r2 * cos(M_PI_2 - azi1), pointutm.y + r2 * sin(M_PI_2 - azi1));
	boxutm += Coord(pointutm.x + r2 * cos(M_PI_2 - azi2), pointutm.y + r2 * sin(M_PI_2 - azi2));
	for (int i = -3; i <= 7; ++i) { // add the remaining extremes of the circle (north, south, east, west) if they're part of the doughnut-slice; assume unnormalized angles
		double azi = i * M_PI_2;
		if ((azi1 < azi) && (azi < azi2)) {
			boxutm += Coord(pointutm.x + r1 * cos(M_PI_2 - azi), pointutm.y + r1 * sin(M_PI_2 - azi));
			boxutm += Coord(pointutm.x + r2 * cos(M_PI_2 - azi), pointutm.y + r2 * sin(M_PI_2 - azi));
		}
	}
  } else {
	boxutm += pointutm; // add the point itself
	boxutm += Coord(pointutm.x + r1 * cos(M_PI_2 - azi1), pointutm.y + r1 * sin(M_PI_2 - azi1)); // add the two extremes of the doughnut-slice
	boxutm += Coord(pointutm.x + r1 * cos(M_PI_2 - azi2), pointutm.y + r1 * sin(M_PI_2 - azi2));
	for (int i = -3; i <= 7; ++i) { // add the remaining extremes of the circle (north, south, east, west) if they're part of the doughnut-slice; assume unnormalized angles
		double azi = i * M_PI_2;
		if ((azi1 < azi) && (azi < azi2)) {
			boxutm += Coord(pointutm.x + r1 * cos(M_PI_2 - azi), pointutm.y + r1 * sin(M_PI_2 - azi));
		}
	}
  }

  CoordBounds boxwgs = csyWgs->cbConv(csyUtm, boxutm);

  // LOWER LEFT and UPPER RIGHT point of the future raster
  double lowerleftx = floor(boxwgs.MinX()/scalewgs)*scalewgs;
  double lowerlefty = floor(boxwgs.MinY()/scalewgs)*scalewgs;
  double upperrightx = ceil(boxwgs.MaxX()/scalewgs)*scalewgs;
  double upperrighty = ceil(boxwgs.MaxY()/scalewgs)*scalewgs;

  // DETERMINE width and height of needed raster
  int boxwidth = round((upperrightx - lowerleftx) / scalewgs);
  int boxheight = round((upperrighty - lowerlefty) / scalewgs);
  RowCol rcSize (boxheight, boxwidth);

  FileName fnGeoRef(fnObj, ".grf");
  GeoRef gr;
  gr.SetPointer(new GeoRefCorners(fnGeoRef, csyWgs, rcSize, true, Coord(lowerleftx, lowerlefty), Coord(upperrightx, upperrighty)));
  return gr;
}

void MapProbabilityDensity::Store()
{
  MapVirtual::Store();
  WriteElement("MapVirtual", "Type", "MapProbabilityDensity");
  WriteElement("MapProbabilityDensity", "Coordinate", crdwgs);
  WriteElement("MapProbabilityDensity", "Distance", distance);
  WriteElement("MapProbabilityDensity", "Sigma_Distance", sigma_distance);
  WriteElement("MapProbabilityDensity", "Direction", direction);
  WriteElement("MapProbabilityDensity", "Sigma_Direction", sigma_direction);
}

MapProbabilityDensity::~MapProbabilityDensity()
{
}

CoordSystem MapProbabilityDensity::pay_utmzone_from_point(const Coord & p) {
  // At present, given point p must have srid=4326 (WGS84)
  // SRIDs 32601 up to 32660 (32701 up to 32760) are the standard for UTM zones 1 up to 60 North (South)
  // This function is meant to be used with a point as argument, but will work for an arbitrary geometry,
  // as long as at least one point is in it.
	bool fNorth = true;
	int utmzone = 0;
    if (p.y < 0.0) // we are in a southern zone
		fNorth = false;
	else
		fNorth = true;
    if (p.x == -180.0) // special borderline case      
      ++utmzone;
	else
      utmzone += ceil(((p.x)+180.0)/6.0) ;

	String sUTM ("UTM%d%s.csy", utmzone, fNorth ? "N" : "S");
	FileName fnUTM = FileName::fnUnique(sUTM); 
	CoordSystemProjection * csprUTM = new CoordSystemProjection(fnUTM, 1);
	csprUTM->datum = new MolodenskyDatum("WGS 1984","");
	csprUTM->ell = csprUTM->datum->ell;

	Projection prj = Projection(String("UTM"), csprUTM->ell);

	prj->Param(pvZONE, (long)utmzone);
	prj->Param(pvNORTH, (long)(fNorth ? 1 : 0));

	prj->Prepare();
	csprUTM->prj = prj;

	CoordSystem csyUtm;
	csyUtm.SetPointer(csprUTM);
	csyUtm->fErase = true; // only for local calculation, no permanent store

    return csyUtm;
}

double MapProbabilityDensity::pay_modified_besseli_zero(double x) {
  if (abs(x) < 3.75) {
    // In the range -3.75<=x<=3.75  the changing pattern of the curve is same and can approximated by the condition first.
    double y = x*x/(3.75*3.75);
    return 1.0+y*(3.5156229+y*(3.0899424+y*(1.2067492+y*(0.2659732+y*(0.0360768+y*0.0045813)))));
  } else {
    double y =3.75/abs(x);
    // This condition is valid for the range 3.75 <=x< infinity
    return (exp(abs(x))/sqrt(abs(x)))*(0.39894228+y*(0.01328592+y*(0.00225319+y*(-0.00157565+y*(0.00916281+y*(-0.02057706+y*(0.02635537+y*(-0.01647633+y*0.00392377))))))));
  }
}

double MapProbabilityDensity::pay_modified_besseli_one(double x) {
  double result;
  if (abs(x)<3.75) {
    // In the range -3.75<=x<=3.75  the changing pattern of the curve is same and can approximated by the condition first.
    double y = x*x/(3.75*3.75);
    result = abs(x)*(0.5+y*(0.87890594+y*(0.51498869+y*(0.15084934+y*(0.02658733+y*(0.00301532+y*0.00032411))))));
  } else {
    double y =3.75/abs(x);  // This condition is valid for the range 3.75 <=x< infinity
    result = (exp(abs(x))/sqrt(abs(x)))*(0.39894228+y*(-0.03988024+y*(-0.00362018+y*(0.00163801+y*(-0.01031555+y*(0.02282967+y*(-0.02895312+y*(0.01787654-y*0.00420059))))))));
  }
  if (x<0.0)
	result = -result;
  return result;
}

double MapProbabilityDensity::pay_kappa_estimation(double sigma_input)
{
// Here, I consider a function f(x)= sigma^2-(1-(I1(k)/I0(k))). where sigma will be the given input so it is constant and the value of function depends on kappa value.
  double kappa_high = 200.0;
  double kappa_low = 0.0;

  //variable for error margin
  double tolerance = 0.000001;

  double kappa_mid =(kappa_high + kappa_low)/2.0;
  double fkm = sigma_input * sigma_input - (1.0-(pay_modified_besseli_one(kappa_mid)/pay_modified_besseli_zero(kappa_mid)));
  // Here, function pay_modified_besseli_one( )  return I1 value and pay_modified_besseli_zero return I0 value.
  while (abs(fkm) > tolerance) {
	double fkl = sigma_input * sigma_input -(1.0-(pay_modified_besseli_one(kappa_low)/pay_modified_besseli_zero(kappa_low)));
	double fku = sigma_input * sigma_input -(1.0-(pay_modified_besseli_one(kappa_high)/pay_modified_besseli_zero(kappa_high)));
	if ((fkl*fkm) < 0) // Bisection method logic.
	  kappa_high = kappa_mid;
	else
	  kappa_low = kappa_mid;
	kappa_mid = (kappa_high+kappa_low)/2.0;
	fkm = sigma_input * sigma_input -(1.0-(pay_modified_besseli_one(kappa_mid)/pay_modified_besseli_zero(kappa_mid)));
  }
  return kappa_mid;
}

// distance : \mu_d, the distance mean
// dist_sigma : \sigma, the distance deviation
// azimuth : azimuth of current point
// direction_mean : \mu_c, the direction mean
// kappa : \kappa, the directional concentration parameter
// Case split is to ensure robustness against nmumeric underflows.
double MapProbabilityDensity::pay_probability_density_circular(double distance, double dist_sigma, double azimuth, double direction_mean, double kappa)
{
	if (distance / dist_sigma < -30)
		return 0;
	else
		return (1.0/(2.0*M_PI*pay_modified_besseli_zero(kappa))*exp(kappa*cos(azimuth-direction_mean))) * (1.0/(sqrt(2.0*M_PI*dist_sigma*dist_sigma))*exp((-1.0/(2.0*dist_sigma*dist_sigma))*distance*distance));
}

// surface area of quadrilateral
double MapProbabilityDensity::st_area(Coord & c1, Coord & c2, Coord & c3, Coord & c4) {
	return ((c1.x - c3.x) * (c2.y - c4.y) - (c2.x - c4.x) * (c1.y - c3.y)) / 2.0;
	// return ((c1.x * c2.y - c2.x * c1.y) + (c2.x * c3.y - c3.x * c2.y) + (c3.x * c4.y - c4.x * c3.y) + (c4.x * c1.y - c1.x * c4.y)) / 2.0;
}

double MapProbabilityDensity::st_azimuth(Coord & c1, Coord & c2)
{
	return atan2(c2.x - c1.x, c2.y - c1.y); // definition of ST_Azimuth
}

bool MapProbabilityDensity::fFreezing()
{
	if (distance<=0.0 || sigma_distance<=0.0 || direction<0.0 || direction>M_PI * 2.0 || sigma_direction<0.0)
		throw ErrorObject(TR("Given distance, direction or their deviation values are not acceptable."));

	CoordSystem csyUtm = pay_utmzone_from_point(crdwgs);
	CoordSystem csyWgs ("LatLonWGS84");

	Coord pointutm  = csyUtm->cConv(csyWgs, crdwgs);

	double kappa = pay_kappa_estimation(sigma_direction);

	int boxwidth = gr()->rcSize().Col;
	int boxheight = gr()->rcSize().Row;

	RealBuf bufOut(boxwidth);
	double sum = 0;
	for (long j=0; j<boxheight; j++) {
		for (long i = 0; i < boxwidth; ++i) {
			Coord cornerwgs;
			gr()->RowCol2Coord(j, i, cornerwgs); // TODO: corner or center?? ST_Raster2WorldCoordX/Y returns upperleft corner

    	    //calculate probability density at each corner and at middle point of each cell.  These five points determine the roof of a four sided pyramid.
    	    //For upperleft
			Coord cornerutmUL = csyUtm->cConv(csyWgs, cornerwgs);
			double augment = pay_probability_density_circular(distance - rDist(pointutm, cornerutmUL), sigma_distance, st_azimuth(pointutm, cornerutmUL), direction, kappa);

			//For lowerleft
            Coord cornerutmLL = csyUtm->cConv(csyWgs, Coord(cornerwgs.x, cornerwgs.y - scalewgs));
            augment += pay_probability_density_circular(distance - rDist(pointutm,cornerutmLL), sigma_distance, st_azimuth(pointutm, cornerutmLL), direction, kappa);

            //For lowerright
            Coord cornerutmLR = csyUtm->cConv(csyWgs, Coord(cornerwgs.x + scalewgs, cornerwgs.y - scalewgs));
            augment += pay_probability_density_circular(distance - rDist(pointutm,cornerutmLR), sigma_distance, st_azimuth(pointutm, cornerutmLR), direction, kappa);

    	    //For upperright
            Coord cornerutmUR = csyUtm->cConv(csyWgs, Coord(cornerwgs.x + scalewgs, cornerwgs.y));
    	    augment += pay_probability_density_circular(distance - rDist(pointutm,cornerutmUR), sigma_distance, st_azimuth(pointutm, cornerutmUR), direction, kappa);

    	    //For middle point, which contributes double
            Coord cornerutmC = csyUtm->cConv(csyWgs, Coord(cornerwgs.x + scalewgs / 2.0, cornerwgs.y - scalewgs / 2.0));
    	    augment += 2*pay_probability_density_circular(distance - rDist(pointutm,cornerutmC), sigma_distance, st_azimuth(pointutm, cornerutmC), direction, kappa);

    		// CALCULATE VOLUME under each triangle of pyramid and sum them up; the formula used below is after simplification.   	
    		double cell_uncertainty = st_area(cornerutmUL, cornerutmLL, cornerutmLR, cornerutmUR)*augment/6.0;
			bufOut[i] = cell_uncertainty;
			sum += cell_uncertainty;
		}
		pms->PutLineVal(j, bufOut);
		if (!(j % 10))
			if (trq.fUpdate(j, boxheight)) { return false; }
	}
	RangeReal rrMinMax;
	for (long j=0; j<boxheight; j++) {
		pms->GetLineVal(j, bufOut);
		for (long i = 0; i < boxwidth; ++i) {
			bufOut[i] /= sum;
			rrMinMax += bufOut[i];
		}
		pms->PutLineVal(j, bufOut);
		if (!(j % 10))
			if (trq.fUpdate(j, boxheight)) { return false; }
	}
	//ptr.SetMinMax(rrMinMax);
	ValueRange vr = bptr.dvrs().vr();
	vr->vrr()->rLo() = rrMinMax.rLo();
	vr->vrr()->rHi() = rrMinMax.rHi();

	if (trq.fUpdate(iLines(), iLines())) return false;
	return true;
}

String MapProbabilityDensity::sExpression() const
{
	String s("MapProbabilityDensity(%f,%f,%f,%f,%f,%f)", crdwgs.x, crdwgs.y, distance, sigma_distance, direction, sigma_direction);
	return s;
}
