#include "Headers\toolspch.h"
#include "Engine\Base\DataObjects\ilwisobj.h"
#include "Engine\Base\System\module.h"
#include "Engine\Base\System\engine.h"
#include "XMLDocument.h"
#include "WPSMetaData.h"


WPSMetaData::WPSMetaData(const String& appName) : id(appName){
}

WPSMetaData::~WPSMetaData() {
}

String WPSMetaData::toString() {
	String basePath = getEngine()->getContext()->sIlwDir();
	FileName fnXML(String("%SServices\\wps_process_descriptions.xml",basePath));
	ILWIS::XMLDocument doc(fnXML);
	String xpathq("//ProcessDescription[Identifier=\"%S\"]",id);
	vector<String> results;
	doc.executeXPathExpression(xpathq,results);
	if ( results.size() > 0) {
		return results[0];
	}
	return "";
}

//-----------------------------------------
void ProcessMetaData::addDefinitionFile(const FileName& fn) {
}

WPSMetaData * ProcessMetaData::operator[](const String& name){
	map<String,WPSMetaData *>::iterator cur=find(name);
	if ( cur != end())
		return (*cur).second;
	return 0;
}