#pragma once

namespace ILWIS{

	struct WPSParameter{
		String id;
		String value;
		bool isReference;
	};

	class WPSExecute  : public WPSHandler {
	public:
		static RequestHandler *createHandler(struct mg_connection *c, const struct mg_request_info *request_info, const map<String, String>& kvps, IlwisServer *serv);

		WPSExecute(struct mg_connection *c, const struct mg_request_info *request_info, const map<String, String>& kvps, IlwisServer *serv);

		void writeResponse() const;
		bool needsResponse() const;
		bool doCommand();
	private:
		bool isApplication(const String& sExpress) ;
		String makeApplicationExpression(const String& expr, const map<String, ILWIS::WPSParameter>& orderedInput, const String& outputname);
		String makeNonApplicationExpression(const String& expr ,const map<String, ILWIS::WPSParameter>& orderedInput);
		String execute(vector<WPSParameter>& outputs) const;
		void gatherFiles(ifstream& ifs, map<String, FileName>& files);
		void moveToLocal(const map<String, FileName>& files);
		void adaptPathsToLocal(const map<String, FileName>& files);
		void createNewOutputList(vector<WPSParameter>& outputs) const;
		String processDescribeString(int count, const String& inp) const;
		vector<WPSParameter> inputs;
		String executionDir;
		String outputName;
		FileName fnZip;

		void downloadReferencedData();
		void createExecutionEnviroment();
		void executeOperation();
		String resultType(const String& operation);
		int local_folder_count;
		static int folder_count;
	};
}