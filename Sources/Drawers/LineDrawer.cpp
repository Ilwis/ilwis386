#include "Client\Headers\formelementspch.h"
#include "Engine\Map\basemap.h"
#include "Client\Mapwindow\Drawers\DrawerContext.h"
#include "Engine\Map\Segment\seg.h"
#include "Client\MapWindow\Drawers\drawer_n.h"
#include "Client\Mapwindow\Drawers\SimpleDrawer.h" 
#include "Client\Mapwindow\Drawers\TextDrawer.h"
#include "Client\Ilwis.h"
#include "Engine\Spatialreference\gr.h"
#include "Engine\Map\Raster\Map.h"
#include "Engine\Base\System\RegistrySettings.h"
#include "Client\Mapwindow\MapCompositionDoc.h"
#include "Client\Mapwindow\Drawers\RootDrawer.h"
#include "Client\Mapwindow\Drawers\AbstractObjectdrawer.h"
#include "Client\Mapwindow\Drawers\AbstractMapDrawer.h"
#include "Client\Mapwindow\LayerTreeView.h"
#include "Client\Mapwindow\LayerTreeItem.h"
#include "Client\Mapwindow\Drawers\DrawingColor.h" 
#include "Client\Mapwindow\Drawers\featurelayerdrawer.h"
#include "Client\Mapwindow\Drawers\SetDrawer.h"
#include "Client\Mapwindow\Drawers\FeatureSetDrawer.h"
#include "Client\Mapwindow\Drawers\FeatureDrawer.h"
#include "drawers\linedrawer.h"

using namespace ILWIS;

ILWIS::NewDrawer *createLineDrawer(DrawerParameters *parms) {
	return new LineDrawer(parms);
}

LineDrawer::LineDrawer(DrawerParameters *parms) : 
	FeatureDrawer(parms,"LineDrawer")
{
}

LineDrawer::LineDrawer(DrawerParameters *parms, const String& name) : 
	FeatureDrawer(parms,name)
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
}

void LineDrawer::draw(bool norecursion){
	if (lines.size() == 0)
		return;
	if ( !getDrawerContext()->getCoordBoundsZoom().fContains(cb))
		return;
	setOpenGLColor();
	for(int j = 0; j < lines.size(); ++j) {
		CoordinateSequence *points = lines.at(j);
		glBegin(GL_LINE_STRIP);

		for(int i=0; i<points->size(); ++i) {
			Coordinate c = points->getAt(i);
			c.z = 0;
			glVertex3d( c.x, c.y, c.z);	
		}
		glEnd();
	}
}

void LineDrawer::prepare(PreparationParameters *p){
	FeatureDrawer::prepare(p);
	FeatureSetDrawer *fdr = dynamic_cast<FeatureSetDrawer *>(parentDrawer);
	if (  p->type == ptALL ||  p->type & ptGEOMETRY) {
		CoordSystem csy = fdr->getCoordSystem();
		cb = feature->cbBounds();
		clear();
		feature->getBoundaries(lines);
		FileName fn = drawcontext->getCoordinateSystem()->fnObj;
		bool sameCsy = drawcontext->getCoordinateSystem()->fnObj == csy->fnObj;
		if ( !sameCsy ) {
			for(int j = 0; j < lines.size(); ++j) {
				CoordinateSequence *seq = lines.at(j);
		
				for(int  i = 0; i < seq->size(); ++i) {
					Coord c = csy->cConv(drawcontext->getCoordinateSystem(), Coord(seq->getAt(i)));
					seq->setAt(c,i);
				}
			}
		}
	}
	if (  p->type == ptALL || p->type & RootDrawer::ptRENDER) {
		setColor(fdr->getDrawingColor()->clrRaw(feature->iValue(), fdr->getDrawMethod()));
		double tr = fdr->getTransparency();
		setTransparency(tr);
	}
}

