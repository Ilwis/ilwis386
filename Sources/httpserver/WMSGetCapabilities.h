#pragma once

namespace ILWIS{

	class WMSGetCapabilities  : public RequestHandler {
	public:
		WMSGetCapabilities(struct mg_connection *c, const struct mg_request_info *request_info, const map<String, String>& kvps);
		void handleFile(XERCES_CPP_NAMESPACE::DOMElement *node, XERCES_CPP_NAMESPACE::DOMDocument *doc, const FileName& fn) const;
		void handleFilteredCatalog(const String& location,const String& filter, bool recursive, list<FileName>& files) const;

		void writeResponse(IlwisServer*server=0) const;
		bool needsResponse() const;
	};
}