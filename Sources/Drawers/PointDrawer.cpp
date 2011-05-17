#include "Headers\toolspch.h"
#include "Engine\Map\basemap.h"
#include "Engine\Drawers\SimpleDrawer.h" 
#include "Engine\Drawers\RootDrawer.h"
#include "Drawers\DrawingColor.h" 
#include "Engine\Drawers\SVGElements.h"
//#include "Drawers\SVGDrawers.h"
#include "drawers\pointdrawer.h"
#include "Engine\Drawers\ComplexDrawer.h"
#include "Engine\Drawers\SVGLoader.h"
#include "Engine\Drawers\SVGElements.h"
#include "Engine\Drawers\ZValueMaker.h"

using namespace ILWIS;

String PointDrawer::defaultSymbol="open-rectangle";


ILWIS::NewDrawer *createPointDrawer(DrawerParameters *parms) {
	return new PointDrawer(parms);
}

PointDrawer::PointDrawer(DrawerParameters *parms) : SimpleDrawer(parms,"PointDrawer"), element(0), symbol(defaultSymbol), scale(1.0) {
	drawColor = Color(0,0,0);
}

PointDrawer::PointDrawer(DrawerParameters *parms, const String& name) : SimpleDrawer(parms,name), element(0), symbol(defaultSymbol), scale(1.0){
}

PointDrawer::~PointDrawer() {
}

void PointDrawer::prepare(PreparationParameters *p){
	SimpleDrawer::prepare(p);
	if ( p->type & NewDrawer::ptRENDER && symbol != "") {
		const SVGLoader *loader = NewDrawer::getSvgLoader();
		SVGLoader::const_iterator cur = loader->find(symbol);
		if ( cur == loader->end())
			throw ErrorObject(TR("Unknow symbol"));
		element = (*cur).second;
		calcSize();
	}
}

void PointDrawer::setCoord(const Coord& crd) {
	cNorm = crd;
}

void PointDrawer::calcSize() {
	width = 0;
	for(vector<SVGAttributes>::const_iterator cur = element->begin(); cur != element->end(); ++cur) {
		width = max(width, (*cur).rwidth);	
	}
	height = 0;
	for(vector<SVGAttributes>::const_iterator cur = element->begin(); cur != element->end(); ++cur) {
		height = max(height, (*cur).rheight);	
	}
	CoordBounds cbZoom = getRootDrawer()->getCoordBoundsZoom();
	if (width == 0)
		width = cbZoom.width() / 100.0;
	if (height == 0)
		height = cbZoom.width() / 100.0;
}

void PointDrawer::setScale(double s) {
	scale = s;
}

bool PointDrawer::draw( const CoordBounds& cbArea) const {
	if ( cNorm.fUndef())
		return false;
	const CoordBounds& cbZoom = getRootDrawer()->getCoordBoundsZoom();
	if ( !cbZoom.fContains(cNorm))
			return false;

	bool extrusion = getSpecialDrawingOption(NewDrawer::sdoExtrusion);
	bool filledExtr = getSpecialDrawingOption(NewDrawer::sdoFilled);

	ComplexDrawer *cdrw = (ComplexDrawer *)getParentDrawer();
	ZValueMaker *zvmkr = cdrw->getZMaker();
	bool is3D = getRootDrawer()->is3D();// && zvmkr->getThreeDPossible();
	double z0 = cdrw->getZMaker()->getZ0(is3D);

	double fx = cNorm.x;
	double fy = cNorm.y;
	double fz = is3D ? cNorm.z : z0;

	double symbolScale = cbZoom.width() / 200;
	CoordBounds cb(Coord(fx - symbolScale, fy - symbolScale,fz), Coord(fx + symbolScale, fy + symbolScale,fz));

	double xscale = cb.width() / width;
	double yscale = cb.height() / height;

	glPushMatrix();
	glTranslated(cb.cMin.x, cb.cMin.y, fz);
	glScaled(xscale * scale, yscale * scale , 0);

	//if ( is3D) {
	//	zscale = cdrw->getZMaker()->getZScale();
	//	zoffset = cdrw->getZMaker()->getOffset();
	//	glPushMatrix();
	//	glScaled(1,1,zscale);
	//	glTranslated(0,0,zoffset);
	//}
	for(vector<SVGAttributes>::const_iterator cur = element->begin(); cur != element->end(); ++cur) {
		switch((*cur).type) {
			case SVGAttributes::sCIRCLE:
			case SVGAttributes::sELLIPSE:
				drawEllipse((*cur), fz);
				break;
			case SVGAttributes::sRECTANGLE:
				drawRectangle((*cur), fz);
				break;
			case SVGAttributes::sPOLYGON:
				drawPolygon((*cur), fz);
				break;
			case SVGAttributes::sLINE:
			case SVGAttributes::sPOLYLINE:
				drawLine((*cur), fz);
				break;
			case SVGAttributes::sPATH:
				break;			
		};
	}
	glPopMatrix();

	if ( specialOptions & NewDrawer::sdoSELECTED) {
		CoordBounds cbSelect = cb;
			cbSelect *= 1.2;
		glColor4d(1, 0, 0, 1);
		glBegin(GL_QUADS);						
		glVertex3f( cbSelect.MinX(), cbSelect.MinY(),fz);	
		glVertex3f( cbSelect.MinX(),cbSelect.MaxY(),fz);	
		glVertex3f( cbSelect.MaxX(), cbSelect.MaxY(),fz);
		glVertex3f( cbSelect.MaxX(), cbSelect.MinY(),fz);
		glEnd();

	}

	if ( is3D) {
		if ( extrusion) {
			glBegin(GL_LINE_STRIP) ;
			glVertex3d(cNorm.x,cNorm.y,0);
			glVertex3d(cNorm.x, cNorm.y,fz*zvmkr->getZScale());
			glEnd();
		}

	}




	return true;
}

HTREEITEM PointDrawer::configure(LayerTreeView  *tv, HTREEITEM parent) {
	return parent;
}

void PointDrawer::setDrawColor(const Color& col) {
	drawColor = col;
}

Color PointDrawer::getDrawColor() const {
	return drawColor;
}

void PointDrawer::setSymbol(const String& sym) {
	symbol = sym;
}

String PointDrawer::getSymbol() const {
	return symbol;
}

void PointDrawer::drawRectangle(const SVGAttributes& attributes, double z) const {
	double transp = getTransparency() * attributes.opacity;
	double filltransp = transp ;
	Color fcolor = attributes.fillColor;
	if ( fcolor == colorUSERDEF)
		fcolor = getDrawColor();
	double w = attributes.rwidth != 0 ? attributes.rwidth : width;
	double h = attributes.rheight != 0 ? attributes.rheight : height;

	if ( fcolor != colorUNDEF) {
		glColor4d(fcolor.redP(), fcolor.greenP(), fcolor.blueP(), transp);
		glBegin(GL_QUADS);						
		glVertex3f( attributes.ox, attributes.oy, z);	
		glVertex3f( attributes.ox, attributes.oy + h,z);	
		glVertex3f( attributes.ox + w, attributes.oy + h,z);
		glVertex3f( attributes.ox + w, attributes.oy,z);
		glEnd();
	}
	glLineWidth(attributes.borderThickness);
	glColor4d(attributes.strokeColor.redP(), attributes.strokeColor.greenP(), attributes.strokeColor.blueP(), transp);
	glBegin(GL_LINE_STRIP);						
		glVertex3f( attributes.ox, attributes.oy, z);	
		glVertex3f( attributes.ox, attributes.oy + h,z);	
		glVertex3f( attributes.ox + w, attributes.oy + h,z);
		glVertex3f( attributes.ox + w, attributes.oy,z);
		glVertex3f( attributes.ox, attributes.oy, z);
	glEnd();
}

void PointDrawer::drawEllipse(const SVGAttributes& attributes, double z) const{
	double rx = attributes.rx > 0 ? attributes.rx : width / 2;
	double ry = attributes.ry >0 ? attributes.ry : height / 2;
	double lcx = attributes.cx > 0 ? attributes.cx : width / 2;
	double lcy = attributes.cy > 0 ? attributes.cy : height / 2;
	double r = min(rx,ry);

	Color fcolor = attributes.fillColor == colorUSERDEF ? getDrawColor() : attributes.fillColor;
	double transp = attributes.opacity * getTransparency();

	int sections = 20; //number of triangles to use to estimate a circle
	// (a higher number yields a more perfect circle)
	double twoPi =  2.0 * M_PI;
	if ( attributes.type == SVGAttributes::sCIRCLE)
		rx = ry = r;
	
	if ( fcolor != colorUNDEF) {
		glColor4d(fcolor.redP(), fcolor.greenP(), fcolor.blueP(), transp);
		glBegin(GL_TRIANGLE_FAN);
		glVertex3d(lcx, lcy, z); // origin
		for(int i = 0; i <= sections;i++) { // make $section number of circles
			glVertex3d(lcx + rx * cos(i *  twoPi / sections), 
				lcy + ry* sin(i * twoPi / sections), z);
		}
		glEnd();
	}

	Color scolor = attributes.strokeColor == colorUNDEF ? getDrawColor() :  attributes.strokeColor;
	glColor4d(scolor.redP(), scolor.greenP(), scolor.blueP(), transp);
	glLineWidth(attributes.borderThickness);
	glBegin(GL_LINE_LOOP);
	for(int i = 0; i <= sections;i++) { // make $section number of circles
		glVertex3d(lcx + rx * cos(i *  twoPi / sections), 
			lcy + ry* sin(i * twoPi / sections), z);
	}
	glEnd();
}

void PointDrawer::drawLine(const SVGAttributes& attributes, double z) const{
	Color scolor = attributes.strokeColor == colorUNDEF ? getDrawColor() :  attributes.strokeColor;
	double transp = attributes.opacity * getTransparency();
	glColor4d(scolor.redP(), scolor.greenP(), scolor.blueP(), transp);


	glLineWidth(attributes.borderThickness);
	glBegin(GL_LINE_STRIP);
	for(int i=0; i < attributes.points.size(); ++i) {
		glVertex3d(attributes.points[i].x, attributes.points[i].y, z);
	}

    glEnd();

}

void PointDrawer::drawPolygon(const SVGAttributes& attributes, double z) const{

	Color fcolor = attributes.fillColor == colorUSERDEF ? getDrawColor() : attributes.fillColor;
	double transp = attributes.opacity * getTransparency();
	glColor4f(fcolor.redP(),fcolor.greenP(), fcolor.blueP(), transp);
	for(int i=0; i < attributes.triangles.size() / 3; ++i) {
		Coord c1 = attributes.triangles[3*i];
		Coord c2 = attributes.triangles[3*i + 1];
		Coord c3 = attributes.triangles[3*i + 2];
		glBegin(GL_TRIANGLES);
			glVertex3d(c1.x,c1.y,0);
			glVertex3d(c2.x,c2.y,0);
			glVertex3d(c3.x,c3.y,0);
		glEnd();
	}

}

void PointDrawer::drawPath(const SVGAttributes& attributes, double z) const{
}

void PointDrawer::shareVertices(vector<Coord *>& coords) {
	coords.push_back(&cNorm);
}

