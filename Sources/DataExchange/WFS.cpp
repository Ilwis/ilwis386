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

#include "Headers\messages.h"
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
#include "Engine\DataExchange\gdalproxy.h"
#include <cstdlib> 
#include <time.h>
#include "wfs.h"
#include "Log.h"
#include "Engine/Base/XML/pugixml.hpp"
#include "ConnectorWFS.h"

struct wfsdata{
	String cmd;
	String dir;
};




void importWFS(const String& cmd, const String& dir) {

	//initialize();
	//FOR NOW WE ASUME THAT "dir" COINTAINS THE ADDRRESS OF THE LAYER WE WANT TO RETRIEVE******************************************

	//Create a connector object to handle the "bridge" between the service and ILWIS an the log.
	const std::string str_url = cmd; //"http://localhost:8080/geoserver/ows?";
	ConnectorWFS *conn = new ConnectorWFS(str_url); //Create object in the heap
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



