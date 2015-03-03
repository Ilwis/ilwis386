#include "Headers\toolspch.h"
#include "Engine\Drawers\RootDrawer.h"
#include "SpaceTimeElementsDrawer.h"
#include "Engine\Drawers\SpatialDataDrawer.h"
#include "Drawers\LayerDrawer.h"
#include "Drawers\FeatureLayerDrawer.h"
#include "SpaceTimeDrawer.h"
#include "Engine\Drawers\ZValueMaker.h"

using namespace ILWIS;

PathElement PathElement::undefElement = PathElement(sUNDEF,colorUNDEF,rUNDEF,false);

ILWIS::NewDrawer *createSpaceTimeElementsDrawer(DrawerParameters *parms) {
	return new SpaceTimeElementsDrawer(parms);
}

SpaceTimeElementsDrawer::SpaceTimeElementsDrawer(DrawerParameters *parms)
: ComplexDrawer(parms,"SpaceTimeElementsDrawer")
{
	id = name = "SpaceTimeElementsDrawer";
	setAlpha(1); // opaque
	displayListFootprint = new GLuint;
	*displayListFootprint = 0;
	fRefreshDisplayListFootprint = new bool;
	*fRefreshDisplayListFootprint = false;
	displayListXT = new GLuint;
	*displayListXT = 0;
	fRefreshDisplayListXT = new bool;
	*fRefreshDisplayListXT = false;
	displayListXY = new GLuint;
	*displayListXY = 0;
	fRefreshDisplayListXY = new bool;
	*fRefreshDisplayListXY = false;
	displayListYT = new GLuint;
	*displayListYT = 0;
	fRefreshDisplayListYT = new bool;
	*fRefreshDisplayListYT = false;
}

SpaceTimeElementsDrawer::~SpaceTimeElementsDrawer() {
	if (*displayListFootprint != 0) // which OpenGL context?
		glDeleteLists(*displayListFootprint, 1);
	delete displayListFootprint;
	delete fRefreshDisplayListFootprint;
	if (*displayListXT != 0)
		glDeleteLists(*displayListXT, 1);
	delete displayListXT;
	delete fRefreshDisplayListXT;
	if (*displayListXY != 0)
		glDeleteLists(*displayListXY, 1);
	delete displayListXY;
	delete fRefreshDisplayListXY;
	if (*displayListYT != 0)
		glDeleteLists(*displayListYT, 1);
	delete displayListYT;
	delete fRefreshDisplayListYT;
}

void SpaceTimeElementsDrawer::SetSpaceTimeDrawer(SpaceTimeDrawer * _spaceTimeDrawer)
{
	spaceTimeDrawer = _spaceTimeDrawer;
}

void SpaceTimeElementsDrawer::prepare(PreparationParameters *parms) {
	ComplexDrawer::prepare(parms);
	if ((parms->type & RootDrawer::ptGEOMETRY) || (parms->type & NewDrawer::pt3D) || (parms->type & NewDrawer::ptRESTORE)) {
		*fRefreshDisplayListFootprint = true;
		*fRefreshDisplayListXT = true;
		*fRefreshDisplayListXY = true;
		*fRefreshDisplayListYT = true;
	}
	if ( parms->type & NewDrawer::ptRENDER) {
	}
}

void SpaceTimeElementsDrawer::RefreshDisplayList() const {
	*fRefreshDisplayListFootprint = true;
	*fRefreshDisplayListXT = true;
	*fRefreshDisplayListXY = true;
	*fRefreshDisplayListYT = true;
}

GeneralDrawerProperties *SpaceTimeElementsDrawer::getProperties(){
	return &properties;
}

String SpaceTimeElementsDrawer::store(const FileName& fnView, const String& parentSection) const {
	String currentSection = parentSection + "::" + getType();
	ComplexDrawer::store(fnView, currentSection);
	properties.store(fnView, currentSection);
	return currentSection;
}

void SpaceTimeElementsDrawer::load(const FileName& fnView, const String& currentSection){
	String drawerSection = currentSection + "::" + getType();
	ComplexDrawer::load(fnView, drawerSection);
	properties.load(fnView, drawerSection);
}

bool SpaceTimeElementsDrawer::drawFootprint(const DrawLoop drawLoop, const CoordBounds& cbArea) const {
	if ( !isActive() || !isValid())
		return false;

	if (!properties["footprint"].visible)
		return false;

	double alpha = 1.0 - properties["footprint"].transparency;
	if ((drawLoop == drl3DOPAQUE && alpha != 1.0) || (drawLoop == drl3DTRANSPARENT && alpha == 1.0))
		return false;

	if (*fRefreshDisplayListFootprint) {
		if (*displayListFootprint != 0) {
			glDeleteLists(*displayListFootprint, 1);
			*displayListFootprint = 0;
		}
		*fRefreshDisplayListFootprint = false;
	}

	drawPreDrawers(drawLoop, cbArea);

	// Following 3 lines needed for transparency to work
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);
	glAlphaFunc(GL_GREATER, 0);
	glEnable(GL_ALPHA_TEST);
	glPushMatrix();
	bool is3D = getRootDrawer()->is3D();
	if (is3D) {
		//ZValueMaker *zmaker = getZMaker();
		//double zscale = zmaker->getZScale();
		//double zoffset = zmaker->getOffset();
		//double z0 = getRootDrawer()->getZMaker()->getZ0(is3D); // note that the rootDrawer's zmaker is different than the layer's zmaker
		//glScaled(1,1,zscale);
		//glTranslated(0,0,zoffset + z0);
	} else
		glScaled(1,1,0);

	glEnable(GL_TEXTURE_2D);
	glMatrixMode(GL_TEXTURE);
	glPushMatrix();
	glColor4f(1, 1, 1, alpha);
	//glLineWidth(properties->exaggeration);

	if (*displayListFootprint != 0)
		glCallList(*displayListFootprint);
	else
	{
		*displayListFootprint = glGenLists(1);
		glNewList(*displayListFootprint, GL_COMPILE_AND_EXECUTE);

		CoordBounds cbMap = getRootDrawer()->getMapCoordBounds();
		CoordBounds cbBaseMap = spaceTimeDrawer->getBasemap()->cb();
		bool fZoomed = cbBaseMap.MinX() < cbMap.MinX() || cbBaseMap.MinY() < cbMap.MinY() || cbBaseMap.MaxX() > cbMap.MaxX() || cbBaseMap.MaxY() > cbMap.MaxY();

		if (fZoomed) {
			double pathScale = 0; // properties->exaggeration * cbMap.width() / 50;
			double clip_plane0[]={-1.0, 0.0, 0.0, cbMap.cMax.x + pathScale};
			double clip_plane1[]={1.0, 0.0, 0.0, -cbMap.cMin.x - pathScale};
			double clip_plane2[]={0.0, -1.0, 0.0, cbMap.cMax.y + pathScale};
			double clip_plane3[]={0.0, 1.0, 0.0, -cbMap.cMin.y - pathScale};
			glClipPlane(GL_CLIP_PLANE0,clip_plane0);
			glClipPlane(GL_CLIP_PLANE1,clip_plane1);
			glClipPlane(GL_CLIP_PLANE2,clip_plane2);
			glClipPlane(GL_CLIP_PLANE3,clip_plane3);
			glEnable(GL_CLIP_PLANE0);
			glEnable(GL_CLIP_PLANE1);
			glEnable(GL_CLIP_PLANE2);
			glEnable(GL_CLIP_PLANE3);
		}
		spaceTimeDrawer->drawFootprint();
		if (fZoomed) {
			glDisable(GL_CLIP_PLANE0);
			glDisable(GL_CLIP_PLANE1);
			glDisable(GL_CLIP_PLANE2);
			glDisable(GL_CLIP_PLANE3);
		}
		glEndList();
	}

	//glLineWidth(1);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glDisable(GL_TEXTURE_2D);
	glPopMatrix();
	glDisable(GL_ALPHA_TEST);
	glDisable(GL_BLEND);

	drawPostDrawers(drawLoop, cbArea);
	return true;
}

void SpaceTimeElementsDrawer::callFootprintList() const {
	CoordBounds cbArea;
	drawPreDrawers(drl3DOPAQUE, cbArea);
	glPushMatrix();
	if (!rootDrawer->is3D())
		glScaled(1,1,0);
	glMatrixMode(GL_TEXTURE);
	glPushMatrix();
	if (*displayListFootprint != 0)
		glCallList(*displayListFootprint);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
	drawPostDrawers(drl3DOPAQUE, cbArea);
}

bool SpaceTimeElementsDrawer::drawXT(const DrawLoop drawLoop, const CoordBounds& cbArea) const {
	if ( !isActive() || !isValid())
		return false;
	if (!properties["xt"].visible)
		return false;
	double alpha = 1.0 - properties["xt"].transparency;
	if ((drawLoop == drl3DOPAQUE && alpha != 1.0) || (drawLoop == drl3DTRANSPARENT && alpha == 1.0))
		return false;
	if (*fRefreshDisplayListXT) {
		if (*displayListXT != 0) {
			glDeleteLists(*displayListXT, 1);
			*displayListXT = 0;
		}
		*fRefreshDisplayListXT = false;
	}

	Color color (properties["xt"].color);
	//glColor4f(color.redP(), color.greenP(), color.blueP(), alpha);
	glColor4f(1, 1, 1, alpha);

	if (*displayListXT != 0)
		glCallList(*displayListXT);
	else
	{
		*displayListXT = glGenLists(1);
		glNewList(*displayListXT, GL_COMPILE_AND_EXECUTE);
		CoordBounds cbMap = getRootDrawer()->getMapCoordBounds();
		CoordBounds cbBaseMap = spaceTimeDrawer->getBasemap()->cb();
		bool fZoomed = cbBaseMap.MinX() < cbMap.MinX() || cbBaseMap.MinY() < cbMap.MinY() || cbBaseMap.MaxX() > cbMap.MaxX() || cbBaseMap.MaxY() > cbMap.MaxY();
		if (fZoomed) {
			double pathScale = 0;
			double clip_plane0[]={-1.0, 0.0, 0.0, cbMap.cMax.x + pathScale};
			double clip_plane1[]={1.0, 0.0, 0.0, -cbMap.cMin.x - pathScale};
			double clip_plane2[]={0.0, -1.0, 0.0, cbMap.cMax.y + pathScale};
			double clip_plane3[]={0.0, 1.0, 0.0, -cbMap.cMin.y - pathScale};
			glClipPlane(GL_CLIP_PLANE0,clip_plane0);
			glClipPlane(GL_CLIP_PLANE1,clip_plane1);
			glClipPlane(GL_CLIP_PLANE2,clip_plane2);
			glClipPlane(GL_CLIP_PLANE3,clip_plane3);
			glEnable(GL_CLIP_PLANE0);
			glEnable(GL_CLIP_PLANE1);
			glEnable(GL_CLIP_PLANE2);
			glEnable(GL_CLIP_PLANE3);
		}
		spaceTimeDrawer->drawXT();
		if (fZoomed) {
			glDisable(GL_CLIP_PLANE0);
			glDisable(GL_CLIP_PLANE1);
			glDisable(GL_CLIP_PLANE2);
			glDisable(GL_CLIP_PLANE3);
		}
		glEndList();
	}
	return true;
}

bool SpaceTimeElementsDrawer::drawXY(const DrawLoop drawLoop, const CoordBounds& cbArea) const {
	if ( !isActive() || !isValid())
		return false;
	if (!properties["xy"].visible)
		return false;
	double alpha = 1.0 - properties["xy"].transparency;
	if ((drawLoop == drl3DOPAQUE && alpha != 1.0) || (drawLoop == drl3DTRANSPARENT && alpha == 1.0))
		return false;
	if (*fRefreshDisplayListXY) {
		if (*displayListXY != 0) {
			glDeleteLists(*displayListXY, 1);
			*displayListXY = 0;
		}
		*fRefreshDisplayListXY = false;
	}

	Color color (properties["xy"].color);
	// glColor4f(color.redP(), color.greenP(), color.blueP(), alpha);
	glColor4f(1, 1, 1, alpha);

	if (*displayListXY != 0)
		glCallList(*displayListXY);
	else
	{
		*displayListXY = glGenLists(1);
		glNewList(*displayListXY, GL_COMPILE_AND_EXECUTE);
		CoordBounds cbMap = getRootDrawer()->getMapCoordBounds();
		CoordBounds cbBaseMap = spaceTimeDrawer->getBasemap()->cb();
		bool fZoomed = cbBaseMap.MinX() < cbMap.MinX() || cbBaseMap.MinY() < cbMap.MinY() || cbBaseMap.MaxX() > cbMap.MaxX() || cbBaseMap.MaxY() > cbMap.MaxY();
		if (fZoomed) {
			double pathScale = 0;
			double clip_plane0[]={-1.0, 0.0, 0.0, cbMap.cMax.x + pathScale};
			double clip_plane1[]={1.0, 0.0, 0.0, -cbMap.cMin.x - pathScale};
			double clip_plane2[]={0.0, -1.0, 0.0, cbMap.cMax.y + pathScale};
			double clip_plane3[]={0.0, 1.0, 0.0, -cbMap.cMin.y - pathScale};
			glClipPlane(GL_CLIP_PLANE0,clip_plane0);
			glClipPlane(GL_CLIP_PLANE1,clip_plane1);
			glClipPlane(GL_CLIP_PLANE2,clip_plane2);
			glClipPlane(GL_CLIP_PLANE3,clip_plane3);
			glEnable(GL_CLIP_PLANE0);
			glEnable(GL_CLIP_PLANE1);
			glEnable(GL_CLIP_PLANE2);
			glEnable(GL_CLIP_PLANE3);
		}
		spaceTimeDrawer->drawXY();
		if (fZoomed) {
			glDisable(GL_CLIP_PLANE0);
			glDisable(GL_CLIP_PLANE1);
			glDisable(GL_CLIP_PLANE2);
			glDisable(GL_CLIP_PLANE3);
		}
		glEndList();
	}
	return true;
}

bool SpaceTimeElementsDrawer::drawYT(const DrawLoop drawLoop, const CoordBounds& cbArea) const {
	if ( !isActive() || !isValid())
		return false;
	if (!properties["yt"].visible)
		return false;
	double alpha = 1.0 - properties["yt"].transparency;
	if ((drawLoop == drl3DOPAQUE && alpha != 1.0) || (drawLoop == drl3DTRANSPARENT && alpha == 1.0))
		return false;
	if (*fRefreshDisplayListYT) {
		if (*displayListYT != 0) {
			glDeleteLists(*displayListYT, 1);
			*displayListYT = 0;
		}
		*fRefreshDisplayListYT = false;
	}

	Color color (properties["yt"].color);
	// glColor4f(color.redP(), color.greenP(), color.blueP(), alpha);
	glColor4f(1, 1, 1, alpha);

	if (*displayListYT != 0)
		glCallList(*displayListYT);
	else
	{
		*displayListYT = glGenLists(1);
		glNewList(*displayListYT, GL_COMPILE_AND_EXECUTE);
		CoordBounds cbMap = getRootDrawer()->getMapCoordBounds();
		CoordBounds cbBaseMap = spaceTimeDrawer->getBasemap()->cb();
		bool fZoomed = cbBaseMap.MinX() < cbMap.MinX() || cbBaseMap.MinY() < cbMap.MinY() || cbBaseMap.MaxX() > cbMap.MaxX() || cbBaseMap.MaxY() > cbMap.MaxY();
		if (fZoomed) {
			double pathScale = 0;
			double clip_plane0[]={-1.0, 0.0, 0.0, cbMap.cMax.x + pathScale};
			double clip_plane1[]={1.0, 0.0, 0.0, -cbMap.cMin.x - pathScale};
			double clip_plane2[]={0.0, -1.0, 0.0, cbMap.cMax.y + pathScale};
			double clip_plane3[]={0.0, 1.0, 0.0, -cbMap.cMin.y - pathScale};
			glClipPlane(GL_CLIP_PLANE0,clip_plane0);
			glClipPlane(GL_CLIP_PLANE1,clip_plane1);
			glClipPlane(GL_CLIP_PLANE2,clip_plane2);
			glClipPlane(GL_CLIP_PLANE3,clip_plane3);
			glEnable(GL_CLIP_PLANE0);
			glEnable(GL_CLIP_PLANE1);
			glEnable(GL_CLIP_PLANE2);
			glEnable(GL_CLIP_PLANE3);
		}
		spaceTimeDrawer->drawYT();
		if (fZoomed) {
			glDisable(GL_CLIP_PLANE0);
			glDisable(GL_CLIP_PLANE1);
			glDisable(GL_CLIP_PLANE2);
			glDisable(GL_CLIP_PLANE3);
		}
		glEndList();
	}
	return true;
}

void SpaceTimeElementsDrawer::callXTList() const {
	if (*displayListXT != 0)
		glCallList(*displayListXT);
}

void SpaceTimeElementsDrawer::callXYList() const {
	if (*displayListXY != 0)
		glCallList(*displayListXY);
}

void SpaceTimeElementsDrawer::callYTList() const {
	if (*displayListYT != 0)
		glCallList(*displayListYT);
}

// #######################################################################################################

PathElementProperties::PathElementProperties() : GeneralDrawerProperties() {
	elements["spacetimepath"] = PathElement("Space Time Path", colorUNDEF,0.0, true);
	elements["footprint"] = PathElement("Footprint", colorUNDEF,0.0, false);
	elements["xt"] = PathElement("XT", Color(255,0,0),0.0, false);
	elements["xy"] = PathElement("XY", Color(255,0,0),0.0, false);
	elements["yt"] = PathElement("YT", Color(255,0,0),0.0, false);
}

PathElementProperties::PathElementProperties(PathElementProperties *lp){
	for(map<String, PathElement>::const_iterator cur = lp->elements.begin(); cur != lp->elements.end(); ++cur){
		(*this)[(*cur).first] = (*cur).second;
	}
}

PathElement& PathElementProperties::operator[](const String& key) const {
	map<String, PathElement>::iterator cur = const_cast<PathElementProperties *>(this)->elements.find(key) ;
	if ( cur != elements.end())
		return (*cur).second;
	return PathElement::undefElement;
}

String PathElementProperties::store(const FileName& fnView, const String& parentSection) const {
	ObjectInfo::WriteElement(parentSection.c_str(), "NrPathElements", fnView, (int)(elements.size()));
	int index = 0;
	for (map<String, PathElement>::const_iterator cur = elements.begin(); cur != elements.end(); ++cur) {
		String elementName = cur->first;
		PathElement element = cur->second;
		String elementSection ("%S::PathElement%d", parentSection, index++);
		ObjectInfo::WriteElement(elementSection.c_str(), "Name", fnView, elementName);
		element.store(fnView, elementSection);
	}
	return parentSection;
}

void PathElementProperties::load(const FileName& fnView, const String& parentSection) {
	int nrElements;
	if (!ObjectInfo::ReadElement(parentSection.c_str(), "NrPathElements", fnView, nrElements))
		nrElements = 0;
	for (int index = 0; index < nrElements; ++index) {
		String elementName;
		String elementSection ("%S::PathElement%d", parentSection, index);
		ObjectInfo::ReadElement(elementSection.c_str(), "Name", fnView, elementName);
		PathElement element;
		element.load(fnView, elementSection);
		elements[elementName] = element;
	}
}

String PathElement::store(const FileName& fnView, const String& parentSection) const {
	ObjectInfo::WriteElement(parentSection.c_str(), "Color", fnView, color);
	ObjectInfo::WriteElement(parentSection.c_str(), "Transparency", fnView, transparency);
	ObjectInfo::WriteElement(parentSection.c_str(), "Visible", fnView, visible);
	ObjectInfo::WriteElement(parentSection.c_str(), "Label", fnView, label);
	return parentSection;
}

void PathElement::load(const FileName& fnView, const String& parentSection) {
	ObjectInfo::ReadElement(parentSection.c_str(), "Color", fnView, color);
	ObjectInfo::ReadElement(parentSection.c_str(), "Transparency", fnView, transparency);
	ObjectInfo::ReadElement(parentSection.c_str(), "Visible", fnView, visible);
	ObjectInfo::ReadElement(parentSection.c_str(), "Label", fnView, label);
}
