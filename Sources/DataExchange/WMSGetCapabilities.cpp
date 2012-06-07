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
#include <stack>
#include "Engine\Base\DataObjects\URL.h"
#include "Engine\Base\DataObjects\RemoteXMLObject.h"
#include "DataExchange\WMSGetCapabilities.h"


static const char*  gMemBufId = "prodInfo";

using namespace ILWIS;

WMSGetCapabilities::WMSGetCapabilities(const URL& url) : RemoteObject() {
	URL localUrl = url;
	if ( localUrl.getQueryValue("REQUEST") == "") {
		localUrl.setQueryValue("REQUEST","GetCapabilities");
	}
	getRequest(localUrl.sVal());

}

vector<WMSLayerInfo *> WMSGetCapabilities::getLayerInfo() {
	if ( layers.size() == 0 )
		parse();
	return layers;
}

WMSGetCapabilities::~WMSGetCapabilities() {
	for(int  i =0; i < layers.size(); ++i)
		delete layers[i];
	layers.clear();
}

void WMSGetCapabilities::parse() {
	RemoteObject::parse();


	String txt((const char*)chunk.memory);
	ILWIS::XMLDocument doc(txt);
	parseLayer(doc, "//Capability/Layer", layers);
	for(int i=0; i < layers.size(); ++i)
		postprocess(layers[i]);

}

void WMSGetCapabilities::postprocess(WMSLayerInfo *info) {
	for(map<String, CoordBounds>::iterator cur = info->srs.begin(); cur !=  info->srs.end(); ++cur) {
		String srs = (*cur).first;
		for(int i = 0; i < info->layers.size(); ++i) {
			WMSLayerInfo *childInfo = info->layers[i];
			if ( childInfo->srs.find(srs) == childInfo->srs.end()) {
				childInfo->srs[srs] = (*cur).second;
			}
			postprocess(childInfo);
		}
	}
}

CoordBounds WMSGetCapabilities::parseBoundingBox(const pugi::xml_node& node, String& srs ) {
	double max,may,mix,miy;
	for(pugi::xml_attribute atr = node.first_attribute(); atr; atr = atr.next_attribute()) {
		String name = atr.name();
		name.toLower();
		if ( name == "srs")
			srs = atr.value();
		if ( name == "maxx")
			max = String("%s", atr.value()).rVal();
		if ( name == "maxy")
			may = String("%s", atr.value()).rVal();
		if ( name == "minx")
			mix = String("%s", atr.value()).rVal();
		if ( name == "miny")
			miy = String("%s", atr.value()).rVal();
	}
	return CoordBounds(Coord(mix, miy), Coord(max, may));
}

void WMSGetCapabilities::parseLayer(const ILWIS::XMLDocument& doc, const String& expr, vector<WMSLayerInfo *>& parent) {
	vector<pugi::xml_node> results;
	doc.executeXPathExpression(expr, results);

	for (int i=0; i < results.size(); ++i) {
		pugi::xml_node n = results[i];
		WMSLayerInfo *info = new WMSLayerInfo();
		for(pugi::xml_node child = n.first_child(); child; child = child.next_sibling()) {
			String name = child.name();
			name.toLower();
			if ( name == "title") {
				info->title = child.child_value();
			}
			if ( name == "name") {
				info->name = child.child_value();
			}
			if ( name == "boundingbox") {
				String srs;
				CoordBounds cb = parseBoundingBox(child,srs);
				if ( srs != "")
					info->srs[srs] = cb;
			}
			if ( name == "latlonboundingbox") {
				String srs;
				CoordBounds cb = parseBoundingBox(child,srs);
				if ( cb.fValid())
					info->bbLatLon = cb;
			}

	
		}
		parseLayer(doc, expr + "/Layer",info->layers);
		parent.push_back(info);
	}
}


//-------------------------
WMSLayerInfo::~WMSLayerInfo() {
	for(vector<WMSLayerInfo *>::iterator cur = layers.begin(); cur != layers.end(); ++cur) {
		WMSLayerInfo *info = *cur;
		delete info;
	}
}

void WMSLayerInfo::PartialCopy(WMSLayerInfo *inf) {
	//name = inf->name;
	//for(map<String, vector<double> >::iterator cur3 = inf->srs.begin(); cur3 != inf->srs.end(); ++cur3) {
	//	srs[(*cur3).first] = (*cur3).second;
	//}
}




