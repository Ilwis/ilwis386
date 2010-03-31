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
class WMSGetCapabilities;



class WMSLayerInfo {
public:
	WMSLayerInfo(String n) { name = n; prevLayer = NULL;}
	WMSLayerInfo() { name= ""; prevLayer = NULL; }
	void PartialCopy(WMSLayerInfo *inf);
	~WMSLayerInfo();
	String title;
	String compatibleName;
	String name;
	map<String, vector<double>> srs;
	WMSLayerInfo *prevLayer;
	vector<WMSLayerInfo *> layers;
	vector<double> bbLatLon;

};

XERCES_CPP_NAMESPACE_BEGIN

class WMSSaxHandler : public HandlerBase {

private:
	void handleLayerEnd(String &tag);
	//void handleSRS(String &tag);
	vector<double> handleBBBounds(AttributeList&  attributes);
	//void propagateSRS(vector<WMSLayerInfo *>& curlayers);
	void handleBoundingBox(map<String, vector<double> >& srs, AttributeList&  attributes);

	int prev, cur;
	WMSLayerInfo *currentLayer, *root;
	String currentTag;
	stack<WMSLayerInfo *> lstack;
	WMSGetCapabilities *capabilities;
	bool inStyle;

public:
	WMSSaxHandler(WMSGetCapabilities *cap);
	~WMSSaxHandler();
    void startElement(const XMLCh* const name, AttributeList& attributes);
	void endElement(const XMLCh* const name);
    void characters(const XMLCh* const chars, const unsigned int length);
    void ignorableWhitespace(const XMLCh* const chars, const unsigned int length);
	void error(const SAXParseException& e);
	void fatalError(const SAXParseException& e);
	void warning(const SAXParseException& e);
	vector<WMSLayerInfo *> getLayers();
	//void getLayers(vector<WMSLayerInfo *>& newlayers, vector<WMSLayerInfo *>& oldlayers);
    void resetDocument();
};

XERCES_CPP_NAMESPACE_END

class WMSGetCapabilities : public RemoteXMLObject {
	friend class WMSSaxHandler;
	vector<WMSLayerInfo *> layers;
	void parse();
	void propagateSRSInfo(WMSLayerInfo *prevlyr);
public:
	WMSGetCapabilities(const URL& url);
	vector<WMSLayerInfo *> getLayerInfo();


};
