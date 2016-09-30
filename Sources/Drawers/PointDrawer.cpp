#include "Headers\toolspch.h"
#include "Engine\Map\basemap.h"
#include "Engine\Drawers\SimpleDrawer.h"
#include "Engine\Drawers\ComplexDrawer.h"
#include "Drawers\LayerDrawer.h"
#include "Engine\Drawers\RootDrawer.h"
#include "Drawers\DrawingColor.h" 
#include "Engine\Drawers\SVGElements.h"
//#include "Drawers\SVGDrawers.h"
#include "drawers\pointdrawer.h"
#include "Engine\Drawers\ComplexDrawer.h"
#include "Engine\Drawers\SVGLoader.h"
#include "Engine\Drawers\SVGElements.h"
#include "Engine\Drawers\SVGPath.h"
#include "Engine\Drawers\OpenGLText.h"
#include "Engine\Drawers\TextDrawer.h"
#include "Engine\Drawers\ZValueMaker.h"
#include "Engine\Drawers\DrawerContext.h"

using namespace ILWIS;


ILWIS::NewDrawer *createPointDrawer(DrawerParameters *parms) {
	return new PointDrawer(parms);
}

PointDrawer::PointDrawer(DrawerParameters *parms) : SimpleDrawer(parms,"PointDrawer"), element(0){
	properties.symbol = DEFAULT_POINT_SYMBOL_TYPE;
}

PointDrawer::PointDrawer(DrawerParameters *parms, const String& name) : SimpleDrawer(parms,name), element(0){
	properties.symbol = DEFAULT_POINT_SYMBOL_TYPE;
}

PointDrawer::~PointDrawer() {
}

void PointDrawer::setGeneralProperties(GeneralDrawerProperties *prop){
	properties.set((PointProperties *)prop);
}

void PointDrawer::prepare(PreparationParameters *p){
	SimpleDrawer::prepare(p);
	if ( p->type & NewDrawer::ptRENDER || p->type & NewDrawer::ptRESTORE) {
		if ( p ) {
			properties.symbol = p->props.symbolType;
			properties.scale = p->props.symbolSize / 100.0;
		}
		const SVGLoader *loader = NewDrawer::getSvgLoader();
		if ( properties.symbol != ""){
			SVGLoader::const_iterator cur = loader->find(properties.symbol);
			bool isEnd = cur == loader->end();
			if ( isEnd == false) {
				bool isHatch = (*cur).second->getType() == IVGElement::ivgHATCH;
				if (isEnd || isHatch)
					return;
		
				element = (*cur).second;
			} else { // fall back to default
				SVGLoader::const_iterator iter = loader->find(DEFAULT_POINT_SYMBOL_TYPE);
				element = (*iter).second;
			}
			calcSize();
		} else {
			SVGLoader::const_iterator iter = loader->find(DEFAULT_POINT_SYMBOL_TYPE);
			element = (*iter).second;
		}
	}
}

void PointDrawer::select(bool yesno) {
	if (yesno)
		specialOptions |= NewDrawer::sdoSELECTED;
	else
		specialOptions &= ~NewDrawer::sdoSELECTED;
}

void PointDrawer::setCoord(const Coord& crd) {
	cNorm = crd;
}

void PointDrawer::calcSize() {
	width = 0;
	for(vector<IVGAttributes *>::const_iterator cur = element->begin(); cur != element->end(); ++cur) {
		width = max(width, (*cur)->rwidth);	
	}
	height = 0;
	for(vector<IVGAttributes *>::const_iterator cur = element->begin(); cur != element->end(); ++cur) {
		height = max(height, (*cur)->rheight);	
	}
	CoordBounds cbZoom = getRootDrawer()->getCoordBoundsZoom();
	if (width == 0)
		width = cbZoom.width() / 100.0;
	if (height == 0)
		height = cbZoom.width() / 100.0;
}

bool PointDrawer::draw(const DrawLoop drawLoop, const CoordBounds& cbArea) const {
	if ( !isActive() || !isValid())
		return false;

	if ( cNorm.fUndef())
		return false;
	const CoordBounds& cbZoom = getRootDrawer()->getCoordBoundsZoom();
	if ( !cbZoom.fContains(cNorm))
			return false;
	if ( element != 0 ) {
		bool extrusion = getSpecialDrawingOption(NewDrawer::sdoExtrusion);
		bool filledExtr = getSpecialDrawingOption(NewDrawer::sdoFilledPlain);

		ComplexDrawer *cdrw = (ComplexDrawer *)getParentDrawer();
		ZValueMaker *zvmkr = cdrw->getZMaker();
		bool is3D = getRootDrawer()->is3D();// && zvmkr->getThreeDPossible();
		bool is3DPossible = cdrw->getZMaker()->getThreeDPossible() && !isSupportingDrawer;

		double zscale = zvmkr->getZScale();
		double zoffset = zvmkr->getOffset();

		double fx = cNorm.x;
		double fy = cNorm.y;
		double fz = (is3D && is3DPossible) ? cNorm.z * zscale : 0;

		if ( label && label->getParentDrawer()->isActive()) {
			Coord c = label->coord();
			double xshift = cbZoom.width() / 250.0;
			double yshift = cbZoom.height() / 250.0;
			c += Coord(xshift,yshift);
			c.z = fz;
			label->setCoord(c);
		}

		double symbolScale = cbZoom.width() / 200;
		CoordBounds cb(Coord(fx - symbolScale, fy - symbolScale,fz), Coord(fx + symbolScale, fy + symbolScale,fz));
		CoordBounds localCb = element->getCb();
		double f = localCb.width() > 0 ? localCb.height() / localCb.width()  : 1.0;

		double xscale = cb.width() / width;
		double yscale = f * cb.height() / height;

		glPushMatrix();
		glTranslated(fx,fy,fz + zoffset);
		glScaled(xscale * properties.scaling() * element->getDefaultScale(), -yscale * properties.scaling() * element->getDefaultScale(), zscale ); // opengl's coordinate system is mirrored vertcally compared to svg
		glRotated(properties.angle,0,0,100);
		if ( properties.threeDOrientation){
			glTranslated(0,0,symbolScale);
			glRotated(-90,100,0,0);
		}

		map<IVGAttributes*, GLuint> & SVGSymbolDisplayListAreas = getRootDrawer()->getDrawerContext()->getSVGSymbolDisplayListAreas();
		map<IVGAttributes*, GLuint> & SVGSymbolDisplayListContours = getRootDrawer()->getDrawerContext()->getSVGSymbolDisplayListContours();		

		for(vector<IVGAttributes *>::const_iterator cur = element->begin(); cur != element->end(); ++cur) {

			double alpha = (*cur)->opacity * getAlpha();
			if ((drawLoop == drl3DOPAQUE && alpha != 1.0) || (drawLoop == drl3DTRANSPARENT && alpha == 1.0))
				continue;

			GLuint & displayListArea = SVGSymbolDisplayListAreas[*cur];
			GLuint & displayListContour = SVGSymbolDisplayListContours[*cur];

			glLineWidth(properties.thickness!= 0 ? properties.thickness : (*cur)->strokewidth);
			Color fcolor = (*cur)->fillColor.fEqual(colorUSERDEF) ? properties.drawColor : (*cur)->fillColor;
			Color scolor = (*cur)->strokeColor.fEqual(colorUSERDEF) ? properties.drawColor : (*cur)->strokeColor;
			switch((*cur)->type) {
				case IVGAttributes::sCIRCLE:
				case IVGAttributes::sELLIPSE:
					{
						glPushMatrix();
						double rx = (*cur)->rx > 0 ? (*cur)->rx : width / 2.0;
						double ry = (*cur)->ry > 0 ? (*cur)->ry : height / 2.0;
						double lcx = (*cur)->points[0].x;
						double lcy = (*cur)->points[0].y;
						glTranslated(lcx, lcy, 0);
						if ( (*cur)->type == IVGAttributes::sCIRCLE) {
							double r = min(rx, ry);
							glScaled(r, r, 0);
						} else
							glScaled(rx, ry, 0);

						if (!fcolor.fEqual(colorUNDEF)) {
							glColor4d(fcolor.redP(), fcolor.greenP(), fcolor.blueP(), alpha);
							if (displayListArea != 0)
								glCallList(displayListArea);
							else {
								displayListArea = glGenLists(1);
								glNewList(displayListArea, GL_COMPILE_AND_EXECUTE);
								drawCircleArea(*cur);
								glEndList();
							}
						}
						if (!scolor.fEqual(colorUNDEF)) {
							glColor4d(scolor.redP(), scolor.greenP(), scolor.blueP(), alpha);
							if (displayListContour != 0)
								glCallList(displayListContour);
							else {
								displayListContour = glGenLists(1);
								glNewList(displayListContour, GL_COMPILE_AND_EXECUTE);
								drawCircleBoundaries(*cur);
								glEndList();
							}
						}

						glPopMatrix();
					}
					break;
				case IVGAttributes::sRECTANGLE:
					{
						glPushMatrix();

						if ( (*cur)->transformations.size() > 0) // is rectangle the only itemtype that can have transformations??
							transform((*cur));

						double hw = ((*cur)->rwidth != 0 ? (*cur)->rwidth : width) / 2.0;
						double hh = ((*cur)->rheight != 0 ? (*cur)->rheight : height) / 2.0;
						Coord center = (*cur)->bounds.middle();
						glTranslated(center.x, center.y, 0);
						glScaled(hw, hh, 0);

						if (!fcolor.fEqual(colorUNDEF)) {
							glColor4d(fcolor.redP(), fcolor.greenP(), fcolor.blueP(), alpha);
							if (displayListArea != 0)
								glCallList(displayListArea);
							else {
								displayListArea = glGenLists(1);
								glNewList(displayListArea, GL_COMPILE_AND_EXECUTE);
								drawRectangleArea(*cur);
								glEndList();
							}
						}
						if (!scolor.fEqual(colorUNDEF)) {
							glColor4d(scolor.redP(), scolor.greenP(), scolor.blueP(), alpha);
							if (displayListContour != 0)
								glCallList(displayListContour);
							else {
								displayListContour = glGenLists(1);
								glNewList(displayListContour, GL_COMPILE_AND_EXECUTE);
								drawRectangleBoundaries(*cur);
								glEndList();
							}
						}

						glPopMatrix();
					}
					break;
				case IVGAttributes::sPOLYGON:
					{
						if (!fcolor.fEqual(colorUNDEF)) {
							glColor4f(fcolor.redP(),fcolor.greenP(), fcolor.blueP(), alpha);
							if (displayListArea != 0)
								glCallList(displayListArea);
							else {
								displayListArea = glGenLists(1);
								glNewList(displayListArea, GL_COMPILE_AND_EXECUTE);
								drawPolygonArea(*cur);
								glEndList();
							}
						}
						if (!scolor.fEqual(colorUNDEF)) {
							glColor4f(scolor.redP(),scolor.greenP(), scolor.blueP(), alpha);
							if (displayListContour != 0)
								glCallList(displayListContour);
							else {
								displayListContour = glGenLists(1);
								glNewList(displayListContour, GL_COMPILE_AND_EXECUTE);
								drawPolygonBoundaries(*cur);
								glEndList();
							}
						}
					}
					break;
				case IVGAttributes::sLINE:
				case IVGAttributes::sPOLYLINE:
					{
						if (!scolor.fEqual(colorUNDEF)) {
							glColor4d(scolor.redP(), scolor.greenP(), scolor.blueP(), alpha);
							if (displayListContour != 0)
								glCallList(displayListContour);
							else {
								displayListContour = glGenLists(1);
								glNewList(displayListContour, GL_COMPILE_AND_EXECUTE);
								drawLine(*cur);
								glEndList();
							}
						}
					}
					break;
				case IVGAttributes::sPATH:
					{
						if ((*cur)->isPolygon()) {
							if (!fcolor.fEqual(colorUNDEF)) {
								glColor4f(fcolor.redP(),fcolor.greenP(), fcolor.blueP(), alpha);
								if (displayListArea != 0)
									glCallList(displayListArea);
								else {
									displayListArea = glGenLists(1);
									glNewList(displayListArea, GL_COMPILE_AND_EXECUTE);
									drawPolygonArea(*cur);
									glEndList();
								}
							}
							if (!scolor.fEqual(colorUNDEF)) {
								glColor4f(scolor.redP(),scolor.greenP(), scolor.blueP(), alpha);
								if (displayListContour != 0)
									glCallList(displayListContour);
								else {
									displayListContour = glGenLists(1);
									glNewList(displayListContour, GL_COMPILE_AND_EXECUTE);
									drawPolygonBoundaries(*cur);
									drawPath(*cur);
									glEndList();
								}
							}
						}
						else if (!scolor.fEqual(colorUNDEF)) {
							glColor4f(scolor.redP(),scolor.greenP(), scolor.blueP(), alpha); // was fcolor, but probably wrong
							if (displayListContour != 0)
								glCallList(displayListContour);
							else {
								displayListContour = glGenLists(1);
								glNewList(displayListContour, GL_COMPILE_AND_EXECUTE);
								drawPath((*cur));
								glEndList();
							}
						}
					}
					break;			
			}
		}
		glPopMatrix();

		if ( specialOptions & NewDrawer::sdoSELECTED) {
			if ((drawLoop == drl2D) || (drawLoop == drl3DOPAQUE)) {
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
		}

		if ( is3D) {
			if ( extrusion) {
				double alpha = getAlpha() * element->at(element->size() - 1)->opacity; // transparency of extrusion is the transparency of the last element
				if ((drawLoop == drl2D) || (drawLoop == drl3DOPAQUE && alpha == 1.0) || (drawLoop == drl3DTRANSPARENT && alpha != 1.0)) {
					glBegin(GL_LINE_STRIP) ;
					glVertex3d(cNorm.x,cNorm.y,0);
					glVertex3d(cNorm.x, cNorm.y,fz*zvmkr->getZScale());
					glEnd();
				}
			}
		}
	}

	return true;
}

void PointDrawer::transform(const IVGAttributes* attributes) const{
	for(int i=0; i < attributes->transformations.size(); ++i) {
		const Transform& tr = attributes->transformations.at(i);
		if ( tr.type == Transform::tROTATE) {
			if (tr.parameters.size() == 1) {
				glRotated(tr.parameters[0], 0,0,100);
			}
			else
				glRotated(tr.parameters[0],tr.parameters[1],tr.parameters[2],100);
		} else if ( tr.type == Transform::tTRANSLATE) {
			if (tr.parameters.size() == 1)
				glTranslated(tr.parameters[0],0,0);
			else
				glTranslated(tr.parameters[0],tr.parameters[1],0);
		} else if ( tr.type == Transform::tSCALE) {
			if (tr.parameters.size() == 1)
				glScaled(tr.parameters[0],0,0);
			else
				glScaled(tr.parameters[0],tr.parameters[1],0);
		} else if ( tr.type == Transform::tMATRIX) {
		}
	}
}

void PointDrawer::drawRectangleArea(const IVGAttributes* attributes) const {
	glBegin(GL_QUADS);						
	glVertex3f( -1.0, -1.0, 0.0);	
	glVertex3f( -1.0,  1.0, 0.0);	
	glVertex3f(  1.0,  1.0, 0.0);
	glVertex3f(  1.0, -1.0, 0.0);	
	glEnd();
}

void PointDrawer::drawRectangleBoundaries(const IVGAttributes* attributes) const {
	glBegin(GL_LINE_STRIP);						
	glVertex3f( -1.0, -1.0, 0.0);	
	glVertex3f( -1.0,  1.0, 0.0);	
	glVertex3f(  1.0,  1.0, 0.0);
	glVertex3f(  1.0, -1.0, 0.0);
	glVertex3f( -1.0, -1.0, 0.0);	
	glEnd();
}

void PointDrawer::drawCircleArea(const IVGAttributes* attributes) const {
	const int sections = 20; //number of triangles to use to estimate a circle (a higher number yields a more perfect circle)
	const double twoPi =  2.0 * M_PI;

	glBegin(GL_TRIANGLE_FAN);
	glVertex3d(0, 0, 0); // origin
	for(int i = 0; i <= sections;i++) // make $section number of circles
		glVertex3d(cos(i *  twoPi / sections), sin(i * twoPi / sections), 0);
	glEnd();
}

void PointDrawer::drawCircleBoundaries(const IVGAttributes* attributes) const {
	const int sections = 20; //number of triangles to use to estimate a circle (a higher number yields a more perfect circle)
	const double twoPi =  2.0 * M_PI;

	glBegin(GL_LINE_LOOP);
	for(int i = 0; i <= sections;i++) // make $section number of circles
		glVertex3d(cos(i *  twoPi / sections), sin(i * twoPi / sections), 0.0);
	glEnd();
}

void PointDrawer::drawLine(const IVGAttributes* attributes) const {
	glBegin(GL_LINE_STRIP);
	for(int i=0; i < attributes->points.size(); ++i) {
		glVertex3d(attributes->points[i].x , attributes->points[i].y, 0.0);
	}
	glEnd();
}

void PointDrawer::drawPolygonArea(const IVGAttributes* attributes) const {
	for(int i=0; i < attributes->triangleStrips.size(); ++i){
		glBegin(attributes->triangleStrips[i].first);
		for(int j=0; j < attributes->triangleStrips[i].second.size(); ++j) {
			Coord c = attributes->triangleStrips[i].second[j];
			glVertex3d(c.x,c.y,0.0);
		}
		glEnd();
	}
}

void PointDrawer::drawPolygonBoundaries(const IVGAttributes* attributes) const {
	glBegin(GL_LINE_STRIP);
	for(int i=0; i < attributes->points.size(); ++i)
		glVertex3d(attributes->points[i].x , attributes->points[i].y , 0.0);
	glEnd();
}

void PointDrawer::drawPath(const IVGAttributes* attributes) const{
	const SVGPath& path = (const SVGPath&) attributes;
	for(int i = 0; i < path.noOfElements(); ++i) {
		PathElement el = path.getElement(i);
		if ( el.type == PathElement::eLINE) {
			glBegin(GL_LINE_STRIP);
			for(int j = el.start; j <= el.end; ++j)
				glVertex3d(attributes->points[j].x, attributes->points[j].y, 0.0);
			glEnd();
		}
	}
}

void PointDrawer::shareVertices(vector<Coord *>& coords) {
	coords.push_back(&cNorm);
}

GeneralDrawerProperties *PointDrawer::getProperties() {
	return &properties;
}

//----------------------------------------------
String PointProperties::store(const FileName& fnView, const String& parentSection) const{
	ObjectInfo::WriteElement(parentSection.c_str(),"Thickness",fnView, thickness);
	ObjectInfo::WriteElement(parentSection.c_str(),"Symbol",fnView, symbol);
	ObjectInfo::WriteElement(parentSection.c_str(),"DrawColor",fnView, drawColor);
	ObjectInfo::WriteElement(parentSection.c_str(),"IgnoreColor",fnView, ignoreColor);
	ObjectInfo::WriteElement(parentSection.c_str(),"Scale",fnView, scale);
	ObjectInfo::WriteElement(parentSection.c_str(),"ScaleMode",fnView, scaleMode);
	ObjectInfo::WriteElement(parentSection.c_str(),"RadiusArea",fnView, radiusArea);
	ObjectInfo::WriteElement(parentSection.c_str(),"UseDirection",fnView, (long)useDirection);
	ObjectInfo::WriteElement(parentSection.c_str(),"StretchScale",fnView, stretchScale);
	ObjectInfo::WriteElement(parentSection.c_str(),"StretchRange",fnView, stretchRange);
	ObjectInfo::WriteElement(parentSection.c_str(),"StretchColumn",fnView, stretchColumn);
	ObjectInfo::WriteElement(parentSection.c_str(),"Exaggeration",fnView, exaggeration);
	ObjectInfo::WriteElement(parentSection.c_str(),"Angle",fnView, angle);
	

	return parentSection;
}

void PointProperties::load(const FileName& fnView, const String& parentSection){
	ObjectInfo::ReadElement(parentSection.c_str(),"Thickness",fnView, thickness);
	ObjectInfo::ReadElement(parentSection.c_str(),"Symbol",fnView, symbol);
	ObjectInfo::ReadElement(parentSection.c_str(),"DrawColor",fnView, drawColor);
	ObjectInfo::ReadElement(parentSection.c_str(),"IgnoreColor",fnView, ignoreColor);
	ObjectInfo::ReadElement(parentSection.c_str(),"Scale",fnView, scale);
	long m;
	ObjectInfo::ReadElement(parentSection.c_str(),"ScaleMode",fnView, m);
	scaleMode = (Scaling)m;
	ObjectInfo::ReadElement(parentSection.c_str(),"RadiusArea",fnView, m);
	radiusArea = (RadiusArea)m;
	ObjectInfo::ReadElement(parentSection.c_str(),"UseDirection",fnView, useDirection);
	ObjectInfo::ReadElement(parentSection.c_str(),"StretchScale",fnView, stretchScale);
	ObjectInfo::ReadElement(parentSection.c_str(),"StretchRange",fnView, stretchRange);
	ObjectInfo::ReadElement(parentSection.c_str(),"StretchColumn",fnView, stretchColumn);
	ObjectInfo::ReadElement(parentSection.c_str(),"Exaggeration",fnView, exaggeration);
	ObjectInfo::ReadElement(parentSection.c_str(),"Angle",fnView, angle);
	
}