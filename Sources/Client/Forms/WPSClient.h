#pragma once

class FieldStringList;
class FieldListView;
class FieldOneSelectTextOnly;

struct ParameterInfo{
	ParameterInfo() { optional = false;}
	ParameterInfo(const String& _id, const String& n, const String& s, const String& ex) { id = _id; name = n; type = s; ext=ex; optional=false; }
	bool filetype();
	bool stringtype();
	bool numerictype();
	int toIndex();
	int toMajorIndex() { return id.sHead(".").iVal(); }
	String name;
	String type;
	String value;
	String ext;
	String id;
	String info;
	bool optional;
	vector<String> choices;
};

struct ExpressionToken {
	enum ExprTokeType{ettENUM, ettGROUP, ettSIMPLE};
	ExpressionToken()  { type = ettSIMPLE; }
	ParameterInfo pi;
	ExprTokeType type;
	list<ExpressionToken> variants;

};

class WPSClient  : public FormBaseDialog {
public:
	WPSClient(const String& url="");
	~WPSClient();

private:
	FieldString *fldUrl;
	FieldString *fldCatalog;
	FieldStringList *fldOperations;
	FieldListView *fldParameters;
	FieldStringMulti *fldDescr;
	FieldDataType *fldFileParam;
	FieldReal *fldNumericParam;
	FieldString *fldStringParam;
	FieldOneSelectTextOnly *fldListChoice;
	FieldString *fsOut;
	FieldOneSelectString *fldChoices;
	FieldOneSelectString *fldVariants;
	FieldStringMulti *fldParmInfo;
	FieldOneSelectString *fldRemoteParam;
	CheckBox *cbCatalog;
	RadioGroup *rg;
	CheckBox *cbShow;
	String urlString;
	String operation;
	String description;
	String describeProcessURL;
	String executeProcessURL;
	String urlCatalog;
	vector<String> content;
	int operationIndex;
	int currentParmIndex;
	String stringField;
	int boolField;
	double number;
	String outputName;
	long operationVariant;
	long choiceValue;
	bool fShow;
	bool remoteCatalog;
	long stringChoice;
	vector<String> remoteFiles;
	String parmDescr;
	vector<String> currentChoices;
	vector< vector<ParameterInfo> > parameterValues;
	FormEntry *activeParameterField;
	String xmlGetCapabilities, xmlDescribeProcess, xmlExecute;
	String txtCapabilitiesRequest, txtDescribeRequest, txtExecuteRequest;

	int fetchDescribeProcess(Event *ev);
	int fetchGetCapabilities(Event *ev);
	int parameterSelection(Event *ev);
	int setParmListView(Event *ev);
	void fillListView();
	int parmChange(Event *ev);
	int execute(Event *ev) ;
	int stringChange(Event *ev);
	int stringChange2(Event *ev);
	int showXMLFormCap(Event *ev);
	int showXMLFormDesc(Event *ev);
	int showXMLFormExe(Event *ev);
	int showXMLFormCapR(Event *ev);
	int showXMLFormExeR(Event *ev);
	int showXMLFormDescR(Event *ev);
	String getTypeIcon(const String& type);
	void parseParameters(const ILWIS::XMLDocument& doc);
	void parseSimpleParameter(const ILWIS::XMLDocument& doc, const pugi::xml_node& node, list<ExpressionToken>& tokens);
	void parseParameter(const ILWIS::XMLDocument& doc, const pugi::xml_node& node, list<ExpressionToken>& tokens) ;
	ParameterInfo parseInputNode(const ILWIS::XMLDocument& doc, const pugi::xml_node& node) ;
	void makeParameterLists(list<ExpressionToken>& tokens, int index);
	int exec();
	void initVars();

};

class XMLForm : public FormWithDest {
public:
	XMLForm(CWnd *par, const String& txt);
	String text;
};
