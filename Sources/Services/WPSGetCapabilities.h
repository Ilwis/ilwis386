#pragma once

namespace ILWIS{

	class WPSGetCapabilities  : public WPSHandler {
	public:
		WPSGetCapabilities(struct mg_connection *c, const struct mg_request_info *request_info, const map<String, String>& kvps, IlwisServer *serv);

		static RequestHandler *createHandler(struct mg_connection *c, const struct mg_request_info *request_info, const map<String, String>& kvps, IlwisServer *serv);

		void writeResponse() const;
		bool needsResponse() const;
		bool doCommand() ;
	};
}