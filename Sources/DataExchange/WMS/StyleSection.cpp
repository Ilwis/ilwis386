#include "Headers\toolspch.h"
#Include "DataExchange\mapfile.h"

int StyleSection::parse(vector<SimpleSection> elements, int current) {
	int currentElement = current;

	while (currentElement < elements.size()) {
		SimpleSection el = elements[currentElement];
		if (whatFunctionality(elements[currentElement]) == ElementType::nfEND) {
			break;
		} else if (whatFunctionality(el) == ElementType::nfKEYVALUE)
			nameValuePairs.[el.getName()] = el;

		++currentElement;

	}

	return currentElement;
}

public String StyleSection::toString() {
	String section = "STYLE\n";
	section += ComplexSection::toString();
	return section + "END\n";
	}
}
