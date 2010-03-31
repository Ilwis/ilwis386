#include "Headers\toolspch.h"
#Include "DataExchange\mapfile.h"

int WebSection::parse(ArrayList<SimpleSection> elements, int current) {

	int currentElement = current;

	while (currentElement < elements.size()) {
		SimpleSection el = elements[currentElement];
		ComplexSection sectio;
		if (whatFunctionality(el) == ElementType::nfEND) {
			break;
		}else if (whatFunctionality(el) == ElementType::nfKEYVALUE)
			nameValuePairs[el.getName()]  = el;
		if (el.getName() == "METADATA") {
			section = new  MetaDataSection();
		}
		
		if (section != null )
		{
			currentElement = section.parse(elements, ++currentElement);
			sections.push_back(section);
		}
		++currentElement;

	}

	return currentElement;
}

 String WebSection::toString() {
	String section = "WEB\n";
	section += ComplexSection::toString();
	return section + "END\n";
}

