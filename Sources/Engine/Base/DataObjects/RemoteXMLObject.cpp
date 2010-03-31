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
#include "Engine\DataExchange\curlIncludes\curl.h"
#include "Engine\DataExchange\curlIncludes\easy.h"

#include "Engine\Base\DataObjects\URL.h"
#include "Engine\Base\DataObjects\RemoteXMLObject.h"

RemoteXMLObject::RemoteXMLObject() {
}

RemoteXMLObject::RemoteXMLObject(const URL& url): parser(NULL) {
	getRequest(url.sVal().sVal());
}

size_t RemoteXMLObject::WriteMemoryCallback(void *ptr, size_t size, size_t nmemb, void *data)
{
	size_t realsize = size * nmemb;
	MemoryStruct *mem = (MemoryStruct *)data;

	mem->memory = (unsigned char *)myrealloc(mem->memory, mem->size + realsize + 1);
	if (mem->memory) {
		memcpy(&(mem->memory[mem->size]), ptr, realsize);
		mem->size += realsize;
		mem->memory[mem->size] = 0;
	}
	return realsize;
}

RemoteXMLObject::~RemoteXMLObject() {
    if(chunk.memory)
      free(chunk.memory);
}

void *RemoteXMLObject::myrealloc(void *ptr, size_t size)
{
	/* There might be a realloc() out there that doesn't like reallocing
	NULL pointers, so we take care of it here */
	if(ptr)
		return realloc(ptr, size);
	else
		return malloc(size);

}

MemoryStruct *RemoteXMLObject::get() {
	MemoryStruct *mem = new MemoryStruct();
	mem->size = chunk.size;
	mem->memory = new unsigned char[mem->size];
	memcpy(mem->memory, chunk.memory, chunk.size);

	return mem;
}

void RemoteXMLObject::getRequest(char *url) {
	CURL *curl_handle;
  

    chunk.memory=NULL; /* we expect realloc(NULL, size) to work */
    chunk.size = 0;    /* no data at this point */
  
    curl_global_init(CURL_GLOBAL_ALL);
    curl_handle = curl_easy_init();
    curl_easy_setopt(curl_handle, CURLOPT_URL, url);
    curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
    curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, (void *)&chunk);
    curl_easy_setopt(curl_handle, CURLOPT_USERAGENT, "libcurl-agent/1.0");
    CURLcode result = curl_easy_perform(curl_handle);
	if ( result != 0)
		throw ErrorObject(curl_easy_strerror(result));
    curl_easy_cleanup(curl_handle);
  
  
    curl_global_cleanup();
}

void RemoteXMLObject::parse() {
	parser = new XERCES_CPP_NAMESPACE::SAXParser;
	parser->setValidationScheme(XERCES_CPP_NAMESPACE::SAXParser::Val_Auto);
	parser->setDoNamespaces(false);
	parser->setValidationSchemaFullChecking(false);

}

