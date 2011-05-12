#pragma once

namespace ILWIS{

	class WPSGetCapabilities  : public WPSHandler {
	public:
		WPSGetCapabilities(struct mg_connection *c, const struct mg_request_info *request_info, const map<String, String>& kvps);

		void writeResponse(IlwisServer*server=0) const;
		void writeResponse2(IlwisServer*server=0) const; //backup
		bool needsResponse() const;
		//void createHeader(XERCES_CPP_NAMESPACE::DOMDocument *doc, const String& xsd) const;
	};
}