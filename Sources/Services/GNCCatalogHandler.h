#pragma once

namespace ILWIS{

	class GNCCatalogHandler  : public RequestHandler {
	public:
		static RequestHandler *createHandler(struct mg_connection *c, const struct mg_request_info *request_info, const map<String, String>& kvps, IlwisServer *serv);

		GNCCatalogHandler(struct mg_connection *c, const struct mg_request_info *request_info, const map<String, String>& kvps, IlwisServer *serv);
		void writeResponse(IlwisServer*server=0) const;
		bool needsResponse() const;
	private:
		String context;
	};
}