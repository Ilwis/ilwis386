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
#include "Engine\Scripting\Script.h"
#include "Engine\Base\File\zlib.h"
#include "Engine\Base\File\unzip.h"
#include "Engine\Base\DataObjects\URL.h"
#include "Engine\Base\DataObjects\Downloader.h"
#include "Engine\DataExchange\CONV.H"



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
			if (inputParm.value.find("href") == string::npos)
				inputParm.value = inputParm.value.sHead("@");
			inputParm.isReference = inputParm.value.find("href") != string::npos;
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
	String rootDir = getConfigValue("wps:ServiceContext:LocalRoot");
	local_folder_count = folder_count;
	executionDir = String("%S\\process_%d", rootDir, folder_count);
	++folder_count;
	_mkdir(executionDir.c_str());
	getEngine()->SetCurDir(executionDir);
}

// URL-decode input buffer into destination buffer.
// 0-terminate the destination buffer. Return the length of decoded data.
// form-url-encoded data differs from URI encoding in a way that it
// uses '+' as character for space, see RFC 1866 section 8.2.1
// http://ftp.ics.uci.edu/pub/ietf/html/rfc1866.txt
static size_t url_decode(const char *src, size_t src_len, char *dst,
						 size_t dst_len, int is_form_url_encoded) {
							 size_t i, j;
							 int a, b;
#define HEXTOI(x) (isdigit(x) ? x - '0' : x - 'W')

							 for (i = j = 0; i < src_len && j < dst_len - 1; i++, j++) {
								 if (src[i] == '%' &&
									 isxdigit(* (const unsigned char *) (src + i + 1)) &&
									 isxdigit(* (const unsigned char *) (src + i + 2))) {
										 a = tolower(* (const unsigned char *) (src + i + 1));
										 b = tolower(* (const unsigned char *) (src + i + 2));
										 dst[j] = (char) ((HEXTOI(a) << 4) | HEXTOI(b));
										 i += 2;
								 } else if (is_form_url_encoded && src[i] == '+') {
									 dst[j] = ' ';
								 } else {
									 dst[j] = src[i];
								 }
							 }

							 dst[j] = '\0'; /* Null-terminate the destination */

							 return j;
}

void WPSExecute::downloadReferencedData() {
	for(int i=0; i < inputs.size(); ++i) {
		if ( inputs[i].isReference) {
			String sUrl = inputs[i].value.sTail("=");
			char urlDecoded [2048];
			url_decode(sUrl.c_str(), (size_t)sUrl.length(), urlDecoded, 2048, 0);
			URL url(urlDecoded);
			FileName fnDest;
			if ( url.getProtocol() == "http") {
				Downloader loader(url);
				loader.download(executionDir);
				int index = url.sVal().find_last_of("/");
				String name="";
				if ( index != string::npos) {
					name =  url.sVal().substr(index+1);
				}
				name = executionDir + "\\" + name;
				fnDest = FileName(name);
				unzip(fnDest);
			} else if ( url.getProtocol() == "file")  {// local
				FileName fnOrg = url.toFileName2();
				String dest = executionDir + "\\" + fnOrg.sFile + fnOrg.sExt;
				CopyFile(fnOrg.sPhysicalPath().c_str(),dest.c_str(),false);
				fnDest = FileName(dest);
				if ( fnDest.sExt == ".zip")
					unzip(fnDest);
			}
			FileName fnTiff(fnDest,".tiff");
			bool fExist = fnTiff.fExist();
			if ( !fExist) {
				fnTiff.sExt = ".tif";
				fExist = fnTiff.fExist();
			}
			if ( fExist ) {
				FileName fnILWIS(fnTiff,".mpr");
				String expr("gdalrasterimport %S %S false", fnTiff.sPhysicalPath(), fnILWIS.sPhysicalPath());
				//getEngine()->Execute(expr);
				Script::Exec(expr);
			} else {
				FileName fnShp(fnDest,".shp");
				if ( fnShp.fExist()) {
					String expr("gdalogrimport shape %S %S", fnShp.sPhysicalPath(), fnShp.sPhysicalPath());	
					Script::Exec(expr);
				}
			}
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

String WPSExecute::makeApplicationExpression(const String& expr, const map<String, ILWIS::WPSParameter>& orderedInput, const String& outputname) {
	String expression = expr + "(";
	int i = 0;
	for(map<String, ILWIS::WPSParameter>::const_iterator cur=orderedInput.begin(); cur != orderedInput.end();++cur,++i) {
		WPSParameter par = (*cur).second;
		if (i > 0)
			expression += ",";
		if ( par.isReference) {
			String url = par.value.sTail("=");
			char urlDecoded [2048];
			url_decode(url.c_str(), (size_t)url.length(), urlDecoded, 2048, 0);
			url = String(urlDecoded);
			int index = url.find_last_of("/");
			String name="";
			if ( index != string::npos) {
				name =  url.substr(index+1);
			}
			name = FileName(name).sFile;


			expression += name;
		} else if ( IOTYPE(par.value) != IlwisObject::iotANY) {
			String sharedDir = getConfigValue("wps:ServiceContext:SharedData");
			expression += sharedDir + "\\" + par.value;
		}
		else {
			String parm = par.value;
			if ( parm.find("geometry(") != string::npos) {
				FileName fnout(outputname);
				parm = "geometry(" + fnout.sFile + "," + parm.substr(9, parm.size());
				expression += parm;
			} else
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
			name = FileName(name).sFile;

			expression += name;
		} else {
			expression += par.value;
		}
	}
	return expression ;
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
	String curDir = getEngine()->getContext()->sCurDir();
	String outputdef = getValue("responsedocument") ;
	outputName = outputdef.sHead("@") + resultType(operation);

	bool isApp = isApplication(expression);
	if ( isApp) {
		expression = makeApplicationExpression(expression, orderedInput,outputName);
	} else {
		expression = makeNonApplicationExpression(expression, orderedInput);
	}


	getEngine()->SetCurDir(executionDir);

	if ( outputName != sUNDEF ) {
		if ( isApp)
			expression = "*" + FileName(outputName).sShortNameQuoted() + ":=" + expression;
		else {
			expression = expression + " " + outputName;
		}
	}
	getEngine()->Execute(expression);
	FileName fnOut(outputName);
	FileName fnTxt(fnOut,".txt");
	if ( !fnTxt.fExist()) {
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
	}

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
	String format = outputdef.sTail("@").toLower();
	if ( format == "format=image/geotiff") {
		FileName fnOut(outputName);
		String expr = String("TIFF(%S, %S)",outputName, fnOut.sFile);
		::Export(expr);
		outputName = fnOut.sFile + ".tif";
	} else if ( outputdef.sTail("@") == "format=application/x-zipped-shp" ) {
		FileName fnOut(outputName);
		String expr = String("Shapefile(%S, %S)",outputName, fnOut.sFile);
		::Export(expr);
		outputName = fnOut.sFile + ".shp";
	}
	fnOut = FileName(executionDir + "\\" + outputName);
	getEngine()->Execute(String("zip %S",fnOut.sFullNameQuoted()));
	fnZip = FileName(String("%S_%S.zip",fnOut.sFile, fnOut.sExt.sTail(".")));

}

void WPSExecute::adaptPathsToLocal(const map<String, FileName>& files) {
	String localPath = getEngine()->sGetCurDir();
	for(map<String, FileName>::const_iterator cur = files.begin(); cur != files.end(); ++cur) {
		FileName fnObj((*cur).second);
		String path = fnObj.sPath();
		if ( IOTYPEBASEMAP(fnObj) ) {
			BaseMap bmp(fnObj);
			if ( bmp->cs()->fnObj.sPath() != localPath && !bmp->cs()->fSystemObject()) {
				FileName fn( bmp->cs()->fnObj);
				CoordSystem cs(FileName(fn.sFile + fn.sExt));
				bmp->SetCoordSystem(cs);
			}
			if ( bmp->dvrs().dm()->fnObj.sPath() != localPath && !bmp->dm()->fSystemObject()) {
				FileName fn(bmp->dvrs().dm()->fnObj);
				Domain dm(FileName(fn.sFile + fn.sExt));
				bmp->SetDomainValueRangeStruct(DomainValueRangeStruct(dm));
			}
			if (IOTYPE(fnObj) == IlwisObject::iotRASMAP) {
				Map mp(fnObj);
				bool fSystem = mp->gr()->fSystemObject();
				bool fLocal = mp->gr()->fnObj.sPath() != localPath ;
				if ( fLocal && !fSystem) {
					FileName fn( mp->gr()->fnObj);
					GeoRef grf(FileName(fn.sFile + fn.sExt));
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
		FileName fnHere(fnObj.sFile + fnObj.sExt);

		if ( IOTYPE(fnObj) != IlwisObject::iotANY) {
			IlwisObject obj = IlwisObject::obj(fnObj);
			if ( obj.fValid()) {
				if ( obj->fSystemObject()) {
					continue;
				}
			}
		}
		if ( fnHere.fExist()) {
			continue;
		}
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

String WPSExecute::processDescribeString(int count, const String& inp) const {
	String tag;
	String txt;
	int parmCount = 0;
	bool start = false;
	bool skip = false;
	for(int i = 0; i < inp.size(); ++i) {
		char c = inp[i];
		if ( c == '<') {
			tag = "";
			start = true;
		}
		if ( c == '>') {
			start = false;
			tag += c;
		}
		if ( start)
			tag += c;
		if ( tag == "<Input minOccurs")
			++parmCount;
		if ( tag == "<LiteralData>") {
			txt += "<Data>\n<LiteralData>";
			for(int k =0; k < inputs.size(); ++k) {
				if ( inputs[k].id == String("%d", parmCount)) {
					txt += inputs[k].value;
					break;
				}
			}
			skip = true;
			 
			tag = "";
		}
		else if ( tag == "</LiteralData>") {
			txt += tag + "\n" + "</Data>\n";
			tag = "";
			skip = false;
		}
		else if ( !start && !skip && tag != "") {
			txt += tag;
			tag = "";
		}
		else if (!start && !skip)
			txt += c;
	
	}
	return txt;
}
void WPSExecute::writeResponse() const{
	vector<CommandInfo *> infos;
	String operation = getValue("identifier");
	operation.toLower();
	Engine::modules.getCommandInfo( operation, infos);
	if ( infos.size() == 0)
		return;

	XMLDocument doc;
	doc.set_name("ExecuteResponse");
	pugi::xml_node erp = doc.addNodeTo(doc,"wps:ExecuteResponse");
	createHeader(doc, "http://www.opengis.net/wps/1.0.0 http://schemas.opengis.net/wps/1.0.0/wpsExecute_response.xsd"); 

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
			xmldoc.addNameSpace("fn","http://www.w3.org/2005/xpath-functions");
			xmldoc.addNameSpace("fo","http://www.w3.org/1999/XSL/Format");
			xmldoc.addNameSpace("ilwis","http://dummy");
			xmldoc.addNameSpace("wps","http://www.opengis.net/wps/1.0.0");
			xmldoc.addNameSpace("ows","http://www.opengis.net/ows/1.1");
			xmldoc.addNameSpace("xlink","http://www.w3.org/1999/xlink");
			xmldoc.addNameSpace("xsi","http://www.w3.org/2001/XMLSchema-instance");

			pugi::xml_node first = doc.first_child();

			//first.append_attribute("service") = "WPS";
			//first.append_attribute("version") = "1.0.0";
			//first.append_attribute("xml:lang") = "en-US";
			//first.append_attribute("xsi:schemaLocation") = "http://www.opengis.net/wps/1.0.0 http://schemas.opengis.net/wps/1.0.0/wpsDescribeProcess_response.xsd";
			//first.append_attribute("wps:processVersion") = "1";
			first.append_attribute("serviceInstance") = "ILWIS";

			pugi::xml_node process = doc.addNodeTo(erp,"wps:Process");
			process.append_attribute("wps:processVersion") = "1.0.0";
			doc.addNodeTo(process, "ows:Identifier", getValue("identifier"));

			xmldoc.executeXPathExpression("//ProcessDescription/ows:Title/text()", results);
			if ( results.size() == 1) {
				doc.addNodeTo(process, "ows:Title", results[0]);
			}

			xmldoc.executeXPathExpression("//ProcessDescription/ows:Abstract/text()", results);
			if ( results.size() == 1) {
				doc.addNodeTo(process, "ows:Abstract", results[0]);
			}

			pugi::xml_node st = doc.addNodeTo(erp,"wps:Status");
			st.append_attribute("creationTime") = ILWIS::Time::now().toString().c_str();
			doc.addNodeTo(st,"wps:ProcessSucceeded");

			//pugi::xml_node inp = doc.addNodeTo(doc.first_child(), "DataInputs");
			/*
			xmldoc.executeXPathExpression("//ProcessDescription/DataInputs", results);
			for(int j = 0; j < results.size(); ++j) {
				String res = processDescribeString(j, results[j]);
				XMLDocument xmldoc(res);
				doc.addNodeTo(doc.first_child(), xmldoc);
			}
			pugi::xml_node outd = doc.addNodeTo(erp, "OutputDefinitions");
			xmldoc.executeXPathExpression("//ProcessDescription/ProcessOutputs/Output", results);
			for(int j = 0; j < results.size(); ++j) {
					XMLDocument xmldoc(results[j]);
					doc.addNodeTo(outd, xmldoc);
			}
			*/
			pugi::xml_node outp = doc.addNodeTo(erp,"wps:ProcessOutputs");
			for(int i=0; i < outputs.size(); ++i) {
				WPSParameter& parm = outputs.at(i);
				pugi::xml_node op = doc.addNodeTo(outp,"wps:Output");

				doc.addNodeTo(op,"ows:Identifier", parm.value.sHead("@"));
				String xpathq ("//Output[ows:Identifier=\"%S\"]/ows:Title/child::text()",parm.value.sHead("@"));
				xmldoc.executeXPathExpression(xpathq,results);
				if ( results.size() == 1) {
					doc.addNodeTo(op,"ows:Title", results[0]);
				}
				xpathq = String("//Output[ows:Identifier=\"%S\"]/ows:Abstract/child::text()",parm.value.sHead("@"));
				xmldoc.executeXPathExpression(xpathq,results);
				if ( results.size() == 1) {
					doc.addNodeTo(op,"ows:Abstract", results[0]);
				}

				if ( parm.isReference) {
					String root = getConfigValue("wps:ServiceContext:ShareServer");
					root += String("/output_data/WPS/process_%d/%S%S",local_folder_count, fnZip.sFile, fnZip.sExt);
					pugi::xml_node ref = doc.addNodeTo(op, "wps:Reference");
					ref.append_attribute("href") = root.c_str();
				}
			}
		}
	}

	String txt = doc.toString();
	char *buf = new char[txt.size() + 1];
	memset(buf,0,txt.size() + 1);
	memcpy(buf,txt.c_str(), txt.size());
	writeHeaders("text/xml", txt.size());
	mg_write(getConnection(), buf, txt.size());

	delete [] buf;
	ilwisServer->addTimeOutLocation(executionDir,time(0));
}

void WPSExecute::createNewOutputList(vector<WPSParameter>& outputs) const{
	outputs.clear();
	ifstream infile("outputfiles.txt");
	while(infile.is_open() && !infile.bad()) {
		char buf[255];
		infile.getline(buf,255);
		//String item(buf);
		URL url(buf);
		WPSParameter outParm;
		outParm.id = url.endSegment();
		outParm.value = url.sVal();
		outParm.isReference = true;
		outputs.push_back(outParm);
	}
	infile.close();
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

	
