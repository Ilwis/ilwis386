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

#include <xercesc/parsers/SAXParser.hpp>
#include <xercesc/framework/MemBufInputSource.hpp>
#include <xercesc/util/OutOfMemoryException.hpp>
#include <xercesc/sax/HandlerBase.hpp>
#include <stack>
#include "Engine\Base\DataObjects\URL.h"
#include "Engine\Base\DataObjects\RemoteXMLObject.h"
#include "DataExchange\WMSGetCapabilities.h"

static const char*  gMemBufId = "prodInfo";

WMSGetCapabilities::WMSGetCapabilities(const URL& url) : RemoteXMLObject(url) {
}

vector<WMSLayerInfo *> WMSGetCapabilities::getLayerInfo() {
	if ( layers.size() == 0 )
		parse();
	return layers;
}

void WMSGetCapabilities::parse() {
	XERCES_CPP_NAMESPACE::XMLPlatformUtils::Initialize();
	RemoteXMLObject::parse();

	XERCES_CPP_NAMESPACE::WMSSaxHandler handler(this);
	parser->setDocumentHandler(&handler);
	parser->setErrorHandler(&handler);

	XERCES_CPP_NAMESPACE::MemBufInputSource* source = new XERCES_CPP_NAMESPACE::MemBufInputSource
    (
        (const XMLByte*)chunk.memory
        , (const unsigned int)chunk.size
        , gMemBufId
        , false
    );

	parser->parse(*source);
	int p = parser->getErrorCount();

	layers = handler.getLayers();
	for(vector<WMSLayerInfo *>::iterator cur = layers.begin(); cur != layers.end(); ++cur) {
		WMSLayerInfo *lyr = *cur;
		propagateSRSInfo(lyr);
	}
	delete source;

	XERCES_CPP_NAMESPACE::XMLPlatformUtils::Terminate();

}

void WMSGetCapabilities::propagateSRSInfo(WMSLayerInfo *prevlyr) {
	for(vector<WMSLayerInfo *>::iterator nextlyr = prevlyr->layers.begin(); nextlyr != prevlyr->layers.end(); ++nextlyr   ) {
		for(map<String, vector<double>>::iterator cursrs = (*nextlyr)->srs.begin(); cursrs != (*nextlyr)->srs.end() ; ++cursrs) {
				for(map<String, vector<double>>::iterator cursrs2 = prevlyr->srs.begin(); cursrs2 != prevlyr->srs.end() ; ++cursrs2) {
					if ( (*cursrs).first == (*cursrs2).first && (*cursrs).second.size() == 0 && (*cursrs2).second.size() > 0) {
						(*cursrs).second.push_back((*cursrs2).second[0]);
						(*cursrs).second.push_back((*cursrs2).second[1]);
						(*cursrs).second.push_back((*cursrs2).second[2]);
						(*cursrs).second.push_back((*cursrs2).second[3]);
				}
			}
		} 
		propagateSRSInfo(*nextlyr);
	}

}
	


//--------------------------------------------
XERCES_CPP_NAMESPACE::WMSSaxHandler::WMSSaxHandler(WMSGetCapabilities *cap) {
	capabilities = cap;
	prev = cur = 0;
	inStyle = false;
	root = currentLayer = new WMSLayerInfo("");
}

XERCES_CPP_NAMESPACE::WMSSaxHandler::~WMSSaxHandler() {
}

void XERCES_CPP_NAMESPACE::WMSSaxHandler::startElement(const XMLCh* const name, AttributeList&  attributes)
{
	currentTag = String(StrX(name).scVal());
	if ( currentTag == "Layer" ) {
		WMSLayerInfo *layer = new WMSLayerInfo("");
		currentLayer->layers.push_back(layer);
		if (prev != cur) {
			lstack.push(currentLayer);
			layer->prevLayer = currentLayer;
		}
		else
			layer->prevLayer = currentLayer->prevLayer;
		currentLayer = layer;
		prev = cur++;
	}
	if ( currentTag == "LatLonBoundingBox") {
		currentLayer->bbLatLon = handleBBBounds(attributes);
	}

	if ( currentTag == "BoundingBox") {
		handleBoundingBox(currentLayer->srs, attributes);
	}

	if ( currentTag == "Style")
		inStyle = true;
	if ( currentTag == "HTML" )
		throw ErrorObject("Fatal error in request");
}

void XERCES_CPP_NAMESPACE::WMSSaxHandler::handleBoundingBox(map<String, vector<double> >& srs, AttributeList&  attributes) {
	String srsName = String(StrX(attributes.getValue("SRS")).scVal());
	vector<double> v = handleBBBounds(attributes);
	srs[srsName] = v;
}

vector<double> XERCES_CPP_NAMESPACE::WMSSaxHandler::handleBBBounds(AttributeList&  attributes) {
	vector<double> bb;
	bb.push_back(StrX(attributes.getValue("minx")).rVal());
	bb.push_back(StrX(attributes.getValue("miny")).rVal());
	bb.push_back(StrX(attributes.getValue("maxx")).rVal());
	bb.push_back(StrX(attributes.getValue("maxy")).rVal());

	return bb;
}

void XERCES_CPP_NAMESPACE::WMSSaxHandler::handleLayerEnd(String &tag)
{
	if ( tag == "Layer" ) {
		currentTag = "";
		currentLayer = lstack.top();
		if ( cur == prev) {
			lstack.pop();
			if ( lstack.size() != 0)
				currentLayer = lstack.top();
			else
				currentLayer = root;
		}
		--cur;
	}
}

void XERCES_CPP_NAMESPACE::WMSSaxHandler::endElement(const XMLCh* const name) {
	String tag(StrX(name).scVal());
	handleLayerEnd(tag);
	if ( tag == "Style")
		inStyle = false;
}

void XERCES_CPP_NAMESPACE::WMSSaxHandler::characters(const XMLCh* const chars, const unsigned int length)
{
	String sVal = String(StrX(chars).scVal());
	if ( currentTag == "Title" && !inStyle) {
		if ( currentLayer->title == "")
			currentLayer->title = sVal;
	}
	if ( currentTag == "Name" && !inStyle) {
		if ( currentLayer->name == "")
			currentLayer->name = sVal;
	}
	if ( currentTag == "ServiceException" )
		throw ErrorObject(sVal);
	if ( currentTag == "SRS") {
		currentLayer->srs[sVal] = vector<double>();
	}
}

vector<WMSLayerInfo *> XERCES_CPP_NAMESPACE::WMSSaxHandler::getLayers() {
	return root->layers;
}

void XERCES_CPP_NAMESPACE::WMSSaxHandler::ignorableWhitespace(const XMLCh* const chars, const unsigned int length)
{
}

void XERCES_CPP_NAMESPACE::WMSSaxHandler::resetDocument()
{
}

void  XERCES_CPP_NAMESPACE::WMSSaxHandler::error(const SAXParseException& e)
{
	String sError("Error : %S at line %d column %d", String(StrX(e.getMessage()).scVal()), e.getLineNumber(), e.getColumnNumber());
	throw ErrorObject(sError);
}

void  XERCES_CPP_NAMESPACE::WMSSaxHandler::fatalError(const SAXParseException& e)
{
	String sError("Fatal error : %S at line %d column %d", String(StrX(e.getMessage()).scVal()), e.getLineNumber(), e.getColumnNumber());
	throw ErrorObject(sError);

}

void  XERCES_CPP_NAMESPACE::WMSSaxHandler::warning(const SAXParseException& e)
{
}

//-------------------------
WMSLayerInfo::~WMSLayerInfo() {
	for(vector<WMSLayerInfo *>::iterator cur = layers.begin(); cur != layers.end(); ++cur) {
		WMSLayerInfo *info = *cur;
		delete info;
	}
}

void WMSLayerInfo::PartialCopy(WMSLayerInfo *inf) {
	name = inf->name;
	for(map<String, vector<double> >::iterator cur3 = inf->srs.begin(); cur3 != inf->srs.end(); ++cur3) {
		srs[(*cur3).first] = (*cur3).second;
	}
}




