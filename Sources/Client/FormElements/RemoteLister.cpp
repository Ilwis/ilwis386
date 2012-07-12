#include "Client\Headers\formelementspch.h"
#include "Client\FormElements\nameedit.h"
#include "Engine\Base\DataObjects\URL.h"
#include "Engine\Base\DataObjects\XMLDocument.h"
#include "Engine\Base\DataObjects\RemoteXMLObject.h"
#include "Client\ilwis.h"
#include "Client\FormElements\RemoteLister.h"

RemoteLister::~RemoteLister(){
}

void RemoteLister::getFiles(vector<String>& files){
	RemoteObject xmlObj(remoteURL.sVal());
	String txt = xmlObj.toString();
	if ( txt != "") {
		if ( remoteURL.getQueryValue("service") == "gnc") {
			handleGNCCase(txt, files);
		}

	}
	if ( txt != "") {
		if ( remoteURL.getQueryValue("service") == "ilwis") {
			handleIlwisCase(txt, files);
		}

	}
}

void RemoteLister::handleGNCCase(const String& txt, vector<String>& files) {
	ILWIS::XMLDocument doc(txt);
	vector<String> values;
	doc.executeXPathExpression("//GNC_Catalog/File/ImageTime/text()",values);
	sort(values.begin(), values.end());
    values.erase(std::unique(values.begin(), values.end()), values.end());
	for(int i=0; i < values.size(); ++i) {
		files.push_back(values[i]);
	}
}

void RemoteLister::handleIlwisCase(const String& txt,vector<String>& files) {
	ILWIS::XMLDocument doc(txt);
	vector<String> values;
	doc.executeXPathExpression("//Ilwis_Catalog/File/Filename/text()",values);
	sort(values.begin(), values.end());
    values.erase(std::unique(values.begin(), values.end()), values.end());
	for(int i=0; i < values.size(); ++i) {
		files.push_back(values[i]);
	}
}