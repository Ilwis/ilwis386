#include "headers/toolspch.h"
#include "Engine\Base\DataObjects\ilwisobj.h"
#include "Engine\Base\System\Engine.h"
#include "HttpServer\IlwisServer.h"
#include "HttpServer\mongoose.h"
#include "httpserver\RequestHandler.h"
#include "Engine\Base\DataObjects\XMLDocument.h"
#include "Engine\Base\System\module.h"
#include "Engine\Applications\ModuleMap.h"
#include "Engine\Base\System\commandhandler.h"
#include "Services\UpdateService.h"
#include "Engine\Base\DataObjects\URL.h"
#include "Engine\Base\DataObjects\Downloader.h"
#include "Engine\Base\File\Zipper.h"
#include <process.h>


using namespace ILWIS;

map<String,unsigned long> UpdateService::updateRequests;

RequestHandler *UpdateService::createHandler(struct mg_connection *c, const struct mg_request_info *request_info, const map<String, String>& kvps, IlwisServer *serv) {
	return new UpdateService(c, request_info,kvps, serv);
}

//----------------------------------------------------
UpdateService::UpdateService(struct mg_connection *c, const struct mg_request_info *ri, const map<String, String>& _kvps, IlwisServer *serv)
: RequestHandler("UpdateServiceHandler", c,ri,_kvps, serv), rtype(rtNONE)
{
	config.add("Services", getId());
}

bool UpdateService::needsResponse() const{
	if ( rtNONE)
		return false;
	return true;
}


void UpdateService::toMap(map<String, IlwisFileInfo>& files, const ILWIS::XMLDocument& doc) {
	vector<pugi::xml_node> nodes;
	doc.executeXPathExpression("//ILWIS_Update_Info/File",nodes);
	IlwisFileInfo info;
	for(int  i =0; i < nodes.size(); ++i) {
		for(pugi::xml_node child = nodes[i].first_child(); child; child = child.next_sibling()) {
			String name = child.name();
			String txt = child.first_child().value();
			if ( name == "Location" )
				info.location = txt;
			if ( name == "Name")
				info.name = txt;
			if ( name == "Size")
				info.size = txt.iVal();
			if ( name == "ModifiedTime" ) {
				info.modifiedTime = ILWIS::Time(txt);
			}
			if ( name == "Check")
				info.check = txt.iVal();
		}
		FileName fn(info.name);
		if ( fn.sExt == ".dll") // << for testing, else everything is included
			files[info.id()] = info;
	}
}

void UpdateService::getDifferences(const map<String, IlwisFileInfo>& baseFiles, const map<String, IlwisFileInfo>& externalFiles, vector<IlwisFileInfo>& differences) {
	String baseDir = getConfigValue("Update:ServiceContext:BaseFileLocation");
	for(map<String, IlwisFileInfo>::const_iterator iter = baseFiles.begin(); iter != baseFiles.end(); ++iter) {
		map<String, IlwisFileInfo>::const_iterator loc = externalFiles.find((*iter).second.id());
		if ( loc == externalFiles.end()) {
			differences.push_back((*iter).second);
		}
		else {
			if (( *iter).second != (*loc).second)
				//differences.push_back(FileName(String("%S\\ilwis3\\%S",baseDir, (*iter).second.id())));
				differences.push_back(((*iter).second));
		}
	}
}

void UpdateService::createUpdateFile() {
	char buffer[1001];
	String result;
	int n = -1;
	while ( (n = mg_read(connection,buffer,1000)) > 0) {
		buffer[n] = 0;
		result += String(buffer);

	}
	XMLDocument doc(result);
	client_id = doc.first_child().attribute("client_id").value();
	String temp = doc.first_child().attribute("check").value();
	check = temp.iVal();
	String resultDir = getConfigValue("Update:ServiceContext:RootDownload");
	String baseDir = getConfigValue("Update:ServiceContext:BaseFileLocation");
	CFileFind finder;

	FileName fnStatus(String("%S\\status.xml",baseDir));
	ILWIS::XMLDocument docStatus;
	ILWIS::XMLDocument docExtern;
	FileName fnConfig(String("\\update_%lu.zip",check));
	if ( !fnConfig.fExist()) {
		if ( !fnStatus.fExist()) {
			vector<IlwisFileInfo> files;
			CommandHandler::gatherFromFolder(baseDir + "\\ilwis3\\", baseDir + "\\ilwis3\\",files);
			pugi::xml_node first = docStatus.addNodeTo(docStatus,"ILWIS_Update_Info");

			for(int i = 0 ; i < files.size(); ++i) {
				pugi::xml_node fl = docStatus.addNodeTo(first,"File");
				docStatus.addNodeTo(fl,"Location",files[i].location);
				docStatus.addNodeTo(fl,"Name",files[i].name);
				docStatus.addNodeTo(fl,"Size",String("%d", files[i].size));
				docStatus.addNodeTo(fl,"ModifiedTime",files[i].modifiedTime.toString());
				docStatus.addNodeTo(fl,"Check",String("%d", files[i].check));
			}
			docStatus.save_file(fnStatus.sFullPath().c_str());
		} else {
			docStatus.load_file(fnStatus.sFullPath().c_str());
		}
		map<String, IlwisFileInfo> baseFiles, externalFiles;
		toMap(baseFiles,docStatus);
		toMap(externalFiles,doc);
		vector<IlwisFileInfo> differences;
		getDifferences(baseFiles, externalFiles, differences);

		long counter = iUNDEF;
		ILWISSingleLock lock(&cs);
		counter = check;
		vector<FileName> tobeZippedFiles;
		for(int i=0 ; i < differences.size(); ++i) {
			tobeZippedFiles.push_back(FileName(String("%S\\ilwis3\\%S",baseDir, differences[i].id())));
		}
		FileName fnOutD(String("%S\\update_description_%lu.xml", resultDir, counter));
		createDescriptionFile(fnOutD,baseFiles,differences);

		tobeZippedFiles.push_back(fnOutD);
		Zipper zipper(tobeZippedFiles, baseDir + "\\ilwis3\\");
		FileName fnOut(String("%S\\update_%lu.zip", resultDir, counter));
		zipper.zip(fnOut);

	}
	ILWISSingleLock lock(&cs);
	UpdateService::updateRequests[client_id] = check;
}

void UpdateService::createDescriptionFile(const FileName& fnOut, const map<String, IlwisFileInfo>& baseFiles, const vector<IlwisFileInfo>& files) {
	XMLDocument doc;
	pugi::xml_node first = doc.addNodeTo(doc,"ILWIS_Update_Descrption");
	doc.addNodeTo(first, "CreationTime", ILWIS::Time::now().toString());
	for(int i = 0 ; i < files.size(); ++i) {
		pugi::xml_node fl = doc.addNodeTo(first,"File");
		doc.addNodeTo(fl,"Location",files[i].location);
		doc.addNodeTo(fl,"Name",files[i].name);
		doc.addNodeTo(fl,"Size",String("%d", files[i].size));
		doc.addNodeTo(fl,"ModifiedTime",files[i].modifiedTime.toString());
		doc.addNodeTo(fl,"Check",String("%d", files[i].check));
	}
	doc.save_file(fnOut.sFullPath().c_str());
}

void UpdateService::writeResponse() const{
	if ( rtype == rtSTATUS) {
		ILWISSingleLock lock(&(const_cast<UpdateService *>(this)->cs));
		map<String,unsigned long>::const_iterator cur = UpdateService::updateRequests.find(client_id);
		if ( cur != UpdateService::updateRequests.end())
			mg_printf(getConnection(),"finished");
		else
			mg_printf(getConnection(),"pending");
	}
	if ( rtype == rtGETUPDATE) {
		map<String,unsigned long>::const_iterator cur = UpdateService::updateRequests.find(client_id);
		if ( cur != UpdateService::updateRequests.end()) {
			String resultDir = getConfigValue("Update:ServiceContext:RootDownload");
			unsigned long configId = (*cur).second;
			FileName fnOut(String("%S\\update_%lu.zip",resultDir, configId));
			CFileStatus fstat;
			CFile::GetStatus(fnOut.sFullPath().c_str(), fstat);
			ULONGLONG sz = fstat.m_size;
			ifstream zippie(fnOut.sFullPath().c_str(), ios_base::in | ios_base::binary);
			const long chunkSize = 1000;
			ULONGLONG szLeft = sz;
			long readSize = min(1000,szLeft);
			char buf[chunkSize];
			int ret = 0;
			if ( zippie.is_open() && readSize > 0) {
				while(zippie.good()) {
					zippie.read(buf,readSize);
					szLeft -= chunkSize;
					ret = mg_write(getConnection(),buf,readSize);
				}
			}
			zippie.close();

		}
	}
}

bool UpdateService::doCommand() {
	String s(request_info->request_method);
	if ( s == "POST") {
		createUpdateFile();
	}
	if ( s == "GET") {
		map<String,String>::const_iterator cur2 = kvps.find("client_id");
		if ( cur2 != kvps.end()) {
			client_id = (*cur2).second;
			map<String,String>::const_iterator cur = kvps.find("query");
			if ( cur != kvps.end()) {
				query = (*cur).second;
				if ( query == "status")
					rtype = rtSTATUS;
				if ( query == "getupdate")
					rtype = rtGETUPDATE;
			}
		}
	}


	return true;

}

