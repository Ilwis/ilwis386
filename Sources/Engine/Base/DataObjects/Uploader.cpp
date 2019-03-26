#include "Headers\toolspch.h"
#include <sys/stat.h>
#include <iostream>
#include <sstream>
#include "Engine\Base\DataObjects\URL.h"
#include "Engine\Base\DataObjects\Uploader.h"
#include "Engine\Base\System\Engine.h"
#include "Engine\DataExchange\curlincludes\CurlProxy.h"

size_t read_callback(void *ptr, size_t size, size_t nmemb, void *userp);

Uploader::Uploader(const URL& _url) :
url(_url),
sizeleft(0),
file(0),
dataSize(0),
fValid(true)
{
}

Uploader::~Uploader() {
	if ( file)
		fclose(file);
}

void Uploader::setText(const String& txt) {
	text = txt;
	if (file)
		fclose(file);
	sizeleft = txt.size();
}

void Uploader::setFile(const FileName& fn) {
	text = "";
	if (file)
		fclose(file);
	file = 0;
	struct _stat buf;
	errno_t err = _stat( fn.sFullPath().c_str(), &buf );
	sizeleft += dataSize = buf.st_size;
	fnFile = fn;
}
void Uploader::upload() {

	if(!fValid)
		return;

	CURL *curl;
	CURLcode res;

    getEngine()->curl->curl_global_init(CURL_GLOBAL_ALL);
	curl = getEngine()->curl->curl_easy_init();
	if(curl) {
		struct curl_slist *headers=NULL;
		headers = getEngine()->curl->curl_slist_append(headers, "Content-Type: application/binary");

		getEngine()->curl->curl_easy_setopt(curl, CURLOPT_URL, url.sVal().c_str());
		getEngine()->curl->curl_easy_setopt(curl, CURLOPT_POST, TRUE);
		getEngine()->curl->curl_easy_setopt(curl, CURLOPT_READFUNCTION, read_callback);
		getEngine()->curl->curl_easy_setopt(curl, CURLOPT_READDATA, this);
		getEngine()->curl->curl_easy_setopt(curl, CURLOPT_VERBOSE, 1);
		getEngine()->curl->curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, sizeleft);
		getEngine()->curl->curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);


		res = getEngine()->curl->curl_easy_perform(curl);

		getEngine()->curl->curl_slist_free_all(headers);

		getEngine()->curl->curl_easy_cleanup(curl);
	}
}


size_t Uploader::getNextBytes(void* bytes, size_t size) {
	if ( text.size() != 0) {
		int i = 0;
		int actualSize = size > sizeleft ? sizeleft : size;
		int offset = text.size() - sizeleft;
		for(; i < actualSize; ++i) {
			((char *)bytes)[i] = text[ offset + i];
		}
		sizeleft -= actualSize;
		sizeleft = max(0, sizeleft);
		return i;
	} else {
		if ( !file )
			file = fopen(fnFile.sFullPath().c_str(), "rb");
		int actualSize = size > sizeleft ? sizeleft : size;
		size_t ret = fread(bytes, 1,actualSize,file);
		sizeleft-= actualSize;
		return ret;
	}
}


size_t read_callback(void *ptr, size_t size, size_t nmemb, void *userp)
{
	Uploader *uploader = (Uploader *)userp;

	if(size*nmemb < 1)
		return 0;

	if(uploader->getSizeLeft()) {
		return uploader->getNextBytes(ptr, nmemb); /* copy one single byte */
	}

	return -1;                        /* no more data left to deliver */
}

int Uploader::getSizeLeft() {
	return sizeleft;
}


