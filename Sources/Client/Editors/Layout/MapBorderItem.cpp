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
// MapBorderItem.cpp: implementation of the MapBorderItem class.
//
//////////////////////////////////////////////////////////////////////

#include "Client\Headers\formelementspch.h"
#include "Client\Editors\Layout\LayoutItem.h"
#include "Client\Editors\Layout\TextLayoutItem.h"
#include "Client\Editors\Layout\MapLayoutItem.h"
#include "Client\Editors\Layout\MapBorderItem.h"
#include "Headers\constant.h"
#include "Client\Base\IlwisDocument.h"
#include "Client\Editors\Layout\LayoutDoc.h"
#include "Headers\Hs\Drwforms.hs"
#include "Headers\Hs\Editor.hs"
#include "Client\FormElements\fldcolor.h"
#include "Client\FormElements\fldfontn.h"
#include "Client\FormElements\fentdms.h"
#include "Engine\Base\Round.h"
#include "Headers\Hs\Layout.hs"
#include "Client\FormElements\FormBasePropertyPage.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

MapBorderItem::MapBorderItem(LayoutDoc* ld)
	: TextLayoutItem(ld), mli(0)
	, lnBorder(Color(0,0,0),0.5)
{
	fBorderLine = false;
	fNeatLineGraduation = false;
	fGridTicks = false;
	fGratTicks = false;
	fGridText = false;
	fGratText = false;
	fCornerCoords = false;
	fill(arfEnabledTextLocations, arfEnabledTextLocations + 5, true);
	fill(arfEnabledGratTextLocations, arfEnabledGratTextLocations + 5, true);		
	iHorzBorderTextRot = 0;
	iVertBorderTextRot = 0;	
	iHorzBorderGratTextRot = 0;
	iVertBorderGratTextRot = 0;		
}

MapBorderItem::MapBorderItem(LayoutDoc* ld, MapLayoutItem* mapli)
	: TextLayoutItem(ld)
	, mli(mapli)
{
	SetPosition(mmPosBasedOnMli(),-1);
	fBorderLine = false;
	fNeatLineGraduation = false;
	fGridTicks = false;
	fGratTicks = false;
	fGridText = false;
	fGratText = false;
	fCornerCoords = false;
	eGraduationType = eMETRES;
	fill(arfEnabledTextLocations, arfEnabledTextLocations + 5, true);	
	fill(arfEnabledGratTextLocations, arfEnabledGratTextLocations + 5, true);		
	iHorzBorderTextRot = 0;
	iVertBorderTextRot = 0;
	iHorzBorderGratTextRot = 0;
	iVertBorderGratTextRot = 0;			

	CoordBounds cb = mli->cbBounds(); // in m
	MinMax mm = mli->mmPosition(); // in 0.1 mm
	LatLon llMin = mli->llConv(cb.cMin);
	LatLon llMax = mli->llConv(cb.cMax);
	double rDist = rounding(150 * cb.width() / mm.width()); // 1.5 cm paper size
	double rLL = rRoundDMS(150 * (llMax.Lat - llMin.Lat) / mm.width()); // 1.5 cm paper size

	rNeatLineInterval = rDist;
	rGridTickInterval = rDist;
	rGridTextInterval = rDist;
	rNeatLineDegreesInterval = rLL;
	rGratTickInterval = rLL;
	rGratTextInterval = rLL;

	rTickLineWidth = 0.1; // tick line width
	rNeatLineWidth = 2.0; // 2 mm
	rGridTickLength = 3.5; // mm
	rGratTickLength = 3.5; // mm
	rGridTextOffset = 4.0; // mm
	rGratTextOffset = 4.0; // mm
	rBorderLineDist = 25.0; // 2.5 cm

	clrNeatLine = Color(0,0,0);
	clrGridTick = Color(0,0,0);
	clrGratTick = Color(0,0,0);
	clrGridText = Color(0,0,0);
	clrGratText = Color(0,0,0);
	clrCornerCoords = Color(0,0,0);

	eGridTextType = eSHORTENEDCOORDS;
	eGratTextType = eSHORTENEDCOORDS;

	lf.lfHeight = 12;
	lfGridText = lf;
	lfGratText = lf;
	lf.lfHeight = 8;
}

MapBorderItem::~MapBorderItem()
{
}

void MapBorderItem::ReadElements(ElementContainer& ec, const char* sSection)
{
	TextLayoutItem::ReadElements(ec, sSection);
	int iNr;
	ObjectInfo::ReadElement(sSection, "Map View", ec, iNr);
	mli = dynamic_cast<MapLayoutItem*>(ld->liFindID(iNr));
	fill(arfEnabledTextLocations, arfEnabledTextLocations + 5, true);
	fill(arfEnabledGratTextLocations, arfEnabledGratTextLocations + 5, true);	

	iHorzBorderTextRot = 0;
	iVertBorderTextRot = 0;	
	iHorzBorderGratTextRot = 0;
	iVertBorderGratTextRot = 0;			
	
	ObjectInfo::ReadElement(sSection, "Tick Line Width", ec, rTickLineWidth);

	ObjectInfo::ReadElement(sSection, "Neat Line Graduation", ec, fNeatLineGraduation);
	ObjectInfo::ReadElement(sSection, "Neat Line Color", ec, clrNeatLine);
	String sType;
	ObjectInfo::ReadElement(sSection, "Neat Line Graduation Type", ec, sType);
	if ("Degrees" == sType)
		eGraduationType = eDEGREES;
	else
		eGraduationType = eMETRES;
	ObjectInfo::ReadElement(sSection, "Neat Line Interval", ec, rNeatLineInterval);
	ObjectInfo::ReadElement(sSection, "Neat Line Degrees Interval", ec, rNeatLineDegreesInterval);
	ObjectInfo::ReadElement(sSection, "Neat Line Width", ec, rNeatLineWidth);

	ObjectInfo::ReadElement(sSection, "Grid Ticks", ec, fGridTicks);
	ObjectInfo::ReadElement(sSection, "Grid Tick Color", ec, clrGridTick);
	ObjectInfo::ReadElement(sSection, "Grid Tick Interval", ec, rGridTickInterval);
	ObjectInfo::ReadElement(sSection, "Grid Tick Length", ec, rGridTickLength);

	ObjectInfo::ReadElement(sSection, "Grat Ticks", ec, fGratTicks);
	ObjectInfo::ReadElement(sSection, "Grat Tick Color", ec, clrGratTick);
	ObjectInfo::ReadElement(sSection, "Grat Tick Interval", ec, rGratTickInterval);
	ObjectInfo::ReadElement(sSection, "Grat Tick Length", ec, rGratTickLength);

	ObjectInfo::ReadElement(sSection, "Grid Text", ec, fGridText);
	ObjectInfo::ReadElement(sSection, "Grid Text Color", ec, clrGridText);
	ObjectInfo::ReadElement(sSection, "Grid Text Interval", ec, rGridTextInterval);
	ObjectInfo::ReadElement(sSection, "Grid Text Offset", ec, rGridTextOffset);
	String sFont;
	ObjectInfo::ReadElement(sSection, "Grid Text Font", ec, sFont);
	lstrcpy(lfGridText.lfFaceName, sFont.c_str());
	int iLen = sizeof(LOGFONT) - LF_FACESIZE;
	ObjectInfo::ReadElement(sSection, "Grid Text LogFont", ec, (char*)&lfGridText, iLen);
	ObjectInfo::ReadElement(sSection, "Grid Text Coord Type", ec, sType);
	if ("Shortened" == sType)
		eGridTextType = eSHORTENEDCOORDS;
	else
		eGridTextType = eFULLCOORDS;

	ObjectInfo::ReadElement(sSection, "Grat Text", ec, fGratText);
	ObjectInfo::ReadElement(sSection, "Grat Text Color", ec, clrGratText);
	ObjectInfo::ReadElement(sSection, "Grat Text Interval", ec, rGratTextInterval);
	ObjectInfo::ReadElement(sSection, "Grat Text Offset", ec, rGratTextOffset);
	ObjectInfo::ReadElement(sSection, "Grat Text Font", ec, sFont);
	lstrcpy(lfGratText.lfFaceName, sFont.c_str());
	iLen = sizeof(LOGFONT) - LF_FACESIZE;
	ObjectInfo::ReadElement(sSection, "Grat Text LogFont", ec, (char*)&lfGratText, iLen);
	ObjectInfo::ReadElement(sSection, "Grat Text Coord Type", ec, sType);
	if ("Shortened" == sType)
		eGratTextType = eSHORTENEDCOORDS;
	else
		eGratTextType = eFULLCOORDS;

	ObjectInfo::ReadElement(sSection, "Corner Coords", ec, fCornerCoords);
	ObjectInfo::ReadElement(sSection, "Corner Coords Color", ec, clrCornerCoords);

	ObjectInfo::ReadElement(sSection, "Border Line", ec, fBorderLine);
	ObjectInfo::ReadElement(sSection, "Border Line Offset", ec, rBorderLineDist);
	if (fBorderLine)
		lnBorder.Read(sSection, "Border Line", ec);
	bool fDummy;
	if ( ObjectInfo::ReadElement(sSection, "EnabledBorderTexts1", ec, fDummy) != 0 )
	{
		for(int i=1; i<=4; ++i)
			ObjectInfo::ReadElement(sSection, String("EnabledBorderTexts%d",i).c_str(), ec, arfEnabledTextLocations[i]);
	}		
	if ( ObjectInfo::ReadElement(sSection, "EnabledBorderGrat Texts1", ec, fDummy ) != 0)	
	{
		for(int i=1; i<=4; ++i)
			ObjectInfo::ReadElement(sSection, String("EnabledBorderGrat Texts%d",i).c_str(), ec, arfEnabledGratTextLocations[i]);
	}		

	ObjectInfo::ReadElement(sSection, "Horizontal border text rotation", ec, iHorzBorderTextRot);
	if ( iHorzBorderTextRot == shUNDEF )
		iHorzBorderTextRot = 0;
	ObjectInfo::ReadElement(sSection, "Vertical border text rotation", ec, iVertBorderTextRot);
	if ( iVertBorderTextRot == shUNDEF )
		iVertBorderTextRot = 0;	

	ObjectInfo::ReadElement(sSection, "Horizontal border Graticule text rotation", ec, iHorzBorderGratTextRot);
	if ( iHorzBorderGratTextRot == shUNDEF )
		iHorzBorderGratTextRot = 0;	
	ObjectInfo::ReadElement(sSection, "Vertical border Graticule text rotation", ec, iVertBorderGratTextRot);
	if ( iVertBorderGratTextRot == shUNDEF )
		iVertBorderGratTextRot = 0;		

}

void MapBorderItem::WriteElements(ElementContainer& ec, const char* sSection)
{
	TextLayoutItem::WriteElements(ec, sSection);
	ObjectInfo::WriteElement(sSection, "Map View", ec, mli->iID());

	ObjectInfo::WriteElement(sSection, "Tick Line Width", ec, rTickLineWidth);

	ObjectInfo::WriteElement(sSection, "Neat Line Graduation", ec, fNeatLineGraduation);
	ObjectInfo::WriteElement(sSection, "Neat Line Color", ec, clrNeatLine);
	String sType;
	switch (eGraduationType) {
		case eDEGREES:
			sType = "Degrees"; break;
		case eMETRES:
			sType = "Metres"; break;
	}
	ObjectInfo::WriteElement(sSection, "Neat Line Graduation Type", ec, sType);
	ObjectInfo::WriteElement(sSection, "Neat Line Interval", ec, rNeatLineInterval);
	ObjectInfo::WriteElement(sSection, "Neat Line Degrees Interval", ec, rNeatLineDegreesInterval);
	ObjectInfo::WriteElement(sSection, "Neat Line Width", ec, rNeatLineWidth);

	ObjectInfo::WriteElement(sSection, "Grid Ticks", ec, fGridTicks);
	ObjectInfo::WriteElement(sSection, "Grid Tick Color", ec, clrGridTick);
	ObjectInfo::WriteElement(sSection, "Grid Tick Interval", ec, rGridTickInterval);
	ObjectInfo::WriteElement(sSection, "Grid Tick Length", ec, rGridTickLength);

	ObjectInfo::WriteElement(sSection, "Grat Ticks", ec, fGratTicks);
	ObjectInfo::WriteElement(sSection, "Grat Tick Color", ec, clrGratTick);
	String sInterVal("%0.12f", rGratTickInterval);
	ObjectInfo::WriteElement(sSection, "Grat Tick Interval", ec, sInterVal);
	ObjectInfo::WriteElement(sSection, "Grat Tick Length", ec, rGratTickLength);

	ObjectInfo::WriteElement(sSection, "Grid Text", ec, fGridText);
	ObjectInfo::WriteElement(sSection, "Grid Text Color", ec, clrGridText);
	ObjectInfo::WriteElement(sSection, "Grid Text Interval", ec, rGridTextInterval);
	ObjectInfo::WriteElement(sSection, "Grid Text Offset", ec, rGridTextOffset);
	ObjectInfo::WriteElement(sSection, "Grid Text Font", ec, lfGridText.lfFaceName);
	long iLen = sizeof(LOGFONT) - LF_FACESIZE;
	ObjectInfo::WriteElement(sSection, "Grid Text LogFont", ec, (char*)&lfGridText, iLen);
	switch (eGridTextType) {
		case eSHORTENEDCOORDS:
			sType = "Shortened"; break;
		case eFULLCOORDS:
			sType = "Full"; break;
	}
	ObjectInfo::WriteElement(sSection, "Grid Text Coord Type", ec, sType);

	ObjectInfo::WriteElement(sSection, "Grat Text", ec, fGratText);
	ObjectInfo::WriteElement(sSection, "Grat Text Color", ec, clrGratText);
	ObjectInfo::WriteElement(sSection, "Grat Text Interval", ec, rGratTextInterval);
	ObjectInfo::WriteElement(sSection, "Grat Text Offset", ec, rGratTextOffset);
	ObjectInfo::WriteElement(sSection, "Grat Text Font", ec, lfGratText.lfFaceName);
	iLen = sizeof(LOGFONT) - LF_FACESIZE;
	ObjectInfo::WriteElement(sSection, "Grat Text LogFont", ec, (char*)&lfGratText, iLen);
	switch (eGratTextType) {
		case eSHORTENEDCOORDS:
			sType = "Shortened"; break;
		case eFULLCOORDS:
			sType = "Full"; break;
	}
	ObjectInfo::WriteElement(sSection, "Grat Text Coord Type", ec, sType);

	ObjectInfo::WriteElement(sSection, "Corner Coords", ec, fCornerCoords);
	ObjectInfo::WriteElement(sSection, "Corner Coords Color", ec, clrCornerCoords);

	ObjectInfo::WriteElement(sSection, "Border Line", ec, fBorderLine);
	ObjectInfo::WriteElement(sSection, "Border Line Offset", ec, rBorderLineDist);
	if (fBorderLine)
		lnBorder.Write(sSection, "Border Line", ec);

	for(int i=1; i<=4; ++i)
		ObjectInfo::WriteElement(sSection, String("EnabledBorderTexts%d",i).c_str(), ec, arfEnabledTextLocations[i]);
	for(int i=1; i<=4; ++i)
		ObjectInfo::WriteElement(sSection, String("EnabledBorderGrat Texts%d",i).c_str(), ec, arfEnabledGratTextLocations[i]);	

	ObjectInfo::WriteElement(sSection, "Horizontal border text rotation", ec, iHorzBorderTextRot);
	ObjectInfo::WriteElement(sSection, "Vertical border text rotation", ec, iVertBorderTextRot);		

	ObjectInfo::WriteElement(sSection, "Horizontal border Graticule text rotation", ec, iHorzBorderGratTextRot);
	ObjectInfo::WriteElement(sSection, "Vertical border Graticule text rotation", ec, iVertBorderGratTextRot);		
}	

struct FindPointsX: public vector<CPoint>
{
	FindPointsX(MapLayoutItem* mli, MapLayoutItem::Side side, 
	            CPoint p0, CPoint p1, double rDX, bool fIncludeEnds)
	{
		ASSERT(rDX > 0);
		Coord c0 = mli->cConv(p0);
		Coord c1 = mli->cConv(p1);
		if (c1.x < c0.x) {
			std::swap(p0,p1);
			std::swap(c0,c1);
		}
		if (fIncludeEnds) {
			push_back(p0);
			vr.push_back(c0.x);
		}
		double rX = floor(c0.x / rDX + 1) * rDX;
		for (; rX < c1.x; rX += rDX) { 
			push_back(mli->ptBorderX(side,rX));
			vr.push_back(rX);
		}
		if (fIncludeEnds) {
			push_back(p1);
			vr.push_back(c1.x);
		}
	}
	vector<double> vr;
};

struct FindPointsY: public vector<CPoint>
{
	FindPointsY(MapLayoutItem* mli, MapLayoutItem::Side side, 
	            CPoint p0, CPoint p1, double rDY, bool fIncludeEnds)
	{
		ASSERT(rDY > 0);
		Coord c0 = mli->cConv(p0);
		Coord c1 = mli->cConv(p1);
		if (c1.y < c0.y) {
			std::swap(p0,p1);
			std::swap(c0,c1);
		}
		if (fIncludeEnds) {
			push_back(p0);
			vr.push_back(c0.y);
		}
		double rY = floor(c0.y / rDY + 1) * rDY;
		for (; rY < c1.y; rY += rDY) {
			push_back(mli->ptBorderY(side,rY));
			vr.push_back(rY);
		}
		if (fIncludeEnds)	{
			push_back(p1);
			vr.push_back(c1.y);
		}
	}
	vector<double> vr;
};

struct FindPointsLat: public vector<CPoint>
{
	FindPointsLat(MapLayoutItem* mli, MapLayoutItem::Side side, 
	            CPoint p0, CPoint p1, double rDLat, bool fIncludeEnds)
	{
		ASSERT(rDLat > 0);
		LatLon ll0 = mli->llConv(p0);
		LatLon ll1 = mli->llConv(p1);
		if (ll1.Lat < ll0.Lat) {
			std::swap(p0,p1);
			std::swap(ll0,ll1);
		}
		if (ll0.fUndef() || ll1.fUndef()) {
			ll0.Lat = -180;
			ll1.Lat = 180;
			fIncludeEnds = false;
		}
		if (fIncludeEnds) {
			push_back(p0);
			vr.push_back(ll0.Lat);
		}
		double rLat = floor(ll0.Lat / rDLat + 1) * rDLat;
		for (; rLat < ll1.Lat; rLat += rDLat) { 
			CPoint pt = mli->ptBorderLat(side,rLat);
			if (0 == pt.x || 0 == pt.y)
				continue;
			push_back(pt);
			vr.push_back(rLat);
		}
		if (fIncludeEnds) {
			push_back(p1);
			vr.push_back(ll1.Lat);
		}
	}
	vector<double> vr;
};

struct FindPointsLon: public vector<CPoint>
{
	FindPointsLon(MapLayoutItem* mli, MapLayoutItem::Side side, 
	            CPoint p0, CPoint p1, double rDLon, bool fIncludeEnds)
	{
		ASSERT(rDLon > 0);
		LatLon ll0 = mli->llConv(p0);
		LatLon ll1 = mli->llConv(p1);
		if (ll1.Lon < ll0.Lon) {
			std::swap(p0,p1);
			std::swap(ll0,ll1);
		}
		if (ll0.fUndef() || ll1.fUndef()) {
			ll0.Lon = -90;
			ll1.Lon = 90;
			fIncludeEnds = false;
		}
		if (fIncludeEnds) {
			push_back(p0);
			vr.push_back(ll0.Lon);
		}
		double rLon = floor(ll0.Lon / rDLon + 1) * rDLon;
		for (; rLon < ll1.Lon; rLon += rDLon) { 
			CPoint pt = mli->ptBorderLon(side,rLon);
			if (0 == pt.x || 0 == pt.y)
				continue;
			push_back(pt);
			vr.push_back(rLon);
		}
		if (fIncludeEnds) {
			push_back(p1);
			vr.push_back(ll1.Lon);
		}
	}
	vector<double> vr;
};

typedef vector<CPoint>::iterator VPI;
typedef vector<double>::iterator VRI;

void MapBorderItem::DrawHorizontalGraduation(CDC* cdc, vector<CPoint>& vp, int iWidth) const
{
	CBrush brBlack(clrNeatLine);
	CBrush brWhite(Color(255,255,255));
	CGdiObject* brOld = cdc->SelectObject(&brBlack);

	CPoint p0;
	CPoint p1 = vp[0];
	CRect rct(p1.x,p1.y,p1.x,p1.y+iWidth);
	bool fBlack = true;
	for(VPI vpi = vp.begin(); vpi != vp.end(); ++vpi) {
		p0 = p1;
		p1 = *vpi;
		rct.left = p0.x;
		rct.right = p1.x;
		if (fBlack)
			cdc->SelectObject(&brBlack);
		else
			cdc->SelectObject(&brWhite);
		cdc->Rectangle(&rct);
		fBlack = !fBlack;
	}
	cdc->SelectObject(brOld);
}

void MapBorderItem::DrawVerticalGraduation(CDC* cdc, vector<CPoint>& vp, int iWidth) const
{
	CBrush brBlack(clrNeatLine);
	CBrush brWhite(Color(255,255,255));
	CGdiObject* brOld = cdc->SelectObject(&brBlack);

	CPoint p0;
	CPoint p1 = vp[0];
	CRect rct(p1.x,p1.y,p1.x+iWidth,p1.y);
	bool fBlack = true;
	for(VPI vpi = vp.begin(); vpi != vp.end(); ++vpi) {
		p0 = p1;
		p1 = *vpi;
		rct.top = p0.y;
		rct.bottom = p1.y;
		if (fBlack)
			cdc->SelectObject(&brBlack);
		else
			cdc->SelectObject(&brWhite);
		cdc->Rectangle(&rct);
		fBlack = !fBlack;
	}
	cdc->SelectObject(brOld);
}

void MapBorderItem::DrawHorizontalTickMarks(CDC* cdc, vector<CPoint>& vp, int iWidth, Color clr) const
{
	CPen pen(PS_SOLID,(int)(10*rTickLineWidth),clr);
	CPen* penOld = cdc->SelectObject(&pen);

	for(VPI vpi = vp.begin(); vpi != vp.end(); ++vpi) {
		CPoint pt = *vpi;
		cdc->MoveTo(pt);
		pt.y += iWidth;
		cdc->LineTo(pt);
	}
	cdc->SelectObject(penOld);
}

void MapBorderItem::DrawVerticalTickMarks(CDC* cdc, vector<CPoint>& vp, int iWidth, Color clr) const
{
	CPen pen(PS_SOLID,(int)(10*rTickLineWidth),clr);
	CPen* penOld = cdc->SelectObject(&pen);

	for(VPI vpi = vp.begin(); vpi != vp.end(); ++vpi) {
		CPoint pt = *vpi;
		cdc->MoveTo(pt);
		pt.x += iWidth;
		cdc->LineTo(pt);
	}
	cdc->SelectObject(penOld);
}

String MapBorderItem::sTextMetric(double rVal, CoordType ct) const
{
	switch (ct) 
	{
		case eFULLCOORDS:
			return String("%.f", rVal);
		case eSHORTENEDCOORDS: 
		{
			int iPow = (int)(floor(log10(rGridTextInterval)));
			int iStep = rounding(pow(10.0,iPow));
			int iStep1 = 100;
			double rFact = rGridTextInterval / iStep;
			if (rFact - floor(rFact) > 0.01) {
				iStep1 *= 10;
				iStep /= 10;
			}
			int iVal = iStep * (iStep1 + rounding(rVal/iStep) % iStep1);
			String sVal("%i", iVal);
			return sVal.c_str() + 1;
		}
	}
	return "";
}

String MapBorderItem::sTextDMS(double rVal, bool fLat, CoordType ct) const
{
	switch (ct) 
	{
		case eFULLCOORDS:
			if (fLat)
				return LatLon::sLat(rVal);
			else
				return LatLon::sLon(rVal);
		case eSHORTENEDCOORDS: 
		{
			const double rSEC = 1/3600.0;
			const double rMIN = 1/60.0;
			rVal = abs(rVal);
			if (rGratTextInterval < rMIN) {
				rVal = rounding(rVal * 3600) % 60;
				int iVal = 100 + rounding(rVal) % 100;
				String sVal("%i\"", iVal);
				return sVal.c_str() + 1;
			}
			else if (rGratTextInterval < 1) {
				rVal = rounding(rVal * 60) % 60;
				int iVal = 100 + rounding(rVal) % 100;
				String sVal("%i\'", iVal);
				return sVal.c_str() + 1;
			}
			else {
				int iVal = rounding(rVal);
				return String("%i°", iVal);
			}
		}
	}
	return "";
}


void MapBorderItem::DrawHorizontalTexts(CDC* cdc, vector<CPoint>& vp, vector<double>& vr, int iWidth, bool fMetric, CoordType ct, MapLayoutItem::Side side, int iRot) const
{
	VRI vri = vr.begin();
	for(VPI vpi = vp.begin(); vpi != vp.end(); ++vpi, ++vri) 
	{
		CPoint pt = *vpi;
		pt.y += iWidth;
		double rVal = *vri;
		String sVal;
		if (fMetric)
			sVal = sTextMetric(rVal, ct);
		else
			sVal = sTextDMS(rVal, fLatHor, ct);
		CString str = sVal.c_str();
		CSize sz = cdc->GetTextExtent(str);
		if ( side == MapLayoutItem::sideTOP )
		{
			
			if ( iRot != 90)
			{
				pt.x += (sin(rDEGRAD(iRot)) * sz.cx) / 2;
				pt.y -= (sin(rDEGRAD(iRot)) * sz.cy);
			}
			else
			{
				pt.x += sz.cy / 2;
				pt.y -= sz.cx / 2;					
			}				

		}				
		else if ( side == MapLayoutItem::sideBOTTOM )
		{
			if ( iRot == 90)
			{
				pt.x -= sz.cy / 2;
				pt.y += sz.cx / 2;
			}				
			else
			{
				pt.x -= (sin(rDEGRAD(iRot)) * sz.cx) / 2;
				pt.y += (sin(rDEGRAD(iRot)) * sz.cx) / 2;
			}				
		}				

		cdc->TextOut(pt.x, pt.y, str);
	}
}

void MapBorderItem::DrawVerticalTexts(CDC* cdc, vector<CPoint>& vp, vector<double>& vr, int iWidth, bool fMetric, CoordType ct, MapLayoutItem::Side side, int iRot ) const
{
	VRI vri = vr.begin();
	for(VPI vpi = vp.begin(); vpi != vp.end(); ++vpi, ++vri) {
		CPoint pt = *vpi;
		pt.x += iWidth;
		double rVal = *vri;
		String sVal;
		if (fMetric)
			sVal = sTextMetric(rVal, ct);
		else
			sVal = sTextDMS(rVal, !fLatHor, ct);
		CString str = sVal.c_str();
		CSize sz = cdc->GetTextExtent(str);
		if ( side == MapLayoutItem::sideLEFT )
		{
			if ( iRot == 90.0)
			{
				pt.y -= sz.cx / 2;
				pt.x -= sz.cy;
			}				
			else
			{
				pt.y -= (sz.cy - sin(rDEGRAD(iRot)) * sz.cy) / 2;
				pt.x -= ( sin(rDEGRAD(iRot)) * sz.cy) / 2;
			}				
		}				
		else if ( side == MapLayoutItem::sideRIGHT )
		{
			if ( iRot == 90)
				pt.y += sz.cx / 2;
			else
			{
				pt.y += (-sz.cy + sin(rDEGRAD(iRot)) * sz.cy) / 2;
			}				
		}				
		else
			pt.y -= sz.cy / 2;
		cdc->TextOut(pt.x, pt.y, str);
	}
}

void MapBorderItem::OnDraw(CDC* cdc)
{
	CPen pen(PS_SOLID, 0, Color(0,0,0));
	CBrush brBlack(Color(0,0,0));
	CBrush brWhite(Color(255,255,255));
	CPen* penOld = cdc->SelectObject(&pen);
	CGdiObject* brOld = cdc->SelectStockObject(NULL_BRUSH);

	const CRect rect = mli->rectPos();
	const CPoint ptl = rect.TopLeft();
	const CPoint pbl(rect.left, rect.bottom);
	const CPoint ptr(rect.right, rect.top);
	const CPoint pbr = rect.BottomRight();
	const Coord ctl = mli->cConv(ptl);
	const Coord cbl = mli->cConv(pbl);
	const Coord ctr = mli->cConv(ptr);
	const Coord cbr = mli->cConv(pbr);

	const double rDXhor = abs(cbr.x - cbl.x);
	const double rDXver = abs(ctl.x - cbl.x);
	const bool fXhor = rDXhor > rDXver;

	const LatLon lltl = mli->llConv(ptl);
	const LatLon llbr = mli->llConv(pbr);
	const LatLon llbl = mli->llConv(pbl);
	const double rDLatHor = abs(llbr.Lat - llbl.Lat);
	const double rDLatVer = abs(lltl.Lat - llbl.Lat);
	fLatHor = rDLatHor > rDLatVer;


	// tick marks
	if (fGridTicks) {
		if (fXhor) {
			// top
			FindPointsX fpt(mli, MapLayoutItem::sideTOP, ptl, ptr, rGridTickInterval, false);
			DrawHorizontalTickMarks(cdc, fpt, (int)(-10*rGridTickLength), clrGridTick);
			// bottom
			FindPointsX fpb(mli, MapLayoutItem::sideBOTTOM, pbl, pbr, rGridTickInterval, false);
			DrawHorizontalTickMarks(cdc, fpb, (int)(10*rGridTickLength), clrGridTick);
			// left
			FindPointsY fpl(mli, MapLayoutItem::sideLEFT, ptl, pbl, rGridTickInterval, false);
			DrawVerticalTickMarks(cdc, fpl, (int)(-10*rGridTickLength), clrGridTick);
			// right
			FindPointsY fpr(mli, MapLayoutItem::sideRIGHT, ptr, pbr, rGridTickInterval, false);
			DrawVerticalTickMarks(cdc, fpr, (int)(10*rGridTickLength), clrGridTick);
		}
		else {
			// top
			FindPointsY fpt(mli, MapLayoutItem::sideTOP, ptl, ptr, rGridTickInterval, false);
			DrawHorizontalTickMarks(cdc, fpt, (int)(-10*rGridTickLength), clrGridTick);
			// bottom
			FindPointsY fpb(mli, MapLayoutItem::sideBOTTOM, pbl, pbr, rGridTickInterval, false);
			DrawHorizontalTickMarks(cdc, fpb, (int)(10*rGridTickLength), clrGridTick);
			// left
			FindPointsX fpl(mli, MapLayoutItem::sideLEFT, ptl, pbl, rGridTickInterval, false);
			DrawVerticalTickMarks(cdc, fpl, (int)(-10*rGridTickLength), clrGridTick);
			// right
			FindPointsX fpr(mli, MapLayoutItem::sideRIGHT, ptr, pbr, rGridTickInterval, false);
			DrawVerticalTickMarks(cdc, fpr, (int)(10*rGridTickLength), clrGridTick);
		}
	}

	if (fGratTicks) {
		if (rGratTickInterval <= 0)
			rGratTickInterval = 30;
		if (fLatHor) {
			// top
			FindPointsLat fpt(mli, MapLayoutItem::sideTOP, ptl, ptr, rGratTickInterval, false);
			DrawHorizontalTickMarks(cdc, fpt, (int)(-10*rGratTickLength), clrGratTick);
			// bottom
			FindPointsLat fpb(mli, MapLayoutItem::sideBOTTOM, pbl, pbr, rGratTickInterval, false);
			DrawHorizontalTickMarks(cdc, fpb, (int)(10*rGratTickLength), clrGratTick);
			// left
			FindPointsLon fpl(mli, MapLayoutItem::sideLEFT, ptl, pbl, rGratTickInterval, false);
			DrawVerticalTickMarks(cdc, fpl, (int)(-10*rGratTickLength), clrGratTick);
			// right
			FindPointsLon fpr(mli, MapLayoutItem::sideRIGHT, ptr, pbr, rGratTickInterval, false);
			DrawVerticalTickMarks(cdc, fpr, (int)(10*rGratTickLength), clrGratTick);
		}
		else {
			// top
			FindPointsLon fpt(mli, MapLayoutItem::sideTOP, ptl, ptr, rGratTickInterval, false);
			DrawHorizontalTickMarks(cdc, fpt, (int)(-10*rGratTickLength), clrGratTick);
			// bottom
			FindPointsLon fpb(mli, MapLayoutItem::sideBOTTOM, pbl, pbr, rGratTickInterval, false);
			DrawHorizontalTickMarks(cdc, fpb, (int)(10*rGratTickLength), clrGratTick);
			// left
			FindPointsLat fpl(mli, MapLayoutItem::sideLEFT, ptl, pbl, rGratTickInterval, false);
			DrawVerticalTickMarks(cdc, fpl, (int)(-10*rGratTickLength), clrGratTick);
			// right
			FindPointsLat fpr(mli, MapLayoutItem::sideRIGHT, ptr, pbr, rGratTickInterval, false);
			DrawVerticalTickMarks(cdc, fpr, (int)(10*rGratTickLength), clrGratTick);
		}
	}

	// texts
	if (fGridText) 
	{
		InitFont init(lfGridText,clrGridText,cdc, iHorzBorderTextRot);
		if (fXhor) 
		{
			// top
			if ( arfEnabledTextLocations[MapLayoutItem::sideTOP])
			{
				FindPointsX fpt(mli, MapLayoutItem::sideTOP, ptl, ptr, rGridTextInterval, false);
				cdc->SetTextAlign(TA_CENTER|TA_BOTTOM);
				int iOffset = (int)(-10*rGridTextOffset);								
				DrawHorizontalTexts(cdc, fpt, fpt.vr, iOffset, true, eGridTextType, MapLayoutItem::sideTOP, iHorzBorderTextRot);
			}
			// bottom
			if ( arfEnabledTextLocations[MapLayoutItem::sideBOTTOM])
			{			
				FindPointsX fpb(mli, MapLayoutItem::sideBOTTOM, pbl, pbr, rGridTextInterval, false);
				cdc->SetTextAlign(TA_CENTER|TA_TOP);
				int iOffset = (int)(10*rGridTextOffset);				
				DrawHorizontalTexts(cdc, fpb, fpb.vr, iOffset, true, eGridTextType, MapLayoutItem::sideBOTTOM, iHorzBorderTextRot);
			}
			InitFont init(lfGridText,clrGridText,cdc, iVertBorderTextRot);			
			// left
			if ( arfEnabledTextLocations[MapLayoutItem::sideLEFT])
			{			
				FindPointsY fpl(mli, MapLayoutItem::sideLEFT, ptl, pbl, rGridTextInterval, false);
				cdc->SetTextAlign(TA_RIGHT|TA_TOP);
				int iOffset = (int)(-10*rGridTextOffset); // (sin(rDEGRAD(iVertBorderTextRot)) + 1.0) * -10*rGridTextOffset;
				DrawVerticalTexts(cdc, fpl, fpl.vr, iOffset, true, eGridTextType, MapLayoutItem::sideLEFT, iVertBorderTextRot);
			}
			// right
			if ( arfEnabledTextLocations[MapLayoutItem::sideRIGHT])
			{			
				FindPointsY fpr(mli, MapLayoutItem::sideRIGHT, ptr, pbr, rGridTextInterval, false);
				cdc->SetTextAlign(TA_LEFT|TA_TOP);
				int iOffset = (int)(10*rGridTextOffset);
				DrawVerticalTexts(cdc, fpr, fpr.vr, iOffset, true, eGridTextType, MapLayoutItem::sideRIGHT, iVertBorderTextRot);
			}
		}
		else 
		{
			InitFont init(lfGridText,clrGridText,cdc, iHorzBorderTextRot);			
			// top
			if ( arfEnabledTextLocations[MapLayoutItem::sideTOP])
			{			
				FindPointsY fpt(mli, MapLayoutItem::sideTOP, ptl, ptr, rGridTextInterval, false);
				cdc->SetTextAlign(TA_CENTER|TA_BOTTOM);
				DrawHorizontalTexts(cdc, fpt, fpt.vr, (int)(-10*rGridTextOffset), true, eGridTextType, MapLayoutItem::sideTOP, iHorzBorderTextRot);
			}
			// bottom
			if ( arfEnabledTextLocations[MapLayoutItem::sideBOTTOM])
			{			
				FindPointsY fpb(mli, MapLayoutItem::sideBOTTOM, pbl, pbr, rGridTextInterval, false);
				cdc->SetTextAlign(TA_CENTER|TA_TOP);
				DrawHorizontalTexts(cdc, fpb, fpb.vr, (int)(10*rGridTextOffset), true, eGridTextType, MapLayoutItem::sideBOTTOM, iHorzBorderTextRot);
			}
			InitFont init2(lfGridText,clrGridText,cdc, iVertBorderTextRot);						
			// left
			if ( arfEnabledTextLocations[MapLayoutItem::sideLEFT])
			{			
				FindPointsX fpl(mli, MapLayoutItem::sideLEFT, ptl, pbl, rGridTextInterval, false);
				cdc->SetTextAlign(TA_RIGHT|TA_TOP);
	//			cdc->SetTextAlign(TA_CENTER|TA_BOTTOM);
				DrawVerticalTexts(cdc, fpl, fpl.vr, (int)(-10*rGridTextOffset), true, eGridTextType, MapLayoutItem::sideLEFT, iVertBorderTextRot);
			}
			// right
			if ( arfEnabledTextLocations[MapLayoutItem::sideRIGHT])
			{			
				FindPointsX fpr(mli, MapLayoutItem::sideRIGHT, ptr, pbr, rGridTextInterval, false);
				cdc->SetTextAlign(TA_LEFT|TA_TOP);
	//			cdc->SetTextAlign(TA_CENTER|TA_TOP);
				DrawVerticalTexts(cdc, fpr, fpr.vr, (int)(10*rGridTextOffset), true, eGridTextType, MapLayoutItem::sideRIGHT, iVertBorderTextRot);
			}
		}
	}

	if (fGratText) {
		if (rGratTextInterval <= 0)
			rGratTextInterval = 30;
		if (fLatHor) 
		{
			InitFont init(lfGratText,clrGratText,cdc, iHorzBorderGratTextRot);			
			if ( arfEnabledGratTextLocations[MapLayoutItem::sideTOP])
			{				
			// top
				FindPointsLat fpt(mli, MapLayoutItem::sideTOP, ptl, ptr, rGratTextInterval, false);
				cdc->SetTextAlign(TA_CENTER|TA_BOTTOM);
				DrawHorizontalTexts(cdc, fpt, fpt.vr, (int)(-10*rGratTextOffset), false, eGratTextType, MapLayoutItem::sideTOP, iHorzBorderGratTextRot);
			}
			// bottom
			if ( arfEnabledGratTextLocations[MapLayoutItem::sideBOTTOM])
			{			
				FindPointsLat fpb(mli, MapLayoutItem::sideBOTTOM, pbl, pbr, rGratTextInterval, false);
				cdc->SetTextAlign(TA_CENTER|TA_TOP);
				DrawHorizontalTexts(cdc, fpb, fpb.vr, (int)(10*rGratTextOffset), false, eGratTextType, MapLayoutItem::sideBOTTOM, iHorzBorderGratTextRot);
			}
			InitFont init2(lfGratText,clrGratText,cdc, iVertBorderGratTextRot);						
			// left
			if ( arfEnabledGratTextLocations[MapLayoutItem::sideLEFT])
			{			
				FindPointsLon fpl(mli, MapLayoutItem::sideLEFT, ptl, pbl, rGratTextInterval, false);
				cdc->SetTextAlign(TA_RIGHT|TA_TOP);
	//			cdc->SetTextAlign(TA_CENTER|TA_BOTTOM);
				DrawVerticalTexts(cdc, fpl, fpl.vr, (int)(-10*rGratTextOffset), false, eGratTextType, MapLayoutItem::sideLEFT, iVertBorderGratTextRot);
			}
			// right
			if ( arfEnabledGratTextLocations[MapLayoutItem::sideRIGHT])
			{			
				FindPointsLon fpr(mli, MapLayoutItem::sideRIGHT, ptr, pbr, rGratTextInterval, false);
				cdc->SetTextAlign(TA_LEFT|TA_TOP);
//			cdc->SetTextAlign(TA_CENTER|TA_TOP);
				DrawVerticalTexts(cdc, fpr, fpr.vr, (int)(10*rGratTextOffset), false, eGratTextType, MapLayoutItem::sideRIGHT, iVertBorderGratTextRot);
			}				
		}
		else 
		{
			InitFont init(lfGratText,clrGratText,cdc, iHorzBorderGratTextRot);						
			// top
			if ( arfEnabledGratTextLocations[MapLayoutItem::sideTOP])
			{			
				FindPointsLon fpt(mli, MapLayoutItem::sideTOP, ptl, ptr, rGratTextInterval, false);
				cdc->SetTextAlign(TA_CENTER|TA_BOTTOM);
				DrawHorizontalTexts(cdc, fpt, fpt.vr, (int)(-10*rGratTextOffset), false, eGratTextType, MapLayoutItem::sideTOP, iHorzBorderGratTextRot);
			}				
			// bottom
			if ( arfEnabledGratTextLocations[MapLayoutItem::sideBOTTOM])
			{			
				FindPointsLon fpb(mli, MapLayoutItem::sideBOTTOM, pbl, pbr, rGratTextInterval, false);
				cdc->SetTextAlign(TA_CENTER|TA_TOP);
				DrawHorizontalTexts(cdc, fpb, fpb.vr, (int)(10*rGratTextOffset), false, eGratTextType, MapLayoutItem::sideBOTTOM, iHorzBorderGratTextRot);
			}
			// left
			InitFont init2(lfGratText,clrGratText,cdc, iVertBorderGratTextRot);						
			if ( arfEnabledGratTextLocations[MapLayoutItem::sideLEFT])
			{			
				FindPointsLat fpl(mli, MapLayoutItem::sideLEFT, ptl, pbl, rGratTextInterval, false);
				cdc->SetTextAlign(TA_RIGHT|TA_TOP);
				DrawVerticalTexts(cdc, fpl, fpl.vr, (int)(-10*rGratTextOffset), false, eGratTextType, MapLayoutItem::sideLEFT, iVertBorderGratTextRot);
			}
			// right
			if ( arfEnabledGratTextLocations[MapLayoutItem::sideRIGHT])
			{			
				FindPointsLat fpr(mli, MapLayoutItem::sideRIGHT, ptr, pbr, rGratTextInterval, false);
				cdc->SetTextAlign(TA_LEFT|TA_TOP);
				DrawVerticalTexts(cdc, fpr, fpr.vr, (int)(10*rGratTextOffset), false, eGratTextType, MapLayoutItem::sideRIGHT, iVertBorderGratTextRot);
			}				
		}
	}

	// neatline
	CRect rct = rect;
	CPen penNeatLine(PS_SOLID, (int)(10*rTickLineWidth), clrNeatLine);
	cdc->SelectObject(&penNeatLine);
	cdc->Rectangle(&rct);
	if (fNeatLineGraduation) {
		int iDist = (int)(rNeatLineWidth * 10);
		rct.top -= iDist;
		rct.bottom += iDist;
		rct.left -= iDist;
		rct.right += iDist;
		cdc->Rectangle(&rct);
		if (eGraduationType == eMETRES) {
			if (fXhor) {
				// top
				FindPointsX fpt(mli, MapLayoutItem::sideTOP, ptl, ptr, rNeatLineInterval, true);
				DrawHorizontalGraduation(cdc, fpt, (int)(-rNeatLineWidth*10));
				// bottom
				FindPointsX fpb(mli, MapLayoutItem::sideBOTTOM, pbl, pbr, rNeatLineInterval, true);
				DrawHorizontalGraduation(cdc, fpb, (int)(rNeatLineWidth*10));
				// left
				FindPointsY fpl(mli, MapLayoutItem::sideLEFT, ptl, pbl, rNeatLineInterval, true);
				DrawVerticalGraduation(cdc, fpl, (int)(-rNeatLineWidth*10));
				// right
				FindPointsY fpr(mli, MapLayoutItem::sideRIGHT, ptr, pbr, rNeatLineInterval, true);
				DrawVerticalGraduation(cdc, fpr, (int)(rNeatLineWidth*10));
			}
			else {
				// top
				FindPointsY fpt(mli, MapLayoutItem::sideTOP, ptl, ptr, rNeatLineInterval, true);
				DrawHorizontalGraduation(cdc, fpt, (int)(-rNeatLineWidth*10));
				// bottom
				FindPointsY fpb(mli, MapLayoutItem::sideBOTTOM, pbl, pbr, rNeatLineInterval, true);
				DrawHorizontalGraduation(cdc, fpb, (int)(rNeatLineWidth*10));
				// left
				FindPointsX fpl(mli, MapLayoutItem::sideLEFT, ptl, pbl, rNeatLineInterval, true);
				DrawVerticalGraduation(cdc, fpl, (int)(-rNeatLineWidth*10));
				// right
				FindPointsX fpr(mli, MapLayoutItem::sideRIGHT, ptr, pbr, rNeatLineInterval, true);
				DrawVerticalGraduation(cdc, fpr, (int)(rNeatLineWidth*10));
			}
		}
		else {
			if (fLatHor) {
				// top
				FindPointsLat fpt(mli, MapLayoutItem::sideTOP, ptl, ptr, rNeatLineDegreesInterval, true);
				DrawHorizontalGraduation(cdc, fpt, (int)(-rNeatLineWidth*10));
				// bottom
				FindPointsLat fpb(mli, MapLayoutItem::sideBOTTOM, pbl, pbr, rNeatLineDegreesInterval, true);
				DrawHorizontalGraduation(cdc, fpb, (int)(rNeatLineWidth*10));
				// left
				FindPointsLon fpl(mli, MapLayoutItem::sideLEFT, ptl, pbl, rNeatLineDegreesInterval, true);
				DrawVerticalGraduation(cdc, fpl, (int)(-rNeatLineWidth*10));
				// right
				FindPointsLon fpr(mli, MapLayoutItem::sideRIGHT, ptr, pbr, rNeatLineDegreesInterval, true);
				DrawVerticalGraduation(cdc, fpr, (int)(rNeatLineWidth*10));
			}
			else {
				// top
				FindPointsLon fpt(mli, MapLayoutItem::sideTOP, ptl, ptr, rNeatLineDegreesInterval, true);
				DrawHorizontalGraduation(cdc, fpt, (int)(-rNeatLineWidth*10));
				// bottom
				FindPointsLon fpb(mli, MapLayoutItem::sideBOTTOM, pbl, pbr, rNeatLineDegreesInterval, true);
				DrawHorizontalGraduation(cdc, fpb, (int)(rNeatLineWidth*10));
				// left
				FindPointsLat fpl(mli, MapLayoutItem::sideLEFT, ptl, pbl, rNeatLineDegreesInterval, true);
				DrawVerticalGraduation(cdc, fpl, (int)(-rNeatLineWidth*10));
				// right
				FindPointsLat fpr(mli, MapLayoutItem::sideRIGHT, ptr, pbr, rNeatLineDegreesInterval, true);
				DrawVerticalGraduation(cdc, fpr, (int)(rNeatLineWidth*10));
			}
		}
	}

	// corner coords
	if (fCornerCoords) {
		InitFont init(lf,clrCornerCoords,cdc);
		int iWidth = 10;
		if (fNeatLineGraduation)
			iWidth += 10 * rNeatLineWidth;
		vector<CPoint> vp(2);
		vector<double> vr(2);
		vp[0] = ptl;
		vp[1] = ptr;
		if (fXhor) {
			vr[0] = ctl.x;
			vr[1] = ctr.x;
		} else {
			vr[0] = ctl.y;
			vr[1] = ctr.y;
		}
		cdc->SetTextAlign(TA_CENTER|TA_BOTTOM);
		DrawHorizontalTexts(cdc, vp, vr, -iWidth, true, eFULLCOORDS);
		// bottom
		vp[0] = pbl;
		vp[1] = pbr;
		if (fXhor) {
			vr[0] = cbl.x;
			vr[1] = cbr.x;
		} else {
			vr[0] = cbl.y;
			vr[1] = cbr.y;
		}
		cdc->SetTextAlign(TA_CENTER|TA_TOP);
		DrawHorizontalTexts(cdc, vp, vr, iWidth, true, eFULLCOORDS);
		// left
		vp[0] = ptl;
		vp[1] = pbl;
		if (fXhor) {
			vr[0] = ctl.y;
			vr[1] = cbl.y;
		} else {
			vr[0] = ctl.x;
			vr[1] = cbl.x;
		}
		cdc->SetTextAlign(TA_RIGHT|TA_TOP);
		DrawVerticalTexts(cdc, vp, vr, -iWidth, true, eFULLCOORDS);
		// right
		vp[0] = ptr;
		vp[1] = pbr;
		if (fXhor) {
			vr[0] = ctr.y;
			vr[1] = cbr.y;
		} else {
			vr[0] = ctr.x;
			vr[1] = cbr.x;
		}
		cdc->SetTextAlign(TA_LEFT|TA_TOP);
		DrawVerticalTexts(cdc, vp, vr, iWidth, true, eFULLCOORDS);
	}

	// border 
	if (fBorderLine) {
		CRect rect = mli->rectPos();
		int iDist = (int)(rBorderLineDist * 10);
		rect.top -= iDist;
		rect.bottom += iDist;
		rect.left -= iDist;
		rect.right += iDist;
		lnBorder.drawRectangle(cdc, rect);
	}

	cdc->SelectObject(penOld);
	cdc->SelectObject(brOld);
}

String MapBorderItem::sType() const
{
	return "MapBorder";
}

String MapBorderItem::sName() const
{
	if (0 == mli)
		return TR("Map Border");
	else
		return String(TR("Map Border of %S").c_str(), mli->sName());
}

bool MapBorderItem::fDependsOn(LayoutItem* li)
{
	return li == mli;
}

bool MapBorderItem::fOnChangedItemSize(LayoutItem* li)
{
	if (li == mli)
		SetPosition(mmPosBasedOnMli(),-1);
	return li == this || li== mli;
}

MinMax MapBorderItem::mmPosBasedOnMli() const
{
	MinMax mm = mli->mmPosition();
	mm.MinCol() -= 300;
	mm.MaxCol() += 300;
	mm.MinRow() -= 300;
	mm.MaxRow() += 300;
	return mm;
}

MinMax MapBorderItem::mmWishForMli(MinMax mm) const
{
	mm.MinCol() += 300;
	mm.MaxCol() -= 300;
	mm.MinRow() += 300;
	mm.MaxRow() -= 300;
	return mm;
}

void MapBorderItem::SetPosition(MinMax mm, int iHit)
{
	if (0 == mli) {
		LayoutItem::SetPosition(mm, iHit);
		return;
	}
	MinMax mmViewPos = mmWishForMli(mm);
	MinMax mmOldViewPos = mli->mmPosition();
	if (mmViewPos != mmOldViewPos) {
		mli->SetPosition(mmViewPos, iHit);
		ld->UpdateAllViews(0, LayoutDoc::hintITEMSIZE, mli);
	}
	else 
		LayoutItem::SetPosition(mm, iHit);
}


class MapBorderPropPage: public FormBasePropertyPage
{
public:
	MapBorderPropPage(const String& sTitle, LayoutDoc* doc)
		: FormBasePropertyPage(sTitle)
		, ld(doc)
		{}
	BOOL OnApply()
	{
		FormEntry *pfe = CheckData();
		if (pfe) 
		{
			MessageBeep(MB_ICONEXCLAMATION);
			pfe->SetFocus();
			return FALSE;
		}
		BOOL fRet = FormBasePropertyPage::OnApply();
		if (fRet) {
			ld->SetModifiedFlag();
			ld->UpdateAllViews(0, LayoutDoc::hintITEM, 0);
		}
		return fRet;
	}
private:
	LayoutDoc* ld;
};

class MapBorderItemForm : public CPropertySheet
{
public:
  MapBorderItemForm(CWnd* wnd, MapBorderItem* mbi) 
		: CPropertySheet(TR("Edit Map Border").c_str(), wnd)
  {
		// --- NeatLine
		ppNeatLine = new MapBorderPropPage(TR("Neat Line"), mbi->ld);
		FormEntry* root = ppNeatLine->feRoot();

		// neatline
		new FieldColor(root, TR("&Color"), &mbi->clrNeatLine);
		CheckBox* cb = new CheckBox(root, TR("Neat Line &Graduation"), &mbi->fNeatLineGraduation);
		cb->SetIndependentPos();
		FieldGroup* fg = new FieldGroup(cb);
		fg->Align(cb,AL_UNDER);
		RadioGroup* rg = new RadioGroup(fg, "", (int*)&mbi->eGraduationType);
		RadioButton* rbMetres = new RadioButton(rg, TR("&Meters"));
		RadioButton* rbDegrees = new RadioButton(rg, TR("&Degrees"));
		FieldReal* frMetres = new FieldReal(rbMetres, TR("&Interval (m)"), &mbi->rNeatLineInterval, ValueRange(0.1,1e9,0.1));
		frMetres->Align(rbDegrees, AL_UNDER);
		FieldDMS* fdDegrees = new FieldDMS(rbDegrees, TR("&Interval"), &mbi->rNeatLineDegreesInterval, 50);
		fdDegrees->Align(rbDegrees, AL_UNDER);
		new FieldReal(fg, TR("&Width (mm)"), &mbi->rNeatLineWidth, ValueRange(0.1,20,0.1));
		new FieldBlank(root);

		ppNeatLine->SetMenHelpTopic("ilwismen\\layout_editor_insert_edit_map_border_neatline.htm");
		ppNeatLine->create();
		AddPage(ppNeatLine);

		// --- Grid 
		ppGrid = new MapBorderPropPage(TR("Grid"), mbi->ld);
		root = ppGrid->feRoot();

		// ticks
		cb = new CheckBox(root, TR("&Grid Ticks"), &mbi->fGridTicks);
		cb->SetIndependentPos();
		fg = new FieldGroup(cb);
		fg->Align(cb,AL_UNDER);
		new FieldReal(fg, TR("&Interval (m)"), &mbi->rGridTickInterval, ValueRange(0.1,1e9,0.1));
		new FieldReal(fg, TR("Tick &Length (mm)"), &mbi->rGridTickLength, ValueRange(-20,30,0.1));
		new FieldColor(fg, TR("&Color"), &mbi->clrGridTick);
		new FieldBlank(root);

		// Grid text
		cb = new CheckBox(root, TR("Grid &Coordinates"), &mbi->fGridText);
		cb->SetIndependentPos();
		fg = new FieldGroup(cb);
		fg->Align(cb,AL_UNDER);
		
		StaticText *st2 = new StaticText(fg, TR("Border texts"));
		CheckBox *cb1 = new CheckBox(fg, TR("Left"), &(mbi->arfEnabledTextLocations[MapLayoutItem::sideLEFT]));
		CheckBox *cb3 = cb1;
		cb1->Align(st2, AL_UNDER);		
		cb1->SetIndependentPos();
		CheckBox *cb2 = new CheckBox(fg, TR("Right"), &(mbi->arfEnabledTextLocations[MapLayoutItem::sideRIGHT]));
		cb2->Align(cb1, AL_AFTER);
		cb2->SetIndependentPos();
		cb1 = new CheckBox(fg, TR("Top"), &(mbi->arfEnabledTextLocations[MapLayoutItem::sideTOP]));
		cb1->Align(cb2, AL_AFTER);		
		cb1->SetIndependentPos();		
		cb2 = new CheckBox(fg, TR("Bottom"), &(mbi->arfEnabledTextLocations[MapLayoutItem::sideBOTTOM]));					
		cb2->Align(cb1, AL_AFTER);
		cb2->SetIndependentPos();	
		FieldInt *fr1 = new FieldInt(fg, TR("Rotate horizontal border texts"), &mbi->iHorzBorderTextRot, ValueRangeInt(0,90), true);
		fr1->Align(cb3, AL_UNDER);
		FieldInt *fr2 = new FieldInt(fg, TR("Rotate vertical border texts"), &mbi->iVertBorderTextRot, ValueRangeInt(0,90), true);		
		
		FieldReal *fr = new FieldReal(fg, TR("&Interval (m)"), &mbi->rGridTextInterval, ValueRange(0.1,1e9,0.1));
		fr->Align(fr2, AL_UNDER);
		fr->SetIndependentPos();
		RadioGroup* rgGrid = new RadioGroup(fg, "", (int*)&mbi->eGridTextType, true);
		rgGrid->SetIndependentPos();
		new RadioButton(rgGrid, TR("&Full Coordinates"));
		new RadioButton(rgGrid, TR("&Shortened Coordinates"));
		new FieldReal(fg, TR("Text &Offset (mm)"), &mbi->rGridTextOffset, ValueRange(-10,30,0.1));
		new FieldColor(fg, TR("&Color"), &mbi->clrGridText);
    FieldLogFont *fl = new FieldLogFont(fg, &mbi->lfGridText);
	

		ppGrid->SetMenHelpTopic("ilwismen\\layout_editor_insert_edit_map_border_grid.htm");
		ppGrid->create();
		AddPage(ppGrid);


		// --- Graticule
		ppGrat = new MapBorderPropPage(TR("Graticule"), mbi->ld);
		root = ppGrat->feRoot();

		// graticule ticks
		cb = new CheckBox(root, TR("&Graticule Ticks"), &mbi->fGratTicks);
		cb->SetIndependentPos();
		fg = new FieldGroup(cb);
		fg->Align(cb,AL_UNDER);
		new FieldDMS(fg, TR("&Interval"), &mbi->rGratTickInterval, 50);
		new FieldReal(fg, TR("Tick &Length (mm)"), &mbi->rGratTickLength, ValueRange(-20,30,0.1));
		new FieldColor(fg, TR("&Color"), &mbi->clrGratTick);
		new FieldBlank(root);

		// Grat text
		cb = new CheckBox(root, TR("Graticule &Coordinates"), &mbi->fGratText);
		cb->SetIndependentPos();
		fg = new FieldGroup(cb);
		fg->Align(cb,AL_UNDER);

		st2 = new StaticText(fg, TR("Border texts"));
		cb1 = new CheckBox(fg, TR("Left"), &(mbi->arfEnabledGratTextLocations[MapLayoutItem::sideLEFT]));
		cb3 = cb1;
		cb1->Align(st2, AL_UNDER);		
		cb1->SetIndependentPos();
		cb2 = new CheckBox(fg, TR("Right"), &(mbi->arfEnabledGratTextLocations[MapLayoutItem::sideRIGHT]));
		cb2->Align(cb1, AL_AFTER);
		cb2->SetIndependentPos();
		cb1 = new CheckBox(fg, TR("Top"), &(mbi->arfEnabledGratTextLocations[MapLayoutItem::sideTOP]));
		cb1->Align(cb2, AL_AFTER);		
		cb1->SetIndependentPos();		
		cb2 = new CheckBox(fg, TR("Bottom"), &(mbi->arfEnabledGratTextLocations[MapLayoutItem::sideBOTTOM]));					
		cb2->Align(cb1, AL_AFTER);
		cb2->SetIndependentPos();	
		fr1 = new FieldInt(fg, TR("Rotate horizontal border texts"), &mbi->iHorzBorderGratTextRot, ValueRangeInt(0,90), true);
		fr1->Align(cb3, AL_UNDER);
		fr2 = new FieldInt(fg, TR("Rotate vertical border texts"), &mbi->iVertBorderGratTextRot, ValueRangeInt(0,90), true);		

		FieldDMS *fdms = new FieldDMS(fg, TR("&Interval"), &mbi->rGratTextInterval, 50);
		fdms->Align(fr2, AL_UNDER);
		fdms->SetIndependentPos();		
		RadioGroup* rgGrat = new RadioGroup(fg, "", (int*)&mbi->eGratTextType, true);
		rgGrat->SetIndependentPos();
		new RadioButton(rgGrat, TR("&Full Coordinates"));
		new RadioButton(rgGrat, TR("&Shortened Coordinates"));
		new FieldReal(fg, TR("Text &Offset (mm)"), &mbi->rGratTextOffset, ValueRange(-10,30,0.1));
		new FieldColor(fg, TR("&Color"), &mbi->clrGratText);
    new FieldLogFont(fg, &mbi->lfGratText);

		ppGrat->SetMenHelpTopic("ilwismen\\layout_editor_insert_edit_map_border_graticule.htm");
		ppGrat->create();
		AddPage(ppGrat);

		// --- Corners
		ppCorners = new MapBorderPropPage(TR("Corners"), mbi->ld);
		root = ppCorners->feRoot();

		cb = new CheckBox(root, TR("&Corner Coordinates"), &mbi->fCornerCoords);
		fg = new FieldGroup(cb);
		fg->Align(cb,AL_UNDER);
		new FieldColor(fg, TR("&Color"), &mbi->clrCornerCoords);
    new FieldLogFont(fg, &mbi->lf);
		ppCorners->SetMenHelpTopic("ilwismen\\layout_editor_insert_edit_map_border_corners.htm");
		ppCorners->create();
		AddPage(ppCorners);

		// --- Border
		ppBorder = new MapBorderPropPage(TR("Outline"), mbi->ld);
		root = ppBorder->feRoot();

		// border
		cb = new CheckBox(root, TR("&Outline"), &mbi->fBorderLine);
		fg = new FieldGroup(cb);
		fg->Align(cb,AL_UNDER);
		new FieldReal(fg, TR("Outline &Offset (mm)"), &mbi->rBorderLineDist, ValueRange(0.1, 50, 0.1));
		new FieldLine(fg, &mbi->lnBorder, true);

		ppBorder->SetMenHelpTopic("ilwismen\\layout_editor_insert_edit_map_border_outline.htm");
		ppBorder->create();
		AddPage(ppBorder);
	}
	~MapBorderItemForm()
	{
		delete ppNeatLine;
		delete ppGrid;
		delete ppGrat;
		delete ppCorners;
		delete ppBorder;
	}
	FormBasePropertyPage *ppBorder, *ppGrid, *ppGrat, *ppNeatLine, *ppCorners;
};



bool MapBorderItem::fConfigure()
{
	MapBorderItemForm frm(ld->wndGetActiveView(), this);
	int iRet = frm.DoModal();
	fTransparent = true;
	return iRet == IDOK;
}

bool MapBorderItem::fIsotropic() const
{
	if (0 == mli)
		return true;
	return mli->fIsotropic();
}

