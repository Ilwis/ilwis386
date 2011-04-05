#include "headers/toolspch.h"
#include "Engine\Base\DataObjects\ilwisobj.h"
#include "Engine\Base\System\Engine.h"
#include "HttpServer\command.h"
#include "HttpServer\mongoose.h"
#include "httpserver\RequestHandler.h"
#include "httpserver\WPSHandler.h"
#include "httpserver\WPSExecute.h"
#include "Engine\Base\System\module.h"
#include "Engine\Applications\ModuleMap.h"
#include "httpserver\XMLDocument.h"
#include "Engine\Base\File\zlib.h"
#include "Engine\Base\File\unzip.h"
#include "Engine\Base\DataObjects\URL.h"
#include "HttpServer\Downloader.h"
#include <xercesc\dom\DOMLSSerializer.hpp>


using namespace ILWIS;
using namespace XERCES_CPP_NAMESPACE;

int WPSExecute::folder_count=0;

WPSExecute::WPSExecute(struct mg_connection *c, const struct mg_request_info *ri, const map<String, String>& _kvps)
: WPSHandler(c,ri,_kvps)
{
	String inParameters;
	if((inParameters = getValue("datainputs")) != sUNDEF) {
		Array<String> parms;
		Split(inParameters, parms,";");
		for(int i =0 ; i< parms.size(); ++i) {
			String parm = parms[i];
			WPSParameter inputParm;
			inputParm.id = parm.sHead("=");
			inputParm.value = parm.sTail("=");
			inputParm.isReference = inputParm.value.find("href") != string::npos ? true : false;
			inputs.push_back(inputParm);
		}
	}
}

void WPSExecute::doCommand() {
	try{
		createExecutionEnviroment();
		downloadReferencedData();
		executeOperation();
	} catch(ErrorObject err) {
		String errTxt = err.sWhat();
	}

}

void WPSExecute::createExecutionEnviroment() {
	String rootDir = getConfigValue("WPS:ExecutionContext:Root");
	executionDir = String("%S\\process_%d", rootDir, folder_count);
	_mkdir(executionDir.scVal());
	getEngine()->SetCurDir(executionDir);
}

void WPSExecute::downloadReferencedData() {
	for(int i=0; i < inputs.size(); ++i) {
		if ( inputs[i].isReference) {
			URL url(inputs[i].value.sTail("="));
			Downloader loader(url);
			loader.download(executionDir);
			int index = url.sVal().find_last_of("/");
			String name="";
			if ( index != string::npos) {
				name =  url.sVal().substr(index+1);
			}
			name = executionDir + "\\" + name;
			unzip(FileName(name));
		}
	}
}

void WPSExecute::executeOperation() {

	String expression = getValue("identifier");
	String operation = expression;
	operation.toLower();

	map<String, ILWIS::WPSParameter> orderedInput;
	for(int i=0; i < inputs.size(); ++i) {
		orderedInput[inputs[i].id] = inputs[i];

	}
	expression += "(";
	int i = 0;
	for(map<String, ILWIS::WPSParameter>::const_iterator cur=orderedInput.begin(); cur != orderedInput.end();++cur,++i) {
		WPSParameter par = (*cur).second;
		if (i > 0)
			expression += ",";
		if ( par.isReference) {
			String url = par.value.sTail("=");
			int index = url.find_last_of("/");
			String name="";
			if ( index != string::npos) {
				name =  url.substr(index+1);
			}
			name = executionDir + "\\" + name;
			index = name.find_last_of("_");
			if ( index != string::npos) {
				String ext = name.substr(index+1);
				name = name.substr(0,index);
				name = String("%S.%S",name, ext.sHead("."));
			} else
				name = FileName(name).sFile;

			expression += name;
		} else {
			expression += par.value;
		}
	}
	expression += ")";
	String curDir = getEngine()->getContext()->sCurDir();
	outputName = getValue("responsedocument") + resultType(operation);

	if ( outputName != sUNDEF) {
		expression = "*" + outputName + ":=" + expression;
	}
	getEngine()->Execute(expression);

	FileName fnOut(executionDir + "\\" + outputName);
	getEngine()->Execute(String("zip %S",fnOut.sFullNameQuoted()));
	fnZip = FileName(String("%S_%S.zip",fnOut.sFile, fnOut.sExt.sTail(".")));

}

String WPSExecute::resultType(const String& operation) {
	vector<ApplicationInfo *> infos;
	getEngine()->modules.getAppInfo(operation, infos);
	String ext;
	if ( infos.size() > 0 && infos[0]->metadata){
		ApplicationQueryData query;
		query.queryType = "OUTPUTTYPE";
		query.expression = operation;
		ApplicationMetadata md = (infos[0]->metadata)(&query);
		if ( md.returnType != IlwisObject::iotANY) {
			switch( md.returnType){
				case IlwisObject::iotPOINTMAP :
					ext = ".pnt"; break;
				case IlwisObject::iotPOLYGONMAP :
					ext = ".mpa"; break;
				case IlwisObject::iotSEGMENTMAP :
					ext = ".mps"; break;
				case IlwisObject::iotRASMAP :
					ext = ".mpr"; break;
				case IlwisObject::iotMAPLIST :
					ext = ".mpl"; break;
				case IlwisObject::iotTABLE :
					ext = ".tbl"; break;
				case IlwisObject::iotMATRIX :
					ext = ".mat"; break;
				case IlwisObject::iotOBJECTCOLLECTION :
					ext = ".col"; break;
				case IlwisObject::iotSTEREOPAIR :
					ext = ".stp"; break;
			};
		}
	}
	return ext;
}

void WPSExecute::writeResponse(IlwisServer *server) const{
	XMLPlatformUtils::Initialize();
	DOMImplementation* dom = DOMImplementationRegistry::getDOMImplementation(XMLString::transcode("core"));
	XERCES_CPP_NAMESPACE::DOMDocument *doc = dom->createDocument(0, L"ExecuteResponse", 0);
	createHeader(doc, "WPSExecute_response.xsd");
	XERCES_CPP_NAMESPACE::DOMElement *root = doc->getDocumentElement();

	vector<ApplicationInfo *> infos;
	String operation = getValue("identifier");
	operation.toLower();
	Engine::modules.getAppInfo( operation, infos);
	if ( infos.size() == 0)
		return; // exception must be handled here
	
	ApplicationInfo *info = infos[0];
	if ( info->metadata != NULL) {
		ApplicationQueryData query;
		ApplicationMetadata amd = (info->metadata)(&query);
		if ( amd.wpsxml != "") {
			vector<WPSParameter> outputs;
	
			String status = execute(outputs);

			String xml = amd.wpsxml;
			vector<String> results;
			ILWIS::XMLDocument xmldoc(xml);
			xmldoc.addNameSpace("ows","http://www.opengis.net/ows/1.1");
			xmldoc.addNameSpace("wps","http://www.opengis.net/wps/1.0.0");

			XERCES_CPP_NAMESPACE::DOMElement *node1,*node2,*node3, *node4;
			node1 = doc->createElement(L"wps:Process");
			root->appendChild(node1);
			node2 = createTextNode(doc,"ows:Identifier",getValue("identifier"));
			node1->appendChild(node2);
			xmldoc.executeXPathExpression("//ProcessDescription/ows:Abstract/text()", results);
			if ( results.size() == 1) {
				node2 = createTextNode(doc,"ows:Abstract",results[0]);
				node1->appendChild(node2);
			}
			results.clear();
			xmldoc.executeXPathExpression("//ProcessDescription/ows:Title/text()", results);
			if ( results.size() == 1) {
				node2 = createTextNode(doc,"ows:Title",results[0]);
				node1->appendChild(node2);
			}
			node1 = doc->createElement(L"wps:Status");
			root->appendChild(node1);
			CTime time = CTime::GetCurrentTime();
			wchar_t result[250];
			String times("%d-%d-%dT%d:%d:%d", time.GetYear(), time.GetMonth(), time.GetDay(), time.GetHour(), time.GetMinute(), time.GetSecond());
			node1->setAttribute(L"creationTime",times.toWChar(result));
			node2 = doc->createElement(status.toWChar(result));
			node1->appendChild(node2);
			node1 = doc->createElement(L"wps:DataInputs");
			root->appendChild(node1);
			for(int i = 0; i < inputs.size(); ++i){
				node2 = doc->createElement(L"wps:Input");
				node1->appendChild(node2);
				node3 = createTextNode(doc,"ows:Identifier",inputs[i].id);
				node2->appendChild(node3);
				if ( inputs[i].isReference) {
					node3 = doc->createElement(L"wps:Reference");
					node2->appendChild(node3);
					node3->setAttribute(L"xlink:href",inputs[i].value.toWChar(result));
				} else {
					node3 = doc->createElement(L"wps:Data");
					node2->appendChild(node3);
					node4 = createTextNode(doc, "wps:LiteralData", inputs[i].value);
					node3->appendChild(node4);
				}
			}
			node1 = doc->createElement(L"wps:ProcessDefinitions");
			root->appendChild(node1);
			node1 = doc->createElement(L"wps:ProcessOutputs");
			root->appendChild(node1);
			for(int i=0; i < outputs.size(); ++i) {
				WPSParameter& parm = outputs.at(i);
				node2 = doc->createElement(L"wps:Output");
				node1->appendChild(node2);
				node3 = createTextNode(doc,"ows:Identifier", parm.value);
				node2->appendChild(node3);
				node4 = doc->createElement(L"wps:Reference");
				node3->appendChild(node4);
				String root = getConfigValue("wps:OperationMetaData:ResultServer");
				root += String("/result_data/process_%d/%S%S",folder_count, fnZip.sFile, fnZip.sExt);
				node4->setAttribute(L"xlink:href", root.toWChar(result));
				results.clear();
				String xpathq("//wps:Output/ows:Title[../ows:Identifier=\"%S\"]",parm.value);
				xmldoc.executeXPathExpression(xpathq,results);
				if ( results.size() == 1) {
					node3 = createTextNode(doc, "ows:Title", results[0]);
					node2->appendChild(node3);
				}
				results.clear();
				xpathq = String("//wps:Output/ows:Abstract[../ows:Identifier=\"%S\"]",parm.value);
				xmldoc.executeXPathExpression(xpathq,results);
				if ( results.size() == 1) {
					node3 = createTextNode(doc, "ows:Abstract", results[0]);
					node2->appendChild(node3);
				}
			}
		}
	}

	String txt = createOutput(doc);
	char *buf = new char[txt.size() + 1];
	memset(buf,0,txt.size() + 1);
	memcpy(buf,txt.scVal(), txt.size());
	mg_write(getConnection(), buf, txt.size()+1);
	delete [] buf;
	server->addTimeOutLocation(executionDir,time(0));
}

String WPSExecute::execute(vector<WPSParameter>& outputs) const{
	WPSParameter outParm;
	outParm.id = "ResponseDocument";
	outParm.value = getValue("ResponseDocument");
	outParm.isReference = true;
	outputs.push_back(outParm);

	return "ProcessSucceeded";
}

bool WPSExecute::needsResponse() const{
	return true;
}

	
