#include "headers/toolspch.h"
#include <sstream>
#include "XMLDocument.h"

using namespace ILWIS;

ILWIS::XMLDocument::XMLDocument(const FileName& fn) {
	ifstream istream(fn.sFullPath().c_str());
	load(istream);

}

ILWIS::XMLDocument::XMLDocument(const String& xml) {
	if ( xml != "")
		load(xml.c_str());
}

ILWIS::XMLDocument::~XMLDocument() {
}

void ILWIS::XMLDocument::executeXPathExpression(const String& xpathexpr, vector<String>& results) const {
	results.clear();
	pugi::xpath_node_set tools = select_nodes(xpathexpr.c_str());
	for (pugi::xpath_node_set::const_iterator it = tools.begin(); it != tools.end(); ++it)
	{
		stringstream str;
		pugi::xpath_node node = *it;
		node.node().print(str);
		String s(str.str());
		s = s.sTrimSpaces(true);
		results.push_back(s);
	}
}

void ILWIS::XMLDocument::executeXPathExpression(const String& xpathexpr, vector<pugi::xml_node>& results) const {
	results.clear();
	pugi::xpath_node_set tools = select_nodes(xpathexpr.c_str());
	for (pugi::xpath_node_set::const_iterator it = tools.begin(); it != tools.end(); ++it)
	{
		pugi::xpath_node node = *it;
		results.push_back(node.node());
	}
}


String ILWIS::XMLDocument::toString() {
	stringstream str;
	save(str);
	String endResult(str.str());

	return endResult;

}

void ILWIS::XMLDocument::addNameSpace(const String& name, const String& uri) {
	pugi::xml_node first = first_child();
	if ( first == 0)
		return;
	String nsId("xlmns:%S",name);
	first.append_attribute(nsId.c_str()) = uri.c_str();
}

pugi::xml_node ILWIS::XMLDocument::addNodeTo(pugi::xml_node& nodeParent, const String& name, const String& value) {
	pugi::xml_node_type type = pugi::node_element;
	pugi::xml_node nodeChild = nodeParent.append_child();
	nodeChild.set_name(name.c_str());
	if ( value != "") {
		pugi::xml_node nodeGrandChild = nodeChild.append_child(pugi::node_pcdata);
		nodeGrandChild.set_value(value.c_str());
	}

	return nodeChild;
}

void ILWIS::XMLDocument::addNodeTo(pugi::xml_node& nodeParent, const pugi::xml_node& node) {
	if ( !node) {
		return;
	}
	//pugi::xml_node_type ty = node.type();
	//if ( ty == pugi::node_document)
	//	addNodeTo(nodeParent, node.first_child());
	//else {
	bool emptyNode = strlen(node.value()) == 0 && strlen(node.name()) == 0;
	pugi::xml_node parent = nodeParent;
	if ( !emptyNode ) {
		pugi::xml_node newChild = nodeParent.append_child(node.type());
		newChild.set_name(node.name());
		newChild.set_value(node.value());
		for (pugi::xml_attribute attr = node.first_attribute(); attr; attr = attr.next_attribute()) {
			newChild.append_attribute(attr.name()) = attr.value();
		}
		parent = newChild;
	}
	for (pugi::xml_node child = node.first_child(); child; child = child.next_sibling()) {
		addNodeTo(parent,child);
	}


}


