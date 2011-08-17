#pragma once

namespace ILWIS{

	class WMSGetCapabilities  : public RequestHandler {
	public:
		static RequestHandler *createHandler(struct mg_connection *c, const struct mg_request_info *request_info, const map<String, String>& kvps, IlwisServer *serv);

		WMSGetCapabilities(struct mg_connection *c, const struct mg_request_info *request_info, const map<String, String>& kvps, IlwisServer *serv);
		void handleFile(pugi::xml_node& layer,ILWIS::XMLDocument& doc, const FileName& fn) const;
		void handleFilteredCatalog(const String& location,const String& filter, bool recursive, list<FileName>& files) const;

		void writeResponse(IlwisServer*server=0) const;
		bool needsResponse() const;
	};
}