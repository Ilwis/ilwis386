#include "Client\Headers\formelementspch.h"
#include "Client\FormElements\fldlist.h"
#include "Client\FormElements\FldOneSelectTextOnly.h"
#include "Client\FormElements\FieldStringList.h"
#include "Engine\Base\DataObjects\URL.h"
#include "Engine\Base\DataObjects\RemoteXMLObject.h"
#include "Engine\Base\DataObjects\XMLDocument.h"
#include "Client\FormElements\FieldListView.h"
#include "client\formelements\objlist.h"
#include "Client\Forms\WPSClient.h"
#include "Engine\Base\System\Engine.h"
#include "Engine\Base\File\Zipper.h"
#include "Engine\Base\DataObjects\Downloader.h"
#include "Client\FormElements\DatFileLister.h"
#include "Client\ilwis.h"
#include "Headers\constant.h"

bool ParameterInfo::filetype() {
	return  type == "rastermap" || type == "polygonmap" || 
			type == "pointmap" || type == "segmentmap" ||
			type == "georeference" || type == "maplist" ||
			type == "table" || type == "domain" || 
			type == "sampleset" || type == "matrix";
}

bool ParameterInfo::stringtype() {
	return type == "column" || type == "string" || type == "dateTime";
}

bool ParameterInfo::numerictype() {
	return type == "float" || type == "integer";
}

int ParameterInfo::toIndex() {
	Array<String> parts;
	Split(id, parts, ".");
	int index = 0;
	for(int i = 0; i < parts.size(); ++i)
		index += parts[i].iVal();
	return index;
}

//----------------------------
WPSClient::WPSClient(const String& url) : 
	FormBaseDialog(IlwWinApp()->GetMainWnd(), TR("WPS Client"), true, false, true),
	//FormWithDest(IlwWinApp()->GetMainWnd(),TR("WPS Client")),
	urlString(url),
	operationIndex(-1),
	currentParmIndex(iUNDEF),
	number(0),
	activeParameterField(0),
	boolField(0),
	choiceValue(-1),
	operationVariant(iUNDEF),
	fShow(true)
{
	getEngine()->Execute("startserver");

	FieldGroup *fg0 = new FieldGroup(root);
	fldUrl = new FieldString(fg0,TR("WPS server"),&urlString);
	fldUrl->SetWidth(200);
	PushButton *fldSend = new PushButton(fg0,TR("Send"), (NotifyProc) &WPSClient::fetchGetCapabilities);
	fldSend->Align(fldUrl, AL_AFTER);
	fldSend->SetHeight(fldUrl->psn->iHeight);
	fg0->SetIndependentPos();
	FieldGroup *fg1 = new FieldGroup(root);
	FieldGroup *fg2 = new FieldGroup(fg1);
	fg1->Align(fg0, AL_UNDER);
	fldOperations = new FieldStringList(fg2,&operation,content);
	FieldGroup *fg3 = new FieldGroup(fg1);
	//fg3->SetBevelStyle(FormEntry::bsLOWERED);
	//fg3->SetWidth(150);

	fldOperations->SetCallBack((NotifyProc) &WPSClient::fetchDescribeProcess);
	fldOperations->SetHeight(320);
	fg3->Align(fg2, AL_AFTER);
	DWORD style = WS_GROUP | WS_TABSTOP | ES_MULTILINE | ES_WANTRETURN|
					ES_AUTOVSCROLL | WS_VSCROLL | WS_BORDER | ES_READONLY; 
	fldDescr = new FieldStringMulti(fg3,&description,style);
	fldDescr->SetWidth(160);
	fldDescr->SetHeight(35);
	vector<FLVColumnInfo> v;

	vector<String> dummy;
	fldVariants = new FieldOneSelectString(fg3,TR("Variants"),&operationVariant,dummy);
	fldVariants->SetIndependentPos();
	fldVariants->SetComboWidth(135);
	fldVariants->SetCallBack((NotifyProc)&WPSClient::setParmListView);

	new StaticText(fg3, TR("Parameters"));

	v.push_back(FLVColumnInfo(TR("Name"), 120));
	v.push_back(FLVColumnInfo(TR("Value"),100));
	v.push_back(FLVColumnInfo(TR("Optional"),40));

	fldParameters = new FieldListView(fg3,v);
	fldParameters->SetCallBack((NotifyProc)&WPSClient::parameterSelection);

	fldParmInfo = new FieldStringMulti(fg3,&parmDescr,style);
	fldParmInfo->SetWidth(160);
	fldParmInfo->SetHeight(20); 
	
	fldFileParam = new FieldDataType(fg3,TR("DummyXXXXXXXXXXXXX"),&stringField,new ObjectExtensionLister(0,".mpr"),true);
	fldFileParam->SetIndependentPos();
	fldFileParam->SetCallBack((NotifyProc)&WPSClient::parmChange);
	
	fldNumericParam = new FieldReal(fg3, TR("DummyXXXXXXXXXXXXXX"), &number); 
	fldNumericParam->Align(fldParmInfo, AL_UNDER);
	fldNumericParam->SetIndependentPos();
	
	fldStringParam = new FieldString(fg3, TR("DummyXXXXXXXXXXXXXX"), &stringField); 
	fldStringParam->Align(fldParmInfo, AL_UNDER);
	fldStringParam->SetWidth(90);
	fldStringParam->SetIndependentPos();
	fldStringParam->SetCallBack((NotifyProc)&WPSClient::stringChange);
	
	rg = new RadioGroup(fg3,TR("DummyXXXXXXXXXXXXXX"),&boolField,true);
	new RadioButton(rg,TR("True"));
	new RadioButton(rg,TR("False"));
	rg->Align(fldParmInfo, AL_UNDER);

	fldChoices = new FieldOneSelectString(fg3,TR("Choices"),&choiceValue,currentChoices);
	fldChoices->Align(fldParmInfo, AL_UNDER);
	fldChoices->SetIndependentPos();


	FieldGroup *fg4 = new FieldGroup(fg3);
	new FieldBlank(fg4);
	PushButton *pb = new PushButton(fg4,TR("Execute"),(NotifyProc)&WPSClient::execute);

	fsOut = new FieldString(fg4, TR("Output name"),&outputName);
	fsOut->SetWidth(70);
	fsOut->Align(pb, AL_AFTER);
	cbShow = new CheckBox(fg4,TR("Show"),&fShow);
	cbShow->Align(fsOut, AL_AFTER);
	cbShow->SetBevelStyle(FormEntry::bsLOWERED);
	fg4->SetIndependentPos();
	fg4->Align(fldStringParam, AL_UNDER);

	fg3->Align(fg2,AL_AFTER);
	create();
}

int WPSClient::setParmListView(Event *ev) {
	int res = fldVariants->iVal();
	if ( res != -1) {
		operationVariant = res;
		fillListView();
	}
	return 1;
}

int WPSClient::exec() {
	return 1;
}
int WPSClient::stringChange(Event *ev) {
	if ( ev && ev->message() == WM_KILLFOCUS) {
		fldStringParam->StoreData();
		if ( stringField!="")
			fillListView();
	}

	return 1;
}
int WPSClient::execute(Event *ev) {
	parameterSelection(0); // forces save of unsaved info;
	fsOut->StoreData();
	char usBuf[300];
	GetPrivateProfileString("WPS:ServiceContext","ShareServer","",usBuf,300,String("%S\\Services\\wpshandlers.ini", getEngine()->getContext()->sIlwDir()).c_str());
	String server(usBuf);
	if ( server == "")
		throw ErrorObject(TR("No share server defined in configuration file"));
	String url = executeProcessURL + String("service=WPS&Request=Execute&version=1.0.0&Identifier=") + operation;
	for(int i=0; i < parameterValues[operationVariant].size(); ++i) {
		if ( i == 0)
			url += "&DataInputs=";
		else
			url += ";";

		ParameterInfo pi = parameterValues[operationVariant][i];
		if ( pi.filetype()) {
			ILWIS::Zipper zipper(pi.value);
			FileName fnZip(pi.value, ".zip");
			zipper.zip(fnZip);
			url += String("%S=@xlink:href=%S/wps:shared_data/%S", pi.id, server, fnZip.sFile + fnZip.sExt);
			
		} else {
				url += String("%S=%S", pi.id, pi.value);
		} 
	}
	if ( outputName != "")
		url += String("&ResponseDocument=%S&StoreExecuteResponse=true", outputName);
	RemoteObject xmlObj(url);
	String response = xmlObj.toString();
	ILWIS::XMLDocument doc(response);
	vector<pugi::xml_node> results;
	doc.executeXPathExpression("//wps:ProcessOutputs/wps:Output/wps:Reference[@xlink:href]", results);
	if ( results.size() > 0) {
		String url = results[0].first_attribute().value() ;
		int index = url.find_last_of("/");
		String file = getEngine()->sGetCurDir() + url.substr(index + 1);
		Downloader dl(url);
		dl.download(getEngine()->sGetCurDir());
		ILWIS::Zipper zip;
		zip.unzip(file);
		getEngine()->PostMessage(ILW_READCATALOG, 0, 0);
		cbShow->StoreData();
		FileName fn(file);
		index = fn.sFile.find_last_of("_");
		String ext = fn.sFile.substr(index + 1,fn.sFile.size() - index - 1);
		file = fn.sFile.substr(0, index);
		getEngine()->Execute("open " + file + "." + ext);
	}
	
	return 1;
}

int WPSClient::parmChange(Event *ev) {
	if ( activeParameterField) {
		activeParameterField->StoreData();
		fillListView();
	}
	return 1;
}
void WPSClient::fillListView() {
	if ( operationVariant == iUNDEF)
		return;

	vector<String> record(3);
	fldParameters->clear();
	if ( currentParmIndex != iUNDEF && activeParameterField) {
		activeParameterField->StoreData();
		if ( currentParmIndex < parameterValues[operationVariant].size()) {
			ParameterInfo pi = parameterValues[operationVariant][currentParmIndex];
			if ( pi.numerictype() ) {
				parameterValues[operationVariant][currentParmIndex].value = String("%f",number);
			} else
				parameterValues[operationVariant][currentParmIndex].value = stringField;
		}
	}
	if ( parameterValues.size() == 0)
		return;

	String sCurDir = getEngine()->sGetCurDir();
	for(int i = 0; i < parameterValues[operationVariant].size(); ++i) {
		record[0] = parameterValues[operationVariant][i].name + parameterValues[operationVariant][i].ext;
		FileName fn(parameterValues[operationVariant][i].value);
		String path = fn.sPath();
		record[1] =  path == sCurDir ? fn.sFile : parameterValues[operationVariant][i].value;
		record[2] = parameterValues[operationVariant][i].optional ? "true" : "";
		fldParameters->AddData(record);
	}
	fldParameters->update();
}

int WPSClient::parameterSelection(Event *ev) {
	vector<int> rowIndexes;
	initVars();
	fldParameters->getSelectedRowNumbers(rowIndexes);
	if ( rowIndexes.size() == 1 && operationVariant >= 0) {
		if ( activeParameterField) {
			activeParameterField->StoreData();
			activeParameterField->Hide();
		}
		if ( currentParmIndex != iUNDEF) {
			if ( parameterValues[operationVariant][currentParmIndex].numerictype())
				parameterValues[operationVariant][currentParmIndex].value = String("%f", number);
			else if ( parameterValues[operationVariant][currentParmIndex].type == "boolean") {
				parameterValues[operationVariant][currentParmIndex].value	= boolField ? "true" : "false";
			} else if ( parameterValues[operationVariant][currentParmIndex].type == "choice") {
				if ( choiceValue >= 0)
					parameterValues[operationVariant][currentParmIndex].value	= currentChoices[choiceValue];
			}else
				parameterValues[operationVariant][currentParmIndex].value = stringField;
		}
		currentParmIndex = rowIndexes[0];
		ParameterInfo pi = parameterValues[operationVariant][currentParmIndex];
		if ( pi.filetype()) {
			activeParameterField = fldFileParam;
			stringField = pi.value;
			fldFileParam->SetVal(stringField);
			fldFileParam->SetObjLister(new DatFileLister(pi.ext));
		} else if ( pi.numerictype()) {
				activeParameterField = fldNumericParam;
				number = pi.value.rVal();
				fldNumericParam->SetVal(number);
		} else if ( pi.type == "boolean") {
				activeParameterField = rg;
				boolField = pi.value.fVal() ? 1 : 0;
				rg->SetVal(boolField);
		} else if ( pi.type == "choice") {
				activeParameterField = fldChoices;
				currentChoices.clear();
				for(int i=0; i < pi.choices.size(); ++i)
					currentChoices.push_back(pi.choices[i]);
				fldChoices->resetContent(currentChoices);
				fldChoices->SelectVal(pi.value);
				
		}else {
				activeParameterField = fldStringParam;
				stringField = pi.value;
				fldStringParam->SetVal(stringField);
		}
		fldParmInfo->SetVal(pi.info);
		if ( activeParameterField) {
			activeParameterField->Show();
			activeParameterField->setLabel(pi.name);
		}
	}
	fillListView();
	return 1;
}

void WPSClient::initVars() {
	stringField = "";
	number = 0;
	boolField = true;
	parmDescr = "";
}

int WPSClient::fetchDescribeProcess(Event *ev) {
	initVars();
	currentParmIndex = 0;
	int index = fldOperations->iGetSingleSelection();
	if ( index >= 0  && operationIndex != index) {
		operation = content[index].sHead(".");
		String icon = content[index].sTail(".");
		operation = operation.sTrimSpaces(true);
		String url = describeProcessURL + "Service=WPS&Request=DescribeProcess&Version=1.0.0&Identifier=" + operation;
		RemoteObject xmlObj(url);
		MemoryStruct *mem = xmlObj.get();
		String txt;
		for(int i = 0; i < mem->size; ++i)
			txt += mem->memory[i];
		ILWIS::XMLDocument doc(txt);
		vector<String> results;
		doc.executeXPathExpression("//wps:ProcessDescriptions//wps:ProcessDescription/ows:Abstract/child::text()", results);
		if ( results.size() > 0) {
			fldDescr->SetVal(results[0]);
		}
		//vector<pugi::xml_node> resultNodes;
		parameterValues.clear();
		parseParameters(doc);
		vector<String> names;
		for(int j = 0; j < parameterValues.size(); ++j) {
		String name = operation + "(";
			for(int i = 0; i < parameterValues[j].size(); ++i) {
				ParameterInfo pi = parameterValues[j][i];
				if ( i > 0)
					name += ",";
				name += pi.name;
			}
			name += ")." + icon ;
			names.push_back(name);
		}
		fldVariants->resetContent(names);
		fldVariants->SetVal(0);
		operationVariant = 0;
	


		operationIndex = index;

	}
	fldFileParam->Hide();
	fldNumericParam->Hide();
	fldStringParam->Hide();
	fldChoices->Hide();
	rg->Hide();
	fillListView();
	return 1;
}

void WPSClient::parseParameters(const ILWIS::XMLDocument& doc) {
	vector<pugi::xml_node> resultNodes;

	doc.executeXPathExpression("//wps:ProcessDescriptions//wps:ProcessDescription/wps:DataInputs", resultNodes);
	if ( resultNodes.size() == 0) // expression without input parameters
		return;
	list<ExpressionToken> tokens;
	parseParameter(doc, resultNodes[0], tokens);
	parameterValues.resize(1);
	makeParameterLists(tokens,0);

}


void WPSClient::makeParameterLists(list<ExpressionToken>& tokens, int index) {
	for(list<ExpressionToken>::iterator iter = tokens.begin(); iter != tokens.end(); ++iter) {
		if ( (*iter).type == ExpressionToken::ettSIMPLE) {
			parameterValues[index].push_back((*iter).pi);
		} else if ( (*iter).type == ExpressionToken::ettGROUP) {
			//parameterValues[index].push_back((*iter).pi);
			for(int i = 0; i < parameterValues.size(); ++i)
				makeParameterLists((*iter).variants,i);
		}
		else {
			int count = 0;
			int sz = parameterValues[index].size();
			for(list<ExpressionToken>::iterator iter2 = (*iter).variants.begin(); iter2 != (*iter).variants.end(); ++iter2,++count) {
				parameterValues.push_back(vector<ParameterInfo>());
				for(int i =0; i < sz; ++i) {
					parameterValues.back().push_back( parameterValues[index][i]);
				}
				makeParameterLists((*iter2).variants, index + count);
			}

		}
	}
}

void WPSClient::parseSimpleParameter(const ILWIS::XMLDocument& doc, const pugi::xml_node& node, list<ExpressionToken>& tokens) {
	ParameterInfo pi = parseInputNode(doc, node);
	ExpressionToken ep;
	ep.pi = pi;
	tokens.push_back(ep);
}

void WPSClient::parseParameter(const ILWIS::XMLDocument& doc, const pugi::xml_node& node, list<ExpressionToken>& tokens) {

	for(pugi::xml_node child = node.first_child(); child; child = child.next_sibling()) {
		String childName(child.name());
		if ( childName == "wps:Input") {
			String minOcc = child.attribute("minOcc").value();
			String maxOcc = child.attribute("maxOcc").value();
			parseSimpleParameter(doc, child, tokens);
			if ( minOcc == "0" && maxOcc == "1")
				tokens.back().pi.optional = true;
		}
		else if (childName == "ilwis:Parameter") {
			String attr = child.attribute("type").value();
			if ( attr == "enumerate") {
				ExpressionToken ep;
				ep.type = ExpressionToken::ettENUM;
				parseParameter(doc, child, ep.variants);
				tokens.push_back(ep);

			} else if (attr == "group") {
				list<ExpressionToken> groups;
				parseParameter(doc, child, groups);
				ExpressionToken ep;
				ep.type = ExpressionToken::ettGROUP;
				for(list<ExpressionToken>::iterator iter = groups.begin(); iter != groups.end(); ++iter) {
					ep.variants.push_back(*iter);
				}
				tokens.push_back(ep);
			}
		}
	}
}

ParameterInfo WPSClient::parseInputNode(const ILWIS::XMLDocument& doc, const pugi::xml_node& node) {
	vector<String> record(2);
	String type;
	String id;
	String name;
	String icon;
	String info;
	vector<String> names;

	for(pugi::xml_node child = node.first_child(); child; child = child.next_sibling()) {
		String childName(child.name());
		if ( childName == "ows:Identifier") {
			id = child.first_child().value();
		}
		if ( childName == "ows:Title")
			name = child.first_child().value();
		if ( childName == "ows:Abstract")
			info = child.first_child().value();
		if ( childName == "ows:LiteralData") {
			pugi::xml_node litNode = child.first_child();
			String nodeName = litNode.name();
			if ( nodeName == "ows:DataType") {
				pugi::xml_node grandChild = litNode.first_child();
				type = grandChild.value();
			} else if ( nodeName == "wps:LiteralValueChoice") {
				pugi::xml_node n0 = child.first_child();
				for(pugi::xml_node n1 = n0.first_child(); n1 ; n1 = n1.next_sibling()) {
					String n1Name = n1.name();
					if ( n1Name == "ows:AllowedValues") {
						for(pugi::xml_node n2 = n1.first_child(); n2 ; n2 = n2.next_sibling()) {
							String n2Name = n2.name();
							if ( n2Name == "ows:Value") {
								names.push_back(n2.first_child().value());
							}
						}
					}
				}
				//doc.executeXPathExpression("//wps:ProcessDescriptions/wps:ProcessDescription/wps:DataInputs/wps:Input/ows:LiteralData/wps:LiteralValueChoice/ows:AllowedValues/ows:Value/child::text()", names);
				type = "choice";
			}
			icon += getTypeIcon(type);
		}
	}
	if ( id != "" && type != "") {
		ParameterInfo pi(id, name, type, icon);
		pi.info = info;
		for(int j = 0; j < names.size(); ++j)
			pi.choices.push_back(names[j]);
		return pi;
	}
	return ParameterInfo();
}

String WPSClient::getTypeIcon(const String& type) {
	if ( type == "rastermap")
		return ".mpr";
	if ( type == "segmentmap")
		return ".mps";
	if ( type == "polygonmap")
		return ".mpa";
	if ( type == "pointmap")
		return ".mpp";
	if ( type == "table")
		return ".tbt";
	if ( type == "column")
		return ".col";
	if ( type == "georeference")
		return ".grf";
	if ( type == "domain")
		return ".dom";
	if ( type == "integer")
		return ".Integer";
	if ( type == "float")
		return ".Float";
	if ( type == "boolean")
		return ".bool";
	if ( type == "string")
		return ".atx";
	if ( type == "dateTime")
		return ".History";
	if ( type == "sampleset")
		return ".sms";
	if ( type == "maplist")
		return ".mpl";
	if ( type == "matrix")
		return ".mat";
	if ( type == "choice")
		return ".choice";
	return "";
}

int WPSClient::fetchGetCapabilities(Event *ev) {
	fldUrl->StoreData();
	RemoteObject xmlObj(urlString);
	MemoryStruct *mem = xmlObj.get();
	String txt = xmlObj.toString();

	ILWIS::XMLDocument doc(txt);
	doc.executeXPathExpression("//wps:ProcessOfferings/wps:Process/ows:Identifier/child::text()", content);
	for(int i=0; i < content.size(); ++i) {
		if ( content[i].substr(0,3) == "Map")
			content[i] += ".mpr";
		if ( content[i].substr(0,10) == "PolygonMap")
			content[i] += ".mpa";
		if ( content[i].substr(0,10) == "SegmentMap")
			content[i] += ".mps";
		if ( content[i].substr(0,8) == "PointMap")
			content[i] += ".mpp";
		if ( content[i].substr(0,5) == "Table")
			content[i] += ".tbt";
		if ( content[i].substr(0,7) == "MapList")
			content[i] += ".mpl";
	}
	fldOperations->resetContent(content);
	vector<String> results;
	//doc.executeXPathExpression("//ows:Operation[@name='DescribeProcess']/ows:DCP/ows:HTTP/ows:Get/@xlink:href",results);
	doc.executeXPathExpression("//ows:Operation[@name='DescribeProcess']/ows:DCP/ows:HTTP/ows:Get[@xlink:href]",results);
	if ( results.size() == 1) {
		String p = results[0].sTail("=");
		int index = p.find("?");
		p = p.substr(1, index);
		describeProcessURL = p;
	}
	doc.executeXPathExpression("//ows:Operation[@name='ExecuteProcess']/ows:DCP/ows:HTTP/ows:Get[@xlink:href]",results);
	if ( results.size() == 1) {
		String p = results[0].sTail("=");
		int index = p.find("?");
		p = p.substr(1, index);
		executeProcessURL = p;
	}
	return 1;
}