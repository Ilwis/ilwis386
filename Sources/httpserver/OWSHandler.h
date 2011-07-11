#pragma once

namespace ILWIS {
	class OWSHandler : public RequestHandler {
	protected:
		OWSHandler(struct mg_connection *c, const struct mg_request_info *ri, const map<String, String>& kvps);
		void writeError(const String& err, const String& code="none") const;
	};
}