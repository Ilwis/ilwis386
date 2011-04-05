#pragma once

namespace ILWIS{

	struct WPSParameter{
		String id;
		String value;
		bool isReference;
	};

	class WPSExecute  : public WPSHandler {
	public:
		WPSExecute(struct mg_connection *c, const struct mg_request_info *request_info, const map<String, String>& kvps);

		void writeResponse(IlwisServer*server=0) const;
		bool needsResponse() const;
		void doCommand();
	private:
		String execute(vector<WPSParameter>& outputs) const;
		vector<WPSParameter> inputs;
		String executionDir;
		String outputName;
		FileName fnZip;

		void downloadReferencedData();
		void createExecutionEnviroment();
		void executeOperation();
		String resultType(const String& operation);
		static int folder_count;
	};
}