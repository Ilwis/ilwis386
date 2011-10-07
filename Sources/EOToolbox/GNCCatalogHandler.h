#pragma once

struct GNCFileInfo {
	ILWIS::Time imageTime;
	FileName fn;
	unsigned long size;
};

namespace ILWIS{

	class GNCCatalogHandler  : public RequestHandler {
	public:
		static RequestHandler *createHandler(struct mg_connection *c, const struct mg_request_info *request_info, const map<String, String>& kvps, IlwisServer *serv);

		GNCCatalogHandler(struct mg_connection *c, const struct mg_request_info *request_info, const map<String, String>& kvps, IlwisServer *serv);
		void writeResponse() const;
		bool needsResponse() const;
	private:
		bool doCommand();
		void collectCatalog(const String& folder, const String& format, vector<GNCFileInfo>& results);
		void addFolder(const String& folder, const String& format, const String& nameFormat, vector<GNCFileInfo>& results);
		void addFile(const FileName& fn, const String& nameFrmt, const String& timeFormat, vector<GNCFileInfo>& results);
		String handlePathStructure(const String& product, const String& folderIn);
		String makeRegEx(const String& pat);
		map<String, String> timeFormats;
		vector<GNCFileInfo> gncInfo; 

	};
}