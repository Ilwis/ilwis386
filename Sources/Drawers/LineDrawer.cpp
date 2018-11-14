#include "Headers\toolspch.h"
#include "Engine\Map\basemap.h"
#include "Engine\Drawers\DrawerContext.h"
#include "Engine\Drawers\RootDrawer.h"
#include "Engine\Drawers\SimpleDrawer.h" 
#include "drawers\linedrawer.h"
#include "Engine\Drawers\ComplexDrawer.h"
#include "Engine\Drawers\ZValueMaker.h"

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

void LineDrawer::select(bool yesno) {
	if (yesno)
		specialOptions |= NewDrawer::sdoSELECTED;
	else
		specialOptions &= ~NewDrawer::sdoSELECTED;
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

bool LineDrawer::draw(const DrawLoop drawLoop, const CoordBounds& cbArea) const{
	if ( !isActive() && !isValid())
		return false;

	if (lines.size() == 0)
		return false;
	if ( !getRootDrawer()->getCoordBoundsZoom().fContains(cb))
		return false;

	CoordBounds cbZoom = getRootDrawer()->getCoordBoundsZoom();

	ComplexDrawer *cdrw = (ComplexDrawer *)getParentDrawer();
	bool is3D = getRootDrawer()->is3D(); 
	bool is3DPossible = cdrw->getZMaker()->getThreeDPossible();
	double zscale, zoffset;
	double alpha = getAlpha();

	glColor4f(lproperties.drawColor.redP(),lproperties.drawColor.greenP(), lproperties.drawColor.blueP(),alpha );
	glLineWidth(lproperties.thickness);
	if (lproperties.thickness != 1.0) {
		glEnable(GL_LINE_SMOOTH);
		glHint(GL_LINE_SMOOTH_HINT,GL_NICEST);
		glHint(GL_POLYGON_SMOOTH_HINT,GL_NICEST);
	}

	if (lproperties.linestyle != 0xFFFF) {
		glEnable(GL_LINE_STIPPLE);
		glLineStipple(1,lproperties.linestyle);
	}

	if ( is3D) {
		zscale = cdrw->getZMaker()->getZScale();
		zoffset = cdrw->getZMaker()->getOffset();
		glPushMatrix();
		glScaled(1,1,zscale);
		glTranslated(0,0,zoffset);
	}
	for(int j = 0; j < lines.size(); ++j) {
		CoordinateSequence *points = lines.at(j);
		if ( specialOptions & NewDrawer::sdoSELECTED || selectedCoords.size() > 0) {
			drawSelectedFeature(points, drawLoop, cbZoom, is3D);
			glColor4f(lproperties.drawColor.redP(),lproperties.drawColor.greenP(), lproperties.drawColor.blueP(),alpha );
		}
		if ((drawLoop == drl2D) || (drawLoop == drl3DOPAQUE && alpha == 1.0) || (drawLoop == drl3DTRANSPARENT && alpha != 1.0)) {
			glBegin(GL_LINE_STRIP);
			for(int i=0; i<points->size(); ++i) {
				Coordinate c = points->getAt(i);
				double z = is3D && is3DPossible ? c.z : 0;
				glVertex3d( c.x, c.y, z);
			}
			glEnd();
			if ( specialOptions & NewDrawer::sdoSymbolLineNode) {
				double symbolScale = cbZoom.width() / 250;
				for(int i=0; i<points->size(); ++i) {
					Coordinate c = points->getAt(i);
					double z = is3D && is3DPossible ? c.z : 0;
					glBegin(GL_LINE_STRIP);						
					glVertex3f( c.x - symbolScale, c.y - symbolScale,z);	
					glVertex3f( c.x - symbolScale, c.y + symbolScale,z);	
					glVertex3f( c.x + symbolScale, c.y + symbolScale,z);
					glVertex3f( c.x + symbolScale, c.y - symbolScale,z);
					glVertex3f( c.x - symbolScale, c.y - symbolScale,z);
					glEnd();
				}

			}
		}
		if ( is3D ) {
			if (((specialOptions & NewDrawer::sdoExtrusion) != 0) && ((drawLoop == drl2D) || (drawLoop == drl3DOPAQUE && extrAlpha == 1.0) || (drawLoop == drl3DTRANSPARENT && extrAlpha != 1.0))) {
				Coord cOld;
				Coord cStart = points->getAt(0);
				for(int i=0; i<points->size(); ++i) {
					Coordinate c = points->getAt(i);
					if ( !cOld.fUndef()) {
						glColor4f(lproperties.drawColor.redP(),lproperties.drawColor.greenP(), lproperties.drawColor.blueP(), extrAlpha);
						drawExtrusion(cOld, c, -zoffset, specialOptions);
					}
					cOld = c;
				}
			}
		}
	}
	if ( is3D) {
		glPopMatrix();
	}

	glDisable(GL_LINE_STIPPLE);
	if (lproperties.thickness != 1.0) {
		glDisable(GL_LINE_SMOOTH);
	}

	glLineWidth(1);
	return true;
}

void LineDrawer::drawSelectedFeature(CoordinateSequence *points, const DrawLoop drawLoop, const CoordBounds& cbZoom, bool is3D) const{
	Color clr =  parentDrawer->isSimple() ? Color(1,0,0) : ((ComplexDrawer *)parentDrawer)->getSelectionColor();
	if ((drawLoop == drl2D) || (drawLoop == drl3DOPAQUE)) {
		glColor4d(clr.redP(), clr.greenP(), clr.blueP(), 1);
		glLineWidth(lproperties.thickness * 1.2 + 3);
		glBegin(GL_LINE_STRIP);
		for(int i=0; i<points->size(); ++i) {
			Coordinate c = points->getAt(i);
			double z = is3D ? c.z : 0;
			glVertex3d( c.x, c.y, z);
		}
		glEnd();
	}

	glLineWidth(lproperties.thickness);
	double symbolScale = cbZoom.width() / 250;
	if ((drawLoop == drl2D) || (drawLoop == drl3DTRANSPARENT)) {
		glColor4d(clr.redP(), clr.greenP(), clr.blueP(), 0.5);
		for(int i=0; i < selectedCoords.size(); ++i) {
			Coordinate c = points->getAt(selectedCoords.at(i));
			double fz = is3D ? c.z : 0;;
			glBegin(GL_QUADS);						
				glVertex3f( c.x - symbolScale, c.y - symbolScale,fz);	
				glVertex3f( c.x - symbolScale, c.y + symbolScale,fz);	
				glVertex3f( c.x + symbolScale, c.y + symbolScale,fz);
				glVertex3f( c.x + symbolScale, c.y - symbolScale,fz);
			glEnd();
		}
	}
	if ((drawLoop == drl2D) || (drawLoop == drl3DOPAQUE)) {
		glColor4d(clr.redP(), clr.greenP(), clr.blueP(), 1);
		if (parentDrawer->inEditMode()) {
			for(int i=0; i<points->size(); ++i) {
				Coordinate c = points->getAt(i);
				double fz = is3D ? c.z : 0;;
				glBegin(GL_LINE_STRIP);						
				glVertex3f( c.x - symbolScale, c.y - symbolScale,fz);	
				glVertex3f( c.x - symbolScale, c.y + symbolScale,fz);	
				glVertex3f( c.x + symbolScale, c.y + symbolScale,fz);
				glVertex3f( c.x + symbolScale, c.y - symbolScale,fz);
				glVertex3f( c.x - symbolScale, c.y - symbolScale,fz);
				glVertex3f( c.x + symbolScale, c.y + symbolScale,fz);
				glVertex3f( c.x + symbolScale, c.y - symbolScale,fz);
				glVertex3f( c.x - symbolScale, c.y + symbolScale,fz);	
				glEnd();
			}
		}
		glColor4d(0,0,0,1);
		if ( parentDrawer->inEditMode()) {
			for(int i=0; i < selectedCoords.size(); ++i) {
				Coordinate c = points->getAt(selectedCoords.at(i));
				double fz = is3D ? c.z : 0;;
				glBegin(GL_LINE_STRIP);						
					glVertex3f( c.x - symbolScale, c.y - symbolScale,fz);	
					glVertex3f( c.x - symbolScale, c.y + symbolScale,fz);	
					glVertex3f( c.x + symbolScale, c.y + symbolScale,fz);
					glVertex3f( c.x + symbolScale, c.y - symbolScale,fz);
					glVertex3f( c.x - symbolScale, c.y - symbolScale,fz);
					glVertex3f( c.x + symbolScale, c.y + symbolScale,fz);
					glVertex3f( c.x + symbolScale, c.y - symbolScale,fz);
					glVertex3f( c.x - symbolScale, c.y + symbolScale,fz);	
				glEnd();
			}
		}
	}
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

void LineDrawer::setSpecialDrawingOptions(int option, bool add, const vector<int>& coords){
	SimpleDrawer::setSpecialDrawingOptions(option, add);
	selectedCoords.clear();
	for(int n=0; n < coords.size(); ++n) {
		selectedCoords.push_back(coords.at(n));
	}
}

GeneralDrawerProperties *LineDrawer::getProperties() {
	return &lproperties;
}

int LineDrawer::openGLLineStyle(int linestyle, double sz){
	switch(linestyle) {
		case ldtDot:
			return 0xC0C0;
		case ldtDash:
			return 0xFF00;
		case ldtDashDot:
			return 0xFF18;
		case ldtDashDotDot:
			return 0xFCCC;
		default:
			return 0xFFFF;
	}
	return 0xFFFF;
}

int LineDrawer::ilwisLineStyle(int linestyle, double sz){
	switch(linestyle) {
		case 0xC0C0:
			return ldtDot;
		case 0xFF00:
			return ldtDash;
		case 0xFF18:
			return ldtDashDot;
		case 0xFCCC:
			return ldtDashDotDot;
		default:
			return ldtSingle;
	}
	return 0xFFFF;
}

//----------------------------------------
String LineProperties::store(const FileName& fnView, const String& parentSection) const{
	ObjectInfo::WriteElement(parentSection.c_str(),"Thickness",fnView, thickness);
	ObjectInfo::WriteElement(parentSection.c_str(),"LineStyle",fnView, linestyle);
	ObjectInfo::WriteElement(parentSection.c_str(),"DrawColor",fnView, drawColor);
	ObjectInfo::WriteElement(parentSection.c_str(),"IgnoreColor",fnView, ignoreColor);
	return parentSection;
}

void LineProperties::load(const FileName& fnView, const String& parentSection){
	ObjectInfo::ReadElement(parentSection.c_str(),"Thickness",fnView, thickness);
	ObjectInfo::ReadElement(parentSection.c_str(),"LineStyle",fnView, linestyle);
	ObjectInfo::ReadElement(parentSection.c_str(),"DrawColor",fnView, drawColor);
	ObjectInfo::ReadElement(parentSection.c_str(),"IgnoreColor",fnView, ignoreColor);
}



