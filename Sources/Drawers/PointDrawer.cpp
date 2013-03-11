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
	if ( element == 0)
		throw ErrorObject(TR("Unknow symbol used"));

	bool extrusion = getSpecialDrawingOption(NewDrawer::sdoExtrusion);
	bool filledExtr = getSpecialDrawingOption(NewDrawer::sdoFilled);

	ComplexDrawer *cdrw = (ComplexDrawer *)getParentDrawer();
	ZValueMaker *zvmkr = cdrw->getZMaker();
	bool is3D = getRootDrawer()->is3D();// && zvmkr->getThreeDPossible();
	bool is3DPossible = cdrw->getZMaker()->getThreeDPossible() && !isSupportingDrawer;
	double z0 = cdrw->getZMaker()->getZ0(is3D);

	double zscale = zvmkr->getZScale();
	double zoffset = zvmkr->getOffset();

	double fx = cNorm.x;
	double fy = cNorm.y;
	double fz = (is3D && is3DPossible) ? cNorm.z * zscale : z0;

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
	//double zscale = yscale;


	glPushMatrix();
	glTranslated(fx,fy,fz + zoffset);
	glScaled(xscale * properties.scaling() * element->getDefaultScale(), yscale *  properties.scaling() * element->getDefaultScale(), zscale );
	Coord cMid = localCb.middle();
	glScaled(1.0,-1.0,1.0); // opengl uses other oriented coordinate system then svg. have to mirror it.
	//glTranslated(-cMid.x, -cMid.y,0);
	glRotated(properties.angle,0,0,100);
	if ( properties.threeDOrientation){
		glTranslated(0,0,symbolScale);
		glRotated(-90,100,0,0);
	} 


	for(vector<IVGAttributes *>::const_iterator cur = element->begin(); cur != element->end(); ++cur) {
		switch((*cur)->type) {
			case IVGAttributes::sCIRCLE:
			case IVGAttributes::sELLIPSE:
				drawEllipse((*cur), 0, drawLoop);
				break;
			case IVGAttributes::sRECTANGLE:
				drawRectangle((*cur), 0, drawLoop);
				break;
			case IVGAttributes::sPOLYGON:
				drawPolygon((*cur), 0, drawLoop);
				break;
			case IVGAttributes::sLINE:
			case IVGAttributes::sPOLYLINE:
				drawLine((*cur), 0, drawLoop);
				break;
			case IVGAttributes::sPATH:
				drawPath((*cur),0, drawLoop);
				break;			
		};
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
			double transp = getTransparency() * element->at(element->size() - 1)->opacity; // transparency of extrusion is the transparency of the last element
			if ((drawLoop == drl2D) || (drawLoop == drl3DOPAQUE && transp == 1.0) || (drawLoop == drl3DTRANSPARENT && transp != 1.0)) {
				glBegin(GL_LINE_STRIP) ;
				glVertex3d(cNorm.x,cNorm.y,0);
				glVertex3d(cNorm.x, cNorm.y,fz*zvmkr->getZScale());
				glEnd();
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

void PointDrawer::drawRectangle(const IVGAttributes* attributes, double z, const DrawLoop drawLoop) const {
	double transp = getTransparency() * attributes->opacity;
	if ((drawLoop == drl3DOPAQUE && transp != 1.0) || (drawLoop == drl3DTRANSPARENT && transp == 1.0))
		return;

	double hw = attributes->rwidth != 0 ? attributes->rwidth : width;
	double hh = attributes->rheight != 0 ? attributes->rheight : height;
	hw /= 2.0;
	hh /= 2.0;
	if ( attributes->transformations.size() > 0) {
		glPushMatrix();
		transform(attributes);
	}

	Color fcolor = attributes->fillColor;
	if ( fcolor == colorUSERDEF)
		fcolor = properties.drawColor;

	Coord center = attributes->bounds.middle();
	if ( !fcolor.fEqual(colorUNDEF)) {
		glColor4d(fcolor.redP(), fcolor.greenP(), fcolor.blueP(), transp);
		glBegin(GL_QUADS);						
		glVertex3f( center.x - hw, center.y - hw, z);	
		glVertex3f( center.x - hw,  center.y + hw,z);	
		glVertex3f( center.x + hw,  center.y + hw,z);
		glVertex3f( center.x + hw,  center.y - hw,z);
		glEnd();
	}
	glLineWidth(properties.thickness!= 0 ? properties.thickness : attributes->strokewidth);
	Color scolor = attributes->strokeColor.fEqual(colorUSERDEF) ? properties.drawColor :  attributes->strokeColor;
	glColor4d(scolor.redP(), scolor.greenP(), scolor.blueP(), transp);
	glBegin(GL_LINE_STRIP);						
		glVertex3f( center.x - hw, center.y - hw, z);	
		glVertex3f( center.x - hw,  center.y + hw,z);	
		glVertex3f( center.x + hw,  center.y + hw,z);
		glVertex3f( center.x + hw,  center.y - hw,z);
		glVertex3f( center.x - hw, center.y - hw, z);	
	glEnd();

	if ( attributes->transformations.size() > 0) {
		glPopMatrix();
	}
}

void PointDrawer::drawEllipse(const IVGAttributes* attributes, double z, const DrawLoop drawLoop) const{
	double transp = attributes->opacity * getTransparency();
	if ((drawLoop == drl3DOPAQUE && transp != 1.0) || (drawLoop == drl3DTRANSPARENT && transp == 1.0))
		return;

	double rx = attributes->rx > 0 ? attributes->rx : width / 2;
	double ry = attributes->ry >0 ? attributes->ry : height / 2;
	double lcx = attributes->points[0].x;
	double lcy = attributes->points[0].y;
	double r = min(rx,ry);

	Color fcolor = attributes->fillColor.fEqual(colorUSERDEF) ? properties.drawColor : attributes->fillColor;

	int sections = 20; //number of triangles to use to estimate a circle
	// (a higher number yields a more perfect circle)
	double twoPi =  2.0 * M_PI;
	if ( attributes->type == IVGAttributes::sCIRCLE)
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

	Color scolor = attributes->strokeColor.fEqual(colorUSERDEF) ? properties.drawColor :  attributes->strokeColor;
	glColor4d(scolor.redP(), scolor.greenP(), scolor.blueP(), transp);
	glLineWidth(properties.thickness!= 0 ? properties.thickness : attributes->strokewidth);
	glBegin(GL_LINE_LOOP);
	for(int i = 0; i <= sections;i++) { // make $section number of circles
		glVertex3d(lcx + rx * cos(i *  twoPi / sections), 
			lcy + ry* sin(i * twoPi / sections), z);
	}
	glEnd();
}

void PointDrawer::drawLine(const IVGAttributes* attributes, double z, const DrawLoop drawLoop) const{
	double transp = attributes->opacity * getTransparency();
	if ((drawLoop == drl3DOPAQUE && transp != 1.0) || (drawLoop == drl3DTRANSPARENT && transp == 1.0))
		return;
	Color scolor = attributes->strokeColor.fEqual(colorUSERDEF) ? properties.drawColor :  attributes->strokeColor;
	glColor4d(scolor.redP(), scolor.greenP(), scolor.blueP(), transp);

	glLineWidth(properties.thickness!= 0 ? properties.thickness : attributes->strokewidth);
	glBegin(GL_LINE_STRIP);
	for(int i=0; i < attributes->points.size(); ++i) {
		//glVertex3d(attributes->points[i].x - cMid.x, attributes->points[i].y - cMid.y, z);
			glVertex3d(attributes->points[i].x , attributes->points[i].y , z);
	}

    glEnd();

}

void PointDrawer::drawPolygon(const IVGAttributes* attributes, double z, const DrawLoop drawLoop) const{
	double transp = attributes->opacity * getTransparency();
	if ((drawLoop == drl3DOPAQUE && transp != 1.0) || (drawLoop == drl3DTRANSPARENT && transp == 1.0))
		return;

	Color fcolor = attributes->fillColor.fEqual(colorUSERDEF) ? properties.drawColor : attributes->fillColor;
	if ( fcolor != colorUNDEF) {
		glColor4f(fcolor.redP(),fcolor.greenP(), fcolor.blueP(), transp);
		for(int i=0; i < attributes->triangleStrips.size(); ++i){
			glBegin(GL_TRIANGLE_STRIP);
			for(int j=0; j < attributes->triangleStrips.at(i).size(); ++j) {
				Coord c = attributes->triangleStrips.at(i).at(j);
				glVertex3d(c.x,c.y,z);
			}
			glEnd();
		}
	}
	if ( attributes->strokeColor.fEqual(colorUNDEF)) {
		Color scolor = attributes->strokeColor;
		glLineWidth(properties.thickness != 0 ? properties.thickness : attributes->strokewidth);
		glColor4f(scolor.redP(),scolor.greenP(), scolor.blueP(), transp);
		glBegin(GL_LINE_STRIP);
		for(int i=0; i < attributes->points.size(); ++i) {
			glVertex3d(attributes->points[i].x , attributes->points[i].y , z);
		}
		glEnd();
	}


}

void PointDrawer::drawPath(const IVGAttributes* attributes, double z, const DrawLoop drawLoop) const{
	double transp = attributes->opacity * getTransparency();
	if ((drawLoop == drl3DOPAQUE && transp != 1.0) || (drawLoop == drl3DTRANSPARENT && transp == 1.0))
		return;

	if ( attributes->isPolygon()) {
		drawPolygon(attributes, z, drawLoop);
	}
	Color fcolor = attributes->fillColor.fEqual(colorUSERDEF) ? properties.drawColor : attributes->fillColor;
	glColor4f(fcolor.redP(),fcolor.greenP(), fcolor.blueP(), transp);
	const SVGPath& path = (const SVGPath&) attributes;
	for(int i = 0; i < path.noOfElements(); ++i) {
		PathElement el = path.getElement(i);
		if ( el.type == PathElement::eLINE) {
			glBegin(GL_LINE_STRIP);
			for(int j = el.start; j <= el.end; ++j) {
				glVertex3d(attributes->points[j].x, attributes->points[j].y, z);
			}
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