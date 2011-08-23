#include "headers/toolspch.h"
#include "Engine/base/system/engine.h"
#include "HttpServer/ServiceConfiguration.h"

using namespace ILWIS;

ServiceConfiguration::ServiceConfiguration(const String& dir, const String& name){
	String ilwDir = getEngine()->getContext()->sIlwDir();
	String generalConfig("%SServices.ini",ilwDir);
	readConfigFile(generalConfig);
	add(dir,name);
}

void ServiceConfiguration::add(const String& dir, const String& name) {
	if ( name != "") {
		String ilwDir = getEngine()->getContext()->sIlwDir();
		FileName altConfig(String("%S%S", ilwDir, get("Handler:Configurations:"+name)));
		if ( altConfig.fExist())
			readConfigFile(altConfig);
		else
			readConfigFile(String("%S%S\\%S.ini", ilwDir, dir, name));
	}
}

void ServiceConfiguration::readConfigFile(FileName fnConfig) {
	if ( fnConfig.fExist() == false)
		return;
	ifstream configfile(fnConfig.sFullPath().c_str());
	String line;
	if ( configfile.is_open()) {
		String prefix;
		while(configfile.good()) {
			string l;
			getline(configfile,l);
			line = l;
			line = line.sTrimSpaces();
			if (line[0] == '[') {
				prefix = line.sSub(1, line.size() - 2);
			} else {
				if ( line != "") {
					String key = line.sHead("=");
					key = key.sTrimSpaces();
					String value = line.sTail("=");
					value = value.sTrimSpaces();
					key = prefix + ":" + key;
					key.toLower();
					config[key] = value;
				}
			}

		}
	}
}

String ServiceConfiguration::get(const String& key) const {
	String tempKey = key;
	tempKey.toLower();
	map<String, String>::const_iterator iter;
	if ( (iter = config.find(tempKey)) != config.end()) {
		return (*iter).second;
	}
	return sUNDEF;
}