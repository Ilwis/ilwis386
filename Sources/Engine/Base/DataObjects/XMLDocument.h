#pragma once 

#include "Engine/Base/XML/pugixml.hpp"

namespace ILWIS {
	class _export XMLDocument : public pugi::xml_document{
	public:
		XMLDocument(const FileName& source);
		XMLDocument(const String& xml="");
		~XMLDocument();
		void executeXPathExpression(const String& xpathexpr, vector<String>& results) const;
		void executeXPathExpression(const String& xpathexpr, vector<pugi::xml_node>& results) const;
		void addNameSpace(const String& name, const String& uri);
		pugi::xml_node addNodeTo(pugi::xml_node&, const String& name, const String& value="");
		void addNodeTo(pugi::xml_node& nodeParent, const pugi::xml_node& child);
		String toString();
	private:
	};
}