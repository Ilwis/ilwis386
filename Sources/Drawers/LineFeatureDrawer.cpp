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
#include "drawers\linedrawer.h"
#include "drawers\linefeaturedrawer.h"

using namespace ILWIS;

ILWIS::NewDrawer *createLineFeatureDrawer(DrawerParameters *parms) {
	return new LineFeatureDrawer(parms);
}

LineFeatureDrawer::LineFeatureDrawer(DrawerParameters *parms) : 
	LineDrawer(parms,"LineFeatureDrawer")
{
	drawColor = Color(0,167,18);
}

LineFeatureDrawer::LineFeatureDrawer(DrawerParameters *parms, const String& name) : 
	LineDrawer(parms,name)
{
}

LineFeatureDrawer::~LineFeatureDrawer() {
}

bool LineFeatureDrawer::draw(bool norecursion, const CoordBounds& cbArea) const {
	return LineDrawer::draw(norecursion, cbArea);
}

void LineFeatureDrawer::addDataSource(void *f, int options) {
	feature = (Feature *)f;

}


void LineFeatureDrawer::prepare(PreparationParameters *p){
	LineDrawer::prepare(p);
	FeatureSetDrawer *fdr = dynamic_cast<FeatureSetDrawer *>(parentDrawer);
	if ( p->type & ptGEOMETRY) {
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
		drawColor = (fdr->getDrawingColor()->clrRaw(feature->iValue(), fdr->getDrawMethod()));
		double tr = fdr->getTransparency();
		setTransparency(tr);
	}
}

