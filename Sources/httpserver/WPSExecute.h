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
		bool doCommand();
	private:
		bool isApplication(const String& sExpress) ;
		String makeApplicationExpression(const String& expr, const map<String, ILWIS::WPSParameter>& orderedInput);
		String makeNonApplicationExpression(const String& expr ,const map<String, ILWIS::WPSParameter>& orderedInput);
		String execute(vector<WPSParameter>& outputs) const;
		void gatherFiles(ifstream& ifs, map<String, FileName>& files);
		void moveToLocal(const map<String, FileName>& files);
		void adaptPathsToLocal(const map<String, FileName>& files);
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