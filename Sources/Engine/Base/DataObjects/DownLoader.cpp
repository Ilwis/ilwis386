#include "Headers\toolspch.h"
#include <sys/stat.h>
#include "Engine\Base\DataObjects\URL.h"
#include "Downloader.h"
#include "Engine\Base\System\Engine.h"
#include "Engine\DataExchange\curlincludes\CurlProxy.h"

size_t write_data(void *buffer, size_t size, size_t nmemb, void *userp) {
	char *d = (char*)buffer;
	Downloader *downloader = (Downloader *)userp;
	FILE *file = downloader->getFile();
	 
	int result = 0;
	if (file != NULL) {
		size_t n = fwrite(d,1,nmemb, file);
		if ( n != nmemb)
			throw ErrorObject("Couldnt write enough bytes");

		result = size * nmemb;
		//downloader->fUpdateTrq(result/1000); //in Kb
	}
	return result;
}

Downloader::Downloader(const URL &_url) : url(_url), count(0) {
}

FileName Downloader::download(const String& executionDir) {

	count = 0;
	CURL* ctx = getEngine()->curl->curl_easy_init() ;
	getEngine()->curl->curl_easy_setopt( ctx , CURLOPT_URL,  url.sVal().c_str() );
	int index = url.sVal().find_last_of("/");
	if ( index == string::npos) 
		return FileName();
	String name = url.sVal().substr(index+1);
	if ( name.find("?") != string::npos)
		name = name.sHead("?");

	trq.SetTitle("Downloading: " + name);
	trq.SetText("Receiving data");
	String targetPath("%S\\%S", executionDir, name);
	file = fopen(targetPath.c_str(), "wb");
	if ( file == NULL)
		return FileName();

	getEngine()->curl->curl_easy_setopt(ctx, CURLOPT_WRITEFUNCTION, write_data);
	getEngine()->curl->curl_easy_setopt( ctx , CURLOPT_WRITEDATA , (void *)this );
	//trq.Start();
	const CURLcode rc = getEngine()->curl->curl_easy_perform( ctx ) ;
	//trq.fUpdate(count);

	getEngine()->curl->curl_easy_cleanup( ctx ) ;
	fflush(file);
	fclose(file);

	//struct stat fileStat;
	//int err = stat( targetPath.c_str(), &fileStat );
	struct __stat64 fileStat; 
	int err = _stat64( targetPath.c_str(), &fileStat );
	FileName fn2(targetPath);

	if ( fileStat.st_size == 0)
		return FileName();
	


	return fn2;
	
}

bool Downloader::fUpdateTrq(long nr) {
	count += nr;
	return trq.fUpdate(count);
}
