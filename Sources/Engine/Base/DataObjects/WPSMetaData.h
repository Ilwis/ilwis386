#pragma once;

#include <xercesc/dom/dom.hpp>

class _export WPSParameter {
public:
	WPSParameter(const String& _identifier, const String& _type, bool _input=true);
	void AddTitle(const String& desc); 
	void AddAbstract(const String& desc); 
	void AddDefault(const String& _def);
	XERCES_CPP_NAMESPACE::DOMElement *createNode(XERCES_CPP_NAMESPACE::DOMDocument *doc);
	bool isInput() const;
private:
	String id;
	String title;
	String abstrct;
	String def;
	String type;
	bool input;


};
class _export WPSMetaData {
public:
	WPSMetaData(const String& appName);
	void AddTitle(const String& desc); 
	void AddAbstract(const String& desc);
	void AddParameter(const WPSParameter& parm);
	void AddKeyword(const String& kw);
	String toString();
private:
	String id;
	String title;
	String abstrct;
	String skeletonExpression;
	vector<WPSParameter> inputParameters;
	vector<WPSParameter> outputParameters;
	vector<String> keywords;

};