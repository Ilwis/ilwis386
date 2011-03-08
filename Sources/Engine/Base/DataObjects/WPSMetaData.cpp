#include "Headers\toolspch.h"
#include "Engine\Base\DataObjects\WPSMetaData.h"
#include <xercesc/dom/domdocument.hpp>


XERCES_CPP_NAMESPACE_USE

XERCES_CPP_NAMESPACE::DOMElement *createTextNode(XERCES_CPP_NAMESPACE::DOMDocument *doc,const String& nodeName, const String& value) {
	wchar_t result[250];
	XERCES_CPP_NAMESPACE::DOMElement *ele = doc->createElement(nodeName.toWChar(result));
	DOMText* text = doc->createTextNode(value.toWChar(result));
	ele->appendChild(text);
	return ele;

}

WPSParameter::WPSParameter(const String& name, const String& _type, bool _input) : id(name), input(_input),type(_type){
}

void WPSParameter::AddTitle(const String& desc) {
	title = desc;
}

bool WPSParameter::isInput() const{
	return input;
}

void WPSParameter::AddAbstract(const String& desc) {
	abstrct = desc;
}

void WPSParameter::AddDefault(const String& _def) {
	def = _def;
}

void WPSMetaData::AddAbstract(const String& desc) {
	abstrct = desc;
}

void WPSMetaData::AddKeyword(const String& kw) {
	keywords.push_back(kw);
}

XERCES_CPP_NAMESPACE::DOMElement *WPSParameter::createNode(XERCES_CPP_NAMESPACE::DOMDocument *doc) {
	wchar_t result[250];
	XERCES_CPP_NAMESPACE::DOMElement *parm = doc->createElement(input ? L"wps:Input" : L"wps:Output");
	parm->setAttribute(L"minOccurs",L"1");
	parm->setAttribute(L"maxOccurs",L"1");
	parm->appendChild(createTextNode(doc, "ows:Identifier", id));
	parm->appendChild(createTextNode(doc, "ows:Title", title));
	if ( abstrct != "")
		parm->appendChild(createTextNode(doc, "ows:Abstract", abstrct));

	if ( type != "zip") {
		XERCES_CPP_NAMESPACE::DOMElement *literalParm = doc->createElement(L"LiteralData");
		XERCES_CPP_NAMESPACE::DOMElement *dt = createTextNode(doc,"ows:reference",type); 
		String datatype("http://www.w3.org/TR/xmlschema-2/%S", type);
		dt->setAttribute(L"ows:reference", datatype.toWChar(result));
		if ( def != "") {
			literalParm->appendChild(createTextNode(doc,"DefaultValue",def));
		}
		literalParm->appendChild(dt);
		parm->appendChild(literalParm);
	} else {
		XERCES_CPP_NAMESPACE::DOMElement *cd = doc->createElement(L"ComplexData");
		parm->appendChild(cd);
		XERCES_CPP_NAMESPACE::DOMElement *def = doc->createElement(L"Default");
		cd->appendChild(def);
		XERCES_CPP_NAMESPACE::DOMElement *frmt = doc->createElement(L"Format");
		def->appendChild(frmt);
		XERCES_CPP_NAMESPACE::DOMElement *mimeType = createTextNode(doc,"MimeType","application/zip");
		frmt->appendChild(mimeType);
		XERCES_CPP_NAMESPACE::DOMElement *encod = createTextNode(doc,"Encoding","Base64");
		frmt->appendChild(encod);
	}

	return parm;
}

//------------------------------------------------------------------------------------
WPSMetaData::WPSMetaData(const String& appName) : id(appName){
}

void WPSMetaData::AddParameter(const WPSParameter& parm) {
	if ( parm.isInput() ) {
		inputParameters.push_back(parm);
	} else {
		outputParameters.push_back(parm);
	}

}

void WPSMetaData::AddTitle(const String& desc) {
	title = desc;
}

String WPSMetaData::toString() {
	wchar_t result[250];
	XMLPlatformUtils::Initialize();
	DOMImplementation* dom = DOMImplementationRegistry::getDOMImplementation(XMLString::transcode("core"));
	XERCES_CPP_NAMESPACE::DOMDocument *doc = dom->createDocument(0, L"ProcessDescriptions", 0);


	XERCES_CPP_NAMESPACE::DOMElement *root = doc->getDocumentElement();
	root->setAttribute(L"xmlns:wps",L"http://www.opengis.net/wps/1.0.0");
	root->setAttribute(L"xmlns:ows",L"http://www.opengis.net/ows/1.1");
	root->setAttribute(L"xmlns:xlink",L"http://www.w3.org/1999/xlink");
	root->setAttribute(L"xmlns:xsi",L"http://www.w3.org/2001/XMLSchema-instance");
	root->setAttribute(L"xsi:schemaLocation",L"http://www.opengis.net/wps/1.0.0 ../wpsDescribeProcess_response.xsd");

	root->setAttribute(L"service",L"WPS");
	root->setAttribute(L"version",L"1.0.0");
	root->setAttribute(L"xml:lang",L"en-CA");
	XERCES_CPP_NAMESPACE::DOMElement *proces = doc->createElement(L"ProcessDescription");
	proces->setAttribute(L"wps:processVersion",L"1");
	proces->setAttribute(L"wps:storeSupported",L"true");
	proces->setAttribute(L"wps:statusSupported",L"false");
	proces->appendChild(createTextNode(doc, "ows:Identifier", id));
	proces->appendChild(createTextNode(doc, "ows:Title", title));
	if ( abstrct != "")
		proces->appendChild(createTextNode(doc, "ows:Abstract", abstrct));
	for(int i = 0; i<keywords.size(); ++i){
		XERCES_CPP_NAMESPACE::DOMElement *metadata1 = doc->createElement(L"ows:Metadata");
		metadata1->setAttribute(L"xlink:title", keywords[i].toWChar(result));
		proces->appendChild(metadata1);
	}
	XERCES_CPP_NAMESPACE::DOMElement *inputs = doc->createElement(L"wps:DataInputs");
	for(int i =0; i < inputParameters.size(); ++i) {
		XERCES_CPP_NAMESPACE::DOMElement *parameter = inputParameters[i].createNode(doc);
		inputs->appendChild(parameter);
	}
	proces->appendChild(inputs);
	XERCES_CPP_NAMESPACE::DOMElement *outputs = doc->createElement(L"wps:ProcessOutputs");
	for(int i =0; i < outputParameters.size(); ++i) {
		XERCES_CPP_NAMESPACE::DOMElement *parameter = outputParameters[i].createNode(doc);
		outputs->appendChild(parameter);
	}
	proces->appendChild(outputs);
	root->appendChild(proces);

	DOMImplementation *domOut = DOMImplementationRegistry::getDOMImplementation(L"LS");
	DOMLSSerializer*  writer = ((DOMImplementationLS*)domOut)->createLSSerializer(); 
	DOMConfiguration* dc = writer->getDomConfig(); 
	//dc->setParameter(XMLUni::fgDOMErrorHandler,errorHandler); 
	dc->setParameter(XMLUni::fgDOMWRTDiscardDefaultContent,true); 
	dc->setParameter(XMLUni::fgDOMWRTFormatPrettyPrint, true);
	CString txt(writer->writeToString(doc));

	doc->release();
	XMLPlatformUtils::Terminate();
	String endResult;
	String temp(txt);
	for(int i=0; i < temp.size(); ++i) {
		if ( temp[i] == '\n') {
			endResult += '\r';
		}
		endResult += temp[i];
	}

	return endResult;
}