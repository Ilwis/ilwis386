#pragma once

#include "curl.h"

typedef CURL* (__cdecl *curl_easy_initFunc)(void);
typedef CURLcode (__cdecl *curl_easy_setoptFunc)(CURL *curl, CURLoption option, ...);
typedef CURLcode (__cdecl *curl_easy_performFunc)(CURL *curl);
typedef void (__cdecl *curl_easy_cleanupFunc)(CURL *curl);
typedef CURLcode (__cdecl *curl_global_initFunc)(long flags);
typedef struct curl_slist* (__cdecl *curl_slist_appendFunc)(struct curl_slist *, const char *);
typedef void (__cdecl *curl_slist_free_allFunc)(struct curl_slist *);
typedef void (__cdecl *curl_global_cleanupFunc)(void);
typedef const char* (__cdecl *curl_easy_strerrorFunc)(CURLcode);

class _export CurlProxy {
public:
	void loadMethods(const String& ilwDir);

	curl_easy_initFunc curl_easy_init;
	curl_easy_setoptFunc curl_easy_setopt;
	curl_easy_performFunc curl_easy_perform;
	curl_easy_cleanupFunc curl_easy_cleanup;
	curl_global_initFunc curl_global_init;
	curl_slist_appendFunc curl_slist_append;
	curl_slist_free_allFunc curl_slist_free_all;
	curl_global_cleanupFunc curl_global_cleanup;
	curl_easy_strerrorFunc curl_easy_strerror;
};