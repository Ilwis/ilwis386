#include "headers/toolspch.h"
#include "Engine\Base\DataObjects\ilwisobj.h"
#include "Engine\Base\System\Engine.h"
#include "Engine\Map\basemap.h"
#include "Engine\Base\DataObjects\ObjectStructure.h"
#include "HttpServer\IlwisServer.h"
#include "HttpServer\mongoose.h"
#include "httpserver\RequestHandler.h"
#include "httpserver\OWSHandler.h"
#include "Services\WPSHandler.h"
#include "Services\WPSExecute.h"
#include "Engine\Map\Raster\Map.h"
#include "Engine\Base\System\module.h"
#include "Engine\Applications\ModuleMap.h"
#include "Engine\Base\DataObjects\XMLDocument.h"
#include "Engine\Base\File\zlib.h"
#include "Engine\Base\File\unzip.h"
#include "Engine\Base\DataObjects\URL.h"
#include "Engine\Base\DataObjects\Downloader.h"



using namespace ILWIS;

RequestHandler *WPSExecute::createHandler(struct mg_connection *c, const struct mg_request_info *request_info, const map<String, String>& kvps, IlwisServer *serv) {
	return new WPSExecute(c, request_info,kvps, serv);
}


int WPSExecute::folder_count=0;

WPSExecute::WPSExecute(struct mg_connection *c, const struct mg_request_info *ri, const map<String, String>& _kvps, IlwisServer *serv)
: WPSHandler("WPSExecuteHandler", c,ri,_kvps, serv)
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

bool WPSExecute::doCommand() {
	try{
		createExecutionEnviroment();
		downloadReferencedData();
		executeOperation();

		return true;
	} catch(const ErrorObject& err) {
		String errTxt = err.sWhat();
		writeError(errTxt);
	}
	return false;

}


void WPSExecute::createExecutionEnviroment() {
	String rootDir = getConfigValue("WPS:ServiceContext:LocalRoot");
	executionDir = String("%S\\process_%d", rootDir, folder_count);
	_mkdir(executionDir.c_str());
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

bool WPSExecute::isApplication(const String& sExpres) {
	if (fCIStrEqual(sExpres.sLeft(14) , "PolygonMapList")) 
			return true;
	else if (fCIStrEqual(sExpres.sLeft(7) , "MapList")) 
		return true;
	else if (fCIStrEqual(sExpres.sLeft(3) , "Map"))
		return true;
	else if (fCIStrEqual(sExpres.sLeft(5) , "Table")) 
	{
		return true;
	}
	else if (fCIStrEqual(sExpres.sLeft(10) , "SegmentMap"))
		return true;
	else if (fCIStrEqual(sExpres.sLeft(10) , "PolygonMap"))
		return true;
	else if (fCIStrEqual(sExpres.sLeft(8) , "PointMap"))
		return true;
	else if (fCIStrEqual(sExpres.sLeft(6) , "Matrix"))
		return true;
	else if (fCIStrEqual(sExpres.sLeft(10) , "StereoPair"))
		return true;
	return false;

}

String WPSExecute::makeApplicationExpression(const String& expr, const map<String, ILWIS::WPSParameter>& orderedInput) {
	String expression = expr + "(";
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
	/*		name = executionDir + "\\" + name;
			index = name.find_last_of("_");
			if ( index != string::npos) {
				String ext = name.substr(index+1);
				name = name.substr(0,index);
				name = String("%S.%S",name, ext.sHead("."));
			} else*/
				name = FileName(name).sFile;

			expression += name;
		} else {
			expression += par.value;
		}
	}
	expression += ")";
	return expression;
}

String WPSExecute::makeNonApplicationExpression(const String& expr, const map<String, ILWIS::WPSParameter>& orderedInput) {
	String expression = expr + " ";
	int i = 0;
	for(map<String, ILWIS::WPSParameter>::const_iterator cur=orderedInput.begin(); cur != orderedInput.end();++cur,++i) {
		WPSParameter par = (*cur).second;
		if (i > 0)
			expression += " ";
		if ( par.isReference) {
			String url = par.value.sTail("=");
			int index = url.find_last_of("/");
			String name="";
			if ( index != string::npos) {
				name =  url.substr(index+1);
			}
			//name = executionDir + "\\" + name;
			//index = name.find_last_of("_");
			//if ( index != string::npos) {
			//	String ext = name.substr(index+1);
			//	name = name.substr(0,index);
			//	name = String("%S.%S",name, ext.sHead("."));
			//} else
				name = FileName(name).sFile;

			expression += name;
		} else {
			expression += par.value;
		}
	}
	return expression + ";";
}

void WPSExecute::executeOperation() {

	String expression = getValue("identifier");
	String operation = expression;
	operation.toLower();
	vector<CommandInfo *> infos;
	getEngine()->modules.getCommandInfo(operation,infos);
	if ( infos.size() == 0)
		throw ErrorObject(String("Uknown operation %S",operation));

	map<String, ILWIS::WPSParameter> orderedInput;
	for(int i=0; i < inputs.size(); ++i) {
		orderedInput[inputs[i].id] = inputs[i];

	}
	bool isApp = isApplication(expression);
	if ( isApp) {
		expression = makeApplicationExpression(expression, orderedInput);
	} else {
		expression = makeNonApplicationExpression(expression, orderedInput);
	}


	String curDir = getEngine()->getContext()->sCurDir();
	outputName = getValue("responsedocument") + resultType(operation);
	getEngine()->SetCurDir(executionDir);

	if ( outputName != sUNDEF ) {
		if ( isApp)
			expression = "*" + outputName + ":=" + expression;
		else {
			expression = expression + " " + outputName;
		}
	}
	getEngine()->Execute(expression);
	CFileFind finder;
	String path(getEngine()->sGetCurDir() + "*.*");
	BOOL fFound = finder.FindFile(path.c_str());
	FileName fnOut(outputName);
	FileName fnTxt(fnOut,".txt");
	ofstream of(fnTxt.sFullPath().c_str());
	while(fFound) {
		fFound = finder.FindNextFile();
		if (!finder.IsDirectory())
		{
			FileName fnNew (finder.GetFilePath());
			String line("%S\n",fnNew.sFile + fnNew.sExt);
			of << line.c_str();

		}
	}
#undef close
	of.close();


	fnOut = FileName(executionDir + "\\" + outputName);
	getEngine()->Execute(String("zip %S",fnOut.sFullNameQuoted()));
	fnZip = FileName(String("%S_%S.zip",fnOut.sFile, fnOut.sExt.sTail(".")));
	ifstream ifs(fnTxt.sFullPath().c_str());
	String line;
	if (ifs.is_open())
	{
		map<String, FileName> files;
		gatherFiles(ifs, files);
		moveToLocal(files);
		adaptPathsToLocal(files);
	
		ifs.close();
	}
}

void WPSExecute::adaptPathsToLocal(const map<String, FileName>& files) {
	String localPath = getEngine()->sGetCurDir();
	for(map<String, FileName>::const_iterator cur = files.begin(); cur != files.end(); ++cur) {
		FileName fnObj((*cur).second);
		String path = fnObj.sPath();
		if ( IOTYPEBASEMAP(fnObj) ) {
			BaseMap bmp(fnObj);
			if ( bmp->cs()->fnObj.sPath() != localPath) {
				FileName fn( bmp->cs()->fnObj);
				CoordSystem cs(FileName(fn.sFile + fn.sExt));
				if ( ! cs->fSystemObject())
					bmp->SetCoordSystem(cs);
			}
			if ( bmp->dvrs().dm()->fnObj.sPath() != localPath) {
				FileName fn(bmp->dvrs().dm()->fnObj);
				Domain dm(FileName(fn.sFile + fn.sExt));
				if ( !dm->fSystemObject())
					bmp->SetDomainValueRangeStruct(DomainValueRangeStruct(dm));
			}
			if (IOTYPE(fnObj) == IlwisObject::iotRASMAP) {
				Map mp(fnObj);
				if ( mp->gr()->fnObj.sPath() != localPath) {
					FileName fn( mp->gr()->fnObj);
					GeoRef grf(FileName(fn.sFile + fn.sExt));
					if ( !grf->fSystemObject())
						mp->SetGeoRef(grf);
				}	
			}
			bmp->Store();
		}
	}
}

void WPSExecute::moveToLocal(const map<String, FileName>& files) {
	for(map<String, FileName>::const_iterator cur = files.begin(); cur != files.end(); ++cur) {
		String entry = (*cur).first;
		FileName fnObj((*cur).second);

		if ( IOTYPE(fnObj) != IlwisObject::iotANY) {
			IlwisObject obj = IlwisObject::obj(fnObj);
			if ( obj.fValid()) {
				if ( obj->fSystemObject()) 
					continue;
			}
		}
		FileName fnHere(fnObj.sFile + fnObj.sExt);
		if ( fnHere.fExist())
			continue;
		CopyFile(fnObj.sFullPath().c_str(), fnHere.sFullPath().c_str(),true);
	}

}
void WPSExecute::gatherFiles(ifstream& ifs, map<String, FileName>& files) {
	String line;
	while ( ifs.good() )
	{
		getline (ifs,line);
		FileName fn(line);
		if ( IOTYPEBASEMAP(fn.sFullPath())) {
			BaseMap bmp(fn);
			ObjectStructure ostruct;
			bmp->GetObjectStructure(ostruct);
			list<String> usedFiles;
			ostruct.GetUsedFiles(usedFiles, false);
			for(list<String>::iterator cur = usedFiles.begin(); cur != usedFiles.end(); ++cur) {
				FileName fnObj(*cur);
				files[fnObj.sFile + fnObj.sExt] = fnObj;
			}
				
		}
	}
}

String WPSExecute::resultType(const String& operation) {
	vector<CommandInfo *> infos;
	getEngine()->modules.getCommandInfo(operation, infos);
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
	vector<CommandInfo *> infos;
	String operation = getValue("identifier");
	operation.toLower();
	Engine::modules.getCommandInfo( operation, infos);
	if ( infos.size() == 0)
		return;

	XMLDocument doc;
	doc.set_name("wps:ExecuteResponse");
	pugi::xml_node erp = doc.addNodeTo(doc,"wps:ExecuteResponse");
	createHeader(doc, "WPSExecute_response.xsd");

	CommandInfo *info = infos[0];
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

			pugi::xml_node proces = doc.addNodeTo(erp,"wps:Process");
			doc.addNodeTo(proces, "ows:Identifier", getValue("identifier"));

			xmldoc.executeXPathExpression("//wps:ProcessDescription/ows:Abstract/text()", results);
			if ( results.size() == 1) {
				doc.addNodeTo(proces, "ows:Abstract", results[0]);
			}

			xmldoc.executeXPathExpression("//wps:ProcessDescription/ows:Title/text()", results);
			if ( results.size() == 1) {
				doc.addNodeTo(proces, "ows:Title", results[0]);
			}
			
			pugi::xml_node st = doc.addNodeTo(erp,"wps:Status");
			st.append_attribute("creationTime") = ILWIS::Time::now().toString().c_str();
			doc.addNodeTo(st,"wps:ProcessSucceeded");

			//pugi::xml_node inp = doc.addNodeTo(doc.first_child(), "wps:DataInputs");
			xmldoc.executeXPathExpression("//wps:ProcessDescription/wps:DataInputs", results);
			for(int j = 0; j < results.size(); ++j) {
				XMLDocument xmldoc(results[j]);
				doc.addNodeTo(doc.first_child(), xmldoc);
			}
			pugi::xml_node outd = doc.addNodeTo(erp, "wps:OutputDefinitions");
			xmldoc.executeXPathExpression("//wps:ProcessDescription/wps:ProcessOutputs/wps:Output", results);
			for(int j = 0; j < results.size(); ++j) {
					XMLDocument xmldoc(results[j]);
					doc.addNodeTo(outd, xmldoc);
			}
			pugi::xml_node outp = doc.addNodeTo(erp,"wps:ProcessOutputs");
			for(int i=0; i < outputs.size(); ++i) {
				WPSParameter& parm = outputs.at(i);
				pugi::xml_node op = doc.addNodeTo(outp,"wps:Output");
				if ( parm.isReference) {
					String root = getConfigValue("WPS:ServiceContext:ShareServer");
					root += String("/wps:shared_data/process_%d/%S%S",folder_count, fnZip.sFile, fnZip.sExt);
					pugi::xml_node ref = doc.addNodeTo(op, "wps:Reference");
					ref.append_attribute("xlink:href") = root.c_str();
				}
				String xpathq("//wps:Output/ows:Title[../ows:Identifier=\"%S\"]",parm.value);
				xmldoc.executeXPathExpression(xpathq,results);
				if ( results.size() == 1) {
					doc.addNodeTo(outp,"ows:Title", results[0]);
				}
				xpathq = String("//wps:Output/ows:Abstract[../ows:Identifier=\"%S\"]",parm.value);
				xmldoc.executeXPathExpression(xpathq,results);
				if ( results.size() == 1) {
					doc.addNodeTo(outp,"ows:Abstract", results[0]);
				}
			}

		}
	}

	String txt = doc.toString();
	char *buf = new char[txt.size() + 1];
	memset(buf,0,txt.size() + 1);
	memcpy(buf,txt.c_str(), txt.size());
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

	
