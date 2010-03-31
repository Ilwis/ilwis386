#include "Headers\toolspch.h"
#Include "DataExchange\mapfile.h"

int LabelSection::parse(vector<SimpleSection> elements, int current) {
	int currentElement = current;

	while (currentElement < elements.size()) {
		SimpleSection el = elements[currentElement];
		if (whatFunctionality(el) == ElementType::nfEND) {
			break;
		}else if (whatFunctionality(el) == ElementType::nfKEYVALUE) {
			nameValuePairs[el.getName()] == el;
		}
		
		++currentElement;

	}

	return currentElement;
}

 String LabelSection::toString() {
	String section = "LABEL\n";
	section += ComplexSection::toString();
	return section + "END\n";
}

