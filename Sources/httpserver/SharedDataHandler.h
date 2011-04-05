#pragma once

namespace ILWIS{

	class SharedDataHandler  : public RequestHandler {
	public:
		SharedDataHandler(struct mg_connection *c, const struct mg_request_info *request_info, const map<String, String>& kvps, const String& ctx);
		void writeResponse(IlwisServer*server=0) const;
		bool needsResponse() const;
	private:
		String context;
	};
}