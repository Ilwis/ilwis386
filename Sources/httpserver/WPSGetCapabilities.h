#pragma once

namespace ILWIS{

	class WPSGetCapabilities  : public WPSHandler {
	public:
		WPSGetCapabilities(struct mg_connection *c, const struct mg_request_info *request_info, const map<String, String>& kvps);

		void writeResponse(IlwisServer*server=0) const;
		bool needsResponse() const;
		bool doCommand() ;
	};
}