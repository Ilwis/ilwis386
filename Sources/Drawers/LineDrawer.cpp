#include "Client\Headers\formelementspch.h"
#include "Client\FormElements\syscolor.h"
#include "Client\MapWindow\Drawers\DrawerContext.h"
#include "Client\Mapwindow\Drawers\RootDrawer.h"
#include "Client\Mapwindow\Drawers\SimpleDrawer.h" 
#include "Client\Ilwis.h"
#include "drawers\linedrawer.h"
#include "Client\Editors\Utils\line.h"
#include "Client\Mapwindow\Drawers\ComplexDrawer.h"
#include "Client\Mapwindow\Drawers\ZValueMaker.h"

using namespace ILWIS;

ILWIS::NewDrawer *createLineDrawer(DrawerParameters *parms) {
	return new LineDrawer(parms);
}

LineDrawer::LineDrawer(DrawerParameters *parms) : 
SimpleDrawer(parms,"LineDrawer")
{
}

LineDrawer::LineDrawer(DrawerParameters *parms, const String& name) : 
SimpleDrawer(parms,name)
{
}

LineDrawer::~LineDrawer() {
	clear();
}

void LineDrawer::clear() {
	for(int i = 0; i < lines.size(); ++i) {
		delete lines.at(i);
	}
	lines.clear();
	cb = CoordBounds();
}

void LineDrawer::addCoords(const vector<Coord>& v,int options) {
	if ( options == 1){
		clear();
	}
	CoordinateSequence *seq = new CoordinateArraySequence();
	for(int i = 0; i < v.size(); ++i) {
		Coord c = v.at(i);
		seq->add(v.at(i));
		cb+= c;
	}
	lines.push_back(seq);
}

int LineDrawer::openGLLineStyle(int linestyle, double sz){
	switch(linestyle) {
		case ldtDot:
			return 0xAAAA;
		case ldtDash:
			return 0xF0F0;
		case ldtDashDot:
			return 0x6B5A;
		case ldtDashDotDot:
			return 0x56B5;
		default:
			return 0xFFFF;
	}
	return 0xFFFF;
}

int LineDrawer::ilwisLineStyle(int linestyle, double sz){
	switch(linestyle) {
		case 0xAAAA:
			return ldtDot;
		case 0xF0F0:
			return ldtDash;
		case 0x6B5A:
			return ldtDashDot;
		case 0x56B5:
			return ldtDashDotDot;
		default:
			return ldtSingle;
	}
	return 0xFFFF;
}

bool LineDrawer::draw(bool norecursion , const CoordBounds& cbArea) const{
	if (lines.size() == 0)
		return false;
	if ( !getRootDrawer()->getCoordBoundsZoom().fContains(cb))
		return false;

	CoordBounds cbZoom = getRootDrawer()->getCoordBoundsZoom();

	bool extrusion = getSpecialDrawingOption(NewDrawer::sdoExtrusion);
	bool filledExtr = getSpecialDrawingOption(NewDrawer::sdoFilled);
	ComplexDrawer *cdrw = (ComplexDrawer *)getParentDrawer();
	bool is3D = getRootDrawer()->is3D() && cdrw->getZMaker()->getThreeDPossible();
	double zscale, zoffset, fakez=0;


	glColor4f(lproperties.drawColor.redP(),lproperties.drawColor.greenP(), lproperties.drawColor.blueP(), getTransparency());
	glLineWidth(lproperties.thickness);
	if (lproperties.linestyle != 0xFFFF) {
		glEnable (GL_LINE_STIPPLE);
		glLineStipple(1,lproperties.linestyle);
	}
	if ( getRootDrawer()->is3D())
		fakez = getRootDrawer()->getFakeZ() *1.2;
	if ( is3D) {
		zscale = cdrw->getZMaker()->getZScale();
		zoffset = cdrw->getZMaker()->getOffset();
		glPushMatrix();
		glScaled(1,1,zscale);
		glTranslated(0,0,zoffset);
	}
	for(int j = 0; j < lines.size(); ++j) {
		CoordinateSequence *points = lines.at(j);
		if ( specialOptions & NewDrawer::sdoSELECTED) {
			glColor4d(1, 0, 0, 1);
			drawSelectedFeature(points, cbZoom, is3D);
			glColor4f(lproperties.drawColor.redP(),lproperties.drawColor.greenP(), lproperties.drawColor.blueP(), getTransparency());
		}
		glBegin(GL_LINE_STRIP);
		for(int i=0; i<points->size(); ++i) {
			Coordinate c = points->getAt(i);
			double z = is3D ? c.z : fakez;
			glVertex3d( c.x, c.y, z);
		}
		glEnd();
		if ( is3D) {
			if ( extrusion) {
				glColor4f(lproperties.drawColor.redP(),lproperties.drawColor.greenP(), lproperties.drawColor.blueP(), getTransparency());
				Coord cOld;
				Coord cStart = points->getAt(0);
				for(int i=0; i<points->size(); ++i) {
					Coordinate c = points->getAt(i);
					if ( !cOld.fUndef()) {
						glColor4f(lproperties.drawColor.redP(),lproperties.drawColor.greenP(), lproperties.drawColor.blueP(), getTransparency());
						drawExtrusion(cOld, c, fakez, filledExtr);
						glColor4f(lproperties.drawColor.redP(),lproperties.drawColor.greenP(), lproperties.drawColor.blueP(), getTransparency());
						glBegin(GL_LINE_STRIP);
						glVertex3d(c.x,c.y,c.z);
						glVertex3d(c.x,c.y,fakez);
						glEnd();
					}
					cOld = c;
				}
				//drawExtrusion(cOld, cStart, cOld.z, filledExtr); 
			}
		}
	}
	if ( is3D) {
		glPopMatrix();
	}
	glDisable (GL_LINE_STIPPLE);
	glLineWidth(1);
	return true;
}

void LineDrawer::drawSelectedFeature(CoordinateSequence *points, const CoordBounds& cbZoom, bool is3D) const{
	glLineWidth(lproperties.thickness * 1.2 + 3);
	glBegin(GL_LINE_STRIP);
	for(int i=0; i<points->size(); ++i) {
		Coordinate c = points->getAt(i);
		double z = is3D ? c.z : 0;
		glVertex3d( c.x, c.y, z);
	}
	glEnd();
	glLineWidth(lproperties.thickness);
	for(int i=0; i<points->size(); ++i) {
		Coordinate c = points->getAt(i);
		bool isPointSelected = findSelectedPoint(c);
		double symbolScale = cbZoom.width() / 200;
		double fz = is3D ? c.z : 0;;
		if ( isPointSelected) {
			glBegin(GL_QUADS);						
			glVertex3f( c.x - symbolScale, c.y - symbolScale,fz);	
			glVertex3f( c.x - symbolScale, c.y + symbolScale,fz);	
			glVertex3f( c.x + symbolScale, c.y + symbolScale,fz);
			glVertex3f( c.x + symbolScale, c.y - symbolScale,fz);
			glEnd();
		}
		else {
		}
		glBegin(GL_LINE_STRIP);						
			glVertex3f( c.x - symbolScale, c.y - symbolScale,fz);	
			glVertex3f( c.x - symbolScale, c.y + symbolScale,fz);	
			glVertex3f( c.x + symbolScale, c.y + symbolScale,fz);
			glVertex3f( c.x + symbolScale, c.y - symbolScale,fz);
			glVertex3f( c.x - symbolScale, c.y - symbolScale,fz);
		glEnd();
	}
}

bool LineDrawer::findSelectedPoint(const Coord& c) const{
	for(int i=0; i < selectedCoords.size(); ++i) {
		if ( selectedCoords.at(i) == c)
			return true;
	}
	return false;
}
void LineDrawer::prepare(PreparationParameters *p){
	SimpleDrawer::prepare(p);
}

void LineDrawer::shareVertices(vector<Coord *>& coords) {
	//coords.push_back(&cNorm);

	int count = 0;
	for(int j = 0; j < lines.size(); ++j) {
		CoordinateSequence *points = lines.at(j);
		count += points->size();
	}
	coords.resize(count);
	int index = 0;
	for(int j = 0; j < lines.size(); ++j) {
		CoordinateSequence *points = lines.at(j);
		for(int i = 0; i < points->size(); ++i) {
			const Coord *c = (const Coord *)&(points->getAt(i));
			coords[index++] = const_cast<Coord *>(c);
		}
			
	}
}

void LineDrawer::setSpecialDrawingOptions(int option, bool add, vector<Coord>* coords){
	SimpleDrawer::setSpecialDrawingOptions(option, add);
	selectedCoords.clear();
	if ( coords) {
		for(int n=0; n < coords->size(); ++n) {
			for(int j = 0; j < lines.size(); ++j) {
				CoordinateSequence *points = lines.at(j);
				for(int i = 0; i < points->size(); ++i) {
					const Coord *c = (const Coord *)&(points->getAt(i));
					if ( c->equals(coords->at(n)))
						selectedCoords.push_back(coords->at(n));
				}
				
			}
		}
	}
	else {
		selectedCoords.clear();
	}

}

GeneralDrawerProperties *LineDrawer::getProperties() {
	return &lproperties;
}





