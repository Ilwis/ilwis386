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
#include "Engine\Base\DataObjects\RemoteXMLObject.h"
#include "Engine\DataExchange\curlIncludes\curl.h"
#include "Engine\DataExchange\curlIncludes\easy.h"

unsigned int RemoteObject::iRef = 0;

RemoteObject::RemoteObject()
: file(0)
{
	if (iRef == 0)
		curl_global_init(CURL_GLOBAL_ALL);
	++iRef;

	curl_handle = curl_easy_init();
}

RemoteObject::RemoteObject(const URL& url)
: file(0)
{
	if (iRef == 0)
		curl_global_init(CURL_GLOBAL_ALL);
	++iRef;

	curl_handle = curl_easy_init();

	getRequest(url.sVal());
}

size_t RemoteObject::WriteMemoryCallback(void *ptr, size_t size, size_t nmemb, void *obj)
{
	RemoteObject *ro = (RemoteObject *)obj;
	size_t realsize = size * nmemb;
	if ( ro->file == 0) {
		MemoryStruct *mem = &(ro->chunk);
		char *ss = (char *)ptr;
		mem->memory = (unsigned char *)myrealloc(mem->memory, mem->size + realsize + 1);
		if (mem->memory) {
			memcpy(&(mem->memory[mem->size]), ptr, realsize);
			mem->size += realsize;
			mem->memory[mem->size] = 0;
		}
	} else {
		ro->file->write((char *)ptr, realsize);

		//for(int i = 0 ; i < realsize; ++i) {
		//	*(ro->file) << ((char *)ptr)[i];
		//}
	}
	return realsize;
}

RemoteObject::~RemoteObject() {
    if(chunk.memory)
		free(chunk.memory);
	curl_easy_cleanup(curl_handle);
	--iRef;
	if (iRef == 0)
		curl_global_cleanup();
}

void *RemoteObject::myrealloc(void *ptr, size_t size)
{
	/* There might be a realloc() out there that doesn't like reallocing
	NULL pointers, so we take care of it here */
	if(ptr)
		return realloc(ptr, size);
	else
		return malloc(size);

}

MemoryStruct *RemoteObject::get() {
	MemoryStruct *mem = new MemoryStruct();
	mem->size = chunk.size;
	mem->memory = new unsigned char[mem->size];
	memcpy(mem->memory, chunk.memory, chunk.size);

	return mem;
}

void RemoteObject::getRequest(const String& url) {
    chunk.memory=NULL; /* we expect realloc(NULL, size) to work */
    chunk.size = 0;    /* no data at this point */
  
	curl_easy_setopt(curl_handle, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
    curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, (void *)this);
    curl_easy_setopt(curl_handle, CURLOPT_USERAGENT, "libcurl-agent/1.0");
    CURLcode result = curl_easy_perform(curl_handle);
	if ( result != 0)
		TRACE("Curl error: %s", curl_easy_strerror(result)); // most of the time it is CURLE_PARTIAL_FILE ... do we retry?
		//throw ErrorObject(curl_easy_strerror(result)); // can we remove this "throw"?
}

void RemoteObject::parse() {
}

void RemoteObject::setStream(ofstream *stream){
	if ( stream == 0) {
		if ( file && file->is_open()) {
			file->close();
		}
	} else {
		file = stream;
		if(chunk.memory)
			free(chunk.memory);
		chunk.memory = 0;
	}
}

String RemoteObject::toString() {
	String txt;
	if(chunk.memory) {
		for(int i = 0; i < chunk.size; ++i)
			txt += chunk.memory[i];
	}
	return txt;
}

