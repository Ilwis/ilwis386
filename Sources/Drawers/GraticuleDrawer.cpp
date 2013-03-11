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
	setTransparency(0.2);
	lproperties.thickness = 1;
	lproperties.drawColor = Color(0,0,0);
	lproperties.linestyle = 0xFFFF;
	setValid(true);
}

GraticuleDrawer::~GraticuleDrawer() {
}


bool GraticuleDrawer::draw(const DrawLoop drawLoop, const CoordBounds& cbArea) const{
	if ( !isActive() || !isValid())
		return false;
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);
	if ( getRootDrawer()->is3D()) {
		glPushMatrix();
		double z0 = getRootDrawer()->getZMaker()->getZ0(true);
		glTranslated(0,0,z0);
	}
	ComplexDrawer::draw(drawLoop, cbArea);
	if ( getRootDrawer()->is3D())
		glPopMatrix();
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
			calcBounds(cbMap, csy,llMin, llMax);
			if ( llMin.fUndef() || llMax.fUndef()) {
				setValid(false);
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
			if ((rDist == rUNDEF) || (pp->type & NewDrawer::ptNEWCSY))
				rDist = rRound((llMax.Lat - llMin.Lat) / 7);

			Coord c1, c2;
		
			prepareGrid(csy,rDist,llMax, llMin);
			getZMaker()->setThreeDPossible(true);
		}
	}
	if ( pp->type & NewDrawer::ptRENDER) {
		for(int i=0; i < drawers.size(); ++i) {
			LineDrawer *ld = (LineDrawer *)drawers.at(i);
			((LineProperties *)ld->getProperties())->drawColor = lproperties.drawColor;
			((LineProperties *)ld->getProperties())->linestyle = lproperties.linestyle;
			((LineProperties *)ld->getProperties())->thickness = lproperties.thickness;
			ld->setTransparency(getTransparency());
		}
	}
}




void GraticuleDrawer::prepareGrid(const CoordSystem &csy, double rDist, const LatLon& llMax, const LatLon& llMin ) {
	LatLon ll1, ll2;
	ll1 = llMin;
	ll2 = llMin;
	ll2.Lat = llMax.Lat;
	for (double lon = ceil(llMin.Lon / rDist) * rDist; lon < llMax.Lon ; lon += rDist)
	{
		Coord c1,c2;
		ll1.Lon = lon;
		ll2.Lon = lon;
		AddGraticuleLine(csy,ll1, ll2);
	}

	ll1 = llMin;
	ll2 = llMin;
	ll2.Lon = llMax.Lon;
	for (double lat = ceil(llMin.Lat / rDist) * rDist; lat < llMax.Lat ; lat += rDist)
	{
		ll1.Lat = ll2.Lat = lat;
		AddGraticuleLine(csy,ll1, ll2);
	}
}

#define STEP  25

void GraticuleDrawer::calcBounds(const CoordBounds& cbMap, const CoordSystem& cs,  LatLon& llMin, LatLon& llMax)
{
  Coord cMin = getRootDrawer()->glToWorld(cbMap.cMin);
  Coord cMax = getRootDrawer()->glToWorld(cbMap.cMax);
  llMin = LatLon(100,200);
  llMax = LatLon(-100,-200);
  int i, j;	
  Coord c;
  double rDX = (cMax.x - cMin.x) / STEP;
  double rDY = (cMax.y - cMin.y) / STEP;
  for (i = 0, c.y = cMin.y; i <= STEP; c.y += rDY, ++i) {
	  for (j = 0, c.x = cMin.x; j <= STEP; c.x += rDX, ++j) {
      LatLon ll = cs->llConv(c);
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
}
void GraticuleDrawer::AddGraticuleLine(const CoordSystem &csy, const LatLon& llBoundary1, const LatLon& llBoundary2)
{
	ILWIS::DrawerParameters dp(getRootDrawer(), this);
	PreparationParameters pp(NewDrawer::ptGEOMETRY);
	GraticuleLine *line = (GraticuleLine *)NewDrawer::getDrawer("LineDrawer", &pp, &dp);
	double z = 	getZMaker()->getZ0(true);
	vector<Coord> coords;
	int steps = 250;
	if ( llBoundary1.Lon == llBoundary1.Lon) {
		double rShift = (llBoundary1.Lat - llBoundary2.Lat) / steps;
		LatLon ll1 = llBoundary2;
		Coord c1 = csy->cConv(ll1);
		c1 = getRootDrawer()->glConv(c1);
		c1.z = z;
		coords.push_back(c1);
		for(int i=0; i < steps; ++i) {
			LatLon ll2 = ll1;
			ll2.Lat = ll1.Lat + rShift;
			Coord c2 = csy->cConv(ll2);
			c2 = getRootDrawer()->glConv(c2);
			if ( c1.y != c2.y) {
				c2.z = z;
				coords.push_back(c2);
			}
			ll1 = ll2;
		}
	}
	if ( llBoundary1.Lat == llBoundary1.Lat) {
		double rShift = (llBoundary1.Lon - llBoundary2.Lon) / steps;
		LatLon ll1 = llBoundary2;
		Coord c1 = csy->cConv(ll1);
		c1 = getRootDrawer()->glConv(c1);
		for(int i=0; i < steps; ++i) {
			LatLon ll2 = ll1;
			ll2.Lon = ll1.Lon + rShift;
			Coord c2 = csy->cConv(ll2);
			c2 = getRootDrawer()->glConv(c2);
			if ( c1.x != c2.x) {
				c2.z = z;
				coords.push_back(c2);
			}
			ll1 = ll2;
		}
	}
	line->addCoords(coords);
	((LineProperties *)line->getProperties())->drawColor = lproperties.drawColor;
	addDrawer(line); 
}

//void GraticuleDrawer::DrawCurvedLine(Coord c1, Coord c2)
//{
//	Coord cStep, crd;
//	int iSteps = 500;
//	cStep.x = (c2.x - c1.x) / iSteps;
//	cStep.y = (c2.y - c1.y) / iSteps;
//	p = psn->pntPos(c1);
//	if (p.x == shUNDEF || p.y == shUNDEF)
//		fPointOk = false;
//	else {
//		fPointOk = true;
//		cdc->MoveTo(p);
//	}
//	crd = c1;
//	for (int i = 0; i < iSteps; ++i) {
//		crd += cStep;
//		p = psn->pntPos(crd);
//		if (p.x == shUNDEF || p.y == shUNDEF)
//			fPointOk = false;
//		else if (fPointOk) 
//			cdc->LineTo(p);
//		else {
//			fPointOk = true;
//			cdc->MoveTo(p);
//		}
//	}
//}

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





