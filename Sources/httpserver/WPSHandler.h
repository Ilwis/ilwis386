#pragma once

namespace ILWIS {
	class WPSHandler : public RequestHandler {
	protected:
		WPSHandler(struct mg_connection *c, const struct mg_request_info *ri, const map<String, String>& kvps);
		void createHeader(XERCES_CPP_NAMESPACE::DOMDocument *doc, const String& xsd) const;
	};
}