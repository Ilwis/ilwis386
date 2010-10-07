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
#include "Client\Base\IlwisDocument.h"
#include "Client\MainWindow\Catalog\CatalogDocument.h"
#include "Engine\Map\Mapview.h"
#include "Client\Mapwindow\MapCompositionDoc.h"
#include "Client\Mapwindow\SimpleMapPaneView.h"
#include "Client\Mapwindow\MapPaneViewTool.h"
#include "Client\Mapwindow\DistanceMeasurer.h"
#include "Client\Mapwindow\InfoLine.h"
#include "Headers\Hs\Mapwind.hs"
#include "Engine\SpatialReference\Coordsys.h"
#include "Engine\SpatialReference\Ellips.h"
#include "Engine\SpatialReference\csviall.h"
#include "Engine\SpatialReference\Distance.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// DistanceMeasurer
const double rDefaultEarthRadius = 6371007.1809185;

DistanceMeasurer::DistanceMeasurer(SimpleMapPaneView* mappaneview)
: MapPaneViewTool(mappaneview), 
  mpv(mappaneview) 
{
	SetCursor(zCursor("MeasureCursor"));
	csy = mpv->GetDocument()->rootDrawer->getCoordinateSystem();
  fDown = FALSE;
}

DistanceMeasurer::~DistanceMeasurer()
{
  if (fDown) {
    drawLine();
  }
	mpv->info->text(CPoint(0,0),"");
}

void DistanceMeasurer::InfoReport()
{
  double rDist = rDistance();
	double rSphDist = rSphericalDistance(rDefaultEarthRadius);
	bool fLatLon = fLatLonCoords();
	String sMsg;
  if (rDist == rUNDEF && rSphDist == rUNDEF)
    mpv->info->text(pEnd, "");
  else {
		if (!fLatLon)
			sMsg = String("%.2f m", rDist);
		else
			sMsg = String("%.2f m", rSphDist);
    zPoint p = pEnd;
    p.x += 25;
    mpv->info->text(p, sMsg);
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
    sMsg = SMWMsgDistCalcImpossible;
  else {
		sMsg = String(SMWMsgFrom_S.scVal(),csy->sValue(cStart,0));
	  sMsg &= String("\n");
	  sMsg &= String(SMWMsgTo_S.scVal(),csy->sValue(cEnd,0));
		if (fLatLonCoords())
			sMsg &= String("\n\nNo metric co-ordinates available");
		else {
			sMsg &= String("\n\n%S: %.2f m", SMWMsgMapDistance, rDist); 
			sMsg &= String("\n%S: %.2f °", SMWMsgMapAzim, rDir);
		}
		if(fLatLonCoords() || fProjectedCoords())
		{	
			double rScaleF, rMeridConv;
			if(fEllipsoidalCoords() && (rEllDist < 800000)) { //above 800km ellips dist unreliable
				sMsg &= String("\n\n%S: %.2f m", 
							SMWMsgEllDistance, rEllDist);
				sMsg &= String("\n%S: %.2f °",	SMWMsgEllAzim, rEllAzim);
				rScaleF = rDist / rEllDist;
				rMeridConv = rEllAzim - rDir;
				if (rMeridConv > M_PI_2)
					rMeridConv = rMeridConv - M_PI * 2;
			}
			else {
				if 	(rSphDist < 1000)
					sMsg &= String("\n\n%S: %.2f m", SMWMsgSphDistance, rSphDist);
				else 
					sMsg &= String("\n\n%S: %.3f km", 
										SMWMsgSphDistance, rSphDist/1000); // long distances in km
				rScaleF = rDist / rSphDist;
				sMsg &= String("\n%S: %.2f °", SMWMsgSphAzim, rSphAzim);
				rMeridConv = rSphAzim - rDir;
				if (rMeridConv > M_PI_2)
					rMeridConv = rMeridConv - M_PI * 2;
			}
			if(!fLatLonCoords()) {
				sMsg &= String("\n\n%S: %.10f", SMWMsgScaleFactor, rScaleF);
				//sMsg &= String("\n%S: %.3f °", SMWMsgMeridConverg, rMeridConv);
				sMsg &= String("\n%S: %.3f °", SMWMsgMeridConverg, rSphericalMeridConv(rRadi));
			}
		}
	}
  mpv->MessageBox(sMsg.scVal(), SMWMsgDistance.scVal());
}

void DistanceMeasurer::drawLine()
{
	CClientDC cdc(mpv);
	int iROP = cdc.SetROP2(R2_XORPEN);
	CGdiObject* pn = cdc.SelectStockObject(WHITE_PEN);
	cdc.MoveTo(pStart);
	cdc.LineTo(pEnd);
	cdc.SelectObject(pn);
	cdc.SetROP2(iROP);
}

/*
void DistanceMeasurer::DrawSphericCurvedLine() // see GraticuleDrawer::DrawCurvedLine(  )
{
	Zpoint p1, p2, pMoving;
	p1 = pStart;
	p2 = pEnd;
	cStart = mpv->crdPnt(pStart);
	cEnd = mpv->crdPnt(pEnd);
	LatLon llMoving;
	if (cStart.fUndef() || cEnd.fUndef())
    return rUNDEF;
	LatLon llStart, llEnd;
	if (!fLatLonCoords() && !fProjectedCoords()) 
		return rUNDEF;
	else {
		llStart = csy->llConv(cStart);
		llEnd = csy->llConv(cEnd);
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

void DistanceMeasurer::OnMouseMove(UINT nFlags, CPoint point)
{
	if (fDown) {
		if (pEnd == point)
			return;
    mpv->info->ShowWindow(SW_HIDE);
		drawLine();
		pEnd = point;
		drawLine();
		InfoReport();
	}
}

void DistanceMeasurer::OnLButtonDown(UINT nFlags, CPoint point)
{
  pEnd = pStart = point;
  fDown = TRUE;
  drawLine();
}

void DistanceMeasurer::OnLButtonUp(UINT nFlags, CPoint point)
{
	mpv->info->text(point,"");
  drawLine();
  pEnd = point;
  drawLine();
  fDown = FALSE;
  if (pStart == pEnd) 
    return;
  Report();
  drawLine();
  //Stop(); // no stop (request of Petra 14/6/00)
}

double DistanceMeasurer::rDistance()
{
	cStart = mpv->crdPnt(pStart);
  if (cStart.fUndef())
    return rUNDEF;
	cEnd = mpv->crdPnt(pEnd);
  if (cEnd.fUndef())
    return rUNDEF;
  return rDist(cStart, cEnd);
}

double DistanceMeasurer::rAzim()
{
  if (cStart.fUndef() || cEnd.fUndef())
    return rUNDEF;
  double rAzim = atan2(cEnd.x - cStart.x, cEnd.y - cStart.y);
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
	cStart = mpv->crdPnt(pStart);
  if (cStart.fUndef())
    return rUNDEF;

	cEnd = mpv->crdPnt(pEnd);
  if (cEnd.fUndef())
    return rUNDEF;

	Distance dist(csy, Distance::distSPHERE, rRadius);
	return dist.rDistance(cStart, cEnd);
}

double DistanceMeasurer::rSphericalAzim(const double rRadius)
{
  if (cStart.fUndef() || cEnd.fUndef())
    return rUNDEF;
	LatLon llStart, llEnd;
	if (!fLatLonCoords() && !fProjectedCoords()) 
		return rUNDEF;
	else {
		llStart = csy->llConv(cStart);
		llEnd = csy->llConv(cEnd);
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
  if (cStart.fUndef() || cEnd.fUndef())
    return rUNDEF;
	LatLon llStart, llEnd;
	Coord cEndInY; // lies due North or south (y-direction on map)
	cEndInY.x = cStart.x;
	cEndInY.y = cEnd.y;
	if (!fLatLonCoords() && !fProjectedCoords()) 
		return rUNDEF;
	else {
		llStart = csy->llConv(cStart);
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
	cStart = mpv->crdPnt(pStart);
  if (cStart.fUndef())
    return rUNDEF;
	cEnd = mpv->crdPnt(pEnd);
  if (cEnd.fUndef())
    return rUNDEF;
	LatLon llStart, llEnd;
	if (!fLatLonCoords() && !fProjectedCoords()) 
		return rUNDEF;
	else {
		llStart = csy->llConv(cStart);
		llEnd  = csy->llConv(cEnd);
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
	if (cStart.fUndef() || cEnd.fUndef())
    return rUNDEF;
	LatLon llStart, llEnd;
	if (!fLatLonCoords() && !fProjectedCoords()) 
		return rUNDEF;
	else {
		llStart = csy->llConv(cStart);
		llEnd = csy->llConv(cEnd);
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
	
