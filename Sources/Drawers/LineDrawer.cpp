#include "Client\Headers\formelementspch.h"
#include "Engine\Map\basemap.h"
#include "Client\Mapwindow\Drawers\DrawerContext.h"
#include "Engine\Map\Segment\seg.h"
#include "Client\MapWindow\Drawers\drawer_n.h"
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
#include "drawers\featurelayerdrawer.h"
#include "Client\Mapwindow\Drawers\FeatureDrawer.h"
#include "drawers\linedrawer.h"

using namespace ILWIS;


ILWIS::NewDrawer *createLineDrawer(DrawerParameters *parms) {
	return new LineDrawer(parms);
}

LineDrawer::LineDrawer(DrawerParameters *parms) : 
	FeatureDrawer(parms,"LineDrawer"),
	points(0),
	lineColor(Color(0,176,20)) 
{
}

LineDrawer::LineDrawer(DrawerParameters *parms, const String& name) : 
	FeatureDrawer(parms,name), 
	points(0),
	lineColor(Color(0,176,20)) 
{
}

LineDrawer::~LineDrawer() {
	delete points;
}

void LineDrawer::draw(bool norecursion){
	if (!points)
		return;
	double r = (double)lineColor.red() / 255.0;
	double g = (double)lineColor.green() / 255.0;
	double b = (double)lineColor.blue() / 255.0;
	glColor3f(r,g,b);
	glBegin(GL_LINE_STRIP);
	for(int i=0; i<points->size(); ++i) {
		Coordinate c = points->getAt(i);
		c.z = 0;
		glVertex3d( c.x, c.y, c.z);	
	}
	glEnd();
}

void LineDrawer::prepare(PreparationParameters *p){
	FeatureDrawer::prepare(p);
	FeatureLayerDrawer *fdr = dynamic_cast<FeatureLayerDrawer *>(p->parentDrawer);
	BaseMap bm = fdr->getBaseMap();
	double rVal = rUNDEF;
	int iRaw = iUNDEF;
	if (  p->type == ptALL ||  p->type == ptGEOMETRY) {
		CoordSystem csy = bm->cs();
		ILWIS::Segment *line = (ILWIS::Segment *)feature;
		FileName fn = drawcontext->getCoordinateSystem()->fnObj;
		if ( drawcontext->getCoordinateSystem()->fnObj == csy->fnObj) {
			points = line->getCoordinates();
		}
		else {
			if ( points){
				delete points;
				points = 0;
			}
			CoordinateSequence *seq = line->getCoordinates();
			points = new CoordinateArraySequence(seq->size());

			for(int  i = 0; i < seq->size(); ++i) {
				Coord c = csy->cConv(drawcontext->getCoordinateSystem(), Coord(seq->getAt(i)));
				points->setAt(c,i);
			}
			delete seq;
		}
	}
	if (  p->type == ptALL ||  p->type == ptRENDER) {
		ILWIS::Segment *line = (ILWIS::Segment *)feature;
		Domain dm = bm->dm();
		DomainValue *pdv = dm->pdv();
		Representation rpr = fdr->getRepresentation();
		NewDrawer::DrawMethod drm = fdr->getDrawMethod();
		if (!rpr.fValid())
			drm = drmSINGLE;
		if (bm->dm()->pdvi() || bm->dm()->pdvr()) {
			rVal = line->rValue();
			iRaw = longConv(rVal); // for multiple colors
			if (rVal == rUNDEF)
				return;
		}
		else {
			iRaw = line->iValue();
			if (iRaw == iUNDEF) return;
			if (fdr->useAttributeTable()) {
				Column colAtt = fdr->getAtttributeColumn();
				if (colAtt->dm()->pdvi() || colAtt->dm()->pdvr()) {
					rVal = colAtt->rValue(iRaw);
					iRaw = longConv(rVal); // for multiple colors
					if (rVal == rUNDEF) return;
				}
				else {
					iRaw = colAtt->iRaw(iRaw);
					if (iRaw == iUNDEF) return;
				}
			}
		}
		switch(drm) {
		case NewDrawer::drmSINGLE:
			lineColor = fdr->getSingleColor();
			break;
		case NewDrawer::drmRPR:
			if (bm->dm()->pdv()) {
				if (fdr->isStretched())
					lineColor = rpr->clr(rVal,fdr->getStretchRange());
				else  
					lineColor = rpr->clr(rVal);
			}   
			else 
				lineColor = rpr->clrRaw(iRaw);

			break;

		}
	}
}