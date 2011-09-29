/***************************************************************
ILWIS integrates image, vector and thematic data in one unique 
and powerful package on the desktop. ILWIS delivers a wide 
range of feautures including import/export, digitizing, editing, 
analysis and display of data as well as production of 
quality mapsinformation about the sensor mounting platform

Exclusive rights of use by 52°North Initiative for Geospatial 
Open Source Software GmbH 2007, Germany

Copyright (C) 2007 by 52°North Initiative for Geospatial
Open Source Software GmbH

Author: Jan Hendrikse, Willem Nieuwenhuis,Wim Koolhoven 
Bas Restsios, Martin Schouwenburg, Lichun Wang, Jelle Wind 

Contact: Martin Schouwenburg; schouwenburg@itc.nl; 
tel +31-534874371

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
version 2 as published by the Free Software Foundation.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program (see gnu-gpl v2.txt); if not, write to
the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
Boston, MA 02111-1307, USA or visit the web page of the Free
Software Foundation, http://www.fsf.org.

Created on: 2007-02-8
***************************************************************/
// DistanceMeasurer.cpp : implementation file
//

#include "Client\Headers\formelementspch.h"
#include "Engine\Base\System\RegistrySettings.h"
#include "Client\ilwis.h"
#include "Client\Base\datawind.h"
#include "Client\Mapwindow\MapWindow.h"
#include "Headers\constant.h"
#include "Engine\Drawers\RootDrawer.h"
#include "Engine\Drawers\ComplexDrawer.h"
#include "Engine\Drawers\SimpleDrawer.h" 
#include "Drawers\LayerDrawer.h"
#include "Client\Base\IlwisDocument.h"
#include "Client\MainWindow\Catalog\CatalogDocument.h"
#include "Engine\Map\Mapview.h"
#include "Client\Mapwindow\MapCompositionDoc.h"
#include "Client\Mapwindow\SimpleMapPaneView.h"
#include "Client\Mapwindow\MapPaneViewTool.h"
#include "Client\MapWindow\Drawers\DrawerTool.h"
#include "Client\Mapwindow\MapPaneView.h"
#include "Client\Mapwindow\LayerTreeView.h"
#include "Client\Mapwindow\LayerTreeItem.h" 
#include "DrawersUI\SetDrawerTool.h"
#include "Engine\Drawers\DrawerContext.h"
#include "DrawersUI\LayerDrawerTool.h"
#include "drawers\linedrawer.h"
#include "DistanceMeasurer.h"
#include "Client\Mapwindow\InfoLine.h"
#include "Headers\Hs\Mapwind.hs"
#include "Engine\SpatialReference\Coordsys.h"
#include "Engine\SpatialReference\Ellips.h"
#include "Engine\SpatialReference\csviall.h"
#include "Engine\SpatialReference\Distance.h"
#include "DrawersUI\GlobalTool.h"
#include "Engine\Drawers\ZValueMaker.h"

using namespace ILWIS;

MeasurerLine::MeasurerLine(DrawerParameters *parms) : 
LineDrawer(parms,"MeasurerLine")
{
	useEllipse = false;
	rDist = rUNDEF;
	setSupportingDrawer(true);
}

MeasurerLine::~MeasurerLine() {
}


bool MeasurerLine::draw( const CoordBounds& cbArea) const{
	glClearColor(1.0,1.0,1.0,0.0);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);
	LineDrawer::draw(cbArea);
	glDisable(GL_BLEND);
	if ( useEllipse && rDist > 0 ) {
		int sections = 50;
		double twoPi =  2.0 * M_PI;
		double rx = rDist;
		double ry = rDist;
		double lcx = center.x;
		double lcy = center.y;
		bool is3D = getRootDrawer()->is3D();// && zvmkr->getThreeDPossible();
		double z0 = rootDrawer->getZMaker()->getZ0(is3D) ;
		z0 += z0; // supprting drawer, lies on top of the actual layerdrawer
		glBegin(GL_LINE_LOOP);
		for(int i = 0; i <= sections;i++) { // make $section number of circles
			glVertex3d(lcx + rx * cos(i *  twoPi / sections), 
				lcy + ry* sin(i * twoPi / sections), z0);
		}
	glEnd();
	}

	return true;
}

void MeasurerLine::prepare(PreparationParameters *p){
	LineDrawer::prepare(p);
}
//------------------------------------------------

DrawerTool *createMeasurerTool(ZoomableView* zv, LayerTreeView *view, NewDrawer *drw) {
	return new DistanceMeasurer(zv, view, drw);
}

const double rDefaultEarthRadius = 6371007.1809185;

DistanceMeasurer::DistanceMeasurer(ZoomableView* zv, LayerTreeView *view, NewDrawer *drw)
: DrawerTool(TR("DistanceMeasurer"),zv, view, drw)
{
	csy = view->GetDocument()->rootDrawer->getCoordinateSystem();
	fDown = FALSE;
	active = true;
	line = 0;
	useEllipse = false;
	stay = true;
}

DistanceMeasurer::~DistanceMeasurer()
{
	//tree->GetDocument()->mpvGetView()->info->text(CPoint(0,0),"");
	if ( line)
		drawer->getRootDrawer()->removeDrawer(line->getId());
	line = 0;
}

bool DistanceMeasurer::isToolUseableFor(ILWIS::DrawerTool *tool){
	return dynamic_cast<GlobalTool *>(tool) != 0;
}

HTREEITEM DistanceMeasurer::configure( HTREEITEM parentItem){
	DisplayOptionTreeItem *item = new DisplayOptionTreeItem(tree,parentItem,drawer);
	item->setCheckAction(this,0, (DTSetCheckFunc)&DistanceMeasurer::setcheckTool);
	htiNode = insertItem(TR("Distance Measurer"),"Measurer",item,0);
	item = new DisplayOptionTreeItem(tree,htiNode,drawer);
	item->setCheckAction(this,0, (DTSetCheckFunc)&DistanceMeasurer::setUseEllipse);
	insertItem(TR("Equidistance display"),"Circle",item,0);


	
	return htiNode;
}

void DistanceMeasurer::setUseEllipse(void *w, HTREEITEM ) {
	useEllipse = *(bool *)w;
}

void DistanceMeasurer::setcheckTool(void *w, HTREEITEM ) {
	bool yesno = *(bool *)w;
	if ( yesno) {
		line  = 0;
		tree->GetDocument()->mpvGetView()->addTool(this, getId());
	}
	else {
		SetCursor(zCursor(Arrow));
		if ( line)
			drawer->getRootDrawer()->removeDrawer(line->getId(), true);
		line = 0;
		tree->GetDocument()->mpvGetView()->noTool(getId());
	}
}

String DistanceMeasurer::getMenuString() const{
	return TR("Distance Measurer");
}


void DistanceMeasurer::InfoReport()
{
	double rDist = rDistance();
	double rSphDist = rSphericalDistance(rDefaultEarthRadius);
	bool fLatLon = fLatLonCoords();
	String sMsg;
	if (rDist == rUNDEF && rSphDist == rUNDEF) {
		//tree->GetDocument()->mpvGetView()->info->text(pEnd, "");
	}
	else {
		if (!fLatLon)
			sMsg = String("%.2f m", rDist);
		else
			sMsg = String("%.2f m", rSphDist);
		// tree->GetDocument()->mpvGetView()->info->text(p, sMsg);
	}
}

void DistanceMeasurer::Report()
{
	double rDist = rDistance();
	double rDir = rAzim();
	double rRadi = rDefaultEarthRadius;
	if(csy->pcsViaLatLon())
		rRadi = csy->pcsViaLatLon()->ell.a;
	// rDefaultEarthRadius is overruled by userdefined csy radius a
	double rSphDist = rSphericalDistance(rRadi);
	double rSphAzim = rSphericalAzim(rRadi);
	double rEllDist;
	double rEllAzim;
	if (fEllipsoidalCoords()) {
		rEllDist = rEllipsoidDistance(csy);
		rEllAzim = rEllipsoidAzimuth(csy);
	}
	String sMsg = String("");
	String sMgsDist, sMgsAzim, sMgsEllDist, sMgsEllAzim, sMsgEnd;
	if (rDist == rUNDEF || rDir == rUNDEF)
		sMsg = TR("Distance calculation impossible");
	else {
		sMsg = String(TR("From %S").c_str(),csy->sValue(cStart(),0));
		sMsg &= String("\n");
		sMsg &= String(TR("To %S").c_str(),csy->sValue(cEnd(),0));
		if (fLatLonCoords())
			sMsg &= String("\n\nNo metric co-ordinates available");
		else {
			sMsg &= String("\n\n%S: %.2f m", TR("Distance on map"), rDist); 
			sMsg &= String("\n%S: %.2f °", TR("Azimuth on map"), rDir);
		}
		if(fLatLonCoords() || fProjectedCoords())
		{	
			double rScaleF, rMeridConv;
			if(fEllipsoidalCoords() && (rEllDist < 800000)) { //above 800km ellips dist unreliable
				sMsg &= String("\n\n%S: %.2f m", 
					TR("Ellipsoidal Distance"), rEllDist);
				sMsg &= String("\n%S: %.2f °",	TR("Ellipsoidal Azimuth"), rEllAzim);
				rScaleF = rDist / rEllDist;
				rMeridConv = rEllAzim - rDir;
				if (rMeridConv > M_PI_2)
					rMeridConv = rMeridConv - M_PI * 2;
			}
			else {
				if 	(rSphDist < 1000)
					sMsg &= String("\n\n%S: %.2f m", TR("Spherical Distance"), rSphDist);
				else 
					sMsg &= String("\n\n%S: %.3f km", 
					TR("Spherical Distance"), rSphDist/1000); // long distances in km
				rScaleF = rDist / rSphDist;
				sMsg &= String("\n%S: %.2f °", TR("Spherical Azimuth"), rSphAzim);
				rMeridConv = rSphAzim - rDir;
				if (rMeridConv > M_PI_2)
					rMeridConv = rMeridConv - M_PI * 2;
			}
			if(!fLatLonCoords()) {
				sMsg &= String("\n\n%S: %.10f", TR("Scale Factor"), rScaleF);
				//sMsg &= String("\n%S: %.3f °", TR("Meridian Convergence"), rMeridConv);
				sMsg &= String("\n%S: %.3f °", TR("Meridian Convergence"), rSphericalMeridConv(rRadi));
			}
		}
	}
	tree->GetDocument()->mpvGetView()->MessageBox(sMsg.c_str(), TR("Distance").c_str());
}

void DistanceMeasurer::drawLine()
{
	//glClearColor(1.0,1.0,1.0,0.0);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//glEnable(GL_BLEND);
	//glColor3d(1,1,1);
	//glBegin(GL_LINE_STRIP);
	//glVertex3d( cStart().x, cStart().y, 0); // mpv->GetDocument()->rootDrawer->getFakeZ());
	//glVertex3d( cEnd().x, cEnd().y, 0);
	//glEnd();
	//glDisable(GL_BLEND);
	mpvGetView()->Invalidate();


}

/*
void DistanceMeasurer::DrawSphericCurvedLine() // see GraticuleDrawer::DrawCurvedLine(  )
{
Zpoint p1, p2, pMoving;
p1 = pStart;
p2 = pEnd;
cStart() = mpv->crdPnt(pStart);
cEnd() = mpv->crdPnt(pEnd);
LatLon llMoving;
if (cStart().fUndef() || cEnd().fUndef())
return rUNDEF;
LatLon llStart, llEnd;
if (!fLatLonCoords() && !fProjectedCoords()) 
return rUNDEF;
else {
llStart = csy->llConv(cStart());
llEnd = csy->llConv(cEnd());
}
if (llStart.fUndef() || llEnd.fUndef())
return rUNDEF;
int iStep = (int)(rSphericalDistance(rDefaultEarthRadius) / 30);
llMoving = llStart;
double rSin, rCos;
for (int i = 0; i < 30; i++) {
p1 = psn->pntPos(csy->cConv(llMoving));
rSin = sin(rSphericalAzim(rDefaultEarthRadius));
rCos = sin(rSphericalAzim(rDefaultEarthRadius));
pMoving.x += iStep * rCos;
pMoving.y += iStep * rSin;
p2 = psn->pntPos(csy->cConv(llMoving));
cdc.MoveTo(p1);
cdc.LineTo(p2);
}
}
*/

void DistanceMeasurer::setCoords() {
	line->addCoords(coords, 1);
	line->setDistance(rDistance());
	line->setCenter(cStart());
	line->useEllipse = useEllipse;
}

void DistanceMeasurer::OnMouseMove(UINT nFlags, CPoint point)
{
	if ( !line)
		return;

	if (fDown) {
		SetCursor(zCursor("MeasureCursor"));
		tree->GetDocument()->mpvGetView()->info->ShowWindow(SW_HIDE);
		drawLine();
		Coord c1 = tree->GetDocument()->rootDrawer->screenToWorld(RowCol(point.y, point.x));
		coords[coords.size() - 1] = c1;
		setCoords();
		drawLine();
		InfoReport();
	}
}

void DistanceMeasurer::OnLButtonUp(UINT nFlags, CPoint point)
{
	if ( !line) {
		DrawerParameters dp(drawer->getRootDrawer(), drawer);
		line = new MeasurerLine(&dp);
		drawer->getRootDrawer()->addPostDrawer(729,line);
		Coord c1 = tree->GetDocument()->rootDrawer->screenToWorld(RowCol(point.y, point.x));
		coords.push_back(c1);
		Coord c2 = tree->GetDocument()->rootDrawer->screenToWorld(RowCol(point.y, point.x));
		coords.push_back(c2);
		tree->GetDocument()->mpvGetView()->setBitmapRedraw(true);
		fDown = true;
		setCoords();
	} else {
		tree->GetDocument()->mpvGetView()->info->text(point,"");

		short state = ::GetKeyState(VK_CONTROL);
		if ( state & 0x8000) {
			coords.push_back(coords[coords.size() - 1]);
		} else {
			setCoords();
			fDown = false;
			Report();
			drawer->getRootDrawer()->removeDrawer(line->getId(), true);
			line = 0;
			coords.clear();
			tree->GetDocument()->mpvGetView()->setBitmapRedraw(false);
		}
	}
	drawLine();
}

double DistanceMeasurer::rDistance()
{

	if (cStart().fUndef())
		return rUNDEF;

	if (cEnd().fUndef())
		return rUNDEF;
	double rD = 0;
	for(int i=1; i < coords.size(); ++i) {
		rD += rDist(coords[i-1], coords[i]);
	}
	return rD;
}

double DistanceMeasurer::rAzim()
{
	if (cStart().fUndef() || cEnd().fUndef())
		return rUNDEF;
	double rAzim = atan2(cEnd().x - cStart().x, cEnd().y - cStart().y);
	rAzim *= 180 / M_PI;
	if (rAzim < 0)
		rAzim += 360;
	return rAzim;
}

bool DistanceMeasurer::fLatLonCoords()
{
	CoordSystemLatLon* csll = csy->pcsLatLon();
	return (0 != csll);
}

bool DistanceMeasurer::fProjectedCoords()
{
	return csy->fCoord2LatLon();// dit is wellicht een betere check //17 april 00
	//CoordSystemProjection* cspr = csy->pcsProjection();
	//return (0 != cspr);
}

bool DistanceMeasurer::fEllipsoidalCoords()
{
	CoordSystemViaLatLon* csviall = csy->pcsViaLatLon();
	bool fSpheric = true;
	if (csviall)
		fSpheric= (csviall->ell.fSpherical());
	return (0 != csviall &&  0 == fSpheric);
}

double DistanceMeasurer::rSphericalDistance(const double rRadius)
{
	if (cStart().fUndef())
		return rUNDEF;

	if (cEnd().fUndef())
		return rUNDEF;

	Distance dist(csy, Distance::distSPHERE, rRadius);
	return dist.rDistance(cStart(), cEnd());
}

double DistanceMeasurer::rSphericalAzim(const double rRadius)
{
	if (cStart().fUndef() || cEnd().fUndef())
		return rUNDEF;
	LatLon llStart, llEnd;
	if (!fLatLonCoords() && !fProjectedCoords()) 
		return rUNDEF;
	else {
		llStart = csy->llConv(cStart());
		llEnd = csy->llConv(cEnd());
	}
	if (llStart.fUndef() || llEnd.fUndef())
		return rUNDEF;
	// llStart and llEnd are now LatLons in degrees
	double phi1 = llStart.Lat * M_PI/180.0; //conversion to radians
	double lam1 = llStart.Lon * M_PI/180.0; 
	double phi2 = llEnd.Lat * M_PI/180.0; 
	double lam2 = llEnd.Lon * M_PI/180.0;
	double tanazimN = sin(lam2 - lam1)* cos(phi2);
	double tanazimD = cos(phi1)*sin(phi2)- sin(phi1)*cos(phi2)* cos(lam2 - lam1);
	double rAzim = atan2(tanazimN, tanazimD) * 180.0 / M_PI;//conversion to degrees
	if (rAzim < 0)
		rAzim += 360;
	return rAzim;
}

double DistanceMeasurer::rSphericalMeridConv(const double rRadius)
{
	if (cStart().fUndef() || cEnd().fUndef())
		return rUNDEF;
	LatLon llStart, llEnd;
	Coord cEndInY; // lies due North or south (y-direction on map)
	cEndInY.x = cStart().x;
	cEndInY.y = cEnd().y;
	if (!fLatLonCoords() && !fProjectedCoords()) 
		return rUNDEF;
	else {
		llStart = csy->llConv(cStart());
		llEnd = csy->llConv(cEndInY);
	}
	if (llStart.fUndef() || llEnd.fUndef())
		return rUNDEF;
	// llStart and llEnd are now LatLons in degrees
	double phi1 = llStart.Lat * M_PI/180.0; //conversion to radians
	double lam1 = llStart.Lon * M_PI/180.0; 
	double phi2 = llEnd.Lat * M_PI/180.0; 
	double lam2 = llEnd.Lon * M_PI/180.0;
	double tanazimN = sin(lam2 - lam1)* cos(phi2);
	double tanazimD = cos(phi1)*sin(phi2)- sin(phi1)*cos(phi2)* cos(lam2 - lam1);
	double rAzim = atan2(tanazimN, tanazimD) * 180.0 / M_PI;//conversion to degrees
	if (rAzim < 0)
		rAzim += 360;
	//return rAzim;// clockw angle of mapgridnorth w.r.t. meridian
	return 360 - rAzim;// clockw angle of meridian w.r.t.mapgridnorth
}

double DistanceMeasurer::rEllipsoidDistance(const CoordSystem& cs) 
{
	double rD;
	if (cStart().fUndef())
		return rUNDEF;

	if (cEnd().fUndef())
		return rUNDEF;
	LatLon llStart, llEnd;
	if (!fLatLonCoords() && !fProjectedCoords()) 
		return rUNDEF;
	else {
		llStart = csy->llConv(cStart());
		llEnd  = csy->llConv(cEnd());
	}
	if (llStart.fUndef() || llEnd.fUndef())
		return rUNDEF;
	if (fEllipsoidalCoords()) {
		CoordSystemViaLatLon* pcsvll= cs->pcsViaLatLon();
		rD = pcsvll->ell.rEllipsoidalDistance(llStart, llEnd);
	}
	return rD;
}

double DistanceMeasurer::rEllipsoidAzimuth(const CoordSystem& cs)
{
	double rAz;
	if (cStart().fUndef() || cEnd().fUndef())
		return rUNDEF;
	LatLon llStart, llEnd;
	if (!fLatLonCoords() && !fProjectedCoords()) 
		return rUNDEF;
	else {
		llStart = csy->llConv(cStart());
		llEnd = csy->llConv(cEnd());
	}
	if (llStart.fUndef() || llEnd.fUndef())
		return rUNDEF;
	if (fEllipsoidalCoords()) {
		CoordSystemViaLatLon* pcsvll= cs->pcsViaLatLon();
		rAz = pcsvll->ell.rEllipsoidalAzimuth(llStart, llEnd);
	}
	if (rAz < 0)
		rAz += 360;
	else if (rAz > 360)
		rAz -= 360;
	return rAz;
}

Coord DistanceMeasurer::cStart() {
	if ( coords.size() > 0)
		return coords[0];
	return Coord();
}
Coord DistanceMeasurer::cEnd(){
	if ( coords.size() > 0)
		return coords[coords.size() - 1];
	return Coord();

}