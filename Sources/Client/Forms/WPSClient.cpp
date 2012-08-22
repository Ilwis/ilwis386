#include "Client\Headers\formelementspch.h"
#include "Client\FormElements\fldlist.h"
#include "Client\FormElements\FldOneSelectTextOnly.h"
#include "Client\FormElements\FieldStringList.h"
#include "Engine\Base\DataObjects\URL.h"
#include "Engine\Base\DataObjects\RemoteXMLObject.h"
#include "Engine\Base\DataObjects\XMLDocument.h"
#include "Client\FormElements\FieldListView.h"
#include "client\formelements\objlist.h"
#include "Client\FormElements\RemoteLister.h"
#include "Client\Forms\WPSClient.h"
#include "Engine\Base\System\Engine.h"
#include "Engine\Base\File\Zipper.h"
#include "Engine\Base\DataObjects\Downloader.h"
#include "Client\FormElements\DatFileLister.h"
#include "Engine\Base\System\RegistrySettings.h"
#include "Client\ilwis.h"
#include "Headers\constant.h"

bool ParameterInfo::filetype() {
	bool result =  type == "rastermap" || type == "polygonmap" || 
			type == "pointmap" || type == "segmentmap" ||
			type == "georeference" || type == "maplist" ||
			type == "table" || type == "domain" || 
			type == "sampleset" || type == "matrix" || type == "image/ilwisraster";
	return result;
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
	fShow(true),
	remoteCatalog(false)

{
	//getEngine()->Execute("startserver");

	IlwisSettings settings("IlwisWPS");
	urlString = settings.sValue("GetCapabilities");
	urlCatalog = settings.sValue("RemoteCatalog");

	FieldGroup *fg0 = new FieldGroup(root);
	fldUrl = new FieldString(fg0,TR("WPS server"),&urlString);
	fldUrl->SetWidth(200);
	PushButton *fldSend = new PushButton(fg0,TR("Send"), (NotifyProc) &WPSClient::fetchGetCapabilities);
	fldSend->Align(fldUrl, AL_AFTER);
	fldSend->SetHeight(fldUrl->psn->iHeight);
	cbCatalog = new CheckBox(fg0, TR("Use remote Catalog"), &remoteCatalog);
	cbCatalog->Align(fldUrl, AL_UNDER);
	fldCatalog = new FieldString(cbCatalog,"",&urlCatalog);
	fldCatalog->SetWidth(200);
	fldCatalog->Align(cbCatalog, AL_AFTER);
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
	
	fldFileParam = new FieldDataType(fg3,TR("Local files"),&stringField,new ObjectExtensionLister(0,".mpr"),true);
	fldFileParam->SetIndependentPos();
	fldFileParam->SetCallBack((NotifyProc)&WPSClient::parmChange);

	fldRemoteParam = new FieldOneSelectString(fg3,TR("Remote files"),&stringField,remoteFiles,false);
	fldRemoteParam->Align(fldParmInfo, AL_UNDER);
	fldRemoteParam->SetIndependentPos();
	fldRemoteParam->SetCallBack((NotifyProc)&WPSClient::parmChange);
	
	fldNumericParam = new FieldReal(fg3, TR("Numeric value"), &number); 
	fldNumericParam->Align(fldParmInfo, AL_UNDER);
	fldNumericParam->SetIndependentPos();
	
	fldStringParam = new FieldString(fg3, TR("String value"), &stringField); 
	fldStringParam->Align(fldParmInfo, AL_UNDER);
	fldStringParam->SetWidth(90);
	fldStringParam->SetIndependentPos();
	fldStringParam->SetCallBack((NotifyProc)&WPSClient::stringChange);
	
	rg = new RadioGroup(fg3,TR("Boolean choice"),&boolField,true);
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
	fsOut->SetCallBack((NotifyProc)&WPSClient::stringChange2);
	fsOut->SetWidth(70);
	fsOut->Align(pb, AL_AFTER);
	cbShow = new CheckBox(fg4,TR("Show"),&fShow);
	cbShow->Align(fsOut, AL_AFTER);
	cbShow->SetBevelStyle(FormEntry::bsLOWERED);
	fg4->SetIndependentPos();
	fg4->Align(fldStringParam, AL_UNDER);

	fg3->Align(fg2,AL_AFTER);

	FieldGroup *fg5 = new FieldGroup(root);
	PushButton *pb1, *pb2, *pb3, *pb4, *pb5, *pb6;

	pb1 = new PushButton(fg5,TR("XML Last Capabilities"),(NotifyProc)&WPSClient::showXMLFormCap);
	pb2 =new PushButton(fg5,TR("XML Last Describe"),(NotifyProc)&WPSClient::showXMLFormDesc);
	pb3 =new PushButton(fg5,TR("XML Last Execute"),(NotifyProc)&WPSClient::showXMLFormExe);
	pb4 =new PushButton(fg5,TR("Last Capabilities Request"),(NotifyProc)&WPSClient::showXMLFormCapR);
	pb5 =new PushButton(fg5,TR("Last Describe Request"),(NotifyProc)&WPSClient::showXMLFormDescR);
	pb6 =new PushButton(fg5,TR("Last Execute request"),(NotifyProc)&WPSClient::showXMLFormExeR);
	int w = 65;
	pb1->SetWidth(w);
	pb2->SetWidth(w);
	pb3->SetWidth(w);
	pb4->SetWidth(w);
	pb5->SetWidth(w);
	pb6->SetWidth(w);

	fg5->Align(fg3, AL_AFTER,-100);
	create();
}

WPSClient::~WPSClient() {
	IlwisSettings settings("IlwisWPS");
	settings.SetValue("GetCapabilities",urlString);
	settings.SetValue("RemoteCatalog", urlCatalog);
}

int WPSClient::stringChange2(Event *ev) {
	if ( ev && ev->message() == WM_SETFOCUS) { 
		if ( activeParameterField) {
			parameterSelection(0);
		}
	}
	return 1;
}
int WPSClient::showXMLFormCap(Event *ev) {
	new XMLForm(this, xmlGetCapabilities);
	return 1;
}

int WPSClient::showXMLFormDesc(Event *ev) {
	new XMLForm(this, xmlDescribeProcess);
	return 1;
}

int WPSClient::showXMLFormExe(Event *ev) {
	new XMLForm(this, xmlExecute);
	return 1;
}

int WPSClient::showXMLFormCapR(Event *ev) {
	new XMLForm(this, txtCapabilitiesRequest);
	return 1;
}

int WPSClient::showXMLFormDescR(Event *ev) {
	new XMLForm(this, txtDescribeRequest);
	return 1;
}

int WPSClient::showXMLFormExeR(Event *ev) {
	new XMLForm(this, txtExecuteRequest);
	return 1;
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
		else {
			if (url[url.size() - 1] != ';') // else optional params would generate multiple ';'
				url += ";";
		}

		ParameterInfo pi = parameterValues[operationVariant][i];
		if ( pi.filetype()) {
			if ( remoteCatalog) {
			/*	long index = fldRemoteParam->iVal();
				if ( index == -1)
					throw ErrorObject(TR("No input file selected"));
				String file = remoteFiles[index];*/
				if ( pi.value == "" && pi.optional)
					continue;
				url += String("%S=%S", pi.id, pi.value.sTrimSpaces());

			} else {
				if ( pi.value == "" && pi.optional)
					continue;
				ILWIS::Zipper zipper(pi.value);
				FileName fnZip(pi.value, ".zip");
				zipper.zip(fnZip);
				url += String("%S=@xlink:href=%S/wps:shared_data/%S", pi.id, server, fnZip.sFile + fnZip.sExt);
			}
			
		} else {
			if ( pi.optional && ( pi.value.rVal() == rUNDEF || pi.value == sUNDEF))
				continue;
			url += String("%S=%S", pi.id, pi.value.sTrimSpaces());
		} 
	}
	if ( outputName != "")
		url += String("&ResponseDocument=%S&StoreExecuteResponse=true", outputName);
	txtExecuteRequest = url;
	RemoteObject xmlObj(url);
	xmlExecute = xmlObj.toString();
	ILWIS::XMLDocument doc(xmlExecute);
	vector<pugi::xml_node> results;
	doc.executeXPathExpression("//wps:ProcessOutputs/wps:Output/wps:Reference", results);
	if ( results.size() > 0) {
		String url = results[0].first_attribute().value() ;
		int index = url.find_last_of("/");
		String file = getEngine()->sGetCurDir() + url.substr(index + 1);
		if ( file.find("?") != string::npos)
			file = file.sHead("?");
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
		//MessageBox(String("%S%S",file, ext).c_str(), 0, MB_OK);
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
				if ( number != rUNDEF)
					parameterValues[operationVariant][currentParmIndex].value = String("%f",number);
				else
					parameterValues[operationVariant][currentParmIndex].value = sUNDEF;
			} else if ( parameterValues[operationVariant][currentParmIndex].type == "choice") {
				if ( choiceValue >= 0)
					parameterValues[operationVariant][currentParmIndex].value	= currentChoices[choiceValue];
			}else
				parameterValues[operationVariant][currentParmIndex].value = stringField;
		}
		stringField = "";

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
			if ( parameterValues[operationVariant][currentParmIndex].numerictype()){
				if ( number != rUNDEF)
					parameterValues[operationVariant][currentParmIndex].value = String("%f", number);
				else
					parameterValues[operationVariant][currentParmIndex].value = sUNDEF;
			}
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
			cbCatalog->StoreData();
			if ( !remoteCatalog) {
				fldFileParam->SetVal(stringField);
				fldFileParam->SetObjLister(new DatFileLister(pi.ext));
			}
			else {
				fldCatalog->StoreData();
				remoteFiles.clear();
				String url = urlCatalog + "?request=catalog&service=ilwis&version=1.0&filter=" + pi.type +"&catalog=";
				RemoteLister rm(url);
				rm.getFiles(remoteFiles);

				fldRemoteParam->resetContent(remoteFiles);
				activeParameterField = fldRemoteParam;
			}
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
		txtDescribeRequest = url;
		RemoteObject xmlObj(url);
		MemoryStruct *mem = xmlObj.get();
		if ( mem == 0)
			return 1;
		String txt;
		for(int i = 0; i < mem->size; ++i)
			txt += mem->memory[i];
		xmlDescribeProcess = txt;
		ILWIS::XMLDocument doc(xmlDescribeProcess);
		vector<String> results;
		doc.executeXPathExpression("//wps:ProcessDescriptions//ProcessDescription/ows:Abstract/child::text()", results);
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
	fldRemoteParam->Hide();
	rg->Hide();
	fillListView();
	return 1;
}

void WPSClient::parseParameters(const ILWIS::XMLDocument& doc) {
	vector<pugi::xml_node> resultNodes;

	doc.executeXPathExpression("//wps:ProcessDescriptions//ProcessDescription/DataInputs", resultNodes);
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
	if ( pi.name == "")
		return;
	ExpressionToken ep;
	ep.pi = pi;
	tokens.push_back(ep);
}

void WPSClient::parseParameter(const ILWIS::XMLDocument& doc, const pugi::xml_node& node, list<ExpressionToken>& tokens) {

	for(pugi::xml_node child = node.first_child(); child; child = child.next_sibling()) {
		String childName(child.name());
		if ( childName == "Input") {
			String minOcc = child.attribute("minOccurs").value();
			String maxOcc = child.attribute("maxOccurs").value();
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
		if ( childName == "LiteralData") {
			pugi::xml_node litNode = child.first_child();
			String nodeName = litNode.name();
			if ( nodeName == "ows:DataType") {
				pugi::xml_node grandChild = litNode.first_child();
				type = grandChild.value();
			} else if ( nodeName == "LiteralValueChoice") {
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
		if ( childName == "ComplexData") {
			pugi::xml_node compNode = child.first_child();
			String name = compNode.name();
			if (  name == "Default") {
				pugi::xml_node formatNode = compNode.first_child();
				name = formatNode.name();
				if ( name == "Format") {
					pugi::xml_node vnode = formatNode.first_child();
					name = vnode.child_value();
					type = String(name);
					icon += getTypeIcon(type);

				}
			}
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
	if ( type == "rastermap" || type == "image/ilwisraster")
		return ".mpr";
	if ( type == "segmentmap" || type == "image/ilwisline")
		return ".mps";
	if ( type == "polygonmap" || type == "image/ilwispolygon")
		return ".mpa";
	if ( type == "pointmap" || type == "image/ilwispoint")
		return ".mpp";
	if ( type == "table" || type == "binary/ilwistable")
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
	txtCapabilitiesRequest = urlString;
	RemoteObject xmlObj(urlString);
	MemoryStruct *mem = xmlObj.get();
	xmlGetCapabilities = xmlObj.toString();

	ILWIS::XMLDocument doc(xmlGetCapabilities);
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
//-----------------------
XMLForm::XMLForm(CWnd *par, const String& txt)  : FormWithDest(par,"XML",true){
	for(int i = 0; i < txt.size(); ++i) {
		char c = txt[i];
		if ( c != '\n')
			text += c;
		else
			text += "\r\n";
	}
	FieldStringMulti *fs = new FieldStringMulti(root,&text,true);
	fs->SetHeight(350);
	fs->SetWidth(400);

	create();

}