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
	setTransparency(1); // opaque
	displayList = new GLuint;
	*displayList = 0;
	fRefreshDisplayList = new bool;
	*fRefreshDisplayList = false;
	fFootprintComputed = new bool;
	*fFootprintComputed = false;
}

SpaceTimeElementsDrawer::~SpaceTimeElementsDrawer() {
	if (*displayList != 0) // which OpenGL context?
		glDeleteLists(*displayList, 1);
	delete displayList;
	delete fRefreshDisplayList;
	delete fFootprintComputed;
}

void SpaceTimeElementsDrawer::SetSpaceTimeDrawer(SpaceTimeDrawer * _spaceTimeDrawer)
{
	spaceTimeDrawer = _spaceTimeDrawer;
}

void SpaceTimeElementsDrawer::prepare(PreparationParameters *parms) {
	ComplexDrawer::prepare(parms);
	if ((parms->type & RootDrawer::ptGEOMETRY) || (parms->type & NewDrawer::pt3D) || (parms->type & NewDrawer::ptRESTORE))
		*fRefreshDisplayList = true;
	if ( parms->type & NewDrawer::ptRENDER) {
		if ( parms->props )
			*fRefreshDisplayList = true;
	}
}

void SpaceTimeElementsDrawer::RefreshDisplayList() const {
	*fRefreshDisplayList = true;
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

bool SpaceTimeElementsDrawer::draw(const CoordBounds& cbArea) const{
	if ( !isActive() || !isValid())
		return false;

	if (!properties["footprint"].visible)
		return false;

	double alpha = 1.0 - properties["footprint"].transparency;

	if (*fRefreshDisplayList) {
		if (*displayList != 0) {
			glDeleteLists(*displayList, 1);
			*displayList = 0;
		}
		*fRefreshDisplayList = false;
	}

	drawPreDrawers(cbArea);

	// Following 3 lines needed for transparency to work
	glClearColor(1.0,1.0,1.0,0.0);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);
	glAlphaFunc(GL_GREATER, 0);
	glEnable(GL_ALPHA_TEST);
	glPushMatrix();
	bool is3D = getRootDrawer()->is3D();
	if (is3D) {
		ZValueMaker *zmaker = getZMaker();
		double zscale = zmaker->getZScale();
		double zoffset = zmaker->getOffset();
		double z0 = getRootDrawer()->getZMaker()->getZ0(is3D); // note that the rootDrawer's zmaker is different than the layer's zmaker
		glScaled(1,1,zscale);
		glTranslated(0,0,zoffset + z0);
	} else
		glScaled(1,1,0);

	glEnable(GL_TEXTURE_2D);
	glMatrixMode(GL_TEXTURE);
	glPushMatrix();
	glColor4f(1, 1, 1, alpha);
	//glLineWidth(properties->exaggeration);

	if (*displayList != 0)
		glCallList(*displayList);
	else
	{
		*displayList = glGenLists(1);
		glNewList(*displayList, GL_COMPILE_AND_EXECUTE);

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
		*fFootprintComputed = true;
	}

	//glLineWidth(1);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glDisable(GL_TEXTURE_2D);
	glPopMatrix();
	glDisable(GL_ALPHA_TEST);
	glDisable(GL_BLEND);

	drawPostDrawers(cbArea);
	return true;
}

// #######################################################################################################

PathElementProperties::PathElementProperties() : GeneralDrawerProperties() {
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
