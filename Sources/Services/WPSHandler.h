#pragma once

namespace ILWIS {
	class XMLDocument;

	class WPSHandler : public OWSHandler {
	protected:
		WPSHandler(const String& name,struct mg_connection *c, const struct mg_request_info *ri, const map<String, String>& kvps, IlwisServer *serv);
		void createHeader(ILWIS::XMLDocument& doc, const String& xsd) const;
	};
}