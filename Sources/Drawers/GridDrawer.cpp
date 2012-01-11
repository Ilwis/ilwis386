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
#include "Drawers\GridDrawer.h"
#include "Engine\Drawers\ZValueMaker.h"
#include "Headers\Hs\Drwforms.hs"

using namespace ILWIS;

ILWIS::NewDrawer *createGridDrawer(DrawerParameters *parms) {
	return new GridDrawer(parms);
}

GridDrawer::GridDrawer(DrawerParameters *parms) : 
ComplexDrawer(parms,"GridDrawer")
{
	setDrawMethod(drmSINGLE); // default;
	id = name = "GridDrawer";
	rDist = rUNDEF;
	setActive(false);
	//setTransparency(0.4);
	threeDGrid = false;
	lproperties.thickness = 1;
	lproperties.drawColor = Color(0,0,0);
	planeColor = Color(210,210,255);
	lproperties.linestyle = 0xFFFF;
	mode = mGRID | mVERTICALS | mAXIS;
}

GridDrawer::~GridDrawer() {
	//for(int i = 0; i < planeQuads.size(); ++i) {
	//	delete planeQuads[i];
	//}
}


bool GridDrawer::draw( const CoordBounds& cbArea) const{
	if ( !isActive() && !isValid())
		return false;
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);
	if ( getRootDrawer()->is3D()) {
		glPushMatrix();
		double z0 = getRootDrawer()->getZMaker()->getZ0(true);
		glTranslated(0,0,z0);
	}
	ComplexDrawer::draw( cbArea);
	if ( mode & mPLANE) {
		drawPlane(cbArea);
	}
	if ( getRootDrawer()->is3D())
		glPopMatrix();
	glDisable(GL_BLEND);
	return true;
}


bool GridDrawer::drawPlane(const CoordBounds& cbArea) const{
	glClearColor(1.0,1.0,1.0,0.0);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);

	ComplexDrawer *cdrw = (ComplexDrawer *)getParentDrawer();
	ZValueMaker *zmaker = cdrw->getZMaker();

	glColor4f(planeColor.redP(),planeColor.greenP(), planeColor.blueP(), getTransparency());
	double zscale = zmaker->getZScale();
	double zoffset = zmaker->getOffset();
	glPushMatrix();
	glScaled(1,1,zscale);
	glTranslated(0,0,zoffset);

	for(int i = 0; i < planeQuads.size(); ++i) {
		glBegin(GL_QUADS);
			Coord c1 = planeQuads[i][0];
			Coord c2 = planeQuads[i][1];
			Coord c3 = planeQuads[i][2];
			Coord c4 = planeQuads[i][3];

			glVertex3f( c1.x, c1.y, c1.z);	
			glVertex3f( c2.x, c2.y, c2.z);	
			glVertex3f( c3.x, c3.y, c3.z);
			glVertex3f( c4.x, c4.y, c4.z);
		glEnd();
	}
	glPopMatrix();
	return true;
}

void GridDrawer::prepare(PreparationParameters *pp) {
	if (  pp->type & RootDrawer::ptGEOMETRY){ 
		String sVal;
		Coord c, cMin, cMax;
		Color clr;
		clear();
	/*	getZMaker()->setThreeDPossible(true);
		getZMaker()->setZOrder(100, getZMaker()->getZ0(true));*/
		CoordBounds cbMap = getRootDrawer()->getMapCoordBounds();
		cMin = getRootDrawer()->glToWorld(cbMap.cMin);
		cMax = getRootDrawer()->glToWorld(cbMap.cMax);
		if (cMin.x > cMax.x) { // swap
			double x = cMin.x;
			cMin.x = cMax.x;
			cMax.x = x;
		}
		if (cMin.y > cMax.y) { // swap
			double y = cMin.y;
			cMin.y = cMax.y;
			cMax.y = y;
		}
		if ((rDist == rUNDEF) || (pp->type & NewDrawer::ptNEWCSY))
			rDist = rRound((cMax.x - cMin.x) / 7);

		Coord c1, c2;
	
		if ( (mode & GridDrawer::mGRID) || (mode & GridDrawer::mGROUNDLEVEL))
			prepareGrid(rDist,cMax, cMin);
		if ( threeDGrid) {
			if ( mode & GridDrawer::mPLANE)
				preparePlanes(rDist,cMax, cMin);
			if (mode & GridDrawer::mAXIS)
				prepareVAxis(rDist,cMax, cMin);
			if ( mode & GridDrawer::mVERTICALS)
				prepareVerticals(rDist, cMax, cMin);
			if ( mode & GridDrawer::mCUBE)
				prepareCube(rDist, cMax, cMin);
		}
		getZMaker()->setThreeDPossible(true);
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

void GridDrawer::prepareVAxis(double rDist,const Coord& cMax, const Coord& cMin) {
	if ( planeDistances.size() == 0)
		return;
	double maxz = planeDistances[planeDistances.size() - 1];
	Coord c1, c2, oldc2,startc2;
	c1 = cMin;
	c1.z  = getZMaker()->getZ0(true);;
	c2 = c1;
	c2.z = maxz;
	startc2 = oldc2 = c2;
	AddGridLine(c1,c2);
	c1.x = cMin.x;
	c1.y = cMax.y;
	c1.z  = getZMaker()->getZ0(true);;
	c2 = c1;
	c2.z = maxz;
	AddGridLine(oldc2,c2);
	oldc2 = c2;
	AddGridLine(c1,c2);
	c1 = cMax;
	c1.z  = getZMaker()->getZ0(true);;
	c2 = c1;
	c2.z = maxz;
	AddGridLine(oldc2,c2);
	oldc2 = c2;
	AddGridLine(c1,c2);
	c1.x = cMax.x;
	c1.y = cMin.y;
	c1.z  = getZMaker()->getZ0(true);;
	c2 = c1;
	c2.z = maxz;
	AddGridLine(oldc2,c2);
	oldc2 = c2;
	AddGridLine(c1,c2);
	AddGridLine(oldc2, startc2);
	double z = maxz;


	

}
void GridDrawer::prepareVerticals(double rDist,const Coord& cMax, const Coord& cMin) {
	Coord c1, c2;
	if ( planeDistances.size() == 0)
		return;

	double maxz = planeDistances[planeDistances.size() - 1];
	for (double x = ceil(cMin.x / rDist) * rDist; x < cMax.x ; x += rDist)
	{
		for (double y = ceil(cMin.y / rDist) * rDist; y < cMax.y ; y += rDist)
		{
			c1.x = x;
			c1.y = y;
			c1.z =  getZMaker()->getZ0(true);;
			c2.x = x;
			c2.y = y;
			c2.z = maxz;
			AddGridLine(c1, c2);
		}
	}
}

void GridDrawer::getLayerDistances(vector<double>& dist) {
	//if ( dist.size() == 0)
	//	return;

	RootDrawer *rootDrawer = getRootDrawer();
	int n = rootDrawer->getDrawerCount();

	double offset = rUNDEF;
	for(int i = 0; i < n; ++i) {
		ComplexDrawer *cdr  = dynamic_cast<ComplexDrawer *>(rootDrawer->getDrawer(i));
		if ( !cdr)
			continue;
		for(int j = 0; j < cdr->getDrawerCount(); ++j ){
			ComplexDrawer *cdr2 = dynamic_cast<ComplexDrawer *>(cdr->getDrawer(j));
			if (!cdr2)
				continue;
			offset = cdr2->getZMaker()->getOffset();
			dist.push_back(offset);
		}
	}
	double minOffset = dist[dist.size() - 1] * 0.02;
	for(int i = 0; i < dist.size(); ++i) {
		dist[i] += minOffset;
	}
}

void GridDrawer::preparePlanes(double rDist, const Coord& cMax, const Coord& cMin ) {
	Coord c1, c2;
	double z = 	getZMaker()->getZ0(true);
	planeDistances.clear();
	getLayerDistances(planeDistances);
	if ( planeDistances.size() == 0)
		return;

	int zplanes = threeDGrid ? planeDistances.size() : 0;
	resizeQuadsVector(zplanes);
	for(int i=0; i <= zplanes; ++i) {
		c1.z = c2.z = z;
		c1.y = cMin.y;
		c2.y = cMax.y;
		planeQuads[i][0] = getRootDrawer()->glConv(Coord(cMin.x,cMin.y,z));
		planeQuads[i][1] = getRootDrawer()->glConv(Coord(cMin.x,cMax.y,z));
		planeQuads[i][2] = getRootDrawer()->glConv(Coord(cMax.x,cMax.y,z));
		planeQuads[i][3] = getRootDrawer()->glConv(Coord(cMax.x,cMin.y,z));
	
		z  = planeDistances[i];
	}
	
}

void GridDrawer::prepareCube(double rDist, const Coord& cMax, const Coord& cMin ) {
	Coord c1, c2;
	if ( planeDistances.size() == 0)
		return;

	double z = 	getZMaker()->getZ0(true);;
	double maxz = planeDistances[planeDistances.size() - 1];

	c1 = cMin;
	c1.z = c2.z = z;
	c2.x = cMin.x;
	c2.y = cMax.y;
	AddGridLine(c1, c2);
	c1 = c2;
	c2.x = cMax.x;
	c2.y = cMax.y;
	AddGridLine(c1, c2);
	c1 = c2;
	c2.x = cMax.x;
	c2.y = cMin.y;
	AddGridLine(c1, c2);
	c1 = c2;
	c2.x = cMin.x;
	c2.y = cMin.y;
	AddGridLine(c1, c2);
	c1 = cMin;
	c1.z = c2.z = maxz;
	c2.x = cMin.x;
	c2.y = cMax.y;
	AddGridLine(c1, c2);
	c1 = c2;
	c2.x = cMax.x;
	c2.y = cMax.y;
	AddGridLine(c1, c2);
	c1 = c2;
	c2.x = cMax.x;
	c2.y = cMin.y;
	AddGridLine(c1, c2);
	c1 = c2;
	c2.x = cMin.x;
	c2.y = cMin.y;
	AddGridLine(c1, c2);
	c1 = c2 = cMin;
	c1.z = z;
	c2.z = maxz;
	AddGridLine(c1, c2);
	c1 = c2 = Coord(cMin.x, cMax.y);
	c1.z = z;
	c2.z = maxz;
	AddGridLine(c1, c2);
	c1 = c2 = Coord(cMax.x, cMax.y);
	c1.z = z;
	c2.z = maxz;
	AddGridLine(c1, c2);
	c1 = c2 = Coord(cMax.x, cMin.y);
	c1.z = z;
	c2.z = maxz;
	AddGridLine(c1, c2);
}

void GridDrawer::prepareGrid(double rDist, const Coord& cMax, const Coord& cMin ) {
	Coord c1, c2;
	planeDistances.clear();
	getLayerDistances(planeDistances);
	if ( planeDistances.size() == 0)
		return;

	int zplanes = threeDGrid ? planeDistances.size() : 0;
	resizeQuadsVector(zplanes);
	int nPlanes = mode & mGRID ? zplanes : 0;
	for(int i=0; i <= nPlanes; ++i) {
		double z = planeDistances[i];
		c1.z = c2.z = z;
		c1.y = cMin.y;
		c2.y = cMax.y;
		for (double x = ceil(cMin.x / rDist) * rDist; x < cMax.x ; x += rDist)
		{
			c1.x = c2.x = x;
			AddGridLine(c1, c2);
		}

		c1.x = cMin.x;
		c2.x = cMax.x;
		for (double y = ceil(cMin.y / rDist) * rDist; y < cMax.y ; y += rDist)
		{
			c1.y = c2.y = y;
			AddGridLine(c1, c2);
		}
		if ( threeDGrid && (mode & GridDrawer::mGRID)) {
			AddGridLine(Coord(cMin.x,cMin.y,z), Coord(cMin.x, cMax.y,z));
			AddGridLine(Coord(cMin.x,cMax.y,z), Coord(cMax.x, cMax.y,z));
			AddGridLine(Coord(cMax.x,cMax.y,z), Coord(cMax.x, cMin.y,z));
			AddGridLine(Coord(cMax.x,cMin.y,z), Coord(cMin.x, cMin.y,z));
		}
		z = planeDistances[i];
	}
}

void GridDrawer::resizeQuadsVector(int planes) {
	if ( planes == 0)
		return;

	for(int i = 0; i < planeQuads.size(); ++i) {
		delete [] planeQuads[i];
	}
	planeQuads.resize(planes + 1);
	for(int i = 0; i < planes + 1; ++i) {
		planeQuads[i] = new Coord[4];
	}
}

void GridDrawer::AddGridLine(Coord c1, Coord c2)
{
	ILWIS::DrawerParameters dp(getRootDrawer(), this);
	PreparationParameters pp(NewDrawer::ptGEOMETRY);
	GridLine *line = (GridLine *)NewDrawer::getDrawer("GridLine", &pp, &dp);
	c1 = getRootDrawer()->glConv(c1);
	c2 = getRootDrawer()->glConv(c2);
	line->addDataSource(&c1);
	line->addDataSource(&c2);
	((LineProperties *)line->getProperties())->drawColor = lproperties.drawColor;
	addDrawer(line);
}

void GridDrawer::prepareChildDrawers(PreparationParameters *parms) {
	for(int i = 0; i < drawers.size(); ++i) {
		GridLine *gline = (GridLine *)drawers.at(i);
		if ( gline) {
			LineProperties *oldprops = (LineProperties *)gline->getProperties();
			oldprops->drawColor = lproperties.drawColor;
			oldprops->linestyle = lproperties.linestyle;
			oldprops->thickness = lproperties.thickness;
			gline->prepare(parms);
		}
	}
}

String GridDrawer::store(const FileName& fnView, const String& parentSection) const{
	ComplexDrawer::store(fnView, getType());
	ObjectInfo::WriteElement(getType().c_str(),"Distance",fnView, rDist);
	lproperties.store(fnView,getType());
	ObjectInfo::WriteElement(getType().c_str(),"ThreeDGrid",fnView, threeDGrid);
	ObjectInfo::WriteElement(getType().c_str(),"Mode",fnView, mode);
	ObjectInfo::WriteElement(getType().c_str(),"PlaneColor",fnView, planeColor);
	return getType();
}

void GridDrawer::load(const FileName& fnView, const String& parenSection){
	ObjectInfo::ReadElement(getType().c_str(),"Distance",fnView, rDist);
	lproperties.store(fnView,getType());
	ObjectInfo::ReadElement(getType().c_str(),"ThreeDGrid",fnView, threeDGrid);
	ObjectInfo::ReadElement(getType().c_str(),"Mode",fnView, mode);
	ObjectInfo::ReadElement(getType().c_str(),"PlaneColor",fnView, planeColor);
}


double GridDrawer::getGridSpacing() const{
	return rDist;
}
void GridDrawer::setGridSpacing(double d){
	rDist = d;
}

GeneralDrawerProperties *GridDrawer::getProperties() {
	return &lproperties;
}

int GridDrawer::getMode() const{
	return mode;
}

void GridDrawer::setMode(int m) {
	mode = m;
}

Color GridDrawer::getPlaneColor() const {
	return planeColor;
}
void GridDrawer::setPlaneColor(Color clr){
	planeColor = clr;
}

void GridDrawer::set3DGrid(bool yesno){
	threeDGrid = yesno;
}

//---------------------------------------------
ILWIS::NewDrawer *createGridLine(DrawerParameters *parms) {
	return new GridLine(parms);
}

GridLine::GridLine(DrawerParameters *parms) : LineDrawer(parms,"GridLine"){
}

GridLine::~GridLine(){
}

bool GridLine::draw( const CoordBounds& cbArea) const{
	return LineDrawer::draw( cbArea);
}

void GridLine::prepare(PreparationParameters *pp){
	LineDrawer::prepare(pp);
	if ( pp->type & NewDrawer::ptGEOMETRY)
		clear();

}

void GridLine::addDataSource(void *crd, int options) {
	Coord c = *((Coord *)crd);
	cb += c;
	if (lines.size() == 0)
		lines.push_back(new CoordinateArraySequence());
	lines.at(0)->add(c);
}





