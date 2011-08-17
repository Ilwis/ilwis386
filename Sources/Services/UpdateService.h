#pragma once

struct IlwisFileInfo;

namespace ILWIS{


	class UpdateService  : public RequestHandler {
		enum ResponseType{rtSTATUS, rtGETUPDATE, rtNONE};
	public:
		static RequestHandler *createHandler(struct mg_connection *c, const struct mg_request_info *request_info, const map<String, String>& kvps, IlwisServer *serv);
		UpdateService(struct mg_connection *c, const struct mg_request_info *request_info, const map<String, String>& kvps, IlwisServer *serv);
		void writeResponse(IlwisServer*server=0) const;
		bool needsResponse() const;
		bool doCommand();
	private:
		void getDifferences(const map<String, IlwisFileInfo>& baseFiles, const map<String, IlwisFileInfo>& externalFiles, vector<IlwisFileInfo>& differences);
		void toMap(map<String, IlwisFileInfo>& baseFiles, const ILWIS::XMLDocument& doc);
		void createUpdateFile();
		void doQuery(const String& query);
		long knownConfiguration(const vector<IlwisFileInfo>& names);
		void createDescriptionFile(const FileName& fnOut, const map<String, IlwisFileInfo>& baseFiles, const vector<IlwisFileInfo>& differences);

		static map<String, unsigned long> updateRequests;
		String context;
		CCriticalSection  cs;
		String client_id;
		unsigned long check;
		ResponseType rtype;
		String query;
	};
}