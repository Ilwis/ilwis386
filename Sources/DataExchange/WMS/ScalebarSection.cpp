#include "Headers\toolspch.h"
#Include "DataExchange\mapfile.h"

int ScalebarSection::parse(vector<SimpleSection> elements, int current) {
	int currentElement = current;

	while (currentElement < elements.size()) {
		SimpleSection el = elements[currentElement];
		if (whatFunctionality(el == ElementType::nfEND) {
			break;
		} else if (el.getName() == "LABEL") {
			currentElement = label.parse(elements, ++currentElement);
		} else if (whatFunctionality(el) == ElementType::nfKEYVALUE)
			nameValuePairs[el.getName] = el;

		++currentElement;

	}

	return currentElement;
}

String ScalebarSection::toString() {
	String section = "SCALEBAR\n";
	section += ComplexSection::toString();
	section += label.toString();
	return section + "END\n";
}

