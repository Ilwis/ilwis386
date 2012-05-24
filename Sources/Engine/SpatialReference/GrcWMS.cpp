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
/* GeoRefCornersWMS
   Copyright Ilwis System Development ITC
   march 1995, by Wim Koolhoven
	Last change:  WK   11 Jun 98    2:03 pm
*/
#include "Engine\SpatialReference\Gr.h"
#include "Engine\SpatialReference\Grsmpl.h"
#include "Engine\SpatialReference\Grcornrs.h"
#include "Engine\Table\tbl.h"
#include "Engine\DataExchange\ForeignFormat.h"
#include "Engine\SpatialReference\GrcWMS.h"

const char* GeoRefCornersWMS::sSyntax()
{
  return "GeoRefCornersWMS(rows,cols,cornersofcorners,minx,miny,maxx,maxy)\ncornersofcorners=0,1";
}

GeoRefCornersWMS::GeoRefCornersWMS(const FileName& fn)
: GeoRefCorners(fn)
, wmsFormat (0)
, retrieveImageProc(0)
{
	fChanged = false;
	crdMinInit = crdMin;
	crdMaxInit = crdMax;
}

String GeoRefCornersWMS::sType() const
{
  return "GeoReference Corners WMS";
}

String GeoRefCornersWMS::sName(bool fExt, const String& sDirRelative) const
{
  String s = IlwisObjectPtr::sName(fExt, sDirRelative);
  if (s.length() != 0)
    return s;
  return String("GeoRefCornersWMS(%li,%li,%i,%g,%g,%g,%g)", 
                 this->rcSize().Row, rcSize().Col, fCornersOfCorners,
                 crdMin.x, crdMin.y, crdMax.x, crdMax.y);
}

void GeoRefCornersWMS::Store()
{
  GeoRefCorners::Store();
  WriteElement("GeoRef", "Type", "GeoRefCornersWMS");
  DoNotStore(true); // store only once and never more
}

CoordBounds GeoRefCornersWMS::calcNewCB(MinMax mx, MinMax zoom, MinMax oldBounds) {
	double f1 = (double)zoom.width() / oldBounds.width();
	double f2 = (double)zoom.height() / oldBounds.height();
	double f3 = (double)(zoom.MinCol() - oldBounds.MinCol()) / oldBounds.width(); 
	double f4 = (double)(zoom.MinRow() - oldBounds.MinRow()) / oldBounds.height();
	double x1 = crdMin.x + f3 * (crdMax.x - crdMin.x);
	double y1 = crdMax.y  - f4 * (crdMax.y - crdMin.y);
	double w = abs(crdMin.x - crdMax.x) * f1;
	double h = abs(crdMin.y - crdMax.y) * f2;
	double x2 = x1 + w;
	double y2 = y1 - h;
	if ( x1 == crdMin.x && y2 == crdMin.y && x2 == crdMax.x && y1 == crdMax.y)
		fChanged = false;
	else 
		fChanged = true;

	return CoordBounds(Coord(x1,y1), Coord(x2, y2));
}

void GeoRefCornersWMS::AdjustBounds(MinMax mx, MinMax zoom, MinMax oldBounds,bool recalcCoords) {
	if (oldBounds.width() > 0 && oldBounds.height() > 0 && recalcCoords) {

		CoordBounds cb = calcNewCB(mx, zoom, oldBounds);
		crdMin = cb.cMin;
		crdMax = cb.cMax;
		if ( recalcCoords )
			fChanged = true;
	}
	setMinMax(mx);
}

void GeoRefCornersWMS::setMinMax(MinMax mx) {
	_rc = mx.rcSize();
	Compute();
}

bool GeoRefCornersWMS::hasChanged() {
	return fChanged;
}

void GeoRefCornersWMS::resetChanged(){
	fChanged = false;
}

bool GeoRefCornersWMS::initial() {
	if ( fInitial) {
		crdMinInit = crdMin;
		crdMaxInit = crdMax;
	}
	bool temp = fInitial;
	fInitial = false;
	return temp;

}

void GeoRefCornersWMS::reset() {
	crdMin = crdMinInit;
	crdMax = crdMaxInit;
}

CoordBounds GeoRefCornersWMS::getInitialCoordBounds() {
	return CoordBounds(crdMinInit, crdMaxInit);
}

void GeoRefCornersWMS::Lock() {
	csHandleRequest.Lock();
}

void GeoRefCornersWMS::Unlock() {
	csHandleRequest.Unlock();
}

CoordBounds GeoRefCornersWMS::cbWMSRequest() const {
	return m_cbWMSRequest;
}

RowCol GeoRefCornersWMS::rcWMSRequest() const {
	return m_rcWMSRequest;
}

void GeoRefCornersWMS::SetRCWMSRequest(RowCol rc) {
	m_rcWMSRequest = rc;
}

void GeoRefCornersWMS::SetCBWMSRequest(const CoordBounds & cb) {
	m_cbWMSRequest = cb;
}

void GeoRefCornersWMS::SetRetrieveProc(ForeignFormat* context, RetrieveImageProc proc) {
	wmsFormat = context;
	retrieveImageProc = proc;
}

bool GeoRefCornersWMS::retrieveImage() {
	return (wmsFormat->*retrieveImageProc)();
}