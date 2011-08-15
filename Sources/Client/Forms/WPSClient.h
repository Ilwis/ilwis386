#pragma once

class FieldStringList;
class FieldListView;

struct ParameterInfo{
	ParameterInfo(const String& _id, const String& n, const String& s, const String& ex) { id = _id; name = n; type = s; ext=ex; }
	bool filetype();
	bool stringtype();
	bool numerictype();
	String name;
	String type;
	String value;
	String ext;
	String id;
};

class WPSClient  : public FormWithDest {
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
	FieldString *fsOut;
	String urlString;
	String operation;
	String description;
	String describeProcessURL;
	String executeProcessURL;
	vector<String> content;
	int operationIndex;
	int currentParmIndex;
	String stringField;
	double number;
	String outputName;
	vector<ParameterInfo> parameterValues;
	FormEntry *activeParameterField;

	int fetchDescribeProcess(Event *ev);
	int fetchGetCapabilities(Event *ev);
	int parameterSelection(Event *ev);
	void fillListView();
	int parmChange(Event *ev);
	int execute(Event *ev) ;
	int stringChange(Event *ev);

};
