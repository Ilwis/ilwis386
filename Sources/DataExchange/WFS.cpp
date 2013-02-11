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

#include "Headers\toolspch.h"
#include "Engine\SpatialReference\Coordsys.h"
#include "Engine\Table\Col.h"
#include "Engine\Base\DataObjects\valrange.h"
#include "Engine\DataExchange\ForeignFormat.h"
#include "Engine\Map\Point\ilwPoint.h"
#include "Engine\Map\Point\PNT.H"
#include "Engine\Map\Polygon\POL.H"
#include "Engine\Map\Point\PNTSTORE.H"
#include "Engine\Map\Polygon\POLSTORE.H"
#include "Engine\Map\Polygon\POL14.H"
#include "Engine\Base\DataObjects\valrange.h"
#include "Engine\Domain\Dmvalue.h"
#include "Engine\Base\System\Engine.h"
#include "wfs.h"

struct wfsdata{
	String cmd;
	String dir;
};

void importWFS(const String& cmd, const String& dir) {
	//Domain dom(fnPnt, 100, dmtUNIQUEID, "Pnt");

	//PointMap mp(FileName("aap"),csy,CoordBounds(8686,l3403403,696796,3330), DomainValueRangeStruct(dm);
	//ILWIS::Point *f = CPOINT(mp->newFeature());
	//
	//Table table("somename", dm);
	//Column col = table->colNew("ppp",Domain("value"),ValueRange(0,100));
	//col->SetOwnedByTable(true);

	//table->rec(100);

	//col->PutVal(4, 33);

	//PolygonMap polm();
	//ILWIS::Polygon *pol = CPOLYGON(polm->newFeature());


	//CoordinateArraySequence *seq = new CoordinateArraySequence();
	//for(int i = 0; i < count; ++i) {
	//	double x,y,z;
	//	//funcs.ogrGetPoints(hGeom, i,&x,&y,&z);
	//	Coord c(x,y,z);
	//	seq->add(c,false);
	//}
	//if ( seq->size() < 3 || seq->front() != seq->back())
	//	return 0;
	//LinearRing *ring =  new LinearRing(seq, new GeometryFactory());

 //   pol->addBoundary(ring);
	//pol->addHole(ring);
}

UINT WfsThread(void * data) {
	wfsdata *d = (wfsdata *)data;
	getEngine()->InitThreadLocalVars();
	String cmd = d->cmd;
	String dir = d->dir;
	delete d;
	importWFS(cmd, dir);
	getEngine()->RemoveThreadLocalVars();

	return 1;
}


void wfsimportlayer(const String& cmd) {
	ParmList parms(cmd);
	if ( !parms.fExist("quiet")) {
		wfsdata *d = new wfsdata();
		d->cmd = cmd;
		d->dir = getEngine()->sGetCurDir();
		AfxBeginThread(WfsThread, (LPVOID)d);
	} 
}