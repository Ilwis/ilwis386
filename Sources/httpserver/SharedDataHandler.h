#pragma once

namespace ILWIS{

	class SharedDataHandler  : public RequestHandler {
	public:
		SharedDataHandler(struct mg_connection *c, const struct mg_request_info *request_info, const map<String, String>& kvps, IlwisServer *serv);
		void writeResponse() const;
		bool needsResponse() const;
	private:
		bool isOutput;
		String service;

	};
}