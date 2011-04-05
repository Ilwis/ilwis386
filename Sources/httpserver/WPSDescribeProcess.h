#pragma once

namespace ILWIS{

	class WPSDescribeProcess  : public WPSHandler {
	public:
		WPSDescribeProcess(struct mg_connection *c, const struct mg_request_info *request_info, const map<String, String>& kvps);

		void writeResponse(IlwisServer *server=0) const;
		bool needsResponse() const;
	private:
		Array<String> processIDs;
	};
}