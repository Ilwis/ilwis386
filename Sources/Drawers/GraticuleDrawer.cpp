#include "Headers\toolspch.h"
#include "Engine\Base\Round.h"
#include "Engine\Map\basemap.h"
#include "Engine\Map\Point\ilwPoint.h"
#include "Engine\Drawers\DrawerContext.h"
#include "Engine\Drawers\ComplexDrawer.h"
#include "Engine\Drawers\SimpleDrawer.h" 
#include "Engine\Base\System\RegistrySettings.h"
#include "Engine\Drawers\RootDrawer.h"
#include "Drawers\LineDrawer.h"
#include "Drawers\GraticuleDrawer.h"
#include "Engine\Drawers\ZValueMaker.h"
#include "Engine\SpatialReference\GR3D.H"
#include "Headers\Hs\Drwforms.hs"

using namespace ILWIS;

ILWIS::NewDrawer *createGraticuleDrawer(DrawerParameters *parms) {
	return new GraticuleDrawer(parms);
}

GraticuleDrawer::GraticuleDrawer(DrawerParameters *parms) : 
ComplexDrawer(parms,"GraticuleDrawer")
{
	setDrawMethod(drmSINGLE); // default;
	id = name = "GraticuleDrawer";
	rDist = rUNDEF;
	setActive(false);
	setAlpha(0.8);
	lproperties.thickness = 1;
	lproperties.drawColor = Color(0,0,0);
	lproperties.linestyle = 0xFFFF;
	valid = true;
}

GraticuleDrawer::~GraticuleDrawer() {
}

bool GraticuleDrawer::draw(const DrawLoop drawLoop, const CoordBounds& cbArea) const{
	if ( !isActive() || !valid)
		return false;
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);
	ComplexDrawer::draw(drawLoop, cbArea);
	glDisable(GL_BLEND);
	return true;
}

void GraticuleDrawer::prepare(PreparationParameters *pp) {
	if (  pp->type & RootDrawer::ptGEOMETRY){ 
		String sVal;
		Coord c, cMin, cMax;
		Color clr;
		clear();
		CoordBounds cbMap = getRootDrawer()->getMapCoordBounds();
		CoordSystem csy =  getRootDrawer()->getCoordinateSystem();
		if ( csy->fLatLon2Coord()) {
			LatLon llMin, llMax;
			calcBounds(getRootDrawer()->getGeoReference(), cbMap, csy, llMin, llMax);
			if ( llMin.fUndef() || llMax.fUndef()) {
				valid = false;
				return;
			}
			if (llMin.Lon > llMax.Lon) { // swap
				double lon = llMin.Lon;
				llMin.Lon = llMax.Lon;
				llMax.Lon = lon;
			}
			if (llMin.Lat > llMax.Lat) { // swap
				double lat = llMin.Lat;
				llMin.Lat = llMax.Lat;
				llMax.Lat = lat;
			}
			if ((rDist == rUNDEF) || (pp->type & NewDrawer::ptNEWCSY)) {
				rDist = (llMax.Lat - llMin.Lat) / 7;
				if (rDist > 30)
					rDist = 30;
				else if (rDist > 0.9)
					rDist = rRound(rDist);
				else {
					rDist *= 60;  // work in minutes
					if (rDist > 0.25)   // if larger than 15 seconds
						rDist = rRound(rDist);
					else {
						rDist *= 60;  // work in seconds
						if (rDist > 0.2)
							rDist = rRound(rDist);
						else
							rDist = 1;
						rDist /= 60;
					}  
					rDist /= 60;
				}  
			}

			llMin.Lat -= rDist;
			llMin.Lon -= rDist;
			llMax.Lat += rDist;
			llMax.Lon += rDist;

			Coord c1, c2;
		
			prepareGrid(csy,rDist,llMin, llMax);
			getZMaker()->setThreeDPossible(true);
		}
	}
	if ( pp->type & NewDrawer::ptRENDER) {
		for(int i=0; i < drawers.size(); ++i) {
			LineDrawer *ld = (LineDrawer *)drawers.at(i);
			((LineProperties *)ld->getProperties())->drawColor = lproperties.drawColor;
			((LineProperties *)ld->getProperties())->linestyle = lproperties.linestyle;
			((LineProperties *)ld->getProperties())->thickness = lproperties.thickness;
			ld->setAlpha(getAlpha());
		}
	}
}

void GraticuleDrawer::prepareGrid(const CoordSystem &csy, double rDist, const LatLon& llMin, const LatLon& llMax ) {
	LatLon ll1, ll2;
	ll1.Lat = ceil(llMin.Lat / rDist) * rDist;
	ll2.Lat = floor(llMax.Lat / rDist) * rDist;
	for (double lon = ceil(llMin.Lon / rDist) * rDist; lon < llMax.Lon ; lon += rDist)
	{
		Coord c1,c2;
		ll1.Lon = ll2.Lon = lon;
		AddGraticuleLine(csy,ll1, ll2); // meridians
	}
	ll1.Lon = ceil(llMin.Lon / rDist) * rDist;	
	ll2.Lon = floor(llMax.Lon / rDist) * rDist;
	for (double lat = ceil(llMin.Lat / rDist) * rDist; lat < llMax.Lat ; lat += rDist)
	{
		ll1.Lat = ll2.Lat = lat;
		AddGraticuleLine(csy,ll1, ll2); // parallels
	}
}

#define STEP  25

void GraticuleDrawer::calcBounds(const GeoRef& grf, const CoordBounds& cbMap, const CoordSystem& cs, LatLon& llMin, LatLon& llMax)
{
	llMin = LatLon(100,200);
	llMax = LatLon(-100,-200);
	if (grf.fValid() && 0 != grf->pg3d()) {
		GeoRef gr = grf->pg3d()->mapDTM->gr();
		MinMax mmGr = MinMax(RowCol(0.0,0.0), gr->rcSize());

		long dR = mmGr.height() / STEP;
		long dC = mmGr.width() / STEP;
		if (dR <= 0 || dC <= 0)
			return;
		CoordSystem cs = gr->cs();
		RowCol rc;
		int i, j;				 
		for (i = 0, rc.Row = mmGr.MinRow(); i <= STEP; rc.Row += dR, ++i) {
			for (j = 0, rc.Col = mmGr.MinCol(); j <= STEP; rc.Col += dC, ++j) {
				Coord crd = gr->cConv(rc);
				if (crd.fUndef())
					continue;
				LatLon ll = cs->llConv(crd);
				if (ll.fUndef())
					continue;
				if (ll.Lat < llMin.Lat)
					llMin.Lat = ll.Lat;
				if (ll.Lon < llMin.Lon)
					llMin.Lon = ll.Lon;
				if (ll.Lat > llMax.Lat)
					llMax.Lat = ll.Lat;
				if (ll.Lon > llMax.Lon)
					llMax.Lon = ll.Lon;
			}  
		}  
	} else {
		long dR = cbMap.height() / STEP;
		long dC = cbMap.width() / STEP;
		if (dR <= 0 || dC <= 0)
			return;
		double rRow, rCol;
		for (rRow = cbMap.cMin.y;; rRow += dR)
		{
			bool fBreak = false;
			if (rRow > cbMap.cMax.y) {
				rRow = cbMap.cMax.y;
				fBreak = true;
			}
			for (rCol = cbMap.cMin.x;; rCol += dC)
			{
				bool fBreak = false;
				if (rCol > cbMap.cMax.x) {
					rCol = cbMap.cMax.x;
					fBreak = true;
				}
				Coord crd = getRootDrawer()->glToWorld(Coord(rCol,rRow));
				if (!crd.fUndef()) {
					LatLon ll = cs->llConv(crd);
					if (!ll.fUndef()) {
						if (ll.Lat < llMin.Lat)
							llMin.Lat = ll.Lat;
						if (ll.Lon < llMin.Lon)
							llMin.Lon = ll.Lon;
						if (ll.Lat > llMax.Lat)
							llMax.Lat = ll.Lat;
						if (ll.Lon > llMax.Lon)
							llMax.Lon = ll.Lon;
					}
				}
				if (fBreak)
					break;
			}          
			if (fBreak)
				break;
		}    
	}
}

void GraticuleDrawer::AddGraticuleLine(const CoordSystem &csy, const LatLon& llBoundary1, const LatLon& llBoundary2)
{
	ILWIS::DrawerParameters dp(getRootDrawer(), this);
	PreparationParameters pp(NewDrawer::ptGEOMETRY);
	GraticuleLine *line = (GraticuleLine *)NewDrawer::getDrawer("LineDrawer", &pp, &dp);
	vector<Coord> coords;
	int steps = 250;
	if ( llBoundary1.Lon == llBoundary2.Lon) { // meridian
		double rShift = (llBoundary2.Lat - llBoundary1.Lat) / steps;
		LatLon ll1 = llBoundary1;
		Coord c1 = csy->cConv(ll1);
		if (!c1.fUndef()) {
			c1 = getRootDrawer()->glConv(c1);
			c1.z = 0;
			coords.push_back(c1);
		}
		for(int i=0; i < steps; ++i) {
			ll1.Lat += rShift;
			Coord c2 = csy->cConv(ll1);
			if (!c2.fUndef()) {
				c2 = getRootDrawer()->glConv(c2);
				if (c1.y != c2.y) {
					c2.z = 0;
					coords.push_back(c2);
					c1 = c2;
				}
			}
		}
	} else if ( llBoundary1.Lat == llBoundary2.Lat) { // parallel
		double rShift = (llBoundary2.Lon - llBoundary1.Lon) / steps;
		LatLon ll1 = llBoundary1;
		Coord c1 = csy->cConv(ll1);
		if (!c1.fUndef()) {
			c1 = getRootDrawer()->glConv(c1);
			c1.z = 0;
			coords.push_back(c1);
		}
		for(int i=0; i < steps; ++i) {
			ll1.Lon += rShift;
			Coord c2 = csy->cConv(ll1);
			if (!c2.fUndef()) {
				c2 = getRootDrawer()->glConv(c2);
				if ( c1.x != c2.x) {
					c2.z = 0;
					coords.push_back(c2);
					c1 = c2;
				}
			}
		}
	}
	line->addCoords(coords);
	((LineProperties *)line->getProperties())->drawColor = lproperties.drawColor;
	addDrawer(line); 
}

String GraticuleDrawer::store(const FileName& fnView, const String& parentSection) const{
	ComplexDrawer::store(fnView, getType());
	ObjectInfo::WriteElement(getType().c_str(),"Distance",fnView, rDist);
	lproperties.store(fnView,getType());
	return getType();
}

void GraticuleDrawer::load(const FileName& fnView, const String& parenSection){
	ComplexDrawer::load(fnView, getType());
	ObjectInfo::ReadElement(getType().c_str(),"Distance",fnView, rDist);
	lproperties.load(fnView,getType());
}

void GraticuleDrawer::setGridSpacing(double d){
	rDist = d;
}

GeneralDrawerProperties *GraticuleDrawer::getProperties() {
	return &lproperties;
}

void GraticuleDrawer::prepareChildDrawers(PreparationParameters *parms) {
	for(int i = 0; i < drawers.size(); ++i) {
		GraticuleLine *gline = (GraticuleLine *)drawers.at(i);
		if ( gline) {
			LineProperties *oldprops = (LineProperties *)gline->getProperties();
			oldprops->drawColor = lproperties.drawColor;
			oldprops->linestyle = lproperties.linestyle;
			oldprops->thickness = lproperties.thickness;
			gline->prepare(parms);
		}
	}
}

//---------------------------------------------
ILWIS::NewDrawer *createGraticuleLine(DrawerParameters *parms) {
	return new GraticuleLine(parms);
}

GraticuleLine::GraticuleLine(DrawerParameters *parms) : LineDrawer(parms,"GraticuleLine"){
}

GraticuleLine::~GraticuleLine(){
}

bool GraticuleLine::draw(const DrawLoop drawLoop, const CoordBounds& cbArea) const{
	return LineDrawer::draw(drawLoop, cbArea);
}

void GraticuleLine::prepare(PreparationParameters *pp){
	LineDrawer::prepare(pp);
	if ( pp->type & NewDrawer::ptGEOMETRY)
		clear();
}

void GraticuleLine::addDataSource(void *crd, int options) {
	Coord c = *((Coord *)crd);
	cb += c;
	if (lines.size() == 0)
		lines.push_back(new CoordinateArraySequence());
	lines.at(0)->add(c);
}





