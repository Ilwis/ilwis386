#include "Client\Headers\formelementspch.h"
#include "Client\FormElements\fldlist.h"
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
#include "Client\ilwis.h"

bool ParameterInfo::filetype() {
	return  type == "rastermap" || type == "polygonmap" || 
			type == "pointmap" || type == "segmentmap" ||
			type == "georeference" || type == "maplist" ||
			type == "table" || type == "domain";
}

bool ParameterInfo::stringtype() {
	return type == "column";
}

bool ParameterInfo::numerictype() {
	return type == "float" || type == "integer";
}

//----------------------------
WPSClient::WPSClient(const String& url) : 
	FormWithDest(IlwWinApp()->GetMainWnd(), TR("WPS Client")),
	urlString(url),
	operationIndex(-1),
	currentParmIndex(iUNDEF),
	number(0),
	activeParameterField(0)
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
	fldOperations->SetCallBack((NotifyProc) &WPSClient::fetchDescribeProcess);
	fldOperations->SetHeight(280);
	fg3->Align(fg2, AL_AFTER);
	DWORD style = WS_GROUP | WS_TABSTOP | ES_MULTILINE | ES_WANTRETURN|
					ES_AUTOVSCROLL | WS_VSCROLL | WS_BORDER | ES_READONLY; 
	fldDescr = new FieldStringMulti(fg3,&description,style);
	fldDescr->SetWidth(150);
	fldDescr->SetHeight(35);
	vector<FLVColumnInfo> v;
	new StaticText(fg3, TR("Parameters"));
	v.push_back(FLVColumnInfo(TR("Name"), 140));
	v.push_back(FLVColumnInfo(TR("Value"),180));
	fldParameters = new FieldListView(fg3,v);
	fldParameters->SetCallBack((NotifyProc)&WPSClient::parameterSelection);
	fldFileParam = new FieldDataType(fg3,TR("DummyXXXXXXXXXXXXX"),&stringField,new ObjectExtensionLister(0,".mpr"),true);
	fldFileParam->SetIndependentPos();
	fldFileParam->SetCallBack((NotifyProc)&WPSClient::parmChange);
	fldNumericParam = new FieldReal(fg3, TR("DummyXXXXXXXXXXXXXX"), &number); 
	fldNumericParam->Align(fldParameters, AL_UNDER);
	fldNumericParam->SetIndependentPos();
	fldStringParam = new FieldString(fg3, TR("DummyXXXXXXXXXXXXXX"), &stringField); 
	fldStringParam->Align(fldParameters, AL_UNDER);
	fldStringParam->SetWidth(90);
	fldStringParam->SetIndependentPos();
	fldStringParam->SetCallBack((NotifyProc)&WPSClient::stringChange);
	FieldGroup *fg4 = new FieldGroup(fg3);
	new FieldBlank(fg4);
	PushButton *pb = new PushButton(fg4,TR("Execute"),(NotifyProc)&WPSClient::execute);
	fsOut = new FieldString(fg4, TR("Output name"),&outputName);
	fsOut->SetWidth(90);
	fsOut->Align(pb, AL_AFTER);
	fg4->SetIndependentPos();


	fg3->Align(fg2,AL_AFTER);
	create();
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
	GetPrivateProfileString("WPS:ServiceContext","ShareServer","",usBuf,300,String("%Sconfig.ini", getEngine()->getContext()->sIlwDir()).c_str());
	String server(usBuf);
	String url = executeProcessURL + String("service=WPS&Request=Execute&version=1.0.0&Identifier=") + operation;
	for(int i=0; i < parameterValues.size(); ++i) {
		if ( i == 0)
			url += "&DataInputs=";
		else
			url += ";";

		ParameterInfo pi = parameterValues[i];
		if ( pi.filetype()) {
			ILWIS::Zipper zipper(pi.value);
			FileName fnZip(pi.value, ".zip");
			zipper.zip(fnZip);
			url += String("%S=@xlink:href=%S/shared_data/%S", pi.id, server, fnZip.sFile + fnZip.sExt);
			
		} if ( pi.stringtype()) {
			url += String("%S=%S", pi.id, pi.value);
		}
	}
	url += String("&ResponseDocument=%S&StoreExecuteResponse=true", outputName);
	RemoteObject xmlObj(url);
	String response = xmlObj.toString();
	ILWIS::XMLDocument doc(response);
	vector<pugi::xml_node> results;
	doc.executeXPathExpression("//wps:ProcessOutputs/wps:Output/wps:Reference[@xlink:href]", results);
	if ( results.size() > 0) {
		String url = results[0].first_attribute().value();
		int index = url.find_last_of("/");
		String file = getEngine()->sGetCurDir() + url.substr(index + 1);
		Downloader dl(url);
		dl.download(getEngine()->sGetCurDir());

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
	vector<String> record(2);
	fldParameters->clear();
	if ( currentParmIndex != iUNDEF) {
		parameterValues[currentParmIndex].value = stringField;
	}
	String sCurDir = getEngine()->sGetCurDir();
	for(int i = 0; i < parameterValues.size(); ++i) {
		record[0] = parameterValues[i].name + parameterValues[i].ext;
		FileName fn(parameterValues[i].value);
		String path = fn.sPath();
		record[1] =  path == sCurDir ? fn.sFile : parameterValues[i].value;
		fldParameters->AddData(record);
	}
	fldParameters->update();
}

int WPSClient::parameterSelection(Event *ev) {
	vector<int> rowIndexes;
	stringField = "";
	number = 0;
	fldParameters->getSelectedRowNumbers(rowIndexes);
	if ( rowIndexes.size() == 1) {
		if ( activeParameterField) {
			activeParameterField->StoreData();
			activeParameterField->Hide();
		}
		if ( currentParmIndex != iUNDEF) {
			parameterValues[currentParmIndex].value = stringField;
		}
		currentParmIndex = rowIndexes[0];
		ParameterInfo pi = parameterValues[currentParmIndex];
		if ( pi.filetype()) {
			activeParameterField = fldFileParam;
		}
		else {
			if ( pi.numerictype()) {
				activeParameterField = fldNumericParam;
			} else
				activeParameterField = fldStringParam;
		}
		if ( activeParameterField) {
			activeParameterField->Show();
			activeParameterField->setLabel(pi.name);
		}
	}
	return 1;
}
int WPSClient::fetchDescribeProcess(Event *ev) {
	int index = fldOperations->iGetSingleSelection();
	if ( index >= 0  && operationIndex != index) {
		operation = content[index].sHead(".");
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
		vector<pugi::xml_node> resultNodes;
		parameterValues.clear();
		doc.executeXPathExpression("//wps:ProcessDescriptions//wps:ProcessDescription/wps:DataInputs/wps:Input", resultNodes);
		fldParameters->clear();
		for(int i = 0; i < resultNodes.size(); ++i) {
			vector<String> record(2);
			String type;
			String id;
			String name;
			String icon;
			for(pugi::xml_node child = resultNodes[i].first_child(); child; child = child.next_sibling()) {
				String childName(child.name());
				if ( childName == "ows:Identifier") {
					id = child.first_child().value();
				}
				if ( childName == "ows:Title")
					name = child.first_child().value();
				if ( childName == "ows:LiteralData") {
					pugi::xml_node litNode = child.first_child();
					pugi::xml_node grandChild = litNode.first_child();
					type = grandChild.value();
					if ( type == "rastermap")
						icon += ".mpr";
					if ( type == "segmentmap")
						icon += ".mps";
					if ( type == "polygonmap")
						icon += ".mpa";
					if ( type == "pointmap")
						icon += ".mpp";
					if ( type == "table")
						icon += ".tbt";
					if ( type == "column")
						icon += ".col";
					if ( type == "georeference")
						icon += ".grf";
					if ( type == "domain")
						icon += ".dom";
					if ( type == "integer")
						icon += ".Integer";
					if ( type == "float")
						icon += ".Float";
					if ( type == "boolean")
						icon += ".bool";
				}
			}
			record[0] = name + icon;
			if ( id != "" && type != "")
				parameterValues.push_back(ParameterInfo(id, name, type, icon));

			fldParameters->AddData(record);
		}
		operationIndex = index;

	}
	fldFileParam->Hide();
	fldNumericParam->Hide();
	fldStringParam->Hide();
	return 1;
}

int WPSClient::fetchGetCapabilities(Event *ev) {
	fldUrl->StoreData();
	RemoteObject xmlObj(urlString);
	MemoryStruct *mem = xmlObj.get();
	String txt;
	for(int i = 0; i < mem->size; ++i)
		txt += mem->memory[i];
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