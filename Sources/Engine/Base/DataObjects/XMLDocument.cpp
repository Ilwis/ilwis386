#include "headers/toolspch.h"
#include <sstream>
#include "XMLDocument.h"

using namespace ILWIS;

ILWIS::XMLDocument::XMLDocument(const FileName& fn) {
	ifstream istream(fn.sFullPath().scVal());
	load(istream);

}

ILWIS::XMLDocument::XMLDocument(const String& xml) {
	if ( xml != "")
		load(xml.scVal());
}

ILWIS::XMLDocument::~XMLDocument() {
}

void ILWIS::XMLDocument::executeXPathExpression(const String& xpathexpr, vector<String>& results) const {
	results.clear();
	pugi::xpath_node_set tools = select_nodes(xpathexpr.scVal());
	for (pugi::xpath_node_set::const_iterator it = tools.begin(); it != tools.end(); ++it)
	{
		stringstream str;
		pugi::xpath_node node = *it;
		node.node().print(str);
		results.push_back(str.str());
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
	first.append_attribute(name.scVal()) = uri.scVal();
}

pugi::xml_node ILWIS::XMLDocument::addNodeTo(pugi::xml_node& nodeParent, const String& name, const String& value) {
	pugi::xml_node_type type = pugi::node_element;
	if ( value != "")
		type = pugi::node_pcdata;
	pugi::xml_node nodeChild = nodeParent.append_child();
	nodeChild.set_name(name.scVal());
	if ( value != "")
		nodeChild.set_value(value.scVal());

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


