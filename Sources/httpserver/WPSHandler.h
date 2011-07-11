#pragma once

namespace ILWIS {
	class XMLDocument;

	class WPSHandler : public OWSHandler {
	protected:
		WPSHandler(struct mg_connection *c, const struct mg_request_info *ri, const map<String, String>& kvps);
		void createHeader(ILWIS::XMLDocument& doc, const String& xsd) const;
	};
}