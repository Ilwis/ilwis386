#pragma once;

#include <xercesc/dom/dom.hpp>
#include "Engine\Applications\ModuleMap.h"

#define TEXT_SIZE 1000

class _export WPSParameter {
public:
	enum ParameterType{
		pmtRASMAP, pmtPOLYGONMAP, pmtSEGMENTMAP, pmtPOINTMAP, pmtTABLE, 
		pmtMAPLIST, pmtOBJECTCOLLECTION, pmtMAPVIEW, pmtLAYOUT, pmtANNOTATIONTEXT, pmtGRAPH, 
		pmtDOMAIN, pmtREPRESENTATION, pmtGEOREF, pmtCOORDSYSTEM, 
		pmtHISTRAS, pmtHISTPOL, pmtHISTSEG, pmtHISTPNT, 
		pmtSAMPLESET, pmtSTEREOPAIR, pmtCRITERIATREE, 
		pmtTABLE2DIM, pmtMATRIX, pmtFILTER, pmtFUNCTION, pmtSCRIPT, pmtCOLUMN, pmtZIP, pmtRANGE,pmtGROUP,pmtDATETIME,pmtNONE, // after this simple types or things that can be translated to simple
		pmtENUM, pmtINTEGER, pmtREAL,pmtSTRING,pmtBOOL ,
		pmtANY};

	WPSParameter(const String& _identifier, const String& _title, ParameterType pt, bool _input=true);
	void AddTitle(const String& desc); 
	void AddAbstract(const String& desc); 
	void AddDefault(const String& _def);
	void setOptional(bool yesno);
	bool isOptional() const;
	void setRange(const RangeInt& r);
	RangeInt getRange() const;
	virtual XERCES_CPP_NAMESPACE::DOMElement *createNode(XERCES_CPP_NAMESPACE::DOMDocument *doc);
	bool isInput() const;
	String paremeterTypeToWWW3String() const;
protected:
	String id;
	String title;
	String abstrct;
	String def;
	bool input;
	bool optional;
	ParameterType pmt;
	RangeInt range;


};


class _export WPSParameterGroup : public WPSParameter{
public:
	WPSParameterGroup();
	WPSParameterGroup(const String& id, int startOrdinal, const String& _title);
	~WPSParameterGroup();
	void addParameter(WPSParameter* p);
	int getNumberOfParameters() const;
	WPSParameter *getParameters(int index);
	virtual XERCES_CPP_NAMESPACE::DOMElement *createNode(XERCES_CPP_NAMESPACE::DOMDocument *doc);
private:
	vector<WPSParameter *> parameters;
	int startOrdinal;
	bool exclusiveList;

};

class _export WPSMetaData {
public:
	WPSMetaData(const String& appName);
	~WPSMetaData();
	void AddTitle(const String& desc); 
	void AddAbstract(const String& desc);
	void AddParameter(WPSParameter *parm);
	void AddKeyword(const String& kw);
	String toString();
private:
	String id;
	String title;
	String abstrct;
	String skeletonExpression;
	vector<WPSParameter *> inputParameters;
	vector<WPSParameter *> outputParameters;
	vector<String> keywords;

};