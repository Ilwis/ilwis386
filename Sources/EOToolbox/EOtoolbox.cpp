#include "headers/toolspch.h"
#include "Engine\Base\DataObjects\ilwisobj.h"
#include "Engine\Base\System\Engine.h"
#include "Engine\Base\System\module.h"
#include "Engine\Applications\ModuleMap.h"
#include "Engine\Base\DataObjects\WPSMetaData.h"
#include "Engine\Base\System\commandhandler.h"
#include "Engine/base/system/engine.h"
#include "Engine\Base\DataObjects\ilwisobj.h"
#include "Engine\Base\DataObjects\XMLDocument.h"
#include "EOToolbox\EOtoolbox.h"


EOToolbox *GNCObject = new EOToolbox();

String createTimeString(const String& timep, const FormatInfo& info) {
	if ( info.format == "yyyymmddhhmm") {
		ILWIS::Time time(timep);
		String stime("%d%2d%2d",(int)time.get(ILWIS::Time::tpDATE), (int)time.get(ILWIS::Time::tpHOUR), (int)time.get(ILWIS::Time::tpMINUTE));
		return stime;
	}
	if ( info.format == "yyyymmdd") {
		String prt = timep.substr(0,8);
		return prt;
	}
	return "";

}

String removeBeginEndSlash(const String& str) {
	String tmp = str;
	if ( str[0] == '\\')
		tmp = str.substr(1,tmp.size() - 2);
	if ( tmp[tmp.size() - 1] == '\\') {
		tmp = tmp.substr(0,tmp.size() - 2);
	}
	return tmp;
}

void executeGNCCommand(const String& cmd) {
	Array<String> parts;
	Split(cmd, parts," ");
	String id = parts[0];
	FormatInfo info = GNCObject->get(id);
	String time = createTimeString(parts[1], info);
	FileName fnInput(info.fnInput);
	String inputDrive = fnInput.sDrive;
	String inputPath = removeBeginEndSlash(fnInput.sDir);
	FileName fnOutput(getEngine()->sGetCurDir());
	String outputDrive = fnOutput.sDrive;
	String outputPath = removeBeginEndSlash(fnOutput.sDir);

	String scriptPath = GNCObject->pluginDir() + info.command;
	String ilwdir = getEngine()->getContext()->sIlwDir();
	ilwdir = ilwdir.substr(0, ilwdir.length() - 1);
	FileName fnScript(scriptPath);
	String batCmd = String("run %S %S %S %S %S %S %S %S %S", 
					fnScript.sFullPathQuoted(),
					time,
					inputDrive,
					inputPath,
					outputDrive,
					outputPath,
					GNCObject->gdalLocation(),
					ilwdir,
					GNCObject->utilLocation()
					);
	getEngine()->Execute(batCmd);

}

String wpsmetadataGeonetcast() {
	WPSMetaData metadata("Geonetcast");

	return metadata.toString();
}

ApplicationMetadata metadataGeonetcast(ApplicationQueryData *query) {
	ApplicationMetadata md;
	if ( query->queryType == "WPSMETADATA" || query->queryType == "") {
		md.wpsxml = wpsmetadataGeonetcast();
	}
	if ( query->queryType == "OUTPUTTYPE" || query->queryType == "")
		md.returnType = IlwisObject::iotANY;
	if ( query->queryType == "EXPERSSION" || query->queryType == "")
		md.skeletonExpression = "geonetcast ...";

	return md;
}

extern "C" _export void moduleInitUI(ILWIS::Module *module) {
	getEngine()->getContext()->ComHandler()->AddCommand("geonetcast",executeGNCCommand, metadataGeonetcast);

}

extern "C" _export ILWIS::Module *getModuleInfo(const FileName& fnModule) {
	ILWIS::Module *module = new ILWIS::Module("EO-Toolbox", fnModule,ILWIS::Module::mi38,"1.0");
	module->addMethod(ILWIS::Module::ifInit, (void *)moduleInitUI);
	String ilwDir = getEngine()->getContext()->sIlwDir();
	String formType="EO-Toolbox";
	FileName plugin(ilwDir + "Extensions\\EO-Toolbox\\config.xml");
	GNCObject->ReadConfigFile(plugin);

	return module;
}

//-------------------------------------
EOToolbox::EOToolbox() {

}

void EOToolbox::ReadConfigFile(FileName fnConfig) {
	if ( fnConfig.fExist() == false)
		return;

	ILWIS::XMLDocument doc(fnConfig);

		int count = 0;
		for(pugi::xml_node child = doc.first_child(); child; child = child.next_sibling()) {
			build(child,doc.name(),count); 
		}
		String ilwDir = getEngine()->getContext()->sIlwDir();
		String formType="EO-Toolbox";
		String pluginDir = ilwDir + "Extensions\\EO-Toolbox";
		String tempName = formType;
		FileName fnIni(pluginDir+ "\\geonetcast.ini");
		if ( fnIni.fExist()) {
			for(map<String, FormatInfo>::iterator cur = formats.begin(); cur != formats.end(); ++cur) {
				String folderId = (*cur).second.folderId;
				if ( folderId == "") 
					continue;
				char name[255];
				GetPrivateProfileString(folderId.c_str(), "InputFolder", 0,
                                     name, 255, fnIni.sFullPath().c_str());
				String tmp(name);
				if ( tmp[tmp.size() - 1] != '\\')
					tmp += "\\";
				(*cur).second.fnInput = FileName(tmp);
				//ObjectInfo::ReadElement(folderId.c_str(), "InputFolder",fnIni, name);
				//ObjectInfo::ReadElement(folderId.c_str(), "OutputFolder",fnIni, (*cur).second.fnOutput);
				GetPrivateProfileString(folderId.c_str(), "OutputFolder", 0,
                                     name, 255, fnIni.sFullPath().c_str());
				tmp = String(name);
				if ( tmp[tmp.size() - 1] != '\\')
					tmp += "\\";
				(*cur).second.fnOutput = FileName(tmp);
			}
		}
			
	
}

void EOToolbox::build(pugi::xml_node node,  String current, int& count) {
	pugi::xml_node_type type = node.type();
	if ( type == pugi::node_element) {
		String nodeName = node.name();
		String path  = current;
			FormatInfo info;
			String name(node.attribute("name").value());
			if ( name != "")
				path = current + "|"+ name;
			if ( nodeName == "Product") {
				info.folderId= node.attribute("folderid").value();
				info.command =  node.attribute("script").value();
				info.format = node.attribute("format").value();
				info.type = name;
				String id = makeId(info.command);
				if ( id != "")
					formats[id] = info;

				++count;
			} 

		
		for(pugi::xml_node child = node.first_child(); child; child = child.next_sibling()) {
			build(child,path,count); 
		}
	} 
}

String EOToolbox::makeId(const String& path) {
	Array<String> parts;
	Split(path, parts,"\\");
	if ( parts.size() > 1) {
		String temp =  parts[parts.size() - 2] + ":" + parts[parts.size() - 1];
		int index = temp.find_last_of(".");
		if ( index !=  string::npos)
			temp = temp.substr(0, index);
		return temp.toLower();
	}
	return "";
}

String EOToolbox::gdalLocation() const {
	return pluginDir() + "GDAL\\bin";

}

String EOToolbox::utilLocation() const{
	return  pluginDir() + "util";
}

String EOToolbox::pluginDir() const {
	String ilwDir = getEngine()->getContext()->sIlwDir();
	return ilwDir + "Extensions\\EO-Toolbox\\";
}

FormatInfo EOToolbox::get(const String& id) {
	String tmp = id;
	return formats[tmp.toLower()];
}


