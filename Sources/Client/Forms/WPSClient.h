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

private:
	FieldString *fldUrl;
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
	RadioGroup *rg;
	CheckBox *cbShow;
	String urlString;
	String operation;
	String description;
	String describeProcessURL;
	String executeProcessURL;
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
	String parmDescr;
	vector<String> currentChoices;
	vector< vector<ParameterInfo> > parameterValues;
	FormEntry *activeParameterField;

	int fetchDescribeProcess(Event *ev);
	int fetchGetCapabilities(Event *ev);
	int parameterSelection(Event *ev);
	int setParmListView(Event *ev);
	void fillListView();
	int parmChange(Event *ev);
	int execute(Event *ev) ;
	int stringChange(Event *ev);
	String getTypeIcon(const String& type);
	void parseParameters(const ILWIS::XMLDocument& doc);
	void parseSimpleParameter(const ILWIS::XMLDocument& doc, const pugi::xml_node& node, list<ExpressionToken>& tokens);
	void parseParameter(const ILWIS::XMLDocument& doc, const pugi::xml_node& node, list<ExpressionToken>& tokens) ;
	ParameterInfo parseInputNode(const ILWIS::XMLDocument& doc, const pugi::xml_node& node) ;
	void makeParameterLists(list<ExpressionToken>& tokens, int index);
	int exec();
	void initVars();

};
