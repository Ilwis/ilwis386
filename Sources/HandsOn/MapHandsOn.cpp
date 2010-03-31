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
#include "HandsOn\MapHandsOn.h"
#include "Headers\Htp\Ilwisapp.htp"
#include "Headers\Err\Ilwisapp.err"
#include "Headers\Hs\map.hs"
#include "Headers\Hs\tbl.hs"

IlwisObjectPtr * createMapHandsOn(const FileName& fn, IlwisObjectPtr& ptr, const String& sExpr, vector<void *> parms ) {
	if ( sExpr != "")
		return (IlwisObjectPtr *)MapHandsOn::create(fn, (MapPtr &)ptr, sExpr);
	else
		return (IlwisObjectPtr *)new MapHandsOn(fn, (MapPtr &)ptr);
}

// returns the correct syntax of the object
const char* MapHandsOn::sSyntax() {
	return "";
}

// return the expression with the values as they are for this instance of the application object
String MapHandsOn::sExpression() const {
	return "";
}

// does the parsing of the expression and creates an application object
MapHandsOn* MapHandsOn::create(const FileName& fn, MapPtr& p, const String& sExpr){

	Array<String> as(2);
	// automatic parsing of the expression
	if (!IlwisObjectPtr::fParseParm(sExpr, as))
		ExpressionError(sExpr, sSyntax());

	return NULL;
}

// constructs a totally new application object
MapHandsOn::MapHandsOn(const FileName& fn, MapPtr& p, Map& inputMap)
: MapFromMap(fn, p, inputMap) {
	//administrative stuff of ILWIS
	if (!fnObj.fValid())
		objtime = objdep.tmNewest(); 

}

// recreates an existing application object from its definition on disk
MapHandsOn::MapHandsOn(const FileName& fn, MapPtr& p)
: MapFromMap(fn, p) {

}

// stores all relevant information about this application on disk
void MapHandsOn::Store() {
  MapFromMap::Store();

}

// Does the actual computation for an application
void MapHandsOn::ComputeLineRaw(long iLine, ByteBuf& buf, long iFrom, long iNum) const {

}








