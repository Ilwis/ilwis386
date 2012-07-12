#pragma once

struct FileInfo {
	ILWIS::Time imageTime;
	String fn;
	unsigned long size;
};

namespace ILWIS{

	class IlwisCatalogHandler  : public RequestHandler {
	public:
		static RequestHandler *createHandler(struct mg_connection *c, const struct mg_request_info *request_info, const map<String, String>& kvps, IlwisServer *serv);

		IlwisCatalogHandler(struct mg_connection *c, const struct mg_request_info *request_info, const map<String, String>& kvps, IlwisServer *serv);
		void writeResponse() const;
		bool needsResponse() const;
	private:
		bool doCommand();
		void addFile(const FileName& fn);
		String handlePathStructure(const String& product, const String& folderIn);
		String makeExtensionList(const String& ext);
		vector<FileInfo> results; 

	};
}