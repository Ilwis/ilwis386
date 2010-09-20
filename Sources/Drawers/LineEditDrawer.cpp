#include "Client\Headers\formelementspch.h"
#include "Client\MapWindow\Drawers\drawer_n.h"
#include "Client\Mapwindow\Drawers\SimpleDrawer.h" 
#include "Client\Ilwis.h"
//#include "Engine\Base\System\RegistrySettings.h"
//#include "Client\Mapwindow\MapCompositionDoc.h"
#include "Client\Mapwindow\Drawers\RootDrawer.h"
#include "Drawers\DrawingColor.h" 
//#include "Drawers\SetDrawer.h"
//#include "Client\Mapwindow\LayerTreeView.h"
//#include "Client\Mapwindow\LayerTreeItem.h" 
//#include "Drawers\SetDrawer.h"
#include "Client\Mapwindow\Drawers\DrawerContext.h"
//#include "Drawers\FeatureSetDrawer.h"
#include "drawers\linedrawer.h"
//#include "drawers\linefeaturedrawer.h"
#include "Drawers\LineEditDrawer.h"
#include "Client\Mapwindow\Drawers\ZValueMaker.h"

using namespace ILWIS;

ILWIS::NewDrawer *createLineEditDrawer(DrawerParameters *parms) {
	return new LineEditDrawer(parms);
}

LineEditDrawer::LineEditDrawer(DrawerParameters *parms) : 
LineDrawer(parms,"LineEditDrawer")
{
	setTransparency(1);
}

LineEditDrawer::~LineEditDrawer(){
}

void LineEditDrawer::prepare(PreparationParameters *pp){
	LineDrawer::prepare(pp);
}

HTREEITEM LineEditDrawer::configure(LayerTreeView  *tv, HTREEITEM parent){
	return LineDrawer::configure(tv,parent);
}

bool LineEditDrawer::draw(bool norecursion , const CoordBounds& cbArea) const{
	if (lines.size() == 0)
		return false;
	if ( !getRootDrawer()->getCoordBoundsZoom().fContains(cb))
		return false;

	LineDrawer::draw(norecursion, cbArea);

	ComplexDrawer *cdrw = (ComplexDrawer *)getParentDrawer();
	bool is3D = getRootDrawer()->is3D() && cdrw->getZMaker()->getThreeDPossible();
	if ( is3D) // for the moment no editing in 3D
		return false;

	double zscale = cdrw->getZMaker()->getZScale();
	double zoffset = cdrw->getZMaker()->getOffset();

	glColor4f(drawColor.redP(),drawColor.greenP(), drawColor.blueP(), getTransparency());
	glLineWidth(thickness);

	if (linestyle != 0xFFFF) {
		glEnable (GL_LINE_STIPPLE);
		glLineStipple(1,linestyle);
	}

	if ( is3D) {
		glPushMatrix();
		glScaled(1,1,zscale);
		glTranslated(0,0,zoffset);
	}
	CoordBounds cbZoom = getRootDrawer()->getCoordBoundsZoom();
	double symbolScale = cbZoom.width() / 200;

	for(int j = 0; j < lines.size(); ++j) {
		CoordinateSequence *points = lines.at(j);
		for(int i=0; i<points->size(); ++i) {
			Coordinate c = points->getAt(i);
			double z = is3D ? c.z : 0;
			glBegin(GL_LINE_STRIP);
			glVertex3d( c.x - symbolScale, c.y - symbolScale, z);
			glVertex3d( c.x + symbolScale, c.y + symbolScale, z);	
			glEnd();
			glBegin(GL_LINE_STRIP);
			glVertex3d( c.x + symbolScale, c.y - symbolScale, z);
			glVertex3d( c.x - symbolScale, c.y + symbolScale, z);	
			glEnd();
		}
	}
	if ( is3D) {
		glPopMatrix();
	}
	glDisable (GL_LINE_STIPPLE);
	glLineWidth(1);
	return true;
}