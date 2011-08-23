#pragma once

namespace ILWIS{

	class GNCThreddsHandler  : public RequestHandler {
	public:
		static RequestHandler *createHandler(struct mg_connection *c, const struct mg_request_info *request_info, const map<String, String>& kvps, IlwisServer *serv);

		GNCThreddsHandler(struct mg_connection *c, const struct mg_request_info *request_info, const map<String, String>& kvps, IlwisServer *serv);
		void writeResponse() const;
		bool needsResponse() const;
	private:
		bool doCommand();
		void createExecutionEnviroment();
		String executionDir;
		static long threddsFolderCount;

	};
}