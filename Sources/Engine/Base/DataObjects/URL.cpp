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
#include "Engine\Base\DataObjects\URL.h"

URL::URL() {
}

URL::URL(const String& url) {
	sUrl = url;
	Array<String> parts;
	String squery = getQuery();
	SplitOn(squery, parts, "&");
	for(size_t i =0; i < parts.iSize(); ++i) {
		String part = parts[i];
		String field = part.sHead("=").toLower();
		String value = part.sTail("=").toLower();
		query[field.toLower()] = value;

	}
}
String URL::sVal() const {
	return sUrl;
}

bool URL::isUrl(const String& s) {
	String protocol = s.sHead("://");
	return protocol == "http";
}

String URL::getQuery() const {
	return sUrl.sTail("?");
}
String URL::getPath() const {
	String tail = sUrl.sTail("://");
	String head = tail.sHead("?");
	if ( head == "")
		return "";
	//return head.sSub(2, head.length() - 2);
	return head;
}
String URL::getProtocol() const {
	return sUrl.sHead("://");
}

FileName URL::toFileName2(const String& serverRoot, const String& localRoot) const {
	if ( getProtocol() == "file") {
		String tail = sUrl.sTail("://");
		if ( tail.substr(0,9) == "localhost") {
			tail = tail.sTail("localhost/");
		}
		if ( tail[0] == '/') { // rooted url
			String drive = tail[1];
			String rest = tail.substr(3,tail.size() - 2);
			String name("%S:%S", drive, rest);
			FileName fn(name);
		} else { // server based filename,
			String relPath = tail.substr(serverRoot.size() - 7,tail.size() - (serverRoot.size() - 7));

			return FileName(localRoot + relPath);
			
		}
	}
	return FileName();
}

FileName URL::toFileName(bool root) const {
	String tail = sUrl.sTail("://");
	String head = tail.sHead("?");
	if ( root)
		head = head.sSub(2,head.length() - 2).sHead("/");
	String result = "";
	for(string::const_iterator p=head.begin() ; p != head.end(); ++p) {
		if ( *p == '/' || *p == ':' || *p == '.' || *p == '\\' )
			result += "_";
		else
			result += *p;
	}
	if ( result.sHead("_").iVal() != iUNDEF)
		result = "IP_"+ result;
	return FileName(result);
}

String URL::getQueryValue(const String& key) const{
	map<String, String>::const_iterator p = query.find(String(key).toLower());
	if ( p == query.end())
		return "";

	return (*p).second;
}

void URL::setQueryValue(const String& key, const String& value) {
	query[key] = String(value).toLower();
	String head = sUrl.sHead("?");
	String tail;
	for(map<String,String>::iterator cur=query.begin(); cur !=  query.end(); ++cur) {
		if ( tail != "")
			tail += "&";
		tail += (*cur).first + "=" + (*cur).second;
	}
	if ( tail != "")
		sUrl = head + "?"+ tail;
}


int URL::getPort() const {
	String path = getPath();
	String portPart = path.sTail(":");
	if ( portPart == "")
		return iUNDEF;
	int index = portPart.find_first_of("/");
	if (index == string::npos) 
		return iUNDEF;
	String port = portPart.substr(0,index);
	return port.iVal();
}

String URL::getHost() const{
	String path = getPath();
	int portPart = path.find(":");
	int slashPart = path.find("/");

	if ( portPart == string::npos || portPart > slashPart) {
		return path.substr(0,slashPart);
	}
	else {
		return path.substr(0,portPart);
	}
}

String URL::endSegment() const{
	String path = getPath();
	int index = path.find_last_of("\\");
	if ( index != -1) {
		return path.substr(index + 1,path.size() - index - 1);
	}
	return path;
}
