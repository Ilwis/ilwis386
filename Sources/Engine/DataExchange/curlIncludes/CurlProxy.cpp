#include "Headers\toolspch.h"
#include "Engine\base\System\engine.h"
#include "Curlproxy.h"

void CurlProxy::loadMethods(const String& ilwDir) {
	CFileFind finder;
	String path = ilwDir + "\\libcurl*.dll";
	BOOL fFound = finder.FindFile(path.c_str());
	while(fFound) {
		fFound = finder.FindNextFile();
		if (!finder.IsDirectory())
		{
			FileName fnModule (finder.GetFilePath());
			HMODULE hm = LoadLibrary(fnModule.sFullPath().c_str());
			if ( hm != NULL ) {
				curl_easy_init = (curl_easy_initFunc)GetProcAddress(hm,"curl_easy_init");
				curl_easy_setopt = (curl_easy_setoptFunc)GetProcAddress(hm,"curl_easy_setopt");
				curl_easy_perform = (curl_easy_performFunc)GetProcAddress(hm,"curl_easy_perform");
				curl_easy_cleanup = (curl_easy_cleanupFunc)GetProcAddress(hm,"curl_easy_cleanup");
				curl_global_init = (curl_global_initFunc)GetProcAddress(hm,"curl_global_init");
				curl_slist_append = (curl_slist_appendFunc)GetProcAddress(hm,"curl_slist_append");
				curl_slist_free_all = (curl_slist_free_allFunc)GetProcAddress(hm,"curl_slist_free_all");
				curl_global_cleanup = (curl_global_cleanupFunc)GetProcAddress(hm,"curl_global_cleanup");
				curl_easy_strerror = (curl_easy_strerrorFunc)GetProcAddress(hm,"curl_easy_strerror");
			}
		}
	}
}
