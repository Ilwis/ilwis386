#include "Headers\toolspch.h"
#include <sys/stat.h>
#include "Engine\Base\DataObjects\URL.h"
#include "Engine\DataExchange\curlIncludes\curl.h"
#include "Engine\DataExchange\curlIncludes\easy.h"
#include "Engine\Base\DataObjects\Uploader.h"

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

void Uploader::setFile(const FileName& fn) {
   if (file)
	   fclose(file);
   file = 0;
   struct _stat buf;
   errno_t err = _stat( fn.sFullPath().scVal(), &buf );
   sizeleft += dataSize = buf.st_size;
   fnFile = fn;
}
void Uploader::upload() {

	if(!fValid)
		return;

  CURL *curl;
  CURLcode res;

  curl = curl_easy_init();
  if(curl) {
	struct curl_slist *headers=NULL;
	headers = curl_slist_append(headers, "Content-Type: application/binary");

    curl_easy_setopt(curl, CURLOPT_URL, url.sVal().scVal());
    curl_easy_setopt(curl, CURLOPT_POST, TRUE);
    curl_easy_setopt(curl, CURLOPT_READFUNCTION, read_callback);
    curl_easy_setopt(curl, CURLOPT_READDATA, this);
    curl_easy_setopt(curl, CURLOPT_VERBOSE, 1);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, sizeleft);
	curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);


    res = curl_easy_perform(curl);

	curl_slist_free_all(headers);

    curl_easy_cleanup(curl);
  }
}

int Uploader::getSizeLeft() {
	return sizeleft;
}

size_t Uploader::getNextBytes(void* bytes, size_t size) {
	if ( !file )
	   file = fopen(fnFile.sFullPath().scVal(), "rb");
	int actualSize = size > sizeleft ? sizeleft : size;
	size_t ret = fread(bytes, 1,actualSize,file);
	char *c = (char *)bytes;
	sizeleft-= actualSize;
	return ret;
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