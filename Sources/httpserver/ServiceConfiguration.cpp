#include "headers/toolspch.h"
#include "Engine/base/system/engine.h"
#include "HttpServer/ServiceConfiguration.h"

using namespace ILWIS;

ServiceConfiguration::ServiceConfiguration(const String& dir, const String& name){
	String ilwDir = getEngine()->getContext()->sIlwDir();
	String generalConfig("%SServices.ini",ilwDir);
	readConfigFile(generalConfig);

	add(dir,name);
	isChanged = false;
}

void ServiceConfiguration::add(const FileName& fn) {
	if ( fn.fExist())
		readConfigFile(fn);
}

FileName ServiceConfiguration::add(const String& dir, const String& name) {
	if ( name.size() > 0) {
	//String txt("%S", name);
		String ilwDir = getEngine()->getContext()->sIlwDir();
		String key("Handler:Configurations:%S",name);
		FileName configFn(String("%S%S", ilwDir, get(key)));

		if ( configFn.fExist())
			readConfigFile(configFn);
		else {
			configFn = FileName(String("%S%S\\%S.ini", ilwDir, dir, name));
			readConfigFile(configFn);
		}
		return configFn;
	} else {
		String ilwDir = getEngine()->getContext()->sIlwDir();		
		String rest = get(String("%S:configuration",dir));
		if ( rest != sUNDEF) {
			add(FileName(ilwDir + rest));
		}
	}
	return FileName();
}

void ServiceConfiguration::readConfigFile(FileName fnC) {
	if ( fnC.fExist() == false)
		return;
	fnConfig = fnC;
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
					if ( value.find("[") != -1) {
						String otherconfig = value.sHead("[");
						ServiceConfiguration cfgOther("Services", otherconfig);
						String otherKey = value.sTail("[").sHead("]");
						value = cfgOther.get(otherKey);
					}
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

void ServiceConfiguration::flush(const FileName& fnC){
	if ( isChanged == false)
		return;

	if ( !fnC.fValid() && !fnConfig.fValid())
		return;
	FileName fnSave = fnC.fValid() ? fnC : fnConfig;

	ofstream configfile(fnSave.sFullPath().c_str());
	String line;
	if ( configfile.is_open()) {
		String prevSection;
		for(map<String, String>::const_iterator iter = config.begin(); iter !=  config.end(); ++iter) {
			String key = (*iter).first;
			String value = (*iter).second;
			int index = key.find_last_of(":");
			String section = key.substr(0, index);
			String entry = key.substr(index + 1, key.size() - index);
			if ( prevSection != section) {
				if ( prevSection != "")
					configfile << "\n";
				configfile << String("[%S]\n", section);
			}
			configfile << String("%S=%S\n",key, value);
		}
		configfile.close();
	}
	isChanged = false;
}

void ServiceConfiguration::set(const String& key, const String& value) {
	String tempKey = key;
	tempKey.toLower();
	map<String, String>::iterator iter;
	if ( (iter = config.find(tempKey)) != config.end()) {
		(*iter).second = value;	
	} else {
		config[key] = value;
	}
	isChanged = true;
	flush(); // testing
}